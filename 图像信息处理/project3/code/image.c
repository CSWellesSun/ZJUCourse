#include "image.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
Image *ReadBMP(FILE *finput)
{
    int i, j;

    // Initialize
    Image *bmpImage = (Image*)malloc(sizeof(Image));
    bmpImage->bmpFileHeader = (BITMAPFILEHEADER*)malloc(sizeof(BITMAPFILEHEADER));
    bmpImage->bmpInfo = (BITMAPINFOHEADER*)malloc(sizeof(BITMAPINFOHEADER));

    fseek(finput, 0, SEEK_SET);
    fread(bmpImage->bmpFileHeader, sizeof(BITMAPFILEHEADER), 1, finput);
    fread(bmpImage->bmpInfo, sizeof(BITMAPINFOHEADER), 1, finput);
    
    // Verify BMP
    if (bmpImage->bmpFileHeader->bfType != 0x4d42) {
        printf("This is not BMP file!\n");
        exit(1);
    }

    // read rgbQuad
    switch (bmpImage->bmpInfo->biBitCount) {
        case 8:
            bmpImage->bmpRgbQuad = (RGBQUAD*)malloc(sizeof(RGBQUAD) * 256);
            fread(bmpImage->bmpRgbQuad, sizeof(RGBQUAD), 256, finput);
            break;
        case 24:
            bmpImage->bmpRgbQuad = NULL;
            break;
    }

    // read bmpData
    fseek(finput, bmpImage->bmpFileHeader->bfOffBits, SEEK_SET);
    // BMP request the dataSizePerLine be the multiple of 4
    int colorPad =  (bmpImage->bmpInfo->biWidth * (bmpImage->bmpInfo->biBitCount / 8)) - 
                    (bmpImage->bmpInfo->biWidth * (bmpImage->bmpInfo->biBitCount / 8) + 3) / 4 * 4;
    bmpImage->bmpData = (BYTE*)malloc(sizeof(BYTE) * bmpImage->bmpInfo->biWidth * 
                        bmpImage->bmpInfo->biHeight * (bmpImage->bmpInfo->biBitCount / 8)); 
    for (i = 0; i < bmpImage->bmpInfo->biHeight; i++) {
        for (j = 0; j < bmpImage->bmpInfo->biWidth; j++)
            fread(&bmpImage->bmpData[i * bmpImage->bmpInfo->biWidth * bmpImage->bmpInfo->biBitCount / 8 + j * bmpImage->bmpInfo->biBitCount / 8], sizeof(BYTE), bmpImage->bmpInfo->biBitCount / 8, finput);
        fseek(finput, colorPad, SEEK_CUR);
    }

    return bmpImage;
}


Image *Color2Gray(Image *bmpImage24)
{
    if (bmpImage24->bmpInfo->biBitCount != 24)
        return NULL;
    
    int i, j;

    Image *bmpImage8 = (Image*)malloc(sizeof(Image));
    bmpImage8->bmpFileHeader = (BITMAPFILEHEADER*)malloc(sizeof(BITMAPFILEHEADER));
    bmpImage8->bmpInfo = (BITMAPINFOHEADER*)malloc(sizeof(BITMAPINFOHEADER));

    memcpy(bmpImage8->bmpFileHeader, bmpImage24->bmpFileHeader, sizeof(BITMAPFILEHEADER));
    memcpy(bmpImage8->bmpInfo, bmpImage24->bmpInfo, sizeof(BITMAPINFOHEADER));
    
    bmpImage8->bmpInfo->biBitCount = 8; // 8 bits gray BMP
    bmpImage8->bmpInfo->biSizeImage = (bmpImage8->bmpInfo->biWidth + 3) / 4 * 4 * bmpImage8->bmpInfo->biHeight; // update Image Information Size
    bmpImage8->bmpFileHeader->bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) + 256 * sizeof(RGBQUAD); // update the offbits because of RGBQUAD
    bmpImage8->bmpFileHeader->bfSize = bmpImage8->bmpFileHeader->bfOffBits + bmpImage8->bmpInfo->biSizeImage; // update File Size
    
    bmpImage8->bmpRgbQuad = (RGBQUAD*)malloc(sizeof(RGBQUAD) * 256);
    for (i = 0; i < 256; i++) {
        bmpImage8->bmpRgbQuad[i].rgbBlue = bmpImage8->bmpRgbQuad[i].rgbRed = bmpImage8->bmpRgbQuad[i].rgbGreen = i;
    }

    bmpImage8->bmpData = (BYTE*)malloc(sizeof(BYTE) * bmpImage8->bmpInfo->biWidth * bmpImage8->bmpInfo->biHeight);
    for (i = 0; i < bmpImage8->bmpInfo->biHeight; i++) {
        for (j = 0; j < bmpImage8->bmpInfo->biWidth; j++) {
            bmpImage8->bmpData[i * bmpImage8->bmpInfo->biWidth + j] = bmpImage24->bmpData[i * bmpImage24->bmpInfo->biWidth * 3 + j * 3] * 0.299 +
                                                                      bmpImage24->bmpData[i * bmpImage24->bmpInfo->biWidth * 3 + j * 3 + 1] * 0.587 +
                                                                      bmpImage24->bmpData[i * bmpImage24->bmpInfo->biWidth * 3 + j * 3 + 2] * 0.114;
        }
    }

    return bmpImage8;
}


void WriteBMP(Image *bmpImage, FILE *foutput)
{
    int i, j;
    
    fseek(foutput, 0, SEEK_SET);
    fwrite(bmpImage->bmpFileHeader, sizeof(BITMAPFILEHEADER), 1, foutput);
    fwrite(bmpImage->bmpInfo, sizeof(BITMAPINFOHEADER), 1, foutput);
    if (bmpImage->bmpRgbQuad != NULL)
        fwrite(bmpImage->bmpRgbQuad, sizeof(RGBQUAD), 256, foutput);

    // read bmpData
    fseek(foutput, bmpImage->bmpFileHeader->bfOffBits, SEEK_SET);
    // BMP request the dataSizePerLine be the multiple of 4
    int colorPad =  (bmpImage->bmpInfo->biWidth * (bmpImage->bmpInfo->biBitCount / 8)) - 
                    (bmpImage->bmpInfo->biWidth * (bmpImage->bmpInfo->biBitCount / 8) + 3) / 4 * 4;
    BYTE *padArray = (BYTE *)malloc(sizeof(BYTE) * colorPad);
    memset(padArray, 0, sizeof(BYTE) * colorPad); // initialize the pad
    for (i = 0; i < bmpImage->bmpInfo->biHeight; i++) {
        for (j = 0; j < bmpImage->bmpInfo->biWidth; j++)
            fwrite(&bmpImage->bmpData[i * bmpImage->bmpInfo->biWidth * bmpImage->bmpInfo->biBitCount / 8 + j * bmpImage->bmpInfo->biBitCount / 8], sizeof(BYTE), bmpImage->bmpInfo->biBitCount / 8, foutput);
        fwrite(padArray, sizeof(BYTE), colorPad, foutput);
    }
}

Image *RGB2YUV(Image *bmpImage)
{
    if (bmpImage->bmpInfo->biBitCount != 24)
        return NULL;
    
    Image *bmpImageYUV = Copy(bmpImage);
    int i, j;
    BYTE r, g, b;
    double y, u, v;

    // transform matrix is as follows
    double T[3][3] = {
                    {0.299, 0.587, 0.114},
                    {-0.169, -0.331, 0.5},
                    {0.5, -0.419, -0.081}
                    };

    for (i = 0; i < bmpImageYUV->bmpInfo->biHeight; i++) {
        for (j = 0; j < bmpImageYUV->bmpInfo->biWidth; j++) {
            r = bmpImageYUV->bmpData[i * bmpImageYUV->bmpInfo->biWidth * 3 + j * 3];
            g = bmpImageYUV->bmpData[i * bmpImageYUV->bmpInfo->biWidth * 3 + j * 3 + 1];
            b = bmpImageYUV->bmpData[i * bmpImageYUV->bmpInfo->biWidth * 3 + j * 3 + 2];
            y = r * T[0][0] + g * T[0][1] + b * T[0][2];
            u = r * T[1][0] + g * T[1][1] + b * T[1][2] + 128;
            v = r * T[2][0] + g * T[2][1] + b * T[2][2] + 128;
            bmpImageYUV->bmpData[i * bmpImageYUV->bmpInfo->biWidth * 3 + j * 3] = y;
            bmpImageYUV->bmpData[i * bmpImageYUV->bmpInfo->biWidth * 3 + j * 3 + 1] = u;
            bmpImageYUV->bmpData[i * bmpImageYUV->bmpInfo->biWidth * 3 + j * 3 + 2] = v;
        }
    }

    return bmpImageYUV;
}

Image *YUV2RGB(Image *bmpImage)
{
    if (bmpImage->bmpInfo->biBitCount != 24)
        return NULL;
    
    Image *bmpImageRGB = Copy(bmpImage);
    int i, j;
    BYTE y, u, v;
    double r, g, b;

    // transform matrix is as follows
    double T[3][3] = {
                    {1, 0, 1.403},
                    {1, -0.343, -0.714},
                    {1, 1.770, 0}
                    };

    for (i = 0; i < bmpImageRGB->bmpInfo->biHeight; i++) {
        for (j = 0; j < bmpImageRGB->bmpInfo->biWidth; j++) {
            y = bmpImageRGB->bmpData[i * bmpImageRGB->bmpInfo->biWidth * 3 + j * 3];
            u = bmpImageRGB->bmpData[i * bmpImageRGB->bmpInfo->biWidth * 3 + j * 3 + 1];
            v = bmpImageRGB->bmpData[i * bmpImageRGB->bmpInfo->biWidth * 3 + j * 3 + 2];
            r = y * T[0][0] + (u - 128) * T[0][1] + (v - 128) * T[0][2];
            g = y * T[1][0] + (u - 128) * T[1][1] + (v - 128) * T[1][2];
            b = y * T[2][0] + (u - 128) * T[2][1] + (v - 128) * T[2][2];
            bmpImageRGB->bmpData[i * bmpImageRGB->bmpInfo->biWidth * 3 + j * 3] = r;
            bmpImageRGB->bmpData[i * bmpImageRGB->bmpInfo->biWidth * 3 + j * 3 + 1] = g;
            bmpImageRGB->bmpData[i * bmpImageRGB->bmpInfo->biWidth * 3 + j * 3 + 2] = b;
        }
    }

    return bmpImageRGB;
}

Image *LogTansform(Image *bmpImage, int coefficient)
{
    if (bmpImage->bmpInfo->biBitCount != 8)
        return NULL;
    
    int i, j;
    BYTE maxL = 0, currentL, newL; // max saves the maximum luminance, currentL is the real luminance,
                                   // newL is the updated lumiance
    Image *bmpImageLog = Copy(bmpImage);
    // Find the maximum luminance
    for (i = 0; i < bmpImageLog->bmpInfo->biHeight; i++) {
        for (j = 0; j < bmpImageLog->bmpInfo->biWidth; j++) {
            if (bmpImageLog->bmpData[i * bmpImageLog->bmpInfo->biWidth + j] > maxL)
                maxL = bmpImageLog->bmpData[i * bmpImageLog->bmpInfo->biWidth + j];
        }
    }
    // calculate the new luminance
    for (i = 0; i < bmpImageLog->bmpInfo->biHeight; i++) {
        for (j = 0; j < bmpImageLog->bmpInfo->biWidth; j++) {
                currentL = bmpImageLog->bmpData[i * bmpImageLog->bmpInfo->biWidth + j];
                newL = log((double)currentL + 1) / log((double)maxL + 1) * coefficient;
                bmpImageLog->bmpData[i * bmpImageLog->bmpInfo->biWidth + j] = newL;
        }
    }

    return bmpImageLog;
}

Image *HistogramEqualization(Image *bmpImage)
{
    if (bmpImage->bmpInfo->biBitCount != 8)
        return NULL;
    
    int i, j;
    Image *bmpImageHE = Copy(bmpImage);
    int N = bmpImageHE->bmpInfo->biWidth * bmpImageHE->bmpInfo->biHeight;
    int grayNum[256] = {0}; // save the num of pixels of every grayscale
    double grayRatio[256] = {0}; // save the ratio of grayNum
    double grayAccumulateRatio[256] = {0}; // save the accumulate
    int grayHE[256] = {0}; // save transformed grayscale after HistogramEqualization

    // calculate the num of pixels of every grayscale
    for (i = 0; i < bmpImageHE->bmpInfo->biHeight; i++)
        for (j = 0; j < bmpImageHE->bmpInfo->biWidth; j++)
            grayNum[bmpImageHE->bmpData[i * bmpImageHE->bmpInfo->biWidth + j]]++;

    // calculate the ratio
    for (i = 0; i < 256; i++)
        grayRatio[i] = (double)grayNum[i] / N;
    
    // calculate the accumulate ratio
    grayAccumulateRatio[0] = grayRatio[0];
    for (i = 1; i < 256; i++)
        grayAccumulateRatio[i] = grayAccumulateRatio[i - 1] + grayRatio[i];
    
    // calculate the transformed grayscale after histogram equalization
    for (i = 0; i < 256; i++)
        grayHE[i] = grayAccumulateRatio[i] * 255 + 0.5;

    // update all the pixels
    for (i = 0; i < bmpImageHE->bmpInfo->biHeight; i++)
        for (j = 0; j < bmpImageHE->bmpInfo->biWidth; j++)
            bmpImageHE->bmpData[i * bmpImageHE->bmpInfo->biWidth + j] = grayHE[bmpImageHE->bmpData[i * bmpImageHE->bmpInfo->biWidth + j]]; 

    return bmpImageHE;
}

Image *Copy(Image *origin)
{
    Image *copy = (Image*)malloc(sizeof(Image));
    copy->bmpFileHeader = (BITMAPFILEHEADER*)malloc(sizeof(BITMAPFILEHEADER));
    copy->bmpInfo = (BITMAPINFOHEADER*)malloc(sizeof(BITMAPINFOHEADER));
    memcpy(copy->bmpFileHeader, origin->bmpFileHeader, sizeof(BITMAPFILEHEADER));
    memcpy(copy->bmpInfo, origin->bmpInfo, sizeof(BITMAPINFOHEADER));

    if (origin->bmpRgbQuad != NULL) {
        copy->bmpRgbQuad = (RGBQUAD*)malloc(sizeof(RGBQUAD) * 256);
        memcpy(copy->bmpRgbQuad, origin->bmpRgbQuad, sizeof(RGBQUAD) * 256);
    }
    else
        copy->bmpRgbQuad = NULL;

    copy->bmpData = (BYTE*)malloc(sizeof(BYTE) * origin->bmpInfo->biWidth * 
                        origin->bmpInfo->biHeight * (origin->bmpInfo->biBitCount / 8));
    memcpy(copy->bmpData, origin->bmpData, sizeof(BYTE) * origin->bmpInfo->biWidth * 
                        origin->bmpInfo->biHeight * (origin->bmpInfo->biBitCount / 8));
    
    return copy;
}

void Free(Image *bmpImage)
{
    free(bmpImage->bmpFileHeader);
    free(bmpImage->bmpInfo);
    free(bmpImage->bmpData);

    if (bmpImage->bmpRgbQuad != NULL)
        free(bmpImage->bmpRgbQuad);
    
    free(bmpImage);
}
