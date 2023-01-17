#include "gray.h"
#include <stdlib.h>
#include <stdio.h>

int main()
{
    FILE *finput, *foutputG, *foutputB;
    FILE *foutputD, *foutputE, *foutputO, *foutputC;
    finput = fopen("input.bmp", "rb");
    foutputG = fopen("outputG.bmp", "wb"); // Gray
    foutputB = fopen("outputB.bmp", "wb"); // Binary
    foutputD = fopen("outputD.bmp", "wb"); // Dilation
    foutputE = fopen("outputE.bmp", "wb"); // Erosion
    foutputO = fopen("outputO.bmp", "wb"); // Opening
    foutputC = fopen("outputC.bmp", "wb"); // Closing
    BYTE *Gray = Color2Gray(finput, foutputG);
    Binarization(Gray);
    WriteGrayBMP(Gray, foutputB);
    BYTE structureElement[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
    BYTE *DilationGray = Dilation(Gray, structureElement, 3, 3);
    WriteGrayBMP(DilationGray, foutputD);
    BYTE *ErosionGray = Erosion(Gray, structureElement, 3, 3);
    WriteGrayBMP(ErosionGray, foutputE);
    BYTE *OpeningGray = Opening(Gray, structureElement, 3, 3);
    WriteGrayBMP(OpeningGray, foutputO);
    BYTE *ClosingGray = Closing(Gray, structureElement, 3, 3);
    WriteGrayBMP(ClosingGray, foutputC);
    free(Gray);
    free(DilationGray);
    free(ErosionGray);
    free(OpeningGray);
    free(ClosingGray);
    fclose(finput);
    fclose(foutputG);
    fclose(foutputB);
    fclose(foutputD);
    fclose(foutputE);
    fclose(foutputO);
    fclose(foutputC);
    return 0;
}