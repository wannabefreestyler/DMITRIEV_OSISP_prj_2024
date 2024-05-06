#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define FAT12_BS 512
#define FAT12_SS 1
#define FAT12_RSC 32
#define FAT12_FATS 2
#define FAT12_DPB 16
#define FAT12_RDE 32
#define FAT12_HDS 2
#define FAT12_SPC 9
#define FAT12_SPT 18
#define FAT12_SEC_SIZE 512

typedef struct
{
    uint8_t jmp[3];
    char oem[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t fat_copies;
    uint16_t root_entries;
    uint16_t total_sectors_16;
    uint8_t media_type;
    uint16_t sectors_per_fat;
    uint16_t sectors_per_track;
    uint16_t heads;
    uint32_t hidden_sectors;
    uint32_t total_sectors_32;
} __attribute__((packed)) fat12_bpb_t;

void format_fat12(const char *device)
{
    int fd = open(device, O_RDWR);
    if (fd < 0)
    {
        perror("Error opening device");
        exit(1);
    }

    fat12_bpb_t bpb;
    memset(&bpb, 0, sizeof(fat12_bpb_t));

    strncpy(bpb.oem, "MYFAT   ", 8);
    bpb.bytes_per_sector = FAT12_BS;
    bpb.sectors_per_cluster = FAT12_SS;
    bpb.reserved_sectors = FAT12_RSC;
    bpb.fat_copies = FAT12_FATS;
    bpb.root_entries = FAT12_DPB * FAT12_RDE;
    bpb.total_sectors_16 = FAT12_SPC * FAT12_HDS * FAT12_SPT;
    bpb.media_type = 0xF0;
    bpb.sectors_per_fat = (FAT12_RSC + (FAT12_DPB * FAT12_RDE) / FAT12_SS) * 2;
    bpb.sectors_per_track = FAT12_SPT;
    bpb.heads = FAT12_HDS;

    lseek(fd, 0, SEEK_SET);
    write(fd, &bpb, sizeof(fat12_bpb_t));

    uint8_t fat[FAT12_RSC * FAT12_FATS * FAT12_SS];
    memset(fat, 0, sizeof(fat));
    fat[0] = 0x0FF;
    fat[1] = 0xF0;

    lseek(fd, FAT12_RSC * FAT12_BS, SEEK_SET);
    write(fd, fat, sizeof(fat));

    printf("Formatting completed successfully.\n");
    close(fd);
}

void check_fat12(const char *device)
{
    int fd = open(device, O_RDONLY);
    if (fd < 0)
    {
        perror("Error opening device");
        exit(1);
    }

    fat12_bpb_t bpb;
    lseek(fd, 0, SEEK_SET);
    read(fd, &bpb, sizeof(fat12_bpb_t));

    if (bpb.bytes_per_sector != FAT12_BS ||
        bpb.sectors_per_cluster != FAT12_SS ||
        bpb.reserved_sectors != FAT12_RSC ||
        bpb.fat_copies != FAT12_FATS ||
        bpb.root_entries != FAT12_DPB * FAT12_RDE ||
        bpb.total_sectors_16 != FAT12_SPC * FAT12_HDS * FAT12_SPT ||
        bpb.media_type != 0xF0 ||
        bpb.sectors_per_fat != (FAT12_RSC + (FAT12_DPB * FAT12_RDE) / FAT12_SS) * 2 ||
        bpb.sectors_per_track != FAT12_SPT ||
        bpb.heads != FAT12_HDS)
    {
        printf("Error: Device is not a FAT12 file system\n");
        close(fd);
        exit(1);
    }

    printf("Device is a FAT12 file system\n");
    close(fd);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <device>\n", argv[0]);
        return 1;
    }

    check_fat12(argv[1]);
    format_fat12(argv[1]);
    return 0;
}
