// PointFloatArray.h: interface for the CPointFloatArray class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POINTFLOATARRAY_H__8985A8DA_433B_45BC_89DB_64854D152B16__INCLUDED_)
#define AFX_POINTFLOATARRAY_H__8985A8DA_433B_45BC_89DB_64854D152B16__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PointFloat3D.h"

class CPointFloatArray  
{
public:
	CPointFloatArray();
	virtual ~CPointFloatArray();
	CArray<CPointFloat3D, CPointFloat3D> pointArray;

};

#endif // !defined(AFX_POINTFLOATARRAY_H__8985A8DA_433B_45BC_89DB_64854D152B16__INCLUDED_)
