// Plane.cpp: implementation of the CPlane class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include "Plane.h"
#include <math.h>
#include "NumrecEigenvalues.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifndef ANSI
#define ANSI
#endif
#include "nrutil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const int pauline_version = FALSE;

CPlane::CPlane(CVector3D &normal, double dist)
{
	// represent the plane in Hessian normal form;
	double norm;

	if ( (norm = normal.Norm()) > 0) {
		vecNormal = normal / norm;
		distFromOri = dist / norm;
	}

	matrix_a = NULL;
	bMatrixaValid = FALSE;
	m_b3D = TRUE; 
}

CPlane::CPlane()
{
	m_b3D = TRUE; 
	bMatrixaValid=FALSE;
	matrix_a = NULL;
}

CPlane::~CPlane()
{
	if(bMatrixaValid==TRUE && matrix_a!= NULL)
	{
		if(m_b3D == FALSE)
			free_matrix(matrix_a, 1, 2, 1, 2);
		else
			free_matrix(matrix_a, 1, 3, 1, 3);
	}

	bMatrixaValid = FALSE;

}


////////////////////////////////////////////////////////////////////////

double CPlane::DistFromPt(CPoint3D pt, CVector3D vc)
{
	double result;

	CVector3D fitVel = GetFitVel(pt.x, pt.y, pt.z);

	if(pauline_version == FALSE)
		result = sqrt(pow(vc.z-fitVel.z,2)+pow(vc.y-fitVel.y,2)+pow(vc.x-fitVel.x,2));
	else
		result = (vc.x * vecNormal.x + vc.y * vecNormal.y + vc.z * vecNormal.z + distFromOri);

	return result;
}

double CPlane::SqDistFromPt(CPoint3D pt, CVector3D vc)
{
	double dist = DistFromPt(pt, vc);
	return (dist * dist);
}

bool CPlane::PtOnPlane(CPoint3D &pt, CVector3D &vc)
{
	if(pauline_version == FALSE)
		return DistFromPt(pt, vc)<0.01;
	else
		return ((pt.x * vecNormal.x + pt.y * vecNormal.y + pt.z * vecNormal.z + distFromOri) == 0);
}

float ErrorVelPlane()
{
	AfxMessageBox("Error: wrong index");
	return 0.0f;
}

int	CPlane::LeastSquareFitting(BOOL b3D, CPoint3D *pts, CVector3D *vectors, int nPts)
{
	// pauline_version only uses *pts, PB uses *pts and *vectors

	m_b3D = b3D;

	if(pauline_version==FALSE)
	{

		if(b3D)
		{
			// 3D version using P-B's LMS solver see log 24/11/03
			matrix_a = matrix(1, 3, 1, 3);
			bMatrixaValid = TRUE;

			// compute average velocity
			m_lmsAverageV[0] = m_lmsAverageV[1] = m_lmsAverageV[2] = 0.0f;
			int pt_idx;
			for(pt_idx=0; pt_idx<nPts; pt_idx++)
			{
				m_lmsAverageV[0] = m_lmsAverageV[0] + vectors[pt_idx].x;
				m_lmsAverageV[1] = m_lmsAverageV[1] + vectors[pt_idx].y;
				m_lmsAverageV[2] = m_lmsAverageV[2] + vectors[pt_idx].z;
			}

			m_lmsAverageV[0] = m_lmsAverageV[0]/nPts;
			m_lmsAverageV[1] = m_lmsAverageV[1]/nPts;
			m_lmsAverageV[2] = m_lmsAverageV[2]/nPts;
			
			// compute centroid
			float xcf=0, ycf=0, zcf=0;
			for(pt_idx=0; pt_idx<nPts; pt_idx++)
			{
				xcf = xcf + ((float)pts[pt_idx].x);
				ycf = ycf + ((float)pts[pt_idx].y);
				zcf = zcf + ((float)pts[pt_idx].z);
			}

			int xc, yc, zc;
			xc = FloatToInt(0.5+xcf/((float)nPts));
			yc = FloatToInt(0.5+ycf/((float)nPts));
			zc = FloatToInt(0.5+zcf/((float)nPts));

			// compute W, V
			int i,j;
			float w_matrix[3][3], v_matrix[3][3];
			for(i=0; i<3; i++)
				for(j=0; j<3; j++)
					w_matrix[i][j]=v_matrix[i][j]=0.0f;

			float tmp_w;
			for(pt_idx=0; pt_idx<nPts; pt_idx++)
				for(i=0; i<3; i++)
					for(j=0; j<3; j++)
					{									
						tmp_w = 
							((float)(i==0?pts[pt_idx].x-xc:i==1?pts[pt_idx].y-yc:i==2?pts[pt_idx].z-zc:ErrorVelPlane()))*
							(j==0?(vectors[pt_idx].x)-m_lmsAverageV[0]:
							 j==1?(vectors[pt_idx].y)-m_lmsAverageV[1]:
							 j==2?(vectors[pt_idx].z)-m_lmsAverageV[2]:ErrorVelPlane());

						w_matrix[i][j]=w_matrix[i][j]+tmp_w;

						v_matrix[i][j]=v_matrix[i][j]+
							((float)(i==0?pts[pt_idx].x-xc:i==1?pts[pt_idx].y-yc:i==2?pts[pt_idx].z-zc:ErrorVelPlane())*
							(j==0?pts[pt_idx].x-xc:j==1?pts[pt_idx].y-yc:j==2?pts[pt_idx].z-zc:ErrorVelPlane()));
					}

			for(i=0; i<3; i++)
				for(j=0; j<3; j++)
					{
						//if(w_matrix[i][j]>0.001f)
						//	MessageBeep(0);
						w_matrix[i][j]=w_matrix[i][j]/nPts;
						v_matrix[i][j]=v_matrix[i][j]/nPts;
					}

			// compute V^-1
			float** nrMatrixVinv;
			nrMatrixVinv = matrix(1, 3, 1, 3);
			float** nrVectorB = matrix(1,3,1,1);
			nrVectorB[1][1]=nrVectorB[1][2]=nrVectorB[1][3]=1.0;
			for(i=0; i<3; i++)
				for(j=0; j<3; j++)
					nrMatrixVinv[i+1][j+1]=v_matrix[i][j];

			CNumrecEigenvalues inverter;
			if(inverter.gaussj(nrMatrixVinv, 3, nrVectorB, 1)==0)
				AfxMessageBox("Bad matrix in CPlane::LeastSquareFitting()");

			// compute Tr(V^-1)
			float trace_Vinv = nrMatrixVinv[1][1]+nrMatrixVinv[2][2]+nrMatrixVinv[3][3];

			// compute Ahat = W^T V^-1
			float matrix_Ahat[3][3];
			for(i=0; i<3; i++)
				for(j=0; j<3; j++)
					matrix_Ahat[i][j]=
						w_matrix[0][i]*nrMatrixVinv[1][j+1]+
						w_matrix[1][i]*nrMatrixVinv[2][j+1]+
						w_matrix[2][i]*nrMatrixVinv[3][j+1];

			// compute Tr(Ahat)
			float trace_Ahat = matrix_Ahat[0][0]+matrix_Ahat[1][1]+matrix_Ahat[2][2];

			// compute A (note it's passed from caller as a numerical recipes matrix)
			float traceAdivV = trace_Ahat/trace_Vinv;
			for(i=0; i<3; i++)
				for(j=0; j<3; j++)
					matrix_a[i+1][j+1]=matrix_Ahat[i][j]-traceAdivV*nrMatrixVinv[i+1][j+1];

			// compute velocity at the origin v0 = <v> - A<x>
			v0[0]=m_lmsAverageV[0]-(matrix_a[1][1]*xc+matrix_a[1][2]*yc+matrix_a[1][3]*zc);
			v0[1]=m_lmsAverageV[1]-(matrix_a[2][1]*xc+matrix_a[2][2]*yc+matrix_a[2][3]*zc);
			v0[2]=m_lmsAverageV[2]-(matrix_a[3][1]*xc+matrix_a[3][2]*yc+matrix_a[3][3]*zc);

			centroid.x = xc;
			centroid.y = yc;
			centroid.z = zc;

			return 1;
		}
		else
		{
			// 2D version using P-B's LMS solver see log 24/11/03
			matrix_a = matrix(1, 2, 1, 2);
			bMatrixaValid = TRUE;

			// compute average velocity
			m_lmsAverageV[0] = m_lmsAverageV[1] = m_lmsAverageV[2] = 0.0f;
			int pt_idx;
			for(pt_idx=0; pt_idx<nPts; pt_idx++)
			{
				m_lmsAverageV[0] = m_lmsAverageV[0] + vectors[pt_idx].x;
				m_lmsAverageV[1] = m_lmsAverageV[1] + vectors[pt_idx].y;
			}

			m_lmsAverageV[0] = m_lmsAverageV[0]/nPts;
			m_lmsAverageV[1] = m_lmsAverageV[1]/nPts;
			
			// compute centroid
			float xcf=0, ycf=0;
			for(pt_idx=0; pt_idx<nPts; pt_idx++)
			{
				xcf = xcf + ((float)pts[pt_idx].x);
				ycf = ycf + ((float)pts[pt_idx].y);
			}

			int xc, yc;
			xc = FloatToInt(0.5+xcf/((float)nPts));
			yc = FloatToInt(0.5+ycf/((float)nPts));

			// compute W, V
			int i,j;
			float w_matrix[2][2], v_matrix[2][2];
			for(i=0; i<2; i++)
				for(j=0; j<2; j++)
					w_matrix[i][j]=v_matrix[i][j]=0.0f;

			float tmp_w;
			for(pt_idx=0; pt_idx<nPts; pt_idx++)
				for(i=0; i<2; i++)
					for(j=0; j<2; j++)
					{									
						tmp_w = 
							((float)(i==0?pts[pt_idx].x-xc:i==1?pts[pt_idx].y-yc:ErrorVelPlane()))*
							(j==0?(vectors[pt_idx].x)-m_lmsAverageV[0]:
							 j==1?(vectors[pt_idx].y)-m_lmsAverageV[1]:ErrorVelPlane());

						w_matrix[i][j]=w_matrix[i][j]+tmp_w;

						v_matrix[i][j]=v_matrix[i][j]+
							((float)(i==0?pts[pt_idx].x-xc:i==1?pts[pt_idx].y-yc:ErrorVelPlane())*
							(j==0?pts[pt_idx].x-xc:j==1?pts[pt_idx].y-yc:ErrorVelPlane()));
					}

			for(i=0; i<2; i++)
				for(j=0; j<2; j++)
					{
						//if(w_matrix[i][j]>0.001f)
						//	MessageBeep(0);
						w_matrix[i][j]=w_matrix[i][j]/nPts;
						v_matrix[i][j]=v_matrix[i][j]/nPts;
					}

			// compute V^-1
			float** nrMatrixVinv;
			nrMatrixVinv = matrix(1, 2, 1, 2);
			float** nrVectorB = matrix(1,2,1,1);
			nrVectorB[1][1]=nrVectorB[1][2]=1.0;
			for(i=0; i<2; i++)
				for(j=0; j<2; j++)
					nrMatrixVinv[i+1][j+1]=v_matrix[i][j];

			CNumrecEigenvalues inverter;
			if(inverter.gaussj(nrMatrixVinv, 2, nrVectorB, 1)==0)
				AfxMessageBox("Bad matrix in CPlane::LeastSquareFitting()");

			// compute Tr(V^-1)
			float trace_Vinv = nrMatrixVinv[1][1]+nrMatrixVinv[2][2];

			// compute Ahat = W^T V^-1
			float matrix_Ahat[2][2];
			for(i=0; i<2; i++)
				for(j=0; j<2; j++)
					matrix_Ahat[i][j]=
						w_matrix[0][i]*nrMatrixVinv[1][j+1]+
						w_matrix[1][i]*nrMatrixVinv[2][j+1];

			// compute Tr(Ahat)
			float trace_Ahat = matrix_Ahat[0][0]+matrix_Ahat[1][1]+matrix_Ahat[2][2];

			// compute A (note it's passed from caller as a numerical recipes matrix)
			float traceAdivV = trace_Ahat/trace_Vinv;
			for(i=0; i<2; i++)
				for(j=0; j<2; j++)
					matrix_a[i+1][j+1]=matrix_Ahat[i][j]-traceAdivV*nrMatrixVinv[i+1][j+1];

			// compute velocity at the origin v0 = <v> - A<x>
			v0[0]=m_lmsAverageV[0]-(matrix_a[1][1]*xc+matrix_a[1][2]*yc);
			v0[1]=m_lmsAverageV[1]-(matrix_a[2][1]*xc+matrix_a[2][2]*yc);

			centroid.x = xc;
			centroid.y = yc;
			centroid.z = 0;

			return 1;
		}
		
	}
	else
	{
		// Pauline Ng's own version for 2D only
		// see her code in FlowVis
		// sets values of distFromOri and vecNormal

		// return 0 if failed, 1 if successful
		if ( nPts < 3)  // not enough control points
			return 0;  
		
		double	matrixBufA[9];
		double	*matrixA[3];
		double	matrixB[3];
		CVector3D	vecX;
		int			i;
		
		// initialisation
		for (i = 0; i < 3; i++) {
			matrixA[i] = &matrixBufA[i * 3];
			matrixA[i][0] = matrixA[i][1] = matrixA[i][2] = matrixB[i] = 0;
		}
		
		
		for ( i = 0; i < nPts; i++) {
			
			matrixA[0][0] += pts[i].x * pts[i].x;
			matrixA[0][1] += pts[i].x * pts[i].y;
			matrixA[0][2] += pts[i].x;
			
			matrixA[1][1] += pts[i].y * pts[i].y;
			matrixA[1][2] += pts[i].y;
			
			matrixB[0] += pts[i].x * pts[i].z;
			matrixB[1] += pts[i].y * pts[i].z;
			matrixB[2] += pts[i].z;
		}
		
		matrixA[1][0] = matrixA[0][1];
		matrixA[2][0] = matrixA[0][2];
		matrixA[2][1] = matrixA[1][2];
		matrixA[2][2] = nPts;
		
		
		// solve the equations 
		if ( SolveEquations(matrixBufA, matrixB, &vecX)){
			double	norm;
			
			// change to hessian normal form
			norm = vecX.Norm();
			if ( fabs(norm) < 1.0e-6) {  // plane z = 0
				vecNormal.SetComp(0, 0, 1);
				distFromOri = 0;
			}
			else {
				vecNormal.x = vecX.x / norm;
				vecNormal.y = vecX.y / norm;
				vecNormal.z = -1.0 / norm;
				distFromOri = vecX.z / norm;
			}
			return 1;
		}
		else 
			return 0;
	}

}


int CPlane::SolveEquations(double matrixBufA[], double matrixB[], CVector3D *pVecX)
{
	// this 2D function is obsolete in 3D

	if(pauline_version==FALSE)
		AfxMessageBox("Unexpected error: CPlane::SolveEquations() called with 3D version flag.");

	// solve 3x3 linear equations using Cramer's rule

	double	detA;
	double	detB[3];
	double	matrixBuf[9];
	int			i, j;
	
	detA = CalDeterminant(matrixBufA);

	// matrix is singular 
	if ( fabs(detA) < 1.0E-6) {
		return 0;
	}
	
	// solution exist 
	for (i = 0 ; i < 3; i++){
		
		for ( j = 0; j < 9; j++)
			matrixBuf[j] = matrixBufA[j];
		
		// replace column i with matrixB
		matrixBuf[i * 3] = matrixB[0];
		matrixBuf[i * 3 + 1] = matrixB[1];
		matrixBuf[i * 3 + 2] = matrixB[2];
		
		detB[i] = CalDeterminant(matrixBuf);
	}

	pVecX->x = detB[0] / detA;
	pVecX->y = detB[1] / detA;
	pVecX->z = detB[2] / detA;

	return 1;
}


double CPlane::CalDeterminant(double matrixBuf[])
{

	// this function is obsolete in 3D

	double	det;
  int     i;
	double	*matrix[3];                                                               
	
	for (i = 0; i < 3; i++)
 		matrix [i] = &matrixBuf[i*3];

  det = matrix [0][0] * matrix [1][1] * matrix [2][2] + matrix [1][0] * matrix [2][1] * matrix [0][2]
				+ matrix [2][0] * matrix [1][2] * matrix [0][1] - matrix [0][2] * matrix [1][1] * matrix [2][0]
        - matrix [0][0] * matrix [2][1] * matrix [1][2] - matrix [0][1] * matrix [1][0] * matrix [2][2];

 return det;
}

int CPlane::SolveForZ(double x, double y, double *pz)
{
	// this 2D function is replaced in 3D by GetFitVel()

	if(pauline_version==FALSE)
		AfxMessageBox("Unexpected error: CPlane::SolveForZ() called with 3D version flag.");

	if ( fabs(vecNormal.z) > 1e-6){
		*pz = - (vecNormal.x * x + vecNormal.y * y + distFromOri)/ vecNormal.z;
		return 1;
	}
	else {
		*pz = 0;
		return 0;
	}
}

CVector3D CPlane::GetFitVel(int x, int y, int z)
{
	CVector3D result;
	
	if(m_b3D == TRUE)
	{
		result.y = matrix_a[2][1]*x + matrix_a[2][2]*y + matrix_a[2][3]*z + v0[1];
		result.x = matrix_a[1][1]*x + matrix_a[1][2]*y + matrix_a[1][3]*z + v0[0];
		result.z = matrix_a[3][1]*x + matrix_a[3][2]*y + matrix_a[3][3]*z + v0[2];
	}
	else
	{
		result.y = matrix_a[2][1]*x + matrix_a[2][2]*y + v0[1];
		result.x = matrix_a[1][1]*x + matrix_a[1][2]*y + v0[0];
	}

	return result;
}

int CPlane::FloatToInt(float fnum)
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

CPoint3D CPlane::GetAverageX()
{
	return centroid;
}

CVector3D CPlane::GetAverageV()
{
	return CVector3D(m_lmsAverageV[0], m_lmsAverageV[1], m_lmsAverageV[2]);
}

float** CPlane::GetMatrixA()
{
	if(bMatrixaValid==TRUE)
	{
		float** result;

		if(m_b3D == TRUE)
			result = matrix(1, 3, 1, 3);
		else		
			result = matrix(1, 2, 1, 2);

		result[1][1] = matrix_a[1][1];
		result[1][2] = matrix_a[1][2];
		if(m_b3D == TRUE)
			result[1][3] = matrix_a[1][3];
		result[2][1] = matrix_a[2][1];
		result[2][2] = matrix_a[2][2];
		if(m_b3D == TRUE)
		{
			result[2][3] = matrix_a[2][3];
			result[3][1] = matrix_a[3][1];
			result[3][2] = matrix_a[3][2];
			result[3][3] = matrix_a[3][3];
		}
		return result;
	}
	else
		return NULL;
	

}

CPoint3D CPlane::GetCentroid()
{
	return centroid;
}

CPlane::operator=(const CPlane &toCopy)
{
	if(matrix_a != NULL && bMatrixaValid==TRUE)
	{
		if(m_b3D==TRUE)
			free_matrix(matrix_a,1,3,1,3);
		else
			free_matrix(matrix_a,1,2,1,2);

		bMatrixaValid=FALSE;
	}

	if(toCopy.bMatrixaValid==TRUE)
		if(toCopy.m_b3D==TRUE)
		{
			matrix_a = matrix(1, 3, 1, 3);
			bMatrixaValid = TRUE;
			matrix_a[1][1]	= toCopy.matrix_a[1][1];
			matrix_a[1][2]	= toCopy.matrix_a[1][2];
			matrix_a[1][3]	= toCopy.matrix_a[1][3];
			matrix_a[2][1]	= toCopy.matrix_a[2][1];
			matrix_a[2][2]	= toCopy.matrix_a[2][2];
			matrix_a[2][3]	= toCopy.matrix_a[2][3];
			matrix_a[3][1]	= toCopy.matrix_a[3][1];
			matrix_a[3][2]	= toCopy.matrix_a[3][2];
			matrix_a[3][3]	= toCopy.matrix_a[3][3];
		
		}
		else
		{
			matrix_a = matrix(1, 2, 1, 2);
			bMatrixaValid = TRUE;
			matrix_a[1][1]	= toCopy.matrix_a[1][1];
			matrix_a[1][2]	= toCopy.matrix_a[1][2];
			matrix_a[2][1]	= toCopy.matrix_a[2][1];
			matrix_a[2][2]	= toCopy.matrix_a[2][2];
		
		}

	m_lmsAverageV[0] = toCopy.m_lmsAverageV[0];
	m_lmsAverageV[1] = toCopy.m_lmsAverageV[1];
	m_lmsAverageV[2] = toCopy.m_lmsAverageV[2];

	v0[0] = toCopy.v0[0];
	v0[1] = toCopy.v0[1];
	v0[2] = toCopy.v0[2];

	m_b3D = toCopy.m_b3D;

	centroid = toCopy.centroid;

}

float* CPlane::GetV0()
{
	return v0;
}

void CPlane::SetValuesFromFileData(BOOL b3D, float av_vx, float av_vy, 
								   float av_vz, float **matrixA, float *cv0, 
								   int cx, int cy, int cz)
{
	m_b3D = b3D;
	m_lmsAverageV[0] = av_vx;
	m_lmsAverageV[1] = av_vy;
	m_lmsAverageV[2] = av_vz;
	
	if(m_b3D)
	{
		matrix_a = matrix(1, 3, 1, 3);
		matrix_a[1][1] = matrixA[1][1];
		matrix_a[1][2] = matrixA[1][2];
		matrix_a[1][3] = matrixA[1][3];
		matrix_a[2][1] = matrixA[2][1];
		matrix_a[2][2] = matrixA[2][2];
		matrix_a[2][3] = matrixA[2][3];
		matrix_a[3][1] = matrixA[3][1];
		matrix_a[3][2] = matrixA[3][2];
		matrix_a[3][3] = matrixA[3][3];
	}
	else
	{
		matrix_a = matrix(1, 2, 1, 2);
		matrix_a[1][1] = matrixA[1][1];
		matrix_a[1][2] = matrixA[1][2];
		matrix_a[2][1] = matrixA[2][1];
		matrix_a[2][2] = matrixA[2][2];
	}

	bMatrixaValid = TRUE;

	v0[0] = cv0[0];
	v0[1] = cv0[1];
	v0[2] = cv0[2];

	centroid.x = cx;
	centroid.y = cy;
	centroid.z = cz;
	
}
