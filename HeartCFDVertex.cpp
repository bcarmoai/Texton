// HeartCFDVertex.cpp: implementation of the CHeartCFDVertex class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "texton.h"
#include "HeartCFDVertex.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHeartCFDVertex::CHeartCFDVertex()
{
	x = y = z = xVel = yVel = zVel = 0.0f;
}

CHeartCFDVertex::~CHeartCFDVertex()
{

}
