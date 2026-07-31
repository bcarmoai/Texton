#ifndef __MACROS_INC__
#define __MACROS_INC__


#if defined( _MSC_VER ) && defined( _DEBUG )
   #include <crtdbg.h>

   #define SET_REPORT_OUT \
      _CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDOUT ); \
      _CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDOUT )
   
   #define TRACE0(msg) _RPT1( _CRT_WARN, "--- %s\n", msg )
   
   #define REPORT   _CrtDumpMemoryLeaks()

#else

   #define SET_REPORT_OUT
   #define TRACE0(msg)
   #define REPORT
   #define _ASSERTE

#endif

#endif
