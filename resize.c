// Copies a BMP file

#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    int n = atoi(argv[1]);
    // ensure proper usage
    if (argc != 4 || n == 0 || n > 100 || n < 0)
    {
        fprintf(stderr, "Usage: n infile outfile\n");
        return 1;
    }

    // remember filenames
    char *infile = argv[2];
    char *outfile = argv[3];

    // open input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }



    // define new bitmapinfo giving new dimensions
    BITMAPINFOHEADER bi_new;
    bi_new = bi;
    bi_new.biWidth = bi_new.biWidth * n;
    bi_new.biHeight = bi_new.biHeight * n;


    // determine padding for scanlines
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int padding_new = (4 - (bi_new.biWidth) * sizeof(RGBTRIPLE) % 4) % 4;

    bi_new.biSizeImage = ((sizeof(RGBTRIPLE) * bi_new.biWidth) + abs(padding_new)) * abs(bi_new.biHeight);

    BITMAPFILEHEADER bf_new;
    bf_new = bf;
    bf_new.bfSize = (bf_new.bfOffBits + bi_new.biSizeImage);

    // write outfile's BITMAPFILEHEADER
    fwrite(&bf_new, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi_new, sizeof(BITMAPINFOHEADER), 1, outptr);



    // iterate through each scanline
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        // Resize vertically
        for (int v = 0; v < n; v++)
        {

            // Resize Horizontally
            for (int h = 0; h < bi.biWidth; h++)
            {
                // temporary storage
                RGBTRIPLE triple;

                // read RGB triple from infile and write n times to outfile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

                for (int j = 0; j < n; j++)
                {
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                }
            }

            // Put in new padding
            for (int k = 0; k < padding_new; k++)
            {
                fputc(0x00, outptr);
            }

            // return infile cursor to start of scanline
            if (v < (n - 1))
            {
                fseek(inptr, -(bi.biWidth * sizeof(RGBTRIPLE)), SEEK_CUR);
            }

        }
        // Skip over padding in infile
        fseek(inptr, padding, SEEK_CUR);

    }
    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);
    // success
    return 0;
}
