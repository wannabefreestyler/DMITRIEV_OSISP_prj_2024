#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

// Константы для FAT32
#define FAT32_BOOT_SECTOR_SIZE 512
#define FAT32_MAX_FILENAME 255
#define FAT32_CLUSTER_SIZE 4
#define FAT32_RESERVED_SECTOR_COUNT 32
#define FAT32_FAT_COUNT 2
#define FAT32_ROOT_DIR_ENTRY_COUNT 0
#define FAT32_SECTOR_PER_CLUSTER 1

// Структура загрузочного сектора FAT32
typedef struct {
    uint8_t jmp_boot[3];
    uint8_t oem_name[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sector_count;
    uint8_t fat_count;
    uint16_t root_dir_entry_count;
    uint16_t total_sectors_16;
    uint8_t media_descriptor;
    uint16_t sectors_per_fat_16;
    uint16_t sectors_per_track;
    uint16_t heads_count;
    uint32_t hidden_sectors;
    uint32_t total_sectors_32;
    uint8_t drive_number;
    uint8_t reserved;
    uint8_t extended_boot_signature;
    uint32_t volume_serial_number;
    uint8_t volume_label[11];
    uint8_t file_system_type[8];
    uint8_t boot_code[448];
    uint16_t boot_sector_signature;
} __attribute__((packed)) fat32_boot_sector_t;

// Структура записи каталога FAT32
typedef struct {
    uint8_t filename[11];
    uint8_t attributes;
    uint8_t reserved;
    uint8_t creation_time_tenth;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_access_date;
    uint16_t first_cluster_high;
    uint16_t modification_time;
    uint16_t modification_date;
    uint16_t first_cluster_low;
    uint32_t file_size;
} __attribute__((packed)) fat32_dir_entry_t;

int main(int argc, char *argv[]) {
    // Открываем файл, соответствующий устройству хранения данных
    FILE *fp = fopen("/home/wannabefs/osisp", "r");
    if (!fp) {
        perror("Failed to open device file");
        return 1;
    }

    // Читаем загрузочный сектор
    fat32_boot_sector_t boot_sector;
    if (fread(&boot_sector, FAT32_BOOT_SECTOR_SIZE, 1, fp) != 1) {
        perror("Failed to read boot sector");
        return 1;
        // Распечатываем информацию о загрузочном секторе
printf("Bytes per sector: %u\n", boot_sector.bytes_per_sector);
printf("Sectors per cluster: %u\n", boot_sector.sectors_per_cluster);
printf("Reserved sector count: %u\n", boot_sector.reserved_sector_count);
printf("FAT count: %u\n", boot_sector.fat_count);
printf("Root directory entry count: %u\n", boot_sector.root_dir_entry_count);
printf("Total sectors (16-bit): %u\n", boot_sector.total_sectors_16);
printf("Media descriptor: %u\n", boot_sector.media_descriptor);
printf("Sectors per FAT (16-bit): %u\n", boot_sector.sectors_per_fat_16);
printf("Sectors per track: %u\n", boot_sector.sectors_per_track);
printf("Heads count: %u\n", boot_sector.heads_count);
printf("Hidden sectors: %u\n", boot_sector.hidden_sectors);
printf("Total sectors (32-bit): %u\n", boot_sector.total_sectors_32);
printf("Volume serial number: %u\n", boot_sector.volume_serial_number);
printf("Volume label: %s\n", boot_sector.volume_label);
printf("File system type: %s\n", boot_sector.file_system_type);

    }

    // Закрываем файл
    fclose(fp);

    return 0;
}

