// HeartCFDVertex.h: interface for the CHeartCFDVertex class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HEARTCFDVERTEX_H__77A31D5F_5D04_4B0C_BFEE_C6443503A6FC__INCLUDED_)
#define AFX_HEARTCFDVERTEX_H__77A31D5F_5D04_4B0C_BFEE_C6443503A6FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CHeartCFDVertex  
{
public:
	float z;
	float y;
	float x;
	float xVel;
	float yVel;
	float zVel;
	CHeartCFDVertex();
	virtual ~CHeartCFDVertex();

};

#endif // !defined(AFX_HEARTCFDVERTEX_H__77A31D5F_5D04_4B0C_BFEE_C6443503A6FC__INCLUDED_)
