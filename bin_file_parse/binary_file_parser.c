#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


typedef struct __attribute__((packed)) BMPFileHeader{
    char signature[2];  // "BM"
    uint32_t fileSize;
    uint32_t reserved;
    uint32_t pixelOffset;
}BMPFileHeader;


typedef struct __attribute__((packed)) DIBHeader{
    uint32_t headerSize;
    uint32_t imageWidth;
    uint32_t imageHeight;
    uint16_t planes;           // 2 bytes (not 4)
    uint16_t bitsPerPixel;     // 2 bytes (not 4)
    uint32_t compression;
    uint32_t imageSize;
}DIBHeader;

typedef struct Pixel{
    char * pixel;
}Pixel;

FILE * readBMPFile(const char* filepath){
    FILE *bmp = fopen(filepath, "rb");
    if (bmp == NULL) {
        perror("Error opening file");
    }
    return bmp;
}

BMPFileHeader * readBMPHeader(FILE * bmp){
    BMPFileHeader *bmpHeader = malloc(sizeof(BMPFileHeader));
    fread(bmpHeader, sizeof(BMPFileHeader), 1, bmp); // Read the header
    if (bmpHeader == NULL){
        free(bmpHeader);
        return NULL;
    }
    if (bmpHeader->signature[0] != 'B' || bmpHeader->signature[1] != 'M'){
        free(bmpHeader);
        return NULL;
    }
    return bmpHeader;
}

DIBHeader * readDIBHeader(FILE * bmp){
    DIBHeader *dibHeader = malloc(sizeof(DIBHeader));
    fseek(bmp, 14, SEEK_SET);
    fread(dibHeader, sizeof(DIBHeader), 1, bmp); // Read the header
    if (dibHeader == NULL){
        free(dibHeader);
        return NULL;
    }
    return dibHeader;
}

uint8_t * readPixelData(FILE * bmp, uint32_t offset){
     
}

void main(){
    FILE *bmp = readBMPFile("bmp_24.bmp");
    BMPFileHeader *bmpHeader = readBMPHeader(bmp);
    printf("Signature: %c%c\n", bmpHeader->signature[0], bmpHeader->signature[1]);
    printf("File Size: %u bytes\n", bmpHeader->fileSize);
    printf("Data Offset: %u\n", bmpHeader->pixelOffset);
    DIBHeader *dibHeader = readDIBHeader(bmp);
    printf("Header Size: %u\n", dibHeader->headerSize);
    printf("Image H: %u bytes\n", dibHeader->imageHeight);
    printf("Image W: %u bytes\n", dibHeader->imageWidth);
    printf("Image Size: %u\n", dibHeader->imageSize);
    printf("Bits per Pixel: %u\n", dibHeader->bitsPerPixel);
    uint8_t *pixelData = readPixelData(bmp, bmpHeader->pixelOffset);

    uint8_t *pixel_rows; 

    int rowSize = dibHeader->imageWidth * (dibHeader->bitsPerPixel / 3);

    for(int row = 0; row < dibHeader->imageHeight; row++){
        int rowStart = row * rowSize;
        for(int col = 0; col < dibHeader->imageWidth; col++){
            int pixelStart = rowStart + col * 3;

            unsigned char blue = pixelData[pixelStart];
            unsigned char green = pixelData[pixelStart + 1];
            unsigned char red = pixelData[pixelStart + 2];

            // Print pixel values
            printf("Pixel at Row %d, Col %d: B=%02x G=%02x R=%02x\n", row, col, blue, green, red);

        }
    }
    
}