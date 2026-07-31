// DlgStreamlines.cpp : implementation file
//

#include "stdafx.h"
#include "Texton.h"
#include "DlgStreamlines.h"
#include "TextonView.h"
//#include <ipl.h>
#include <math.h>
#include <float.h>
#include "ImageColour.h"
#include "DlgStrmZoom.h"
#include "Rnd_no_gen.h"
#include "MainFrm.h"
#include "DlgVolView.h"
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
#include <vtkPointData.h>
#include <vtkCellArray.h>
#include <vtkProperty.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgStreamlines dialog


CDlgStreamlines::CDlgStreamlines(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgStreamlines::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgStreamlines)
	m_NoiseSeed = 3;
	m_NoiseMean = 50;
	m_NoiseVariance = 40;
	m_RangeAlpha = 10;
	m_LaplWeight = 0.0f;
	m_NumberIterations = 2;
	m_VelDelta = 1.3f;
	m_VortexRange = 3600;
	m_NoiseBlur = 0;
	m_MagMultiplier = 1.0f;
	m_IntMultiplier = 1.0f;
	m_bAlphaDyn = FALSE;
	m_ZoomSize = 255;
	m_3dImgSizesTxt = _T("");
	//}}AFX_DATA_INIT

	texture = NULL;
	texture3D = NULL;
	noise_img = NULL;
	noise_img3D = NULL;
	img_width = img_height = 256;
	bZoom = FALSE;
	pDlgZoom = NULL;
	savedDataValid = FALSE;
	img_slices = 1;
	mode3D = FALSE;
	iStrmMask = NULL;
	m_iLaplacian = NULL;
	m_bPointCollect = FALSE;
	m_nSeedPoints = 0;
	streamlineArray = NULL;
	m_seedPoints = NULL;
	m_dynAlpha = 0.008; // see TextureIteration3D()

}

CDlgStreamlines::~CDlgStreamlines()
{
	if(streamlineArray != NULL)
		delete [] streamlineArray;
}



void CDlgStreamlines::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgStreamlines)
	DDX_Text(pDX, IDC_STR_NOISE_S, m_NoiseSeed);
	DDX_Text(pDX, IDC_STR_NOISE_M, m_NoiseMean);
	DDX_Text(pDX, IDC_STR_NOISE_V, m_NoiseVariance);
	DDX_Text(pDX, IDC_STRM_ALPHA, m_RangeAlpha);
	DDX_Text(pDX, IDC_STRM_LAPLW, m_LaplWeight);
	DDX_Text(pDX, IDC_STRM_ITS, m_NumberIterations);
	DDX_Text(pDX, IDC_STRM_DELTA, m_VelDelta);
	DDX_Text(pDX, IDC_STRM_VTX_RANGE, m_VortexRange);
	DDX_Text(pDX, IDC_STR_NOISE_B, m_NoiseBlur);
	DDX_Text(pDX, IDC_STRM_MAG_MULT, m_MagMultiplier);
	DDX_Text(pDX, IDC_STRM_INT_MULT, m_IntMultiplier);
	DDX_Check(pDX, IDC_ALPHADYN, m_bAlphaDyn);
	DDX_Text(pDX, IDC_STRM_ZOOM_SIZE, m_ZoomSize);
	DDX_Text(pDX, IDC_STRM_IMGSIZES, m_3dImgSizesTxt);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgStreamlines, CDialog)
	//{{AFX_MSG_MAP(CDlgStreamlines)
	ON_BN_CLICKED(IDC_STRM_OPT_STP, OnOptionStop)
	ON_BN_CLICKED(IDC_STRM_OPT_ITS, OnOptionIterations)
	ON_WM_CANCELMODE()
	ON_BN_CLICKED(IDC_STRM_GO, OnStreamlinesGo)
	ON_BN_CLICKED(IDC_STRM_ITS_NEXT, OnStreamlinesNext)
	ON_BN_CLICKED(IDC_STRM_LOAD, OnStrmLoad)
	ON_BN_CLICKED(IDC_STRM_NOISE, OnStrmNoise)
	ON_BN_CLICKED(IDC_SHOW_MR, OnShowMR)
	ON_BN_CLICKED(IDC_SHOW_NSE, OnShowNse)
	ON_BN_CLICKED(IDC_SHOW_TXT, OnShowTxt)
	ON_BN_CLICKED(IDC_STRM_VORTEX, OnStrmVortex)
	ON_BN_CLICKED(IDC_STRM_RES_SAVE, OnStrmResSave)
	ON_BN_CLICKED(IDC_STRM_TXT_BLUR, OnStrmTxtBlur)
	ON_BN_CLICKED(IDC_STRM_TXT_MAGN, OnStrmTxtMagn)
	ON_BN_CLICKED(IDC_STRM_ZOOM, OnStrmZoom10)
	ON_BN_CLICKED(IDC_STRM_TXT_COLOUR, OnStrmTxtColour)
	ON_BN_CLICKED(IDC_STRM_FLIP_U, OnStrmFlipU)
	ON_BN_CLICKED(IDC_STRM_FLIP_V, OnStrmFlipV)
	ON_BN_CLICKED(IDC_STRM_FLIP_W, OnStrmFlipW)
	ON_BN_CLICKED(IDC_SHOW_U, OnShowU)
	ON_BN_CLICKED(IDC_SHOW_V, OnShowV)
	ON_BN_CLICKED(IDC_SHOW_W, OnShowW)
	ON_BN_CLICKED(IDC_ALPHADYN, OnAlphaDyn)
	ON_BN_CLICKED(IDC_STRM_LOAD3D, OnStrmLoad3d)
	ON_BN_CLICKED(IDC_STRM_NOISE_GRID, OnStrmNoiseGrid)
	ON_BN_CLICKED(IDC_STRM_VTK_STREAMER, OnStrmVtkStreamer)
	ON_BN_CLICKED(IDC_STRM_VTK_LINEMAP, OnStrmVtkLinemap)
	ON_BN_CLICKED(IDC_STRM_VIEW_LAPLACIAN, OnStrmViewLaplacian)
	ON_BN_CLICKED(IDC_STRM_VTK_STREAMER_LIC, OnStrmVtkStreamerSaveLic)
	ON_BN_CLICKED(IDC_STRM_VTK_ARROWS3D, OnStrmVtkArrows3d)
	ON_BN_CLICKED(IDC_STRM_TXT_SEED, OnStrmTxtSeed)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgStreamlines message handlers

void CDlgStreamlines::OnOptionStop() 
{
	mi_iterationOption = 1;
	((CButton*)GetDlgItem(IDC_STRM_ITS_NEXT))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_STRM_ITS))->EnableWindow(FALSE);

}

void CDlgStreamlines::OnOptionIterations() 
{
	mi_iterationOption = 2;
	((CButton*)GetDlgItem(IDC_STRM_ITS_NEXT))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_STRM_ITS))->EnableWindow(TRUE);
	
}

BOOL CDlgStreamlines::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	((CButton*)GetDlgItem(IDC_STRM_OPT_ITS))->SetCheck(1);
	mi_iterationOption = 2;

	if(m_bPointCollect) // we're being called in modal mode
	{
		((CButton*)GetDlgItem(IDC_SHOW_MR))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_SHOW_U))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_SHOW_V))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_SHOW_W))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_STRM_VORTEX))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_STRM_VTX_RANGE))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_STRM_LOAD))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_STRM_LOAD3D))->EnableWindow(FALSE);
	}
	

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgStreamlines::OnCancelMode() 
{
	CDialog::OnCancelMode();
	
	
}

void CDlgStreamlines::OnStreamlinesGo() 
{

	((CButton*)GetDlgItem(IDC_SHOW_TXT))->EnableWindow(FALSE);

	if(!mode3D)
	{
		UpdateData(TRUE);
		
		if(noise_img==NULL)
			OnStrmNoise();
		
		TextureReset();	
		
		// allocate target image
		if(texture != NULL)
			delete [] texture;
		
		texture = new float[img_width*img_height];
		float* start = new float[img_width*img_height];
		
		// texture iterations
		int i;
		if(mi_iterationOption == 2)
		{
			CopyTextureI(noise_img, start);
			TextureIteration(start, m_RangeAlpha, m_LaplWeight);
			CopyTextureFP(texture, start);
			
			for(i=0; i<=m_NumberIterations; i++)
			{
				TextureIteration(start, m_RangeAlpha, m_LaplWeight);
				CopyTextureFP(texture, start);
			}
		}
		else
		{
			CopyTextureI(noise_img, start);
			TextureIteration(start, m_RangeAlpha, m_LaplWeight);
			m_NumberIterations = 1;	
			UpdateData(FALSE);
			((CButton*)GetDlgItem(IDC_STRM_ITS_NEXT))->EnableWindow(TRUE);
		}
		
		((CButton*)GetDlgItem(IDC_SHOW_TXT))->EnableWindow(TRUE);
		delete [] start;
		
		OnShowTxt();
	}
	else
	{
		UpdateData(TRUE);
		
		if(noise_img3D==NULL)
			OnStrmNoise();
		
		TextureReset3D();
		
		// allocate target image
		if(texture3D != NULL)
			delete [] texture3D;
		
		texture3D = new float[img_width*img_height*img_slices];
		float* start3D = new float[img_width*img_height*img_slices];
		
		// texture iterations
		int i;
		if(mi_iterationOption == 2)
		{
			CopyTextureI3D(noise_img3D, start3D);
			TextureIteration3D(start3D, m_RangeAlpha, m_LaplWeight);
			CopyTextureFP3D(texture3D, start3D);
			
			for(i=0; i<=m_NumberIterations; i++)
			{
				TextureIteration3D(start3D, m_RangeAlpha, m_LaplWeight);
				CopyTextureFP3D(texture3D, start3D);
			}
		}
		else
		{
			CopyTextureI3D(noise_img3D, start3D);	
			TextureIteration3D(start3D, m_RangeAlpha, m_LaplWeight);
			m_NumberIterations = 1;	
			UpdateData(FALSE);
			((CButton*)GetDlgItem(IDC_STRM_ITS_NEXT))->EnableWindow(TRUE);
		}
		

		delete [] start3D;
		
	}

	((CButton*)GetDlgItem(IDC_SHOW_TXT))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_STRM_TXT_BLUR))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_STRM_TXT_MAGN))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_STRM_TXT_COLOUR))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_STRM_TXT_SEED))->EnableWindow(TRUE);
	// OnStrmVtkLinemap();

}

float CDlgStreamlines::CalcDeltaS(int direction, int x, int y, float prevx, float prevy, 
				 float prevvx, float prevvy, float vel_magn)
{
	double svals[4];

	svals[0] = ((y+1)-prevy)*(vel_magn/prevvy);
	svals[1] = (y-prevy)*(vel_magn/prevvy);
	svals[2] = ((x+1)-prevx)*(vel_magn/prevvx);
	svals[3] = (x-prevx)*(vel_magn/prevvx);

	float min = HUGE_VAL;
	BOOL min_set = FALSE;
	for(int i=0; i<4; i++)
		if(svals[i]<min && svals[i]>0 && _finite(svals[i]))
		{
			min=svals[i];
			min_set = TRUE;

		}

	if(min_set == FALSE)
		return -1;
	else
//		if(min > 0.7071066 && min < 0.7071068)
			return min+0.01;
//		else
//			return min;

}

float CDlgStreamlines::CalcDeltaS3D(int direction, int x, int y, int z, float prevx, float prevy, float prevz, 
		float prevvx, float prevvy, float prevvz, float vel_magn)
{
	double svals[6];

	svals[0] = ((y+1)-prevy)*(vel_magn/prevvy);
	svals[1] = (y-prevy)*(vel_magn/prevvy);
	svals[2] = ((x+1)-prevx)*(vel_magn/prevvx);
	svals[3] = (x-prevx)*(vel_magn/prevvx);
	svals[4] = ((z+1)-prevz)*(vel_magn/prevvz);
	svals[5] = (z-prevz)*(vel_magn/prevvz);

	float min = HUGE_VAL;
	BOOL min_set = FALSE;
	for(int i=0; i<6; i++)
		if(svals[i]<min && svals[i]>0 && _finite(svals[i]))
		{
			min=svals[i];
			min_set = TRUE;
		}

	if(min_set == FALSE)
	{
		// AfxMessageBox("No svals minimum found.");
		return -1;
	}
	else
//		if(min > 0.7071066 && min < 0.7071068)
			return min+0.01;
//		else
//			return min;

}

void CDlgStreamlines::TextureLaplacian3D(float *laplacian)
{
	const BOOL bPerSlice = TRUE;
	const BOOL bVtk = FALSE;

	if(bPerSlice)
	{
		float* lapl_topass = new float[img_width*img_height];

		int slice, x, y;

		if(texture != NULL)
			delete [] texture;
		
		texture = new float[img_width*img_height];

		for(slice=0; slice<img_slices; slice++)
		{
			// set texture[] to each slice of texture3d[]
			for(x=0; x<img_width; x++)
				for(y=0; y<img_height; y++)
					texture[y*img_width+x] = texture3D[slice*img_width*img_height+y*img_width+x];

			// set each slice of laplacian[] to return of 2D laplacian
			TextureLaplacian(lapl_topass);

			for(x=0; x<img_width; x++)
				for(y=0; y<img_height; y++)
					laplacian[slice*img_width*img_height+y*img_width+x]=
					lapl_topass[y*img_width+x];

		}

		delete [] lapl_topass;
		

	}
	else
	{
		if(bVtk)
		{
			CVtkUtilities vtkutils;
			vtkutils.Laplacian(texture3D, laplacian, img_width, img_height, img_slices);

		}
		else
		{
			if(img_slices < 3)
			{
				AfxMessageBox("Please supply an image with at least 3 slices.");
				return;
			}

			const float o2 = 16.0f;
			const int lsize = 3;
			float coeffs[lsize*lsize*lsize];

			// compute coefficients
			int x, y, z;
		/*	float nx, ny, nz;
			for(x=0; x<3; x++)
				for(y=0; y<3; y++)
					for(z=0; z<3; z++)
					{
						nx = ((float)x)-(((float)lsize)-1.0f)/2.0;
						ny = ((float)y)-(((float)lsize)-1.0f)/2.0;
						nz = ((float)z)-(((float)lsize)-1.0f)/2.0;

						coeffs[z*lsize*lsize+y*lsize+x] =
							(1.0f / o2) * ((nx*nx+ny*ny+nz*nz)/o2-3.0) * (float)exp(-(nx*nx+ny*ny+nz*nz)/(2*o2));
					}
		*/	

			for(x=0; x<3; x++)
				for(y=0; y<3; y++)
					for(z=0; z<3; z++)
						coeffs[z*lsize*lsize+y*lsize+x] = -2.0f;

			coeffs[1*lsize*lsize+1*lsize+1] = 52;

			// convolve with image
			for(x=0; x<img_width; x++)
				for(y=0; y<img_height; y++)
					for(z=0; z<img_slices; z++)
						laplacian[z*img_width*img_height+y*img_width+x]=0.0;

			int cx, cy, cz;
			for(x=1; x<img_width-1; x++)
				for(y=1; y<img_height-1; y++)
					for(z=1; z<img_slices-1; z++)
					{
						for(cx=0; cx<lsize; cx++)
							for(cy=0; cy<lsize; cy++)
								for(cz=0; cz<lsize; cz++)
								{
									laplacian[z*img_width*img_height+y*img_width+x]
										= laplacian[z*img_width*img_height+y*img_width+x]
										+ texture3D[(z-1+cz)*img_width*img_height+(y-1+cy)*img_width+(x-1+cx)]
										* coeffs[cz*lsize*lsize+cy*lsize+cx];
								}


					}
			}			
		}

}

int CDlgStreamlines::TextureIteration(float* start, int alpha, float lapw)
{
	int n, x, y, ix, iy;
	float newx, newy, prevx, prevy;
	float vel_factor = m_VelDelta;
	float vel_magn;
	int term_count;
	double tex_temp;
	int dyn_alpha = (float)alpha/1000.0f;

	float deltas;

	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
		{
			if(m_bAlphaDyn)
				alpha = FloatToInt(ceil(5.0f+dyn_alpha*sqrt(pow(mriData.u[img_width*y+x],2)
				+pow(mriData.v[img_width*y+x],2))));
				// this should map velocity [0-2555] into [5-25] 
				// calling function has called UpdateData() already for m_bAlphaDyn

			//texture[img_width*y+x] = 0;
			tex_temp = 0;
			term_count = 0;

			prevx = x+0.5;
			prevy = y+0.5;

			// positive side
			for(n=1; n<=alpha; n++)
			{
				ix = (int)(floor)(prevx);
				iy = (int)(floor)(prevy);
				vel_magn = (float)sqrt(pow(mriData.u[img_width*iy+ix],2)
					+pow(mriData.v[img_width*iy+ix],2));
				//newx = prevx + floor(mriData.u[img_width*iy+ix]*(1.0f/vel_magn)*vel_factor);
				//newy = prevy + floor(mriData.v[img_width*iy+ix]*(1.0f/vel_magn)*vel_factor);
				deltas = CalcDeltaS(1, ix, iy, prevx, prevy, 
					mriData.u[img_width*iy+ix], mriData.v[img_width*iy+ix], vel_magn);
				if(deltas == -1)
					continue;
				newx = prevx + mriData.u[img_width*iy+ix]*(1.0f/vel_magn)*deltas;
				newy = prevy + mriData.v[img_width*iy+ix]*(1.0f/vel_magn)*deltas;
				if(newx>=img_width)newx=img_width-1;
				if(newy>=img_height)newy=img_height-1;
				if(newx<0)newx=0;
				if(newy<0)newy=0;

				ix = FloatToInt(newx);
				iy = FloatToInt(newy);
				if(ix>=img_width || iy >= img_height || ix < 0 || iy < 0)
					continue;
				// texture[img_width*y+x] = texture[img_width*y+x] + start[img_width*iy+ix];

				// AddArrayPoint(ix, iy, 0, streamlineTest);

				tex_temp = tex_temp + start[img_width*iy+ix];
				term_count++;

				prevx = newx;
				prevy = newy;
				
			}

			prevx = x+0.5;
			prevy = y+0.5;
			// negative side
			for(n=1; n<=alpha; n++)
			{
				ix = (int)(floor)(prevx);
				iy = (int)(floor)(prevy);
				vel_magn = (float)sqrt(pow(mriData.u[img_width*iy+ix],2)
					+pow(mriData.v[img_width*iy+ix],2));
				//newx = prevx - floor(mriData.u[img_width*iy+ix]*(1.0f/vel_magn)*vel_factor);
				//newy = prevy - floor(mriData.v[img_width*iy+ix]*(1.0f/vel_magn)*vel_factor);
				deltas = CalcDeltaS(1, ix, iy, prevx, prevy, 
					(-1.0f)*mriData.u[img_width*iy+ix], (-1.0f)*mriData.v[img_width*iy+ix], vel_magn);
				if(deltas == -1)
					break;
				newx = prevx - mriData.u[img_width*iy+ix]*(1.0f/vel_magn)*deltas;
				newy = prevy - mriData.v[img_width*iy+ix]*(1.0f/vel_magn)*deltas;
				if(newx>=img_width)newx=img_width-1;
				if(newy>=img_height)newy=img_height-1;
				if(newx<0)newx=0;
				if(newy<0)newy=0;

				ix = FloatToInt(newx);
				iy = FloatToInt(newy);
				// texture[img_width*y+x] = texture[img_width*y+x] + start[img_width*iy+ix];

				// AddArrayPoint(ix, iy, 0, streamlineTest);

				if(ix>=img_width || iy >= img_height || ix < 0 || iy < 0)
					continue;
				tex_temp = tex_temp + start[img_width*iy+ix];
				term_count++;

				prevx = newx;
				prevy = newy;
				
			}
			// zero tau part
			//texture[img_width*y+x] = texture[img_width*y+x] + start[img_width*y+x];
			tex_temp = tex_temp + start[img_width*y+x];
			term_count++;
				
			// normalisation
			// texture[img_width*y+x] = texture[img_width*y+x]/(2.0f*(float)alpha+1.0f); // (float)term_count
			texture[img_width*y+x] = tex_temp/(2.0f*(float)alpha+1.0f); //(float)term_count   //2.0f*(float)alpha+1.0f

			continue;

		}

	// take laplacian
	float* text_lap = new float[img_width*img_height];
	TextureLaplacian(text_lap);

	if(m_iLaplacian != NULL)
		delete [] m_iLaplacian;

	m_iLaplacian = new int[img_width*img_height];

	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
		{
			m_iLaplacian[y*img_width+x] = FloatToInt(text_lap[y*img_width+x]);
			texture[img_width*y+x] = texture[img_width*y+x] + lapw*fabs(text_lap[img_width*y+x]);
		}

	delete [] text_lap;

	return 0;
}

void CDlgStreamlines::OnStreamlinesNext() 
{
	if(!mode3D)
	{
		float* start = new float[img_width*img_height];
		CopyTextureFP(texture, start);
		TextureIteration(start, m_RangeAlpha, m_LaplWeight);

		UpdateData(TRUE);
		m_NumberIterations++;
		UpdateData(FALSE);

		OnShowTxt();

		delete [] start;
	}
	else
	{
		float* start = new float[img_width*img_height*img_slices];
		CopyTextureFP3D(texture, start);
		TextureIteration3D(start, m_RangeAlpha, m_LaplWeight);

		UpdateData(TRUE);
		m_NumberIterations++;
		UpdateData(FALSE);

		delete [] start;
	}

}

void CDlgStreamlines::TextureReset() 
{
	UpdateData(TRUE);
	if(texture != NULL)
	{
		delete [] texture;
		texture = NULL;
	}
	((CButton*)GetDlgItem(IDC_STRM_GO))->EnableWindow(TRUE);
	UpdateData(FALSE);
		
}

void CDlgStreamlines::TextureReset3D() 
{
	UpdateData(TRUE);
	if(texture3D != NULL)
	{
		delete [] texture3D;
		texture3D = NULL;
	}
	((CButton*)GetDlgItem(IDC_STRM_GO))->EnableWindow(TRUE);
	UpdateData(FALSE);
		
}

void CDlgStreamlines::OnStrmLoad() 
{
	if(iStrmMask != NULL)
	{
		delete [] iStrmMask;
		iStrmMask = NULL;
	}

	mode3D = FALSE;

	// prompt user for path and load file
	CFileDialog* dDlg = new CFileDialog(FALSE,"sur","mr001",OFN_CREATEPROMPT |
		OFN_HIDEREADONLY | OFN_NOREADONLYRETURN,
		_T("MRI Data (*.sur)|*.sur||"), NULL);

	if(dDlg->DoModal()!=IDOK)
		return;

	mriData.LoadData(dDlg->GetPathName());
	img_height = mriData.img_height;
	img_width = mriData.img_width;
	img_slices = 1;
	delete dDlg;

	UpdateData(TRUE);
	m_3dImgSizesTxt.Format("image: (x, y, s) %i x %i x %i", mriData.img_width, 
		mriData.img_height, 1);
	UpdateData(FALSE);

	int* dmy = new int[img_width*img_height];
	int x, y;
	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
			dmy[img_width*y+x] = mriData.mr[img_width*y+x]/10;

	((CTextonView*)pView)->DrawResultSpecial(dmy, img_width, img_height);
	((CButton*)GetDlgItem(IDC_SHOW_MR))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_SHOW_U))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_SHOW_V))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_SHOW_W))->EnableWindow(TRUE);

	savedDataValid = FALSE;
	delete [] dmy;
	
}

void CDlgStreamlines::OnStrmNoise() 
{
	UpdateData(TRUE);
	
	if(img_width == 0 || img_height == 0 || img_slices == 0)
	{
		AfxMessageBox("Load image first.");
		return;
	}

	if(mode3D == FALSE)
	{
		// adapted from TiP function GenNoise() by S.E. Grigorescu
		int x, y;
		if(noise_img!=NULL)
			delete [] noise_img;
		noise_img = new int[img_width*img_height];
		for(x=0;x<img_width;x++)
			for(y=0;y<img_height;y++)
				noise_img[img_width*y+x] = 0;

		float a = m_NoiseMean;
		int i, j, amt;
		a = (a < 0) ? 0:a;
		a = (a > 100) ? 100:a;
		amt = (int)FloatToInt(img_height*img_width*a/100.0);
		SetGenRand(m_NoiseSeed);
		for (int k = 0; k < amt; k++) 
		{
			j = (int)FloatToInt(GenRand(0, img_width-1));
			i = (int)FloatToInt(GenRand(0, img_height-1));
			noise_img[img_width*i+j] = (255*FloatToInt(GenRand()))
				*(iStrmMask?((float)iStrmMask[img_width*i+j]):1.0f);			
		}
		
		if(m_NoiseBlur > 1)
		{
			int* dmy = new int[img_width*img_height];
			BlurImage(noise_img, dmy, m_NoiseBlur);
			BlurImage(dmy, noise_img, m_NoiseBlur);
			/*
			for(x=0;x<img_width;x++)
			for(y=0;y<img_height;y++)
			noise_img[img_width*y+x] = dmy[img_width*y+x];
			*/
			delete [] dmy;
		}
		
		((CTextonView*)pView)->DrawResultSpecial(noise_img, img_width, img_height);
		((CButton*)GetDlgItem(IDC_SHOW_NSE))->EnableWindow(TRUE);
	}
	else
	{
		int x, y, z;
		if(noise_img3D!=NULL)
			delete [] noise_img3D;
		noise_img3D = new int[img_width*img_height*img_slices];
		for(x=0; x<img_width; x++)
			for(y=0; y<img_height; y++)
				for(z=0; z<img_slices; z++)
					noise_img3D[z*img_width*img_height+y*img_width+x] = 0;

		float a = m_NoiseMean;
		int i, j, l, amt;
		a = (a < 0) ? 0:a;
		a = (a > 100) ? 100:a;
		amt = (int)FloatToInt(img_height*img_width*img_slices*a/100.0);
		SetGenRand(m_NoiseSeed);
		for (int k = 0; k < amt; k++) 
		{
			j = (int)FloatToInt(GenRand(0, img_width-1));
			i = (int)FloatToInt(GenRand(0, img_height-1));
			l = (int)FloatToInt(GenRand(0, img_slices-1));
			noise_img3D[l*img_width*img_height+i*img_width+j] 
				= (255*FloatToInt(GenRand()))
				* (iStrmMask?((float)iStrmMask[l*img_width*img_height+i*img_width+j]):1.0f);			
		}

		/* BlurImage3D function is empty
		if(m_NoiseBlur > 1)
		{
			int* dmy = new int[img_width*img_height*img_slices];
			BlurImage3D(noise_img, dmy, m_NoiseBlur);
			BlurImage3D(dmy, noise_img, m_NoiseBlur);
			delete [] dmy;
		}
		*/

		((CButton*)GetDlgItem(IDC_SHOW_NSE))->EnableWindow(TRUE);
	}

}

void CDlgStreamlines::OnShowMR() 
{
	if(!mode3D)
	{
		int* dmy;
		dmy = new int[img_width*img_height];
		int x, y;
		for(x=0; x<img_width; x++)
			for(y=0; y<img_height; y++)
				dmy[img_width*y+x] = FloatToInt(mriData.mr[img_width*y+x]*(255.0f/3600.0f)+128.0f);

		NormaliseInt(dmy, img_width, img_height);			
		((CTextonView*)pView)->DrawResultSpecial(dmy, img_width, img_height);
		delete [] dmy;
	}
	else
	{
		CDlgVolView* pVolDlg = new CDlgVolView;
		pVolDlg->pView = pView;
		pVolDlg->m_imgInfo.Format("image: (x, y, s) %i x %i x %i", mriData3D.img_width, 
			mriData3D.img_height, mriData3D.img_slices);
		pVolDlg->m_sVolumeTitle.Format("MR data");
		pVolDlg->InitialiseVolume(mriData3D.mr, mriData3D.img_width, mriData3D.img_height, mriData3D.img_slices);
		pVolDlg->Create(IDD_VOL_VIEW, this);
		pVolDlg->ShowWindow(SW_SHOW);				
	}

}

void CDlgStreamlines::OnShowU() 
{

	if(!mode3D)
	{
		int* dmy;
		dmy = new int[img_width*img_height];
		int x, y;
		for(x=0; x<img_width; x++)
			for(y=0; y<img_height; y++)
				dmy[img_width*y+x] = FloatToInt(mriData.u[img_width*y+x]*(255.0f/3600.0f)+128.0f);

		NormaliseInt(dmy, img_width, img_height);			
		((CTextonView*)pView)->DrawResultSpecial(dmy, img_width, img_height);
		delete [] dmy;
	}
	else
	{
		CDlgVolView* pVolDlg = new CDlgVolView;
		pVolDlg->pView = pView;
		pVolDlg->m_imgInfo.Format("image: (x, y, s) %i x %i x %i", mriData3D.img_width, 
			mriData3D.img_height, mriData3D.img_slices);
		pVolDlg->m_sVolumeTitle.Format("velocity data in U direction");
		pVolDlg->InitialiseVolume(mriData3D.u, mriData3D.img_width, mriData3D.img_height, mriData3D.img_slices);
		pVolDlg->Create(IDD_VOL_VIEW, this);
		pVolDlg->ShowWindow(SW_SHOW);				
	}

	
}

void CDlgStreamlines::OnShowV() 
{

	if(!mode3D)
	{
		int* dmy;
		dmy = new int[img_width*img_height];
		int x, y;
		for(x=0; x<img_width; x++)
			for(y=0; y<img_height; y++)
				dmy[img_width*y+x] = FloatToInt(mriData.v[img_width*y+x]*(255.0f/3600.0f)+128.0f);

		NormaliseInt(dmy, img_width, img_height);			
		((CTextonView*)pView)->DrawResultSpecial(dmy, img_width, img_height);
		delete [] dmy;
	}
	else
	{
		CDlgVolView* pVolDlg = new CDlgVolView;
		pVolDlg->pView = pView;
		pVolDlg->m_imgInfo.Format("image: (x, y, s) %i x %i x %i", mriData3D.img_width, 
			mriData3D.img_height, mriData3D.img_slices);
		pVolDlg->m_sVolumeTitle.Format("velocity data in V direction");
		pVolDlg->InitialiseVolume(mriData3D.v, mriData3D.img_width, mriData3D.img_height, mriData3D.img_slices);
		pVolDlg->Create(IDD_VOL_VIEW, this);
		pVolDlg->ShowWindow(SW_SHOW);				
	}


}

void CDlgStreamlines::OnShowW() 
{

	if(!mode3D)
	{
		int* dmy;
		dmy = new int[img_width*img_height];
		int x, y;
		for(x=0; x<img_width; x++)
			for(y=0; y<img_height; y++)
				dmy[img_width*y+x] = FloatToInt(mriData.w[img_width*y+x]*(255.0f/3600.0f)+128.0f);

		NormaliseInt(dmy, img_width, img_height);			
		((CTextonView*)pView)->DrawResultSpecial(dmy, img_width, img_height);
		delete [] dmy;
	}
	else
	{
		CDlgVolView* pVolDlg = new CDlgVolView;
		pVolDlg->pView = pView;
		pVolDlg->m_imgInfo.Format("image: (x, y, s) %i x %i x %i", mriData3D.img_width, 
			mriData3D.img_height, mriData3D.img_slices);
		pVolDlg->m_sVolumeTitle.Format("velocity data in W direction");
		pVolDlg->InitialiseVolume(mriData3D.w, mriData3D.img_width, mriData3D.img_height, mriData3D.img_slices);
		pVolDlg->Create(IDD_VOL_VIEW, this);
		pVolDlg->ShowWindow(SW_SHOW);				
	}


}

void CDlgStreamlines::OnShowNse() 
{
	if(!mode3D)
	{
		int* dmy = new int[img_width*img_height];
		int x, y;
		for(x=0; x<img_width; x++)
			for(y=0; y<img_height; y++)
				dmy[img_width*y+x] = noise_img[img_width*y+x];
			
		NormaliseInt(dmy, img_width, img_height);
			
		((CTextonView*)pView)->DrawResultSpecial(dmy, img_width, img_height);
			
		delete [] dmy;
	}
	else
	{
/*		int* dmy = new int[img_width*img_height*img_slices];
		int x, y, z;
		for(x=0; x<img_width; x++)
			for(y=0; y<img_height; y++)
				for(z=0; z<img_slices; z++)
					dmy[z*img_width*img_height+y*img_width+x] 
					= noise_img3D[z*img_width*img_height+y*img_width+x];
			
		NormaliseInt3D(dmy, img_width, img_height, img_slices);
*/
		CDlgVolView* pVolDlg = new CDlgVolView;
		pVolDlg->pView = pView;
		pVolDlg->m_imgInfo.Format("image: (x, y, s) %i x %i x %i", mriData3D.img_width, 
			mriData3D.img_height, mriData3D.img_slices);
		pVolDlg->m_sVolumeTitle.Format("generated noise");
		pVolDlg->InitialiseVolume(noise_img3D, img_width, img_height, img_slices);
		pVolDlg->Create(IDD_VOL_VIEW, this);
		pVolDlg->ShowWindow(SW_SHOW);
		
//		delete dmy;
	}
}

void CDlgStreamlines::OnShowTxt() 
{

	if(!mode3D)
	{
		int* dmy = new int[img_width*img_height];
		int x, y;
		for(x=0; x<img_width; x++)
			for(y=0; y<img_height; y++)
				dmy[img_width*y+x] = FloatToInt(texture[img_width*y+x]);
			
		NormaliseInt(dmy, img_width, img_height);
			
		((CTextonView*)pView)->DrawResultSpecial(dmy, img_width, img_height);
			
		delete [] dmy;
	}
	else
	{
		CDlgVolView* pVolDlg = new CDlgVolView;
		pVolDlg->pView = pView;
		pVolDlg->m_imgInfo.Format("image: (x, y, s) %i x %i x %i", mriData3D.img_width, 
			mriData3D.img_height, mriData3D.img_slices);
		pVolDlg->m_sVolumeTitle.Format("LIC texture data");
		pVolDlg->InitialiseVolume(texture3D, mriData3D.img_width, mriData3D.img_height, mriData3D.img_slices);
		pVolDlg->Create(IDD_VOL_VIEW, this);
		pVolDlg->ShowWindow(SW_SHOW);				
	}
}

void CDlgStreamlines::OnStrmVortex() 
{
	UpdateData(TRUE);

	img_width = img_height = 256;

	// allocate memory
	mriData.Reset();
	mriData.mr = new short[img_width*img_height];
	mriData.u = new short[img_width*img_height];
	mriData.v = new short[img_width*img_height];
	mriData.w = new short[img_width*img_height];
	mriData.img_width = img_width;
	mriData.img_height = img_height;
	float lower, half;
	lower = (float)m_VortexRange;
	half = lower/2.0f;

	int x, y;
	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
		{
			mriData.u[img_width*y+x]=(short)floor((-lower/255.0)* y + half);
			mriData.v[img_width*y+x]=(short)floor((lower/255.0)* x - half);
			mriData.w[img_width*y+x]=0;
		}

	((CButton*)GetDlgItem(IDC_SHOW_MR))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_SHOW_U))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_SHOW_V))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_SHOW_W))->EnableWindow(TRUE);

}

void CDlgStreamlines::CopyTextureFP(float * texture, float * start)
{
	int x, y;
	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
			start[img_width*y+x] = texture[img_width*y+x];
}

void CDlgStreamlines::CopyTextureFP3D(float * texture, float * start)
{
	int x, y, z;
	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
			for(z=0; z<img_slices; z++)
				start[z*img_width*img_height+y*img_width+x]
				= texture[z*img_width*img_height+y*img_width+x];
}

void CDlgStreamlines::OnStrmResSave() 
{
	((CTextonView*)pView)->SaveResult();
	
}

void CDlgStreamlines::TextureLaplacian(float * laplacian)
{
	const BOOL bLib = FALSE;
	const BOOL bVtk = FALSE;
	
	if(!bLib)
	{
		const int lsize = 5;
		float coeffs[lsize*lsize]={-1.0f,-3.0f,-4.0f,-3.0f,-1.0f,-3.0f,0.0f,6.0f,
			0.0f,-3.0f,-4.0f,6.0f,20.0f,6.0f,-4.0f,-3.0f,0.0f,6.0f,0.0f,-3.0f,-1.0f,
			-3.0f,-4.0f,-3.0f,-1.0f};

		
/*		const int lsize = 3;
		float coeffs[lsize*lsize]={-1.0f,-1.0f,-1.0f,-1.0f,8.0f,-1.0f,-1.0f,-1.0f,-1.0f};
*/

/*
		float nx, ny;
		const float o2 = 16.0f;

		for(x=0; x<3; x++)
			for(y=0; y<3; y++)
				{
					nx = ((float)x)-(((float)lsize)-1.0f)/2.0;
					ny = ((float)y)-(((float)lsize)-1.0f)/2.0;
					
					coeffs[y*lsize+x] =
						(1.0f / o2) * ((nx*nx+ny*ny)/o2-2.0) * (float)exp(-(nx*nx+ny*ny)/(2*o2));
				}
*/
		// convolve with image
		int x, y;
		for(x=0; x<img_width; x++)
			for(y=0; y<img_height; y++)
				laplacian[y*img_width+x]=0.0;

		int cx, cy;
		for(x=2; x<img_width-2; x++)
			for(y=2; y<img_height-2; y++)
			{
				for(cx=0; cx<lsize; cx++)
					for(cy=0; cy<lsize; cy++)
					{
						laplacian[y*img_width+x]
							= laplacian[y*img_width+x]
							+ texture[(y-2+cy)*img_width+(x-2+cx)]
							* coeffs[cy*lsize+cx];
					}					
			}
	}
	else
	{
/*		if(!bVtk)
		{
			// create ipl source image
			IplImage *iplSource;
			iplSource = iplCreateImageHeader(1,0,IPL_DEPTH_16U,"GRAY","GRAY", 
				IPL_DATA_ORDER_PIXEL, IPL_ORIGIN_TL, IPL_ALIGN_DWORD,
				img_width, img_height, NULL, NULL, NULL, NULL );
			if( !iplSource ) return;
			iplAllocateImage( iplSource, 0, 0 );
			if( !iplSource->imageData ) return;
			
			// create ipl destination image
			IplImage *iplDest;
			iplDest = iplCreateImageHeader(1,0,IPL_DEPTH_16U,"GRAY","GRAY", 
				IPL_DATA_ORDER_PIXEL, IPL_ORIGIN_TL, IPL_ALIGN_DWORD,
				img_width, img_height, NULL, NULL, NULL, NULL );
			if( !iplDest ) return;
			iplAllocateImage( iplDest, 0, 0 );
			if( !iplDest->imageData ) return;
			
			// copy texture to source
			short buffer1;
			int x, y;
			for(x=0;x<img_width;x++)
				for(y=0;y<img_height;y++)
				{
					buffer1 = texture[img_width*y+x];
					iplPutPixel(iplSource, x, y, &buffer1);
				}
				
			// apply filter
			iplFixedFilter(iplSource, iplDest, IPL_LAPLACIAN_5x5);
			
			// copy image to array
			short buffer2[4];
			for(x=0;x<img_width;x++)
				for(y=0;y<img_height;y++)
				{
					iplGetPixel(iplDest, x, y, &buffer2);
					laplacian[img_width*y+x] = buffer2[0];
				}
				
			iplDeallocateImage(iplSource);
			iplDeallocateImage(iplDest);
		}
		else
		{

		}*/
	}	
}

void CDlgStreamlines::CopyTextureFPI(float * texture, int * start)
{
	int x, y;
	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
			start[img_width*y+x] = FloatToInt(texture[img_width*y+x]);
}

void CDlgStreamlines::CopyTextureI(int * texture, float * start)
{
	int x, y;
	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
			start[img_width*y+x] = (float)texture[img_width*y+x];
}

void CDlgStreamlines::CopyTextureI3D(int * texture, float * start)
{
	int x, y, z;
	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
			for(z=0; z<img_slices; z++)
				start[z*img_width*img_height+y*img_width+x]
				= (float)texture[z*img_width*img_height+y*img_width+x];
}

int CDlgStreamlines::FloatToInt(float fnum)
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

void CDlgStreamlines::OnStrmTxtBlur() 
{
/*	UpdateData(TRUE);

	// create ipl source image
	IplImage *iplSource;
	iplSource = iplCreateImageHeader(1,0,IPL_DEPTH_8U,"GRAY","GRAY", 
		IPL_DATA_ORDER_PIXEL, IPL_ORIGIN_TL, IPL_ALIGN_DWORD,
		img_width, img_height, NULL, NULL, NULL, NULL );
	if( !iplSource ) return;
	iplAllocateImage( iplSource, 0, 0 );
	if( !iplSource->imageData ) return;

	// create ipl destination image
	IplImage *iplDest;
	iplDest = iplCreateImageHeader(1,0,IPL_DEPTH_8U,"GRAY","GRAY", 
		IPL_DATA_ORDER_PIXEL, IPL_ORIGIN_TL, IPL_ALIGN_DWORD,
		img_width, img_height, NULL, NULL, NULL, NULL );
	if( !iplDest ) return;
	iplAllocateImage( iplDest, 0, 0 );
	if( !iplDest->imageData ) return;

	// copy texture to source
	char buffer1;
	int x, y;
	for(x=0;x<img_width;x++)
		for(y=0;y<img_height;y++)
		{
			buffer1 = FloatToInt(texture[img_width*y+x]);
			iplPutPixel(iplSource, x, y, &buffer1);
		}
	
	// apply filter
	iplBlur(iplSource, iplDest, 3, 3, 1, 1);
	
	// copy image to array
	int* dmy = new int[img_width*img_height];
	char buffer2[4];
	for(x=0;x<img_width;x++)
		for(y=0;y<img_height;y++)
		{
			iplGetPixel(iplDest, x, y, &buffer2);
			dmy[img_width*y+x] = 
				FloatToInt(//m_VelDelta * 
				sqrt(pow(mriData.u[img_width*y+x],2)
				  +pow(mriData.v[img_width*y+x],2)
				  +pow(mriData.w[img_width*y+x],2)) *
				buffer2[0] *
				0.0008);
		}

	iplDeallocateImage(iplSource);
	iplDeallocateImage(iplDest);

	((CTextonView*)pView)->DrawResultSpecial(dmy, img_width, img_height);

	delete [] dmy;
*/		
}

void CDlgStreamlines::OnStrmTxtMagn() 
{
	UpdateData(TRUE);

	if(!mode3D)
	{
		int* dmy = new int[img_width*img_height];
		int x, y;
		for(x=0;x<img_width;x++)
			for(y=0;y<img_height;y++)
			{
				dmy[img_width*y+x] = 
					FloatToInt(1 *
					sqrt(pow(mriData.u[img_width*y+x],2)
					+pow(mriData.v[img_width*y+x],2)
					/*+pow(mriData.w[img_width*y+x],2)*/) *
					texture[img_width*y+x]);
			}
			
		NormaliseInt(dmy, img_height, img_width);
		
		((CTextonView*)pView)->DrawResultSpecial(dmy, img_width, img_height);
		delete [] dmy;
	}
	else
	{
		int* dmy = new int[img_width*img_height*img_slices];
		int x, y, z;
		
		for(x=0; x<img_width; x++)
			for(y=0; y<img_height; y++)
				for(z=0; z<img_slices; z++)
				{
					dmy[z*img_width*img_height+y*img_width+x] = 
						FloatToInt(1 *
						sqrt(pow(mriData3D.u[z*img_width*img_height+y*img_width+x],2)
						+pow(mriData3D.v[z*img_width*img_height+y*img_width+x],2)
						+pow(mriData3D.w[z*img_width*img_height+y*img_width+x],2)) *
						texture3D[z*img_width*img_height+y*img_width+x]);
				}

		CDlgVolView* pVolDlg = new CDlgVolView;
		pVolDlg->pView = pView;
		pVolDlg->m_imgInfo.Format("image: (x, y, s) %i x %i x %i", mriData3D.img_width, 
			mriData3D.img_height, mriData3D.img_slices);
		pVolDlg->m_sVolumeTitle.Format("LIC with magnitude");
		pVolDlg->InitialiseVolume(dmy, mriData3D.img_width, mriData3D.img_height, mriData3D.img_slices);
		pVolDlg->Create(IDD_VOL_VIEW, this);
		pVolDlg->ShowWindow(SW_SHOW);				
		delete [] dmy;
	}



}

void CDlgStreamlines::BlurImage(int * input, int * output, int kernel_size)
{
/*	// create ipl source image
	IplImage *iplSource;
	iplSource = iplCreateImageHeader(1,0,IPL_DEPTH_8U,"GRAY","GRAY", 
		IPL_DATA_ORDER_PIXEL, IPL_ORIGIN_TL, IPL_ALIGN_DWORD,
		img_width, img_height, NULL, NULL, NULL, NULL );
	if( !iplSource ) return;
	iplAllocateImage( iplSource, 0, 0 );
	if( !iplSource->imageData ) return;

	// create ipl destination image
	IplImage *iplDest;
	iplDest = iplCreateImageHeader(1,0,IPL_DEPTH_8U,"GRAY","GRAY", 
		IPL_DATA_ORDER_PIXEL, IPL_ORIGIN_TL, IPL_ALIGN_DWORD,
		img_width, img_height, NULL, NULL, NULL, NULL );
	if( !iplDest ) return;
	iplAllocateImage( iplDest, 0, 0 );
	if( !iplDest->imageData ) return;

	// copy texture to source
	char buffer1;
	int x, y;
	for(x=0;x<img_width;x++)
		for(y=0;y<img_height;y++)
		{
			buffer1 = input[img_width*y+x];
			iplPutPixel(iplSource, x, y, &buffer1);
		}
	
	// apply filter
	iplBlur(iplSource, iplDest, kernel_size, kernel_size, FloatToInt(kernel_size/2.0),
		FloatToInt(kernel_size/2.0));
	
	// copy image to array
	char buffer2[4];
	for(x=0;x<img_width;x++)
		for(y=0;y<img_height;y++)
		{
			iplGetPixel(iplDest, x, y, &buffer2);
			output[img_width*y+x] = buffer2[0];
		}

	iplDeallocateImage(iplSource);
	iplDeallocateImage(iplDest);
*/
}

void CDlgStreamlines::BlurImage3D(int * input, int * output, int kernel_size)
{
	// TODO
}


void CDlgStreamlines::OnStrmZoom10() 
{
	// read by pView to capture clicks
	bZoom = FALSE;
	/*
	if(pDlgZoom!=NULL)
	{
		delete pDlgZoom;
		pDlgZoom = NULL;
	}
	CDlgStrmZoom* pZoom = new CDlgStrmZoom;
	pZoom->pDlgStrm = this;
	pZoom->Create(IDD_STRM_ZOOM, this);
	pZoom->ShowWindow(SW_SHOW);
	ShowWindow(SW_HIDE);
	pDlgZoom=pZoom;
	ZoomRestore();
	*/

}


void CDlgStreamlines::OnCancel() 
{
	((CTextonView*)pView)->StreamlinesClosed();

	if(texture != NULL)
		delete [] texture;

	if(noise_img != NULL)
		delete [] noise_img;

	if(m_iLaplacian != NULL)
		delete [] m_iLaplacian;

	if(m_seedPoints != NULL)
		delete [] m_seedPoints;

	CDialog::OnCancel();
}

void CDlgStreamlines::TakeDownPoint(CPoint clickedPoint)
{
	if(!bZoom)
		return;

	CString outCoords;
	outCoords.Format("%i, %i", clickedPoint.x, clickedPoint.y);

	if(pDlgZoom!=NULL)
	{
		((CDlgStrmZoom*)pDlgZoom)->TakeZoomText(outCoords);
		outCoords.Format("%i, %i", clickedPoint.x, clickedPoint.y);
		((CMainFrame*)AfxGetMainWnd())->SetStatusBarText(outCoords);
	}

	iZoomDown = clickedPoint;

}

void CDlgStreamlines::TakeUpPoint(CPoint clickedPoint)
{
	if(!bZoom)
		return;

	CString outCoords;
	outCoords.Format("%i, %i --> %i, %i (%i, %i)", iZoomDown.x, iZoomDown.y, 
		clickedPoint.x, clickedPoint.y, 
		clickedPoint.x-iZoomDown.x, clickedPoint.y-iZoomDown.y);

	if(pDlgZoom!=NULL)
	{
		((CDlgStrmZoom*)pDlgZoom)->TakeZoomText(outCoords);
		outCoords.Format("%i, %i", clickedPoint.x, clickedPoint.y);
		((CMainFrame*)AfxGetMainWnd())->SetStatusBarText(outCoords);
	}

	iZoomUp = clickedPoint;
}

void CDlgStreamlines::TakeMovePoint(CPoint clickedPoint)
{
	if(!bZoom)
		return;

	CString outCoords;
	outCoords.Format("%i, %i --> %i, %i (%i, %i)", iZoomDown.x, iZoomDown.y, 
		clickedPoint.x, clickedPoint.y, 
		clickedPoint.x-iZoomDown.x, clickedPoint.y-iZoomDown.y);

	if(pDlgZoom!=NULL)
	{
		((CDlgStrmZoom*)pDlgZoom)->TakeZoomText(outCoords);
		outCoords.Format("%i, %i", clickedPoint.x, clickedPoint.y);
		((CMainFrame*)AfxGetMainWnd())->SetStatusBarText(outCoords);
	}

	iZoomUp = clickedPoint;
}

void CDlgStreamlines::OnStrmTxtColour() 
{
	UpdateData(TRUE);

	if(!mode3D)
	{
		CImageColour imgColour;
		imgColour.AllocateData(img_width, img_height);
		
		float fred, fgreen, fblue;
		int* mag_img = new int[img_width*img_height];
		int* norm_text = new int[img_width*img_height];
		
		int x, y;
		for(x = 0; x<img_width; x++)
			for(y=0; y<img_height; y++)
				mag_img[img_width*y+x] = FloatToInt(sqrt(pow(mriData.u[img_width*y+x],2)
				+pow(mriData.v[img_width*y+x],2)
				/*+pow(mriData.w[img_width*y+x],2)*/));

		for(x = 0; x<img_width; x++)
			for(y=0; y<img_height; y++)
				norm_text[img_width*y+x] = FloatToInt(/*mag_img[img_width*y+x]**/texture[img_width*y+x]);

		NormaliseInt(norm_text, img_width, img_height);
		NormaliseInt(mag_img, img_width, img_height);
		//	EqualiseInt(norm_text, img_width, img_height);
		//	EqualiseInt(mag_img, img_width, img_height);

		for(x = 0; x<img_width; x++)
			for(y=0; y<img_height; y++)
				mag_img[img_width*y+x] = FloatToInt(mag_img[img_width*y+x]*m_MagMultiplier);

		for(x = 0; x<img_width; x++)
			for(y=0; y<img_height; y++)
				norm_text[img_width*y+x] = FloatToInt(norm_text[img_width*y+x]*m_IntMultiplier);

		for(x = 0; x<img_width; x++)
			for(y=0; y<img_height; y++)
			{	
				ColourCodeFromMagStream(mag_img[img_width*y+x], norm_text[img_width*y+x], 
					fred, fgreen, fblue);

				imgColour.imgData[img_width*y+x].RED = FloatToInt(fred);
				imgColour.imgData[img_width*y+x].GREEN = FloatToInt(fgreen);
				imgColour.imgData[img_width*y+x].BLUE = FloatToInt(fblue);
			}

		((CTextonView*)pView)->DrawColourImageSpecial(&imgColour);

		delete [] mag_img;
		delete [] norm_text;
	}
	else
	{
		CImageColour imgColour;
		imgColour.AllocateData3D(img_width, img_height, img_slices);
		
		float fred, fgreen, fblue;
		int* mag_img = new int[img_width*img_height*img_slices];
		int* norm_text = new int[img_width*img_height*img_slices];
		
		int x, y, z;
		for(x = 0; x<img_width; x++)
			for(y=0; y<img_height; y++)
				for(z=0; z<img_slices; z++)
					mag_img[z*img_width*img_height+y*img_width+x] = 
					FloatToInt(sqrt(
					 pow(mriData3D.u[z*img_width*img_height+y*img_width+x],2)
					+pow(mriData3D.v[z*img_width*img_height+y*img_width+x],2)
					+pow(mriData3D.w[z*img_width*img_height+y*img_width+x],2) ));

		for(x = 0; x<img_width; x++)
			for(y=0; y<img_height; y++)
				for(z=0; z<img_slices; z++)
					norm_text[z*img_width*img_height+y*img_width+x] = 
					FloatToInt(texture3D[z*img_width*img_height+y*img_width+x]);

		NormaliseInt3D(norm_text, img_width, img_height, img_slices);
		NormaliseInt3D(mag_img, img_width, img_height, img_slices);


		for(x = 0; x<img_width; x++)
			for(y=0; y<img_height; y++)
				for(z=0; z<img_slices; z++)
					mag_img[z*img_width*img_height+y*img_width+x] = 
					FloatToInt(mag_img[z*img_width*img_height+y*img_width+x]*m_MagMultiplier);


		for(x = 0; x<img_width; x++)
			for(y=0; y<img_height; y++)
				for(z=0; z<img_slices; z++)
					norm_text[z*img_width*img_height+y*img_width+x] = 
					FloatToInt(norm_text[z*img_width*img_height+y*img_width+x]*m_IntMultiplier);


		for(x = 0; x<img_width; x++)
			for(y=0; y<img_height; y++)
				for(z=0; z<img_slices; z++)
				{	
					ColourCodeFromMagStream(mag_img[z*img_width*img_height+y*img_width+x], 
						norm_text[z*img_width*img_height+y*img_width+x], 
						fred, fgreen, fblue);
					
					imgColour.imgData[z*img_width*img_height+y*img_width+x].RED = FloatToInt(fred);
					imgColour.imgData[z*img_width*img_height+y*img_width+x].GREEN = FloatToInt(fgreen);
					imgColour.imgData[z*img_width*img_height+y*img_width+x].BLUE = FloatToInt(fblue);
				}

		CDlgVolView* pVolDlg = new CDlgVolView;
		pVolDlg->pView = pView;
		pVolDlg->m_imgInfo.Format("image: (x, y, s) %i x %i x %i", mriData3D.img_width, 
			mriData3D.img_height, mriData3D.img_slices);
		pVolDlg->m_sVolumeTitle.Format("LIC output in colour");
		pVolDlg->InitialiseVolume(imgColour, mriData3D.img_width, mriData3D.img_height, 
			mriData3D.img_slices);
		pVolDlg->Create(IDD_VOL_VIEW, this);
		pVolDlg->ShowWindow(SW_SHOW);				

		delete [] mag_img;
		delete [] norm_text;
	}

}

void CDlgStreamlines::ColourCodeFromMagStream(float vel_mag, float stream, float & red, 
										  float & green, float & blue)
{

//	float intensity = ((vel_mag<20)?0:((200.0f/235.0f)*vel_mag+38.0f)/255.0)*(stream/255.0);
	float intensity = (vel_mag/255.0)*(stream/255.0);

	if(intensity > 1)
		intensity = 1;

	if(vel_mag < 256.0f && vel_mag > 128.0f)
	{
		red = ((255.0f/(255.0f-128.0f))*vel_mag-255.0f) * intensity;
		green = ((-255/(255.0f-128.0f))*vel_mag+510.0f) * intensity;
		blue = 0.0f;
	}
	else
		if(vel_mag <= 128.0f && vel_mag >= 0.0f)
		{
			red = 0.0f;
			green = (255.0f/128.0f)*vel_mag * intensity;
			blue = ((-255.0f/128.0f)*vel_mag+255.0f) * intensity;
		}
		else
		{
			red = 255.0f;
			green = 255.0f;
			blue = 255.0f;
		}

}

void CDlgStreamlines::OnStrmFlipU() 
{
	if(!mode3D)
	{
		int x, y;
		for(x=0; x<img_width;x++)
			for(y=0; y<img_height; y++)
				mriData.u[img_width*y+x] = mriData.u[img_width*y+x] * (-1.0f);
	}
	else
		mriData3D.FlipU();
}

void CDlgStreamlines::OnStrmFlipV() 
{
	if(!mode3D)
	{
		int x, y;
		for(x=0; x<img_width;x++)
			for(y=0; y<img_height; y++)
				mriData.v[img_width*y+x] = mriData.v[img_width*y+x] * (-1.0f);	
	}
	else
		mriData3D.FlipV();
}

void CDlgStreamlines::OnStrmFlipW() 
{
	if(!mode3D)
	{
		int x, y;
		for(x=0; x<img_width;x++)
			for(y=0; y<img_height; y++)
				mriData.w[img_width*y+x] = mriData.w[img_width*y+x] * (-1.0f);
	}
	else
		mriData3D.FlipW();
}

void CDlgStreamlines::ZoomOK()
{
	AfxMessageBox("This function is offline because IPL is not available.");
/*	ShowWindow(SW_SHOW);
	if(pDlgZoom!=NULL)
	{
		delete pDlgZoom;
		pDlgZoom = NULL;
	}

	// save original image data
	// (note ZoomRestore() was called by the Zoom or Restore buttons)
	ZoomSave();

//	iZoomDown.x = 130;
//	iZoomDown.y = 75;
//	iZoomUp.x = 183;
//	iZoomUp.y = 139;


	UpdateData(TRUE);
	mriData.Zoom(iZoomDown, iZoomUp, m_ZoomSize);
	img_height = mriData.img_height;
	img_width = mriData.img_width;

	// delete all texture data
	TextureReset();
	((CMainFrame*)AfxGetMainWnd())->SetStatusBarText("");

	OnShowMR();
	*/

}

void CDlgStreamlines::ZoomCancel()
{
	ShowWindow(SW_SHOW);
	if(pDlgZoom!=NULL)
	{
		delete pDlgZoom;
		pDlgZoom = NULL;
	}
	((CMainFrame*)AfxGetMainWnd())->SetStatusBarText("");
}

void CDlgStreamlines::ZoomRestore()
{
	// load original data into current
	// this function is called by this class's OnStrmZoom10() function which responds
	// to the "Zoom" button

	if(!savedDataValid)
	{
		OnShowMR();
		return;
	}

	// savedData -> mriData
	mriData.SetData(savedData.img_width, savedData.img_height, savedData.mr,
		savedData.u, savedData.v, savedData.w);
	img_height = mriData.img_height;
	img_width = mriData.img_width;

	OnShowMR();

}

void CDlgStreamlines::ZoomSave()
{
	// called by ZoomOk() before data is zoomed in

	// mriData -> savedData
	savedData.SetData(mriData.img_width, mriData.img_height, mriData.mr,
		mriData.u, mriData.v, mriData.w);

	savedDataValid = TRUE;
}

void CDlgStreamlines::NormaliseInt(int * in_out, int width, int height)
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
	float m, b;
	m=255.0/range;
	b=255.0-m*(float)max;

	for(x=0; x<width; x++)
		for(y=0; y<height; y++)
			in_out[width*y+x]=FloatToInt( in_out[width*y+x]*m+b+0.00001 );

}

void CDlgStreamlines::NormaliseInt3D(int * in_out, int width, int height, int slices)
{
	int min;
	int max;
	int x, y, z;

	min = max = in_out[0];

	for(x=0; x<width; x++)
		for(y=0; y<height; y++)
			for(z=0; z<slices; z++)
			{
				if(in_out[z*width*height+y*width+x]>max)
					max = in_out[z*width*height+y*width+x];
				
				if(in_out[z*width*height+y*width+x]<min)
					min = in_out[z*width*height+y*width+x];
			}

	int range = max - min;
	float m, b;
	m=254.0/range;
	b=254.0-m*(float)max;

	for(x=0; x<width; x++)
		for(y=0; y<height; y++)
			for(z=0; z<slices; z++)
				in_out[z*width*height+y*width+x]
				=FloatToInt( in_out[z*width*height+y*width+x]*m+b+0.00001 );

}

void CDlgStreamlines::EqualiseInt(int * img_data, int img_width, int img_height)
{
	/*
	IplImage *imga;
	const int range = 255;
	IplLUT lut = { range+1, NULL,NULL,NULL, IPL_LUT_LOOKUP };
	IplLUT* plut = &lut;
	int i;
	lut.key = (int*)malloc( sizeof(int)*(range+1) );
	lut.value = (int*)malloc( sizeof(int)*(range+1) );
	imga = iplCreateImageHeader(
		1, 0, IPL_DEPTH_8U, "GRAY", "GRAY",
		IPL_DATA_ORDER_PIXEL, IPL_ORIGIN_TL,
		IPL_ALIGN_DWORD, img_width, img_height, NULL, NULL,
		NULL, NULL);
	if( NULL == imga ) return;
	iplAllocateImage( imga, 0, 0 );
	if( NULL == imga->imageData ) return;
	// copy texture to source
	char buffer1;
	int x, y;
	for(x=0;x<img_width;x++)
		for(y=0;y<img_height;y++)
		{
			buffer1 = img_data[img_width*y+x];
			iplPutPixel(imga, x, y, &buffer1);
		}
	// Make the two level data
//	for( i=0; i<8; i++) ((char*)imga->imageData)[i] = (char)7;
	// Initialize the histogram levels
	for( i=0; i<=range; i++) lut.key[i] = i;
	// Compute histogram
	iplComputeHisto( imga, &plut );
	// Equalize histogram = rescale range of image data
//	iplHistoEqualize( imga, imga, &plut );
	// copy image to array
	char buffer2[4];
	for(x=0;x<img_width;x++)
		for(y=0;y<img_height;y++)
		{
			iplGetPixel(imga, x, y, &buffer2);
			img_data[img_width*y+x] = buffer2[0];
		}
	iplDeallocate( imga, IPL_IMAGE_HEADER | IPL_IMAGE_DATA );
	if( lut.key ) free( lut.key );
	if( lut.value ) free( lut.value );
	*/
}

void CDlgStreamlines::NoLoadInitialise(CData3DMRI *slice3D)
{
	if(iStrmMask != NULL)
	{
		delete [] iStrmMask;
		iStrmMask = NULL;
	}
	
	mode3D = TRUE;

	mriData3D.SetData(slice3D->img_width, slice3D->img_height, slice3D->img_slices, slice3D->mr,
		slice3D->u, slice3D->v, slice3D->w);

	img_width = slice3D->img_width;
	img_height = slice3D->img_height;
	img_slices = slice3D->img_slices;

	int* dmy = new int[img_width*img_height];
	int x, y, z;
	z = 0;
	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
			dmy[img_width*y+x] = mriData3D.mr[mriData3D.img_width*mriData3D.img_height*z +
					mriData3D.img_width*y + x]/10;

	((CTextonView*)pView)->DrawResultSpecial(dmy, img_width, img_height);
	((CButton*)GetDlgItem(IDC_SHOW_MR))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_SHOW_U))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_SHOW_V))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_SHOW_W))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_STRM_VORTEX))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_STRM_VTX_RANGE))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_STRM_LOAD))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_STRM_LOAD3D))->EnableWindow(FALSE);

	savedDataValid = FALSE;
	m_bPointCollect = FALSE;

	UpdateData(TRUE);
	m_3dImgSizesTxt.Format("image: (x, y, s) %i x %i x %i", img_width, 
		img_height, img_slices);
	UpdateData(FALSE);

}

void CDlgStreamlines::NoLoadInitialiseMask(CData3DMRI *slice3D, float* mask)
{
	mode3D = TRUE;

	mriData3D.SetData(slice3D->img_width, slice3D->img_height, slice3D->img_slices, slice3D->mr,
		slice3D->u, slice3D->v, slice3D->w);

	img_width = slice3D->img_width;
	img_height = slice3D->img_height;
	img_slices = slice3D->img_slices;

	if(iStrmMask != NULL)
		delete [] iStrmMask;

	iStrmMask = new int[img_width*img_height*img_slices];
	int x, y, z;
	for(x=0; x<mriData3D.img_width;x++)
		for(y=0; y<mriData3D.img_height; y++)
			for(z=0; z<mriData3D.img_slices; z++)
				iStrmMask[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
				= mask[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x];

	int* dmy = new int[img_width*img_height];
	z = 0;
	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
			dmy[img_width*y+x] = mriData3D.mr[mriData3D.img_width*mriData3D.img_height*z +
					mriData3D.img_width*y + x]/10;

	((CTextonView*)pView)->DrawResultSpecial(dmy, img_width, img_height);
	((CButton*)GetDlgItem(IDC_SHOW_MR))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_SHOW_U))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_SHOW_V))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_SHOW_W))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_STRM_VORTEX))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_STRM_VTX_RANGE))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_STRM_LOAD))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_STRM_LOAD3D))->EnableWindow(FALSE);

	savedDataValid = FALSE;
	m_bPointCollect = FALSE;

	UpdateData(TRUE);
	m_3dImgSizesTxt.Format("image: (x, y, s) %i x %i x %i", img_width, 
		img_height, img_slices);
	UpdateData(FALSE);

}

void CDlgStreamlines::NoLoadInitialise(CDataMRI * slice)
{
	if(iStrmMask != NULL)
	{
		delete [] iStrmMask;
		iStrmMask = NULL;
	}

	mode3D = FALSE;

	mriData.SetData(slice->img_width, slice->img_height, slice->mr,
		slice->u, slice->v, slice->w);

	img_width = slice->img_width;
	img_height = slice->img_height;
	img_slices = 1;

	int* dmy = new int[img_width*img_height];
	int x, y;
	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
			dmy[img_width*y+x] = mriData.mr[img_width*y+x]/10;

	((CTextonView*)pView)->DrawResultSpecial(dmy, img_width, img_height);
	((CButton*)GetDlgItem(IDC_SHOW_MR))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_SHOW_U))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_SHOW_V))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_SHOW_W))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_STRM_VORTEX))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_STRM_VTX_RANGE))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_STRM_LOAD))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_STRM_LOAD3D))->EnableWindow(FALSE);

	savedDataValid = FALSE;
	m_bPointCollect = FALSE;

	UpdateData(TRUE);
	m_3dImgSizesTxt.Format("image: (x, y, s) %i x %i x %i", img_width, 
		img_height, 1);
	UpdateData(FALSE);


}

void CDlgStreamlines::NoLoadShowMR()
{
	OnShowU();
}

void CDlgStreamlines::TakeMouseOverPoint(CPoint pointOver)
{
	if(!bZoom)
		return;

	CString outCoords;
	outCoords.Format("%i, %i", pointOver.x, pointOver.y);

	if(pDlgZoom!=NULL)
		((CMainFrame*)AfxGetMainWnd())->SetStatusBarText(outCoords);

}

void CDlgStreamlines::OnAlphaDyn() 
{
	UpdateData(TRUE);
	((CButton*)GetDlgItem(IDC_STRM_ALPHA))->EnableWindow(!m_bAlphaDyn);
}


void CDlgStreamlines::OnStrmLoad3d() 
{
	if(iStrmMask != NULL)
	{
		delete [] iStrmMask;
		iStrmMask = NULL;
	}

	// prompt user for path and load file
	CFileDialog* dDlg = new CFileDialog(FALSE,"*","mr001",OFN_CREATEPROMPT |
		OFN_HIDEREADONLY | OFN_NOREADONLYRETURN,
		_T("MRI CFD Data (*.*)|*.*||"), NULL);

	if(dDlg->DoModal()!=IDOK)
		return;

	mriData3D.LoadDataCFD(dDlg->GetPathName());
	img_height = mriData3D.img_height;
	img_width = mriData3D.img_width;
	img_slices = mriData3D.img_slices;
	delete dDlg;
	
	UpdateData(TRUE);
	m_3dImgSizesTxt.Format("image: (x, y, s) %i x %i x %i", mriData3D.img_width, 
		mriData3D.img_height, mriData3D.img_slices);
	UpdateData(FALSE);

	((CButton*)GetDlgItem(IDC_SHOW_MR))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_SHOW_U))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_SHOW_V))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_SHOW_W))->EnableWindow(TRUE);
	mode3D = TRUE;
	savedDataValid = FALSE;


}

void CDlgStreamlines::OnStrmNoiseGrid() 
{
	UpdateData(TRUE);
	
	if(img_width == 0 || img_height == 0 || img_slices == 0)
	{
		AfxMessageBox("Load image first.");
		return;
	}

	if(mode3D == FALSE)
	{
		int x, y;
		if(noise_img!=NULL)
			delete [] noise_img;
		noise_img = new int[img_width*img_height];

		for(x=0;x<img_width;x++)
			for(y=0;y<img_height;y++)
				if(x%m_NoiseMean==0 && y%m_NoiseMean==0)
					noise_img[img_width*y+x] = 1
					*(iStrmMask?((float)iStrmMask[img_width*y+x]):1.0f);
				else
					noise_img[img_width*y+x] = 0;
		
		((CTextonView*)pView)->DrawResultSpecial(noise_img, img_width, img_height);
		((CButton*)GetDlgItem(IDC_SHOW_NSE))->EnableWindow(TRUE);
	}
	else
	{
		int x, y, z;
		if(noise_img3D!=NULL)
			delete [] noise_img3D;
		noise_img3D = new int[img_width*img_height*img_slices];
		for(x=0; x<img_width; x++)
			for(y=0; y<img_height; y++)
				for(z=0; z<img_slices; z++)
					if(x%m_NoiseMean==0 && y%m_NoiseMean==0 && z%m_NoiseMean==0)
						noise_img3D[z*img_width*img_height+y*img_width+x] = 1
						* (iStrmMask?((float)iStrmMask[z*img_width*img_height+y*img_width+x]):1.0f);
					else
						noise_img3D[z*img_width*img_height+y*img_width+x] = 0;

		((CButton*)GetDlgItem(IDC_SHOW_NSE))->EnableWindow(TRUE);
	}
	
}

void CDlgStreamlines::OnStrmVtkStreamer() 
{

	// load flow data into vtkRectilinearGrid
	vtkFloatArray *fpoints = vtkFloatArray::New();
	vtkPoints* points = vtkPoints::New();
	vtkFloatArray *fspoints = vtkFloatArray::New();
	fspoints->SetNumberOfComponents(1);
	fpoints->SetNumberOfComponents(3);
	
	const float div = 1000.0f;
	int x, y, z;
	if(mode3D)
		for(x=0; x<img_width; x++)
			for(y=0; y<img_height; y++)
				for(z=0; z<img_slices; z++)
				{
					fspoints->InsertNextTuple1(sqrt(pow(mriData3D.u[z*img_width*img_height+y*img_width+x],2.0)+
						pow(mriData3D.v[z*img_width*img_height+y*img_width+x],2.0)+
						pow(mriData3D.w[z*img_width*img_height+y*img_width+x],2.0))/div);
					points->InsertNextPoint(x/10.0, y/10.0, z/10.0);
					//points->InsertNextPoint(x, y, z);
					fpoints->InsertNextTuple3(mriData3D.u[z*img_width*img_height+y*img_width+x]/div,
						mriData3D.v[z*img_width*img_height+y*img_width+x]/div,
						mriData3D.w[z*img_width*img_height+y*img_width+x]/div);

				}
		else
		{
			z = 0;
			for(x=0; x<img_width; x++)
				for(y=0; y<img_height; y++)
				{
					fspoints->InsertNextTuple1(sqrt(pow(mriData.u[z*img_width*img_height+y*img_width+x],2.0)+
						pow(mriData.v[z*img_width*img_height+y*img_width+x],2.0)+
						pow(mriData.v[z*img_width*img_height+y*img_width+x],2.0))/div);
					//points->InsertNextPoint(x, y, z);
					points->InsertNextPoint(x/10.0, y/10.0, z/10.0);
					fpoints->InsertNextTuple3(mriData.u[z*img_width*img_height+y*img_width+x]/div,
						mriData.v[z*img_width*img_height+y*img_width+x]/div, 
						mriData.w[z*img_width*img_height+y*img_width+x]/div);
				}
		}

	
	vtkStructuredGrid *sgrid = vtkStructuredGrid::New();
	//sgrid->SetDimensions(img_width/10.0, img_height/10.0, img_slices/10.0);
	sgrid->SetDimensions(img_width, img_height, img_slices);
	sgrid->SetPoints(points);
	sgrid->GetPointData()->SetScalars(fspoints);
	sgrid->GetPointData()->SetVectors(fpoints);
	
	vtkPolyData* attribs = vtkPolyData::New();
	attribs->Initialize();
	attribs->SetPoints(points);
	attribs->GetPointData()->SetScalars(fpoints);
	attribs->GetPointData()->SetVectors(fpoints);

	vtkRungeKutta4* integ = vtkRungeKutta4::New();
	vtkStreamLine* streamer = vtkStreamLine::New();
	vtkStructuredGridReader* reader = vtkStructuredGridReader::New();

	const BOOL bReader = FALSE;

	if(bReader)
	{
		reader->SetFileName("C:\\vtk40\\VTKData\\Data\\office.binary.vtk");
		reader->Update();

		sgrid->SetDimensions(reader->GetOutput()->GetDimensions());
		sgrid->SetPoints(reader->GetOutput()->GetPoints());
//		sgrid->GetPointData()->SetScalars(reader->GetOutput()->GetPointData()->GetScalars());
		sgrid->GetPointData()->SetVectors(reader->GetOutput()->GetPointData()->GetVectors());

		/*streamer->SetInput(reader->GetOutput());

		vtkStructuredGridWriter* swriter = vtkStructuredGridWriter::New();
		swriter->SetInput(reader->GetOutput());
		swriter->SetFileName("c:\\test.vtk");
		swriter->Write();
		*/

		streamer->SetInput(sgrid);

		vtkStructuredGridWriter* swriter = vtkStructuredGridWriter::New();
		swriter->SetInput(sgrid);
		swriter->SetFileName("c:\\test.vtk");
		swriter->Write();
	}
	else
	{
		streamer->SetInput(sgrid);
/*
		int* dmy = new int[img_width*img_height*img_slices];
		if(0)
			if(mode3D)
				for(x=0; x<img_width; x++)
					for(y=0; y<img_height; y++)
						for(z=0; z<img_slices; z++)
							dmy[z*img_width*img_height+y*img_width+x]=
							sqrt(pow(mriData3D.u[z*img_width*img_height+y*img_width+x],2.0)+
								pow(mriData3D.v[z*img_width*img_height+y*img_width+x],2.0)+
								pow(mriData3D.w[z*img_width*img_height+y*img_width+x],2.0));
			else
				for(x=0; x<img_width; x++)
					for(y=0; y<img_height; y++)
						dmy[y*img_width+x] = sqrt(pow(mriData.u[y*img_width+x],2.0) +
							pow(mriData.v[y*img_width+x],2.0));
		else
			if(mode3D)
				for(x=0; x<img_width; x++)
					for(y=0; y<img_height; y++)
						for(z=0; z<img_slices; z++)
							dmy[z*img_width*img_height+y*img_width+x]=
							texture3D[z*img_width*img_height+y*img_width+x]
							*sqrt(pow(mriData3D.u[z*img_width*img_height+y*img_width+x],2.0)+
								pow(mriData3D.v[z*img_width*img_height+y*img_width+x],2.0)+
								pow(mriData3D.w[z*img_width*img_height+y*img_width+x],2.0));
			else
				for(x=0; x<img_width; x++)
					for(y=0; y<img_height; y++)
						dmy[y*img_width+x] = texture[y*img_width+x]
						*sqrt(pow(mriData.u[y*img_width+x],2.0) +
							pow(mriData.v[y*img_width+x],2.0));

		NormaliseInt3D(dmy, img_width, img_height, img_slices);

		CVtkUtilities vtkutil;
		vtkutil.WriteImageVtk(dmy, img_width, img_height, img_slices, "c:\\test.vtk");
*/
		vtkStructuredGridWriter* swriter = vtkStructuredGridWriter::New();
		swriter->SetInput(sgrid);
		swriter->SetFileName("c:\\test.vtk");
		swriter->Write();

/*
		vtkImageData* sp = vtkImageData::New();
		sp->SetDimensions(sgrid->GetDimensions());
		sp->GetPointData()->SetScalars(fspoints);
*/
//		vtkStructuredPointsReader* spreader = vtkStructuredPointsReader::New();
//		spreader->SetFileName("c:\\vtk40\\VTKData\\Data\\ironProt.vtk");

/*		vtkStructuredPointsWriter* pwriter = vtkStructuredPointsWriter::New();
//		pwriter->SetInput(spreader->GetOutput());
		pwriter->SetInput(sp);
		pwriter->SetFileName("c:\\test.vtk");
		pwriter->Write();
*/
	}

	vtkPointSource* seeds = vtkPointSource::New();
    seeds->SetRadius(2.5);
    seeds->SetCenter(173,117,0);
    seeds->SetNumberOfPoints(50);
    //streamer->SetSource(seeds->GetOutput());
	streamer->SetStartPosition(1.1, 2.1, 0.5);
    streamer->SetMaximumPropagationTime(500);
    streamer->SetStepLength(0.5);
    streamer->SetIntegrationStepLength(0.05);
    streamer->SetIntegrationDirectionToIntegrateBothDirections();
    streamer->SetIntegrator(integ);

/*	vtkTubeFilter* streamTube = vtkTubeFilter::New();
	streamTube->SetInput(streamer->GetOutput());
	streamTube->SetRadius(0.02);
	streamTube->SetNumberOfSides(12);
	streamTube->SetVaryRadiusToVaryRadiusByVector();
	
	vtkPolyDataMapper* mapStreamTube = vtkPolyDataMapper::New();
	mapStreamTube->SetInput(streamTube->GetOutput());

//	if(bReader)
		mapStreamTube->SetScalarRange(reader->GetOutput()->GetPointData()->GetScalars()->GetRange());
	else
		mapStreamTube->SetScalarRange(sgrid->GetPointData()->GetScalars()->GetRange());
//	
	// The tube is wrapped around the generated streamline. By varying the radius
	// by the inverse of vector magnitude, we are creating a tube whose radius is
	// proportional to mass flux (in incompressible flow).

	vtkActor* streamTubeActor = vtkActor::New();
    streamTubeActor->SetMapper(mapStreamTube);
	streamTubeActor->GetProperty()->BackfaceCullingOn();

	// a renderer and render window
	vtkRenderer *ren1 = vtkRenderer::New();
	vtkWin32OpenGLRenderWindow *renWin = vtkWin32OpenGLRenderWindow::New();
	renWin->AddRenderer(ren1);
	
	// an interactor
	vtkWin32RenderWindowInteractor *iren = vtkWin32RenderWindowInteractor::New();
	iren->SetRenderWindow(renWin);
	
	// add the actor to the scene
	ren1->AddActor(streamTubeActor);
	ren1->SetBackground(1,1,1); // Background color white
	
	// render an image (lights and cameras are created automatically)
	//ren1->SetBackground(0,0,0);
	renWin->Render();
	
	// begin mouse interaction
	iren->Start();

	// prevent window from exiting FudgeBox
	iren->Disable();


*/

		
}

void CDlgStreamlines::OnStrmVtkLinemap() 
{

	vtkPoints* points = vtkPoints::New();
	vtkCellArray* cells = vtkCellArray::New();
	cells->InsertNextCell(streamlineTest.GetSize());

	int i;
	for(i=0; i<streamlineTest.GetSize(); i++)
	{
		points->InsertNextPoint(streamlineTest[i].x, streamlineTest[i].y, streamlineTest[i].z);
		cells->InsertCellPoint(i);
	}

/*
	points->InsertNextPoint(0,0,0);
	points->InsertNextPoint(0,0,1);
	points->InsertNextPoint(0,1,1);
	points->InsertNextPoint(1,1,1);
	points->InsertNextPoint(1,1,0);
	points->InsertNextPoint(1,0,0);
	points->InsertNextPoint(1,1,0);
	points->InsertNextPoint(1,1,1);
	cells->InsertNextCell(8);
	cells->InsertCellPoint(0);
	cells->InsertCellPoint(1);
	cells->InsertCellPoint(2);
	cells->InsertCellPoint(3);
	cells->InsertCellPoint(4);
	cells->InsertCellPoint(5);
	cells->InsertCellPoint(6);
	cells->InsertCellPoint(7);
*/
	vtkPolyData* polydata = vtkPolyData::New();
	polydata->SetPoints(points);
	polydata->SetLines(cells);
	
	vtkTubeFilter* streamTube = vtkTubeFilter::New();
	streamTube->SetInput(polydata);
	streamTube->SetRadius(0.2);
	streamTube->SetNumberOfSides(12);
	//streamTube->SetVaryRadiusToVaryRadiusByVector();
	
	vtkPolyDataMapper* mapStreamTube = vtkPolyDataMapper::New();
	mapStreamTube->SetInput(streamTube->GetOutput());
	//mapStreamTube->SetInput(polydata);
	//mapStreamTube->SetScalarRange(sgrid->GetPointData()->GetScalars()->GetRange());
	
	vtkActor* streamTubeActor = vtkActor::New();
    streamTubeActor->SetMapper(mapStreamTube);
	streamTubeActor->GetProperty()->BackfaceCullingOn();

	// a renderer and render window
	vtkRenderer *ren1 = vtkRenderer::New();
	vtkWin32OpenGLRenderWindow *renWin = vtkWin32OpenGLRenderWindow::New();
	renWin->AddRenderer(ren1);
	
	// an interactor
	vtkWin32RenderWindowInteractor *iren = vtkWin32RenderWindowInteractor::New();
	iren->SetRenderWindow(renWin);
	
	// add the actor to the scene
	ren1->AddActor(streamTubeActor);
	ren1->SetBackground(1,1,1); // Background color white
	
	// render an image (lights and cameras are created automatically)
	//ren1->SetBackground(0,0,0);
	renWin->Render();
	
	// begin mouse interaction
	iren->Start();

	// prevent window from exiting FudgeBox
	iren->Disable();

	
}

void CDlgStreamlines::AddArrayPoint(float x, float y, float z, CArray<CPointFloat3D, CPointFloat3D> &array)
{
	int i;
	BOOL bFound = FALSE;
	for(i=0; i<array.GetSize(); i++)
		if(array[i].x==x && array[i].y==y && array[i].z==z)
			bFound = TRUE;

	if(!bFound)
		array.Add(CPointFloat3D(x, y, z));

}

void CDlgStreamlines::OnStrmViewLaplacian() 
{
	if(!mode3D)
	{
		int* dmy;
		dmy = new int[img_width*img_height];
		int x, y;
		for(x=0; x<img_width; x++)
			for(y=0; y<img_height; y++)
				dmy[img_width*y+x] = FloatToInt(m_iLaplacian[img_width*y+x]*(255.0f/3600.0f)+128.0f);

		NormaliseInt(dmy, img_width, img_height);			
		((CTextonView*)pView)->DrawResultSpecial(dmy, img_width, img_height);
		delete [] dmy;
	}
	else
	{
		CDlgVolView* pVolDlg = new CDlgVolView;
		pVolDlg->pView = pView;
		pVolDlg->m_imgInfo.Format("image: (x, y, s) %i x %i x %i", mriData3D.img_width, 
			mriData3D.img_height, mriData3D.img_slices);
		pVolDlg->m_sVolumeTitle.Format("velocity data in U direction");
		pVolDlg->InitialiseVolume(m_iLaplacian, mriData3D.img_width, mriData3D.img_height, mriData3D.img_slices);
		pVolDlg->Create(IDD_VOL_VIEW, this);
		pVolDlg->ShowWindow(SW_SHOW);				
	}
	
}

void CDlgStreamlines::OnStrmVtkStreamerSaveLic() 
{
	int x, y, z;
	int* dmy = new int[img_width*img_height*img_slices];
	if(0)
		if(mode3D)
			for(x=0; x<img_width; x++)
				for(y=0; y<img_height; y++)
					for(z=0; z<img_slices; z++)
						dmy[z*img_width*img_height+y*img_width+x]=
						sqrt(pow(mriData3D.u[z*img_width*img_height+y*img_width+x],2.0)+
							pow(mriData3D.v[z*img_width*img_height+y*img_width+x],2.0)+
							pow(mriData3D.w[z*img_width*img_height+y*img_width+x],2.0));
		else
			for(x=0; x<img_width; x++)
				for(y=0; y<img_height; y++)
					dmy[y*img_width+x] = sqrt(pow(mriData.u[y*img_width+x],2.0) +
						pow(mriData.v[y*img_width+x],2.0));
	else
		if(mode3D)
			for(x=0; x<img_width; x++)
				for(y=0; y<img_height; y++)
					for(z=0; z<img_slices; z++)
						dmy[z*img_width*img_height+y*img_width+x]=
						texture3D[z*img_width*img_height+y*img_width+x]
						*sqrt(pow(mriData3D.u[z*img_width*img_height+y*img_width+x],2.0)+
							pow(mriData3D.v[z*img_width*img_height+y*img_width+x],2.0)+
							pow(mriData3D.w[z*img_width*img_height+y*img_width+x],2.0));
		else
			for(x=0; x<img_width; x++)
				for(y=0; y<img_height; y++)
					dmy[y*img_width+x] = texture[y*img_width+x]
					*sqrt(pow(mriData.u[y*img_width+x],2.0) +
						pow(mriData.v[y*img_width+x],2.0));

	NormaliseInt3D(dmy, img_width, img_height, img_slices);

	CVtkUtilities vtkutil;
	vtkutil.WriteImageVtk(dmy, img_width, img_height, img_slices, "c:\\test.vtk");
	
}

/*
// this function for producing CARS 3D plot
void CDlgStreamlines::OnStrmVtkArrows3d() 
{
	// load flow data into vtkRectilinearGrid
	vtkFloatArray *fpoints = vtkFloatArray::New();
	vtkPoints* points = vtkPoints::New();
	vtkFloatArray *fspoints = vtkFloatArray::New();
	fspoints->SetNumberOfComponents(1);
	fpoints->SetNumberOfComponents(3);
	
	const float div = 1000.0f;
	int x, y, z;
	if(mode3D)
	{

		// x plane
		x=22;
		for(y=12; y<img_height; y++)
			for(z=FloatToInt(img_slices/2.0); z<img_slices; z++)
			{
				fspoints->InsertNextTuple1(sqrt(pow(mriData3D.u[z*img_width*img_height+y*img_width+x],2.0)+
					pow(mriData3D.v[z*img_width*img_height+y*img_width+x],2.0)+
					pow(mriData3D.w[z*img_width*img_height+y*img_width+x],2.0))/div);
				points->InsertNextPoint(x/10.0, y/10.0, z/10.0);
				fpoints->InsertNextTuple3(mriData3D.u[z*img_width*img_height+y*img_width+x]/div,
					mriData3D.v[z*img_width*img_height+y*img_width+x]/div,
					mriData3D.w[z*img_width*img_height+y*img_width+x]/div);

			}

		// y plane
		y=12;
		for(x=22; x<img_width-10; x++)
			for(z=FloatToInt(img_slices/2.0); z<img_slices; z++)
			{
				fspoints->InsertNextTuple1(sqrt(pow(mriData3D.u[z*img_width*img_height+y*img_width+x],2.0)+
					pow(mriData3D.v[z*img_width*img_height+y*img_width+x],2.0)+
					pow(mriData3D.w[z*img_width*img_height+y*img_width+x],2.0))/div);
				points->InsertNextPoint(x/10.0, y/10.0, z/10.0);
				fpoints->InsertNextTuple3(mriData3D.u[z*img_width*img_height+y*img_width+x]/div,
					mriData3D.v[z*img_width*img_height+y*img_width+x]/div,
					mriData3D.w[z*img_width*img_height+y*img_width+x]/div);

			}

		// z plane
		z=FloatToInt(img_slices/2.0);
		for(x=22; x<img_width-10; x++)
			for(y=12; y<img_height; y++)
			{
				fspoints->InsertNextTuple1(sqrt(pow(mriData3D.u[z*img_width*img_height+y*img_width+x],2.0)+
					pow(mriData3D.v[z*img_width*img_height+y*img_width+x],2.0)+
					pow(mriData3D.w[z*img_width*img_height+y*img_width+x],2.0))/div);
				points->InsertNextPoint(x/10.0, y/10.0, z/10.0);
				fpoints->InsertNextTuple3(mriData3D.u[z*img_width*img_height+y*img_width+x]/div,
					mriData3D.v[z*img_width*img_height+y*img_width+x]/div,
					mriData3D.w[z*img_width*img_height+y*img_width+x]/div);

			}
	}
	
	vtkStructuredGrid *sgrid = vtkStructuredGrid::New();
	sgrid->SetDimensions(img_width, img_height, img_slices);
	sgrid->SetPoints(points);
	sgrid->GetPointData()->SetScalars(fspoints);
	sgrid->GetPointData()->SetVectors(fpoints);

	vtkStructuredGridWriter* swriter = vtkStructuredGridWriter::New();
	swriter->SetInput(sgrid);
	swriter->SetFileName("c:\\test3D.vtk");
	swriter->Write();

	fpoints->Delete();
	fspoints->Delete();
	points->Delete();
	sgrid->Delete();
	swriter->Delete();
	
}
*/

// this function for producing MICCAI 3D plot
void CDlgStreamlines::OnStrmVtkArrows3d() 
{
	// load flow data into vtkRectilinearGrid
	vtkFloatArray *fpoints = vtkFloatArray::New();
	vtkPoints* points = vtkPoints::New();
	vtkFloatArray *fspoints = vtkFloatArray::New();
	fspoints->SetNumberOfComponents(1);
	fpoints->SetNumberOfComponents(3);
	
	const float div = 1000.0f;
	int x, y, z;
	if(mode3D)
	{

		// x plane (yz)
		x=12;
		for(y=0; y<85; y++)
			for(z=5; z<img_slices; z++)
				if(mriData3D.u[z*img_width*img_height+y*img_width+x]!=0 && 
				   mriData3D.v[z*img_width*img_height+y*img_width+x]!=0 && 
				   mriData3D.w[z*img_width*img_height+y*img_width+x]!=0)
			{
				fspoints->InsertNextTuple1(sqrt(pow(mriData3D.u[z*img_width*img_height+y*img_width+x],2.0)+
					pow(mriData3D.v[z*img_width*img_height+y*img_width+x],2.0)+
					pow(mriData3D.w[z*img_width*img_height+y*img_width+x],2.0))/div);
				points->InsertNextPoint(x, y, z);
				fpoints->InsertNextTuple3(mriData3D.u[z*img_width*img_height+y*img_width+x]/div,
					mriData3D.v[z*img_width*img_height+y*img_width+x]/div,
					mriData3D.w[z*img_width*img_height+y*img_width+x]/div);

			}

		// y plane (xz)
		y=84;
		for(x=0; x<12; x++)
			for(z=5; z<img_slices; z++)
				if(mriData3D.u[z*img_width*img_height+y*img_width+x]!=0 && 
				   mriData3D.v[z*img_width*img_height+y*img_width+x]!=0 && 
				   mriData3D.w[z*img_width*img_height+y*img_width+x]!=0)
			{
				fspoints->InsertNextTuple1(sqrt(pow(mriData3D.u[z*img_width*img_height+y*img_width+x],2.0)+
					pow(mriData3D.v[z*img_width*img_height+y*img_width+x],2.0)+
					pow(mriData3D.w[z*img_width*img_height+y*img_width+x],2.0))/div);
				points->InsertNextPoint(x, y, z);
				fpoints->InsertNextTuple3(mriData3D.u[z*img_width*img_height+y*img_width+x]/div,
					mriData3D.v[z*img_width*img_height+y*img_width+x]/div,
					mriData3D.w[z*img_width*img_height+y*img_width+x]/div);

			}

		// z plane (xy)
		z=5;
		for(x=0; x<12; x++)
			for(y=0; y<85; y++)
				if(mriData3D.u[z*img_width*img_height+y*img_width+x]!=0 && 
				   mriData3D.v[z*img_width*img_height+y*img_width+x]!=0 && 
				   mriData3D.w[z*img_width*img_height+y*img_width+x]!=0)
			{
				fspoints->InsertNextTuple1(sqrt(pow(mriData3D.u[z*img_width*img_height+y*img_width+x],2.0)+
					pow(mriData3D.v[z*img_width*img_height+y*img_width+x],2.0)+
					pow(mriData3D.w[z*img_width*img_height+y*img_width+x],2.0))/div);
				points->InsertNextPoint(x, y, z);
				fpoints->InsertNextTuple3(mriData3D.u[z*img_width*img_height+y*img_width+x]/div,
					mriData3D.v[z*img_width*img_height+y*img_width+x]/div,
					mriData3D.w[z*img_width*img_height+y*img_width+x]/div);

			}
	}
	
	vtkStructuredGrid *sgrid = vtkStructuredGrid::New();
	sgrid->SetDimensions(img_width, img_height, img_slices);
	sgrid->SetPoints(points);
	sgrid->GetPointData()->SetScalars(fspoints);
	sgrid->GetPointData()->SetVectors(fpoints);

	vtkStructuredGridWriter* swriter = vtkStructuredGridWriter::New();
	swriter->SetInput(sgrid);
	swriter->SetFileName("c:\\test3D.vtk");
	swriter->Write();

	fpoints->Delete();
	fspoints->Delete();
	points->Delete();
	sgrid->Delete();
	swriter->Delete();
	
}

void CDlgStreamlines::OnStrmTxtSeed() 
{
	UpdateData(TRUE);

	if(!mode3D)
	{
		int* dmy = new int[img_width*img_height];
		int x, y;
		for(x=0;x<img_width;x++)
			for(y=0;y<img_height;y++)
			{
				dmy[img_width*y+x] = 
					FloatToInt(1.0 *
					sqrt(pow(mriData.u[img_width*y+x],2)
					+pow(mriData.v[img_width*y+x],2)
					/*+pow(mriData.w[img_width*y+x],2)*/) *
					texture[img_width*y+x]);
			}
			
		NormaliseInt(dmy, img_height, img_width);

		int i;
		for(i=0; i<streamlineTest.GetSize(); i++)
		{
			if(FloatToInt(streamlineTest[i].y) < img_height 
				&& FloatToInt(streamlineTest[i].x) < img_width)
			dmy[img_width*FloatToInt(streamlineTest[i].y)
				+FloatToInt(streamlineTest[i].x)] = 100;
		}
		
		((CTextonView*)pView)->DrawResultSpecial(dmy, img_width, img_height);
		delete [] dmy;
	}
	else
	{
		int* dmy = new int[img_width*img_height*img_slices];
		int x, y, z;
		
		for(x=0; x<img_width; x++)
			for(y=0; y<img_height; y++)
				for(z=0; z<img_slices; z++)
				{
					dmy[z*img_width*img_height+y*img_width+x] = 
						FloatToInt(1.0 *
						sqrt(pow(mriData3D.u[z*img_width*img_height+y*img_width+x],2)
						+pow(mriData3D.v[z*img_width*img_height+y*img_width+x],2)
						+pow(mriData3D.w[z*img_width*img_height+y*img_width+x],2)) *
						texture3D[z*img_width*img_height+y*img_width+x]);
				}

		int i;
		for(i=0; i<streamlineTest.GetSize(); i++)
		{
			if(FloatToInt(streamlineTest[i].y) < img_height 
				&& FloatToInt(streamlineTest[i].x) < img_width
				&& FloatToInt(streamlineTest[i].z) < img_slices)
			dmy[  img_width*img_height*FloatToInt(streamlineTest[i].z)
				+ img_width*FloatToInt(streamlineTest[i].y)
				+ FloatToInt(streamlineTest[i].x)] = 100000;
		}

		CDlgVolView* pVolDlg = new CDlgVolView;
		pVolDlg->pView = pView;
		pVolDlg->m_imgInfo.Format("image: (x, y, s) %i x %i x %i", mriData3D.img_width, 
			mriData3D.img_height, mriData3D.img_slices);
		pVolDlg->m_sVolumeTitle.Format("LIC with magnitude");
		pVolDlg->InitialiseVolume(dmy, mriData3D.img_width, mriData3D.img_height, mriData3D.img_slices);
		pVolDlg->Create(IDD_VOL_VIEW, this);
		pVolDlg->ShowWindow(SW_SHOW);				
		delete [] dmy;
	}


	
}

int CDlgStreamlines::TextureIteration3D(float* start, int alpha, float lapw)
{
	int n, x, y, z, ix, iy, iz;
	float newx, newy, newz, prevx, prevy, prevz;
	float vel_factor = m_VelDelta;
	float vel_magn;
	int term_count;
	double tex_temp;
	float deltas;

	// allocate new streamline array
	m_nPointArrays = 0; // for adding new arrays to the array
	BOOL currArrayUsed = FALSE;
	if(m_bPointCollect==TRUE)
		if(m_nSeedPoints>0)
		{
			if(streamlineArray != NULL)
				delete [] streamlineArray;

			streamlineArray = new CPointFloatArray[m_nSeedPoints];

			if(m_seedPoints == NULL)
			{
				AfxMessageBox("Error: m_seedPoints = NULL;");
				return 1;
			}
		}
		else
		{
			AfxMessageBox("Warning: zero seed points.");
			m_bPointCollect = FALSE;
		}

	/*
	// seed generation for vtk
	//	start[m_strmStartSeedZ*img_width*img_height+m_strmStartSeedY*img_width+m_strmStartSeedX]=1;
	int seed_x, seed_y, seed_z;
	for(seed_z=7; seed_z<9; seed_z++)
		for(seed_y=0; seed_y<27; seed_y=seed_y+3)
			for(seed_x=0; seed_x<35; seed_x=seed_x+3)
				start[seed_z*img_width*img_height+seed_y*img_width+seed_x]=1;
	*/

	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
			for(z=0; z<img_slices; z++)
			{
				if(m_bPointCollect==TRUE)
					if(m_seedPoints[z*img_width*img_height+y*img_width+x]!=1)
						continue;

				if(m_bAlphaDyn)
					alpha = FloatToInt(ceil(5.0f+m_dynAlpha*sqrt(pow(mriData3D.u[z*img_width*img_height+y*img_width+x],2)
					+pow(mriData3D.v[z*img_width*img_height+y*img_width+x],2)+pow(mriData3D.w[z*img_width*img_height+y*img_width+x],2))));
				// this should map velocity [0-2555] into [5-25] 
				// calling function has called UpdateData() already for m_bAlphaDyn
				
				tex_temp = 0;
				term_count = 0;
				
				prevx = x+0.5;
				prevy = y+0.5;
				prevz = img_slices>1? z+0.5 :0;
				
				// positive side
				for(n=1; n<=alpha; n++)
				{
					ix = (int)(floor)(prevx);
					iy = (int)(floor)(prevy);
					iz = (int)(floor)(prevz);
					vel_magn = (float)sqrt(pow(mriData3D.u[iz*img_width*img_height+iy*img_width+ix],2)
						+pow(mriData3D.v[iz*img_width*img_height+iy*img_width+ix],2)
						+pow(mriData3D.w[iz*img_width*img_height+iy*img_width+ix],2));
					deltas = CalcDeltaS3D(1, ix, iy, iz, prevx, prevy, prevz,
						mriData3D.u[iz*img_width*img_height+iy*img_width+ix], 
						mriData3D.v[iz*img_width*img_height+iy*img_width+ix], 
						mriData3D.w[iz*img_width*img_height+iy*img_width+ix], vel_magn);
					if(deltas == -1)
						continue;
					newx = prevx + mriData3D.u[iz*img_width*img_height+iy*img_width+ix]*(1.0f/vel_magn)*deltas;
					newy = prevy + mriData3D.v[iz*img_width*img_height+iy*img_width+ix]*(1.0f/vel_magn)*deltas;
					newz = prevz + mriData3D.w[iz*img_width*img_height+iy*img_width+ix]*(1.0f/vel_magn)*deltas;
					if(newx>=img_width)newx=img_width-1;
					if(newy>=img_height)newy=img_height-1;
					if(newz>=img_slices)newz=img_slices-1;
					if(newx<0)newx=0;
					if(newy<0)newy=0;
					if(newz<0)newz=0;
					
					ix = FloatToInt(newx);
					iy = FloatToInt(newy);
					iz = FloatToInt(newz);

					// case added 3/3/04 to eliminate streamlines along border
					if(!(img_slices > 1))
					{
						if(ix>=img_width-1 || iy >= img_height-1 || iz >= img_slices || 
						   ix <= 0 || iy <= 0 || iz < 0)
							break; // changed from continue to break 3/3/04
					}
					else
					{
						if(ix>=img_width-1 || iy >= img_height-1 || iz >= img_slices-1 || 
						   ix <= 0 || iy <= 0 || iz <= 0)
							break;
					}

					/*
					// streamlineTest
					if(start[z*img_width*img_height+y*img_width+x]==1)
						AddArrayPoint(ix, iy, iz, streamlineTest);
					*/

					// add seeded point to current streamline array
					if(m_bPointCollect==TRUE)
					{
						if(m_seedPoints[z*img_width*img_height+y*img_width+x]==1)
						{
							if(m_nPointArrays >= m_nSeedPoints)
							{
								CString sms133;
								sms133.Format("Error: m_nPointArrays = %i > m_nSeedPoints = %i", 
									m_nPointArrays, m_nSeedPoints);
								AfxMessageBox(sms133);
								m_bPointCollect=FALSE;
							}
							else
							{
								streamlineArray[m_nPointArrays].pointArray.
									Add(CPointFloat3D(newx, newy, newz));
								currArrayUsed = TRUE;
							}
						}
					}

					tex_temp = tex_temp + start[iz*img_width*img_height+iy*img_width+ix];
					term_count++;
					
					prevx = newx;
					prevy = newy;
					prevz = newz;
					
				}

				if(m_bPointCollect==TRUE && currArrayUsed==TRUE)
				{
					m_nPointArrays++;
					currArrayUsed=FALSE;
				}
				
				prevx = x+0.5;
				prevy = y+0.5;
				prevz = img_slices>1? z+0.5 :0;

				// negative side
				for(n=1; n<=alpha; n++)
				{
					ix = (int)(floor)(prevx);
					iy = (int)(floor)(prevy);
					iz = (int)(floor)(prevz);
					vel_magn = (float)sqrt(pow(mriData3D.u[iz*img_width*img_height+iy*img_width+ix],2)
						+pow(mriData3D.v[iz*img_width*img_height+iy*img_width+ix],2)
						+pow(mriData3D.w[iz*img_width*img_height+iy*img_width+ix],2));
					deltas = CalcDeltaS3D(1, ix, iy, iz, prevx, prevy, prevz,
						(-1.0f)*mriData3D.u[iz*img_width*img_height+iy*img_width+ix], 
						(-1.0f)*mriData3D.v[iz*img_width*img_height+iy*img_width+ix], 
						(-1.0f)*mriData3D.w[iz*img_width*img_height+iy*img_width+ix], vel_magn);
					if(deltas == -1)
						break;
					newx = prevx - mriData3D.u[iz*img_width*img_height+iy*img_width+ix]*(1.0f/vel_magn)*deltas;
					newy = prevy - mriData3D.v[iz*img_width*img_height+iy*img_width+ix]*(1.0f/vel_magn)*deltas;
					newz = prevz - mriData3D.w[iz*img_width*img_height+iy*img_width+ix]*(1.0f/vel_magn)*deltas;
					if(newx>=img_width)newx=img_width-1;
					if(newy>=img_height)newy=img_height-1;
					if(newz>=img_slices)newz=img_slices-1;
					if(newx<0)newx=0;
					if(newy<0)newy=0;
					if(newz<0)newz=0;
					
					ix = FloatToInt(newx);
					iy = FloatToInt(newy);
					iz = FloatToInt(newz);
					// case added 3/3/04 to eliminate streamlines along border
					if(!(img_slices > 1))
					{
						if(ix>=img_width-1 || iy >= img_height-1 || iz >= img_slices || 
						   ix <= 0 || iy <= 0 || iz < 0)
							break; // changed from continue to break 3/3/04
					}
					else
					{
						if(ix>=img_width-1 || iy >= img_height-1 || iz >= img_slices-1 || 
						   ix <= 0 || iy <= 0 || iz <= 0)
							break;
					}


					// streamlineTest
			//		if(start[z*img_width*img_height+y*img_width+x]==1)
			//			AddArrayPoint(ix, iy, iz, streamlineTest);

					tex_temp = tex_temp + start[iz*img_width*img_height+iy*img_width+ix];
					term_count++;
					
					prevx = newx;
					prevy = newy;
					prevz = newz;
					
				}
				// zero tau part
				tex_temp = tex_temp + start[z*img_width*img_height+y*img_width+x];
				term_count++;
				
				// normalisation
				texture3D[z*img_width*img_height+y*img_width+x] = tex_temp/(2.0f*(float)alpha+1.0f);
				
			}

	
	// take laplacian
	float* text_lap = new float[img_width*img_height*img_slices];
	TextureLaplacian3D(text_lap);

	if(m_iLaplacian != NULL)
		delete [] m_iLaplacian;

	m_iLaplacian = new int[img_width*img_height*img_slices];

	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
			for(z=0; z<img_slices; z++)
			{
				m_iLaplacian[z*img_width*img_height+y*img_width+x]=
					FloatToInt(text_lap[z*img_width*img_height+y*img_width+x]);

				if(m_iLaplacian[z*img_width*img_height+y*img_width+x]>0)
				texture3D[z*img_width*img_height+y*img_width+x] = 
					texture3D[z*img_width*img_height+y*img_width+x] + 
					lapw*(float)m_iLaplacian[z*img_width*img_height+y*img_width+x];
			}
		
	delete [] text_lap;

	// streamlineTest
	/*
	CString sms;
	sms.Format("array has %i elements", streamlineTest.GetSize());
	AfxMessageBox(sms);
	*/

	return 0;
}

void CDlgStreamlines::NoLoadInitialisePointslist(CData3DMRI *slice3D, int *seeds, int nSeeds)
{
	mode3D = TRUE;

	mriData3D.SetData(slice3D->img_width, slice3D->img_height, slice3D->img_slices, slice3D->mr,
		slice3D->u, slice3D->v, slice3D->w);

	img_width = slice3D->img_width;
	img_height = slice3D->img_height;
	img_slices = slice3D->img_slices;

	if(m_seedPoints == NULL)
		delete [] m_seedPoints;

	m_seedPoints = new int[img_width*img_height*img_slices];
	int x, y, z;
	for(x=0; x<mriData3D.img_width;x++)
		for(y=0; y<mriData3D.img_height; y++)
			for(z=0; z<mriData3D.img_slices; z++)
				m_seedPoints[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
				= seeds[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x];

	int* dmy = new int[img_width*img_height];
	z = 0;
	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
			dmy[img_width*y+x] = mriData3D.mr[mriData3D.img_width*mriData3D.img_height*z +
					mriData3D.img_width*y + x]/10;

	((CTextonView*)pView)->DrawResultSpecial(dmy, img_width, img_height);

	savedDataValid = FALSE;
	m_bPointCollect = TRUE;
	m_nSeedPoints = nSeeds;

	if(nSeeds==1)
		m_3dImgSizesTxt.Format("image: (x, y, s) %i x %i x %i   1 seed point", img_width, 
			img_height, img_slices);
	else
		m_3dImgSizesTxt.Format("image: (x, y, s) %i x %i x %i   %i seed points", img_width, 
			img_height, img_slices, nSeeds);

	m_RangeAlpha = 100;
	m_NumberIterations = 1;
	m_nPointArrays=0;




}

int CDlgStreamlines::GetPointlist(CPointFloatArray* arrayToCopy)
{
	for(int i = 0; i<m_nPointArrays; i++)
		arrayToCopy[i].pointArray.Copy(streamlineArray[i].pointArray);

	return m_nPointArrays;

}

int* CDlgStreamlines::GetStrmTxtMagn()
{
	UpdateData(TRUE);

	if(!mode3D)
	{
		int* dmy = new int[img_width*img_height];
		int x, y;
		for(x=0;x<img_width;x++)
			for(y=0;y<img_height;y++)
			{
				dmy[img_width*y+x] = 
					FloatToInt(1 *
					sqrt(pow(mriData.u[img_width*y+x],2)
					+pow(mriData.v[img_width*y+x],2)
					/*+pow(mriData.w[img_width*y+x],2)*/) *
					texture[img_width*y+x]);
			}
			
		NormaliseInt(dmy, img_height, img_width);
		return dmy;
	}
	else
	{
		int* dmy = new int[img_width*img_height*img_slices];
		int x, y, z;
		
		for(x=0; x<img_width; x++)
			for(y=0; y<img_height; y++)
				for(z=0; z<img_slices; z++)
				{
					dmy[z*img_width*img_height+y*img_width+x] = 
						FloatToInt(1 *
						sqrt(pow(mriData3D.u[z*img_width*img_height+y*img_width+x],2)
						+pow(mriData3D.v[z*img_width*img_height+y*img_width+x],2)
						+pow(mriData3D.w[z*img_width*img_height+y*img_width+x],2)) *
						texture3D[z*img_width*img_height+y*img_width+x]);
				}

		return dmy;
	}

}
