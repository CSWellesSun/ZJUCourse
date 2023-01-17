#ifndef _GRAY_H_
#define _GRAY_H_
#include <stdio.h>
#pragma pack(1)
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef unsigned long       DWORD;
typedef unsigned long       LONG;

typedef struct tagBITMAPFILEHEADER {
        WORD    bfType;
        DWORD   bfSize;
        WORD    bfReserved1;
        WORD    bfReserved2;
        DWORD   bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER{
        DWORD      biSize;
        LONG       biWidth;
        LONG       biHeight;
        WORD       biPlanes;
        WORD       biBitCount;
        DWORD      biCompression;
        DWORD      biSizeImage;
        LONG       biXPelsPerMeter;
        LONG       biYPelsPerMeter;
        DWORD      biClrUsed;
        DWORD      biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagRGBQUAD {
        BYTE    rgbBlue;
        BYTE    rgbGreen;
        BYTE    rgbRed;
        BYTE    rgbReserved;
} RGBQUAD;

typedef struct
{
    BYTE R;
    BYTE G;
    BYTE B;    
}Pixel;

BYTE *Color2Gray(FILE *finput, FILE *foutput);
void WriteGrayBMP(BYTE *Gray, FILE *foutput);
BYTE FindThreshold(BYTE *Gray);
void Binarization(BYTE *Gray);
BYTE *Dilation(BYTE *Gray, BYTE *structureElement, int sHeight, int sWidth);
BYTE *Erosion(BYTE *Gray, BYTE *structureElement, int sHeight, int sWidth);
BYTE *Opening(BYTE *Gray, BYTE *structureElement, int sHeight, int sWidth);
BYTE *Closing(BYTE *Gray, BYTE *structureElement, int sHeight, int sWidth);
#endif