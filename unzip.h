//
// Created by phaz on 3/4/2020.
//
#include <windows.h>

#ifndef GAME_LOOP_OPENGL_4_3_POC_UNZIP_H
#define GAME_LOOP_OPENGL_4_3_POC_UNZIP_H

#define MAX_FILES_PER_ARCHIVE 1024

struct UnzippedFile {
    char name[256];
    size_t dataLength;
    char * data;
};
struct UnzippedArchive {
    int filesLength;
    struct UnzippedFile * files[MAX_FILES_PER_ARCHIVE];
};
class Unzip {
private:
    static VOID decompressFile(int zippedDataSize, const char * zippedData, const char * unzippedData) {
    }
public:
    static VOID unzipArchive(const char * zippedArchiveData, size_t zippedArchiveDataLength, UnzippedArchive * unzippedArchive) {
        unzippedArchive->filesLength = 0;
        int i = 0;
        while (i < zippedArchiveDataLength) {
//            while (zippedArchiveData[i] == 0x50 && zippedArchiveData[i + 1] == 0x4B) {
            auto unzippedFile = (struct UnzippedFile *)malloc(sizeof(struct UnzippedFile));
            int fileNameLength = zippedArchiveData[i + 26] + (zippedArchiveData[i + 27] << 8);
            int headerSize = 26 + fileNameLength + zippedArchiveData[i + 28] + (zippedArchiveData[i + 29] << 8);
            int compressedSize = zippedArchiveData[i + 18] + (zippedArchiveData[i + 19] << 8) +
                    (zippedArchiveData[i + 20] << 16) + (zippedArchiveData[i + 21] << 24);
            int j;
            for (j = 0; j < fileNameLength; j++) {
                unzippedFile->name[j] = zippedArchiveData[i + 30 + j];
            }
            unzippedFile->name[j] = 0;
            unzippedArchive->files[unzippedArchive->filesLength++] = unzippedFile;
//            i += headerSize + compressedSize + ((zippedArchiveData[i + 6] & 3) ? 12 : 0);
            i++;
            while (i < zippedArchiveDataLength && !(
                    zippedArchiveData[i] == 0x50 && zippedArchiveData[i + 1] == 0x4B &&
                            zippedArchiveData[i + 2] == 0x03 && zippedArchiveData[i + 3] == 0x04
            )) {
                i++;
            }
////            for (j = 0; (i + j) < zippedArchiveDataLength && !(zippedArchiveData[i + j] == 0x50 && zippedArchiveData[i + j + 51] == 0x4B); j++) {}
//            char nextA = zippedArchiveData[i];
//            char nextB = zippedArchiveData[i + 1];
            volatile int x = 0;
        }
    }
};


#endif //GAME_LOOP_OPENGL_4_3_POC_UNZIP_H
