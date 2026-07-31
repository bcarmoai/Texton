// VtkUtilities.cpp: implementation of the CVtkUtilities class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "texton.h"
#include "VtkUtilities.h"

#include "vtkSphereSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkRenderWindowInteractor.h"
#include <vtkSLCReader.h> 
#include <vtkVolumeTextureMapper2D.h> 
#include <vtkVolume.h> 
#include <vtkStructuredGridReader.h>
#include <vtkWin32OpenGLRenderWindow.h>
#include <vtkWin32RenderWindowInteractor.h>
#include <vtkCommand.h>
#include <vtkPoints.h>
#include <vtkStructuredGrid.h>
#include <vtkRungeKutta4.h>
#include <vtkStreamLine.h>
#include <vtkTubeFilter.h>
#include <vtkDataSetAttributes.h>
#include <vtkFloatArray.h>
#include <vtkPolyData.h>
#include <vtkStructuredGridWriter.h>
#include <vtkPointSource.h>
#include <vtkStructuredPointsWriter.h>
#include <vtkProbeFilter.h>
#include <vtkImageData.h>
#include <vtkStructuredPoints.h>
#include <vtkStructuredPointsReader.h>
#include <vtkImageLaplacian.h>
#include <vtkJPEGWriter.h>
#include <vtkUnsignedCharArray.h>
#include <vtkStructuredPointsWriter.h>
#include <vtkCellArray.h>
#include <vtkProperty.h>
#include <vtkInteractorStyleTrackballCamera.h> 
#include <vtkPointData.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVtkUtilities::CVtkUtilities()
{

}

CVtkUtilities::~CVtkUtilities()
{

}

void CVtkUtilities::Laplacian(float* input, float *laplacian, int img_width, int img_height, int img_slices)
{
	vtkImageData* id = vtkImageData::New();
	vtkImageLaplacian* imagelaplacian = vtkImageLaplacian::New();
	vtkFloatArray* chararray = vtkFloatArray::New();

	id->SetDimensions(img_width, img_height, img_slices);
	id->SetScalarTypeToFloat();
	id->SetNumberOfScalarComponents(1);
	id->AllocateScalars();

	float* fdata = (float*) id->GetScalarPointer();
	int x, y, z, i;
	i=0;
	for(z=0; z<img_slices; z++)
		for(y=0; y<img_height; y++)
			for(x=0; x<img_width; x++)
			{
				fdata[i]=input[z*img_width*img_height+y*img_width+x];
				i++;
			}


	imagelaplacian->SetInput(id);
	imagelaplacian->SetDimensionality(img_slices>1?3:2);
	imagelaplacian->Update();
	
	vtkImageData* imgresult = imagelaplacian->GetOutput();
	float* resdata = (float*) imgresult->GetScalarPointer();

	i=0;
	for(z=0; z<img_slices; z++)
		for(y=0; y<img_height; y++)
			for(x=0; x<img_width; x++)
			{
				laplacian[z*img_width*img_height+y*img_width+x]=fabs(resdata[i]);
				i++;
			}

	WriteImageJpg(laplacian, img_width, img_height, img_slices, 0, "c:\\test.jpg");

	id->Delete();
	imagelaplacian->Delete();
	chararray->Delete();
	//imgresult->Delete();

}

void CVtkUtilities::WriteImageJpg(float *image, int img_width, int img_height, int img_slices, int sliceno,
								  char* filename)
{

	if(sliceno >= img_slices)
	{
		CString sms;
		sms.Format("Error writing jpeg: bad slice specified (%i for %i slices).", sliceno, img_slices);
		AfxMessageBox(sms);
		return;
	}

	vtkImageData* imagedata = vtkImageData::New();
	vtkImageData* id = vtkImageData::New();
	vtkImageLaplacian* imagelaplacian = vtkImageLaplacian::New();
	vtkUnsignedCharArray* chararray = vtkUnsignedCharArray::New();

	id->SetDimensions(img_width, img_height, img_slices);
	id->SetScalarTypeToUnsignedChar();
	id->SetNumberOfScalarComponents(1);
	id->AllocateScalars();

	unsigned char* fdata = (unsigned char*) id->GetScalarPointer();
	int x, y, z, i;
	i=0;
	z=sliceno;
	for(y=0; y<img_height; y++)
		for(x=0; x<img_width; x++)
		{
			fdata[i]=image[z*img_width*img_height+y*img_width+x];
			i++;
		}

	vtkJPEGWriter * imagewriter = vtkJPEGWriter ::New();
	imagewriter->SetFileName(filename);
	imagewriter->SetInput(id);
	imagewriter->Write();
	imagewriter->Delete();

	imagedata->Delete();
	id->Delete();
	imagelaplacian->Delete();
	chararray->Delete();
	
}

void CVtkUtilities::WriteImageVtk(float *image, int img_width, int img_height, int img_slices,
								  char* filename)
{

	vtkImageData* id = vtkImageData::New();
	vtkUnsignedCharArray* chararray = vtkUnsignedCharArray::New();

	id->SetDimensions(img_width, img_height, img_slices);
	id->SetScalarTypeToFloat();
	id->SetNumberOfScalarComponents(1);
	id->AllocateScalars();

	float* fdata = (float*) id->GetScalarPointer();
	int x, y, z, i;
	i=0;
	for(z=0; z<img_slices; z++)
		for(y=0; y<img_height; y++)
			for(x=0; x<img_width; x++)
			{
				fdata[i]=image[z*img_width*img_height+y*img_width+x];
				i++;
			}

	vtkStructuredPointsWriter * imagewriter = vtkStructuredPointsWriter ::New();
	imagewriter->SetFileName(filename);
	imagewriter->SetInput(id);
	imagewriter->Write();
	imagewriter->Delete();

	id->Delete();
	chararray->Delete();

}

void CVtkUtilities::WriteImageVtk(int *image, int img_width, int img_height, int img_slices,
								  char* filename)
{

	vtkImageData* id = vtkImageData::New();
	vtkUnsignedCharArray* chararray = vtkUnsignedCharArray::New();

	id->SetDimensions(img_width, img_height, img_slices);
	id->SetScalarTypeToUnsignedChar();
	id->SetNumberOfScalarComponents(1);
	id->AllocateScalars();

	unsigned char* fdata = (unsigned char*) id->GetScalarPointer();
	int x, y, z, i;
	i=0;
	for(z=0; z<img_slices; z++)
		for(y=0; y<img_height; y++)
			for(x=0; x<img_width; x++)
			{
				fdata[i]=image[z*img_width*img_height+y*img_width+x];
				i++;
			}

	vtkStructuredPointsWriter * imagewriter = vtkStructuredPointsWriter ::New();
	imagewriter->SetFileName(filename);
	imagewriter->SetInput(id);
	imagewriter->Write();
	imagewriter->Delete();

	id->Delete();
	chararray->Delete();

}

void CVtkUtilities::WriteImageSgrid(vtkStructuredGrid *image, char* filename)
{
	
	vtkStructuredGridWriter* swriter = vtkStructuredGridWriter::New();
	swriter->SetInput(image);
	swriter->SetFileName("c:\\test.vtk");
	swriter->Write();

	swriter->Delete();

}

void CVtkUtilities::RenderStreamlineList(CPointFloatArray* pointArray, int nStreams)
{

	// a renderer and render window
	vtkRenderer *ren1 = vtkRenderer::New();
	vtkWin32OpenGLRenderWindow *renWin = vtkWin32OpenGLRenderWindow::New();
	renWin->AddRenderer(ren1);
	
	// an interactor
	vtkWin32RenderWindowInteractor *iren = vtkWin32RenderWindowInteractor::New();
	iren->SetRenderWindow(renWin);

	vtkInteractorStyleTrackballCamera* icam = vtkInteractorStyleTrackballCamera::New();
	iren->SetInteractorStyle(icam);
	
	// add each steamline as an actor
	int iStream; // the current streamline being added

	vtkPoints** points = new vtkPoints*[nStreams];
	vtkCellArray** cells = new vtkCellArray*[nStreams];
	vtkPolyData** polydata = new vtkPolyData*[nStreams];
	vtkTubeFilter** streamTube = new vtkTubeFilter*[nStreams];
	vtkPolyDataMapper** mapStreamTube = new vtkPolyDataMapper*[nStreams];
	vtkActor** streamTubeActor = new vtkActor*[nStreams];

	for(iStream=0; iStream<nStreams; iStream++)
	{
		if(pointArray[iStream].pointArray.GetSize()<=2)
			continue;

		points[iStream] = vtkPoints::New();
		cells[iStream] = vtkCellArray::New();
		cells[iStream]->InsertNextCell(pointArray[iStream].pointArray.GetSize());

		int i;
		for(i=0; i<pointArray[iStream].pointArray.GetSize(); i++)
		{
			points[iStream]->InsertNextPoint(pointArray[iStream].pointArray[i].x, 
				pointArray[iStream].pointArray[i].y, 
				pointArray[iStream].pointArray[i].z);
			cells[iStream]->InsertCellPoint(i);
		}

		polydata[iStream] = vtkPolyData::New();
		polydata[iStream]->SetPoints(points[iStream]);
		polydata[iStream]->SetLines(cells[iStream]);
		
		streamTube[iStream] = vtkTubeFilter::New();
		streamTube[iStream]->SetInput(polydata[iStream]);
		streamTube[iStream]->SetRadius(0.2);
		streamTube[iStream]->SetNumberOfSides(12);
		
		mapStreamTube[iStream] = vtkPolyDataMapper::New();
		mapStreamTube[iStream]->SetInput(streamTube[iStream]->GetOutput());
		
		streamTubeActor[iStream] = vtkActor::New();
		streamTubeActor[iStream]->SetMapper(mapStreamTube[iStream]);
		streamTubeActor[iStream]->GetProperty()->BackfaceCullingOn();
//		float col[3]; col[0]=(.5, .9, .9);
//		streamTubeActor[iStream]->GetProperty()->SetColor(col);

		// add the actor to the scene
		ren1->AddActor(streamTubeActor[iStream]);

	}

	ren1->SetBackground(1,1,1); // Background color white
	
	// render an image (lights and cameras are created automatically)
	//ren1->SetBackground(0,0,0);
	renWin->Render();
	
	// begin mouse interaction
	iren->Start();

	// prevent window from exiting FudgeBox
	iren->Disable();

	for(iStream=0; iStream<nStreams; iStream++)
	{
		if(pointArray[iStream].pointArray.GetSize()<=2)
			continue;

		points[iStream]->Delete();
		cells[iStream]->Delete();
		polydata[iStream]->Delete();
		streamTube[iStream]->Delete();
		mapStreamTube[iStream]->Delete();
		streamTubeActor[iStream]->Delete();

	}

	delete [] points;
	delete [] cells;
	delete [] polydata;
	delete [] streamTube;
	delete [] mapStreamTube;
	delete [] streamTubeActor;

	ren1->Delete();
	renWin->Delete();
	iren->Delete();
	icam->Delete();	

}

/*
void CVtkUtilities::RenderStreamlineList(CPointFloatArray* pointArray, int nStreams)
{

	// a renderer and render window
	vtkRenderer *ren1 = vtkRenderer::New();
	vtkWin32OpenGLRenderWindow *renWin = vtkWin32OpenGLRenderWindow::New();
	renWin->AddRenderer(ren1);
	
	// an interactor
	vtkWin32RenderWindowInteractor *iren = vtkWin32RenderWindowInteractor::New();
	iren->SetRenderWindow(renWin);

	vtkInteractorStyleTrackballCamera* icam = vtkInteractorStyleTrackballCamera::New();
	iren->SetInteractorStyle(icam);
	
	// add each steamline as an actor
	int iStream; // the current streamline being added

	vtkPoints** points = new vtkPoints*[nStreams];
	vtkCellArray** cells = new vtkCellArray*[nStreams];
	vtkPolyData** polydata = new vtkPolyData*[nStreams];
	vtkTubeFilter** streamTube = new vtkTubeFilter*[nStreams];
	vtkPolyDataMapper** mapStreamTube = new vtkPolyDataMapper*[nStreams];
	vtkActor** streamTubeActor = new vtkActor*[nStreams];

	for(iStream=0; iStream<nStreams; iStream++)
	{
		if(pointArray[iStream].pointArray.GetSize()<=2)
			continue;

		points[iStream] = vtkPoints::New();
		cells[iStream] = vtkCellArray::New();
		cells[iStream]->InsertNextCell(pointArray[iStream].pointArray.GetSize());

		int i;
		for(i=0; i<pointArray[iStream].pointArray.GetSize(); i++)
		{
			points[iStream]->InsertNextPoint(pointArray[iStream].pointArray[i].x, 
				pointArray[iStream].pointArray[i].y, 
				pointArray[iStream].pointArray[i].z);
			cells[iStream]->InsertCellPoint(i);
		}

		polydata[iStream] = vtkPolyData::New();
		polydata[iStream]->SetPoints(points[iStream]);
		polydata[iStream]->SetLines(cells[iStream]);
		
		streamTube[iStream] = vtkTubeFilter::New();
		streamTube[iStream]->SetInput(polydata[iStream]);
		streamTube[iStream]->SetRadius(0.2);
		streamTube[iStream]->SetNumberOfSides(12);
		//streamTube->SetVaryRadiusToVaryRadiusByVector();
		
		mapStreamTube[iStream] = vtkPolyDataMapper::New();
		mapStreamTube[iStream]->SetInput(streamTube[iStream]->GetOutput());
		//mapStreamTube->SetInput(polydata);
		//mapStreamTube->SetScalarRange(sgrid->GetPointData()->GetScalars()->GetRange());
		
		streamTubeActor[iStream] = vtkActor::New();
		streamTubeActor[iStream]->SetMapper(mapStreamTube[iStream]);
		streamTubeActor[iStream]->GetProperty()->BackfaceCullingOn();

		// add the actor to the scene
		ren1->AddActor(streamTubeActor[iStream]);

	}

	ren1->SetBackground(1,1,1); // Background color white
	
	// render an image (lights and cameras are created automatically)
	//ren1->SetBackground(0,0,0);
	renWin->Render();
	
	// begin mouse interaction
	iren->Start();

	// prevent window from exiting FudgeBox
	iren->Disable();

	for(iStream=0; iStream<nStreams; iStream++)
	{
		if(pointArray[iStream].pointArray.GetSize()<=2)
			continue;

		points[iStream]->Delete();
		cells[iStream]->Delete();
		polydata[iStream]->Delete();
		streamTube[iStream]->Delete();
		mapStreamTube[iStream]->Delete();
		streamTubeActor[iStream]->Delete();

	}

	delete [] points;
	delete [] cells;
	delete [] polydata;
	delete [] streamTube;
	delete [] mapStreamTube;
	delete [] streamTubeActor;

	ren1->Delete();
	renWin->Delete();
	iren->Delete();
	icam->Delete();	

}
*/

void CVtkUtilities::SaveArrowData(CData3DMRI &mriData3D)
{
	// load flow data into vtkRectilinearGrid
	vtkFloatArray *fpoints = vtkFloatArray::New();
	vtkPoints* points = vtkPoints::New();
	vtkFloatArray *fspoints = vtkFloatArray::New();
	fspoints->SetNumberOfComponents(1);
	fpoints->SetNumberOfComponents(3);
	
	const float div = 1000.0f;

	const int img_width = mriData3D.img_width;
	const int img_height = mriData3D.img_height;
	const int img_slices = mriData3D.img_slices;

	int x, y, z;

	// save all
	for(y=0; y<img_height; y++)
		for(x=0; x<img_width; x++)
			for(z=0; z<img_slices; z++)
				if(!(mriData3D.u[z*img_width*img_height+y*img_width+x]==0 && 
				   mriData3D.v[z*img_width*img_height+y*img_width+x]==0 && 
				   mriData3D.w[z*img_width*img_height+y*img_width+x]==0))
				{
					fspoints->InsertNextTuple1(sqrt(pow(mriData3D.u[z*img_width*img_height+y*img_width+x],2.0)+
						pow(mriData3D.v[z*img_width*img_height+y*img_width+x],2.0)+
						pow(mriData3D.w[z*img_width*img_height+y*img_width+x],2.0))/div);
					points->InsertNextPoint(x, y, z);
					fpoints->InsertNextTuple3(mriData3D.u[z*img_width*img_height+y*img_width+x]/div,
						mriData3D.v[z*img_width*img_height+y*img_width+x]/div,
						mriData3D.w[z*img_width*img_height+y*img_width+x]/div);
				}

	vtkStructuredGrid *sgrid = vtkStructuredGrid::New();
	sgrid->SetDimensions(img_width, img_height, img_slices);
	sgrid->SetPoints(points);
	sgrid->GetPointData()->SetScalars(fspoints);
	sgrid->GetPointData()->SetVectors(fpoints);

	CString fname;
	fname.Format("c:\\testARROWS.vtk");
	vtkStructuredGridWriter* swriter = vtkStructuredGridWriter::New();
	swriter->SetInput(sgrid);
	swriter->SetFileName(fname);
	swriter->Write();

	fpoints->Delete();
	fspoints->Delete();
	points->Delete();
	sgrid->Delete();
	swriter->Delete();


}
