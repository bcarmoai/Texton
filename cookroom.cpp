
/*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//        Copyright (c) 1998-2000 Intel Corporation. All Rights Reserved.
//
//  VSS
//    $Workfile:: cookroom.cpp          $
//    $Revision:: 2                     $
//    $Modtime:: 12.04.98 10:55         $
//    $Nokeywords:                      $
//
//  Created: 12.04.98 10:55:21 by chan
//
      It was build under Borland 5.02 without problem. 28.04.98
*/

#pragma warning(disable:4786)
#include <stdexcept>
#include <fstream>
using namespace std;

#include <ipl.h>

#include "cookroom.h"
#include "macros.inc"

/* //////////////////////////////////////////////////////////////////////
// Name:       getKeyArg
// Purpose:    get string from command line which corresponds to key
// Context:    parameters of applications are set by using command line
// Returns:    ptr to an argument string or NULL
// Parameters: 
      int argc          - number of parameters
      char** argv       - parameters
      const char* key   - key for parameter
//
//  Notes:     used by the getIntArg, getStrArg functions
*/

char* getKeyArg( int argc, char** argv, const char* key ) {

   for( int i=1; i<argc; ++i ) {
      char* p = argv[i];
      if( p && strlen(p) >= 2 && ('/' == p[0] || '-' == p[0]) && key[0] == p[1] ) return p+2;
   }
   return NULL;
}


/* //////////////////////////////////////////////////////////////////////
// Name:       getIntArg
// Purpose:    get integer value from command line with a key
// Context:    parameters of applications are set by using command line
// Returns:    Fail as a rule
// Parameters: 
      int argc          - number of parameters
      char** argv       - parameters
      const char* key   - key for parameter
      int* value        - value to be returned
//
//  Notes:     Use getStrArg for string parameter getting
*/
ipStatus getIntArg( int argc, char** argv, const char* key, int* value ) {

   char* p = getKeyArg( argc, argv, key );
   if( p && *p ) {
      *value = atoi( p );
      return Good;
   }
   return Fail;
}

bool isKeyUsed( int argc, char** argv, const char* key ) {
   return NULL != getKeyArg( argc, argv, key );
}

/* //////////////////////////////////////////////////////////////////////
// Name:       getStrArg
// Purpose:    get string value from command line with a key
// Context:    parameters of applications are set by using command line
// Returns:    Fail as a rule
// Parameters: 
      int argc          - number of parameters
      char** argv       - parameters
      const char* key   - key for parameter
      char* value       - value to be returned
//
//  Notes:     Use getIntArg for integer parameter getting
*/
ipStatus getStrArg( int argc, char** argv, const char* key, char* value ) {

   char* p = getKeyArg( argc, argv, key );
   if( p && *p ) {
      strcpy( value, p );
      return Good;
   }
   return Fail;
}


/* //////////////////////////////////////////////////////////////////////
// Name:       ipLoad
// Purpose:    load bmp-file
// Context:    IPLib tutorial
// Returns:    bmp
// Parameters: fname - bmp-file name to read
//
//  Notes:     
*/

BITMAPINFOHEADER* ipLoad( const char* fname ) {

   if( !fname ) return NULL;
   
   BITMAPINFOHEADER *infohdr = NULL;

   ifstream fsrc;

   try {
      BITMAPFILEHEADER filehdr;
      BITMAPINFOHEADER bmphdr;

      fsrc.open( fname, ios::in | ios::binary );
      if( fsrc.fail() ) throw runtime_error("Problem to open source file");


      fsrc.read( (char*)&filehdr, sizeof(BITMAPFILEHEADER) );
      if( fsrc.fail() ) throw runtime_error("Problem to read bmp-file header from source file");

      if( 0x4d42 != filehdr.bfType ) throw runtime_error("Wrong type of source file");

      fsrc.read( (char*)&bmphdr, sizeof(BITMAPINFOHEADER) );
      if( fsrc.fail() ) throw runtime_error("Problem to read bmp-header from source file");

      if( 0 == bmphdr.biSizeImage )
         bmphdr.biSizeImage =
            (((( bmphdr.biWidth * bmphdr.biBitCount ) + 31) & ~31) >> 3) * bmphdr.biHeight;

      if( 0 == bmphdr.biClrUsed ) {
         if( BI_BITFIELDS == bmphdr.biCompression )
            bmphdr.biClrUsed = 3;
         else 
            bmphdr.biClrUsed = bmphdr.biBitCount < 16 ? 1 << bmphdr.biBitCount : 256;
      }

      int colorbytes = sizeof(RGBQUAD) * bmphdr.biClrUsed;
      int totalbytes = sizeof(BITMAPINFOHEADER) + colorbytes + bmphdr.biSizeImage;

      infohdr = (BITMAPINFOHEADER*)malloc( totalbytes );
      if( !infohdr ) throw runtime_error("Problem to allocate memory for DIB data");
      
      char* quads = (char*)infohdr + sizeof(BITMAPINFOHEADER);
      char* pixels = (char*)quads + colorbytes;

      memcpy( infohdr, &bmphdr, sizeof( bmphdr) );

      fsrc.read( (char*)quads, colorbytes );
      fsrc.read( (char*)pixels, bmphdr.biSizeImage );

      fsrc.close();
   }
   catch( runtime_error e ) {
      TRACE0( e.what() );
      if( infohdr ) { 
         free( infohdr );
         infohdr = 0;
      }
   }

   return infohdr;
}

/* //////////////////////////////////////////////////////////////////////
// Name:       GenerateRamp
// Purpose:    Fills 8u, 1 channel image with a ramp 
// Context:    IPLib tutorial
// Returns:    ipStatus
// Parameters: img - IPLib image to create ramp
//
//  Notes:     image img should be created ouside of the function
*/

ipStatus GenerateRamp8uC1( IplImage* img )
{
	// check that input is valid
	if( !img || !img->imageData ) return Error;
   /// check 8u and C1
   if( 1 != img->nChannels || IPL_DEPTH_8U != img->depth ) return Fail;

	int x, y, val=0;
	char *p = img->imageData;
	
	//fill in the imagedata field of img
	for( y=0; y < img->height; y++) {
		for( x=0; x < img->width; x++) p[x] = val + x;
      val += img->width;
		p += img->widthStep;
	}
   return Good;
}

/* //////////////////////////////////////////////////////////////////////
// Name:       GenerateRamp8uC3P
// Purpose:    Fills 8u, 3 channel, plane image 
// Context:    IPLib tutorial
// Returns:    ipStatus
// Parameters: img - IPLib image to fill in
//
//  Notes:     
*/

ipStatus GenerateRamp8uC3P( IplImage* img )
{
	// check that input is valid
	if( !img || !img->imageData ) return Error;
   /// check 8u, C3, plane
   if( 3 != img->nChannels 
      || IPL_DEPTH_8U != img->depth 
      || IPL_DATA_ORDER_PLANE != img->dataOrder ) return Fail;

	int x, y, val=0, planesize = img->widthStep * img->height;
	char *p = img->imageData;
	
	/// fill in the imagedata field of img
   /// red plane
	for( y=0; y < img->height; y++) {
		for( x=0; x < img->width; x++) p[x] = val + x;
      val += img->width;
		p += img->widthStep;
	}
   /// green plane
   val = 0;
   int valstep = max( 1, 255/img->height );
   for( y=0; y < img->height; y++) {
		for( x=0; x < img->width; x++) p[x] = val;
      val += valstep;
		p += img->widthStep;
	}
   /// blue plane
   for( int index = 0; index < planesize; index++) {
      if( index % 2 )
         p[index] = (char)0;
      if( index % 3 )
         p[index] = (char)66;
      if( index % 5 )
         p[index] = (char)170;
      else
         p[index] = (char)255;
   }
   return Good;
}


/* //////////////////////////////////////////////////////////////////////
// Name:       parseCmndLine
// Purpose:    simulate argc and argv parameters
// Context:    IPLib tutorial
// Returns:    num of arguments including application name
// Parameters: 
      char* exename - application name
      const char* cmndline - command line from WinMain function parameters
      char* line - buffer to copy and operate with command line
      int linelen - length of the line buffer
      char** argv - buffer to place string pointrs after parsing
      int argvlen - length of beffer of pointersargv
//
//  Notes:     this function is needed to transform WIN main to DOS main
*/
int parseCmndLine( char* exename, const char* cmndline, char* line, int linelen, char** argv, int argvlen ) {

   for( int i=0; i<argvlen; i++) argv[i] = NULL;
   char* seps = " ,";                     /// argement separators
   int argc = 1;                          /// number of parameters
   argv[0] = exename;                     /// the first standard argument
   memset( line, 0, linelen );
   strncpy( line, cmndline, linelen-1 );
   char* token = strtok( line, seps );    /// the first true argument
   while( token != NULL && argc <= argvlen ) {
      argv[argc++] = token;
      token = strtok( NULL, seps );   
   }
   return argc;
}


IplImage* cloneImageHeader( const IplImage* src, int width, int height ) {

   IplImage* clone = NULL;
   if( src ) {
      clone = iplCreateImageHeader( src->nChannels, src->alphaChannel,
         src->depth, (char*)src->colorModel, (char*)src->channelSeq, src->dataOrder,
         src->origin, src->align, width, height, 
         NULL, NULL, NULL, NULL);
   }
   return clone;
}

IplImage* cloneImageHeader( const IplImage* src ) {
   if( !src ) return NULL;
   return cloneImageHeader( src, src->width, src->height );
}

void ipInflateRoi( IplROI *roi, int step ) {
   roi->xOffset -= step;
   roi->yOffset -= step;
   roi->width += (step*2);
   roi->height += (step*2);
}

int ipSetImageROI( IplImage *img, int coi, int x, int y, int w, int h ) {

   if( !img ) return 0;
   if( !img->roi )
      img->roi = iplCreateROI( coi, x,y, w,h );
   else
      iplSetROI( img->roi, coi, x,y, w,h );
   return !img->roi;
}

int ipSetROItoWholeImage( IplImage *img ) {

   if( !img ) return 0;
   img->roi = iplCreateROI( 0, 0,0, img->width, img->height );
   return !img->roi;
}

int ipDrawRoiBound( IplImage *img, IplROI *roi ) {
   if( !img || !roi ) return 0;
   IplROI r = *roi;
   IplROI *old = img->roi;
   img->roi = &r;
   iplXorS( img, img, 0x55 );
   ipInflateRoi( &r, -1 );
   iplXorS( img, img, 0x55 );
   img->roi = old;
   return 1;
}

/// function to read TSC from cpuinfo32 library. I don't 
/// know what it returns but see it exchanges eax and edx
extern "C" __int64 winrdtsc();

__int64 getPentiumCounter() { 
   winrdtsc();
   __asm mov   ecx, edx
   __asm mov   edx, eax
   __asm mov   eax, ecx
}
