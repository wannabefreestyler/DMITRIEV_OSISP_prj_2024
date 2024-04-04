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
    /*
------------------------------------------------------------------------------
# Открываем диск или устройство для записи
disk = open("disk.img", "wb")

# Инициализируем заголовок FAT
header = struct.pack("<HBBHBBHBBH", ...)
disk.write(header)

# Создаем корневой каталог
root_directory = b"\x00" * 512  # Пример: создаем пустой корневой каталог размером 512 байт
disk.write(root_directory)

# Создаем и записываем FAT таблицу
fat_table = b"\x00" * 4096  # Пример: создаем пустую FAT таблицу размером 4096 байт
disk.write(fat_table)

# Закрываем файл диска
disk.close()
--------------------------------------------------------------------------- псевдокод для форматирования
*/
/*
---------------------------------------------------------------------------
def read_root_directory(disk, root_directory_offset):
    disk.seek(root_directory_offset)
    root_directory = disk.read(512)  # Читаем корневой каталог размером 512 байт
    entries = struct.unpack("<32s" * 16, root_directory)  # Предполагается, что в корневом каталоге 16 записей по 32 байта каждая
    for entry in entries:
        # Проверяем запись о файле
        if entry[0] != b"\x00" and entry[0] != b"\xE5":  # Проверяем, что запись не является пустой или удаленной
            # Извлекаем информацию о файле из записи
            filename = entry[0:8].decode("utf-8").strip()
            extension = entry[8:11].decode("utf-8").strip()
            attributes = entry[11]
            # Другие поля в записи о файле...

            # Проверяем целостность и корректность записи о файле
            # Например, проверяем, что размер файла и атрибуты соответствуют ожидаемым значениям
            if attributes & 0x10 == 0x10:  # Проверяем, является ли запись подкаталогом
                # Если запись является подкаталогом, рекурсивно проверяем его содержимое
                subdirectory_offset = ...  # Получаем смещение подкаталога из записи о файле
                read_root_directory(disk, subdirectory_offset)
            else:
                # Если запись является файлом, проверяем его целостность и другие атрибуты

def check_fat_filesystem():
    disk = open("disk.img", "rb")

    # Прочитайте заголовок FAT для получения информации о расположении корневого каталога и FAT таблицы
    header = disk.read(512)  # Предполагается, что заголовок FAT имеет размер 512 байт
    # Извлекаем информацию из заголовка, например, смещения корневого каталога и FAT таблицы

    # Проверяем корневой каталог
    read_root_directory(disk, root_directory_offset)

    # Проверяем FAT таблицу и другие аспекты файловой системы FAT

    disk.close()

# Запускаем проверку файловой системы FAT
check_fat_filesystem()
---------------------------------------------------------------------------псевдокод для проверки файлов
*/
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
