
/*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//        Copyright (c) 1998-2000 Intel Corporation. All Rights Reserved.
//
//  VSS
//    $Workfile:: cooktile.cpp          $
//    $Revision:: 1                     $
//    $Modtime:: 17.05.98 14:07         $
//    $Nokeywords:                      $
//
//  Created: 17.05.98 13:35:16 by chan
//
*/

#include <ipl.h>
#include "cooktile.h"
#include "cookroom.h"


/* //////////////////////////////////////////////////////////////////////
// Name:       getTile
// Purpose:    Returns tile in the tile info structure of shape image.
// Context:    IPLib tutorial
// Returns:    1 if success
// Parameters: 
      IplImage* shape - image to be connected with tile data
      int xindex, int yindex - address of tile
      int tileMode - tile access mode, for future using
//
//  Notes:     The tile is a copy from a source image storage.
               Image storage is shape->ID field.
*/
int getTile( IplImage* shape, int xindex, int yindex ) {

   /// What tile get from? 
   IplImage* tileStorage = (IplImage*)shape->ID;
   /// tile size
   int tileWidth  = shape->tileInfo->width;
   int tileHeight = shape->tileInfo->height;
   /// ROI for copying
   IplROI roi = { 0, tileWidth * xindex, tileHeight * yindex, 
      tileWidth, tileHeight };
   /// create temporary image for copying
   IplImage* tile = cloneImageHeader( shape,  tileWidth, tileHeight );
   iplAllocateImage(tile,0,0);
   /// copy from the tile storage image to tile
   tileStorage->roi = &roi;
   iplCopy( tileStorage, tile );
   tileStorage->roi = NULL;
   /// shape will be owner of tile data
   shape->tileInfo->tileData = tile->imageData;
   /// tile header isn't needed more
   iplDeallocate( tile, IPL_IMAGE_HEADER );
   return 1;
}

/* //////////////////////////////////////////////////////////////////////
// Name:       putTile
// Purpose:    Put tile into a destination image storage
// Context:    IPLib tutorial
// Returns:    1 if success
// Parameters: 
      IplImage* shape - image connected with tile data
      int xindex, int yindex - address of tile
//
//  Notes:     
      The tile is the tile info structure of the shape image. 
      Tile is deleted after an operation. The tileData field 
      is set to NULL.
      Image storage is shape->ID field.
*/
int putTile( const IplImage* shape, int xindex, int yindex ) {

   /// What tile get from? 
   IplImage* tileStorage = (IplImage*)shape->ID;
   /// tile size
   int tileWidth  = shape->tileInfo->width;
   int tileHeight = shape->tileInfo->height;
   /// ROI for copying
   IplROI roi = { 0, tileWidth * xindex, tileHeight * yindex, 
      tileWidth, tileHeight };
   /// Create temporary image for copying   
   IplImage* tile = cloneImageHeader( shape,  tileWidth, tileHeight );
   tile->imageData = shape->tileInfo->tileData;
   shape->tileInfo->tileData = NULL;
   /// copy to a tile storage image
   tileStorage->roi = &roi;
   iplCopy( tile, tileStorage );
   tileStorage->roi = NULL;
   /// tile is no more 
   iplDeallocate( tile, IPL_IMAGE_ALL );
   return 1;
}

/* //////////////////////////////////////////////////////////////////////
// Name:       tileCallback
// Purpose:    Callback function of for this test tiling system
// Context:    IPLib tutorial
// Returns:    void
// Parameters: 
      IplImage* shape - image connected with tile data
      int xindex, int yindex - address of tile
      int tileMode - tile access mode
//
//  Notes:     
*/
void __stdcall tileCallback( const IplImage *shape, int xindex, int yindex, int mode ) {
   /// check image before calling
   if( !shape || !shape->tileInfo || !shape->ID ) return;
   switch( mode ) {
   case IPL_GET_TILE_TO_READ :
   case IPL_GET_TILE_TO_WRITE: 
      getTile( (IplImage*)shape, xindex, yindex ); 
      break;
   case IPL_RELEASE_TILE: 
      putTile( shape, xindex, yindex ); 
      break;
   }
}

/* //////////////////////////////////////////////////////////////////////
// Name:       createTiledImage
// Purpose:    create tiled image
// Context:    IPLib tutorial
// Returns:    image created or NULL
// Parameters: 
      IplImage* src - image which header is used to create new image header
      IplCallBack callback - callback function of a tile system
      int tilexsize, int tileysize - tile size
//
//  Notes:     ID of new image is source image
*/

IplImage* createTiledImage( IplImage* src, IplCallBack callback, int tilexsize,
   int tileysize ) {

   if( !src || !callback ) return NULL;
   IplImage* image = cloneImageHeader( src );
   if( image ) {
      image->ID        = (int)src;
      image->roi       = NULL;
      image->maskROI   = NULL;
      image->imageId   = NULL;
      image->imageData = NULL;
      image->tileInfo  = iplCreateTileInfo( callback, NULL, tilexsize, tileysize );
   }
   return image;
}

