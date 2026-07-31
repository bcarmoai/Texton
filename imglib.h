// Program:     c++ image processing library
// File:        imglib.h
// Description: library header file
// Author:      Qi Zhao <qizhao@ifp.uiuc.edu> (QZ)
// Environment: ANSI C++
// Notes:       main header file
// Revisions:   1.0     09/01/01 (QZ) first release
//              1.1     11/01/01 (QZ) added JPEG read/write support
//              1.2     11/14/01 (QZ) added GIF read support
//              1.3     11/15/01 (QZ) JPEG and GIF support now optional

#ifndef IPL_LIB_H
#define IPL_LIB_H

#include <fstream.h>
#include <math.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WITH_JPEG
    #include "Cjpeg.h"
#endif
#ifdef WITH_GIF
    #include "Cgif.h"
#endif

// some shorthand notations
#ifndef uchar
    typedef unsigned char uchar;
#endif
#ifndef uint
    typedef unsigned int uint;
#endif
#ifndef ulong
    typedef unsigned long int ulong;
#endif
typedef struct RGBCOLOR {
    uchar r;
    uchar g;
    uchar b;
} Color;

// some computational macros
#define CLIP_UPPER(x, y)        ( (x)>(y) ? (y) : (x) )
#define CLIP_LOWER(x,y)         ( (x)<(y) ? (y) : (x) )
#define ABS(x)                  ( (x)>0 ? (x) : -(x) )
#define RINT(x)                 ( x-floor(x)>0.5 ? ceil(x) : floor(x) )
//#define MIN(x, y)               ( (x) >= (y) ? (y) : (x) )
//#define MAX(x, y)               ( (x) >= (y) ? (x) : (y) )
#define LOG2(x)                 ( log(x) / log(2.0) )
#define IS_INTEGER(x)           ( ((double)x)==floor((double)x) ? 1 : 0 )

// some default values
#ifndef TRUE
    #define TRUE 1
#endif
#ifndef FALSE
    #define FALSE 0
#endif
const uint ROW = 512;                 // default image height
const uint COL = 512;                 // default image width
#ifndef PI
    const double PI = 3.14159265358979323846;
#endif
// the image type enumerator
enum TYPE {GRAY, RGB, YUV, BIN};
// all small letters are for ASCII variants
enum FORMAT {RAW, pbm, PBM, pgm, PGM, ppm, PPM, JPG, TIF, BMP, GIF};
enum METHOD {SINCF,SPLINE};

// the Cimage class
class Cimage {

    public:
        Cimage();
        Cimage(uint row, uint col, TYPE type);
        ~Cimage();
        void freeMemory();
        void initialize();

        void setSize(uint row, uint col);
        void setType(TYPE type);
        uint getRow(void);
        uint getCol(void);
        TYPE getType(void);

        // image IO routines
        void input(char *inFile, FORMAT f);
        void output(char *outFile, FORMAT f);
#ifdef WITH_JPEG
        void setJpgProperty(unsigned int quality, int progressive=0);
#endif

        // image manipulation methods
        void fillSolid(Color fillColor);
        void histogram(double *x, ulong xSize, uint *hist, uint numBin);

        void rotateAngle(int angle);
        Cimage* rotateAngle(double degree);
        void RGBtoGray(void);
        void adjustBrightness(double factor);   
        void adjustHue(double factor, bool toRGB=FALSE);
        void adjustSaturation(double factor, bool toRGB=FALSE);
        void cropRect(uint x, uint y, uint width, uint height);
        void mirrorHorizontal();
        void mirrorVertical();
        void reduceColorDepth(uint numBits=8);
        void setHue(double hue, bool toRGB=FALSE);
        void setSaturation(double sat, bool toRGB=FALSE);
        void adjustContrast(double factor, bool toRGB=FALSE);
        void autoContrast(bool toRGB=FALSE);
        Cimage* resize(double rowFactor, double colFactor,
                        METHOD method=SPLINE);
        Cimage* resize(uint newRow, uint newCol, METHOD method=SPLINE);

        void RGB_YCrCb(void);
        void YCrCb_RGB(void);
        void RGB_HSI(void);
        void HSI_RGB(void);        
        
        double *R;
        double *G;
        double *B;
        double *Y;
        double *Cr;
        double *Cb;
        double *H;
        double *S;
        double *I;
        
    private:
        uint row;
        uint col;
        TYPE type;
        bool toHSI;

#ifdef WITH_JPEG
        Cjpeg *jpg;
        unsigned int jpgQuality;
        int jpgProgressive;
#endif
#ifdef WITH_GIF
        Cgif *gif;
#endif

        void switchRow(double *pRow, int i);
        void switchRow(double *pRow1, double *pRow2);
        void rotatePlane_90(double *pPlane);
        void rotatePlane_270(double *pPlane);

        void reverseRow(double *pPlane, int row);
        void reverseCol(double *pPlane, int col);

        void clipUpperLower(double *x, ulong size);

};

#endif
