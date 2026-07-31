/*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//        Copyright (c) 1998-2000 Intel Corporation. All Rights Reserved.
//
//  VSS
//    $Workfile:: cookdraw.cpp          $
//    $Revision:: 5                     $
//    $Modtime:: 18.05.98 17:20         $
//    $Nokeywords:                      $
//
//  Author:  Anton G. Subbotin (cy)
//  Modified by Alexey Korchuganov 

//  Recreated: 12.04.98 14:09:25 by chan
      + simplicity
      + Reset( event ) was added
      + Thread Data structure is not passed as lParam parameter
      + windows are arrived on the screen with shift in thread mode and in
         center of screen in modal mode
      + all types of images (depth, origin, order) reduce to one type
         (8u, pixel) before drawing
      + IPLib functions are used to convert image to drawed type
      + property page was excluded because only one type of images is shown,
         the others are converted to one.
      + drawing in pixel mode uses SetDIBitsToDevice function and is faster

   Teu 13-Jul-1999 16:29, chan
   Shifts were excluded, Scale is used

//
*/

#pragma warning(disable:4786)
#include <string>
using namespace std;

#include <stdlib.h>
#include "macros.inc"

#include <ipl.h>
#include "cookroom.h"
#include "cookdraw.h"

static const char* g_imgErrStr = "Problem to draw IPLib image";
static const char* g_name      = "IPLib Viewer";

ipStatus ipDraw( HDC hdc, IplImage* img );


/// class to simplify a work with WIN32 CriticalSection
/// Now the following declaration is needed only to
/// execute Enter and Leave procedures. Example of using
///   CritSect cs( a_section_ptr );

class CritSect {
   CRITICAL_SECTION* m_section;
public:
   CritSect( CRITICAL_SECTION* section )  : m_section(section) {
      EnterCriticalSection( m_section ); 
   }
   ~CritSect() { LeaveCriticalSection( m_section ); }
};


/// Thread Data item

struct ThreadData {
   DWORD id;                           /// thread-owner identificator
   bool isModalView;                   /// is modal or thread viewer
   IplImage* img;                      /// image to draw
   char text[128];                     /// text about IplImage image
   /// non standard copy operator
   void operator=( const ThreadData & thd ) {
      id = thd.id;
      isModalView = thd.isModalView;
      img = thd.img;
      lstrcpy( text, thd.text );
   }
};

/// Array of thread data

class ThreadDataArray {

   enum { NUMOF = 10 };                /// num of threads allowed

   ThreadData m_data[ NUMOF ];         /// data of threads
   CRITICAL_SECTION m_cs_data;         /// its critical section
   
   int m_numofThreads;                 /// current num of threads
   CRITICAL_SECTION m_cs_numof;        /// its critical section

public:
   /// ctr inits crit sects, event, id's
   ThreadDataArray();
   /// dtr dels crit sects, close event
   ~ThreadDataArray() {
      CritSect csn( &m_cs_numof );
      if( m_numofThreads <= 0 ) {
         ::DeleteCriticalSection( &m_cs_numof );
         ::DeleteCriticalSection( &m_cs_data );
         ::CloseHandle( m_hevent );
      }
   }
   /// find id thread data, id may be 0
   ThreadData* operator[] (DWORD id);
   /// find id thread, del img, dec counter, clear id
   bool close( DWORD idthread );
   /// find id=0, copy thread data, inc counter
   bool open ( const ThreadData& thd );
   /// check if no more place
   bool isfull() {
      CritSect csn( &m_cs_numof );
      return m_numofThreads >= NUMOF;
   }
   /// num of threads which are being executed
   int numof() {
      CritSect csn( &m_cs_numof );
      return m_numofThreads;
   }

   HANDLE m_hevent;                    /// event to wait for dialog inited
};

/// Create two critical sections and event
ThreadDataArray::ThreadDataArray() : m_numofThreads(0) {

   InitializeCriticalSection( &m_cs_data );
   InitializeCriticalSection( &m_cs_numof );

   /// security_attr=NULL, manual_reset=FALSE, init_state=busy=FALSE
   m_hevent = ::CreateEvent( NULL, FALSE, FALSE, "Thread Data Array Event");

   for( int i=0; i<NUMOF; ++i ) m_data[i].id = 0;
}

/// id may be 0 as well, then a free cell is found
ThreadData* ThreadDataArray::operator[] ( DWORD id ) {

   CritSect csd( &m_cs_data );
   for( int i=0; i<NUMOF; ++i ) 
      if( m_data[i].id == id ) return &m_data[i];
   return NULL;
}

/// find id thread, delete img, clear id, dec counter
bool ThreadDataArray::close( DWORD idthread ) {

   CritSect csd( &m_cs_data );
   for( int i=0; i<NUMOF; ++i ) 
      if( m_data[i].id == idthread ) {
         iplDeallocate( m_data[i].img, IPL_IMAGE_ALL );
         CritSect csn( &m_cs_numof );
         m_data[i].id = 0;
         --m_numofThreads;
         return true;
      }
   return false;
}

/// find 0 id, copy thread data, inc counter
bool ThreadDataArray::open( const ThreadData& thd ) {

   CritSect csd( &m_cs_data );
   for( int i=0; i<NUMOF; ++i ) 
      if( 0 == m_data[i].id ) {
         CritSect csn( &m_cs_numof );
         m_data[i] = thd;
         ++m_numofThreads;
         return true;
      }
   return true;
}

/// Global thread data array  /// Global thread data array  /// Global th
static ThreadDataArray g_threads;

/* //////////////////////////////////////////////////////////////////////
// Name:       ViewProc
// Purpose:    Dialog Procedure
// Context:    IPLib viewer
// Returns:    TRUE as a rule
// Parameters: 
      HWND hdlg - handle of dialog window
      UINT message - message code
      WPARAM wParam - commnad code
      LPARAM - not used
//
//  Notes:

*/

static LRESULT CALLBACK ViewProc( HWND hdlg, UINT message, WPARAM wParam, LPARAM ) {

   switch( message ) {
      
   case WM_INITDIALOG : {

      ThreadData *threadData = g_threads[ ::GetCurrentThreadId() ];
      _ASSERTE( NULL != threadData );

      int offset = g_threads.numof() * 15;

      /// Resize and move dialog window
      RECT rect = { 0, 0, threadData->img->width, threadData->img->height };

      /// define window size if client region size is given
      ::AdjustWindowRect(
         &rect,                              // pointer to client-rect structure
         GetWindowLong( hdlg, GWL_STYLE ),   // window styles
         FALSE                               // menu-present flag
      );
      /// window position depends on viewing mode.
      /// The dialog style has DS_CENTER bit, so the place of
      /// the dialog window is center if mode is modal
      /// It doesn't work if Borland is used
      ::SetWindowPos( hdlg, HWND_TOPMOST, 
         offset, offset, 
         rect.right - rect.left,
         rect.bottom - rect.top, 
         SWP_NOZORDER | (threadData->isModalView ? SWP_NOMOVE : 0 ));

      /// Set window caption
      ::SetWindowText( hdlg, threadData->text );
      /// signal that dialog has been initialized 
      ::SetEvent( g_threads.m_hevent );
      return TRUE;
   }

   case WM_COMMAND :
      switch( wParam ) {
      case IDOK : case IDCANCEL :
         EndDialog( hdlg, 0 );
         g_threads.close( ::GetCurrentThreadId() );
         return TRUE;
      }
      break;

   case WM_PAINT : {

      ThreadData *threadData = g_threads[ ::GetCurrentThreadId() ];
      _ASSERTE( NULL != threadData );

      PAINTSTRUCT ps;
      HDC hdc = ::BeginPaint( hdlg, &ps );
      if( !threadData->img || !threadData->img->imageData 
         || Good != ipDraw (hdc, threadData->img )
      )
         TextOut( hdc, 10, 10, g_imgErrStr, strlen( g_imgErrStr ));
      ::EndPaint( hdlg, &ps );
      return TRUE;
   }
   
   }
   return FALSE;
}

/* //////////////////////////////////////////////////////////////////////
// Name:       ViewerThread
// Purpose:    Thread Procedure
// Context:    IPLib viewer
// Returns:    If the DialogBoxIndirectParam fails, the return value is -1
// Parameters: ThreadData* - not used
//
//  Notes:     
*/

static DWORD WINAPI ViewerThread( ThreadData* ) {

   static char buf[1024] = {0};
   static DLGTEMPLATE* dlg = (DLGTEMPLATE*)buf;

   dlg->style = DS_CENTER | WS_POPUPWINDOW | WS_CAPTION | DS_MODALFRAME |  DS_SETFOREGROUND;
   dlg->dwExtendedStyle = 0;
   dlg->cdit = 0;
   dlg->x = dlg->y = 0;
   dlg->cx = 100;
   dlg->cy = 100;

   /// create a modal dialog box from a dialog box template in memory
   return ::DialogBoxIndirectParam( 
      ::GetModuleHandle(NULL),         // handle to application instance
      dlg,                            // identifies dialog box template
      NULL,                            // handle to owner window
      (DLGPROC)ViewProc,               // pointer to dialog box procedure
      (DWORD)0 );                 // initialization value
}

class bexception : public exception {
public:
   bexception( bool should_delete ) : m_should_delete( should_delete ) {}
   bool m_should_delete;
};


/* //////////////////////////////////////////////////////////////////////
// Name:       ipView
// Purpose:    to show IPLib images
// Context:    IPLib viewer
// Returns:    thread handle if there is not a modal view and all is ok
// Parameters: 
      const IplImage* img - IPLimage for viewing
      const char* caption - window caption
      bool isModalView - modal viewer if true, thread waits for a
         viewer closing. Thread doesn't wait. The following viewer
         can be started.
//
//  Notes:     Uses only one depth "8u" and one order "plane" for
               output image to draw. On the enter the following IPLib
               images can be: 8s|8u|16s|16u|32s + pixel|plane
*/

HANDLE ipView( const IplImage* img, const char* caption, bool isModalView ) {

   if( !img ) return 0;

   if( g_threads.isfull() ) {
      MessageBox( NULL, "Too many threads are beign executed. "
         "Please decrease number of viewers or use Modal mode.", 
         g_name, MB_OK | MB_ICONSTOP );
      return 0;
   }

   IplImage *clone = NULL, *img8u = NULL;
   HANDLE hThread = 0;

   /// Note we need not clone in the DEPTH_1U case

   if( NULL == (clone = iplCloneImage( img ))) return 0;
   /// Since this moment we should take care about clone pointer

   try {
      /// Create header for the image to be drawn
      img8u = iplCreateImageHeader(
         img->nChannels,
         img->alphaChannel,
         IPL_DEPTH_8U,                          /// fixed 8u
         (char*)img->colorModel, 
         (char*)img->channelSeq, 
         IPL_DATA_ORDER_PIXEL,                  /// fixed pixel
         img->origin,
         IPL_ALIGN_DWORD,
         img->width, img->height, 
         NULL, NULL, NULL, NULL );
      
      if( !img8u ) throw bexception( false );
   
      iplAllocateImage( img8u, 0,0 );
      if( !img8u->imageData ) throw bexception( true );
   
      /// convert to 8u,plane image for viewing
      if( IPL_DEPTH_1U == img->depth )
         iplBitonalToGray( (IplImage*)img, img8u, 0,255 );
      else if( IPL_DEPTH_8U != img->depth )
         iplScale( img, img8u );
      else
         iplCopy( (IplImage*)img, img8u );

      if( IPL_StsOk != iplGetErrStatus()) throw bexception( true );

      static ThreadData param;
      lstrcpy( param.text, NULL == caption ? "IPLib image" : caption );
      param.img = img8u;
      param.isModalView = isModalView;

      /// prepare for wainting
      ::ResetEvent( g_threads.m_hevent );

      hThread = CreateThread( 
         NULL,                                  // pointer to thread security attributes
         0,                                     // initial thread stack size, in bytes
         (LPTHREAD_START_ROUTINE) ViewerThread, // pointer to thread function
         (LPVOID)(0),                           // argument for new thread
         0,                                     // creation flags, runs immediately
         &param.id);                            // pointer to returned thread identifier
      
      if( !hThread ) throw bexception( true );

      /// set thread data for thread has been started
      if( false == g_threads.open( param )) {
         ::CloseHandle( hThread );
         throw bexception( true );
      }
      
      if( isModalView ) {
         /// wait for the thread will be ended
         ::WaitForSingleObject( hThread, INFINITE );
         ::CloseHandle( hThread );
         return 0;
      }
      else
         /// wait for dialog window will be initialized
         /// thread will be alive after that
         ::WaitForSingleObject ( g_threads.m_hevent, 10000 );
   }
   catch( bexception e ) {
      /// if success then img8u is deleted by the close procedure
      /// when dialog window will be closed by user
      /// else we should delete img8u
      if( e.m_should_delete ) iplDeallocate( img8u, IPL_IMAGE_HEADER );
   }
   /// 
   if( clone ) iplDeallocate( clone, IPL_IMAGE_HEADER );

   return hThread;
}


/* //////////////////////////////////////////////////////////////////////
// Name:       ipDraw
// Purpose:    draw 8u pixel IPLib images
// Context:    IPLib viewer
// Returns:    Error if NULL, Fail if wrong parameters
// Parameters: 
      HDC hdc - defines device context
      IplImage* - IPLib image to draw 
//
//  Notes: 
      supports
         1 channel, IPL_DEPTH_8U  with color mode "GRAY"
         3,4 channels, IPL_DEPTH_8U with color mode "RGB" or "RGBA"
*/

static ipStatus ipDraw( HDC hdc, IplImage* img ) {

   if( !img || !img->imageData ) return Error;
   
   bool isrgb =
      'R' == toupper( img->colorModel[0] ) && 
      'G' == toupper( img->colorModel[1] ) && 
      'B' == toupper( img->colorModel[2] );
   
   bool isgray = 'G' == toupper( img->colorModel[0] );

   if( !isgray && !isrgb ) return Fail;
   if( (1 == img->nChannels) != isgray ) return Fail;

   char buf[ sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*256 ];
   BITMAPINFOHEADER* dibhdr = (BITMAPINFOHEADER*)buf;
   COLORREF* rgb = (COLORREF*)( buf + sizeof(BITMAPINFOHEADER) );

   if( isgray ) for( int i = 0; i < 256; i++) rgb[i] = RGB( i,i,i );

   dibhdr->biSize = sizeof( BITMAPINFOHEADER );
   dibhdr->biWidth = img->width;
   dibhdr->biHeight = img->height;
   dibhdr->biPlanes = 1;
   dibhdr->biBitCount = (DWORD)(8 * img->nChannels);
   dibhdr->biCompression = BI_RGB;
   dibhdr->biSizeImage = img->imageSize;
   dibhdr->biXPelsPerMeter = 0;
   dibhdr->biYPelsPerMeter = 0;
   dibhdr->biClrUsed = 0;
   dibhdr->biClrImportant = 0; 

   ::SetDIBitsToDevice( hdc, 0,0,img->width,img->height, 
      0,0,0,img->height, img->imageData, (BITMAPINFO*)dibhdr, DIB_RGB_COLORS );

   return Good;
}
