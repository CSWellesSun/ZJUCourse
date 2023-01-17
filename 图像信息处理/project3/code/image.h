#ifndef _IMAGE_H_
#define _IMAGE_H_
#include <stdio.h>
#pragma pack(1)

typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef unsigned int        DWORD;
typedef unsigned int        LONG;

typedef struct tagBITMAPFILEHEADER {
        WORD    bfType;
        DWORD   bfSize;
        WORD    bfReserved1;
        WORD    bfReserved2;
        DWORD   bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
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

typedef struct {
        BYTE R;
        BYTE G;
        BYTE B;    
} Pixel;

typedef struct {
        BITMAPFILEHEADER *bmpFileHeader;
        BITMAPINFOHEADER *bmpInfo;      
        RGBQUAD *bmpRgbQuad;
        BYTE *bmpData;
} Image;

/* ------------------------------------
* File Relevant Function 
*/
Image *ReadBMP(FILE *finput);
void WriteBMP(Image *bmpImage, FILE *foutput);
Image *Copy(Image *origin);
void Free(Image *bmpImage);

/* ------------------------------------
* YUV Relevant Function
*/ 
Image *RGB2YUV(Image *bmpImage);
Image *YUV2RGB(Image *bmpImage);

/* ------------------------------------
* Grayscale Relevant Function
*/ 
Image *Color2Gray(Image *bmpImage24);
Image *LogTansform(Image *bmpImage, int coefficient);
Image *HistogramEqualization(Image *bmpImage);


#endif