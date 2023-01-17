#include "image.h"
#include <stdio.h>
#include <stdlib.h>
#define PI 3.1415926
int main()
{
    FILE *finput, *foutputTrans, *foutputRot, *foutputMirror, *foutputShear, *foutputS1, *foutputS2;
    finput = fopen("output\\input.bmp", "rb");
    
    foutputTrans = fopen("output\\outputTrans.bmp", "wb");
    foutputRot = fopen("output\\outputRot.bmp", "wb");
    foutputMirror = fopen("output\\outputMirror.bmp", "wb");
    foutputShear = fopen("output\\outputShear.bmp", "wb");
    foutputS1 = fopen("output\\outputS1.bmp", "wb");
    foutputS2 = fopen("output\\outputS2.bmp", "wb");

    Image *bmpImage24 = ReadBMP(finput);
    Image *bmpImageTrans = Translation(bmpImage24, 100, -50);
    Image *bmpImageRot = Rotation(bmpImage24, PI / 3);
    Image *bmpImageMirror = Mirror(bmpImage24, 1, -1);
    Image *bmpImageShear = Shear(bmpImage24, 0.5, 0);
    Image *bmpImageS1 = Scale(bmpImage24, 0.5, 0.5);
    Image *bmpImageS2 = Scale(bmpImage24, 2, 2);

    WriteBMP(bmpImageTrans, foutputTrans);
    WriteBMP(bmpImageRot, foutputRot);
    WriteBMP(bmpImageMirror, foutputMirror);
    WriteBMP(bmpImageShear, foutputShear);
    WriteBMP(bmpImageS1, foutputS1);
    WriteBMP(bmpImageS2, foutputS2);

    Free(bmpImage24);
    Free(bmpImageTrans);
    Free(bmpImageRot);
    Free(bmpImageMirror);
    Free(bmpImageShear);
    Free(bmpImageS1);
    Free(bmpImageS2);

    fclose(finput);
    fclose(foutputTrans);
    fclose(foutputRot);
    fclose(foutputMirror);
    fclose(foutputShear);
    fclose(foutputS1);
    fclose(foutputS2);
    
    return 0;
}