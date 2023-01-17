#include "data.h"
#include <stdlib.h>
unsigned short bmFlag; // save the 'B' 'M' characters of the BMP 
unsigned int height; // save the height in pixels
unsigned int width; // save the width in pixels
unsigned int offsetData; // save the data part offset

void ReadData(FILE* finput, float **r, float **g, float **b)
{
    // verify the file
    fseek(finput, 0L, SEEK_SET);
    fread(&bmFlag, sizeof(char), 2, finput);
    if (bmFlag != 0x4d42) // Ox4d42 is the ASCII of 'B' 'M'
    {
        printf("This is not BMP file!\n");
        exit(1);
    }

    // read the width and height of the BMP
    fseek(finput, 0x12L, SEEK_SET);
    fread(&width, sizeof(char), 4, finput);
    fseek(finput, 0x16L, SEEK_SET);
    fread(&height, sizeof(char), 4, finput);

    // read the offset of data
    fseek(finput, 0xaL, SEEK_SET);
    fread(&offsetData, sizeof(char), 4, finput);

    // read Data
    fseek(finput, offsetData, SEEK_SET);

    // BMP request the dataSizePerLine be the multiple of 4
    int dataSizePerLine = (width * 3 + 3) / 4 * 4;
    unsigned char *dataLine = (unsigned char*)malloc(sizeof(char) * dataSizePerLine);
    *r = (float*)malloc(sizeof(float) * width * height);
    *g = (float*)malloc(sizeof(float) * width * height);
    *b = (float*)malloc(sizeof(float) * width * height);
    
    int i, j;
    for (i = 0; i < width * height; i++)
        (*r)[i] = (*g)[i] = (*b)[i] = 0;

    for (i = 0; i < height; i++)
    {
        // every time we fread one line and start to process
        fread(dataLine, sizeof(char), dataSizePerLine, finput);
        for (j = 0; j < width; j++)
        {
            // pay attention to the direction
            (*r)[width * (height - i - 1) + j] = (float)dataLine[j * 3 + 2];
            (*g)[width * (height - i - 1) + j] = (float)dataLine[j * 3 + 1];
            (*b)[width * (height - i - 1) + j] = (float)dataLine[j * 3];
        }
    }
    free(dataLine);
}

void ConvertRGB2YUV(float *r, float *g, float *b, float **y, float **u, float **v)
{   
    // transform matrix is as follows
    float T[3][3] = {
                    {0.299, 0.587, 0.114},
                    {-0.169, -0.331, 0.5},
                    {0.5, -0.419, -0.081}
                    };
    *y = (float*)malloc(sizeof(float) * width * height);
    *u = (float*)malloc(sizeof(float) * width * height);
    *v = (float*)malloc(sizeof(float) * width * height);

    int i, j;
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            // the following transformation ensures that Y U V is [0, 255]
            (*y)[i * width + j] = r[i * width + j] * T[0][0] + g[i * width + j] * T[0][1] + b[i * width + j] * T[0][2];
            (*u)[i * width + j] = r[i * width + j] * T[1][0] + g[i * width + j] * T[1][1] + b[i * width + j] * T[1][2] + 128;
            (*v)[i * width + j] = r[i * width + j] * T[2][0] + g[i * width + j] * T[2][1] + b[i * width + j] * T[2][2] + 128;
        }
    }
}

void ChangeY(float *y)
{
    int i;
    for (i = 0; i < width * height; i++)
    {
        y[i] /= 1.2;
    }
}

void ConvertYUV2RGB(float *r, float *g, float *b, float *y, float *u, float *v)
{
    // the transformation matrix is as follows
    float T[3][3] = {
                    {1, 0, 1.403},
                    {1, -0.343, -0.714},
                    {1, 1.770, 0}
                    };
    int i, j;
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            r[i * width + j] = y[i * width + j] * T[0][0] + (u[i * width + j] - 128) * T[0][1] + (v[i * width + j] - 128) * T[0][2];
            g[i * width + j] = y[i * width + j] * T[1][0] + (u[i * width + j] - 128) * T[1][1] + (v[i * width + j] - 128) * T[1][2];
            b[i * width + j] = y[i * width + j] * T[2][0] + (u[i * width + j] - 128) * T[2][1] + (v[i * width + j] - 128) * T[2][2];
        }
    }    
}

void WriteData(FILE *foutput, FILE *finput, float *r, float *g, float *b)
{
    // copy the data before the graphic data
    char *infor = (char*)malloc(sizeof(char) * offsetData);
    fseek(finput, 0, SEEK_SET);
    fread(infor, sizeof(char), offsetData, finput);
    fseek(foutput, 0, SEEK_SET);
    fwrite(infor, sizeof(char), offsetData, foutput);
    free(infor);

    // write color information to the file
    fseek(foutput, offsetData, SEEK_SET);
    int dataSizePerLine = (width * 3 + 3) / 4 * 4;
    unsigned char *dataLine = (unsigned char*)malloc(sizeof(char) * dataSizePerLine);
    
    int i, j;
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < dataSizePerLine; j++)
            dataLine[j] = 0;
        
        for (j = 0; j < width; j++)
        {
            // pay attention to the direction
            dataLine[j * 3 + 2] = (unsigned char)r[width * (height - i - 1) + j];
            dataLine[j * 3 + 1] = (unsigned char)g[width * (height - i - 1) + j];
            dataLine[j * 3] = (unsigned char)b[width * (height - i - 1) + j];
        }
        fwrite(dataLine, sizeof(char), dataSizePerLine, foutput);
    }
    
    free(dataLine);
}

