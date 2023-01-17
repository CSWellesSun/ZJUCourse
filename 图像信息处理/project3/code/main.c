#include <stdio.h>
#include <stdlib.h>
#include "image.h"
int main()
{
    FILE *finput = fopen("gray.bmp", "rb");
    FILE *foutputLog = fopen("outputLog.bmp", "wb");
    FILE *foutputHisteq = fopen("outputHisteq.bmp", "wb");
    Image* bmpImage8 = ReadBMP(finput);
    
    Image* bmpImageLog = LogTansform(bmpImage8, 255);
    WriteBMP(bmpImageLog, foutputLog);
    
    Image* bmpImageHisteq = HistogramEqualization(bmpImage8);
    WriteBMP(bmpImageHisteq, foutputHisteq);

    Free(bmpImage8);
    Free(bmpImageLog);
    Free(bmpImageHisteq);   
    fclose(finput);
    fclose(foutputLog);
    fclose(foutputHisteq);
    return 0;
}