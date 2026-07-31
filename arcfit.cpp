// ArcFit.cpp: implementation of the CArcFit class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Texton.h"
#include "ArcFit.h"
//#include "OpencvContour.h"
#include <math.h>
//#include <cv.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/*
CArcFit::CArcFit()
{
	current_contour = NULL;
	img_temparcs = NULL;
	seed_registry = NULL;
	seed_claims = NULL;
}

CArcFit::~CArcFit()
{
	if(img_temparcs!=NULL)
	{
		delete img_temparcs;
		img_temparcs = NULL;
	}

	if(current_contour!=NULL)
	{
		delete current_contour;
		current_contour = NULL;
	}

}

BOOL CArcFit::Fit(int * img_data, int * img_arcs, int* img_arcbins, int img_width, int img_height, 
				  int min_points, int max_points, float max_error, void (*progress)(char*))
{
	int sx, sy, e1x, e1y, e2x, e2y, 
		prev_e1x, prev_e1y, prev_e2x, prev_e2y, circle_x, circle_y;
	int prev_cX, prev_cY;
	float circle_r, prev_cR;
	double error;

	int x, y;

	// allocate seed registry map
	if(seed_registry!= NULL)
	{
		delete seed_registry;
		seed_registry = NULL;
	}
	seed_registry = new CSeedEntry[img_height*img_width];
	for(x=0; x<img_width; x++)  
		for(y=0; y<img_height; y++)
			seed_registry[img_width*y+x].set = FALSE;

	// allocate seed claims
	if(seed_claims!= NULL)
	{
		delete seed_claims;
		seed_claims = NULL;
	}
	seed_claims = new CSeedEntry*[img_height*img_width];
	for(x=0; x<img_width; x++)  
		for(y=0; y<img_height; y++)
			seed_claims[img_width*y+x] = NULL;

	// allocate temporary contour image
	if(current_contour != NULL)
	{
		delete current_contour;
		current_contour = NULL;
	}
	current_contour = new int[img_height*img_width];

	// allocate temporary arcs image
	if(img_temparcs != NULL)
	{
		delete img_temparcs;
		img_temparcs = NULL;
	}
	img_temparcs = new int[img_height*img_width];
	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
			img_temparcs[img_width*y+x]=0;

	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
			img_arcs[img_width*y+x]=0;

	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
			img_arcbins[img_width*y+x]=0;

	sx = sy = 0;

	int npts;  // counter for number of points

	// pick seed point
	char progress_text[100];
	while(NextSeed(img_data, img_width, img_height, sx, sy))
	{
		sprintf(progress_text, "seed at (%i, %i)", sx, sy);
		progress(progress_text);

		for(x=0; x<img_width; x++)  
			for(y=0; y<img_height; y++)
				current_contour[img_width*y+x]=0;

		// expand endpoints from seed
		e1x = e2x = sx;
		e1y = e2y = sy;

		for(npts = 1; npts <= min_points; )
		{
			if(!ExpandEndpoints(img_data, img_width, img_height, e1x, e1y, e2x, e2y))
				break;
			else
				npts=npts+2;
		}

		if(npts < min_points)
			continue;
		
		for(x=0; x<img_width; x++)
			for(y=0; y<img_height; y++)
				img_temparcs[img_width*y+x]=0;
			
		// fit circle to points
		if(!FitCircle(img_data, img_width, img_height, circle_r, circle_x, circle_y))
		{	
			AfxMessageBox("Error fitting circle in outside Fit() loop.");
			continue;
		}

		// get error
		error = ContourError(img_data, img_width, img_height, circle_r, circle_x, circle_y);

		if(error > max_error)
			continue;   // not a good seed, so go to the next one

		prev_e1x = e1x;
		prev_e1y = e1y;
		prev_e2x = e2x;
		prev_e2y = e2y;

		// loop until max_error reached
		while(ExpandEndpoints(img_data, img_width, img_height, e1x, e1y, e2x, e2y))
		{
			npts=npts+2;

			for(x=0; x<img_width; x++)
				for(y=0; y<img_height; y++)
					img_temparcs[img_width*y+x]=0;

			if(!FitCircle(img_data, img_width, img_height, circle_r, circle_x, circle_y))
			{	
				AfxMessageBox("Error fitting circle in inside Fit() loop.");
				break;
			}
			
			error = ContourError(img_data, img_width, img_height, circle_r, circle_x, circle_y);

			if(error > max_error)
				break;
			else
			{
				prev_e1x = e1x;
				prev_e1y = e1y;
				prev_e2x = e2x;
				prev_e2y = e2y;
				prev_cR = circle_r;
				prev_cX = circle_x;
				prev_cY = circle_y;
			}

			if(npts > max_points)
				break;

		}

		if(error>max_error)
		{
			seed_registry[img_width*sy+sx].set = TRUE;
			seed_registry[img_width*sy+sx].c_x = prev_cX;
			seed_registry[img_width*sy+sx].c_y = prev_cY;
			seed_registry[img_width*sy+sx].c_r = (int)prev_cR;
			seed_registry[img_width*sy+sx].point_count = npts;
			seed_registry[img_width*sy+sx].s_x = sx;
			seed_registry[img_width*sy+sx].s_y = sy;

		}
		else
		{
			// add current data into registry
			seed_registry[img_width*sy+sx].set = TRUE;
			seed_registry[img_width*sy+sx].c_x = circle_x;
			seed_registry[img_width*sy+sx].c_y = circle_y;
			seed_registry[img_width*sy+sx].c_r = (int)circle_r;
			seed_registry[img_width*sy+sx].point_count = npts;
			seed_registry[img_width*sy+sx].s_x = sx;
			seed_registry[img_width*sy+sx].s_y = sy;

		}

		// plot arc pixels in arcs image
		for(x=0; x<img_width; x++)
			for(y=0; y<img_height; y++)
				if(img_temparcs[img_width*y+x]==1)
					img_arcs[img_width*y+x]++;

		// add current seed to seed claims map
		for(x=0; x<img_width; x++)
			for(y=0; y<img_height; y++)
				if(current_contour[img_width*y+x]==1)
					if(seed_claims[img_width*y+x]!=NULL)
					{
						if(seed_claims[img_width*y+x]->point_count<npts)
							seed_claims[img_width*y+x] = &seed_registry[img_width*sy+sx];
					}
					else
						seed_claims[img_width*y+x] = &seed_registry[img_width*sy+sx];

	}

	// plot each seed into bins map
	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
			if(seed_registry[img_width*y+x].set==TRUE)
			{
				if(seed_registry[img_width*y+x].c_y < img_height &&
					seed_registry[img_width*y+x].c_x < img_width &&
					seed_registry[img_width*y+x].c_y >= 0 &&
					seed_registry[img_width*y+x].c_x >= 0)
				{
					img_arcbins[img_width*seed_registry[img_width*y+x].c_y
					+seed_registry[img_width*y+x].c_x]++;
				}
			}

	sprintf(progress_text, "Done.");
	progress(progress_text);

	delete current_contour;
	current_contour = NULL;

	delete img_temparcs;
	img_temparcs = NULL;

	return TRUE;

}

BOOL CArcFit::NextSeed(int * img_data, int img_width, int img_heigth, int & sx, int & sy)
{
	char sx255[100];
	sprintf(sx255, "NextSeed called with (%i, %i)", sx, sy);
	// AfxMessageBox(sx255);

	int x, y;
	BOOL bFirst = (sy!=0);

	for(x=sx; x<img_width-1; x++)
		for(y=0; y<img_heigth-1; y++)
		{
			if(x==sx && bFirst==TRUE)
			{	
				bFirst=FALSE;
				y=sy+1;
				if(!(y<img_heigth-1))
					continue;
			}

			if(img_data[img_width*y+x]>0)
			{
				sx = x;
				sy = y;
				return TRUE;
			}
		}

	return FALSE;
}

BOOL CArcFit::ExpandEndpoints(int * img_data, int img_width, int img_height, 
							  int& e1x, int& e1y, int& e2x, int& e2y)
{

	int prev_e1x, prev_e1y, prev_e2x, prev_e2y;
	prev_e1x = e1x;
	prev_e1y = e1y;
	prev_e2x = e2x;
	prev_e2y = e2y;
	
	// if we're being called with a seed point, mark it first
	if(e1x == e2x && e1y == e2y)
		current_contour[img_width*e1y+e1x]=1;

	// first endpoint
	// look for 90 deg. neighbour
	if(e1y-1>=0)
		if(img_data[img_width*(e1y-1)+e1x] > 0 && current_contour[img_width*(e1y-1)+e1x] != 1)
		{
			e1y--;
			current_contour[img_width*e1y+e1x] = 1;
			goto SecondPoint;
		}

	// look for 45 deg. neighbour
	if(e1y-1>=0 && e1x+1<img_width)
		if(img_data[img_width*(e1y-1)+(e1x+1)] > 0 && current_contour[img_width*(e1y-1)+(e1x+1)] != 1)
		{
			e1y--;
			e1x++;
			current_contour[img_width*e1y+e1x] = 1;
			goto SecondPoint;
		}

	// look for 0 deg. neighbour
	if(e1x+1<img_width)
		if(img_data[img_width*e1y+e1x+1] > 0 && current_contour[img_width*e1y+e1x+1] != 1)
		{
			e1x++;
			current_contour[img_width*e1y+e1x] = 1;
			goto SecondPoint;
		}

	// look for -45 deg. neighbour
	if(e1y+1<img_height && e1x+1<img_width)
		if(img_data[img_width*(e1y+1)+e1x+1] > 0 && current_contour[img_width*(e1y+1)+e1x+1] != 1)
		{
			e1y++;
			e1x++;
			current_contour[img_width*e1y+e1x] = 1;
			goto SecondPoint;
		}

	// look for -90 deg. neighbour
	if(e1y+1<img_height)
		if(img_data[img_width*(e1y+1)+e1x] > 0 && current_contour[img_width*(e1y+1)+e1x] != 1)
		{
			e1y++;
			current_contour[img_width*e1y+e1x] = 1;
			goto SecondPoint;
		}

	// look for -135 deg. neighbour
	if(e1y+1<img_height && e1x-1>=0)
		if(img_data[img_width*(e1y+1)+e1x-1] > 0 && current_contour[img_width*(e1y+1)+e1x-1] != 1)
		{
			e1y++;
			e1x--;
			current_contour[img_width*e1y+e1x] = 1;
			goto SecondPoint;
		}

	// look for 180 deg. neighbour
	if(e1x-1>=0)
		if(img_data[img_width*e1y+e1x-1] > 0 && current_contour[img_width*e1y+e1x-1] != 1)
		{
			e1x--;
			current_contour[img_width*e1y+e1x] = 1;
			goto SecondPoint;
		}

	// look for 135 deg. neighbour
	if(e1y-1>=0 && e1x-1>=0)
		if(img_data[img_width*(e1y-1)+e1x-1] > 0 && current_contour[img_width*(e1y-1)+e1x-1] != 1)
		{
			e1y--;
			e1x--;
			current_contour[img_width*e1y+e1x] = 1;
			goto SecondPoint;
		}

SecondPoint:
	// second endpoint
	// look for 90 deg. neighbour
	if(e2y-1>=0)
		if(img_data[img_width*(e2y-1)+e2x] > 0 && current_contour[img_width*(e2y-1)+e2x] != 1)
		{
			e2y--;
			goto CheckPoints;
		}

	// look for 45 deg. neighbour
	if(e2y-1>=0 && e2x+1<img_width)
		if(img_data[img_width*(e2y-1)+(e2x+1)] > 0 && current_contour[img_width*(e2y-1)+(e2x+1)] != 1)
		{
			e2y--;
			e2x++;
			goto CheckPoints;
		}

	// look for 0 deg. neighbour
	if(e2x+1<img_width)
		if(img_data[img_width*e2y+e2x+1] > 0 && current_contour[img_width*e2y+e2x+1] != 1)
		{
			e2x++;
			goto CheckPoints;
		}

	// look for -45 deg. neighbour
	if(e2y+1<img_height && e2x+1<img_width)
		if(img_data[img_width*(e2y+1)+e2x+1] > 0 && current_contour[img_width*(e2y+1)+e2x+1] != 1)
		{
			e2y++;
			e2x++;
			goto CheckPoints;
		}

	// look for -90 deg. neighbour
	if(e2y+1<img_height)
		if(img_data[img_width*(e2y+1)+e2x] > 0 && current_contour[img_width*(e2y+1)+e2x] != 1)
		{
			e2y++;
			goto CheckPoints;
		}

	// look for -135 deg. neighbour
	if(e2y+1<img_height && e2x-1>=0)
		if(img_data[img_width*(e2y+1)+e2x-1] > 0 && current_contour[img_width*(e2y+1)+e2x-1] != 1)
		{
			e2y++;
			e2x--;
			goto CheckPoints;
		}

	// look for 180 deg. neighbour
	if(e2x-1>=0)
		if(img_data[img_width*e2y+e2x-1] > 0 && current_contour[img_width*e2y+e2x-1] != 1)
		{
			e2x--;
			goto CheckPoints;
		}

	// look for 135 deg. neighbour
	if(e2y-1>=0 && e2x-1>=0)
		if(img_data[img_width*(e2y-1)+e2x-1] > 0 && current_contour[img_width*(e2y-1)+e2x-1] != 1)
		{
			e2y--;
			e2x--;
			goto CheckPoints;
		}

CheckPoints:
	// if one of the endpoints didn't change, reject the whole thing
	if((prev_e1x == e1x && prev_e1y == e1y) || (prev_e2x == e2x && prev_e2y == e2y))
	{
		e1x = prev_e1x;
		e1y = prev_e1y;
		current_contour[img_width*e1y+e1x] = 0;
		e2x = prev_e2x;
		e2y = prev_e2y;
		return FALSE;
	}
	else
	{
		current_contour[img_width*e1y+e1x] = 1;
		current_contour[img_width*e2y+e2x] = 1;
		return TRUE;
	}
}

BOOL CArcFit::FitCircle(int * img_data, int img_width, int img_height, 
						float& circle_r, int& circle_x, int& circle_y)
{
	// collect points
	CArray<CPoint, CPoint> point_array;
	int x, y;
	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
			if(current_contour[img_width*y+x] == 1)
				point_array.Add(CPoint(x, y));

	// fit circle to collected points
	COpencvContour contour;
	int icircle_r;
	contour.fit_circle(point_array, icircle_r, circle_x, circle_y);
	circle_r = (float)icircle_r;

	return TRUE;
}

double CArcFit::ContourError(int* img_data, int img_width, int img_height, 
							float circle_r, int circle_x, int circle_y)
{
	double error;

	DrawArc(img_data, img_temparcs, img_width, img_height, (int)circle_r, circle_x, circle_y, &error);

	return error;
}

double sgn(double x)
{
	return x>=0?1:-1;
}

double pointdist(double x1, double y1, double x2, double y2)
{
	return sqrt(pow(x2-x1,2)+pow(y2-y1,2));
}

BOOL CArcFit::DrawArc(int* img_data, int * img_arcs, int img_width, int img_height, 
					  int circle_r, int circle_x, int circle_y, double* error)
{

	int x, y;
	double zx, zy, ix, iy;
	double dmy;	
	int iix, iiy;
	int point_count;

	point_count = 0;
	*error = 0;

	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
			if(current_contour[img_width*y+x] == 1)
			{
				zx = x-circle_x;
				zy = y-circle_y;

				// intersection point between circle and line between centre and contour point
				ix = sgn(-1.0*zy)*zx*((double)circle_r)/sqrt(pow(zx,2)+pow(zy,2));
				iy = fabs(zy)*((double)circle_r)/sqrt(pow(zx,2)+pow(zy,2));

				// there are two intersections so choose closest
				if(pointdist(ix, iy, zx, zy)>pointdist(-ix, -iy, zx, zy))
				{
					ix = -ix;
					iy = -iy;
				}

				iix = (int)(modf(ix,&dmy)>0.5?ceil(ix):floor(ix));
				iiy = (int)(modf(iy,&dmy)>0.5?ceil(iy):floor(iy));

				// draw intersection point
				if(iix+circle_x<img_width && iix+circle_x>=0 
					&& iiy+circle_y<img_height && iiy+circle_y>=0)
					img_arcs[img_width*(iiy+circle_y)+(iix+circle_x)] = 1;

				// calculate error
				*error = *error + pointdist(iix, iiy, zx, zy);
				point_count++;

			}

	*error = *error/(double)point_count;
	
	return TRUE;
}

double CArcFit::CenterAngle(int cx, int cy, int px, int py)
{
	double angle;
	double pi = 3.141592654;

	// 90-deg
	if(px-cx==0 && py-cy>0)
		return 90;

	// 270-deg
	if(px-cx==0 && py-cy<0)
		return 270;

	// 0-deg
	if(py-cy==0 && px-cx>0)
		return 0;

	// 180-deg
	if(py-cy==0 && px-cx<0)
		return 180;

	angle = atan(((double)(py-cy))/((double)(px-cx)));

	// first quadrant
	if(px-cx>0 && py-cy>0)
		return angle*360.0/(2.0*pi);

	// second quadrant
	if(px-cx<0 && py-cy>0)
		return 180.0+(angle*360.0/(2.0*pi));

	// third quadrant
	if(px-cx<0 && py-cy<0)
		return 180.0+(angle*360.0/(2.0*pi));

	// fourth quadrant
	if(px-cx>0 && py-cy<0)
		return 360.0+(angle*360.0/(2.0*pi));

	return 0;


}

void CArcFit::PlotClaimedSeeds(int* img_seeds, int img_width, int img_height)
{
	int x, y;

	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
			img_seeds[img_width*y+x] = 0;

	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
			if(seed_claims[img_width*y+x]!=NULL)
				if(seed_claims[img_width*y+x]->c_x < img_width &&
				   seed_claims[img_width*y+x]->c_y < img_height &&
				   seed_claims[img_width*y+x]->c_x >= 0 &&
				   seed_claims[img_width*y+x]->c_y >= 0 )
				   img_seeds[seed_claims[img_width*y+x]->c_y*img_width+seed_claims[img_width*y+x]->c_x]++;

}


BOOL SeedPrevExp(CSeedEntry* toSearch, CSeedEntry** searched, int search_count)
{
	int i;

	for(i=0; i<search_count; i++)
	{
		if(searched[i]==toSearch)
			return TRUE;
	}

	return FALSE;

}

BOOL CArcFit::ExpandPointsCount(int* img_data, int img_width, int img_height, int nPoints, int sx, int sy)
{
	int count, e1x, e1y, e2x, e2y;
	count = 0;
	e1x=e2x=sx;
	e1y=e2y=sy;

	for(count = 1; count < nPoints; )
	{
		if(!ExpandEndpoints(img_data, img_width, img_height, e1x, e1y, e2x, e2y))
			break;
		else
			count=count+2;
	}
	
	if(count < nPoints)
		return FALSE;
	else
		return TRUE;
}

void CArcFit::PlotClaimedArcs(int * img_arcs, int* img_edges, int img_width, int img_height)
{
	int x, y, x2, y2;
	CSeedEntry** drawn_seeds;
	int drawn_count = 0;

	drawn_seeds = new CSeedEntry*[img_width*img_height];

	if(current_contour != NULL)
	{
		delete current_contour;
		current_contour = NULL;
	}
	current_contour = new int[img_height*img_width];

	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
		{
			img_arcs[img_width*y+x] = 0;
			current_contour[img_width*y+x] = 0;
			drawn_seeds[img_width*y+x]=NULL;
		}

	double error;

	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
			if(seed_claims[img_width*y+x]!=NULL)
			{
				for(x2=0; x2<img_width; x2++)
					for(y2=0; y2<img_height; y2++)
						current_contour[img_width*y2+x2] = 0;

				// expand seed if we haven't handled it before
				if(!SeedPrevExp(seed_claims[img_width*y+x], drawn_seeds, drawn_count))
					if(!ExpandPointsCount(img_edges, img_width, img_height, seed_claims[img_width*y+x]->point_count,
						seed_claims[img_width*y+x]->s_x, seed_claims[img_width*y+x]->s_y))
						;//AfxMessageBox("Error expanding points in PlotClaimedArcs()");

				// draw arc on expanded seed
				DrawArc(img_edges, img_arcs, img_width, img_height, 
					seed_claims[img_width*y+x]->c_r, seed_claims[img_width*y+x]->c_x, 
					seed_claims[img_width*y+x]->c_y, &error);

			}

	delete current_contour;
	current_contour = NULL;

}
*/