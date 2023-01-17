#include "image.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

Image *ReadBMP(FILE *finput)
{
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
    int dataPerLine =  (bmpImage->bmpInfo->biWidth * (bmpImage->bmpInfo->biBitCount / 8) + 3) / 4 * 4;
    bmpImage->bmpData = (BYTE*)malloc(sizeof(BYTE) * dataPerLine * bmpImage->bmpInfo->biHeight); 
    int i;
    for (i = bmpImage->bmpInfo->biHeight - 1; i >= 0; i--)
        fread(&bmpImage->bmpData[i * dataPerLine], sizeof(BYTE), dataPerLine, finput);

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
    int dataPerLine24 =  (bmpImage24->bmpInfo->biWidth * (bmpImage24->bmpInfo->biBitCount / 8) + 3) / 4 * 4;
    int dataPerLine8 =  (bmpImage8->bmpInfo->biWidth * (bmpImage8->bmpInfo->biBitCount / 8) + 3) / 4 * 4;
    bmpImage8->bmpData = (BYTE*)malloc(sizeof(BYTE) * dataPerLine8 * bmpImage8->bmpInfo->biHeight);
    for (i = 0; i < bmpImage8->bmpInfo->biHeight; i++) {
        for (j = 0; j < bmpImage8->bmpInfo->biWidth; j++) {
            bmpImage8->bmpData[i * dataPerLine8 + j] = bmpImage24->bmpData[i * dataPerLine24 + j * 3] * 0.299 +
                                                       bmpImage24->bmpData[i * dataPerLine24 + j * 3 + 1] * 0.587 +
                                                       bmpImage24->bmpData[i * dataPerLine24 + j * 3 + 2] * 0.114;
        }
    }

    return bmpImage8;
}


void WriteBMP(Image *bmpImage, FILE *foutput)
{
    
    fseek(foutput, 0, SEEK_SET);
    fwrite(bmpImage->bmpFileHeader, sizeof(BITMAPFILEHEADER), 1, foutput);
    fwrite(bmpImage->bmpInfo, sizeof(BITMAPINFOHEADER), 1, foutput);
    if (bmpImage->bmpRgbQuad != NULL)
        fwrite(bmpImage->bmpRgbQuad, sizeof(RGBQUAD), 256, foutput);

    // read bmpData
    fseek(foutput, bmpImage->bmpFileHeader->bfOffBits, SEEK_SET);
    // BMP request the dataSizePerLine be the multiple of 4
    int dataPerLine =  (bmpImage->bmpInfo->biWidth * (bmpImage->bmpInfo->biBitCount / 8) + 3) / 4 * 4;

    int i;
    for (i = bmpImage->bmpInfo->biHeight - 1; i >= 0; i--)
        fwrite(&bmpImage->bmpData[i * dataPerLine], sizeof(BYTE), dataPerLine, foutput);
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
    int dataPerLine24 =  (bmpImageYUV->bmpInfo->biWidth * (bmpImageYUV->bmpInfo->biBitCount / 8) + 3) / 4 * 4;
    for (i = 0; i < bmpImageYUV->bmpInfo->biHeight; i++) {
        for (j = 0; j < bmpImageYUV->bmpInfo->biWidth; j++) {
            r = bmpImageYUV->bmpData[i * dataPerLine24 + j * 3];
            g = bmpImageYUV->bmpData[i * dataPerLine24 + j * 3 + 1];
            b = bmpImageYUV->bmpData[i * dataPerLine24 + j * 3 + 2];
            y = r * T[0][0] + g * T[0][1] + b * T[0][2];
            u = r * T[1][0] + g * T[1][1] + b * T[1][2] + 128;
            v = r * T[2][0] + g * T[2][1] + b * T[2][2] + 128;
            bmpImageYUV->bmpData[i * dataPerLine24 + j * 3] = y;
            bmpImageYUV->bmpData[i * dataPerLine24 + j * 3 + 1] = u;
            bmpImageYUV->bmpData[i * dataPerLine24 + j * 3 + 2] = v;
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
    int dataPerLine24 =  (bmpImageRGB->bmpInfo->biWidth * (bmpImageRGB->bmpInfo->biBitCount / 8) + 3) / 4 * 4;
    for (i = 0; i < bmpImageRGB->bmpInfo->biHeight; i++) {
        for (j = 0; j < bmpImageRGB->bmpInfo->biWidth; j++) {
            y = bmpImageRGB->bmpData[i * dataPerLine24 + j * 3];
            u = bmpImageRGB->bmpData[i * dataPerLine24 + j * 3 + 1];
            v = bmpImageRGB->bmpData[i * dataPerLine24 + j * 3 + 2];
            r = y * T[0][0] + (u - 128) * T[0][1] + (v - 128) * T[0][2];
            g = y * T[1][0] + (u - 128) * T[1][1] + (v - 128) * T[1][2];
            b = y * T[2][0] + (u - 128) * T[2][1] + (v - 128) * T[2][2];
            bmpImageRGB->bmpData[i * dataPerLine24 + j * 3] = r;
            bmpImageRGB->bmpData[i * dataPerLine24 + j * 3 + 1] = g;
            bmpImageRGB->bmpData[i * dataPerLine24 + j * 3 + 2] = b;
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
    int dataPerLine8 =  (bmpImageLog->bmpInfo->biWidth * (bmpImageLog->bmpInfo->biBitCount / 8) + 3) / 4 * 4;

    // Find the maximum luminance
    for (i = 0; i < bmpImageLog->bmpInfo->biHeight; i++) {
        for (j = 0; j < bmpImageLog->bmpInfo->biWidth; j++) {
            if (bmpImageLog->bmpData[i * dataPerLine8 + j] > maxL)
                maxL = bmpImageLog->bmpData[i * dataPerLine8 + j];
        }
    }
    // calculate the new luminance
    for (i = 0; i < bmpImageLog->bmpInfo->biHeight; i++) {
        for (j = 0; j < bmpImageLog->bmpInfo->biWidth; j++) {
                currentL = bmpImageLog->bmpData[i * dataPerLine8 + j];
                newL = log((double)currentL + 1) / log((double)maxL + 1) * coefficient;
                bmpImageLog->bmpData[i * dataPerLine8 + j] = newL;
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
    int dataPerLine8 =  (bmpImageHE->bmpInfo->biWidth * (bmpImageHE->bmpInfo->biBitCount / 8) + 3) / 4 * 4;
    // calculate the num of pixels of every grayscale
    for (i = 0; i < bmpImageHE->bmpInfo->biHeight; i++)
        for (j = 0; j < bmpImageHE->bmpInfo->biWidth; j++)
            grayNum[bmpImageHE->bmpData[i * dataPerLine8 + j]]++;

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
            bmpImageHE->bmpData[i * dataPerLine8 + j] = grayHE[bmpImageHE->bmpData[i * dataPerLine8 + j]]; 

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

    int dataPerLine =  (origin->bmpInfo->biWidth * (origin->bmpInfo->biBitCount / 8) + 3) / 4 * 4;
    copy->bmpData = (BYTE*)malloc(sizeof(BYTE) * origin->bmpInfo->biWidth * dataPerLine);
    memcpy(copy->bmpData, origin->bmpData, sizeof(BYTE) * origin->bmpInfo->biWidth * dataPerLine);
    
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

Image *GeometricTransform(Image *bmpImage, double Matrix[2][3], int canvas)
{
    int i, j, k;

    Image *bmpImageNew = (Image*)malloc(sizeof(Image));
    bmpImageNew->bmpFileHeader = (BITMAPFILEHEADER*)malloc(sizeof(BITMAPFILEHEADER));
    bmpImageNew->bmpInfo = (BITMAPINFOHEADER*)malloc(sizeof(BITMAPINFOHEADER));

    memcpy(bmpImageNew->bmpFileHeader, bmpImage->bmpFileHeader, sizeof(BITMAPFILEHEADER));
    memcpy(bmpImageNew->bmpInfo, bmpImage->bmpInfo, sizeof(BITMAPINFOHEADER));

    int minX = 0, maxX = canvas ? bmpImageNew->bmpInfo->biWidth - 1 : 0; // canvas determines whether to fill the background
    int minY = 0, maxY = canvas ? bmpImageNew->bmpInfo->biHeight - 1 : 0;
    int x, y;
    int vertexs[4][2] = {{0, 0}, {0, bmpImageNew->bmpInfo->biHeight - 1}, {bmpImageNew->bmpInfo->biWidth - 1, 0}, 
                        {bmpImageNew->bmpInfo->biWidth - 1, bmpImageNew->bmpInfo->biHeight - 1}}; // four corner vertexs
    // apply the matrix to the 4 corner vertexs to determine the size of the target image
    for (i = 0; i < 4; i++) {
        x = vertexs[i][0] * Matrix[0][0] + vertexs[i][1] * Matrix[0][1] + Matrix[0][2];
        y = vertexs[i][0] * Matrix[1][0] + vertexs[i][1] * Matrix[1][1] + Matrix[1][2];
        if (x < minX)
            minX = x;
        else if (x > maxX)
            maxX = x;
        if (y < minY)
            minY = y;
        else if (y > maxY)
            maxY = y;
    }
    // get the width and height
    bmpImageNew->bmpInfo->biWidth = maxX - minX + 1;
    bmpImageNew->bmpInfo->biHeight = maxY - minY + 1;
    // the data per line of the BMP must be the multiples of 4, here are the bytes per line of the original and new image 
    int dataPerLine = (bmpImage->bmpInfo->biWidth * (bmpImage->bmpInfo->biBitCount / 8) + 3) / 4 * 4;
    int dataPerLineNew =  (bmpImageNew->bmpInfo->biWidth * (bmpImageNew->bmpInfo->biBitCount / 8) + 3) / 4 * 4;
    
    if (bmpImage->bmpRgbQuad != NULL) {
        bmpImageNew->bmpRgbQuad = (RGBQUAD*)malloc(sizeof(RGBQUAD) * 256);
        memcpy(bmpImageNew->bmpRgbQuad, bmpImage->bmpRgbQuad, sizeof(RGBQUAD) * 256);
        bmpImageNew->bmpFileHeader->bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) + 256 * sizeof(RGBQUAD); // update the offbits because of RGBQUAD
    }
    else {
        bmpImageNew->bmpRgbQuad = NULL;
        bmpImageNew->bmpFileHeader->bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);
    }

    bmpImageNew->bmpInfo->biSizeImage = dataPerLineNew * bmpImageNew->bmpInfo->biHeight; // update Image Information Size
    bmpImageNew->bmpFileHeader->bfSize = bmpImageNew->bmpFileHeader->bfOffBits + bmpImageNew->bmpInfo->biSizeImage; // update File Size    
    bmpImageNew->bmpData = (BYTE*)malloc(sizeof(BYTE) * dataPerLineNew * bmpImageNew->bmpInfo->biHeight);
    memset(bmpImageNew->bmpData, 0, dataPerLineNew * bmpImageNew->bmpInfo->biHeight);

    // apply the matrix to all the pixels
    for (i = 0; i < bmpImage->bmpInfo->biHeight; i++) {
       for (j = 0; j < bmpImage->bmpInfo->biWidth; j++) {
            x = j * Matrix[0][0] + i * Matrix[0][1] + Matrix[0][2];
            y = j * Matrix[1][0] + i * Matrix[1][1] + Matrix[1][2];
            for (k = 0; k < bmpImage->bmpInfo->biBitCount / 8; k++)
                bmpImageNew->bmpData[(y - minY) * dataPerLineNew + (x - minX) * bmpImageNew->bmpInfo->biBitCount / 8 + k] =
                bmpImage->bmpData[i * dataPerLine + j * bmpImage->bmpInfo->biBitCount / 8 + k];
       }
    }

    return bmpImageNew;  
}

Image *Translation(Image *bmpImage, double x, double y)
{
    double translationMatrix[2][3] = {{1, 0, x}, {0, 1, y}};
    Image *bmpImageTranslation = GeometricTransform(bmpImage, translationMatrix, 1);
    return bmpImageTranslation;
}

Image *Rotation(Image *bmpImage, double theta)
{
    int i, j, k;
    double rotMatrix[2][3] = {{cos(theta), -sin(theta), 0}, {sin(theta), cos(theta), 0}}; // rot matrix
    Image *bmpImageRot = GeometricTransform(bmpImage, rotMatrix, 1);
    int dataPerLineRot =  (bmpImageRot->bmpInfo->biWidth * (bmpImageRot->bmpInfo->biBitCount / 8) + 3) / 4 * 4;

    // interpolation with the nearest pixels of the same row
    for (i = 1; i < bmpImageRot->bmpInfo->biHeight; i++) {
        for (j = 1; j < bmpImageRot->bmpInfo->biWidth; j++) {
            for (k = 0; k < bmpImageRot->bmpInfo->biBitCount / 8; k++)
                if (bmpImageRot->bmpData[i * dataPerLineRot + j * bmpImageRot->bmpInfo->biBitCount / 8 + k] == 0 && 
                    bmpImageRot->bmpData[i * dataPerLineRot + (j + 1) * bmpImageRot->bmpInfo->biBitCount / 8 + k] != 0)
                    bmpImageRot->bmpData[i * dataPerLineRot + j * bmpImageRot->bmpInfo->biBitCount / 8 + k] = 
                    bmpImageRot->bmpData[i * dataPerLineRot + (j - 1) * bmpImageRot->bmpInfo->biBitCount / 8 + k];
        }
    }
    return bmpImageRot;
}

Image *Mirror(Image *bmpImage, int sX, int sY)
{
    if ((abs(sX) != 1) || (abs(sY) != 1))
        return NULL;
    double mirrorMatrix[2][3] = {{sX, 0, 0}, {0, sY, 0}}; // mirror matrix
    Image *bmpImageMirror = GeometricTransform(bmpImage, mirrorMatrix, 1);
    return bmpImageMirror;
}

Image *Shear(Image *bmpImage, double dx, double dy)
{
    double shearMatrix[2][3] = {{1, dx, 0}, {dy, 1, 0}}; // shear matrix
    Image *bmpImageShear = GeometricTransform(bmpImage, shearMatrix, 1);
    return bmpImageShear;
}

Image *Scale(Image *bmpImage, double cx, double cy)
{
    int i, j, k;
    double srcX, srcY;
    int x1, y1, x2, y2;
    double scaleMatrix[2][3] = {{cx, 0, 0}, {0, cy, 0}}; // scale matrix
    Image *bmpImageScale = GeometricTransform(bmpImage, scaleMatrix, 0);

    // interpolation
    int dataPerLineScale =  (bmpImageScale->bmpInfo->biWidth * (bmpImageScale->bmpInfo->biBitCount / 8) + 3) / 4 * 4;
    int dataPerLine =  (bmpImage->bmpInfo->biWidth * (bmpImage->bmpInfo->biBitCount / 8) + 3) / 4 * 4;
    for (i = 0; i < bmpImage->bmpInfo->biHeight * cy; i++) {
        if (i == bmpImageScale->bmpInfo->biHeight - 1)
            break;
        for (j = 0; j < bmpImage->bmpInfo->biWidth * cx; j++) {
            if (j == bmpImageScale->bmpInfo->biWidth - 1)
                break;
            // srcX and srcY is the relevant pixels in the original image
            srcX = (j + 0.5) / cx - 0.5;
            srcY = (i + 0.5) / cy - 0.5;
            // figure out the 4 pixels around the (srcX, srcY)
            x1 = min(max(floor(srcX), 0), bmpImage->bmpInfo->biWidth - 2);
            y1 = min(max(floor(srcY), 0), bmpImage->bmpInfo->biHeight - 2);
            x2 = x1 + 1;
            y2 = y1 + 1;
            // bilinear interpolation
            for (k = 0; k < bmpImageScale->bmpInfo->biBitCount / 8; k++) {
                bmpImageScale->bmpData[i * dataPerLineScale + j * bmpImageScale->bmpInfo->biBitCount / 8 + k] = 
                bmpImage->bmpData[y1 * dataPerLine + x1 * bmpImage->bmpInfo->biBitCount / 8 + k] * (x2 - srcX) * (y2 - srcY) +
                bmpImage->bmpData[y1 * dataPerLine + x2 * bmpImage->bmpInfo->biBitCount / 8 + k] * (srcX - x1) * (y2 - srcY) +
                bmpImage->bmpData[y2 * dataPerLine + x1 * bmpImage->bmpInfo->biBitCount / 8 + k] * (x2 - srcX) * (srcY - y1) +
                bmpImage->bmpData[y2 * dataPerLine + x2 * bmpImage->bmpInfo->biBitCount / 8 + k] * (srcX - x1) * (srcY - y1);
            }

        }
    }
    
    return bmpImageScale;
}

Image *Convolution(Image *bmpImage, double *matrix, int size)
{
    int i, j, m, n, k, _m, _n;
    int startX, startY, endX, endY; // the surrounding part of (i, j)
    double result;
    Image *bmpFilter = Copy(bmpImage);
    int dataPerLine =  (bmpFilter->bmpInfo->biWidth * (bmpFilter->bmpInfo->biBitCount / 8) + 3) / 4 * 4; // bytes per line
    BYTE *bmpDataNew = (BYTE*)malloc(sizeof(BYTE) * bmpFilter->bmpInfo->biHeight * dataPerLine); // save the result after convolution
    
    for (i = 0; i < bmpFilter->bmpInfo->biHeight; i++) {
        for (j = 0; j < bmpFilter->bmpInfo->biWidth; j++) {
            startX = j - (size - 1) / 2;
            endX = j + size / 2;
            startY = i - (size - 1) / 2;
            endY = i + size / 2;
            for (k = 0; k < bmpFilter->bmpInfo->biBitCount / 8; k++) {
                result = 0;
                for (m = startY; m <= endY; m++) {
                    for (n = startX; n <= endX; n++) {
                        _m = min(max(0, m), bmpFilter->bmpInfo->biHeight - 1); // consider the fringe
                        _n = min(max(0, n), bmpFilter->bmpInfo->biWidth - 1);
                        result += bmpFilter->bmpData[_m * dataPerLine + _n * bmpFilter->bmpInfo->biBitCount / 8 + k] * matrix[(m - startY) * size + n - startX];
                    }
                }
                if (result > 255)
                    result = 255;
                else if (result < 0)
                    result = 0;
                bmpDataNew[i * dataPerLine + j * bmpFilter->bmpInfo->biBitCount / 8 + k] = result;
            }
        }
    }
    free(bmpFilter->bmpData);
    bmpFilter->bmpData = bmpDataNew;
    return bmpFilter;
}

Image *MeanFilter(Image *bmpImage, int size)
{
    int i;
    double *matrix = (double*)malloc(sizeof(double) * size * size); // mean matrix
    for (i = 0; i < size * size; i++) {
        matrix[i] = 1.0 / (size * size);
    }
    Image *bmpImageMF = Convolution(bmpImage, matrix, size);
    free(matrix);
    return bmpImageMF; 
}

Image *Laplacian(Image *bmpImage)
{
    int i, j, k;
    // double matrix[9] = {-1, -1, -1, -1, 8, -1, -1, -1, -1};
    double matrix[9] = {0, -1, 0, -1, 4, -1, 0, -1, 0}; // laplace matrix
    Image *bmpImageL = Convolution(bmpImage, matrix, 3);
    int dataPerLine =  (bmpImageL->bmpInfo->biWidth * (bmpImageL->bmpInfo->biBitCount / 8) + 3) / 4 * 4;
    int result;
    // fuse the laplacian graph and the original graph
    for (i = 0; i < bmpImage->bmpInfo->biHeight; i++) {
        for (j = 0; j < bmpImage->bmpInfo->biWidth; j++) {
            for (k = 0; k < bmpImage->bmpInfo->biBitCount / 8; k++) {
                result = bmpImage->bmpData[i * dataPerLine + j * bmpImageL->bmpInfo->biBitCount / 8 + k] + bmpImageL->bmpData[i * dataPerLine + j * bmpImageL->bmpInfo->biBitCount / 8 + k];
                if (result > 255)
                    result = 255;
                else if (result < 0)
                    result = 0;
                bmpImageL->bmpData[i * dataPerLine + j * bmpImageL->bmpInfo->biBitCount / 8 + k] = result;
            }
            }
        }
    return bmpImageL;
}