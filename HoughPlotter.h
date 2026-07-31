// HoughPlotter.h: interface for the CHoughPlotter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HOUGHPLOTTER_H__2F4CFF3D_7BA1_497F_84B8_82DA9271AA94__INCLUDED_)
#define AFX_HOUGHPLOTTER_H__2F4CFF3D_7BA1_497F_84B8_82DA9271AA94__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CHoughPlotter  
{
public:
	BOOL PlotHough(int* img_data, int* img_hough, int img_width, int img_height, float min_diam, float max_diam, int circles);
	CHoughPlotter();
	virtual ~CHoughPlotter();

};

#endif // !defined(AFX_HOUGHPLOTTER_H__2F4CFF3D_7BA1_497F_84B8_82DA9271AA94__INCLUDED_)
