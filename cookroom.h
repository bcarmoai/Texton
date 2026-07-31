#ifndef __COOKROOM_H__
#define __COOKROOM_H__

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

char* getKeyArg( int argc, char** argv, const char* key );

typedef enum { Error = -1, Fail = 0, Good = 1 } ipStatus;

/* //////////////////////////////////////////////////////////////////////
// Name:       getIntArg, getStrArg
// Purpose:    get value from command line with a key
// Context:    parameters of applications are set by using command line
// Returns:    Fail as a rule
// Parameters: 
      int argc          - number of parameters
      char** argv       - parameters
      const char* key   - key for parameter
      int* value        - value to be returned
      char* value       - value to be returned
//
//  Notes:     Use getStrArg for string parameter getting
*/
ipStatus getIntArg( int argc, char** argv, const char* key, int* value );
ipStatus getStrArg( int argc, char** argv, const char* key, char* value );

bool isKeyUsed( int argc, char** argv, const char* key );

BITMAPINFOHEADER* ipLoad( const char* fname );

ipStatus GenerateRamp8uC1( IplImage* img );
ipStatus GenerateRamp8uC3P( IplImage* img );

int parseCmndLine( char* exename, const char* cmndline, char* line, int linelen, char** argv, int argvlen );

IplImage* cloneImageHeader( const IplImage* src, int width, int height );
IplImage* cloneImageHeader( const IplImage* src );

void ipInflateRoi( IplROI *roi, int step );
int ipSetImageROI( IplImage *img, int coi, int x, int y, int w, int h );
int ipSetROItoWholeImage( IplImage *img );
int ipDrawRoiBound( IplImage *img, IplROI *roi );


__int64 getPentiumCounter();


#define COPY_IMAGE_HEADER(src,dst,depth) \
   dst = iplCreateImageHeader( src->nChannels, src->alphaChannel, \
   depth, src->colorModel, src->channelSeq, src->dataOrder, \
   src->origin, src->align, src->width, src->height, NULL, NULL, NULL, NULL )


#endif
