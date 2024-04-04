#include <stdio.h>
#include <stdint.h>

// Сигнатура FAT12 и FAT16
#define FAT_SIGNATURE 0xAA55

// Структура заголовка FAT
typedef struct {
    uint16_t signature;         // Сигнатура FAT
    uint16_t bytesPerSector;    // Размер сектора в байтах
    uint8_t sectorsPerCluster;  // Количество секторов на кластер
    uint16_t reservedSectors;   // Зарезервированные секторы перед FAT
    uint8_t fatCopies;          // Количество копий FAT
    uint16_t rootEntries;       // Количество записей в корневом каталоге
    uint16_t totalSectors;      // Общее количество секторов в файловой системе
    uint8_t mediaType;          // Тип носителя
    uint16_t sectorsPerFat;     // Количество секторов на FAT
    uint16_t sectorsPerTrack;   // Количество секторов на дорожке
    uint16_t heads;             // Количество головок диска
    uint32_t hiddenSectors;     // Количество скрытых секторов
    uint32_t largeTotalSectors; // Общее количество секторов (для FAT32)
} FatHeader;

// Функция для чтения информации о файловой системе FAT
void readFatHeader(FILE* diskFile) {
    FatHeader header;

    // Перемещаем указатель в начало диска (зависит от вашей реализации)
    fseek(diskFile, 0, SEEK_SET);

    // Читаем заголовок FAT
    fread(&header, sizeof(FatHeader), 1, diskFile);

    // Проверяем сигнатуру FAT
    if (header.signature != FAT_SIGNATURE) {
        printf("Неверная сигнатура FAT.\n");
        return;
    }

    // Выводим информацию о файловой системе
    printf("Размер сектора: %u байт.\n", header.bytesPerSector);
    printf("Количество секторов на кластер: %u.\n", header.sectorsPerCluster);
    printf("Зарезервированные секторы: %u.\n", header.reservedSectors);
    printf("Количество копий FAT: %u.\n", header.fatCopies);
    printf("Количество записей в корневом каталоге: %u.\n", header.rootEntries);
    printf("Общее количество секторов: %u.\n", header.totalSectors);
    printf("Тип носителя: %u.\n", header.mediaType);
    printf("Количество секторов на FAT: %u.\n", header.sectorsPerFat);
    printf("Количество секторов на дорожке: %u.\n", header.sectorsPerTrack);
    printf("Количество головок диска: %u.\n", header.heads);
    printf("Количество скрытых секторов: %u.\n", header.hiddenSectors);
    printf("Общее количество секторов (для FAT32): %u.\n", header.largeTotalSectors);
}

int main() {
    // Открываем файл диска (замените "disk.img" на путь к вашему образу диска)
    FILE* diskFile = fopen("disk.img", "rb");
    if (diskFile == NULL) {
        printf("Не удалось открыть файл диска.\n");
        return 1;
    }

    // Читаем информацию о файловой системе FAT
    readFatHeader(diskFile);

    // Закрываем файл диска
    fclose(diskFile);

    return 0;
}

