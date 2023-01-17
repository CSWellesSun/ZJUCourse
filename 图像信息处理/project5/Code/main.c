#include "image.h"
#include <stdio.h>
#include <stdlib.h>
#define PI 3.1415926
int main()
{
    FILE *finput, *foutputMF, *foutputL;
    finput = fopen("output\\input.bmp", "rb");
    foutputMF = fopen("output\\outputMF.bmp", "wb");
    foutputL = fopen("output\\outputL.bmp", "wb");
    Image *bmpImage = ReadBMP(finput);
    Image *bmpImageMF = MeanFilter(bmpImage, 3);
    Image *bmpImageL = Laplacian(bmpImage);
    WriteBMP(bmpImageMF, foutputMF);
    WriteBMP(bmpImageL, foutputL);
    free(bmpImage);
    free(bmpImageMF);
    free(bmpImageL);
    fclose(foutputMF);
    fclose(foutputL);
    fclose(finput);    
    return 0;
}