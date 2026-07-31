// ArcFit.h: interface for the CArcFit class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARCFIT_H__D3C872EA_1E05_4C62_A1CA_D05C5A8E0DF2__INCLUDED_)
#define AFX_ARCFIT_H__D3C872EA_1E05_4C62_A1CA_D05C5A8E0DF2__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CSeedEntry
{
public:
	int c_x, c_y, c_r, s_x, s_y;
	BOOL set;
	int point_count;

};
/*
class CArcFit  
{
public:
	void PlotClaimedArcs(int* img_arcs, int* img_edges, int img_width, int img_height);
	void PlotClaimedSeeds(int* img_seeds, int img_width, int img_height);
	double CenterAngle(int cx, int cy, int px, int py);
	CArcFit();
	virtual ~CArcFit();
	BOOL Fit(int * img_data, int * img_arcs, int* img_arcbins, int img_width, int img_height, 
		int min_points, int max_points, float max_error, void (*progress)(char*));
	int* img_temparcs;
	CSeedEntry* seed_registry;
	CSeedEntry** seed_claims;

protected:
	int* current_contour;
	BOOL ExpandPointsCount(int* img_data, int img_width, int img_height, int nPoints, int sx, int sy);
	BOOL DrawArc(int* img_data, int* img_arcs, int img_width, int img_height,
		int circle_r, int circle_x, int circle_y, double* error);
	double ContourError(int* img_data, int img_with, int img_height, float circle_r, int circle_x, int circle_y);
	BOOL FitCircle(int* img_data, int img_width, int img_height, float& circle_r, int& circle_x, int& circle_y);
	BOOL ExpandEndpoints(int* img_data, int img_width, int img_height, int& e1x, int& e1y, int& e2x, int& e2y);
	BOOL NextSeed(int * img_data, int img_width, int img_heigth, int & sx, int & sy);
};
*/
#endif // !defined(AFX_ARCFIT_H__D3C872EA_1E05_4C62_A1CA_D05C5A8E0DF2__INCLUDED_)
