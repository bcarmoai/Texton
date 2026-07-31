// DlgVortexVtk.cpp : implementation file
//

#include "stdafx.h"
#include "texton.h"
#include "DlgVortexVtk.h"

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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgVortexVtk dialog


CDlgVortexVtk::CDlgVortexVtk(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgVortexVtk::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgVortexVtk)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgVortexVtk::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgVortexVtk)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgVortexVtk, CDialog)
	//{{AFX_MSG_MAP(CDlgVortexVtk)
	ON_BN_CLICKED(IDC_VORTEX_VTK_GO, OnVortexVtkGo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgVortexVtk message handlers

void CDlgVortexVtk::OnVortexVtkGo() 
{
/*	// create sphere geometry
	vtkSphereSource *sphere = vtkSphereSource::New();
	sphere->SetRadius(1.0);
	sphere->SetThetaResolution(18);
	sphere->SetPhiResolution(18);
	
	// map to graphics library
	vtkPolyDataMapper *map = vtkPolyDataMapper::New();
	map->SetInput(sphere->GetOutput());
	
	// actor coordinates geometry, properties, transformation
	vtkActor *aSphere = vtkActor::New();
	aSphere->SetMapper(map);
	aSphere->GetProperty()->SetColor(0,0,1); // sphere color blue
	
	// a renderer and render window
	vtkRenderer *ren1 = vtkRenderer::New();
	vtkWin32OpenGLRenderWindow *renWin = vtkWin32OpenGLRenderWindow::New();
	renWin->AddRenderer(ren1);
	
	// an interactor
	vtkWin32RenderWindowInteractor *iren = vtkWin32RenderWindowInteractor::New();
	iren->SetRenderWindow(renWin);
	
	// add the actor to the scene
	ren1->AddActor(aSphere);
	ren1->SetBackground(1,1,1); // Background color white
	
	// render an image (lights and cameras are created automatically)
	renWin->Render();
	
	// begin mouse interaction
	iren->Start();

	// prevent window from exiting FudgeBox
	iren->Disable();
	*/

}

void CDlgVortexVtk::OnCancel() 
{
	
	CDialog::OnCancel();
}
