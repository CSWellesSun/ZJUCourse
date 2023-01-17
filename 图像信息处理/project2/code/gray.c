#include "gray.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
BITMAPFILEHEADER bmpFileHeader;
BITMAPINFOHEADER bmpInfo;

BYTE *Color2Gray(FILE *finput, FILE *foutput)
{
    fseek(finput, 0, SEEK_CUR);
    fread(&bmpFileHeader, sizeof(BITMAPFILEHEADER), 1, finput);
    fread(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, finput);
    
    // Verify BMP
    if (bmpFileHeader.bfType != 0x4d42)
    {
        printf("This is not BMP file!\n");
        exit(1);
    }

    // read Data
    fseek(finput, bmpFileHeader.bfOffBits, SEEK_SET);
    // BMP request the dataSizePerLine be the multiple of 4
    int i, j;
    int colorPad = (bmpInfo.biWidth * 3 + 3) / 4 * 4 - (bmpInfo.biWidth * 3);
    BYTE *Gray = (BYTE*)malloc(sizeof(BYTE) * bmpInfo.biWidth * bmpInfo.biHeight); 
    Pixel temp;

    for (i = 0; i < bmpInfo.biHeight; i++)
    {
        for (j = 0; j < bmpInfo.biWidth; j++)
        {
            fread(&temp, sizeof(BYTE), 3, finput);
            Gray[i * bmpInfo.biWidth + j] = temp.R * 0.299 + temp.G * 0.587 + temp.B * 0.114;
        }
        fseek(finput, colorPad, SEEK_CUR);
    }

    // Write Gray BMP
    WriteGrayBMP(Gray, foutput);
    return Gray;
}

BYTE FindThreshold(BYTE *Gray)
{
    // Get min and max pixel value
    int i, j;
    BYTE min = Gray[0];
    BYTE max = Gray[0];
    for (i = 0; i < bmpInfo.biHeight; i++)
        for (j = 0; j < bmpInfo.biWidth; j++)
        {
            if (Gray[i * bmpInfo.biWidth + j] < min)
                min = Gray[i * bmpInfo.biWidth + j];
            if (Gray[i * bmpInfo.biWidth + j] > max)
                max = Gray[i * bmpInfo.biWidth + j];
        }

    int threshold, optimalThreshold;
    double nF, nB;
    double wF, wB;
    double sumF, sumB;
    double averageF, averageB;
    double betweenVariance, maxBetweenVariance;
    maxBetweenVariance = 0;

    for (threshold = min + 1; threshold < max; threshold++)
    {
        nF = nB = sumF = sumB = 0;
        for (i = 0; i < bmpInfo.biHeight * bmpInfo.biWidth; i++)
            if (Gray[i] < threshold) // Background
            {
                nB++;
                sumB += Gray[i];
            }
            else // Foreground
            {
                nF++;
                sumF += Gray[i];
            }
        
        wF = nF / (bmpInfo.biHeight * bmpInfo.biWidth);
        wB = nB / (bmpInfo.biHeight * bmpInfo.biWidth);
        averageB = sumB / nB;
        averageF = sumF / nF;
        
        betweenVariance = wF * wB * (averageF - averageB) * (averageF - averageB);
        if (betweenVariance > maxBetweenVariance)
        {
            maxBetweenVariance = betweenVariance;
            optimalThreshold = threshold; 
        }
    }

    return (BYTE)optimalThreshold;
}

void Binarization(BYTE *Gray)
{
    BYTE threshold = FindThreshold(Gray);

    int i;
    for (i = 0; i < bmpInfo.biWidth * bmpInfo.biHeight; i++)
        if (Gray[i] < threshold)
            Gray[i] = 0;
        else
            Gray[i] = 255;
}

void WriteGrayBMP(BYTE *Gray, FILE *foutput)
{
    int i, j;
    // Write Gray BMP
    bmpInfo.biBitCount = 8; // 8 bits gray BMP
    bmpInfo.biSizeImage = (bmpInfo.biWidth + 3) / 4 * 4 * bmpInfo.biHeight; // update Image Information Size
    bmpFileHeader.bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) + 256 * sizeof(RGBQUAD); // update the offbits because of RGBQUAD
    bmpFileHeader.bfSize = bmpFileHeader.bfOffBits + bmpInfo.biSizeImage; // update File Size
    
    // Create RGBQUAD
    RGBQUAD *rgbquad = (RGBQUAD*)malloc(256 * sizeof(RGBQUAD));
    for (i = 0; i < 256; i++)
        rgbquad[i].rgbBlue = rgbquad[i].rgbGreen = rgbquad[i].rgbRed = i;
    
    fseek(foutput, 0, SEEK_SET);
    fwrite(&bmpFileHeader, sizeof(BITMAPFILEHEADER), 1, foutput);
    fwrite(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, foutput);
    fwrite(rgbquad, sizeof(RGBQUAD), 256, foutput);

    int grayPad = (bmpInfo.biWidth + 3) / 4 * 4 - bmpInfo.biWidth; // because every line needs to be multiples of 4, grayPad is the padding we need to fill in the end
    BYTE *padArray = (BYTE *)malloc(sizeof(BYTE) * grayPad);
    memset(padArray, 0, sizeof(BYTE) * grayPad); // initialize the pad
    for (i = 0; i < bmpInfo.biHeight; i++)
    {
        for (j = 0; j < bmpInfo.biWidth; j++)
            fwrite(&Gray[i * bmpInfo.biWidth + j], sizeof(BYTE), 1, foutput);
        fwrite(padArray, sizeof(BYTE), grayPad, foutput);
    }
    
    free(rgbquad); 
    free(padArray);   
}

BYTE *Dilation(BYTE *Gray, BYTE *structureElement, int sHeight, int sWidth)
{
    int i, j, m, n;
    BYTE *DilationGray = (BYTE*)malloc(sizeof(BYTE) * bmpInfo.biWidth * bmpInfo.biHeight);
    memcpy(DilationGray, Gray, sizeof(BYTE) * bmpInfo.biWidth * bmpInfo.biHeight);
    for (i = 0; i < bmpInfo.biHeight; i++)
    {
        for (j = 0; j < bmpInfo.biWidth; j++)
        {
            if (DilationGray[i * bmpInfo.biWidth + j] == 255)
                continue;
            for (m = - sHeight / 2; m <= sHeight / 2; m++)
            {
                for (n = -sWidth / 2; n <= sWidth / 2; n++)
                {
                    if (i + m < 0 || i + m >= bmpInfo.biHeight || j + n < 0 || j + n >= bmpInfo.biWidth)
                        continue;
                    if (Gray[(i + m) * bmpInfo.biWidth + j + n] == 255 && structureElement[(sHeight / 2 + m) * sWidth + (sWidth / 2 + n)])
                        DilationGray[i * bmpInfo.biWidth + j] = 255;
                }
            }
        }
    }
    return DilationGray;
}

BYTE *Erosion(BYTE *Gray, BYTE *structureElement, int sHeight, int sWidth)
{
    int i, j, m, n;
    BYTE *ErosionGray = (BYTE*)malloc(sizeof(BYTE) * bmpInfo.biWidth * bmpInfo.biHeight);
    memcpy(ErosionGray, Gray, sizeof(BYTE) * bmpInfo.biWidth * bmpInfo.biHeight);
    for (i = 0; i < bmpInfo.biHeight; i++)
    {
        for (j = 0; j < bmpInfo.biWidth; j++)
        {
            if (ErosionGray[i * bmpInfo.biWidth + j] == 0)
                continue;
            for (m = - sHeight / 2; m <= sHeight / 2; m++)
            {
                for (n = -sWidth / 2; n <= sWidth / 2; n++)
                {
                    if (i + m < 0 || i + m >= bmpInfo.biHeight || j + n < 0 || j + n >= bmpInfo.biWidth)
                        continue;
                    if (Gray[(i + m) * bmpInfo.biWidth + j + n] == 0 && structureElement[(sHeight / 2 + m) * sWidth + (sWidth / 2 + n)])
                        ErosionGray[i * bmpInfo.biWidth + j] = 0;
                }
            }
        }
    }
    return ErosionGray;
}

BYTE *Opening(BYTE *Gray, BYTE *structureElement, int sHeight, int sWidth)
{
    int i, j, m, n;
    BYTE *ErosionGray = Erosion(Gray, structureElement, sHeight, sWidth);
    BYTE *OpeningGray = Dilation(ErosionGray, structureElement, sHeight, sWidth);
    free(ErosionGray);
    return OpeningGray;
}

BYTE *Closing(BYTE *Gray, BYTE *structureElement, int sHeight, int sWidth)
{
    int i, j, m, n;
    BYTE *DilationGray = Dilation(Gray, structureElement, sHeight, sWidth);
    BYTE *ClosingGray = Erosion(DilationGray, structureElement, sHeight, sWidth);
    free(DilationGray);
    return ClosingGray;
}