// Data3DMRI.h: interface for the CData3DMRI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATA3DMRI_H__2CF9C712_DD98_4B58_B399_CCAAD260FF09__INCLUDED_)
#define AFX_DATA3DMRI_H__2CF9C712_DD98_4B58_B399_CCAAD260FF09__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "DataMRI.h"
#include "HeartCFDCell.h"

class CData3DMRI  
{
public:
	void SaveDataMatlab(CString filename);
	void LoadDataHeart7D(CString filename, int timeframe);
	void LoadDataHeartCFDTexton(CString filename);
	void SaveDataHeartCFD(CString filename);
	void SaveDataSur(int idx, int zb_slice);
	void LoadDataHeartCFD(CString filename, int cfdHeight);
	void FlipW();
	void FlipV();
	void FlipU();
	void Zoom(CPoint upperLeft, CPoint lowerRight, int startSlice, int endSlice, float scaling);
	void LoadDataFrame(CString filename);
	int FloatToInt(float fnum);
	void LoadDataCFD(CString filename);
	void SetData(int width, int height, int slices, short * amri, short * au, short * av, short * aw);
	void Reset();
	CData3DMRI();
	virtual ~CData3DMRI();
	short* mr;
	short* u;
	short* v;
	short* w;
	int img_slices;
	int img_height;
	int img_width;
	CDataMRI* Get2dSlice(int slice);
	CDataMRI* Get2dYSlice(int y_coord);
	CDataMRI* Get2dXSlice(int x_coord);

protected:
	CHeartCFDCell* DataHeartCFDGetClosestCell(CHeartCFDCell* cell_array, CHeartCFDVertex* vertex_array,
		float x, float y, float z);
	void DataHeartCFDConvertCoordinates(int x, int y, int z, int cfdHeight, float& fx, float& fy, float& fz);
	void DataHeartCFDInterpolation(CString filename, int cfdHeight);
	void DataHeartCFDAveraging(CString filename, int cfdHeight);
	void SavePointCount(int* point_count, int pcount);
	void InterpolateVoxel(short* src, short* dst, int dx, int dy, int dz, 
		int sROIx, int sROIy, int sROIz, float scale,
		int destWidth, int destHeight, BOOL twoD);
};

#endif // !defined(AFX_DATA3DMRI_H__2CF9C712_DD98_4B58_B399_CCAAD260FF09__INCLUDED_)
