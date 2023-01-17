#include <stdio.h>
#include <stdlib.h>
#include "data.h"
int main()
{
    float *r, *g, *b; // save the R G B
    float *y, *u, *v; // save the Y U V
    float *gray; // save the gray scale

    FILE *finput; // Input BMP file
    finput = fopen("input.bmp", "rb");
    if (finput == NULL)
    {
        printf("No input file!\n");
        exit(1);
    }
    
    FILE *fgoutput; // Output the gray BMP
    fgoutput = fopen("grayoutput.bmp", "wb");
    if (fgoutput == NULL)
    {
        printf("Fail to write!\n");
        exit(1);
    }

    FILE *fcoutput; // Output the RGB color BMP
    fcoutput = fopen("coloroutput.bmp", "wb");
    if (fgoutput == NULL)
    {
        printf("Fail to write!\n");
        exit(1);
    }

    ReadData(finput, &r, &g, &b);
    ConvertRGB2YUV(r, g, b, &y, &u, &v);
    WriteData(fgoutput, finput, y, y, y);
    ChangeY(y);
    ConvertYUV2RGB(r, g, b, y, u, v);
    WriteData(fcoutput, finput, r, g, b);
    

    fclose(finput);
    fclose(fgoutput);
    fclose(fcoutput);
    free(r);
    free(g);
    free(b);
    free(y);
    free(u);
    free(v);
    free(gray);
    return 0;
}