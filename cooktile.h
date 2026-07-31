#ifndef __COOKTILE_H__
#define __COOKTILE_H__

IplImage* createTiledImage( IplImage* src, IplCallBack callback, int tilexsize, int tileysize );

int getTile( IplImage* shape, int xindex, int yindex, int tileMode );
int putTile( const IplImage* shape, int xindex, int yindex, int tileMode );
void __stdcall tileCallback( const IplImage *shape, int xindex, int yindex, int mode );

#endif // __COOKTILE_H__
