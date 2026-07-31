// VtkUtilities.h: interface for the CVtkUtilities class.
//
//////////////////////////////////////////////////////////////////////

#include <vtkStructuredGrid.h>

#if !defined(AFX_VTKUTILITIES_H__EE2F55A8_EC85_48AA_AD93_AA0B4084E8FE__INCLUDED_)
#define AFX_VTKUTILITIES_H__EE2F55A8_EC85_48AA_AD93_AA0B4084E8FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PointFloatArray.h"
#include "Data3DMRI.h"

class CVtkUtilities  
{
public:
	void SaveArrowData(CData3DMRI& mriData3D);
	void WriteImageSgrid(vtkStructuredGrid *image, char* filename);
	void WriteImageVtk(float* image, int img_width, int img_height, int img_slices,
								  char* filename);
	void WriteImageVtk(int* image, int img_width, int img_height, int img_slices,
								  char* filename);
	void WriteImageJpg(float* image, int img_width, int img_height, int img_slices, int sliceno, 
		char* filename);
	void Laplacian(float* input, float* laplacian, int img_width, int img_height, int img_slices);
	CVtkUtilities();
	virtual ~CVtkUtilities();
	void RenderStreamlineList(CPointFloatArray* pointArray, int nStreams);

};

#endif // !defined(AFX_VTKUTILITIES_H__EE2F55A8_EC85_48AA_AD93_AA0B4084E8FE__INCLUDED_)
