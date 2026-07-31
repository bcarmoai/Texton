// DlgRestoration.cpp : implementation file
//

#include "stdafx.h"
#include "Texton.h"
#include "DlgRestoration.h"
#include "TextonView.h"
#include <math.h>
#include <stdio.h>
#include <float.h>
#include "Rnd_no_gen.h"
#include "DlgTextBox.h"
#include "DlgRestorROI.h"
#include "MainFrm.h"
#include <fstream.h>
#include "DlgRender.h"
#include "DlgZoomedSlice.h"
#include "Dlg7DLoadOptions.h"

#include <vtkPoints.h>
#include <vtkStructuredGrid.h>
#include <vtkFloatArray.h>
#include <vtkStructuredGridWriter.h>
#include <vtkPointSource.h>

#include "VtkUtilities.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgRestoration dialog


CDlgRestoration::CDlgRestoration(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRestoration::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgRestoration)
	m_3DVortexRange = 3600;
	m_RestoredSliceNo = 1;
	m_3DNoiseS = 1;
	m_3DNoiseM = 0.0f;
	m_3DNoiseV = 50.0f;
	m_3dRestorVortexSlices = 5;
	m_3DRestorLambda = 0.0f;
	m_3DRestorMinLambdaDiff = 0.05f;
	m_3DRestorRoiText = _T("");
	m_3DRestorSliceEnd = 5;
	m_Restor3dLambdaFixed = FALSE;
	m_3DRestorSigma = 0.1f;
	m_3DRestorDeltaT = 0.01f;
	m_3DRestorIterations = 10;
	m_Restor3dImgSizesTxt = _T("");
	m_fZoomScale = 1.0f;
	m_ZeroSkip = FALSE;
	//}}AFX_DATA_INIT

	pStrm = NULL;
	pDlgRoi = NULL;
	bZoom = FALSE;
	iRestor2d_xtop = 0;
	iRestor2d_ytop = 0;
	iRestor2d_xbottom = 255;
	iRestor2d_ybottom = 255;
	char sz255[100];
	sprintf(sz255, "%i, %i --> %i, %i (%i, %i)", iRestor2d_xtop, iRestor2d_ytop, 
		iRestor2d_xbottom, iRestor2d_ybottom, 
		iRestor2d_xbottom-iRestor2d_xtop, iRestor2d_ybottom-iRestor2d_ytop);
}


void CDlgRestoration::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRestoration)
	DDX_Text(pDX, IDC_RESTOR_3DVRANGE, m_3DVortexRange);
	DDX_Text(pDX, IDC_RESTOR_3D_RESTORED_SLICE, m_RestoredSliceNo);
	DDX_Text(pDX, IDC_RESTOR_3DNOISE_S2, m_3DNoiseS);
	DDX_Text(pDX, IDC_RESTOR_3DNOISE_M2, m_3DNoiseM);
	DDX_Text(pDX, IDC_RESTOR_3DNOISE_V2, m_3DNoiseV);
	DDX_Text(pDX, IDC_RESTOR_3DNOISE_SLICES, m_3dRestorVortexSlices);
	DDX_Text(pDX, IDC_3DRESTOR_LAMBDA, m_3DRestorLambda);
	DDX_Text(pDX, IDC_3DRESTOR_MINLAMBDA_DIFF, m_3DRestorMinLambdaDiff);
	DDX_Text(pDX, IDC_3DRESTOR_ROI_TEXT, m_3DRestorRoiText);
	DDX_Text(pDX, IDC_RESTOR_3D_RESTORED_RANGE, m_3DRestorSliceEnd);
	DDX_Check(pDX, IDC_RESTOR3D_LAMBDA_FIX, m_Restor3dLambdaFixed);
	DDX_Text(pDX, IDC_3DRESTOR_SIGMA, m_3DRestorSigma);
	DDX_Text(pDX, IDC_3DRESTOR_DELTAT, m_3DRestorDeltaT);
	DDX_Text(pDX, IDC_3DRESTOR_ITERATIONS, m_3DRestorIterations);
	DDX_Text(pDX, IDC_RESTOR3D_IMGSIZES, m_Restor3dImgSizesTxt);
	DDX_Text(pDX, IDC_RESTOR3D_ZOOM_SCALE, m_fZoomScale);
	DDV_MinMaxFloat(pDX, m_fZoomScale, 1.f, 30.f);
	DDX_Check(pDX, IDC_RESTOR_ZERO_SKIP, m_ZeroSkip);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgRestoration, CDialog)
	//{{AFX_MSG_MAP(CDlgRestoration)
	ON_BN_CLICKED(IDC_RESTOR_3DVORTEX, OnRestor3dvortex)
	ON_BN_CLICKED(IDC_RESTOR_3DVIS_DATA, OnRestor3dvisData)
	ON_BN_CLICKED(IDC_RESTOR_3D_NOISE_GO, OnRestor3dNoiseGo)
	ON_BN_CLICKED(IDC_RESTOR_3DVIS_NOISY, OnRestor3dvisNoisy)
	ON_BN_CLICKED(IDC_RESTOR_3D_ROI, OnRestor3dRoi)
	ON_BN_CLICKED(IDC_RESTOR_3DGO, OnRestor3dgo)
	ON_BN_CLICKED(IDC_RESTOR_3DVIS_RESTORED, OnRestor3dvisRestored)
	ON_BN_CLICKED(IDC_RESTOR_3DLOAD, OnRestor3dload)
	ON_BN_CLICKED(IDC_RESTOR_3DLOAD2, OnRestor3dloadFrame)
	ON_BN_CLICKED(IDC_RESTOR_FIND_VORTEX, OnRestorFindVortex)
	ON_BN_CLICKED(IDC_RESTOR_3DVIS_DATA2, OnRestor3dvisData2)
	ON_BN_CLICKED(IDC_RESTOR_3DVIS_NOISYDATA, OnRestor3dvisNoisydata)
	ON_BN_CLICKED(IDC_RESTOR_3D_HEART_LOAD, OnRestor3dHeartLoad)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_RESTOR_CCFD_ROI_PRESET, OnRestorCcfdRoiPreset)
	ON_BN_CLICKED(IDC_RESTOR_7DLOAD, OnRestorLoad7D)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgRestoration message handlers

void CDlgRestoration::OnCancel() 
{
	((CTextonView*)pView)->RestorationClosed();
	CDialog::OnCancel();
}

void CDlgRestoration::StreamlinesClosed()
{
	ShowWindow(SW_SHOW);
	((CTextonView*)pView)->dlgStrm = NULL;
	pStrm = NULL;
}

/*
void CDlgRestoration::BlurImage(float * input, float * output, int kernel_size)
{
	int x, y;
	double temp;
	int n;

	for(n=0; n<kernel_size; n++)
		for(x=1; x<mriData2D.img_width-1; x++)
			for(y=1; y<mriData2D.img_height-1; y++)
			{
				temp = input[mriData2D.img_width*y+x];
				temp = temp + input[mriData2D.img_width*y+(x-1)];
				temp = temp + input[mriData2D.img_width*y+(x+1)];
				temp = temp + input[mriData2D.img_width*(y+1)+(x-1)];
				temp = temp + input[mriData2D.img_width*(y+1)+(x+1)];
				temp = temp + input[mriData2D.img_width*(y-1)+(x-1)];
				temp = temp + input[mriData2D.img_width*(y-1)+(x+1)];
				temp = temp + input[mriData2D.img_width*(y+1)+x];
				temp = temp + input[mriData2D.img_width*(y-1)+x];
				output[mriData2D.img_width*y+x]=(float)(temp/9.0);
				
			}
}
*/

int CDlgRestoration::FloatToInt(float fnum)
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


UINT RestorInterface( LPVOID pParam )
{
	CDlgRestoration* pDlg = (CDlgRestoration*)pParam;

	CDlgTextBox* pText = new CDlgTextBox;
	BOOL active = TRUE;
	pText->isActive = &active;
	pText->Create(IDD_TEXTOUT, pText);
	pText->ShowWindow(SW_SHOW);

	while(pDlg->bThreadRun == TRUE && pText != NULL)
	{
		while(pDlg->bThreadString!=TRUE){}
		pDlg->bThreadString = FALSE;
		pText->AddText(pDlg->sThreadString);
		pDlg->sThreadString = "";
		pDlg->bThreadString = TRUE;
		
		Sleep(100);
	}

	return 0;
}

#define BORDER 0


void CDlgRestoration::Restor3dIterationProjectVector(LDOUBLE & up, LDOUBLE & vp, 
													 LDOUBLE & wp, LDOUBLE u, 
													 LDOUBLE v, LDOUBLE w)
{
	LDOUBLE magv2, vdotw;

	magv2 = fabs(pow(u,2.0)+pow(v,2.0)+pow(w,2.0));
	vdotw  = up*u + vp*v + wp*w;

	up = up - u * (vdotw/magv2);
	vp = vp - v * (vdotw/magv2);
	wp = wp - w * (vdotw/magv2);

}

void CDlgRestoration::TakeDownPoint(CPoint clickedPoint)
{
	if(!bZoom)
		return;

	CString outCoords;
	outCoords.Format("%i, %i", clickedPoint.x, clickedPoint.y);

	if(pDlgRoi!=NULL)
	{
		((CDlgRestorROI*)pDlgRoi)->TakeRoiText(outCoords);
		outCoords.Format("%i, %i", clickedPoint.x, clickedPoint.y);
		((CMainFrame*)AfxGetMainWnd())->SetStatusBarText(outCoords);
	}

	iRoiDown = clickedPoint;

}

void CDlgRestoration::TakeUpPoint(CPoint clickedPoint)
{
	if(!bZoom)
		return;

	CString outCoords;
	outCoords.Format("%i, %i --> %i, %i (%i, %i)", iRoiDown.x, iRoiDown.y, 
		clickedPoint.x, clickedPoint.y, 
		clickedPoint.x-iRoiDown.x, clickedPoint.y-iRoiDown.y);

	if(pDlgRoi!=NULL)
	{
		((CDlgRestorROI*)pDlgRoi)->TakeRoiText(outCoords);
		outCoords.Format("%i, %i", clickedPoint.x, clickedPoint.y);
		((CMainFrame*)AfxGetMainWnd())->SetStatusBarText(outCoords);
	}

	iRoiUp = clickedPoint;

}

void CDlgRestoration::TakeMovePoint(CPoint clickedPoint)
{
	if(!bZoom)
		return;

	CString outCoords;
	outCoords.Format("%i, %i --> %i, %i (%i, %i)", iRoiDown.x, iRoiDown.y, 
		clickedPoint.x, clickedPoint.y, 
		clickedPoint.x-iRoiDown.x, clickedPoint.y-iRoiDown.y);

	if(pDlgRoi!=NULL)
	{
		((CDlgRestorROI*)pDlgRoi)->TakeRoiText(outCoords);
		outCoords.Format("%i, %i", clickedPoint.x, clickedPoint.y);
		((CMainFrame*)AfxGetMainWnd())->SetStatusBarText(outCoords);
	}

	iRoiUp = clickedPoint;

}

void CDlgRestoration::RoiOK()
{
	UpdateData(TRUE);
	ShowWindow(SW_SHOW);
	m_3DRestorRoiText = ((CDlgRestorROI*)pDlgRoi)->GetText();
	if(((CDlgRestorROI*)pDlgRoi)->m_bUserTyped)
	{
		iRestor2d_xtop = ((CDlgRestorROI*)pDlgRoi)->m_edTopX;
		iRestor2d_ytop = ((CDlgRestorROI*)pDlgRoi)->m_edTopY;
		iRestor2d_xbottom = ((CDlgRestorROI*)pDlgRoi)->m_edBottomX;
		iRestor2d_ybottom = ((CDlgRestorROI*)pDlgRoi)->m_edBottomY;
		m_3DRestorRoiText.Format("%i, %i --> %i, %i (%i, %i)", iRestor2d_xtop, iRestor2d_ytop, 
		iRestor2d_xbottom, iRestor2d_ybottom, 
		iRestor2d_xbottom-iRestor2d_xtop, iRestor2d_ybottom-iRestor2d_ytop);
	}
	else
	{
		// the roi points are now stored in iRoiUp and iRoiDown
		iRestor2d_xtop = iRoiDown.x;
		iRestor2d_ytop = iRoiDown.y;
		iRestor2d_xbottom = iRoiUp.x;
		iRestor2d_ybottom = iRoiUp.y;
	}

	UpdateData(FALSE);
	if(pDlgRoi!=NULL)
	{
		delete pDlgRoi;
		pDlgRoi = NULL;
	}


	((CMainFrame*)AfxGetMainWnd())->SetStatusBarText("");

}

void CDlgRestoration::RoiCancel()
{
	UpdateData(TRUE);
	ShowWindow(SW_SHOW);
	if(pDlgRoi!=NULL)
	{
		delete pDlgRoi;
		pDlgRoi = NULL;
	}

	iRestor2d_xtop = iRestor2d_prevxtop;
	iRestor2d_ytop = iRestor2d_prevytop ;
	iRestor2d_xbottom = iRestor2d_prevxbottom;
	iRestor2d_ybottom = iRestor2d_prevybottom;

	char sz255[100];
	sprintf(sz255, "%i, %i --> %i, %i (%i, %i)", iRestor2d_xtop, iRestor2d_ytop, 
		iRestor2d_xbottom, iRestor2d_ybottom, 
		iRestor2d_xbottom-iRestor2d_xtop, iRestor2d_ybottom-iRestor2d_ytop);
	m_3DRestorRoiText = sz255;
	UpdateData(FALSE);
	((CMainFrame*)AfxGetMainWnd())->SetStatusBarText("");

}

void CDlgRestoration::TakeMouseOverPoint(CPoint clickedPoint)
{
	if(!bZoom)
		return;

	char sz255[100];
	sprintf(sz255, "%i, %i", 
		clickedPoint.x, clickedPoint.y);

	if(pDlgRoi!=NULL)
		((CMainFrame*)AfxGetMainWnd())->SetStatusBarText(sz255);

}

void CDlgRestoration::OnRestor3dvortex() 
{
	UpdateData(TRUE);
	m_3DRestorSliceEnd = m_3dRestorVortexSlices;
	UpdateData(FALSE);

	const int VTXSZ = 255;

	// allocate memory
	mriData3D.Reset();
	mriData3D.mr = new short[VTXSZ*VTXSZ*m_3dRestorVortexSlices];
	mriData3D.u = new short[VTXSZ*VTXSZ*m_3dRestorVortexSlices];
	mriData3D.v = new short[VTXSZ*VTXSZ*m_3dRestorVortexSlices];
	mriData3D.w = new short[VTXSZ*VTXSZ*m_3dRestorVortexSlices];
	mriData3D.img_width = mriData3D.img_height = VTXSZ;
	mriData3D.img_slices = m_3dRestorVortexSlices;
	float lower, half;
	lower = (float)m_3DVortexRange;
	half = lower/2.0f;

	int x, y, z;
	for(x=0; x<mriData3D.img_width; x++)
		for(y=0; y<mriData3D.img_height; y++)
			for(z=0; z<mriData3D.img_slices; z++)
			{
				mriData3D.u[z*VTXSZ*VTXSZ+y*VTXSZ+x]=(short)floor((-lower/255.0)* y + half);
				mriData3D.v[z*VTXSZ*VTXSZ+y*VTXSZ+x]=(short)floor((lower/255.0)* x - half);
				mriData3D.w[z*VTXSZ*VTXSZ+y*VTXSZ+x]=/*(short)floor((lower/255.0)* x - half)*/0;
				mriData3D.mr[z*VTXSZ*VTXSZ+y*VTXSZ+x]=0;
			}

	iRestor2d_xtop =0;
	iRestor2d_ytop =0;
	iRestor2d_xbottom=VTXSZ-1;
	iRestor2d_ybottom=VTXSZ-1;	
	
	m_Restor3dImgSizesTxt.Format("image: %i x %i x %i", mriData3D.img_width, 
		mriData3D.img_height, mriData3D.img_slices);
	UpdateData(FALSE);
	((CButton*)GetDlgItem(IDC_RESTOR_3DVIS_NOISY))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_RESTOR_3DVIS_NOISYDATA))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_RESTOR_3DVIS_RESTORED))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_RESTOR_FIND_VORTEX))->EnableWindow(FALSE);
}

void CDlgRestoration::OnRestor3dvisData() 
{
	Restor3dVis(mriData3D);
}

void CDlgRestoration::OnRestor3dNoiseGo() 
{
	UpdateData(TRUE);

	// generate noise
	float* unoise_img = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
	Noise3D(unoise_img, mriData3D.img_width, mriData3D.img_height, mriData3D.img_slices,
							  m_3DNoiseS, m_3DNoiseM*10.0f, m_3DNoiseV*10.0f);
	float* vnoise_img = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
	Noise3D(vnoise_img, mriData3D.img_width, mriData3D.img_height, mriData3D.img_slices,
							  m_3DNoiseS+1, m_3DNoiseM*10.0f, m_3DNoiseV*10.0f);
	float* wnoise_img = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
	Noise3D(wnoise_img, mriData3D.img_width, mriData3D.img_height, mriData3D.img_slices,
							  m_3DNoiseS+2, m_3DNoiseM*10.0f, m_3DNoiseV*10.0f);

	// add velocities to noise
	int x, y, z, index;
	short* mr = new short[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
	short* u = new short[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
	short* v = new short[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
	short* w = new short[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];

	for(z=0; z<mriData3D.img_slices; z++)
		for(x=0; x<mriData3D.img_width; x++)
			for(y=0; y<mriData3D.img_height; y++)
			{
				index = z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x;
				
				if(m_ZeroSkip)
					if(mriData3D.u[index]==0 && mriData3D.v[index]==0 && mriData3D.w[index]==0)
					{
						u[index] = 0;
						continue;
					}

				// u part
				if(mriData3D.u[index] + unoise_img[index] > 1800)
					u[index] = 1800;
				else
					if(mriData3D.u[index] + unoise_img[index] < -1800)
						u[index] = -1800;
					else
						u[index] = FloatToInt(mriData3D.u[index]
						+ unoise_img[index]);
			}

	for(z=0; z<mriData3D.img_slices; z++)
		for(x=0; x<mriData3D.img_width; x++)
			for(y=0; y<mriData3D.img_height; y++)
			{
				index = z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x;
				
				if(m_ZeroSkip)
					if(mriData3D.u[index]==0 && mriData3D.v[index]==0 && mriData3D.w[index]==0)
					{
						v[index] = 0;
						continue;
					}

				// v part
				if(mriData3D.v[index] + vnoise_img[index] > 1800)
					v[index] = 1800;
				else
					if(mriData3D.v[index] + vnoise_img[index] < -1800)
						v[index] = -1800;
					else
						v[index] = FloatToInt(mriData3D.v[index] 
						+ vnoise_img[index]);
			}
			
	for(z=0; z<mriData3D.img_slices; z++)
		for(x=0; x<mriData3D.img_width; x++)
			for(y=0; y<mriData3D.img_height; y++)
			{
				index = z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x;
				
				if(m_ZeroSkip)
					if(mriData3D.u[index]==0 && mriData3D.v[index]==0 && mriData3D.w[index]==0)
					{
						w[index] = 0;
						continue;
					}

				// w part
				if(mriData3D.w[index] + wnoise_img[index] > 1800)
					w[index] = 1800;
				else
					if(mriData3D.w[index] + wnoise_img[index] < -1800)
						w[index] = -1800;
					else
						w[index] = FloatToInt(mriData3D.w[index]
						+ wnoise_img[index]);
					
							
			}
			
	mriData3DNoisy.SetData(mriData3D.img_width, mriData3D.img_height, mriData3D.img_slices, 
		mriData3D.mr, u, v, w);

	delete [] mr;
	delete [] u;
	delete [] v;
	delete [] w;
	delete [] unoise_img;
	delete [] vnoise_img;
	delete [] wnoise_img;

	((CButton*)GetDlgItem(IDC_RESTOR_3DVIS_NOISY))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_RESTOR_3DVIS_NOISYDATA))->EnableWindow(TRUE);


}

void CDlgRestoration::Noise3D(float * img_data, int img_width, int img_height, int img_slices, int seed, float mean, float variance)
{
	if(img_width == 0 || img_height == 0 || img_slices == 0)
	{
		AfxMessageBox("Load image first.");
		return;
	}

	int x, y, z;
	for(x=0;x<img_width;x++)
		for(y=0;y<img_height;y++)
			for(z=0; z<img_slices;z++)
				img_data[z*img_width*img_height+y*img_width+x] = 0.0f;
		
	float a = 100;
	int i, j, h, amt;
	a = (a < 0) ? 0:a;
	a = (a > 100) ? 100:a;
	amt = (int)FloatToInt(img_height*img_width*img_slices*a/100.0);
	SetGenRand(seed);
	for (int k = 0; k < amt; k++) 
	{
		j = (int)FloatToInt(GenRand(0, img_width-1));
		i = (int)FloatToInt(GenRand(0, img_height-1));
		h = (int)FloatToInt(GenRand(0, img_slices-1));
		img_data[h*img_width*img_height
				+i*img_width+j] = GaussRand(mean, variance);			
	}

}

void CDlgRestoration::OnRestor3dvisNoisy() 
{
	Restor3dVis(mriData3DNoisy);
}

void CDlgRestoration::OnRestor3dRoi() 
{
	UpdateData(TRUE);

	// show data on screen
	if(mriData3D.u == NULL)
		return;
	if(pStrm != NULL)
		delete pStrm;

	CDataMRI* slice = mriData3D.Get2dSlice(m_RestoredSliceNo-1);
	pStrm = new CDlgStreamlines;
	pStrm->pView = pView;
	((CTextonView*)pView)->dlgStrm = pStrm;
	pStrm->Create(IDD_STREAMLS, this);
	pStrm->ShowWindow(SW_HIDE);
	pStrm->NoLoadInitialise(slice);
	pStrm->NoLoadShowMR();
	delete pStrm;
	pStrm = NULL;

	// read by pView to capture clicks
	bZoom = TRUE;

	if(pDlgRoi!=NULL)
	{
		delete pDlgRoi;
		pDlgRoi = NULL;
	}
	CDlgRestorROI* pRoi = new CDlgRestorROI;
	pRoi->pDlgRestor = this;
	pRoi->Create(IDD_REST_ROI, this);
	pRoi->ShowWindow(SW_SHOW);
	ShowWindow(SW_HIDE);
	pDlgRoi=pRoi;
						  
	iRestor2d_prevxtop =iRestor2d_xtop;
	iRestor2d_prevytop =iRestor2d_ytop;
	iRestor2d_prevxbottom=iRestor2d_xbottom;
	iRestor2d_prevybottom=iRestor2d_ybottom;

}

#define LDOUBLE long double

void CDlgRestoration::Restor3dInterface(LDOUBLE lambda, LDOUBLE ctve, LDOUBLE utve, LDOUBLE ctutve, LDOUBLE rms)
{
/*
	sProg.Format("%i", iterations+1);
	m_CtrlRestor3dIts.SetSel(0,-1);
	m_CtrlRestor3dIts.ReplaceSel(sProg);
	pText->AddText("\r\nit " + sProg);
	outf << sProg;
	
	if(!m_Restor3dLambdaFixed)	
	{
		while(bThreadString!=TRUE){}
		bThreadString = FALSE;
		sThreadString = sThreadString + "\r\niteration " + sProg; 
	}
	
	sProg.Format("%.3f", lambda);
	m_CtrlRestor3dLambda.SetSel(0,-1);
	m_CtrlRestor3dLambda.ReplaceSel(sProg);
	outf << "\t" << sProg;
	
	if(!m_Restor3dLambdaFixed) 
	{
		pText->AddText("\r\nlm " + sProg);
		sThreadString = sThreadString + "\r\nlambda = " + sProg;
	}
	
	sProg.Format("%.3f", ctve);
	m_CtrlRestor3dCTVE.SetSel(0,-1);
	m_CtrlRestor3dCTVE.ReplaceSel(sProg);
	pText->AddText("\r\nctve " + sProg);
	outf << "\t" << sProg;
	
	if(!m_Restor3dLambdaFixed) 
	{
		sThreadString = sThreadString + "\r\nctve = " + sProg;
	}
	
	sProg.Format("%.3f", utve);
	m_CtrlRestor3dUTVE.SetSel(0,-1);
	m_CtrlRestor3dUTVE.ReplaceSel(sProg);
	pText->AddText("\r\nutve " + sProg);
	outf << "\t" << sProg;
	
	if(!m_Restor3dLambdaFixed) 
	{
		sThreadString = sThreadString + "\r\nutve = " + sProg;
	}
	
	sProg.Format("%.3f", utve-ctve);
	pText->AddText("\r\nu-c " + sProg);
	outf << "\t" << sProg;
	
	if(!m_Restor3dLambdaFixed)	
	{
		sThreadString = sThreadString + "\r\nutve-ctve " + sProg; 
		bThreadString = TRUE;
	}
	
	sProg.Format("%.3f", Restor3dIterationRMS(u, v, w, mriData3D.u, 
		mriData3D.v, mriData3D.w, magn_map));
	m_Restor3dRMS.SetSel(0,-1);
	m_Restor3dRMS.ReplaceSel(sProg);
	pText->AddText("\r\nrms " + sProg);
	outf << "\t" << sProg << "\n";
	sThreadString = sThreadString + "\r\nrms = " + sProg;
*/
}

void CDlgRestoration::OnRestor3dgo() 
{
	UpdateData(TRUE);

	if(m_3DRestorSliceEnd-m_RestoredSliceNo < 0)
	{
		AfxMessageBox("Please enter an appropriate slice range.");
		return;
	}

	/*
	if(!m_Restor3dLambdaFixed)
	{
		bThreadRun = TRUE;
		CWinThread* pThread = AfxBeginThread(RestorInterface, this);
		bThreadString = FALSE;
		sThreadString = "Starting...";
		bThreadString = TRUE;
	}
	*/

	/*CDlgTextBox* pText = new CDlgTextBox;
	BOOL active = TRUE;
	pText->isActive = &active;
	pText->Create(IDD_TEXTOUT, this);
	pText->ShowWindow(SW_HIDE);*/


	LDOUBLE* u = new LDOUBLE[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
	LDOUBLE* v = new LDOUBLE[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
	LDOUBLE* w = new LDOUBLE[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
	LDOUBLE* unoisy = new LDOUBLE[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
	LDOUBLE* vnoisy = new LDOUBLE[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
	LDOUBLE* wnoisy = new LDOUBLE[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
	LDOUBLE* magn_map = new LDOUBLE[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];

	CString fname;
	fname.Format("c:\\test%i_%.4f_%.1f.txt", m_3DRestorIterations, m_3DRestorSigma, m_3DRestorLambda);
	ofstream outf;
	outf.open(fname);

	int x, y, z, index;
	LDOUBLE v_magn;
	for(z=0; z<mriData3D.img_slices; z++)
		for(x=0; x<mriData3D.img_width; x++)
			for(y=0; y<mriData3D.img_height; y++)
			{
				index = mriData3D.img_width*mriData3D.img_height*z +
					mriData3D.img_width*y + x;
				
				magn_map[index] =
					sqrt(pow(mriData3DNoisy.u[index],2)+
					pow(mriData3DNoisy.v[index],2)+
					pow(mriData3DNoisy.w[index],2));

				if(magn_map[index] < 0.0001)  // added 20/11/02
					magn_map[index] = 0.0;
				
				unoisy[index] = u[index] = 
					magn_map[index]>0.0?
					((LDOUBLE)mriData3DNoisy.u[index])
					/ magn_map[index] : 0.0;
				
				vnoisy[index] = v[index] = 
					magn_map[index]>0.0?
					((LDOUBLE)mriData3DNoisy.v[index]) 
					/ magn_map[index] : 0.0;
				
				wnoisy[index] = w[index] = 
					magn_map[index]>0.0?
					((LDOUBLE)mriData3DNoisy.w[index]) 
					/ magn_map[index] : 0.0;

			}

	const LDOUBLE sigmaSquared = (LDOUBLE)pow(m_3DRestorSigma, 2.0);
	const int bigomega = abs(iRestor2d_xbottom-iRestor2d_xtop)
		*abs(iRestor2d_ybottom-iRestor2d_ytop)
		*abs(m_3DRestorSliceEnd-m_RestoredSliceNo+1);
	const LDOUBLE ht_rterm = sigmaSquared * ((LDOUBLE)bigomega);
	const LDOUBLE deltat = m_3DRestorDeltaT;

	LDOUBLE* ubigpi_map = new LDOUBLE[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
	LDOUBLE* vbigpi_map = new LDOUBLE[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
	LDOUBLE* wbigpi_map = new LDOUBLE[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
	LDOUBLE* uwt_map = new LDOUBLE[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
	LDOUBLE* vwt_map = new LDOUBLE[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
	LDOUBLE* wwt_map = new LDOUBLE[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
	LDOUBLE* e_map = new LDOUBLE[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];

	int iterations;
	float cur_error;
	int mindiff_times = 0;
	const int mindiff_times_pass = 100;
	LDOUBLE hu_current, lambda, prev_lambda, utve=0, ctve=0;
	LDOUBLE prev_energy = FLT_MAX;
	m_3DRestorMinEnergyDiff = 0.001;
	lambda = m_3DRestorLambda;

	double errorTest1 = Restor3dIterationRMS(mriData3DNoisy.u, mriData3DNoisy.v, mriData3DNoisy.w, 
		mriData3D.u, mriData3D.v, mriData3D.w);

	double errorTest2 = Restor3dIterationRMS(u, v, w, mriData3D.u, mriData3D.v, mriData3D.w);

	outf << "IT_NO\tLAMBDA\tCTVE\tUTVE\tCT-UT\tRMS\n" ;
	for(iterations = 0; iterations < m_3DRestorIterations; iterations++)
	{

		if(!_finite(lambda))
			break;

		prev_lambda = lambda;

		// u(n+1) = u(n) + deltat * bigpi( sum(wt) + lambda*u(n) )
		Restor3dIterationE_Map(u, v, w, e_map);
		Restor3dIterationWtMap(u, v, w, uwt_map, vwt_map, wwt_map, e_map);
		Restor3dIterationBigPi(ubigpi_map, vbigpi_map, wbigpi_map, unoisy, vnoisy, 
			wnoisy, uwt_map, vwt_map, wwt_map, lambda, u, v, w);

		// lambda = lambda + deltat * 0.5 * hu_current;
		hu_current = Restor3dIterationHU(u, v, w, unoisy, 
			vnoisy, wnoisy, ht_rterm);
		if(!m_Restor3dLambdaFixed)
			lambda = lambda + deltat * 0.5 * hu_current;

		utve = Restor3dIterationUTVE(u, v, w, e_map);
		ctve = utve + 0.5 * lambda * hu_current;

		cur_error = Restor3dIterationRMS(u, v, w, mriData3D.u, mriData3D.v, mriData3D.w);
		outf << iterations+1 << "\t" << lambda << "\t"; 		
		outf << ctve << "\t" << utve << "\t" << utve-ctve << "\t"  
			<< cur_error
			<< "\t" << "\n";

		outf.flush();

		for(z=m_RestoredSliceNo-1; z<m_3DRestorSliceEnd; z++)
			for(x=iRestor2d_xtop; x<=iRestor2d_xbottom; x++)
				for(y=iRestor2d_ytop; y<=iRestor2d_ybottom; y++)
				{
					index = mriData3D.img_width*mriData3D.img_height*z +
						mriData3D.img_width*y + x;

					if(_finite(ubigpi_map[index]))
						u[index] = u[index] + deltat * ubigpi_map[index];
					else
						u[index] = u[index];
					
					if(_finite(vbigpi_map[index]))
						v[index] = v[index] + deltat * vbigpi_map[index];
					else
						v[index] = v[index];
					
					if(_finite(wbigpi_map[index]))
						w[index] = w[index] + deltat * wbigpi_map[index];
					else
						w[index] = w[index];

					v_magn = sqrt(pow(u[index],2)+
						pow(v[index],2)+
						pow(w[index],2));
					
					u[index] = v_magn>0.0? u[index] / v_magn : 0.0;
					v[index] = v_magn>0.0? v[index] / v_magn : 0.0;
					w[index] = v_magn>0.0? w[index] / v_magn : 0.0;

				}
				
		if(	!m_Restor3dLambdaFixed && fabs(lambda - prev_lambda) < m_3DRestorMinLambdaDiff)
		{
			mindiff_times++;
			if(mindiff_times > mindiff_times_pass)
				break;
		}
		else
			mindiff_times = 0;

		if(m_Restor3dLambdaFixed && fabs(utve - prev_energy) < m_3DRestorMinEnergyDiff)
			break;
		else
			prev_energy = utve;
		
	}

	outf.close();

	short* su = new short[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
	short* sv = new short[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
	short* sw = new short[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
	
	for(z=0; z<mriData3D.img_slices; z++)
		for(x=0; x<mriData3D.img_width; x++)
			for(y=0; y<mriData3D.img_height; y++)
			{
				index = mriData3D.img_width*mriData3D.img_height*z +
					mriData3D.img_width*y + x;				
				su[index] = (short)FloatToInt(u[index]
					* magn_map[index]);//-mriData3D.u[index];
				sv[index] = (short)FloatToInt(v[index]
					* magn_map[index]);//-mriData3D.v[index];
				sw[index] = (short)FloatToInt(w[index]
					* magn_map[index]);//-mriData3D.w[index];
			}

	mriData3DRestored.SetData(mriData3D.img_width, mriData3D.img_height, 
		mriData3D.img_slices, mriData3D.mr, su, sv, sw);

	CString smsgout;
	smsgout.Format("Iterations finished. Error = %f Lambda = %f", Restor3dIterationRMS(mriData3DRestored.u, 
		mriData3DRestored.v, mriData3DRestored.w, mriData3D.u, mriData3D.v, mriData3D.w), lambda);
//	AfxMessageBox(smsgout);
	m_3DRestorLambdaFinal = (double)lambda;

	delete [] u;
	delete [] v;
	delete [] w;
	delete [] unoisy;
	delete [] vnoisy;
	delete [] wnoisy;
	delete [] magn_map;
	delete [] su;
	delete [] sv;
	delete [] sw;
	delete [] ubigpi_map;
	delete [] vbigpi_map;
	delete [] wbigpi_map;
	delete [] uwt_map;
	delete [] vwt_map;
	delete [] wwt_map;
	delete [] e_map;

	// bThreadRun = FALSE;
	// pText->ShowWindow(SW_SHOW);

	((CButton*)GetDlgItem(IDC_RESTOR_3DVIS_RESTORED))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_RESTOR_FIND_VORTEX))->EnableWindow(TRUE);


}

void CDlgRestoration::Restor3dGetNeighs(CPoint3D* points, int* neigh_count, int img_width, int img_height,
					   int img_slices, int curx, int cury, int curz)
{
	int neighs = 0;

	int x, y, z;

	for(x=curx-1;x<=curx+1;x++)
		for(y=cury-1;y<=cury+1;y++)
			for(z=curz-1;z<=curz+1;z++)
				if(x>=0 && y>=0 && z>=0 && x<img_width && y<img_height && z<img_slices
					&& neighs < 27)
				{
					points[neighs].x = x;
					points[neighs].y = y;
					points[neighs].z = z;
					neighs++;
				}

	*neigh_count = neighs;

}

void CDlgRestoration::Restor3dIterationE_Map(LDOUBLE* u, LDOUBLE* v, LDOUBLE* w, LDOUBLE* e_map)
{
	// e(a) = sqr( sum( ed(a, neigh) ) )
	int neighs = 27;
	CPoint3D* neighPoints = new CPoint3D[neighs];
	int x, y, z, index, nindex, ni;
	LDOUBLE e_temp = 0.0f;

	for(z=m_RestoredSliceNo-1; z<m_3DRestorSliceEnd; z++)
		for(x=iRestor2d_xtop; x<=iRestor2d_xbottom; x++)
			for(y=iRestor2d_ytop; y<=iRestor2d_ybottom; y++)
			{
				index = mriData3D.img_width*mriData3D.img_height*z +
					mriData3D.img_width*y + x;				
				e_temp = 0.0f;
				Restor3dGetNeighs(neighPoints, &neighs, mriData3D.img_width, 
					mriData3D.img_height, mriData3D.img_slices, x, y, z);
				for(ni=0; ni<neighs; ni++)
				{	
					nindex = mriData3D.img_width*mriData3D.img_height*neighPoints[ni].z +
						mriData3D.img_width*neighPoints[ni].y + neighPoints[ni].x;
					
					e_temp = e_temp + ((LDOUBLE)(pow(u[nindex]-u[index],2.0) + pow(v[nindex]
						-v[index],2.0) + pow(w[nindex]-w[index],2.0)));
					
				}
				if(_finite(e_map[index]))
					e_map[index] = sqrt(e_temp) < 0.001 ? 0.001 : sqrt(e_temp);
				else
					e_map[index] = 1.0;

			}

}

void CDlgRestoration::Restor3dIterationWtMap(LDOUBLE* u, LDOUBLE* v, LDOUBLE* w, LDOUBLE* uwt_map, 
							LDOUBLE* vwt_map, LDOUBLE* wwt_map, LDOUBLE* e_map)
{
	// sum( wt(a,b) * u(b) )
	int neighs = 27;
	CPoint3D* neighPoints = new CPoint3D[neighs];
	int x, y, z, index, nindex, ni;
	LDOUBLE uwt_temp = 0.0, vwt_temp = 0.0, wwt_temp = 0.0;
	for(z=m_RestoredSliceNo-1; z<m_3DRestorSliceEnd; z++)
		for(x=iRestor2d_xtop; x<=iRestor2d_xbottom; x++)
			for(y=iRestor2d_ytop; y<=iRestor2d_ybottom; y++)
			{
				index = mriData3D.img_width*mriData3D.img_height*z +
					mriData3D.img_width*y + x;
				uwt_temp = vwt_temp = wwt_temp = 0.0f;
				Restor3dGetNeighs(neighPoints, &neighs, mriData3D.img_width, 
					mriData3D.img_height, mriData3D.img_slices, x, y, z);
				for(ni=0; ni<neighs; ni++)
				{
					nindex = mriData3D.img_width*mriData3D.img_height*neighPoints[ni].z +
						mriData3D.img_width*neighPoints[ni].y + neighPoints[ni].x;
					
					uwt_temp = uwt_temp + 
						u[nindex] * (1.0/e_map[index] + 1.0/e_map[nindex])/* / 2.0*/;
					
					vwt_temp = vwt_temp + 
						v[nindex] * (1.0/e_map[index] + 1.0/e_map[nindex])/* / 2.0*/;
					
					wwt_temp = wwt_temp + 
						w[nindex] * (1.0/e_map[index] + 1.0/e_map[nindex])/* / 2.0*/;
				}
				
				uwt_map[index] = uwt_temp;
				vwt_map[index] = vwt_temp;
				wwt_map[index] = wwt_temp;
			}

}

void CDlgRestoration::Restor3dIterationBigPi(LDOUBLE* ubigpi, LDOUBLE* vbigpi, LDOUBLE* wbigpi, LDOUBLE* unoisy, 
							LDOUBLE* vnoisy, LDOUBLE* wnoisy, LDOUBLE* uwt_map, LDOUBLE* vwt_map, 
							LDOUBLE* wwt_map, LDOUBLE lambda, LDOUBLE* u, LDOUBLE* v, LDOUBLE* w)	
{
	int x, y, z, index;
	for(z=m_RestoredSliceNo-1; z<m_3DRestorSliceEnd; z++)
		for(x=iRestor2d_xtop; x<=iRestor2d_xbottom; x++)
			for(y=iRestor2d_ytop; y<=iRestor2d_ybottom; y++)
			{
				index = mriData3D.img_width*mriData3D.img_height*z +
					mriData3D.img_width*y + x;

				ubigpi[index] = uwt_map[index] + lambda * unoisy[index];
				
				vbigpi[index] = vwt_map[index] + lambda * vnoisy[index];
				
				wbigpi[index] = wwt_map[index] + lambda * wnoisy[index];
				
				Restor3dIterationProjectVector(ubigpi[index],
					vbigpi[index], wbigpi[index], u[index], v[index], w[index]);
				
			}
}

LDOUBLE CDlgRestoration::Restor3dIterationHU(LDOUBLE* u, LDOUBLE* v, LDOUBLE* w, LDOUBLE* noisyu, LDOUBLE* noisyv, 
						  LDOUBLE* noisyw, LDOUBLE ht_rterm)
{
	// ( sum( dl^2(u, u0) ) - ht_rterm)
	int x, y, z, index;
	LDOUBLE ht_temp = 0.0f;
	for(z=m_RestoredSliceNo-1; z<m_3DRestorSliceEnd; z++)
		for(x=iRestor2d_xtop; x<=iRestor2d_xbottom; x++)
			for(y=iRestor2d_ytop; y<=iRestor2d_ybottom; y++)
			{
				index = mriData3D.img_width*mriData3D.img_height*z +
					mriData3D.img_width*y + x;

				ht_temp = ht_temp + (LDOUBLE)(pow(u[index]
					-noisyu[index],2) +
					pow(v[index]
					-noisyv[index],2) +
					pow(w[index]
					-noisyw[index],2));
		
				if(!_finite(ht_temp))
					ht_temp = 1.0;
					
			}
			
			return ht_temp-ht_rterm;
}

LDOUBLE CDlgRestoration::Restor3dIterationUTVE(LDOUBLE* u, LDOUBLE* v, LDOUBLE* w, LDOUBLE* e_map)
{
	// e = sum_image( sqrt( sum_neighs( edist(ua, ub) ) ) )
	//   = sum_image( e_map )

	int x, y, z;
	LDOUBLE e_temp = 0.0;

	for(z=m_RestoredSliceNo-1; z<m_3DRestorSliceEnd; z++)
		for(x=iRestor2d_xtop; x<=iRestor2d_xbottom; x++)
			for(y=iRestor2d_ytop; y<=iRestor2d_ybottom; y++)
				e_temp = e_temp + e_map[mriData3D.img_width*mriData3D.img_height*z +
					mriData3D.img_width*y + x];

	return e_temp;
}

void CDlgRestoration::OnRestor3dvisRestored() 
{
	Restor3dVis(mriData3DRestored);	
}

void CDlgRestoration::NormaliseInt(int * in_out, int width, int height)
{
	int min;
	int max;
	int x, y;

	min = max = in_out[0];

	for(x=0; x<width; x++)
		for(y=0; y<height; y++)
		{
			if(in_out[width*y+x]>max)
				max = in_out[width*y+x];

			if(in_out[width*y+x]<min)
				min = in_out[width*y+x];
		}

	int range = max - min;
	LDOUBLE m, b;
	m=255.0/range;
	b=255.0-m*(LDOUBLE)max;

	for(x=0; x<width; x++)
		for(y=0; y<height; y++)
			in_out[width*y+x]=FloatToInt( in_out[width*y+x]*m+b+0.00001 );

}

void CDlgRestoration::OnRestor3dload() 
{
	// prompt user for path and load file
	CFileDialog* dDlg = new CFileDialog(FALSE,"*","mr001",OFN_CREATEPROMPT |
		OFN_HIDEREADONLY | OFN_NOREADONLYRETURN,
		_T("MRI CFD Data (*.*)|*.*||"), NULL);

	if(dDlg->DoModal()!=IDOK)
		return;

	mriData3D.LoadDataCFD(dDlg->GetPathName());
	UpdateData(TRUE);
	m_3DRestorSliceEnd = mriData3D.img_slices;
	UpdateData(FALSE);
	delete dDlg;
	
	iRestor2d_xtop = 0;
	iRestor2d_ytop = 0;
	iRestor2d_xbottom = mriData3D.img_width-1;
	iRestor2d_ybottom = mriData3D.img_height-1;

	m_Restor3dImgSizesTxt.Format("image: (x, y, s) %i x %i x %i", mriData3D.img_width, 
		mriData3D.img_height, mriData3D.img_slices);
	m_3DRestorRoiText.Format("%i, %i --> %i, %i (%i, %i)", iRestor2d_xtop, iRestor2d_ytop, 
		iRestor2d_xbottom, iRestor2d_ybottom, 
		iRestor2d_xbottom-iRestor2d_xtop, iRestor2d_ybottom-iRestor2d_ytop);
	UpdateData(FALSE);
	((CButton*)GetDlgItem(IDC_RESTOR_3DVIS_NOISY))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_RESTOR_3DVIS_NOISYDATA))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_RESTOR_3DVIS_RESTORED))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_RESTOR_FIND_VORTEX))->EnableWindow(FALSE);

}

double CDlgRestoration::Restor3dIterationRMS(short* u, short* v, 
											 short* w, short* ori_u,
											 short* ori_v, short* ori_w)
{
	UpdateData(TRUE);

	// calculate RMS error
	int x, y, z, index, img_size;
	LDOUBLE mean, mag_rest, mag_ori, un, vn, wn, uon, von, won;

	img_size = abs(m_3DRestorSliceEnd-m_RestoredSliceNo+1)*abs(iRestor2d_xtop-iRestor2d_xbottom+1)
		*abs(iRestor2d_ytop-iRestor2d_ybottom+1);
	
	mean = 0.0;
	for(z=m_RestoredSliceNo-1; z<m_3DRestorSliceEnd; z++)
		for(x=iRestor2d_xtop; x<=iRestor2d_xbottom; x++)
			for(y=iRestor2d_ytop; y<=iRestor2d_ybottom; y++)
			{
				index = mriData3D.img_width*mriData3D.img_height*z +
					mriData3D.img_width*y + x;

				mag_rest = sqrt(pow(u[index],2)+pow(v[index],2)+pow(w[index],2));
				mag_ori = sqrt(pow(ori_u[index],2)+pow(ori_v[index],2)+pow(ori_w[index],2));

				if(m_ZeroSkip && mag_ori == 0)
					continue;

				// mag_rest = 1.0;

				if(mag_rest > 0.0)
				{
					un = ((double)u[index])/mag_rest;
					vn = ((double)v[index])/mag_rest;
					wn = ((double)w[index])/mag_rest;
				}
				else
					un = vn = wn = 0.0;

				// mag_ori = 1.0;

				if(mag_ori > 0.0)
				{
					uon = ((LDOUBLE)ori_u[index])/mag_ori;
					von = ((LDOUBLE)ori_v[index])/mag_ori;
					won = ((LDOUBLE)ori_w[index])/mag_ori;
				}
				else
					uon = von = won = 0.0;
			
				mean = mean + (pow(un-uon,2.0)+pow(vn-von,2.0)+pow(wn-won,2.0)) / (LDOUBLE)img_size;
			}

	return sqrt(mean);

}

double CDlgRestoration::Restor3dIterationRMS(LDOUBLE* u, LDOUBLE* v, 
											 LDOUBLE* w, short* ori_u,
											 short* ori_v, short* ori_w)
{
	// calculate RMS error
	int x, y, z, index, img_size;
	LDOUBLE mean, mag_rest, mag_ori, un, vn, wn, uon, von, won;

	img_size = abs(m_3DRestorSliceEnd-m_RestoredSliceNo+1)*abs(iRestor2d_xtop-iRestor2d_xbottom+1)
		*abs(iRestor2d_ytop-iRestor2d_ybottom+1);
	
	mean = 0.0;
	for(z=m_RestoredSliceNo-1; z<m_3DRestorSliceEnd; z++)
		for(x=iRestor2d_xtop; x<=iRestor2d_xbottom; x++)
			for(y=iRestor2d_ytop; y<=iRestor2d_ybottom; y++)
			{
				index = mriData3D.img_width*mriData3D.img_height*z +
					mriData3D.img_width*y + x;

				mag_rest = sqrt(pow(u[index],2)+pow(v[index],2)+pow(w[index],2));
				mag_ori = sqrt(pow(ori_u[index],2)+pow(ori_v[index],2)+pow(ori_w[index],2));

				// mag_rest = 1.0;

				if(mag_rest > 0.0)
				{
					un = ((double)u[index])/mag_rest;
					vn = ((double)v[index])/mag_rest;
					wn = ((double)w[index])/mag_rest;
				}
				else
					un = vn = wn = 0.0;

				// mag_ori = 1.0;

				if(mag_ori > 0.0)
				{
					uon = ((LDOUBLE)ori_u[index])/mag_ori;
					von = ((LDOUBLE)ori_v[index])/mag_ori;
					won = ((LDOUBLE)ori_w[index])/mag_ori;
				}
				else
					uon = von = won = 0.0;
			
				mean = mean + (pow(un-uon,2.0)+pow(vn-von,2.0)+pow(wn-won,2.0)) / (LDOUBLE)img_size;
			}

	return sqrt(mean);

}

void CDlgRestoration::OnRestor3dloadFrame() 
{
	// prompt user for path and load file
	CFileDialog* dDlg = new CFileDialog(TRUE,"sur","mr001",OFN_CREATEPROMPT |
		OFN_HIDEREADONLY | OFN_NOREADONLYRETURN,
		_T("MRI Data (*.sur)|*.sur||"), NULL);

	if(dDlg->DoModal()!=IDOK)
		return;

	mriData3D.LoadDataFrame(dDlg->GetPathName());
	UpdateData(TRUE);
	m_3DRestorSliceEnd = mriData3D.img_slices;
	UpdateData(FALSE);
	delete dDlg;
	
	iRestor2d_xtop = 0;
	iRestor2d_ytop = 0;
	iRestor2d_xbottom = mriData3D.img_width-1;
	iRestor2d_ybottom = mriData3D.img_height-1;

	m_Restor3dImgSizesTxt.Format("image: %i x %i x %i", mriData3D.img_width, 
		mriData3D.img_height, mriData3D.img_slices);
	m_3DRestorRoiText.Format("%i, %i --> %i, %i (%i, %i)", iRestor2d_xtop, iRestor2d_ytop, 
		iRestor2d_xbottom, iRestor2d_ybottom, 
		iRestor2d_xbottom-iRestor2d_xtop, iRestor2d_ybottom-iRestor2d_ytop);
	UpdateData(FALSE);

	((CButton*)GetDlgItem(IDC_RESTOR_3DVIS_NOISY))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_RESTOR_3DVIS_NOISYDATA))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_RESTOR_3DVIS_RESTORED))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_RESTOR_FIND_VORTEX))->EnableWindow(FALSE);

	
}
/*
void CDlgRestoration::OnRestor3dErrorComp() 
{
	CData3DMRI mriData2nd;

	// load a restored image
	// prompt user for path and load file
	CFileDialog* dDlg = new CFileDialog(TRUE,"*","mr001",OFN_CREATEPROMPT |
		OFN_HIDEREADONLY | OFN_NOREADONLYRETURN,
		_T("MRI CFD Data (*.*)|*.*||"), NULL);

	if(dDlg->DoModal()!=IDOK)
		return;

	mriData2nd.LoadDataFrame(dDlg->GetPathName());
	UpdateData(TRUE);
	delete dDlg;

	// work out error between loaded and restored image
	CString sms;
	sms.Format("%f", Restor3dIterationRMS(mriData2nd.u, mriData2nd.v, mriData2nd.w, 
		mriData3DRestored.u, mriData3DRestored.v, mriData3DRestored.w));
	AfxMessageBox(sms);

	CDataMRI* slice = mriData2nd.Get2dSlice(m_RestoredSliceNo-1);
	pStrm = new CDlgStreamlines;
	pStrm->pView = pView;
	((CTextonView*)pView)->dlgStrm = pStrm;
	pStrm->Create(IDD_STREAMLS, this);
	pStrm->ShowWindow(SW_HIDE);
	pStrm->NoLoadInitialise(slice);
	pStrm->NoLoadShowMR();
	delete pStrm;
	pStrm = NULL;

}
*/

/*
void CDlgRestoration::On3dError3dcomp() 
{
	CData3DMRI mriData2nd;

	// load a restored image
	// prompt user for path and load file
	CFileDialog* dDlg = new CFileDialog(TRUE,"*","mr001",OFN_CREATEPROMPT |
		OFN_HIDEREADONLY | OFN_NOREADONLYRETURN,
		_T("MRI CFD Data (*.*)|*.*||"), NULL);

	if(dDlg->DoModal()!=IDOK)
		return;

	mriData2nd.LoadDataCFD(dDlg->GetPathName());
	UpdateData(TRUE);
	delete dDlg;
	
	// work out error between loaded and restored image
	CString sms;
	sms.Format("%f", Restor3dIterationRMS(mriData2nd.u, mriData2nd.v, mriData2nd.w, 
		mriData3DRestored.u, mriData3DRestored.v, mriData3DRestored.w));
	AfxMessageBox(sms);

	CDataMRI* slice = mriData2nd.Get2dSlice(m_RestoredSliceNo-1);
	pStrm = new CDlgStreamlines;
	pStrm->pView = pView;
	((CTextonView*)pView)->dlgStrm = pStrm;
	pStrm->Create(IDD_STREAMLS, this);
	pStrm->ShowWindow(SW_HIDE);
	pStrm->NoLoadInitialise(slice);
	pStrm->NoLoadShowMR();
	delete pStrm;
	pStrm = NULL;

	
}
*/
/*
void CDlgRestoration::On3dError3dcompMem() 
{
	
	// work out error between original and restored image
	CString sms;
	sms.Format("%f", Restor3dIterationRMS(mriData3DRestored.u, mriData3DRestored.v, mriData3DRestored.w, 
		mriData3D.u, mriData3D.v, mriData3D.w));
	AfxMessageBox(sms);

	CDataMRI* slice = mriData3DRestored.Get2dSlice(m_RestoredSliceNo-1);
	pStrm = new CDlgStreamlines;
	pStrm->pView = pView;
	((CTextonView*)pView)->dlgStrm = pStrm;
	pStrm->Create(IDD_STREAMLS, this);
	pStrm->ShowWindow(SW_HIDE);
	pStrm->NoLoadInitialise(slice);
	pStrm->NoLoadShowMR();
	delete pStrm;
	pStrm = NULL;

	
}
*/
void CDlgRestoration::Restor3dVis(CData3DMRI &source)
{
	if(pStrm != NULL)
		delete pStrm;

	if(m_RestoredSliceNo > mriData3D.img_slices || m_RestoredSliceNo < 1)
		return;

	UpdateData(TRUE);
	CData3DMRI tempScaled;
	tempScaled.SetData(source.img_width, source.img_height, source.img_slices,
		source.mr, source.u, source.v, source.w);
	tempScaled.Zoom(CPoint(iRestor2d_xtop, iRestor2d_ytop), CPoint(iRestor2d_xbottom, iRestor2d_ybottom),
		m_RestoredSliceNo-1, m_3DRestorSliceEnd-1, m_fZoomScale);
	CDlgZoomedSlice dlgSlice;
	dlgSlice.m_psImgSize.Format("Image (s,x,y): %i x %i x %i", tempScaled.img_slices, tempScaled.img_width,
		tempScaled.img_height);
if(dlgSlice.DoModal()==IDCANCEL)
		return;
	CDataMRI* slice;
	BOOL data3D = FALSE;

	switch(dlgSlice.m_dirSelect)
	{
		case 0:
			return;
		case 1:
			slice = tempScaled.Get2dSlice(dlgSlice.m_z-1);
			break;
		case 2:
			slice = tempScaled.Get2dXSlice(dlgSlice.m_x);
			break;
		case 3:
			slice = tempScaled.Get2dYSlice(dlgSlice.m_y);
			break;
		case 4:
			data3D = TRUE;
			slice = NULL;
		default:
			break;
	}

	if(slice==NULL && data3D == FALSE)
	{
		AfxMessageBox("Internal error: NULL projection slice.");
		return;
	}

	pStrm = new CDlgStreamlines;
	pStrm->pView = pView;
	((CTextonView*)pView)->dlgStrm = pStrm;
	pStrm->Create(IDD_STREAMLS, this);
	if(data3D == FALSE)
		pStrm->NoLoadInitialise(slice);
	else
		pStrm->NoLoadInitialise(&tempScaled);
	pStrm->ShowWindow(SW_SHOW);
	ShowWindow(SW_HIDE);
	delete slice;	
}

void CDlgRestoration::RoiWhole()
{
	iRestor2d_xtop = 0;
	iRestor2d_ytop = 0;
	iRestor2d_xbottom = mriData3D.img_width-1;
	iRestor2d_ybottom = mriData3D.img_height-1;
	ShowWindow(SW_SHOW);
	UpdateData(TRUE);
	m_3DRestorRoiText.Format("%i, %i --> %i, %i (%i, %i)", iRestor2d_xtop, iRestor2d_ytop, 
		iRestor2d_xbottom, iRestor2d_ybottom, 
		iRestor2d_xbottom-iRestor2d_xtop, iRestor2d_ybottom-iRestor2d_ytop);
	UpdateData(FALSE);

}

void CDlgRestoration::OnRestorFindVortex() 
{
	if(m_RestoredSliceNo > mriData3D.img_slices || m_RestoredSliceNo < 1)
		return;

	UpdateData(TRUE);
	CData3DMRI tempScaled;
	tempScaled.SetData(mriData3DRestored.img_width, mriData3DRestored.img_height, mriData3DRestored.img_slices,
		mriData3DRestored.mr, mriData3DRestored.u, mriData3DRestored.v, mriData3DRestored.w);
	tempScaled.Zoom(CPoint(iRestor2d_xtop, iRestor2d_ytop), CPoint(iRestor2d_xbottom, iRestor2d_ybottom),
		m_RestoredSliceNo-1, m_3DRestorSliceEnd-1, m_fZoomScale);

	CDlgFindVortex* pDlgVortex;
	pDlgVortex = new CDlgFindVortex;
	pDlgVortex->pView = pView;
	((CTextonView*)pView)->dlgVortexFinder = pDlgVortex;
	pDlgVortex->Create(IDD_FINDVORTEX, this);
	pDlgVortex->NoLoadInitialise(&tempScaled);
	pDlgVortex->ShowWindow(SW_SHOW);

}

void CDlgRestoration::OnRestor3dvisData2() 
{
	UpdateData(TRUE);
	CData3DMRI tempScaled;
	if(mriData3D.img_width>0 && mriData3D.img_height>0 && mriData3D.img_slices>0)
	{
		tempScaled.SetData(mriData3D.img_width, mriData3D.img_height, mriData3D.img_slices,
			mriData3D.mr, mriData3D.u, mriData3D.v, mriData3D.w);
		tempScaled.Zoom(CPoint(iRestor2d_xtop, iRestor2d_ytop), CPoint(iRestor2d_xbottom, iRestor2d_ybottom),
			m_RestoredSliceNo-1, m_3DRestorSliceEnd-1, m_fZoomScale);
	}

	CDlgFindVortex* pDlgVortex;
	pDlgVortex = new CDlgFindVortex;
	pDlgVortex->pView = pView;
	((CTextonView*)pView)->dlgVortexFinder = pDlgVortex;
	pDlgVortex->Create(IDD_FINDVORTEX, this);
	pDlgVortex->NoLoadInitialise(&tempScaled);
	pDlgVortex->ShowWindow(SW_SHOW);
	
}

void CDlgRestoration::OnRestor3dvisNoisydata() 
{
	UpdateData(TRUE);
	CData3DMRI tempScaled;
	tempScaled.SetData(mriData3D.img_width, mriData3D.img_height, mriData3D.img_slices,
		mriData3DNoisy.mr, mriData3DNoisy.u, mriData3DNoisy.v, mriData3DNoisy.w);
	tempScaled.Zoom(CPoint(iRestor2d_xtop, iRestor2d_ytop), CPoint(iRestor2d_xbottom, iRestor2d_ybottom),
		m_RestoredSliceNo-1, m_3DRestorSliceEnd-1, m_fZoomScale);

	CDlgFindVortex* pDlgVortex;
	pDlgVortex = new CDlgFindVortex;
	pDlgVortex->pView = pView;
	((CTextonView*)pView)->dlgVortexFinder = pDlgVortex;
	pDlgVortex->Create(IDD_FINDVORTEX, this);
	pDlgVortex->NoLoadInitialise(&tempScaled);
	pDlgVortex->ShowWindow(SW_SHOW);
	
}

void CDlgRestoration::OnRestor3dHeartLoad() 
{
	// prompt user for path and load file
	CFileDialog* dDlg = new CFileDialog(TRUE,"*","a001",OFN_CREATEPROMPT |
		OFN_HIDEREADONLY | OFN_NOREADONLYRETURN,
		_T("CFD position data (*.geo)|*.geo|All Files (*.*)|*.*||"), NULL);

	if(dDlg->DoModal()!=IDOK)
		return;

	UpdateData(TRUE);

	mriData3D.LoadDataHeartCFDTexton(dDlg->GetPathName());
	m_3DRestorSliceEnd = mriData3D.img_slices;
	UpdateData(FALSE);
	delete dDlg;
	
	iRestor2d_xtop = 0;
	iRestor2d_ytop = 0;
	iRestor2d_xbottom = mriData3D.img_width-1;
	iRestor2d_ybottom = mriData3D.img_height-1;

	m_Restor3dImgSizesTxt.Format("image: (x, y, s) %i x %i x %i", mriData3D.img_width, 
		mriData3D.img_height, mriData3D.img_slices);
	m_3DRestorRoiText.Format("%i, %i --> %i, %i (%i, %i)", iRestor2d_xtop, iRestor2d_ytop, 
		iRestor2d_xbottom, iRestor2d_ybottom, 
		iRestor2d_xbottom-iRestor2d_xtop, iRestor2d_ybottom-iRestor2d_ytop);
	UpdateData(FALSE);
	((CButton*)GetDlgItem(IDC_RESTOR_3DVIS_NOISY))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_RESTOR_3DVIS_NOISYDATA))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_RESTOR_3DVIS_RESTORED))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_RESTOR_FIND_VORTEX))->EnableWindow(FALSE);
	
}


/*
// this function for loading all 3D heart CFD velocity files and getting maximum and minumum value
void CDlgRestoration::OnButton1() 
{
	CString filename, toLoad;
	int idx;

	filename = "C:\\Documents and Settings\\bsc01r\\My Documents\\DevStudio\\data\\3D\\3D_LV_FlowData\\";

	ifstream infvelocity;
	char sdmy[20];
	float fdmy;
	float max, min;
	max = -10000.0f;
	min =  10000.0f;

	double itotal=0, ftotal=0;

	for(idx=1; idx<=33; idx++)
	{
		if(idx<10)
			toLoad.Format("%sa00%i.velocity", filename, idx);
		else
			toLoad.Format("%sa0%i.velocity", filename, idx);

		infvelocity.open(toLoad);

		// look for 'Vector' marker in velocity file, before first velocity
		while(!infvelocity.eof())
		{
			infvelocity >> sdmy;

			if(strcmp(sdmy, "Vector")==0)
				break;
		}
	
		// load next number and set min/max values
		while(!infvelocity.eof())
		{
			infvelocity >> fdmy;

			if(fdmy<min)
				min=fdmy;

			if(fdmy>max)
				max=fdmy;

			ftotal++;
			itotal++;

			if(ftotal == 130464)
				break;

		}

		infvelocity.close();
		ftotal=0.0;


	}

	CString sms;
	sms.Format("min=%.6f max=%.6f %f numbers checked", min, max, itotal);
	AfxMessageBox(sms);
	
}
*/
/*
// this function for saving all geo+velocity files into irregular vtk grids
void CDlgRestoration::OnButton1() 
{

	// vtk data
	vtkFloatArray *fpoints = vtkFloatArray::New();
	vtkPoints* points = vtkPoints::New();
	vtkFloatArray *fspoints = vtkFloatArray::New();
	fspoints->SetNumberOfComponents(1);
	fpoints->SetNumberOfComponents(3);
	vtkStructuredGrid *sgrid = vtkStructuredGrid::New();
	sgrid->SetDimensions(5,5,5);
	vtkStructuredGridWriter* swriter = vtkStructuredGridWriter::New();

	CString filename, toLoadVel, toLoadGeo, vtkToSave;

	filename = "C:\\Documents and Settings\\bsc01r\\My Documents\\DevStudio\\data\\3D\\3D_LV_FlowData\\";

	ifstream infvelocity, infgeo;
	char sdmy[20];
	float xPos, yPos, zPos, xVel, yVel, zVel;
	int idx;
	float ftotal;

	for(idx=1; idx<=33; idx++)
	{
		fpoints = vtkFloatArray::New();
		points = vtkPoints::New();
		fspoints = vtkFloatArray::New();
		fspoints->SetNumberOfComponents(1);
		fpoints->SetNumberOfComponents(3);

		ftotal = 0.0f;

		if(idx<10)
			toLoadVel.Format("%sa00%i.velocity", filename, idx);
		else
			toLoadVel.Format("%sa0%i.velocity", filename, idx);

		if(idx<10)
			toLoadGeo.Format("%sa00%i.geo", filename, idx);
		else
			toLoadGeo.Format("%sa0%i.geo", filename, idx);

		infvelocity.open(toLoadVel);
		infgeo.open(toLoadGeo);

		// look for 'Vector' marker in velocity file, before first velocity
		while(!infvelocity.eof())
		{
			infvelocity >> sdmy;

			if(strcmp(sdmy, "Vector")==0)
				break;
		}

		// look for '43488' marker in geo file, before first coordinate
		while(!infgeo.eof())
		{
			infgeo >> sdmy;
			
			if(strcmp(sdmy, "43488")==0)
				break;
		}
	
		// load coordinates into vtk file
		while(!infvelocity.eof())
		{
			ftotal++;
			if(ftotal > 43488)
				break;

			infgeo >> xPos >> yPos >> zPos;
			infvelocity >> xVel >> yVel >> zVel;

			fspoints->InsertNextTuple1(sqrt(pow(xVel,2.0)+pow(yVel,2.0)+pow(zVel,2.0)));
			points->InsertNextPoint(xPos, yPos, zPos);
			fpoints->InsertNextTuple3(xVel, yVel, zVel);

		}

		infvelocity.close();
		infgeo.close();

		// save vtk file
		vtkToSave.Format("%sccfd%i.vtk", filename, idx);
		sgrid->SetPoints(points);
		sgrid->GetPointData()->SetScalars(fspoints);
		sgrid->GetPointData()->SetVectors(fpoints);
		swriter->SetInput(sgrid);
		swriter->SetFileName(vtkToSave);
		swriter->Write();

		if(ftotal!=43489.0f)
		{
			CString sms;
			sms.Format("Warning: wrote %f points to ccfd%i.", ftotal, idx);
			AfxMessageBox(sms);
		}

		fpoints->Delete();
		points->Delete();
		fspoints->Delete();


	}

	fpoints->Delete();
	points->Delete();
	fspoints->Delete();
	sgrid->Delete();
	swriter->Delete();
	
}
*/

// this function for saving slice 12 of all geo+velocity files
// into .sur files
/*
void CDlgRestoration::OnButton1() 
{
	CString filename, toLoadGeo, surmToSave, suruToSave, survToSave, surwToSave;
	filename = "C:\\Documents and Settings\\bsc01r\\My Documents\\DevStudio\\data\\3D\\3D_LV_FlowData\\";
	int idx;
	float ftotal;
	CData3DMRI mrDataObj;

	for(idx=1; idx<=33; idx++)
	{
		ftotal = 0.0f;

		// format filename
		if(idx<10)
			toLoadGeo.Format("%sa00%i.geo", filename, idx);
		else
			toLoadGeo.Format("%sa0%i.geo", filename, idx);

		// load file data
		mrDataObj.LoadDataHeartCFD(toLoadGeo, 100);

		// save data to sur file
		mrDataObj.SaveDataSur(idx, 11);

	}

}
*/

/*
void CDlgRestoration::OnButton1() 
{
	CString filename = "C:\\Documents and Settings\\bsc01r\\My Documents\\DevStudio\\data\\3D\\interpolated_hcfd\\test.geo";
	mriData3D.SaveDataHeartCFD(filename);
}
*/

/*
// this function for saving interpolated versions of all geo+velocity files
void CDlgRestoration::OnButton1() 
{
	CString filename, toLoadGeo, tosave;
	filename = "C:\\Documents and Settings\\bsc01r\\My Documents\\DevStudio\\data\\3D\\3D_LV_FlowData\\";
	int idx;
	float ftotal;
	CData3DMRI mrDataObj;

	for(idx=1; idx<=33; idx++)
	{
		if(idx==12)
			continue;

		ftotal = 0.0f;

		// format filename
		if(idx<10)
			toLoadGeo.Format("%sa00%i.geo", filename, idx);
		else
			toLoadGeo.Format("%sa0%i.geo", filename, idx);

		// load file data
		mrDataObj.LoadDataHeartCFD(toLoadGeo, 100);

		// save data to geo file
		tosave.Format("C:\\Documents and Settings\\bsc01r\\My Documents\\DevStudio\\data\\3D\\interpolated_hcfd\\cfd%i_10.geo", idx); 
		mrDataObj.SaveDataHeartCFD(tosave);

	}

}
*/



// this version for restoring using different lambdas and saving the different RMS errors obtained
/*
void CDlgRestoration::OnButton1() 
{
	UpdateData(TRUE);

	double error;

	// open file for output
	ofstream of;
	of.open("rms_sigma_10_30.txt");

	CString fgeo;

	// get noisy error
	error = Restor3dIterationRMS(mriData3DNoisy.u, mriData3DNoisy.v, mriData3DNoisy.w, 
		mriData3D.u, mriData3D.v, mriData3D.w);

	// save noisy error
	of << "LAM\tRMS\n" << "noisy\t" << error << "\n" << "dt = " << m_3DRestorDeltaT << "\n" ;

	// float lmb;
	float sgm;

	for(sgm=0.10f; sgm <= 0.30f; sgm=sgm+0.01f)
	{
		// of << "lambda = " << lmb << "\n";
		of << "sigma = " << sgm << "\n";

		of.flush();

		// set lambda and sigma value
		// m_3DRestorLambda = lmb;
		m_3DRestorSigma = sgm;
		UpdateData(FALSE);

		// restore
		OnRestor3dgo();

		// get error
		error = Restor3dIterationRMS(mriData3DRestored.u, 
			mriData3DRestored.v, mriData3DRestored.w, mriData3D.u, mriData3D.v, mriData3D.w);

		// save error and lambda
		of << m_3DRestorLambdaFinal << "\t" << error << "\n";

		// save vtk arrows
		CDlgFindVortex* pDlgVortex;
		pDlgVortex = new CDlgFindVortex;
		pDlgVortex->Create(IDD_FINDVORTEX, this);
		pDlgVortex->NoLoadInitialise(&mriData3DRestored);
		pDlgVortex->ShowWindow(SW_HIDE);
		pDlgVortex->VtkSaveArrows(FALSE, TRUE, 0,0,0,0,0,0,FloatToInt(sgm*100.0f));
		delete pDlgVortex;

		fgeo.Format("cfd12_sigma_%.3f.geo", sgm);
		mriData3DRestored.SaveDataHeartCFD(fgeo);

	}

}
*/

/*
void CDlgRestoration::OnButton1() 
{
	mriData3D.SaveDataSur(1, 12);

}
*/

/*
void CDlgRestoration::OnButton1() 
{
	UpdateData(TRUE);

	double error;

	// open file for output
	ofstream of;
	of.open("rms_lambda_-1_9.txt");

	CString fgeo;

	// get noisy error
	error = Restor3dIterationRMS(mriData3DNoisy.u, mriData3DNoisy.v, mriData3DNoisy.w, 
		mriData3D.u, mriData3D.v, mriData3D.w);

	// save noisy error
	of << "LAM\tRMS\n" << "noisy\t" << error << "\n" << "dt = " << m_3DRestorDeltaT << "\n" ;

	float lmb;
	// float sgm;

	for(lmb=-1.0f; lmb <= 9.00f; lmb=lmb+1.0f)
	{
		of << "lambda = " << lmb << "\n";
		// of << "sigma = " << sgm << "\n";

		of.flush();

		// set lambda and sigma value
		m_3DRestorLambda = lmb;
		// m_3DRestorSigma = sgm;
		UpdateData(FALSE);

		// restore
		OnRestor3dgo();

		// get error
		error = Restor3dIterationRMS(mriData3DRestored.u, 
			mriData3DRestored.v, mriData3DRestored.w, mriData3D.u, mriData3D.v, mriData3D.w);

		// save error and lambda
		of << m_3DRestorLambdaFinal << "\t" << error << "\n";

		// save vtk arrows
		CDlgFindVortex* pDlgVortex;
		pDlgVortex = new CDlgFindVortex;
		pDlgVortex->Create(IDD_FINDVORTEX, this);
		pDlgVortex->NoLoadInitialise(&mriData3DRestored);
		pDlgVortex->ShowWindow(SW_HIDE);
		pDlgVortex->VtkSaveArrows(FALSE, TRUE, 0,0,0,0,0,0,lmb);
		delete pDlgVortex;

		fgeo.Format("cfd12_lambda_%.0f.geo", lmb);
		mriData3DRestored.SaveDataHeartCFD(fgeo);

	}

}
*/

void CDlgRestoration::OnRestorCcfdRoiPreset() 
{
	UpdateData(TRUE);

	iRestor2d_xtop = 0;
	iRestor2d_ytop = 0.65*(float)mriData3D.img_height;
	iRestor2d_xbottom = mriData3D.img_width-1;
	iRestor2d_ybottom = mriData3D.img_height-1;
	m_3DRestorSliceEnd = mriData3D.img_slices;
	m_RestoredSliceNo = 1;
	
	m_3DRestorRoiText.Format("%i, %i --> %i, %i (%i, %i)", iRestor2d_xtop, iRestor2d_ytop, 
		iRestor2d_xbottom, iRestor2d_ybottom, 
		iRestor2d_xbottom-iRestor2d_xtop, iRestor2d_ybottom-iRestor2d_ytop);

	m_ZeroSkip = TRUE;

	UpdateData(FALSE);
	
}
/*

// converts Texton .geo files into .sur files
void CDlgRestoration::OnButton1() 
{
	CString filename, toLoadGeo, surmToSave, suruToSave, survToSave, surwToSave;
	filename = "C:\\Documents and Settings\\bsc01r\\My Documents\\PhD\\data\\3D\\interpolated_hcfd\\";
	int idx;
	float ftotal;

	for(idx=1; idx<=33; idx++)
	{
		ftotal = 0.0f;

		// format filename
		toLoadGeo.Format("%scfd%i_10.geo", filename, idx);

		// load file data
		mriData3D.LoadDataHeartCFDTexton(toLoadGeo);

		// set ROI
		OnRestorCcfdRoiPreset();

		// zoom data
		mriData3D.Zoom(CPoint(iRestor2d_xtop, iRestor2d_ytop), CPoint(iRestor2d_xbottom, iRestor2d_ybottom),
			m_RestoredSliceNo-1, m_3DRestorSliceEnd-1, 2.0);

		// save data to sur file
		mriData3D.SaveDataSur(idx, 21);
		mriData3D.SaveDataSur(idx, 22);
		mriData3D.SaveDataSur(idx, 23);
		mriData3D.SaveDataSur(idx, 24);
		mriData3D.SaveDataSur(idx, 25);
		mriData3D.SaveDataSur(idx, 26);
		mriData3D.SaveDataSur(idx, 27);
		mriData3D.SaveDataSur(idx, 28);
		mriData3D.SaveDataSur(idx, 29);
		mriData3D.SaveDataSur(idx, 30);
		mriData3D.SaveDataSur(idx, 31);
		mriData3D.SaveDataSur(idx, 32);

	}

}
*/

/*
// scans .sur files and reports violations of the [-1800, 1800] range
void CDlgRestoration::OnButton1() 
{
	CString filename, toLoadSur, surmToSave, suruToSave, survToSave, surwToSave;
	filename = "C:\\Documents and Settings\\bsc01r\\My Documents\\DevStudio\\data\\cfd_sur\\";
	int idx;
	float ftotal;

	for(idx=1; idx<=33; idx++)
	{
		ftotal = 0.0f;

		// format filename
		if(idx<10)
			toLoadSur.Format("%smr00%i", filename, idx);
		else
			toLoadSur.Format("%smr0%i", filename, idx);

		// load file data
		mriData3D.LoadDataFrame(toLoadSur);

		int i,j,k;
		for(i=0; i<


	}

}
*/

/*
void CDlgRestoration::OnButton1() 
{
///*	CString filename, toLoadGeo;
	filename = "C:\\Documents and Settings\\bsc01r\\My Documents\\DevStudio\\data\\3D\\interpolated_hcfd\\geo data used up to 2_2_04\\";
	CData3DMRI mrDataObj;

	// format filename
	toLoadGeo.Format("%scfd12_10_noise7.geo", filename);

	// load file data
	mrDataObj.LoadDataHeartCFDTexton(toLoadGeo);


	CData3DMRI mrDataZeroes;
	mrDataZeroes.mr = new short[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
	mrDataZeroes.u = new short[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
	mrDataZeroes.v = new short[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
	mrDataZeroes.w = new short[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];

	for(i=0; i<maxIndex; i++)
	{
		mrDataZeroes.mr[i] = 0;
		mrDataZeroes.u[i] = 0;
		mrDataZeroes.v[i] = 0;
		mrDataZeroes.w[i] = 0;
	}
//
	int i, maxIndex;
	maxIndex = mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices;
	double magn;
///*
	CData3DMRI mrDataNoiseNorm, mrDataOriginalNoiseNorm;
	mrDataNoise.mr = new short[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
	mrDataNoise.u = new short[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
	mrDataNoise.v = new short[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
	mrDataNoise.w = new short[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];

	for(i=0; i<maxIndex; i++)
	{
		mrDataNoise.mr[i] = 0;
		mrDataNoise.u[i] = mriData3D.u[i]-mriData3DNoisy.u[i];
		mrDataNoise.v[i] = mriData3D.v[i]-mriData3DNoisy.v[i];
		mrDataNoise.w[i] = mriData3D.w[i]-mriData3DNoisy.w[i];
	}
//
	CData3DMRI mrDataNoise;
	mrDataNoise.mr = new short[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
	mrDataNoise.u = new short[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
	mrDataNoise.v = new short[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
	mrDataNoise.w = new short[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];

	for(i=0; i<maxIndex; i++)
	{
		mrDataNoise.mr[i] = 0;
		mrDataNoise.u[i] = mriData3D.u[i]-mriData3DNoisy.u[i];
		mrDataNoise.v[i] = mriData3D.v[i]-mriData3DNoisy.v[i];
		mrDataNoise.w[i] = mriData3D.w[i]-mriData3DNoisy.w[i];
	}

	double var = 0.0;
	int x, y, z;

	double img_size = abs(m_3DRestorSliceEnd-m_RestoredSliceNo+1)*abs(iRestor2d_xtop-iRestor2d_xbottom+1)
		*abs(iRestor2d_ytop-iRestor2d_ybottom+1);

	for(z=m_RestoredSliceNo-1; z<m_3DRestorSliceEnd; z++)
		for(x=iRestor2d_xtop; x<=iRestor2d_xbottom; x++)
			for(y=iRestor2d_ytop; y<=iRestor2d_ybottom; y++)
			{
				magn = sqrt(pow(mrDataNoise.u[i], 2.0) 
						  + pow(mrDataNoise.v[i], 2.0) 
						  + pow(mrDataNoise.w[i], 2.0));

				if(magn==0.0)
					continue;

				var = var +
					( pow(((double)mrDataNoise.u[i]), 2.0) 
					+ pow(((double)mrDataNoise.v[i]), 2.0) 
					+ pow(((double)mrDataNoise.w[i]), 2.0) )
					/ ((double)img_size);
			}

	var = sqrt(var);

	CString sms133;
	sms133.Format("var = %f", var);
	AfxMessageBox(sms133);


}
*/
void CDlgRestoration::OnRestorLoad7D() 
{

	CDlg7DLoadOptions* dlgLoadOptions = new CDlg7DLoadOptions;
	if(dlgLoadOptions->DoModal()!=IDOK)
		return;

	// prompt user for path and load file
	CFileDialog* dDlg = new CFileDialog(TRUE,"*","raw",OFN_CREATEPROMPT |
		OFN_HIDEREADONLY | OFN_NOREADONLYRETURN,
		_T("7D dataset (*.raw)|*.raw|All Files (*.*)|*.*||"), NULL);

	if(dDlg->DoModal()!=IDOK)
		return;

	UpdateData(TRUE);

	mriData3D.LoadDataHeart7D(dDlg->GetPathName(), dlgLoadOptions->m_timeFrame);
	m_3DRestorSliceEnd = mriData3D.img_slices;
	UpdateData(FALSE);
	delete dDlg;
	delete dlgLoadOptions;
	
	iRestor2d_xtop = 0;
	iRestor2d_ytop = 0;
	iRestor2d_xbottom = mriData3D.img_width-1;
	iRestor2d_ybottom = mriData3D.img_height-1;

	m_Restor3dImgSizesTxt.Format("image: (x, y, s) %i x %i x %i", mriData3D.img_width, 
		mriData3D.img_height, mriData3D.img_slices);
	m_3DRestorRoiText.Format("%i, %i --> %i, %i (%i, %i)", iRestor2d_xtop, iRestor2d_ytop, 
		iRestor2d_xbottom, iRestor2d_ybottom, 
		iRestor2d_xbottom-iRestor2d_xtop, iRestor2d_ybottom-iRestor2d_ytop);
	UpdateData(FALSE);
	((CButton*)GetDlgItem(IDC_RESTOR_3DVIS_NOISY))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_RESTOR_3DVIS_NOISYDATA))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_RESTOR_3DVIS_RESTORED))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_RESTOR_FIND_VORTEX))->EnableWindow(FALSE);
	
}

/*
void CDlgRestoration::OnButton1() 
{
	CString filename, toLoadGeo, toSaveGeo, suruToSave, survToSave, surwToSave;
	filename = "C:\\Documents and Settings\\bsc01r\\My Documents\\DevStudio\\data\\3D\\interpolated_hcfd\\";
	int idx;
	float ftotal;

	for(idx=1; idx<=33; idx++)
	{
		ftotal = 0.0f;

		// format filename
		toLoadGeo.Format("%scfd%i_10.geo", filename, idx);

		// load file data
		mriData3D.LoadDataHeartCFDTexton(toLoadGeo);

		// set ROI
		OnRestorCcfdRoiPreset();
		m_3DRestorSliceEnd = 50;

		// zoom data
		mriData3D.Zoom(CPoint(iRestor2d_xtop, iRestor2d_ytop), CPoint(iRestor2d_xbottom, iRestor2d_ybottom),
			m_RestoredSliceNo-1, m_3DRestorSliceEnd-1, 1.0);

		// save data to geo file
		toSaveGeo.Format("%scfd%i_10_roi.geo", filename, idx);
		mriData3D.SaveDataHeartCFD(toSaveGeo);

	}


}
*/
/*
#include "CMRStreamlines.h"

void CDlgRestoration::OnButton1() 
{
	// 2D source vector field
	short* u, *v;
	u = mriData3D.u;
	v = mriData3D.v;

	// 2D target RGB image
	int* r, *g, *b, *gs;
	r  = new int[mriData3D.img_width*mriData3D.img_height];
	g  = new int[mriData3D.img_width*mriData3D.img_height];
	b  = new int[mriData3D.img_width*mriData3D.img_height];
	gs = new int[mriData3D.img_width*mriData3D.img_height];

	// generate streamlines
	CCMRStreamlines streamer;
	streamer.StreamlinePlot(u, v, r, g, b, gs, mriData3D.img_width, mriData3D.img_height, 3.0f);

	// render result
	CImageColour imgColour;
	imgColour.AllocateData3D(mriData3D.img_width, mriData3D.img_height, 1);

	int x, y;
	for(x = 0; x<mriData3D.img_width; x++)
		for(y=0; y<mriData3D.img_height; y++)
		{	
			imgColour.imgData[y*mriData3D.img_width+x].RED   = r[y*mriData3D.img_width+x];
			imgColour.imgData[y*mriData3D.img_width+x].GREEN = g[y*mriData3D.img_width+x];
			imgColour.imgData[y*mriData3D.img_width+x].BLUE  = b[y*mriData3D.img_width+x];
		}

	// colour output
	CDlgVolView* pVolDlg = new CDlgVolView;
	pVolDlg->pView = pView;
	pVolDlg->m_imgInfo.Format("image: (x, y, s) %i x %i x %i", mriData3D.img_width, 
		mriData3D.img_height, 1);
	pVolDlg->m_sVolumeTitle.Format("LIC output in colour");
	pVolDlg->InitialiseVolume(imgColour, mriData3D.img_width, mriData3D.img_height, 
		1);
	pVolDlg->Create(IDD_VOL_VIEW, this);
	pVolDlg->ShowWindow(SW_SHOW);				

	
	// BW output
//	CDlgVolView* pVolDlg = new CDlgVolView;
//	pVolDlg->pView = pView;
//	pVolDlg->m_imgInfo.Format("image: (x, y, s) %i x %i x %i", mriData3D.img_width, 
//		mriData3D.img_height, 1);
//	pVolDlg->m_sVolumeTitle.Format("LIC output in B&W");
//	pVolDlg->InitialiseVolume(gs, mriData3D.img_width, mriData3D.img_height, 
//		1);
//	pVolDlg->Create(IDD_VOL_VIEW, this);
//	pVolDlg->ShowWindow(SW_SHOW);				

	delete [] r;
	delete [] g;
	delete [] b;
	delete [] gs;

}

*/

/*
// loads matlab floats into CCFD preset size and saves as vtk for isoSurface
void CDlgRestoration::OnButton1() 
{
	int img_width = 35;
	int img_height = 27;
	int img_slices = 78;

	FILE* inf;
	inf = fopen("c:\\asymmetry_or8to25_sumdim4.mat", "r");

	int max_size = img_width*img_height*img_slices;
	float* theData = new float[max_size];

	int x, y, z;
	int index;
	float test;
	for(index=0; index<max_size; index++)
	{
		fscanf(inf, "%f", &test);
		theData[index] = test;
	}

	CVtkUtilities saver;
	saver.WriteImageVtk(theData, img_width, img_height, img_slices, "c:\\testVolume.vtk");

}
*/

void CDlgRestoration::OnButton1() 
{
	float error = Restor3dIterationRMS(mriData3DNoisy.u, mriData3DNoisy.v, mriData3DNoisy.w, 
		mriData3D.u, mriData3D.v, mriData3D.w);

	CString sms133;
	sms133.Format("%f", error);
	AfxMessageBox(sms133);

}