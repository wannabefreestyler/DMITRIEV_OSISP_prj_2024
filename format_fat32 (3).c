#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#define SECTOR_SIZE 512
#define FAT32_EOC 0x0FFFFFF8

// Структура BPB (BIOS Parameter Block)
struct __attribute__((__packed__)) bpb_t {
    uint8_t  jmpBoot[3];
    char     OEMName[8];
    uint16_t BytsPerSec;
    uint8_t  SecPerClus;
    uint16_t RsvdSecCnt;
    uint8_t  NumFATs;
    uint16_t RootEntCnt;
    uint16_t TotSec16;
    uint8_t  Media;
    uint16_t FATSz16;
    uint16_t SecPerTrk;
    uint16_t NumHeads;
    uint32_t HiddSec;
    uint32_t TotSec32;
    uint32_t FATSz32;
    uint16_t ExtFlags;
    uint16_t FSVer;
    uint32_t RootClus;
    uint16_t FSInfo;
    uint16_t BkBootSec;
    uint8_t  Reserved[12];
    uint8_t  DrvNum;
    uint8_t  Reserved1;
    uint8_t  BootSig;
    uint32_t VolID;
    char     VolLab[11];
    char     FilSysType[8];
    uint8_t  BootCode[420];
    uint16_t BootSign;
};

// Функция для записи данных на устройство
void write_sector(int fd, uint32_t sector, const void *buffer) {
    if (lseek(fd, sector * SECTOR_SIZE, SEEK_SET) == -1) {
        perror("lseek");
        exit(EXIT_FAILURE);
    }
    if (write(fd, buffer, SECTOR_SIZE) != SECTOR_SIZE) {
        perror("write");
        exit(EXIT_FAILURE);
    }
}

// Функция для очистки устройства
void clear_device(const char *device, uint32_t size) {
    int fd = open(device, O_RDWR);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    uint8_t zero_sector[SECTOR_SIZE] = {0};

    for (uint32_t i = 0; i < size; ++i) {
        write_sector(fd, i, zero_sector);
        if ((i + 1) % 10000 == 0) {     // Проверка, чтобы выводить сообщение каждые 10000 секторов
            printf("Очищен сектор %u\n", i + 1);
        }
    }

    close(fd);
}

// Функция для проверки файловой системы FAT32
int check_fat32(const char *device, uint32_t total_sectors) {
    int fd = open(device, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return -1;
    }

    struct bpb_t bpb;
    if (read(fd, &bpb, sizeof(bpb)) != sizeof(bpb)) {
        perror("read");
        return -1;
    }

    // Проверяем, соответствует ли BPB стандарту FAT32
    if (memcmp(bpb.jmpBoot, "\xEB\x58\x90", 3) != 0 ||
        bpb.BytsPerSec != SECTOR_SIZE ||
        bpb.SecPerClus != 8 ||
        bpb.RsvdSecCnt != 32 ||
        bpb.NumFATs != 2 ||
        bpb.Media != 0xF8 ||
        bpb.TotSec32 == 0|
        bpb.FATSz32 == 0 ||
        bpb.RootClus != 2 ||
        bpb.FSInfo != 1 ||
        bpb.BkBootSec != 6 ||
        bpb.DrvNum != 0x80 ||
        bpb.BootSig != 0x29 ) {
        fprintf(stderr, "Error: Invalid FAT32 BPB\n");
        return -1;
    }

    close(fd);
    return 0;
}

// Функция для форматирования устройства
void format_device(const char *device, uint32_t total_sectors) {
    int fd = open(device, O_RDWR);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    struct bpb_t bpb;
    memset(&bpb, 0, sizeof(bpb));

    // Устанавливаем параметры BPB
    memcpy(bpb.jmpBoot, "\xEB\x58\x90", 3);
    memcpy(bpb.OEMName, "MSWIN4.1", 8);
    bpb.BytsPerSec = SECTOR_SIZE;
    bpb.SecPerClus = 8;
    bpb.RsvdSecCnt = 32;
    bpb.NumFATs = 2;
    bpb.Media = 0xF8;
    bpb.TotSec32 = total_sectors;  // Количество секторов
    bpb.FATSz32 = (total_sectors / bpb.SecPerClus) / 128;  // Примерный размер FAT
    bpb.RootClus = 2;
    bpb.FSInfo = 1;
    bpb.BkBootSec = 6;
    bpb.DrvNum = 0x80;
    bpb.BootSig = 0x29;
    bpb.VolID = 0x12345678;
    memcpy(bpb.VolLab, "NO NAME    ", 11);
    memcpy(bpb.FilSysType, "FAT32   ", 8);
    bpb.BootSign = 0xAA55;

    // Записываем BPB в первый сектор
    write_sector(fd, 0, &bpb);

    // Создаем FSInfo
    uint8_t fsinfo[SECTOR_SIZE] = {0};
    *(uint32_t*)&fsinfo[0] = 0x41615252;
    *(uint32_t*)&fsinfo[484] = 0x61417272;
    *(uint32_t*)&fsinfo[488] = 0xFFFFFFFF;
    *(uint32_t*)&fsinfo[492] = 0xFFFFFFFF;
    *(uint16_t*)&fsinfo[510] = 0xAA55;
    write_sector(fd, 1, fsinfo);

    // Создаем FAT
    uint8_t fat[SECTOR_SIZE] = {0};
    *(uint32_t*)&fat[0] = 0x0FFFFFF8;  // Медиа-метка
    *(uint32_t*)&fat[4] = FAT32_EOC;  // Конец корневого каталога
    *(uint32_t*)&fat[8] = FAT32_EOC;  // Конец корневого каталога

    // Записываем FAT
    for (int i = 0; i < bpb.FATSz32; ++i) {
        write_sector(fd, bpb.RsvdSecCnt + i, fat);
        write_sector(fd, bpb.RsvdSecCnt + bpb.FATSz32 + i, fat);
    }

    close(fd);
    printf("Устройство %s успешно отформатировано в FAT32.\n", device);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <device> <size_in_sectors>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *device = argv[1];
    uint32_t size_in_sectors = atoi(argv[2]);

    // Проверка файловой системы
    if (check_fat32(device, size_in_sectors) == -1) {
        return EXIT_FAILURE;
    }

    // Очистка устройства
    clear_device(device, size_in_sectors);

    // Форматирование устройства
    format_device(device, size_in_sectors);

    return EXIT_SUCCESS;
}
