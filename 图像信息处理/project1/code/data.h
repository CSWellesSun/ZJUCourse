#ifndef _DATA_H_
#define _DATA_H_
#include <stdio.h>
// Read th BMP data from finput
void ReadData(FILE* finput, float **r, float **g, float **b);
// Convert RGB to YUV
void ConvertRGB2YUV(float *r, float *g, float *b, float **y, float **u, float **v);
// Change Y of YUV: judge whether y[i] is below the average and then increase y[i]
void ChangeY(float *y);
// Convert YUV to RGB
void ConvertYUV2RGB(float *r, float *g, float *b, float *y, float *u, float *v);
// Write the BMP data to foutput (need the information of finput)
void WriteData(FILE *foutput, FILE *finput, float *r, float *g, float *b);
#endif