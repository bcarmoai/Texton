// IntArray.h: interface for the CIntArray class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INTARRAY_H__B07250B0_818D_4BC8_8164_13A884A307C8__INCLUDED_)
#define AFX_INTARRAY_H__B07250B0_818D_4BC8_8164_13A884A307C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Int.h"

class CIntArray  
{
public:
	CIntArray();
	virtual ~CIntArray();
	CArray<CInt, CInt> arrayData;

};

#endif // !defined(AFX_INTARRAY_H__B07250B0_818D_4BC8_8164_13A884A307C8__INCLUDED_)
