// HeartCFDCell.cpp: implementation of the CHeartCFDCell class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "texton.h"
#include "HeartCFDCell.h"
#include "math.h"

#include "VtkUtilities.h"
#include <vtkFloatArray.h>
#include <vtkStructuredGridWriter.h>
#include <vtkPointData.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define INTERP_POINTS 11
#define INTERP_DIST 0.1000f
#define INTERP_VOXELS TRUE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHeartCFDCell::CHeartCFDCell()
{
	if(INTERP_VOXELS==FALSE)
		interpPointData = new CHeartCFDVertex[INTERP_POINTS*INTERP_POINTS*INTERP_POINTS];
}

CHeartCFDCell::~CHeartCFDCell()
{
	if(INTERP_VOXELS==FALSE)
		delete [] interpPointData;
}

void CHeartCFDCell::CreateInterpolatedData(CHeartCFDVertex* vertex_array, int cfdHeight,
						float* u_voxels, float* v_voxels, float* w_voxels, int* vPointCount)

{
	// see log 7/5/03 for point geometry
	float e1x, e1y, e1z, e1Velx, e1Vely, e1Velz,
		e2x, e2y, e2z, e2Velx, e2Vely, e2Velz,
		e3x, e3y, e3z, e3Velx, e3Vely, e3Velz,
		e4x, e4y, e4z, e4Velx, e4Vely, e4Velz,
		e5x, e5y, e5z, e5Velx, e5Vely, e5Velz,
		e6x, e6y, e6z, e6Velx, e6Vely, e6Velz,
		px, py, pz, pvelx, pvely, pvelz;

	const float e1ax = vertex_array[vertex_points[1]].x, e1bx = vertex_array[vertex_points[4]].x-vertex_array[vertex_points[1]].x,
	e1ay = vertex_array[vertex_points[1]].y, e1by = vertex_array[vertex_points[4]].y-vertex_array[vertex_points[1]].y,
	e1az = vertex_array[vertex_points[1]].z, e1bz = vertex_array[vertex_points[4]].z-vertex_array[vertex_points[1]].z,
	e2ax = vertex_array[vertex_points[2]].x, e2bx = vertex_array[vertex_points[3]].x-vertex_array[vertex_points[2]].x,
	e2ay = vertex_array[vertex_points[2]].y, e2by = vertex_array[vertex_points[3]].y-vertex_array[vertex_points[2]].y,
	e2az = vertex_array[vertex_points[2]].z, e2bz = vertex_array[vertex_points[3]].z-vertex_array[vertex_points[2]].z,
	e3ax = vertex_array[vertex_points[6]].x, e3bx = vertex_array[vertex_points[7]].x-vertex_array[vertex_points[6]].x,
	e3ay = vertex_array[vertex_points[6]].y, e3by = vertex_array[vertex_points[7]].y-vertex_array[vertex_points[6]].y,
	e3az = vertex_array[vertex_points[6]].z, e3bz = vertex_array[vertex_points[7]].z-vertex_array[vertex_points[6]].z,
	e4ax = vertex_array[vertex_points[5]].x, e4bx = vertex_array[vertex_points[8]].x-vertex_array[vertex_points[5]].x,
	e4ay = vertex_array[vertex_points[5]].y, e4by = vertex_array[vertex_points[8]].y-vertex_array[vertex_points[5]].y,
	e4az = vertex_array[vertex_points[5]].z, e4bz = vertex_array[vertex_points[8]].z-vertex_array[vertex_points[5]].z;

	// interpolate along t1, t2, t3
	float t1, t2, t3;
	int node_no;
	int interpPointNo=0;

	for(t3=0.000f; t3<=1.001f; t3=t3+INTERP_DIST)
	{
		// update extreme positions
		e1x = e1ax + e1bx * t3;
		e1y = e1ay + e1by * t3;
		e1z = e1az + e1bz * t3;
		e2x = e2ax + e2bx * t3;
		e2y = e2ay + e2by * t3;
		e2z = e2az + e2bz * t3;
		e3x = e3ax + e3bx * t3;
		e3y = e3ay + e3by * t3;
		e3z = e3az + e3bz * t3;
		e4x = e4ax + e4bx * t3;
		e4y = e4ay + e4by * t3;
		e4z = e4az + e4bz * t3;

		// update extreme velocities
		e1Velx = t3 * vertex_array[vertex_points[4]].xVel + (1.00f-t3) * vertex_array[vertex_points[1]].xVel;
		e1Vely = t3 * vertex_array[vertex_points[4]].yVel + (1.00f-t3) * vertex_array[vertex_points[1]].yVel;
		e1Velz = t3 * vertex_array[vertex_points[4]].zVel + (1.00f-t3) * vertex_array[vertex_points[1]].zVel;
		e2Velx = t3 * vertex_array[vertex_points[3]].xVel + (1.00f-t3) * vertex_array[vertex_points[2]].xVel;
		e2Vely = t3 * vertex_array[vertex_points[3]].yVel + (1.00f-t3) * vertex_array[vertex_points[2]].yVel;
		e2Velz = t3 * vertex_array[vertex_points[3]].zVel + (1.00f-t3) * vertex_array[vertex_points[2]].zVel;
		e3Velx = t3 * vertex_array[vertex_points[7]].xVel + (1.00f-t3) * vertex_array[vertex_points[6]].xVel;
		e3Vely = t3 * vertex_array[vertex_points[7]].yVel + (1.00f-t3) * vertex_array[vertex_points[6]].yVel;
		e3Velz = t3 * vertex_array[vertex_points[7]].zVel + (1.00f-t3) * vertex_array[vertex_points[6]].zVel;
		e4Velx = t3 * vertex_array[vertex_points[8]].xVel + (1.00f-t3) * vertex_array[vertex_points[5]].xVel;
		e4Vely = t3 * vertex_array[vertex_points[8]].yVel + (1.00f-t3) * vertex_array[vertex_points[5]].yVel;
		e4Velz = t3 * vertex_array[vertex_points[8]].zVel + (1.00f-t3) * vertex_array[vertex_points[5]].zVel;

		for(t1=0.000f; t1<=1.001f; t1=t1+INTERP_DIST)
		{
			// update extreme positions
			e5x = e1x + (e2x - e1x) * t1;
			e5y = e1y + (e2y - e1y) * t1;
			e5z = e1z + (e2z - e1z) * t1;
			e6x = e4x + (e3x - e4x) * t1;
			e6y = e4y + (e3y - e4y) * t1;
			e6z = e4z + (e3z - e4z) * t1;

			// update extreme velocities
			e5Velx = t1 * e2Velx + (1.00f-t1) * e1Velx;
			e5Vely = t1 * e2Vely + (1.00f-t1) * e1Vely;
			e5Velz = t1 * e2Velz + (1.00f-t1) * e1Velz;
			e6Velx = t1 * e3Velx + (1.00f-t1) * e4Velx;
			e6Vely = t1 * e3Vely + (1.00f-t1) * e4Vely;
			e6Velz = t1 * e3Velz + (1.00f-t1) * e4Velz;

			for(t2=0.000f; t2<=1.001f; t2=t2+INTERP_DIST)
			{
				// update point positions
				px = e5x + (e6x - e5x) * t2;
				py = e5y + (e6y - e5y) * t2;
				pz = e5z + (e6z - e5z) * t2;

				// update point velocities
				pvelx = e6Velx * t2 + (1.00f - t2) * e5Velx;
				pvely = e6Vely * t2 + (1.00f - t2) * e5Vely;
				pvelz = e6Velz * t2 + (1.00f - t2) * e5Velz;

				node_no = TestNodeT(t1, t2, t3);

				// if we're on starting nodes, set extremes to nodes
				if(node_no==1 || node_no==2 || node_no==3 || node_no==4)
				{
					e5x = vertex_array[vertex_points[node_no]].x;
					e5y = vertex_array[vertex_points[node_no]].y;
					e5z = vertex_array[vertex_points[node_no]].z;

					e6x = vertex_array[vertex_points[node_no+4]].x;
					e6y = vertex_array[vertex_points[node_no+4]].y;
					e6z = vertex_array[vertex_points[node_no+4]].z;

					e5Velx = vertex_array[vertex_points[node_no]].xVel;
					e5Vely = vertex_array[vertex_points[node_no]].yVel;
					e5Velz = vertex_array[vertex_points[node_no]].zVel;

					e6Velx = vertex_array[vertex_points[node_no+4]].xVel;
					e6Vely = vertex_array[vertex_points[node_no+4]].yVel;
					e6Velz = vertex_array[vertex_points[node_no+4]].zVel;
			
				}

				// if we're in any node, add point with node coords and value and go to next t's
				if(node_no!=0)
				{
					AddPointData(interpPointNo, cfdHeight, vPointCount,
						vertex_array[vertex_points[node_no]].x,
						vertex_array[vertex_points[node_no]].y,
						vertex_array[vertex_points[node_no]].z,
						vertex_array[vertex_points[node_no]].xVel,
						vertex_array[vertex_points[node_no]].yVel,
						vertex_array[vertex_points[node_no]].zVel,
						u_voxels, v_voxels, w_voxels);
					interpPointNo++;
					continue; // statement affects logic in if() clause below
				}

				// we're not on a node so interpolate point value and add point
				AddPointData(interpPointNo, cfdHeight, vPointCount, px, py, pz, pvelx, pvely, pvelz,
					u_voxels, v_voxels, w_voxels);
				interpPointNo++;

			} // for t2
		} // for t1
	} // for t3

	if(interpPointNo != INTERP_POINTS*INTERP_POINTS*INTERP_POINTS)
	{
		CString sms;
		sms.Format("Warning: wrote %i interpolation points instead of expected %i.", interpPointNo, INTERP_POINTS*INTERP_POINTS*INTERP_POINTS);
		AfxMessageBox(sms);
	}

	interpPointCount = interpPointNo;

}

void CHeartCFDCell::SaveVtkCell()
{
	vtkFloatArray *fpoints = vtkFloatArray::New();
	vtkPoints* points = vtkPoints::New();
	vtkFloatArray *fspoints = vtkFloatArray::New();
	fspoints->SetNumberOfComponents(1);
	fpoints->SetNumberOfComponents(3);

	for(int i = 0; i<interpPointCount; i++)
	{
		// fspoints->InsertNextTuple1(sqrt(pow(xVel,2.0)+pow(yVel,2.0)+pow(zVel,2.0)));
		points->InsertNextPoint(interpPointData[i].x, interpPointData[i].y, interpPointData[i].z);
		fpoints->InsertNextTuple3(interpPointData[i].xVel, 
			interpPointData[i].yVel, interpPointData[i].zVel);
	}

	vtkStructuredGrid *sgrid = vtkStructuredGrid::New();
	sgrid->SetDimensions(2, 2, 2);
	sgrid->SetPoints(points);
	sgrid->GetPointData()->SetScalars(fspoints);
	sgrid->GetPointData()->SetVectors(fpoints);

	vtkStructuredGridWriter* swriter = vtkStructuredGridWriter::New();
	swriter->SetInput(sgrid);
	swriter->SetFileName("c:\\testINTERP.vtk");
	swriter->Write();

	sgrid->Delete();
	swriter->Delete();
	fpoints->Delete();
	fspoints->Delete();
	points->Delete();

}

BOOL CHeartCFDCell::TestNodeT(float t1, float t2, float t3)
{
	if(t1==0.000f && t2==0.000f && t3==0.000f)
		return 1;
	else
		if(t1==1.000f && t2==0.000f && t3==0.000f)
			return 2;
		else
			if(t1==0.000f && t2==0.000f && t3==1.000f)
				return 4;
			else
				if(t1==1.000f && t2==0.000f && t3==1.000f)
					return 3;
				else
					if(t1==0.000f && t2==1.000f && t3==0.000f)
						return 5;
					else
						if(t1==1.000f && t2==1.000f && t3==0.000f)
							return 6;
						else
							if(t1==0.000f && t2==1.000f && t3==1.000f)
								return 8;
							else
								if(t1==1.000f && t2==1.000f && t3==1.000f)
									return 7;
								else
									return 0;

}

float CHeartCFDCell::GetCellPoint(float x, float y, float z, float &velx, float &vely, float &velz)
{
	float min_dist, cur_dist;
	int min_point;

	// get closest point to given one
	min_dist = (float)sqrt( pow(x-interpPointData[0].x, 2.0)+ 
							pow(y-interpPointData[0].y, 2.0)+ 
							pow(z-interpPointData[0].z, 2.0) );
	min_point = 0;
	for(int i=1; i<interpPointCount; i++)
	{
		cur_dist = (float)sqrt( pow(x-interpPointData[i].x, 2.0)+
				  				pow(y-interpPointData[i].y, 2.0)+ 
								pow(z-interpPointData[i].z, 2.0) );

		if(cur_dist<min_dist)
		{
			min_dist = cur_dist;
			min_point = i;
		}
	}

	// return closest point velocity
	velx = interpPointData[min_point].xVel;
	vely = interpPointData[min_point].yVel;
	velz = interpPointData[min_point].zVel;

	return min_dist;

}

float CHeartCFDCell::GetTotalPointDistance(CHeartCFDVertex* vertex_array, float x, float y, float z)
{
	// get total distance between given point and all vertices in the cell
	// used for point in cell test

	return sqrt( pow(x-vertex_array[vertex_points[1]].x, 2.0)+ 
				 pow(y-vertex_array[vertex_points[1]].y, 2.0)+ 
				 pow(z-vertex_array[vertex_points[1]].z, 2.0) ) +
		   sqrt( pow(x-vertex_array[vertex_points[2]].x, 2.0)+ 
				 pow(y-vertex_array[vertex_points[2]].y, 2.0)+ 
				 pow(z-vertex_array[vertex_points[2]].z, 2.0) ) +
		   sqrt( pow(x-vertex_array[vertex_points[3]].x, 2.0)+ 
				 pow(y-vertex_array[vertex_points[3]].y, 2.0)+ 
				 pow(z-vertex_array[vertex_points[3]].z, 2.0) ) +
		   sqrt( pow(x-vertex_array[vertex_points[4]].x, 2.0)+ 
				 pow(y-vertex_array[vertex_points[4]].y, 2.0)+ 
				 pow(z-vertex_array[vertex_points[4]].z, 2.0) ) +
		   sqrt( pow(x-vertex_array[vertex_points[5]].x, 2.0)+ 
				 pow(y-vertex_array[vertex_points[5]].y, 2.0)+ 
				 pow(z-vertex_array[vertex_points[5]].z, 2.0) ) +
		   sqrt( pow(x-vertex_array[vertex_points[6]].x, 2.0)+ 
				 pow(y-vertex_array[vertex_points[6]].y, 2.0)+ 
				 pow(z-vertex_array[vertex_points[6]].z, 2.0) ) +
		   sqrt( pow(x-vertex_array[vertex_points[7]].x, 2.0)+ 
				 pow(y-vertex_array[vertex_points[7]].y, 2.0)+ 
				 pow(z-vertex_array[vertex_points[7]].z, 2.0) ) +
		   sqrt( pow(x-vertex_array[vertex_points[8]].x, 2.0)+ 
				 pow(y-vertex_array[vertex_points[8]].y, 2.0)+ 
				 pow(z-vertex_array[vertex_points[8]].z, 2.0) ) / 8.00f;


}

int CHeartCFDCell::FloatToInt(float fnum)
{
	double dmy;

	if(fnum>0)
		if(modf(fnum,&dmy)==0.5)
			return ((int)fnum)%2 ? (int)ceil(fnum) : (int)floor(fnum);
		else	
			return (int)(modf(fnum,&dmy)>0.5? (int)ceil(fnum): (int)floor(fnum));
	else
		if(modf(fnum,&dmy)==0.5)
			return ((int)fnum)%2 ? (int)floor(fnum) : (int)ceil(fnum);
		else	
			return (int)(modf(fnum,&dmy)>0.5? (int)floor(fnum): (int)ceil(fnum));
}

void CHeartCFDCell::AddPointData(int point_no, int cfdHeight, int* vPointCount, 
		float px, float py, float pz, float pvelx, float pvely, float pvelz, 
		float *u_voxels, float *v_voxels, float *w_voxels)
{
	if(INTERP_VOXELS)
	{
		// convert positions
		static const double xAspect = 2.6116305899E-01; 
		static const double zAspect = 7.7501522209E-01;
		int img_width  = ceil(cfdHeight*xAspect);
		int img_height = cfdHeight;
		int img_slices = ceil(cfdHeight*zAspect);

		float trf_mx, trf_my, trf_mz, trf_bx, trf_by, trf_bz;
		trf_mx = (float)(img_width-1)/(6.2569800540E-02);
		trf_my = (float)(img_height-1)/(2.3958135880E-01);
		trf_mz = (float)(img_slices-1)/(1.8567920000E-01);
		trf_bx = (-1.0f)*trf_mx*-6.2577300000E-02;
		trf_by = (-1.0f)*trf_my*2.7641200000E-05;
		trf_bz = (-1.0f)*trf_mz*2.1516800000E-02;

		float ftest1 = 2.76e-5;
		float ftest2 = 2.40e-1;

		// transform position to voxel coordinates
		int xiPos = FloatToInt( trf_mx * px + trf_bx);
		int yiPos = FloatToInt( trf_my * py + trf_by);
		int ziPos = FloatToInt( trf_mz * pz + trf_bz);

		// set values
		u_voxels[ziPos*img_width*img_height+yiPos*img_width+xiPos] += pvelx;
		v_voxels[ziPos*img_width*img_height+yiPos*img_width+xiPos] += pvely;
		w_voxels[ziPos*img_width*img_height+yiPos*img_width+xiPos] += pvelz;

		// increment point count
		vPointCount[ziPos*img_width*img_height+yiPos*img_width+xiPos]++;

	}
	else
	{
		interpPointData[point_no].x=px;
		interpPointData[point_no].y=py;
		interpPointData[point_no].z=pz;
		interpPointData[point_no].xVel=pvelx;
		interpPointData[point_no].yVel=pvely;
		interpPointData[point_no].zVel=pvelz;
	}

}
