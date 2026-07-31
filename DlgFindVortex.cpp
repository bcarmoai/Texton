// DlgFindVortex.cpp : implementation file
//

#include "stdafx.h"
#include "texton.h"
#include "DlgFindVortex.h"
#include "DlgVolView.h"
#include <math.h>
#include "MainFrm.h"
#include "NumrecEigenvalues.h"
#include "TextonView.h"
#include "DlgVortexVtk.h"
#include "VtkUtilities.h"
#include <vtkFloatArray.h>
#include <vtkStructuredGridWriter.h>
#include "DlgTracking.h"
#include "DlgSaveVtkArrows.h"
#include "3d.h" // used in function LmsComputeApbDistance3D
#include <vtkPointData.h>
#include "DlgMoreVtx.h"
#include "DlgVolumeDicom.h"

#ifndef ANSI
#define ANSI
#endif
#include "nrutil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MOUSE_EIGEN 1
#define MOUSE_VORTICITY 2
#define MOUSE_LAMBDA 3

/////////////////////////////////////////////////////////////////////////////
// CDlgFindVortex dialog


CDlgFindVortex::CDlgFindVortex(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFindVortex::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgFindVortex)
	m_ImgSizesTxt = _T("");
	m_selMagnThres = 0.0f;
	m_eigenPosTxt = _T("At position [click on View image for values]");
	m_eigen1Txt = _T("");
	m_eigen2Txt = _T("");
	m_eigen3Txt = _T("");
	m_dudxTxt = _T("");
	m_dudyTxt = _T("");
	m_dudzTxt = _T("");
	m_dvdxTxt = _T("");
	m_dvdyTxt = _T("");
	m_dvdzTxt = _T("");
	m_dwdxTxt = _T("");
	m_dwdyTxt = _T("");
	m_dwdzTxt = _T("");
	m_vortexRange = 3600.0f;
	m_vortexSlices = 21;
	m_uFlippedTxt = _T("^");
	m_vFlippedTxt = _T("^");
	m_wFlippedTxt = _T("^");
	m_windingCircleRad = 4;
	m_windingTol = 0.001;
	m_vorticityX = _T("");
	m_vorticityY = _T("");
	m_vorticityZ = _T("");
	m_dudxLM = _T("");
	m_dudyLM = _T("");
	m_dudzLM = _T("");
	m_dvdxLM = _T("");
	m_dvdyLM = _T("");
	m_dvdzLM = _T("");
	m_dwdxLM = _T("");
	m_dwdyLM = _T("");
	m_dwdzLM = _T("");
	m_eig1LM = _T("");
	m_eig2LM = _T("");
	m_eig3LM = _T("");
	m_lambdaAtPosition = _T("At position [click on View image for values]");
	m_strmVorticity = FALSE;
	m_strmVorticityThres = 0.0f;
	m_strmMagn = FALSE;
	m_strmMagnThres = 0.0f;
	m_strmEigIm = FALSE;
	m_strmEigImThres = 0.0f;
	m_strmPos = FALSE;
	m_strmPosX = 0;
	m_strmPosY = 0;
	m_strmPosZ = 0;
	m_strmMagnitudeValid = FALSE;
	m_strmImagValid = FALSE;
	m_strmVorticityValid = FALSE;
	m_strmLambda = FALSE;
	m_strmLambdaValid = TRUE;
	m_strmWN = FALSE;
	m_bVorticityVectors = FALSE;
	m_lmsRegionSize = 2;
	m_lmsUse = TRUE;
	m_lmsVtkPositions = FALSE;
	m_lmsVtkDistances = FALSE;
	m_vortexPixels = 21;
	m_RegionCrPoints = FALSE;
	m_bSeedStrm = FALSE;
	//}}AFX_DATA_INIT

	m_velMagn = NULL;
	m_thresVelMagn = NULL;
	m_velGrad_dudx = m_velGrad_dudy = m_velGrad_dudz = m_velGrad_dvdx = m_velGrad_dvdy = 
		m_velGrad_dvdz = m_velGrad_dwdx = m_velGrad_dwdy = m_velGrad_dwdz = NULL;
	bTakeMouse = FALSE;
	pVelGradVolDlg = NULL;
	m_eigenValsReal = NULL;
	m_eigenValsImag = NULL;
	m_eigenImag = NULL;
	m_eigenReal = NULL;
	bNoLoadInitialise = FALSE;
	m_windingPlot = NULL;
	m_vorticityPlotX = NULL;
	m_vorticityPlotY = NULL;
	m_vorticityPlotZ = NULL;
	m_lambdaEigenValsReal = NULL;
	m_lambda11 = NULL;
	m_lambda12 = NULL;
	m_lambda13 = NULL;
	m_lambda21 = NULL;
	m_lambda22 = NULL;
	m_lambda23 = NULL;
	m_lambda31 = NULL;
	m_lambda32 = NULL;
	m_lambda33 = NULL;
	m_lambdaLambda = NULL;
	m_lambdaNegative = NULL;
	m_vorticityPlotX = NULL;
	m_vorticityPlotY = NULL;
	m_vorticityPlotZ = NULL;
	m_lambdaEigenValsReal = NULL;
	m_lambdaLambda = NULL;
	m_lambdaNegative = NULL;
	m_lmsDistances = NULL;
	m_lmsPositions = NULL;
	m_RegionCriticalPoints=NULL;
	pDlgStrm = NULL;

}

void CDlgFindVortex::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgFindVortex)
	DDX_Text(pDX, IDC_STATIC_IMAGEINFO, m_ImgSizesTxt);
	DDX_Text(pDX, IDC_FINDVORTEX_VELMAGN_THRES, m_selMagnThres);
	DDX_Text(pDX, IDC_VORTEX_ATPOSITION, m_eigenPosTxt);
	DDX_Text(pDX, IDC_VORTEX_EIG1, m_eigen1Txt);
	DDX_Text(pDX, IDC_VORTEX_EIG2, m_eigen2Txt);
	DDX_Text(pDX, IDC_VORTEX_EIG3, m_eigen3Txt);
	DDX_Text(pDX, IDC_VORTEX_DUDX, m_dudxTxt);
	DDX_Text(pDX, IDC_VORTEX_DUDX4, m_dudyTxt);
	DDX_Text(pDX, IDC_VORTEX_DUDX9, m_dudzTxt);
	DDX_Text(pDX, IDC_VORTEX_DUDX2, m_dvdxTxt);
	DDX_Text(pDX, IDC_VORTEX_DUDX6, m_dvdyTxt);
	DDX_Text(pDX, IDC_VORTEX_DUDX7, m_dvdzTxt);
	DDX_Text(pDX, IDC_VORTEX_DUDX3, m_dwdxTxt);
	DDX_Text(pDX, IDC_VORTEX_DUDX8, m_dwdyTxt);
	DDX_Text(pDX, IDC_VORTEX_DUDX5, m_dwdzTxt);
	DDX_Text(pDX, IDC_VORTEX_3DVRANGE, m_vortexRange);
	DDX_Text(pDX, IDC_VORTEX_3DNOISE_SLICES, m_vortexSlices);
	DDX_Text(pDX, IDC_VORTEX_FLIPPEDU, m_uFlippedTxt);
	DDX_Text(pDX, IDC_VORTEX_FLIPPEDv, m_vFlippedTxt);
	DDX_Text(pDX, IDC_VORTEX_FLIPPEDw, m_wFlippedTxt);
	DDX_Text(pDX, IDC_VORTEX_CIRCLERAD, m_windingCircleRad);
	DDX_Text(pDX, IDC_VORTEX_CINDEX_TOL, m_windingTol);
	DDX_Text(pDX, IDC_VORTEX_VORTICITY_X, m_vorticityX);
	DDX_Text(pDX, IDC_VORTEX_VORTICITY_Y, m_vorticityY);
	DDX_Text(pDX, IDC_VORTEX_VORTICITY_Z, m_vorticityZ);
	DDX_Text(pDX, IDC_VORTEX_LAMBDA_DUDX, m_dudxLM);
	DDX_Text(pDX, IDC_VORTEX_LAMBDA_DUDY, m_dudyLM);
	DDX_Text(pDX, IDC_VORTEX_LAMBDA_DUDZ, m_dudzLM);
	DDX_Text(pDX, IDC_VORTEX_LAMBDA_DVDX, m_dvdxLM);
	DDX_Text(pDX, IDC_VORTEX_LAMBDA_DVDY, m_dvdyLM);
	DDX_Text(pDX, IDC_VORTEX_LAMBDA_DVDZ, m_dvdzLM);
	DDX_Text(pDX, IDC_VORTEX_LAMBDA_DWDX, m_dwdxLM);
	DDX_Text(pDX, IDC_VORTEX_LAMBDA_DWDY, m_dwdyLM);
	DDX_Text(pDX, IDC_VORTEX_LAMBDA_DWDZ, m_dwdzLM);
	DDX_Text(pDX, IDC_VORTEX_LAMBDA_EIG1, m_eig1LM);
	DDX_Text(pDX, IDC_VORTEX_LAMBDA_EIG2, m_eig2LM);
	DDX_Text(pDX, IDC_VORTEX_LAMBDA_EIG3, m_eig3LM);
	DDX_Text(pDX, IDC_VORTEX_LAMBDA_ATPOSITION, m_lambdaAtPosition);
	DDX_Check(pDX, IDC_VORTEX_STRM_VORTICITY, m_strmVorticity);
	DDX_Text(pDX, IDC_VORTEX_VORTICITY_THRES, m_strmVorticityThres);
	DDX_Check(pDX, IDC_VORTEX_STRM_MAGN, m_strmMagn);
	DDX_Text(pDX, IDC_VORTEX_MAGN_THRES, m_strmMagnThres);
	DDX_Check(pDX, IDC_VORTEX_STRM_EIGIMAG, m_strmEigIm);
	DDX_Text(pDX, IDC_VORTEX_STRM_EIGIMAG_THRES, m_strmEigImThres);
	DDX_Check(pDX, IDC_VORTEX_STRM_POSITION, m_strmPos);
	DDX_Text(pDX, IDC_VORTEX_STRM_EIGIMAG_POS_X, m_strmPosX);
	DDX_Text(pDX, IDC_VORTEX_STRM_EIGIMAG_POS_Y, m_strmPosY);
	DDX_Text(pDX, IDC_VORTEX_STRM_EIGIMAG_POS_Z, m_strmPosZ);
	DDX_Check(pDX, IDC_VORTEX_STRM_MAGN_VALID, m_strmMagnitudeValid);
	DDX_Check(pDX, IDC_VORTEX_STRM_EIGIMAG_VALID, m_strmImagValid);
	DDX_Check(pDX, IDC_STRM_VORTICITY_VALID, m_strmVorticityValid);
	DDX_Check(pDX, IDC_VORTEX_STRM_LAMBDA, m_strmLambda);
	DDX_Check(pDX, IDC_VORTEX_STRM_LAMBDA_VALID, m_strmLambdaValid);
	DDX_Check(pDX, IDC_VORTEX_STRM_WN, m_strmWN);
	DDX_Check(pDX, IDC_STRM_VORTICITY_VECTOR, m_bVorticityVectors);
	DDX_Text(pDX, IDC_LMS_SIZE, m_lmsRegionSize);
	DDX_Check(pDX, IDC_LMS_USE, m_lmsUse);
	DDX_Check(pDX, IDC_LMS_VTK_POSITIONS, m_lmsVtkPositions);
	DDX_Check(pDX, IDC_LMS_VTK_DISTANCES, m_lmsVtkDistances);
	DDX_Text(pDX, IDC_VORTEX_3DNOISE_PIXELS, m_vortexPixels);
	DDX_Check(pDX, IDC_VORTEX_REGION_CRPOINTS, m_RegionCrPoints);
	DDX_Check(pDX, IDC_VTX_SEED_STRM, m_bSeedStrm);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgFindVortex, CDialog)
	//{{AFX_MSG_MAP(CDlgFindVortex)
	ON_BN_CLICKED(IDC_VORTEX_3DLOAD, OnVortex3dload)
	ON_BN_CLICKED(IDC_VORTEX_DATALOAD, OnVortexDataload)
	ON_BN_CLICKED(IDC_VORTEX_VELMAGN, OnVortexVelmagn)
	ON_BN_CLICKED(IDC_VORTEX_VELMAGN_VIEW, OnVortexVelmagnView)
	ON_BN_CLICKED(IDC_VORTEX_VELMAGN_THRES, OnVortexVelmagnThresGo)
	ON_BN_CLICKED(IDC_VORTEX_VELMAGN_VIEW_THRES, OnVortexVelmagnViewThres)
	ON_EN_CHANGE(IDC_FINDVORTEX_VELMAGN_THRES, OnChangeVelmagnEdit)
	ON_BN_CLICKED(IDC_VORTEX_GRADMAT_GO, OnVortexGradmatGo)
	ON_BN_CLICKED(IDC_VORTEX_VORTEX_GRADMAT_VIEW, OnVortexGradmatView)
	ON_BN_CLICKED(IDC_VORTEX_3DVORTEX, OnGenerateVortex)
	ON_BN_CLICKED(IDC_VORTEX_VIEW_REAL, OnVortexViewReal)
	ON_BN_CLICKED(IDC_VORTEX_VIEW_IMAG, OnVortexViewImag)
	ON_BN_CLICKED(IDC_VORTEX_FLIPU, OnVortexFlipU)
	ON_BN_CLICKED(IDC_VORTEX_FLIPV, OnVortexFlipV)
	ON_BN_CLICKED(IDC_VORTEX_FLIPW, OnVortexFlipW)
	ON_BN_CLICKED(IDC_VORTEX_WN_SLICE, OnVortexWnSelectSlice)
	ON_BN_CLICKED(IDC_VORTEX_WN_GO, OnVortexWindingGo)
	ON_BN_CLICKED(IDC_VORTEX_WN_VIEW, OnVortexWindingView)
	ON_BN_CLICKED(IDC_VORTEX_VIEW_TOLERANCE, OnVortexViewTolerance)
	ON_BN_CLICKED(IDC_VORTEX_VORTICITY_GO, OnVortexVorticityGo)
	ON_BN_CLICKED(IDC_VORTEX_VORTICITY_VIEW, OnVortexVorticityView)
	ON_BN_CLICKED(IDC_VORTEX_LAMBDA_GO, OnVortexLambdaGo)
	ON_BN_CLICKED(IDC_VORTEX_LAMBDA_VIEW, OnVortexLambdaView)
	ON_BN_CLICKED(IDC_VORTEX_LAMBDA_VIEW_NEGATIVE, OnVortexLambdaViewNegative)
	ON_BN_CLICKED(IDC_VORTEX_STRM_VIEW, OnVortexStrmView)
	ON_BN_CLICKED(IDC_VORTEX_VTK_VIEW, OnVortexVtkView)
	ON_BN_CLICKED(IDC_VORTEX_VTK_ARROWS, OnVortexVtkSelectedVectors)
	ON_BN_CLICKED(IDC_VORTEX_STRM_VIEW_MASK, OnVortexStrmViewMask)
	ON_BN_CLICKED(IDC_FINDVORTEX_TRACKING, OnDlgTracking)
	ON_BN_CLICKED(IDC_VORTEX_VTK_SAVE_ARROWS, OnVortexVtkSaveArrows)
	ON_BN_CLICKED(IDC_LMS_POS, OnViewLmsPositions)
	ON_BN_CLICKED(IDC_LMS_DISTS, OnViewLmsDistances)
	ON_BN_CLICKED(IDC_VORTEX_SAVEDATA, OnVortexSavedata)
	ON_BN_CLICKED(IDC_MORE_VTX, OnMoreVtx)
	ON_BN_CLICKED(IDC_VORTEX_SAVEDATA3D, OnVortexSaveData3D)
	ON_BN_CLICKED(IDC_FINDVORTEX_DICOM, OnFindvortexDicom)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgFindVortex message handlers

void CDlgFindVortex::OnVortex3dload() 
{

	((CButton*)GetDlgItem(IDC_VORTEX_VELMAGN))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_VORTEX_GRADMAT_GO))->EnableWindow(FALSE);

	// prompt user for path and load file
	CFileDialog* dDlg = new CFileDialog(FALSE,"*","mr001",OFN_CREATEPROMPT |
		OFN_HIDEREADONLY | OFN_NOREADONLYRETURN,
		_T("MRI CFD Data (*.*)|*.*||"), NULL);

	if(dDlg->DoModal()!=IDOK)
		return;

	mriData3D.LoadDataCFD(dDlg->GetPathName());
	delete dDlg;
	
	UpdateData(TRUE);
	m_ImgSizesTxt.Format("image: (x, y, s) %i x %i x %i", mriData3D.img_width, 
		mriData3D.img_height, mriData3D.img_slices);

	m_uFlippedTxt="^";
	m_vFlippedTxt="^";
	m_wFlippedTxt="^";
	ControlsDataReset();

}

void CDlgFindVortex::OnVortexDataload() 
{
	((CButton*)GetDlgItem(IDC_VORTEX_VELMAGN))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_VORTEX_GRADMAT_GO))->EnableWindow(FALSE);

	// prompt user for path and load file
	CFileDialog* dDlg = new CFileDialog(TRUE,"sur","mr001",OFN_CREATEPROMPT |
		OFN_HIDEREADONLY | OFN_NOREADONLYRETURN,
		_T("MRI Data (*.sur)|*.sur||"), NULL);

	if(dDlg->DoModal()!=IDOK)
		return;

	mriData3D.LoadDataFrame(dDlg->GetPathName());
	delete dDlg;

	UpdateData(TRUE);
	m_ImgSizesTxt.Format("image: (x, y, s) %i x %i x %i", mriData3D.img_width, 
		mriData3D.img_height, mriData3D.img_slices);

	m_uFlippedTxt="^";
	m_vFlippedTxt="^";
	m_wFlippedTxt="^";
	ControlsDataReset();
	
}

void CDlgFindVortex::OnGenerateVortex() 
{
	((CButton*)GetDlgItem(IDC_VORTEX_VELMAGN))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_VORTEX_GRADMAT_GO))->EnableWindow(FALSE);

	UpdateData(TRUE);

	int VTX_SZ = m_vortexPixels;

	// allocate memory
	mriData3D.Reset();
	mriData3D.mr = new short[VTX_SZ*VTX_SZ*m_vortexSlices];
	mriData3D.u = new short[VTX_SZ*VTX_SZ*m_vortexSlices];
	mriData3D.v = new short[VTX_SZ*VTX_SZ*m_vortexSlices];
	mriData3D.w = new short[VTX_SZ*VTX_SZ*m_vortexSlices];
	mriData3D.img_width = mriData3D.img_height = VTX_SZ;
	mriData3D.img_slices = m_vortexSlices;
	float lower, half;
	lower = (float)m_vortexRange;
	half = lower/2.0f;

	int x, y, z;
	for(x=0; x<mriData3D.img_width; x++)
		for(y=0; y<mriData3D.img_height; y++)
			for(z=0; z<mriData3D.img_slices; z++)
			{
				mriData3D.u[z*VTX_SZ*VTX_SZ+y*VTX_SZ+x]=(short)floor((-lower/((float)VTX_SZ))* y + half
				/*	* ((2.0f/mriData3D.img_slices)*z) */);
				mriData3D.v[z*VTX_SZ*VTX_SZ+y*VTX_SZ+x]=(short)floor((lower/((float)VTX_SZ))* x - half);
				mriData3D.w[z*VTX_SZ*VTX_SZ+y*VTX_SZ+x]=(short)floor((lower/((float)VTX_SZ))* x - half);
				mriData3D.mr[z*VTX_SZ*VTX_SZ+y*VTX_SZ+x]=0;
			}

	m_ImgSizesTxt.Format("image: (x, y, s) %i x %i x %i", mriData3D.img_width, 
		mriData3D.img_height, mriData3D.img_slices);

	m_uFlippedTxt="^";
	m_vFlippedTxt="^";
	m_wFlippedTxt="^";
	ControlsDataReset();

}

void CDlgFindVortex::NoLoadInitialise(CData3DMRI *slice3D)
{
	((CButton*)GetDlgItem(IDC_VORTEX_VELMAGN))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_VORTEX_GRADMAT_GO))->EnableWindow(FALSE);

	UpdateData(TRUE);
	bNoLoadInitialise = TRUE;

	mriData3D.SetData(slice3D->img_width, slice3D->img_height, slice3D->img_slices, slice3D->mr,
		slice3D->u, slice3D->v, slice3D->w);

	m_ImgSizesTxt.Format("image: (x, y, s) %i x %i x %i", mriData3D.img_width, 
		mriData3D.img_height, mriData3D.img_slices);

	m_uFlippedTxt="^";
	m_vFlippedTxt="^";
	m_wFlippedTxt="^";
	ControlsDataReset();

}

void CDlgFindVortex::OnVortexVelmagn() 
{
	((CButton*)GetDlgItem(IDC_VORTEX_VELMAGN_VIEW))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_VORTEX_VELMAGN_THRES))->EnableWindow(FALSE);
	((CWnd*)GetDlgItem(IDC_FINDVORTEX_VELMAGN_THRES))->EnableWindow(FALSE);

	if(m_velMagn != NULL)
		delete m_velMagn;

	m_velMagn = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];

	int x, y, z;
	for(x=0; x<mriData3D.img_width;x++)
		for(y=0; y<mriData3D.img_height; y++)
			for(z=0; z<mriData3D.img_slices; z++)
				m_velMagn[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x] =
				(float)sqrt(pow(mriData3D.u[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x],2.0)
				+ pow(mriData3D.v[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x],2.0)
				+ pow(mriData3D.w[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x],2.0) );

	((CButton*)GetDlgItem(IDC_VORTEX_VELMAGN_VIEW))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_VORTEX_VELMAGN_THRES))->EnableWindow(TRUE);
	((CWnd*)GetDlgItem(IDC_FINDVORTEX_VELMAGN_THRES))->EnableWindow(TRUE);
		
}

void CDlgFindVortex::OnVortexVelmagnView() 
{
	pVolDlg = new CDlgVolView;
	pVolDlg->pView = pView;
	pVolDlg->m_sVolumeTitle.Format("velocity magnitude");
	pVolDlg->InitialiseVolume(m_velMagn, mriData3D.img_width, mriData3D.img_height, mriData3D.img_slices);
	pVolDlg->m_imgInfo = m_ImgSizesTxt;
	pVolDlg->Create(IDD_VOL_VIEW, this);
	pVolDlg->ShowWindow(SW_SHOW);
}

int CDlgFindVortex::FloatToInt(float fnum)
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

void CDlgFindVortex::OnCancel() 
{
	if(m_velMagn != NULL)
		delete [] m_velMagn;

	if(m_velGrad_dudx != NULL)
		delete [] m_velGrad_dudx;
	if(m_velGrad_dudy != NULL) 
		delete [] m_velGrad_dudy;
	if(m_velGrad_dudz != NULL)
		delete [] m_velGrad_dudz;
	if(m_velGrad_dvdx != NULL) 
		delete [] m_velGrad_dvdx;
	if(m_velGrad_dvdy != NULL) 
		delete [] m_velGrad_dvdy;
	if(m_velGrad_dvdz != NULL)
		delete [] m_velGrad_dvdz;
	if(m_velGrad_dwdx != NULL) 
		delete [] m_velGrad_dwdx;
	if(m_velGrad_dwdy != NULL) 
		delete [] m_velGrad_dwdy;
	if(m_velGrad_dwdz != NULL)
		delete [] m_velGrad_dwdz;

	if(m_eigenValsReal != NULL)
		delete [] m_eigenValsReal;
	if(m_eigenValsImag != NULL)
		delete [] m_eigenValsImag;
	if(m_eigenImag != NULL)
		delete [] m_eigenImag;
	if(m_eigenReal != NULL)
		delete [] m_eigenReal;

	if(m_windingPlot != NULL)
		delete [] m_windingPlot;

	if(m_vorticityPlotX!=NULL)
		delete [] m_vorticityPlotX;
	if(m_vorticityPlotY!=NULL)
		delete [] m_vorticityPlotY;
	if(m_vorticityPlotZ!=NULL)
		delete [] m_vorticityPlotZ;

	if(m_lambdaEigenValsReal!=NULL)
		delete [] m_lambdaEigenValsReal;
	if(m_lambdaLambda != NULL)
		delete [] m_lambdaLambda;
	if(m_lambdaNegative != NULL)
		delete [] m_lambdaLambda;

	if(m_lmsDistances != NULL)
		delete [] m_lmsDistances;
	if(m_lmsPositions != NULL)
		delete [] m_lmsPositions;

	if(m_RegionCriticalPoints != NULL)
		delete [] m_RegionCriticalPoints;

	if(!bNoLoadInitialise)
		((CMainFrame*)AfxGetMainWnd())->pDlgTasks->ShowWindow(SW_SHOW);
	
	CDialog::OnCancel();
}

void CDlgFindVortex::OnVortexVelmagnThresGo() 
{
	((CButton*)GetDlgItem(IDC_VORTEX_VELMAGN_VIEW_THRES))->EnableWindow(FALSE);
	UpdateData(TRUE);

	if(m_thresVelMagn != NULL)
		delete m_thresVelMagn;

	m_thresVelMagn = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];

	float temp_magn;

	int x, y, z;
	for(x=0; x<mriData3D.img_width;x++)
		for(y=0; y<mriData3D.img_height; y++)
			for(z=0; z<mriData3D.img_slices; z++)
			{
				temp_magn = (float)sqrt(pow(mriData3D.u[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x],2.0)
				+ pow(mriData3D.v[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x],2.0)
				+ pow(mriData3D.w[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x],2.0) );

				m_thresVelMagn[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x] =
					temp_magn <= m_selMagnThres ? 255.0f : 0.0f;
				
			}
	((CButton*)GetDlgItem(IDC_VORTEX_VELMAGN_VIEW_THRES))->EnableWindow(TRUE);
	
}

void CDlgFindVortex::OnVortexVelmagnViewThres() 
{
	pVolDlg = new CDlgVolView;
	pVolDlg->pView = pView;
	pVolDlg->m_sVolumeTitle.Format("white is vel. magn. lower than %.3f", m_selMagnThres);
	pVolDlg->InitialiseVolume(m_thresVelMagn, mriData3D.img_width, mriData3D.img_height, mriData3D.img_slices);
	pVolDlg->m_imgInfo = m_ImgSizesTxt;
	pVolDlg->Create(IDD_VOL_VIEW, this);
	pVolDlg->ShowWindow(SW_SHOW);

}

void CDlgFindVortex::OnChangeVelmagnEdit() 
{
	((CButton*)GetDlgItem(IDC_VORTEX_VELMAGN_VIEW_THRES))->EnableWindow(FALSE);
	
}

BOOL CDlgFindVortex::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	return TRUE;
	              
}

void CDlgFindVortex::OnVortexGradmatGo() 
{
	((CButton*)GetDlgItem(IDC_VORTEX_VORTEX_GRADMAT_VIEW))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_VORTEX_VIEW_REAL))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_VORTEX_VIEW_IMAG))->EnableWindow(FALSE);

	if(m_eigenValsReal != NULL)
		delete [] m_eigenValsReal;
	if(m_eigenValsImag != NULL)
		delete [] m_eigenValsImag;
	if(m_eigenImag != NULL)
		delete [] m_eigenImag;
	if(m_eigenReal != NULL)
		delete [] m_eigenReal;

	int x, y, z;
	if(mriData3D.img_slices>1)
	{
		m_eigenValsReal = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices][3];
		m_eigenValsImag = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices][3];
		m_eigenImag = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
		m_eigenReal = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];

		for(x=0; x<mriData3D.img_width;x++)
			for(y=0; y<mriData3D.img_height; y++)
				for(z=0; z<mriData3D.img_slices; z++)
					m_eigenImag[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]=
					m_eigenReal[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]=
					0.0f;

		ComputeVelocityGradients();

		if(!m_lmsUse)
			for(x=1; x<mriData3D.img_width-1;x++)
				for(y=1; y<mriData3D.img_height-1; y++)
					for(z=1; z<mriData3D.img_slices-1; z++)
					{
						
						// get eigenvalues
						ComputeEigenvalues3D(m_velGrad_dudx[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x],
							m_velGrad_dudy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x],
							m_velGrad_dudz[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x],
							m_velGrad_dvdx[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x],
							m_velGrad_dvdy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x],
							m_velGrad_dvdz[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x],
							m_velGrad_dwdx[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x],
							m_velGrad_dwdy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x],
							m_velGrad_dwdz[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x],
							x, y, z);
						
					}
	}
	else
	{
		z=0;
		m_eigenValsReal = new float[mriData3D.img_width*mriData3D.img_height][3];
		m_eigenValsImag = new float[mriData3D.img_width*mriData3D.img_height][3];
		m_eigenImag = new float[mriData3D.img_width*mriData3D.img_height];
		m_eigenReal = new float[mriData3D.img_width*mriData3D.img_height];


		for(x=0; x<mriData3D.img_width;x++)
			for(y=0; y<mriData3D.img_height; y++)
				m_eigenImag[y*mriData3D.img_width+x]=
				m_eigenReal[y*mriData3D.img_width+x]=
				0.0f;

		ComputeVelocityGradients();

		if(!m_lmsUse)
		{
			const int RAD = 1;
			for(x=RAD; x<mriData3D.img_width-RAD;x++)
				for(y=RAD; y<mriData3D.img_height-RAD; y++)
					{	
						// get eigenvalues
						ComputeEigenvalues2D(m_velGrad_dudx[y*mriData3D.img_width+x],
							m_velGrad_dudy[y*mriData3D.img_width+x],
							m_velGrad_dvdx[y*mriData3D.img_width+x],
							m_velGrad_dvdy[y*mriData3D.img_width+x],
							x, y);
						
					}
		}
	}

	((CButton*)GetDlgItem(IDC_VORTEX_VORTEX_GRADMAT_VIEW))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_VORTEX_VIEW_REAL))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_VORTEX_VIEW_IMAG))->EnableWindow(TRUE);
	bTakeMouse = FALSE;


}

void CDlgFindVortex::ComputeEigenvalues3D(float aa, float ab, float ac, float ba, float bb, float bc, 
										float ca, float cb, float cc, int x, int y, int s)
{
	float** nrMatrixA;
	nrMatrixA = matrix(1, 3, 1, 3);
	nrMatrixA[1][1]=aa;
	nrMatrixA[1][2]=ab;
	nrMatrixA[1][3]=ac;
	nrMatrixA[2][1]=ba;
	nrMatrixA[2][2]=bb;
	nrMatrixA[2][3]=bc;
	nrMatrixA[3][1]=ca;
	nrMatrixA[3][2]=cb;
	nrMatrixA[3][3]=cc;

	float* nrEigenReal = vector(1, 3);
	float* nrEigenImag = vector(1, 3);

	CNumrecEigenvalues nrEigen;
	nrEigen.ComputeEigenvalues(nrMatrixA, nrEigenReal, nrEigenImag, 3);

	m_eigenImag[s*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
		= m_eigenReal[s*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
		= 0.0f;

	int i;
	for(i=0; i<3 ; i++)
	{
		m_eigenValsReal[s*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x][i] 
			= nrEigenReal[i+1];
		m_eigenValsImag[s*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x][i] 
			= nrEigenImag[i+1];
		m_eigenImag[s*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
			+= fabs(nrEigenImag[i+1]);
		m_eigenReal[s*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
			+= fabs(nrEigenReal[i+1]);
	}
	
	free_matrix(nrMatrixA,1,3,1,3);
	free_vector(nrEigenReal,1,3);
	free_vector(nrEigenImag,1,3);

}

void CDlgFindVortex::ComputeEigenvalues2D(float aa, float ab, float ba, float bb, int x, int y)
{
	float** nrMatrixA;
	nrMatrixA = matrix(1, 2, 1, 2);
	nrMatrixA[1][1]=aa;
	nrMatrixA[1][2]=ab;
	nrMatrixA[2][1]=ba;
	nrMatrixA[2][2]=bb;

	float* nrEigenReal = vector(1, 2);
	float* nrEigenImag = vector(1, 2);

	CNumrecEigenvalues nrEigen;
	nrEigen.ComputeEigenvalues(nrMatrixA, nrEigenReal, nrEigenImag, 2);

	m_eigenImag[y*mriData3D.img_width+x] = m_eigenReal[y*mriData3D.img_width+x] = 0.0f;

	int i;
	for(i=0; i<2 ; i++)
	{
		m_eigenValsReal[y*mriData3D.img_width+x][i]	= nrEigenReal[i+1];
		m_eigenValsImag[y*mriData3D.img_width+x][i]	= nrEigenImag[i+1];
		m_eigenImag[y*mriData3D.img_width+x] += fabs(nrEigenImag[i+1]);
		m_eigenReal[y*mriData3D.img_width+x] += fabs(nrEigenReal[i+1]);
	}

	free_matrix(nrMatrixA,1,2,1,2);
	free_vector(nrEigenReal,1,2);
	free_vector(nrEigenImag,1,2);

}

void CDlgFindVortex::OnVortexGradmatView() 
{
	OnVortexVelmagn();

	pVelGradVolDlg = new CDlgVolView;
	pVelGradVolDlg->pView = pView;
	pVelGradVolDlg->m_sVolumeTitle.Format("velocity magnitude", m_selMagnThres);
	pVelGradVolDlg->InitialiseVolume(m_velMagn, mriData3D.img_width, mriData3D.img_height, mriData3D.img_slices);
	pVelGradVolDlg->m_imgInfo = m_ImgSizesTxt;
	pVelGradVolDlg->Create(IDD_VOL_VIEW, this);
	pVelGradVolDlg->ShowWindow(SW_SHOW);


	bTakeMouse = TRUE;	
	iMouseState = MOUSE_EIGEN;
}

void CDlgFindVortex::TakeMovePoint(CPoint point)
{
	UpdateData(TRUE);

	int x, y, s;

	switch(iMouseState)
	{
	case MOUSE_EIGEN:
		{
			pVelGradVolDlg->GetMouseProjection(x, y, s, point);

			if((s-1)<0 || (s-1)>=mriData3D.img_slices || x<0 || x >= mriData3D.img_width 
				|| y<0 || y>= mriData3D.img_height)
				return;

			m_eigenPosTxt.Format("At position (x, y, s) : (%i, %i, %i) :", x, y, s);
			s--;

			if(mriData3D.img_slices>1)
			{
				m_eigen1Txt.Format("%.3f + i %.3f", 
					m_eigenValsReal[s*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x][0],
					m_eigenValsImag[s*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x][0] );
				m_eigen2Txt.Format("%.3f + i %.3f", 
					m_eigenValsReal[s*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x][1],
					m_eigenValsImag[s*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x][1] );
				m_eigen3Txt.Format("%.3f + i %.3f", 
					m_eigenValsReal[s*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x][2],
					m_eigenValsImag[s*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x][2] );
				m_dudxTxt.Format("%.3f", 
					m_velGrad_dudx[s*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]);
				m_dudyTxt.Format("%.3f", 
					m_velGrad_dudy[s*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]);
				m_dudzTxt.Format("%.3f", 
					m_velGrad_dudz[s*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]);
				m_dvdxTxt.Format("%.3f", 
					m_velGrad_dvdx[s*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]);
				m_dvdyTxt.Format("%.3f", 
					m_velGrad_dvdy[s*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]);
				m_dvdzTxt.Format("%.3f", 
					m_velGrad_dvdz[s*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]);
				m_dwdxTxt.Format("%.3f", 
					m_velGrad_dwdx[s*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]);
				m_dwdyTxt.Format("%.3f", 
					m_velGrad_dwdy[s*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]);
				m_dwdzTxt.Format("%.3f", 
					m_velGrad_dwdz[s*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]);

			}
			else
			{
				m_eigen1Txt.Format("%.3f + i %.3f", 
					m_eigenValsReal[y*mriData3D.img_width+x][0],
					m_eigenValsImag[y*mriData3D.img_width+x][0] );
				m_eigen2Txt.Format("%.3f + i %.3f", 
					m_eigenValsReal[y*mriData3D.img_width+x][1],
					m_eigenValsImag[y*mriData3D.img_width+x][1] );
				m_eigen3Txt.Format("");
				m_dudxTxt.Format("%.3f", m_velGrad_dudx[y*mriData3D.img_width+x]);
				m_dudyTxt.Format("%.3f", m_velGrad_dudy[y*mriData3D.img_width+x]);
				m_dudzTxt.Format("");
				m_dvdxTxt.Format("%.3f", m_velGrad_dvdx[y*mriData3D.img_width+x]);
				m_dvdyTxt.Format("%.3f", m_velGrad_dvdy[y*mriData3D.img_width+x]);
				m_dvdzTxt.Format("");
				m_dwdxTxt.Format("");
				m_dwdyTxt.Format("");
				m_dwdzTxt.Format("");
			}
		}
				break;
	case MOUSE_VORTICITY:
		{
			pVorticityDlg->GetMouseProjection(x, y, s, point);

			if((s-1)<0 || (s-1)>=mriData3D.img_slices || x<0 || x >= mriData3D.img_width 
				|| y<0 || y>= mriData3D.img_height)
				return;

			s--;

			if(mriData3D.img_slices>1)
			{
				m_vorticityX.Format("%f", m_vorticityPlotX[s*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]);
				m_vorticityY.Format("%f", m_vorticityPlotY[s*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]);
				m_vorticityZ.Format("%f", m_vorticityPlotZ[s*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]);
			}
			else
			{
				m_vorticityX.Format("");
				m_vorticityY.Format("");
				m_vorticityZ.Format("%f", m_vorticityPlotZ[s*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]);
			}
		}
		break;
	case MOUSE_LAMBDA:
		{
			pLambdaDlg->GetMouseProjection(x, y, s, point);

			if((s-1)<0 || (s-1)>=mriData3D.img_slices || x<0 || x >= mriData3D.img_width 
				|| y<0 || y>= mriData3D.img_height)
				return;

			m_lambdaAtPosition.Format("At position (x, y, s) : (%i, %i, %i) :", x, y, s);
			s--;

			if(mriData3D.img_slices>1)
			{
				m_eig1LM.Format("%.3f", 
					m_lambdaEigenValsReal[s*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x][0]);
				m_eig2LM.Format("%.3f", 
					m_lambdaEigenValsReal[s*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x][1]);
				m_eig3LM.Format("%.3f", 
					m_lambdaEigenValsReal[s*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x][2]);
				m_dudxLM.Format("%.3f", 
					m_velGrad_dudx[s*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]);
				m_dudyLM.Format("%.3f", 
					m_velGrad_dudy[s*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]);
				m_dudzLM.Format("%.3f", 
					m_velGrad_dudz[s*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]);
				m_dvdxLM.Format("%.3f", 
					m_velGrad_dvdx[s*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]);
				m_dvdyLM.Format("%.3f", 
					m_velGrad_dvdy[s*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]);
				m_dvdzLM.Format("%.3f", 
					m_velGrad_dvdz[s*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]);
				m_dwdxLM.Format("%.3f", 
					m_velGrad_dwdx[s*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]);
				m_dwdyLM.Format("%.3f", 
					m_velGrad_dwdy[s*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]);
				m_dwdzLM.Format("%.3f", 
					m_velGrad_dwdz[s*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]);

			}
			else
			{
				m_eig1LM.Format("%.3f", 
					m_lambdaEigenValsReal[s*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x][0]);
				m_eig2LM.Format("%.3f", 
					m_lambdaEigenValsReal[s*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x][1]);
				m_eig3LM.Format("");
				m_dudxLM.Format("%.3f", m_velGrad_dudx[y*mriData3D.img_width+x]);
				m_dudyLM.Format("%.3f", m_velGrad_dudy[y*mriData3D.img_width+x]);
				m_dudzLM.Format("");
				m_dvdxLM.Format("%.3f", m_velGrad_dvdx[y*mriData3D.img_width+x]);
				m_dvdyLM.Format("%.3f", m_velGrad_dvdy[y*mriData3D.img_width+x]);
				m_dvdzLM.Format("");
				m_dwdxLM.Format("");
				m_dwdyLM.Format("");
				m_dwdzLM.Format("");
			}
		}
		break;
	default:
		AfxMessageBox("Internal error: bad mouse state.");
		break;
	}

	UpdateData(FALSE);

}


void CDlgFindVortex::OnVortexViewReal() 
{
	pVelGradVolDlg = new CDlgVolView;
	pVelGradVolDlg->pView = pView;
	pVelGradVolDlg->m_sVolumeTitle.Format("real values", m_selMagnThres);
	pVelGradVolDlg->InitialiseVolume(m_eigenReal, mriData3D.img_width, mriData3D.img_height, mriData3D.img_slices);
	pVelGradVolDlg->m_imgInfo = m_ImgSizesTxt;
	pVelGradVolDlg->Create(IDD_VOL_VIEW, this);
	pVelGradVolDlg->ShowWindow(SW_SHOW);

	bTakeMouse = TRUE;	
	iMouseState = MOUSE_EIGEN;
}

void CDlgFindVortex::OnVortexViewImag() 
{
	pVelGradVolDlg = new CDlgVolView;
	pVelGradVolDlg->pView = pView;
	pVelGradVolDlg->m_sVolumeTitle.Format("imaginary values", m_selMagnThres);
	pVelGradVolDlg->InitialiseVolume(m_eigenImag, mriData3D.img_width, mriData3D.img_height, mriData3D.img_slices);
	pVelGradVolDlg->m_imgInfo = m_ImgSizesTxt;
	pVelGradVolDlg->Create(IDD_VOL_VIEW, this);
	pVelGradVolDlg->ShowWindow(SW_SHOW);

	bTakeMouse = TRUE;	
	iMouseState = MOUSE_EIGEN;
}


void CDlgFindVortex::OnVortexFlipU() 
{
	mriData3D.FlipU();
	UpdateData(TRUE);
	if(m_uFlippedTxt=="^")
		m_uFlippedTxt="v";
	else
		m_uFlippedTxt="^";

	ControlsDataReset();

}

void CDlgFindVortex::OnVortexFlipV() 
{
	mriData3D.FlipV();
	UpdateData(TRUE);
	if(m_uFlippedTxt=="^")
		m_vFlippedTxt="v";
	else
		m_vFlippedTxt="^";

	ControlsDataReset();

}

void CDlgFindVortex::OnVortexFlipW() 
{
	mriData3D.FlipW();	
	UpdateData(TRUE);
	if(m_uFlippedTxt=="^")
		m_wFlippedTxt="v";
	else
		m_wFlippedTxt="^";

	ControlsDataReset();
}

void CDlgFindVortex::OnVortexWnSelectSlice() 
{
	((CButton*)GetDlgItem(IDC_VORTEX_WN_GO))->EnableWindow(TRUE);
	((CEdit*)GetDlgItem(IDC_VORTEX_CIRCLERAD))->EnableWindow(TRUE);

	if(mriData3D.img_slices == 1)
	{
		return;
	}

	if(m_velMagn==NULL)
		OnVortexVelmagn();

	pWindingSelVolDlg = new CDlgVolView;
	pWindingSelVolDlg->pView = pView;
	pWindingSelVolDlg->m_sVolumeTitle.Format("velocity magnitude", m_selMagnThres);
	pWindingSelVolDlg->InitialiseVolume(m_velMagn, mriData3D.img_width, mriData3D.img_height, mriData3D.img_slices);
	pWindingSelVolDlg->m_imgInfo = m_ImgSizesTxt;
	pWindingSelVolDlg->Create(IDD_VOL_VIEW, this);
	pWindingSelVolDlg->ShowWindow(SW_SHOW);

	bTakeMouse = FALSE;
}

void CDlgFindVortex::OnVortexWindingGo() 
{
	/*
	CDataMRI* slice;

	if(mriData3D.img_slices != 1)
	{
		
		// get selected slice info
		char cPlane;
		int iPlane;
		pWindingSelVolDlg->GetCurrentPlane(cPlane, iPlane);

		// allocate slice data
		switch(cPlane)
		{
		case 'x':
			slice = mriData3D.Get2dXSlice(iPlane);
			break;
		case 'y':
			slice = mriData3D.Get2dYSlice(iPlane);
			break;
		case 's':
			slice = mriData3D.Get2dSlice(iPlane);
			break;
		default:
			AfxMessageBox("Internal error: bad current plane.");
			return;
		}
	}
	else
		slice = mriData3D.Get2dSlice(0);
	*/

	WNComputeWindingNumber();

	((CButton*)GetDlgItem(IDC_VORTEX_WN_VIEW))->EnableWindow(TRUE);
	((CEdit*)GetDlgItem(IDC_VORTEX_CINDEX_TOL))->EnableWindow(TRUE);
	((CWnd*)GetDlgItem(IDC_VORTEX_VIEW_TOLERANCE))->EnableWindow(TRUE);

}

void CDlgFindVortex::OnVortexWindingView() 
{
	CDlgVolView* pWindingDlg = new CDlgVolView;
	pWindingDlg->pView = pView;
	pWindingDlg->m_sVolumeTitle.Format("winding angle with radius %i", m_windingCircleRad);
	pWindingDlg->InitialiseVolume(m_windingPlot, mriData3D.img_width, mriData3D.img_height, 
		mriData3D.img_slices);
	pWindingDlg->m_imgInfo = m_ImgSizesTxt;
	pWindingDlg->Create(IDD_VOL_VIEW, this);
	pWindingDlg->ShowWindow(SW_SHOW);
}

void CDlgFindVortex::WNComputeWindingNumber(/*CDataMRI* slice*/) 
{
	const double PI2 = 6.2831853070;
	const int img_width = mriData3D.img_width;
	const int img_height = mriData3D.img_height;
	const int img_slices = mriData3D.img_slices;
	int slice_no;

	CDataMRI* slice;

	UpdateData(TRUE);

	if(m_windingPlot != NULL)
		delete m_windingPlot;

	m_windingPlot = new float[img_width*img_height*img_slices];
	m_windingPlotWidth = img_width;
	m_windingPlotHeight = img_height;

	double* angles;
	
	int x, y, z;
	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
			for(z=0; z<img_slices; z++)
				m_windingPlot[z*img_width*img_height+y*img_width+x] = 0.0f;


	// ** note we're slicing on the X plane **
	for(slice_no = 0; slice_no < img_width; slice_no++)
	{
		slice = mriData3D.Get2dXSlice(slice_no);
		angles = new double[slice->img_width*slice->img_height];

		// calculate angles
		for(x=0; x<slice->img_width; x++)
			for(y=0; y<slice->img_height; y++)
				if(slice->u[slice->img_width*y+x]==0 && slice->v[slice->img_width*y+x]==0)
					angles[slice->img_width*y+x]=0;
				else
					angles[slice->img_width*y+x]
						= atan2(slice->v[slice->img_width*y+x], slice->u[slice->img_width*y+x]);

		// get circle template points
		int iPoints;
		CArray<CPoint, CPoint> circle_points;

		// calculate winding number
		double cIndex;
		double angle_diff;

		WNGetCirclePoints(m_windingCircleRad, circle_points);
		int pc = circle_points.GetSize();

		int i, idx1, idx2;
		for(x=m_windingCircleRad; x<slice->img_width-m_windingCircleRad; x++)
			for(y=m_windingCircleRad; y<slice->img_height-m_windingCircleRad; y++)
			{
				cIndex = 0.0;

				iPoints = circle_points.GetSize();
				if(iPoints<2)
				{
					m_windingPlot[m_windingPlotWidth*y+x] = 0.0f;
					continue;
				}

				idx1 = slice->img_width*(circle_points[0].y+y)+(circle_points[0].x+x);
				idx2 = slice->img_width*(circle_points[iPoints-1].y+y)+(circle_points[iPoints-1].x+x);
				if(idx1 >= slice->img_width*slice->img_height || idx2 >= slice->img_width*slice->img_height)
					AfxMessageBox("Warning: angles array referenced out of bounds in WNComputeWindingNumber.");
				angle_diff = angles[idx1] - angles[idx2];
				cIndex = cIndex + WNWrapAngle(angle_diff);
				
				for(i=0; i<iPoints-1; i++)
				{
					idx1 = slice->img_width*(circle_points[i+1].y+y)+(circle_points[i+1].x+x);
					idx2 = slice->img_width*(circle_points[i].y+y)+(circle_points[i].x+x);
					if(idx1 >= slice->img_width*slice->img_height || idx2 >= slice->img_width*slice->img_height)
						AfxMessageBox("Warning: angles array referenced out of bounds in WNComputeWindingNumber.");
					angle_diff = angles[idx1] - angles[idx2];
					cIndex = cIndex + WNWrapAngle(angle_diff);
				}

				cIndex = cIndex / PI2;

				m_windingPlot[x*img_width*img_height+y*img_width+slice_no] = (float)cIndex;

			}

		delete [] slice;
		delete [] angles;


	}

}

void CDlgFindVortex::WNGetCirclePoints(int radius, CArray<CPoint, CPoint> &points)
{
	points.RemoveAll();

	if(radius<=0)
		return;

	// first point to the right of centre
	CPoint toAdd(0, radius);
	points.Add(toAdd);
	
	int quadrant = 1;
	double distances[3];

	int x, y, bestpnt;

	x = radius;
	y = 0;

	// first quadrant
	while(x != 0)
	{
		distances[0] = fabs(WNGetPointRadius(x, y-1) - (double)radius);
		distances[1] = fabs(WNGetPointRadius(x-1, y-1) - (double)radius);
		distances[2] = fabs(WNGetPointRadius(x-1, y) - (double)radius);

		bestpnt = WNGetMinIdx(distances, 3);

		switch(bestpnt)
		{
		case 0:
			y--;
			break;
		case 1:
			x--;
			y--;
			break;
		case 2:
			x--;
			break;
		default:
			AfxMessageBox("Internal error: bad WNGetMinIdx() at 1st quadrant.");
			return;
		}

		points.Add(CPoint(x, y));

	}

	// second quadrant
	while(y != 0)
	{
		distances[0] = fabs(WNGetPointRadius(x-1, y) - (double)radius);
		distances[1] = fabs(WNGetPointRadius(x-1, y+1) - (double)radius);
		distances[2] = fabs(WNGetPointRadius(x, y+1) - (double)radius);

		bestpnt = WNGetMinIdx(distances, 3);

		switch(bestpnt)
		{
		case 0:
			x--;
			break;
		case 1:
			x--;
			y++;
			break;
		case 2:
			y++;
			break;
		default:
			AfxMessageBox("Internal error: bad WNGetMinIdx() at 2nd quadrant.");
			return;
		}

		points.Add(CPoint(x, y));

	}

	// third quadrant
	while(x != 0)
	{
		distances[0] = fabs(WNGetPointRadius(x, y+1) - (double)radius);
		distances[1] = fabs(WNGetPointRadius(x+1, y+1) - (double)radius);
		distances[2] = fabs(WNGetPointRadius(x+1, y) - (double)radius);

		bestpnt = WNGetMinIdx(distances, 3);

		switch(bestpnt)
		{
		case 0:
			y++;
			break;
		case 1:
			x++;
			y++;
			break;
		case 2:
			x++;
			break;
		default:
			AfxMessageBox("Internal error: bad WNGetMinIdx() at 3rd quadrant.");
			return;
		}

		points.Add(CPoint(x, y));

	}

	// fourth quadrant
	while(y != -1)
	{
		distances[0] = fabs(WNGetPointRadius(x+1, y) - (double)radius);
		distances[1] = fabs(WNGetPointRadius(x+1, y-1) - (double)radius);
		distances[2] = fabs(WNGetPointRadius(x, y-1) - (double)radius);

		bestpnt = WNGetMinIdx(distances, 3);

		switch(bestpnt)
		{
		case 0:
			x++;
			break;
		case 1:
			x++;
			y--;
			break;
		case 2:
			y--;
			break;
		default:
			AfxMessageBox("Internal error: bad WNGetMinIdx() at 4th quadrant.");
			return;
		}

		points.Add(CPoint(x, y));

	}

}

int CDlgFindVortex::WNGetMinIdx(double *dist, int count)
{
	if(count<1)
		return -1;

	int i, imin;
	double minval;
	minval = dist[0];
	imin = 0;

	for(i=1; i<count; i++)
		if(dist[i]<minval)
		{
			minval = dist[i];
			imin = i;
		}

	return imin;
}

double CDlgFindVortex::WNWrapAngle(double angle)
{
	const double PI = 3.1415926535;
	const double PI2 = 6.2831853070;

	if (angle > PI2 || angle < -PI2)
		angle = 0;
	else if (fabs(fabs(angle) - PI) < 0.01)
		angle = PI;
	else if (angle > PI)
		angle = angle - PI2;
	else if (angle < -PI)
		angle = angle + PI2;

  	return angle;

}


double CDlgFindVortex::WNGetPointRadius(int x, int y)
{
	return sqrt( pow(x,2.0) + pow(y,2.0) );
}

void CDlgFindVortex::ControlsDataReset()
{
	((CButton*)GetDlgItem(IDC_VORTEX_VELMAGN))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_VORTEX_VELMAGN_VIEW))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_VORTEX_VELMAGN_THRES))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_VORTEX_VELMAGN_VIEW_THRES))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_VORTEX_GRADMAT_GO))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_VORTEX_VORTEX_GRADMAT_VIEW))->EnableWindow(FALSE);
	((CWnd*)GetDlgItem(IDC_FINDVORTEX_VELMAGN_THRES))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_VORTEX_WN_SLICE))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_VORTEX_WN_GO))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_VORTEX_WN_VIEW))->EnableWindow(FALSE);
	((CWnd*)GetDlgItem(IDC_VORTEX_FLIPU))->EnableWindow(TRUE);
	((CWnd*)GetDlgItem(IDC_VORTEX_FLIPPEDU))->EnableWindow(TRUE);
	((CWnd*)GetDlgItem(IDC_VORTEX_FLIPV))->EnableWindow(TRUE);
	((CWnd*)GetDlgItem(IDC_VORTEX_FLIPPEDv))->EnableWindow(TRUE);
	((CWnd*)GetDlgItem(IDC_VORTEX_FLIPW))->EnableWindow(TRUE);
	((CWnd*)GetDlgItem(IDC_VORTEX_FLIPPEDw))->EnableWindow(TRUE);
	((CEdit*)GetDlgItem(IDC_VORTEX_CIRCLERAD))->EnableWindow(FALSE);
	((CEdit*)GetDlgItem(IDC_VORTEX_CINDEX_TOL))->EnableWindow(FALSE);
	((CWnd*)GetDlgItem(IDC_VORTEX_VIEW_TOLERANCE))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_VORTEX_VORTICITY_GO))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_VORTEX_VORTICITY_VIEW))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_VORTEX_LAMBDA_GO))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_VORTEX_LAMBDA_VIEW))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_VORTEX_LAMBDA_VIEW_NEGATIVE))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_VORTEX_STRM_VORTICITY))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_VORTEX_VORTICITY_THRES))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_VORTEX_STRM_VIEW))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_VORTEX_STRM_MAGN))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_VORTEX_MAGN_THRES))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_VORTEX_STRM_EIGIMAG))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_VORTEX_STRM_EIGIMAG_THRES))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_VORTEX_VTK_VIEW))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_VORTEX_STRM_POSITION))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_VORTEX_STRM_EIGIMAG_POS_X))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_VORTEX_STRM_EIGIMAG_POS_Y))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_VORTEX_STRM_EIGIMAG_POS_Z))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_STRM_VORTICITY_VALID))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_VORTEX_STRM_MAGN_VALID))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_VORTEX_STRM_EIGIMAG_VALID))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_VORTEX_STRM_LAMBDA))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_VORTEX_STRM_LAMBDA_VALID))->EnableWindow(TRUE);
	((CWnd*)GetDlgItem(IDC_VORTEX_STRM_WN))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_VORTEX_STRM_VIEW_MASK))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_VORTEX_WN_GO))->EnableWindow(TRUE);
	((CEdit*)GetDlgItem(IDC_VORTEX_CIRCLERAD))->EnableWindow(TRUE);
	bTakeMouse = FALSE;
	m_eigenPosTxt = _T("At position [click on View image for values]");
	m_eigen1Txt = _T("");
	m_eigen2Txt = _T("");
	m_eigen3Txt = _T("");
	m_dudxTxt = _T("");
	m_dudyTxt = _T("");
	m_dudzTxt = _T("");
	m_dvdxTxt = _T("");
	m_dvdyTxt = _T("");
	m_dvdzTxt = _T("");
	m_dwdxTxt = _T("");
	m_dwdyTxt = _T("");
	m_dwdzTxt = _T("");
	m_vorticityX.Format("");
	m_vorticityY.Format("");
	m_vorticityZ.Format("");
	UpdateData(FALSE);

}

void CDlgFindVortex::OnVortexViewTolerance() 
{
	UpdateData(TRUE);

	int* dmy = new int[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];

	int x, y, z;

	for(x=0; x<mriData3D.img_width; x++)
		for(y=0; y<mriData3D.img_height; y++)
			for(z=0; z<mriData3D.img_slices; z++)
				if((m_windingPlot[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x] > 1.0-m_windingTol 
				 && m_windingPlot[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x] < 1.0+m_windingTol) 
				 || (m_windingPlot[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x] > -1.0-m_windingTol 
				 && m_windingPlot[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x] < -1.0+m_windingTol))
				 dmy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x] = 1;
				else
				 dmy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x] = 0;
/*
	for(x=0; x<m_windingPlotWidth; x++)
		for(y=0; y<m_windingPlotHeight; y++)
			if((m_windingPlot[m_windingPlotWidth*y+x] > 1.0-m_windingTol 
			 && m_windingPlot[m_windingPlotWidth*y+x] < 1.0+m_windingTol) 
			 || (m_windingPlot[m_windingPlotWidth*y+x] > -1.0-m_windingTol 
			 && m_windingPlot[m_windingPlotWidth*y+x] < -1.0+m_windingTol))
				dmy[m_windingPlotWidth*y+x] = 1;
			else
				dmy[m_windingPlotWidth*y+x] = 0;
*/
	CDlgVolView* pWindingDlg = new CDlgVolView;
	pWindingDlg->pView = pView;
	pWindingDlg->m_sVolumeTitle.Format("winding angle, radius %i, tol %f", m_windingCircleRad, m_windingTol);
	pWindingDlg->InitialiseVolume(dmy, mriData3D.img_width, mriData3D.img_height, mriData3D.img_slices);
	pWindingDlg->m_imgInfo = m_ImgSizesTxt;
	pWindingDlg->Create(IDD_VOL_VIEW, this);
	pWindingDlg->ShowWindow(SW_SHOW);

	delete dmy;
	
}

void CDlgFindVortex::OnVortexVorticityGo() 
{

	if(mriData3D.img_width <=0 || mriData3D.img_height <= 0 || mriData3D.img_slices <= 0)
		return;

	((CButton*)GetDlgItem(IDC_VORTEX_VORTICITY_VIEW))->EnableWindow(FALSE);

	OnVortexGradmatGo();

	if(m_vorticityPlotX!=NULL)
		delete [] m_vorticityPlotX;
	if(m_vorticityPlotY!=NULL)
		delete [] m_vorticityPlotY;
	if(m_vorticityPlotZ!=NULL)
		delete [] m_vorticityPlotZ;

	int x, y, z;
	if(mriData3D.img_slices>1)
	{
		m_vorticityPlotX = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
		m_vorticityPlotY = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
		m_vorticityPlotZ = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];

		for(x=0; x<mriData3D.img_width;x++)
			for(y=0; y<mriData3D.img_height; y++)
				for(z=0; z<mriData3D.img_slices; z++)
				{
					m_vorticityPlotX[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
						= m_velGrad_dwdy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
						- m_velGrad_dvdz[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x];

					m_vorticityPlotY[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
						= m_velGrad_dudz[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
						- m_velGrad_dwdx[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x];

					m_vorticityPlotZ[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
						= m_velGrad_dvdx[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
						- m_velGrad_dudy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x];

				}
	}
	else
	{
		m_vorticityPlotX = new float[mriData3D.img_width*mriData3D.img_height];
		m_vorticityPlotY = new float[mriData3D.img_width*mriData3D.img_height];
		m_vorticityPlotZ = new float[mriData3D.img_width*mriData3D.img_height];

		for(x=0; x<mriData3D.img_width;x++)
			for(y=0; y<mriData3D.img_height; y++)
				{
					m_vorticityPlotX[y*mriData3D.img_width+x]=
					m_vorticityPlotY[y*mriData3D.img_width+x]=0.0f;

					m_vorticityPlotZ[y*mriData3D.img_width+x]
						= m_velGrad_dvdx[y*mriData3D.img_width+x]
						- m_velGrad_dudy[y*mriData3D.img_width+x];

				}

	}

	((CButton*)GetDlgItem(IDC_VORTEX_VORTICITY_VIEW))->EnableWindow(TRUE);
}

void CDlgFindVortex::OnVortexVorticityView() 
{
	float* dmy = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
	int x, y, z;
		for(x=0; x<mriData3D.img_width;x++)
			for(y=0; y<mriData3D.img_height; y++)
				for(z=0; z<mriData3D.img_slices; z++)
					dmy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
					= sqrt(
						(mriData3D.img_slices > 1?
						(pow(m_vorticityPlotX[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x], 2.0)
						+pow(m_vorticityPlotY[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x], 2.0))
						: 0.0f)
						+pow(m_vorticityPlotZ[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x], 2.0)
						  );

	pVorticityDlg = new CDlgVolView;
	pVorticityDlg->pView = pView;
	pVorticityDlg->m_sVolumeTitle.Format("vorticity magnitude, radius %i, tol %f", m_windingCircleRad, m_windingTol);
	pVorticityDlg->InitialiseVolume(dmy, mriData3D.img_width, mriData3D.img_height, mriData3D.img_slices);
	pVorticityDlg->m_imgInfo = m_ImgSizesTxt;
	pVorticityDlg->Create(IDD_VOL_VIEW, this);
	pVorticityDlg->ShowWindow(SW_SHOW);

	delete dmy;

	bTakeMouse = TRUE;
	iMouseState = MOUSE_VORTICITY;

}

void CDlgFindVortex::OnVortexLambdaGo() 
{
	((CButton*)GetDlgItem(IDC_VORTEX_LAMBDA_VIEW))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_VORTEX_LAMBDA_VIEW_NEGATIVE))->EnableWindow(FALSE);

	// velocity gradients
	OnVortexGradmatGo();

	// S² + µ²
	if(m_lambda11 != NULL)
		delete [] m_lambda11;
	if(m_lambda12 != NULL)
		delete [] m_lambda12;
	if(m_lambda13 != NULL)
		delete [] m_lambda13;
	if(m_lambda21 != NULL)
		delete [] m_lambda21;
	if(m_lambda22 != NULL)
		delete [] m_lambda22;
	if(m_lambda23 != NULL)
		delete [] m_lambda23;
	if(m_lambda31 != NULL)
		delete [] m_lambda31;
	if(m_lambda32 != NULL)
		delete [] m_lambda32;
	if(m_lambda33 != NULL)
		delete [] m_lambda33;
	if(m_lambdaEigenValsReal != NULL)
		delete [] m_lambdaEigenValsReal;
	if(m_lambdaLambda != NULL)
		delete [] m_lambdaLambda;

	int x, y, z;
	float cur_s11, cur_s12, cur_s13, cur_s21, cur_s22, cur_s23, cur_s31, cur_s32, cur_s33, 
		cur_o11, cur_o12, cur_o13, cur_o21, cur_o22, cur_o23, cur_o31, cur_o32, cur_o33;

	if(mriData3D.img_slices>1)
	{
		m_lambda11 = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
		m_lambda12 = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
		m_lambda13 = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
		m_lambda21 = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
		m_lambda22 = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
		m_lambda23 = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
		m_lambda31 = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
		m_lambda32 = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
		m_lambda33 = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
		m_lambdaLambda = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];

		m_lambdaEigenValsReal = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices][3];

		for(x=0; x<mriData3D.img_width;x++)
			for(y=0; y<mriData3D.img_height; y++)
				for(z=0; z<mriData3D.img_slices; z++)
					m_lambda11[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]=
					m_lambda12[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]=
					m_lambda13[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]=
					m_lambda21[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]=
					m_lambda22[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]=
					m_lambda23[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]=
					m_lambda31[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]=
					m_lambda32[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]=
					m_lambda33[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]=
					0.0f;

		for(x=0; x<mriData3D.img_width;x++)
			for(y=0; y<mriData3D.img_height; y++)
				for(z=0; z<mriData3D.img_slices; z++)
				{
					// compute S
					cur_s11 = m_velGrad_dudx[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x];
					cur_s12 = (m_velGrad_dudy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
						+ m_velGrad_dvdx[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x])
						/ 2.0f;
					cur_s13 = (m_velGrad_dudz[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
						+ m_velGrad_dwdx[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x])
						/ 2.0f;
					cur_s21 = cur_s12;
					cur_s22 = m_velGrad_dvdy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x];
					cur_s23 = (m_velGrad_dvdz[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
						+ m_velGrad_dwdy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x])
						/ 2.0f;
					cur_s31 = cur_s13;
					cur_s32 = cur_s23;
					cur_s33 = m_velGrad_dwdz[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x];
	
					// compute µ
					cur_o11 = 0.0f;
					cur_o12 = (m_velGrad_dudy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
						- m_velGrad_dvdx[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x])
						/ 2.0f;
					cur_o13 = (m_velGrad_dudz[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
						- m_velGrad_dwdx[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x])
						/ 2.0f;
					cur_o21 = (m_velGrad_dvdx[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
						- m_velGrad_dudy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x])
						/ 2.0f;
					cur_o22 = 0.0f;
					cur_o23 = (m_velGrad_dvdz[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
						- m_velGrad_dwdy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x])
						/ 2.0f;
					cur_o31 = (m_velGrad_dwdx[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
						- m_velGrad_dudz[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x])
						/ 2.0f;
					cur_o32 = (m_velGrad_dwdy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
						- m_velGrad_dvdz[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x])
						/ 2.0f;
					cur_o33 = 0.0f;

					// compute S² + µ²
					m_lambda11[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
						= cur_s11*cur_s11+cur_s12*cur_s21+cur_s13*cur_s31
						+ cur_o11*cur_o11+cur_o12*cur_o21+cur_o13*cur_o31;
					m_lambda12[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
						= cur_s11*cur_s12+cur_s12*cur_s22+cur_s13*cur_s32
						+ cur_o11*cur_o12+cur_o12*cur_o22+cur_o13*cur_o32;
					m_lambda13[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
						= cur_s11*cur_s13+cur_s12*cur_s23+cur_s13*cur_s33
						+ cur_o11*cur_o13+cur_o12*cur_o23+cur_o13*cur_o33;
					m_lambda21[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
						= cur_s21*cur_s11+cur_s22*cur_s21+cur_s23*cur_s31
						+ cur_o21*cur_o11+cur_o22*cur_o21+cur_o23*cur_o31;
					m_lambda22[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
						= cur_s21*cur_s12+cur_s22*cur_s22+cur_s23*cur_s32
						+ cur_o21*cur_o12+cur_o22*cur_o22+cur_o23*cur_o32;
					m_lambda23[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
						= cur_s21*cur_s13+cur_s22*cur_s23+cur_s23*cur_s33
						+ cur_o21*cur_o13+cur_o22*cur_o23+cur_o23*cur_o33;
					m_lambda31[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
						= cur_s31*cur_s11+cur_s32*cur_s21+cur_s33*cur_s31
						+ cur_o31*cur_o11+cur_o32*cur_o21+cur_o33*cur_o31;
					m_lambda32[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
						= cur_s31*cur_s12+cur_s32*cur_s22+cur_s33*cur_s32
						+ cur_o31*cur_o12+cur_o32*cur_o22+cur_o33*cur_o32;
					m_lambda33[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
						= cur_s31*cur_s13+cur_s32*cur_s23+cur_s33*cur_s33
						+ cur_o31*cur_o13+cur_o32*cur_o23+cur_o33*cur_o33;

					// compute eigenvalues
					LMComputeEigenvalues3D(
						m_lambda11[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x], 
						m_lambda12[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x], 
						m_lambda13[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x],
						m_lambda21[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x], 
						m_lambda22[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x], 
						m_lambda23[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x], 
						m_lambda31[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x], 
						m_lambda32[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x], 
						m_lambda33[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x], 
						x, y, z);

					// plot second largest eigenvalue (lambda)
					if( m_lambdaEigenValsReal[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x][0]
					  > m_lambdaEigenValsReal[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x][1]
					 && m_lambdaEigenValsReal[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x][0]
					  < m_lambdaEigenValsReal[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x][2]
					 || m_lambdaEigenValsReal[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x][0]
					  > m_lambdaEigenValsReal[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x][2]
					 && m_lambdaEigenValsReal[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x][0]
					  < m_lambdaEigenValsReal[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x][1])
							m_lambdaLambda[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
							= m_lambdaEigenValsReal[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x][0];
					else
						if( m_lambdaEigenValsReal[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x][1]
						  > m_lambdaEigenValsReal[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x][0]
						 && m_lambdaEigenValsReal[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x][1]
						  < m_lambdaEigenValsReal[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x][2]
						 || m_lambdaEigenValsReal[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x][1]
						  > m_lambdaEigenValsReal[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x][2]
						 && m_lambdaEigenValsReal[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x][1]
						  < m_lambdaEigenValsReal[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x][0])
								m_lambdaLambda[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
								= m_lambdaEigenValsReal[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x][1];
						else
							m_lambdaLambda[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
							= m_lambdaEigenValsReal[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x][2];

				}
	}
	else
	{
		m_lambda11 = new float[mriData3D.img_width*mriData3D.img_height];
		m_lambda12 = new float[mriData3D.img_width*mriData3D.img_height];
		m_lambda21 = new float[mriData3D.img_width*mriData3D.img_height];
		m_lambda22 = new float[mriData3D.img_width*mriData3D.img_height];
		m_lambdaLambda = new float[mriData3D.img_width*mriData3D.img_height];

		m_lambdaEigenValsReal = new float[mriData3D.img_width*mriData3D.img_height][3];

		for(x=0; x<mriData3D.img_width;x++)
			for(y=0; y<mriData3D.img_height; y++)
					m_lambda11[y*mriData3D.img_width+x]=
					m_lambda12[y*mriData3D.img_width+x]=
					m_lambda21[y*mriData3D.img_width+x]=
					m_lambda22[y*mriData3D.img_width+x]=
					0.0f;

		for(x=0; x<mriData3D.img_width;x++)
			for(y=0; y<mriData3D.img_height; y++)
				for(z=0; z<mriData3D.img_slices; z++)
				{
					// compute S
					cur_s11 = m_velGrad_dudx[y*mriData3D.img_width+x];
					cur_s12 = (m_velGrad_dudy[y*mriData3D.img_width+x]
						+ m_velGrad_dvdx[y*mriData3D.img_width+x])
						/ 2.0f;
					cur_s21 = cur_s12;
					cur_s22 = m_velGrad_dvdy[y*mriData3D.img_width+x];
	
					// compute µ
					cur_o11 = 0.0f;
					cur_o12 = (m_velGrad_dudy[y*mriData3D.img_width+x]
						- m_velGrad_dvdx[y*mriData3D.img_width+x])
						/ 2.0f;
					cur_o21 = (m_velGrad_dvdx[y*mriData3D.img_width+x]
						- m_velGrad_dudy[y*mriData3D.img_width+x])
						/ 2.0f;
					cur_o22 = 0.0f;

					// compute S² + µ²
					m_lambda11[y*mriData3D.img_width+x]
						= cur_s11*cur_s11+cur_s12*cur_s21
						+ cur_o11*cur_o11+cur_o12*cur_o21;
					m_lambda12[y*mriData3D.img_width+x]
						= cur_s11*cur_s12+cur_s12*cur_s22
						+ cur_o11*cur_o12+cur_o12*cur_o22;
					m_lambda21[y*mriData3D.img_width+x]
						= cur_s21*cur_s11+cur_s22*cur_s21
						+ cur_o21*cur_o11+cur_o22*cur_o21;
					m_lambda22[y*mriData3D.img_width+x]
						= cur_s21*cur_s12+cur_s22*cur_s22
						+ cur_o21*cur_o12+cur_o22*cur_o22;

					// compute eigenvalues
					LMComputeEigenvalues2D(
						m_lambda11[y*mriData3D.img_width+x], 
						m_lambda12[y*mriData3D.img_width+x], 
						m_lambda21[y*mriData3D.img_width+x], 
						m_lambda22[y*mriData3D.img_width+x], 
						x, y);

					// plot second largest eigenvalue (lambda)
					if( m_lambdaEigenValsReal[y*mriData3D.img_width+x][0]
					  < m_lambdaEigenValsReal[y*mriData3D.img_width+x][1] )
							m_lambdaLambda[y*mriData3D.img_width+x]
							= m_lambdaEigenValsReal[y*mriData3D.img_width+x][0];
					else
						m_lambdaLambda[y*mriData3D.img_width+x]
						= m_lambdaEigenValsReal[y*mriData3D.img_width+x][1];

				}

	}

	((CButton*)GetDlgItem(IDC_VORTEX_LAMBDA_VIEW))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_VORTEX_LAMBDA_VIEW_NEGATIVE))->EnableWindow(TRUE);
}

void CDlgFindVortex::LMComputeEigenvalues3D(float aa, float ab, float ac, float ba, float bb, float bc, 
										float ca, float cb, float cc, int x, int y, int s)
{
	float** nrMatrixA;
	nrMatrixA = matrix(1, 3, 1, 3);
	nrMatrixA[1][1]=aa;
	nrMatrixA[1][2]=ab;
	nrMatrixA[1][3]=ac;
	nrMatrixA[2][1]=ba;
	nrMatrixA[2][2]=bb;
	nrMatrixA[2][3]=bc;
	nrMatrixA[3][1]=ca;
	nrMatrixA[3][2]=cb;
	nrMatrixA[3][3]=cc;

	float* nrEigenReal = vector(1, 3);
	float* nrEigenImag = vector(1, 3);

	CNumrecEigenvalues nrEigen;
	nrEigen.ComputeEigenvalues(nrMatrixA, nrEigenReal, nrEigenImag, 3);

	int i;
	for(i=0; i<3 ; i++)
	{
		m_lambdaEigenValsReal[s*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x][i] 
			= nrEigenReal[i+1];
		if(nrEigenImag[i+1] >= 0.01f)
			AfxMessageBox("Warning: non-null imaginary part of eigenvalue.");
	}
	
	free_matrix(nrMatrixA,1,3,1,3);
	free_vector(nrEigenReal,1,3);
	free_vector(nrEigenImag,1,3);

}

void CDlgFindVortex::LMComputeEigenvalues2D(float aa, float ab, float ba, float bb, int x, int y)
{
	float** nrMatrixA;
	nrMatrixA = matrix(1, 2, 1, 2);
	nrMatrixA[1][1]=aa;
	nrMatrixA[1][2]=ab;
	nrMatrixA[2][1]=ba;
	nrMatrixA[2][2]=bb;

	float* nrEigenReal = vector(1, 2);
	float* nrEigenImag = vector(1, 2);

	CNumrecEigenvalues nrEigen;
	nrEigen.ComputeEigenvalues(nrMatrixA, nrEigenReal, nrEigenImag, 2);

	int i;
	for(i=0; i<2 ; i++)
	{
		m_lambdaEigenValsReal[y*mriData3D.img_width+x][i] = nrEigenReal[i+1];
		if(nrEigenImag[i+1] >= 0.01f)
			AfxMessageBox("Warning: non-null imaginary part of eigenvalue.");
	}
	
	free_matrix(nrMatrixA,1,2,1,2);
	free_vector(nrEigenReal,1,2);
	free_vector(nrEigenImag,1,2);

}

void CDlgFindVortex::OnVortexLambdaView() 
{
	pLambdaDlg = new CDlgVolView;
	pLambdaDlg->pView = pView;
	pLambdaDlg->m_sVolumeTitle.Format("lambda2", m_windingCircleRad, m_windingTol);
	pLambdaDlg->InitialiseVolume(m_lambdaLambda, mriData3D.img_width, mriData3D.img_height, mriData3D.img_slices);
	pLambdaDlg->m_imgInfo = m_ImgSizesTxt;
	pLambdaDlg->Create(IDD_VOL_VIEW, this);
	pLambdaDlg->ShowWindow(SW_SHOW);

	bTakeMouse = TRUE;
	iMouseState = MOUSE_LAMBDA;
}

void CDlgFindVortex::OnVortexLambdaViewNegative(BOOL showdialog = TRUE) 
{
	if(m_lambdaNegative!=NULL)
		delete [] m_lambdaNegative;

	m_lambdaNegative = new int[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
	int x, y, z;
	for(x=0; x<mriData3D.img_width;x++)
		for(y=0; y<mriData3D.img_height; y++)
			for(z=0; z<mriData3D.img_slices; z++)
				if(mriData3D.img_slices==1)
					m_lambdaNegative[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
					= m_lambdaLambda[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]<0.0f
					&& (m_lambdaEigenValsReal[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x][0]
					* m_lambdaEigenValsReal[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x][1] < 0.0f)?
					255:0;
				else
					m_lambdaNegative[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
					= m_lambdaLambda[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]<0.0f?
					255:0;

	if(showdialog)
	{
		pLambdaDlg = new CDlgVolView;
		pLambdaDlg->pView = pView;
		pLambdaDlg->m_sVolumeTitle.Format("white = negative lambda2", m_windingCircleRad, m_windingTol);
		pLambdaDlg->InitialiseVolume(m_lambdaNegative, mriData3D.img_width, mriData3D.img_height, mriData3D.img_slices);
		pLambdaDlg->m_imgInfo = m_ImgSizesTxt;
		pLambdaDlg->Create(IDD_VOL_VIEW, this);
		pLambdaDlg->ShowWindow(SW_SHOW);
		
		bTakeMouse = TRUE;
		iMouseState = MOUSE_LAMBDA;
	}

}

float* CDlgFindVortex::SeedMask() 
{
	UpdateData(TRUE);

	float* dmy = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
	int x, y, z;
	for(x=0; x<mriData3D.img_width;x++)
		for(y=0; y<mriData3D.img_height; y++)
			for(z=0; z<mriData3D.img_slices; z++)
				dmy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
				= 1.000f;

	if(m_strmVorticity)
		if(!m_bVorticityVectors)
		{
			OnVortexVorticityGo();

			// compute vorticity modulus
			float* dmy2 = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
			int x, y, z;
				for(x=0; x<mriData3D.img_width;x++)
					for(y=0; y<mriData3D.img_height; y++)
						for(z=0; z<mriData3D.img_slices; z++)
							dmy2[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
							= sqrt(
								(mriData3D.img_slices > 1?
								(pow(m_vorticityPlotX[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x], 2.0)
								+pow(m_vorticityPlotY[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x], 2.0))
								: 0.0f)
								+pow(m_vorticityPlotZ[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x], 2.0)
								  );

			if(m_strmVorticityValid)
				for(x=0; x<mriData3D.img_width;x++)
					for(y=0; y<mriData3D.img_height; y++)
						for(z=0; z<mriData3D.img_slices; z++)
							dmy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
							= dmy2[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
							> m_strmVorticityThres? 1:0;
			else
				for(x=0; x<mriData3D.img_width;x++)
					for(y=0; y<mriData3D.img_height; y++)
						for(z=0; z<mriData3D.img_slices; z++)
							dmy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
							= dmy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x] *
							dmy2[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x];

			delete [] dmy2;
		}
		else
		{
			OnVortexVorticityGo();
			CDlgSaveVtkArrows pDlgArrows;
			pDlgArrows.mps_imgSize.Format("image: (x, y, s) %i x %i x %i", mriData3D.img_width, 
				mriData3D.img_height, mriData3D.img_slices);
			pDlgArrows.DoModal();
			VtkSaveVorticityArrows(pDlgArrows.m_bWholeData, pDlgArrows.m_bHCFD, 
				pDlgArrows.m_bS, pDlgArrows.m_coordZbS-1, 
				pDlgArrows.m_bX, pDlgArrows.m_coordX, 
				pDlgArrows.m_bY, pDlgArrows.m_coordY);

		}

	if(m_strmMagn)
	{
		OnVortexVelmagn();

		if(m_strmMagnitudeValid)
			for(x=0; x<mriData3D.img_width;x++)
				for(y=0; y<mriData3D.img_height; y++)
					for(z=0; z<mriData3D.img_slices; z++)
						dmy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
						= dmy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x] *
						(m_velMagn[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
						< m_strmMagnThres? 1:0);
		else/*
			for(x=1; x<mriData3D.img_width-1;x++)
				for(y=1; y<mriData3D.img_height-1; y++)
					for(z=1; z<mriData3D.img_slices-1; z++)
						dmy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
						= dmy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x] *
						((	m_velMagn[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]+
							m_velMagn[(z-1)*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]+
							m_velMagn[(z+1)*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]+
							m_velMagn[z*mriData3D.img_width*mriData3D.img_height+(y-1)*mriData3D.img_width+x]+
							m_velMagn[(z-1)*mriData3D.img_width*mriData3D.img_height+(y-1)*mriData3D.img_width+x]+
							m_velMagn[(z+1)*mriData3D.img_width*mriData3D.img_height+(y-1)*mriData3D.img_width+x]+
							m_velMagn[z*mriData3D.img_width*mriData3D.img_height+(y+1)*mriData3D.img_width+x]+
							m_velMagn[(z-1)*mriData3D.img_width*mriData3D.img_height+(y+1)*mriData3D.img_width+x]+
							m_velMagn[(z+1)*mriData3D.img_width*mriData3D.img_height+(y+1)*mriData3D.img_width+x]+
							
							m_velMagn[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+(x-1)]+
							m_velMagn[(z-1)*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+(x-1)]+
							m_velMagn[(z+1)*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+(x-1)]+
							m_velMagn[z*mriData3D.img_width*mriData3D.img_height+(y-1)*mriData3D.img_width+(x-1)]+
							m_velMagn[(z-1)*mriData3D.img_width*mriData3D.img_height+(y-1)*mriData3D.img_width+(x-1)]+
							m_velMagn[(z+1)*mriData3D.img_width*mriData3D.img_height+(y-1)*mriData3D.img_width+(x-1)]+
							m_velMagn[z*mriData3D.img_width*mriData3D.img_height+(y+1)*mriData3D.img_width+(x-1)]+
							m_velMagn[(z-1)*mriData3D.img_width*mriData3D.img_height+(y+1)*mriData3D.img_width+(x-1)]+
							m_velMagn[(z+1)*mriData3D.img_width*mriData3D.img_height+(y+1)*mriData3D.img_width+(x-1)]+
							
							m_velMagn[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+(x+1)]+
							m_velMagn[(z-1)*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+(x+1)]+
							m_velMagn[(z+1)*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+(x+1)]+
							m_velMagn[z*mriData3D.img_width*mriData3D.img_height+(y-1)*mriData3D.img_width+(x+1)]+
							m_velMagn[(z-1)*mriData3D.img_width*mriData3D.img_height+(y-1)*mriData3D.img_width+(x+1)]+
							m_velMagn[(z+1)*mriData3D.img_width*mriData3D.img_height+(y-1)*mriData3D.img_width+(x+1)]+
							m_velMagn[z*mriData3D.img_width*mriData3D.img_height+(y+1)*mriData3D.img_width+(x+1)]+
							m_velMagn[(z-1)*mriData3D.img_width*mriData3D.img_height+(y+1)*mriData3D.img_width+(x+1)]+
							m_velMagn[(z+1)*mriData3D.img_width*mriData3D.img_height+(y+1)*mriData3D.img_width+(x+1)]
							
							)/27.0);

*/
			for(x=0; x<mriData3D.img_width;x++)
				for(y=0; y<mriData3D.img_height; y++)
					for(z=0; z<mriData3D.img_slices; z++)
						dmy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
						= dmy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x] *
						(m_velMagn[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]);


	
	}

	if(m_strmEigIm)
	{
		OnVortexGradmatGo();

		if(m_strmImagValid)
			for(x=0; x<mriData3D.img_width;x++)
				for(y=0; y<mriData3D.img_height; y++)
					for(z=0; z<mriData3D.img_slices; z++)
						dmy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
						= dmy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x] *
						(m_eigenImag[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
						> m_strmEigImThres? 1:0);
		else
			for(x=0; x<mriData3D.img_width;x++)
				for(y=0; y<mriData3D.img_height; y++)
					for(z=0; z<mriData3D.img_slices; z++)
						dmy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
						= dmy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x] *
						m_eigenImag[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x];

	}

	if(m_strmLambda)
	{
		OnVortexLambdaGo();
		OnVortexLambdaViewNegative(FALSE);

		if(m_strmLambdaValid)
			for(x=0; x<mriData3D.img_width;x++)
				for(y=0; y<mriData3D.img_height; y++)
					for(z=0; z<mriData3D.img_slices; z++)
						dmy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
						= dmy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x] *
						(m_lambdaNegative[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]>0?
						1:0);
		else
			for(x=0; x<mriData3D.img_width;x++)
				for(y=0; y<mriData3D.img_height; y++)
					for(z=0; z<mriData3D.img_slices; z++)
						dmy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
						= dmy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x] *
						m_lambdaLambda[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x];

	}

	if(m_strmPos)
	{
		for(x=0; x<mriData3D.img_width;x++)
			for(y=0; y<mriData3D.img_height; y++)
				for(z=0; z<mriData3D.img_slices; z++)
					dmy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
					= dmy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x] *
					((x==m_strmPosX && y==m_strmPosY && z==m_strmPosZ)? 1:0);

	}

	if(m_strmWN)
	{
		OnVortexWindingGo();

		for(x=0; x<mriData3D.img_width; x++)
			for(y=0; y<mriData3D.img_height; y++)
				for(z=0; z<mriData3D.img_slices; z++)
					//dmy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x] = 
					//m_windingPlot[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x];
					if((m_windingPlot[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x] > 1.0-m_windingTol 
					 && m_windingPlot[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x] < 1.0+m_windingTol) 
					 || (m_windingPlot[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x] > -1.0-m_windingTol 
					 && m_windingPlot[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x] < -1.0+m_windingTol))
					 dmy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x] = 1;
					else
					 dmy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x] = 0;
					 
				
	}

	if(m_RegionCrPoints)
	{
		for(x=0; x<mriData3D.img_width;x++)
			for(y=0; y<mriData3D.img_height; y++)
				for(z=0; z<mriData3D.img_slices; z++)
				{
					if(m_RegionCriticalPoints[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x] > 0)
						MessageBeep(0);

					dmy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
					= dmy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x] *
					m_RegionCriticalPoints[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x];	
				}

	}

	if(m_lmsVtkPositions)
	{
		if(TRUE)
		{
			for(x=0; x<mriData3D.img_width; x++)
				for(y=0; y<mriData3D.img_height; y++)
					for(z=0; z<mriData3D.img_slices; z++)
						dmy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
						= dmy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
						  *m_lmsPositions[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x];
		}
		else
		{

			float* dmyPos = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
			float* dmyDiff = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];

			/* set arrays to zero */
			for(x=0; x<mriData3D.img_width; x++)
				for(y=0; y<mriData3D.img_height; y++)
					for(z=0; z<mriData3D.img_slices; z++)
						dmy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
						= dmyPos[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
						= dmyDiff[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
						= 0.0f;


			/* smooth unary output */
			for(x=1; x<mriData3D.img_width-1; x++)
				for(y=1; y<mriData3D.img_height-1; y++)
					for(z=1; z<mriData3D.img_slices-1; z++)
						dmyPos[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]=
						(m_lmsPositions[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]+
						m_lmsPositions[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x+1]+
						m_lmsPositions[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x-1]+
						m_lmsPositions[z*mriData3D.img_width*mriData3D.img_height+(y+1)*mriData3D.img_width+(x)]+
						m_lmsPositions[z*mriData3D.img_width*mriData3D.img_height+(y+1)*mriData3D.img_width+(x+1)]+
						m_lmsPositions[z*mriData3D.img_width*mriData3D.img_height+(y+1)*mriData3D.img_width+(x-1)]+
						m_lmsPositions[z*mriData3D.img_width*mriData3D.img_height+(y-1)*mriData3D.img_width+(x)]+
						m_lmsPositions[z*mriData3D.img_width*mriData3D.img_height+(y-1)*mriData3D.img_width+(x+1)]+
						m_lmsPositions[z*mriData3D.img_width*mriData3D.img_height+(y-1)*mriData3D.img_width+(x-1)]+
						m_lmsPositions[(z+1)*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]+
						m_lmsPositions[(z+1)*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+(x+1)]+
						m_lmsPositions[(z+1)*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+(x-1)]+
						m_lmsPositions[(z+1)*mriData3D.img_width*mriData3D.img_height+(y+1)*mriData3D.img_width+x]+
						m_lmsPositions[(z+1)*mriData3D.img_width*mriData3D.img_height+(y+1)*mriData3D.img_width+(x+1)]+
						m_lmsPositions[(z+1)*mriData3D.img_width*mriData3D.img_height+(y+1)*mriData3D.img_width+(x-1)]+
						m_lmsPositions[(z+1)*mriData3D.img_width*mriData3D.img_height+(y-1)*mriData3D.img_width+x]+
						m_lmsPositions[(z+1)*mriData3D.img_width*mriData3D.img_height+(y-1)*mriData3D.img_width+(x+1)]+
						m_lmsPositions[(z+1)*mriData3D.img_width*mriData3D.img_height+(y-1)*mriData3D.img_width+(x-1)]+
						m_lmsPositions[(z-1)*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]+
						m_lmsPositions[(z-1)*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+(x+1)]+
						m_lmsPositions[(z-1)*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+(x-1)]+
						m_lmsPositions[(z-1)*mriData3D.img_width*mriData3D.img_height+(y+1)*mriData3D.img_width+x]+
						m_lmsPositions[(z-1)*mriData3D.img_width*mriData3D.img_height+(y+1)*mriData3D.img_width+(x+1)]+
						m_lmsPositions[(z-1)*mriData3D.img_width*mriData3D.img_height+(y+1)*mriData3D.img_width+(x-1)]+
						m_lmsPositions[(z-1)*mriData3D.img_width*mriData3D.img_height+(y-1)*mriData3D.img_width+x]+
						m_lmsPositions[(z-1)*mriData3D.img_width*mriData3D.img_height+(y-1)*mriData3D.img_width+(x+1)]+
						m_lmsPositions[(z-1)*mriData3D.img_width*mriData3D.img_height+(y-1)*mriData3D.img_width+(x-1)]);

	/*		// apply simple differentiation 
			for(x=1; x<mriData3D.img_width-1; x++)
				for(y=1; y<mriData3D.img_height-1; y++)
					for(z=1; z<mriData3D.img_slices-1; z++)
						dmyDiff[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]=
						(dmyPos[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x-1]
						-dmyPos[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x+1])+
						(dmyPos[(z-1)*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
						-dmyPos[(z+1)*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x])+
						(dmyPos[z*mriData3D.img_width*mriData3D.img_height+(y-1)*mriData3D.img_width+x]
						-dmyPos[z*mriData3D.img_width*mriData3D.img_height+(y+1)*mriData3D.img_width+x])+
						(dmyPos[(z+1)*mriData3D.img_width*mriData3D.img_height+(y+1)*mriData3D.img_width+(x-1)]
						-dmyPos[(z-1)*mriData3D.img_width*mriData3D.img_height+(y-1)*mriData3D.img_width+(x+1)])+
						(dmyPos[(z+1)*mriData3D.img_width*mriData3D.img_height+(y+1)*mriData3D.img_width+(x+1)]
						-dmyPos[(z-1)*mriData3D.img_width*mriData3D.img_height+(y-1)*mriData3D.img_width+(x-1)])+
						(dmyPos[(z-1)*mriData3D.img_width*mriData3D.img_height+(y+1)*mriData3D.img_width+(x+1)]
						-dmyPos[(z+1)*mriData3D.img_width*mriData3D.img_height+(y-1)*mriData3D.img_width+(x-1)])+
						(dmyPos[(z-1)*mriData3D.img_width*mriData3D.img_height+(y-1)*mriData3D.img_width+(x+1)]
						-dmyPos[(z+1)*mriData3D.img_width*mriData3D.img_height+(y+1)*mriData3D.img_width+(x-1)]);
	*/
			/* copy results to return array */
			for(x=0; x<mriData3D.img_width; x++)
				for(y=0; y<mriData3D.img_height; y++)
					for(z=0; z<mriData3D.img_slices; z++)
						dmy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
						= dmyPos[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x];

			delete [] dmyPos;
			delete [] dmyDiff;
	/*
			for(x=0; x<mriData3D.img_width; x++)
				for(y=0; y<mriData3D.img_height; y++)
					for(z=0; z<mriData3D.img_slices; z++)
						dmy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
						= dmy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x] *
						((float)m_lmsPositions[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]);
	*/
		}
	}

	if(m_lmsVtkDistances)
	{
		for(x=0; x<mriData3D.img_width;x++)
			for(y=0; y<mriData3D.img_height; y++)
				for(z=0; z<mriData3D.img_slices; z++)
					dmy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
					= dmy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x] *
					m_lmsDistances[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x];

	}

	if(m_bSeedStrm)
	{
		int* iDmy;

		if(pDlgStrm==NULL)
		{
			AfxMessageBox("SeedMask error: Streamlines not yet generated.");
		}
		else
			if((iDmy = pDlgStrm->GetStrmTxtMagn()) != NULL)
			{
				for(x=0; x<mriData3D.img_width;x++)
					for(y=0; y<mriData3D.img_height; y++)
						for(z=0; z<mriData3D.img_slices; z++)
							dmy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
							= dmy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x] *
							iDmy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x];

				delete iDmy;
			}
	}

	return dmy;
}

void CDlgFindVortex::OnVortexStrmView() 
{
	float* dmy = SeedMask();
	
	if(pDlgStrm != NULL)
		delete pDlgStrm;

	pDlgStrm = new CDlgStreamlines;
	pDlgStrm->pView = pView;
	((CTextonView*)pView)->dlgStrm = pDlgStrm;
	pDlgStrm->Create(IDD_STREAMLS, this);
	pDlgStrm->NoLoadInitialiseMask(&mriData3D, dmy);
	pDlgStrm->ShowWindow(SW_SHOW);

	delete [] dmy;

}

void CDlgFindVortex::OnVortexVtkView() 
{
	/*
	int* dmy = new int[img_width*img_height*img_slices];
	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
			for(z=0; z<img_slices; z++)
				dmy[z*img_width*img_height+y*img_width+x]=
				texture3D[z*img_width*img_height+y*img_width+x]
				*sqrt(pow(mriData3D.u[z*img_width*img_height+y*img_width+x],2.0)+
					pow(mriData3D.v[z*img_width*img_height+y*img_width+x],2.0)+
					pow(mriData3D.w[z*img_width*img_height+y*img_width+x],2.0));
	*/

	float* dmy = SeedMask();

	NormaliseFloat3D(dmy, mriData3D.img_width, mriData3D.img_height, mriData3D.img_slices);

	CVtkUtilities vtkutil;
	vtkutil.WriteImageVtk(dmy, mriData3D.img_width, mriData3D.img_height, mriData3D.img_slices, 
		"c:\\testVOLUME.vtk");

	delete [] dmy;

}

void CDlgFindVortex::NormaliseInt3D(int * in_out, int width, int height, int slices)
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

void CDlgFindVortex::NormaliseFloat3D(float * in_out, int width, int height, int slices)
{
	float min;
	float max;
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

	float range = max - min;
	float m, b;
	m=254.0/range;
	b=254.0-m*(float)max;

	for(x=0; x<width; x++)
		for(y=0; y<height; y++)
			for(z=0; z<slices; z++)
				in_out[z*width*height+y*width+x]
				=FloatToInt( in_out[z*width*height+y*width+x]*m+b+0.00001f );

}

void CDlgFindVortex::OnVortexVtkSelectedVectors() 
{
	int img_width, img_height, img_slices;
	img_width = mriData3D.img_width;
	img_height = mriData3D.img_height;
	img_slices = mriData3D.img_slices;

	// get mask
	float* dmy = SeedMask();
	//NormaliseInt3D(dmy, mriData3D.img_width, mriData3D.img_height, mriData3D.img_slices);

	// save vector data
	// load flow data into vtkRectilinearGrid
	vtkFloatArray *fpoints = vtkFloatArray::New();
	vtkPoints* points = vtkPoints::New();
	vtkFloatArray *fspoints = vtkFloatArray::New();
	fspoints->SetNumberOfComponents(1);
	fpoints->SetNumberOfComponents(3);
	
	const float div = 1.0f;
	int x, y, z;
	if(img_slices > 1)
	{
		for(x=0; x<mriData3D.img_width; x++)
			for(y=0; y<mriData3D.img_height; y++)
				for(z=0; z<mriData3D.img_slices; z++)
					if(dmy[z*img_width*img_height+y*img_width+x]==1)
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
	else
	{
		z = 0;
		for(x=0; x<img_width; x++)
			for(y=0; y<img_height; y++)
				if(dmy[y*img_width+x]==1)
				{
					fspoints->InsertNextTuple1(sqrt(pow(mriData3D.u[z*img_width*img_height+y*img_width+x],2.0)+
						pow(mriData3D.v[z*img_width*img_height+y*img_width+x],2.0))/div);
					points->InsertNextPoint(x, y, z);
					fpoints->InsertNextTuple3(mriData3D.u[z*img_width*img_height+y*img_width+x]/div,
						mriData3D.v[z*img_width*img_height+y*img_width+x]/div, 0.0);
				}
	}
	
	vtkStructuredGrid *sgrid = vtkStructuredGrid::New();
	sgrid->SetDimensions(img_width, img_height, img_slices);
	sgrid->SetPoints(points);
	sgrid->GetPointData()->SetScalars(fspoints);
	sgrid->GetPointData()->SetVectors(fpoints);

	CVtkUtilities vtkutil;
	vtkutil.WriteImageSgrid(sgrid, "c:\\test.vtk");

	points->Delete();
	fpoints->Delete();
	fspoints->Delete();
	sgrid->Delete();

}

void CDlgFindVortex::OnVortexStrmViewMask() 
{
	float* dmy = SeedMask();

	pVolDlg = new CDlgVolView;
	pVolDlg->pView = pView;
	pVolDlg->m_sVolumeTitle.Format("seed mask");
	pVolDlg->InitialiseVolume(dmy, mriData3D.img_width, mriData3D.img_height, mriData3D.img_slices);
	pVolDlg->m_imgInfo = m_ImgSizesTxt;
	pVolDlg->Create(IDD_VOL_VIEW, this);
	pVolDlg->ShowWindow(SW_SHOW);

	delete [] dmy;
	
}


void CDlgFindVortex::OnDlgTracking() 
{
	CDlgTracking dlgTrack;
	dlgTrack.DoModal();
}

void CDlgFindVortex::OnVortexVtkSaveArrows() 
{
	CDlgSaveVtkArrows pDlgArrows;
	pDlgArrows.mps_imgSize.Format("image: (x, y, s) %i x %i x %i", mriData3D.img_width, 
		mriData3D.img_height, mriData3D.img_slices);
	pDlgArrows.DoModal();
	VtkSaveArrows(pDlgArrows.m_bWholeData, pDlgArrows.m_bHCFD, 
		pDlgArrows.m_bS, pDlgArrows.m_coordZbS-1, 
		pDlgArrows.m_bX, pDlgArrows.m_coordX, 
		pDlgArrows.m_bY, pDlgArrows.m_coordY);
	
}

CString CDlgFindVortex::VtkSaveArrows(BOOL b3D, BOOL bHCFD, BOOL bs, int zerob_s, BOOL bx, int ix, BOOL by, int iy, int i_fname)
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
	if(b3D)
	{
		for(y=0; y<img_height; y+=3)
			for(x=0; x<img_width; x+=3)
				for(z=0; z<img_slices; z++)
					if(!(mriData3D.u[z*img_width*img_height+y*img_width+x]==0 && 
					   mriData3D.v[z*img_width*img_height+y*img_width+x]==0 && 
					   mriData3D.w[z*img_width*img_height+y*img_width+x]==0) //&&
					   /*!(x>40 && y>50)*/)
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

	/* OLD VERSION 24/5/03 */
	if(bHCFD)
	{
		// x plane (yz)
		x=12;
		for(y=0; y<85; y++)
			for(z=5; z<img_slices; z++)
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

		// y plane (xz)
		y=84;
		for(x=0; x<12; x++)
			for(z=5; z<img_slices; z++)
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

		// z plane (xy)

		z=5;
		for(x=0; x<12; x++)
			for(y=0; y<85; y++)
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
/*
		z=43;
		for(x=0; x<img_width; x++)
			for(y=0; y<img_height; y++)
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
			*/

	}
	

	// new version compatible with Dr. Long's streamline plot
	/*
	if(bHCFD)
	{
		// x plane (yz)
		x=12;
		for(y=0; y<94; y++)
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

		// y plane (xz)
		y=93;
		for(x=12; x<img_width; x++)
			for(z=5; z<img_slices; z++)
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
	}
	*/
	// x plane (yz)
	if(bx)
	{
		x=ix;
		for(y=0; y<img_height; y++)
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
	}


	// y plane (xz)
	if(by)
	{
		y=iy;
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
	}

	// z plane (xy)
	if(bs)
	{
		z=zerob_s;
		for(x=0; x<img_width; x++)
			for(y=0; y<img_height; y++)
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
	}

	vtkStructuredGrid *sgrid = vtkStructuredGrid::New();
	sgrid->SetDimensions(img_width, img_height, img_slices);
	sgrid->SetPoints(points);
	sgrid->GetPointData()->SetScalars(fspoints);
	sgrid->GetPointData()->SetVectors(fpoints);

	CString fname;
	fname.Format("c:\\testARROWS%i.vtk", i_fname);
	vtkStructuredGridWriter* swriter = vtkStructuredGridWriter::New();
	swriter->SetInput(sgrid);
	swriter->SetFileName(fname);
	swriter->Write();

	fpoints->Delete();
	fspoints->Delete();
	points->Delete();
	sgrid->Delete();
	swriter->Delete();

	return fname;
}

void CDlgFindVortex::VtkSaveVorticityArrows(BOOL b3D, BOOL bHCFD, BOOL bs, int zerob_s, BOOL bx, int ix, BOOL by, int iy)
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
	if(b3D)
	{
		for(y=0; y<img_height; y++)
			for(x=0; x<img_width; x++)
				for(z=0; z<img_slices; z++)
					if(m_vorticityPlotX[z*img_width*img_height+y*img_width+x]!=0 && 
					   m_vorticityPlotY[z*img_width*img_height+y*img_width+x]!=0 && 
					   m_vorticityPlotZ[z*img_width*img_height+y*img_width+x]!=0)
					{
						fspoints->InsertNextTuple1(sqrt(pow(m_vorticityPlotX[z*img_width*img_height+y*img_width+x],2.0)+
							pow(m_vorticityPlotY[z*img_width*img_height+y*img_width+x],2.0)+
							pow(m_vorticityPlotZ[z*img_width*img_height+y*img_width+x],2.0))/div);
						points->InsertNextPoint(x, y, z);
						fpoints->InsertNextTuple3(m_vorticityPlotX[z*img_width*img_height+y*img_width+x]/div,
							m_vorticityPlotY[z*img_width*img_height+y*img_width+x]/div,
							m_vorticityPlotZ[z*img_width*img_height+y*img_width+x]/div);

					}
	}

	if(bHCFD)
	{
		// x plane (yz)
		x=12;
		for(y=0; y<85; y++)
			for(z=5; z<img_slices; z++)
				if(m_vorticityPlotX[z*img_width*img_height+y*img_width+x]!=0 && 
				   m_vorticityPlotY[z*img_width*img_height+y*img_width+x]!=0 && 
				   m_vorticityPlotZ[z*img_width*img_height+y*img_width+x]!=0)
				{
					fspoints->InsertNextTuple1(sqrt(pow(m_vorticityPlotX[z*img_width*img_height+y*img_width+x],2.0)+
						pow(m_vorticityPlotY[z*img_width*img_height+y*img_width+x],2.0)+
						pow(m_vorticityPlotZ[z*img_width*img_height+y*img_width+x],2.0))/div);
					points->InsertNextPoint(x, y, z);
					fpoints->InsertNextTuple3(m_vorticityPlotX[z*img_width*img_height+y*img_width+x]/div,
						m_vorticityPlotY[z*img_width*img_height+y*img_width+x]/div,
						m_vorticityPlotZ[z*img_width*img_height+y*img_width+x]/div);

				}

		// y plane (xz)
		y=84;
		for(x=0; x<12; x++)
			for(z=5; z<img_slices; z++)
				if(m_vorticityPlotX[z*img_width*img_height+y*img_width+x]!=0 && 
				   m_vorticityPlotY[z*img_width*img_height+y*img_width+x]!=0 && 
				   m_vorticityPlotZ[z*img_width*img_height+y*img_width+x]!=0)
				{
					fspoints->InsertNextTuple1(sqrt(pow(m_vorticityPlotX[z*img_width*img_height+y*img_width+x],2.0)+
						pow(m_vorticityPlotY[z*img_width*img_height+y*img_width+x],2.0)+
						pow(m_vorticityPlotZ[z*img_width*img_height+y*img_width+x],2.0))/div);
					points->InsertNextPoint(x, y, z);
					fpoints->InsertNextTuple3(m_vorticityPlotX[z*img_width*img_height+y*img_width+x]/div,
						m_vorticityPlotY[z*img_width*img_height+y*img_width+x]/div,
						m_vorticityPlotZ[z*img_width*img_height+y*img_width+x]/div);

				}

			

		// z plane (xy)
		z=5;
		for(x=0; x<12; x++)
			for(y=0; y<85; y++)
				if(m_vorticityPlotX[z*img_width*img_height+y*img_width+x]!=0 && 
				   m_vorticityPlotY[z*img_width*img_height+y*img_width+x]!=0 && 
				   m_vorticityPlotZ[z*img_width*img_height+y*img_width+x]!=0)
				{
					fspoints->InsertNextTuple1(sqrt(pow(m_vorticityPlotX[z*img_width*img_height+y*img_width+x],2.0)+
						pow(m_vorticityPlotY[z*img_width*img_height+y*img_width+x],2.0)+
						pow(m_vorticityPlotZ[z*img_width*img_height+y*img_width+x],2.0))/div);
					points->InsertNextPoint(x, y, z);
					fpoints->InsertNextTuple3(m_vorticityPlotX[z*img_width*img_height+y*img_width+x]/div,
						m_vorticityPlotY[z*img_width*img_height+y*img_width+x]/div,
						m_vorticityPlotZ[z*img_width*img_height+y*img_width+x]/div);

				}
	}

	// x plane (yz)
	if(bx)
	{
		x=ix;
		for(y=0; y<img_height; y++)
			for(z=0; z<img_slices; z++)
				if(m_vorticityPlotX[z*img_width*img_height+y*img_width+x]!=0 && 
				   m_vorticityPlotY[z*img_width*img_height+y*img_width+x]!=0 && 
				   m_vorticityPlotZ[z*img_width*img_height+y*img_width+x]!=0)
				{
					fspoints->InsertNextTuple1(sqrt(pow(m_vorticityPlotX[z*img_width*img_height+y*img_width+x],2.0)+
						pow(m_vorticityPlotY[z*img_width*img_height+y*img_width+x],2.0)+
						pow(m_vorticityPlotZ[z*img_width*img_height+y*img_width+x],2.0))/div);
					points->InsertNextPoint(x, y, z);
					fpoints->InsertNextTuple3(m_vorticityPlotX[z*img_width*img_height+y*img_width+x]/div,
						m_vorticityPlotY[z*img_width*img_height+y*img_width+x]/div,
						m_vorticityPlotZ[z*img_width*img_height+y*img_width+x]/div);

				}
	}

	// y plane (xz)
	if(by)
	{
		y=iy;
		for(x=0; x<img_width; x++)
			for(z=0; z<img_slices; z++)
				if(m_vorticityPlotX[z*img_width*img_height+y*img_width+x]!=0 && 
				   m_vorticityPlotY[z*img_width*img_height+y*img_width+x]!=0 && 
				   m_vorticityPlotZ[z*img_width*img_height+y*img_width+x]!=0)
				{
					fspoints->InsertNextTuple1(sqrt(pow(m_vorticityPlotX[z*img_width*img_height+y*img_width+x],2.0)+
						pow(m_vorticityPlotY[z*img_width*img_height+y*img_width+x],2.0)+
						pow(m_vorticityPlotZ[z*img_width*img_height+y*img_width+x],2.0))/div);
					points->InsertNextPoint(x, y, z);
					fpoints->InsertNextTuple3(m_vorticityPlotX[z*img_width*img_height+y*img_width+x]/div,
						m_vorticityPlotY[z*img_width*img_height+y*img_width+x]/div,
						m_vorticityPlotZ[z*img_width*img_height+y*img_width+x]/div);

				}
	}

	// z plane (xy)
	if(bs)
	{
		z=zerob_s;
		for(x=0; x<img_width; x++)
			for(y=0; y<img_height; y++)
				if(m_vorticityPlotX[z*img_width*img_height+y*img_width+x]!=0 && 
				   m_vorticityPlotY[z*img_width*img_height+y*img_width+x]!=0 && 
				   m_vorticityPlotZ[z*img_width*img_height+y*img_width+x]!=0)
				{
					fspoints->InsertNextTuple1(sqrt(pow(m_vorticityPlotX[z*img_width*img_height+y*img_width+x],2.0)+
						pow(m_vorticityPlotY[z*img_width*img_height+y*img_width+x],2.0)+
						pow(m_vorticityPlotZ[z*img_width*img_height+y*img_width+x],2.0))/div);
					points->InsertNextPoint(x, y, z);
					fpoints->InsertNextTuple3(m_vorticityPlotX[z*img_width*img_height+y*img_width+x]/div,
						m_vorticityPlotY[z*img_width*img_height+y*img_width+x]/div,
						m_vorticityPlotZ[z*img_width*img_height+y*img_width+x]/div);

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

void CDlgFindVortex::OnViewLmsPositions() 
{
	CDlgVolView* pPosDlg = new CDlgVolView;
	pPosDlg->pView = pView;
	pPosDlg->m_sVolumeTitle.Format("position hits from LMS");
	pPosDlg->InitialiseVolume(m_lmsPositions, mriData3D.img_width, mriData3D.img_height, 
		mriData3D.img_slices);
	pPosDlg->m_imgInfo = m_ImgSizesTxt;
	pPosDlg->Create(IDD_VOL_VIEW, this);
	pPosDlg->ShowWindow(SW_SHOW);
	
}

void CDlgFindVortex::OnViewLmsDistances() 
{
/*	CDlgVolView* pDistDlg = new CDlgVolView;
	pDistDlg->pView = pView;
	pDistDlg->m_sVolumeTitle.Format("distance of point from vortex core");
	pDistDlg->InitialiseVolume(m_lmsDistances, mriData3D.img_width, mriData3D.img_height, 
		mriData3D.img_slices);
	pDistDlg->m_imgInfo = m_ImgSizesTxt;
	pDistDlg->Create(IDD_VOL_VIEW, this);
	pDistDlg->ShowWindow(SW_SHOW);
*/
}

void CDlgFindVortex::ComputeVelocityGradients()
{
	if(m_velGrad_dudx != NULL)
		delete [] m_velGrad_dudx;
	if(m_velGrad_dudy != NULL) 
		delete [] m_velGrad_dudy;
	if(m_velGrad_dudz != NULL)
		delete [] m_velGrad_dudz;
	if(m_velGrad_dvdx != NULL) 
		delete [] m_velGrad_dvdx;
	if(m_velGrad_dvdy != NULL) 
		delete [] m_velGrad_dvdy;
	if(m_velGrad_dvdz != NULL)
		delete [] m_velGrad_dvdz;
	if(m_velGrad_dwdx != NULL) 
		delete [] m_velGrad_dwdx;
	if(m_velGrad_dwdy != NULL) 
		delete [] m_velGrad_dwdy;
	if(m_velGrad_dwdz != NULL)
		delete [] m_velGrad_dwdz;

	UpdateData(TRUE);

	if(m_lmsUse)
	{

		((CButton*)GetDlgItem(IDC_LMS_POS))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_LMS_DISTS))->EnableWindow(TRUE);

		/* ******* LEAST MEAN SQUARES METHOD ******** */

		int x, y, z;
		if(m_lmsPositions != NULL)
			delete [] m_lmsPositions;
		if(m_lmsDistances != NULL)
			delete [] m_lmsDistances;


		if(mriData3D.img_slices>1)
		{
			m_lmsPositions = new int[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
//			m_lmsDistances = new int[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];

			m_velGrad_dudx = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
			m_velGrad_dudy = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
			m_velGrad_dudz = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
			m_velGrad_dvdx = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
			m_velGrad_dvdy = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
			m_velGrad_dvdz = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
			m_velGrad_dwdx = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
			m_velGrad_dwdy = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
			m_velGrad_dwdz = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];

			for(x=0; x<mriData3D.img_width;x++)
				for(y=0; y<mriData3D.img_height; y++)
					for(z=0; z<mriData3D.img_slices; z++)
						m_velGrad_dudx[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]=
						m_velGrad_dudy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]=
						m_velGrad_dudz[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]=
						m_velGrad_dvdx[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]=
						m_velGrad_dvdy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]=
						m_velGrad_dvdz[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]=
						m_velGrad_dwdx[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]=
						m_velGrad_dwdy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]=
						m_velGrad_dwdz[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]=
						m_lmsPositions[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]=
						0.0f;

			// slide block region over data
			for(x=m_lmsRegionSize; x<mriData3D.img_width-m_lmsRegionSize;x++)
				for(y=m_lmsRegionSize; y<mriData3D.img_height-m_lmsRegionSize; y++)
					for(z=m_lmsRegionSize; z<mriData3D.img_slices-m_lmsRegionSize; z++)
						LmsSetVelocityGradients(x, y, z);

		}
		else
		{
			m_lmsPositions = new int[mriData3D.img_width*mriData3D.img_height];
//			m_lmsDistances = new int[mriData3D.img_width*mriData3D.img_height];

			z=0;
			m_velGrad_dudx = new float[mriData3D.img_width*mriData3D.img_height];
			m_velGrad_dudy = new float[mriData3D.img_width*mriData3D.img_height];
			m_velGrad_dudz = NULL;
			m_velGrad_dvdx = new float[mriData3D.img_width*mriData3D.img_height];
			m_velGrad_dvdy = new float[mriData3D.img_width*mriData3D.img_height];
			m_velGrad_dvdz = NULL;
			m_velGrad_dwdx = NULL;
			m_velGrad_dwdy = NULL;
			m_velGrad_dwdz = NULL;

			for(x=0; x<mriData3D.img_width;x++)
				for(y=0; y<mriData3D.img_height; y++)
					m_velGrad_dudx[y*mriData3D.img_width+x]=
					m_velGrad_dudy[y*mriData3D.img_width+x]=
					m_velGrad_dvdx[y*mriData3D.img_width+x]=
					m_velGrad_dvdy[y*mriData3D.img_width+x]=
					m_lmsPositions[y*mriData3D.img_width+x]=
					0.0f;

			// slide block region over data
			for(x=m_lmsRegionSize; x<mriData3D.img_width-m_lmsRegionSize;x++)
				for(y=m_lmsRegionSize; y<mriData3D.img_height-m_lmsRegionSize; y++)
					LmsSetVelocityGradients(x, y, 0);


		}
	}
	else
	{

		/* ******* DIFFERENCES METHOD ******** */

		int x, y, z;
		if(mriData3D.img_slices>1)
		{
			m_velGrad_dudx = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
			m_velGrad_dudy = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
			m_velGrad_dudz = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
			m_velGrad_dvdx = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
			m_velGrad_dvdy = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
			m_velGrad_dvdz = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
			m_velGrad_dwdx = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
			m_velGrad_dwdy = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
			m_velGrad_dwdz = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];

			for(x=0; x<mriData3D.img_width;x++)
				for(y=0; y<mriData3D.img_height; y++)
					for(z=0; z<mriData3D.img_slices; z++)
						m_velGrad_dudx[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]=
						m_velGrad_dudy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]=
						m_velGrad_dudz[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]=
						m_velGrad_dvdx[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]=
						m_velGrad_dvdy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]=
						m_velGrad_dvdz[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]=
						m_velGrad_dwdx[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]=
						m_velGrad_dwdy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]=
						m_velGrad_dwdz[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]=
						0.0f;

			for(x=1; x<mriData3D.img_width-1;x++)
				for(y=1; y<mriData3D.img_height-1; y++)
					for(z=1; z<mriData3D.img_slices-1; z++)
					{
						
						// compute "derivatives"
						m_velGrad_dudx[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
							= (mriData3D.u[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+(x+1)]
							- mriData3D.u[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+(x-1)])
							/ 2.0f;
						
						m_velGrad_dudy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
							= (mriData3D.u[z*mriData3D.img_width*mriData3D.img_height+(y+1)*mriData3D.img_width+x]
							- mriData3D.u[z*mriData3D.img_width*mriData3D.img_height+(y-1)*mriData3D.img_width+x])
							/ 2.0f;
						
						m_velGrad_dudz[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
							= (mriData3D.u[(z+1)*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
							- mriData3D.u[(z-1)*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x])
							/ 2.0f;
						
						m_velGrad_dvdx[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
							= (mriData3D.v[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+(x+1)]
							- mriData3D.v[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+(x-1)])
							/ 2.0f;
						
						m_velGrad_dvdy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
							= (mriData3D.v[z*mriData3D.img_width*mriData3D.img_height+(y+1)*mriData3D.img_width+x]
							- mriData3D.v[z*mriData3D.img_width*mriData3D.img_height+(y-1)*mriData3D.img_width+x])
							/ 2.0f;
						
						m_velGrad_dvdz[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
							= (mriData3D.v[(z+1)*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
							- mriData3D.v[(z-1)*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x])
							/ 2.0f;
						
						m_velGrad_dwdx[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
							= (mriData3D.w[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+(x+1)]
							- mriData3D.w[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+(x-1)])
							/ 2.0f;
						
						m_velGrad_dwdy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
							= (mriData3D.w[z*mriData3D.img_width*mriData3D.img_height+(y+1)*mriData3D.img_width+x]
							- mriData3D.w[z*mriData3D.img_width*mriData3D.img_height+(y-1)*mriData3D.img_width+x])
							/ 2.0f;
						
						m_velGrad_dwdz[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
							= (mriData3D.w[(z+1)*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
							- mriData3D.w[(z-1)*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x])
							/ 2.0f;
											
					}
		}
		else
		{
			z=0;
			m_velGrad_dudx = new float[mriData3D.img_width*mriData3D.img_height];
			m_velGrad_dudy = new float[mriData3D.img_width*mriData3D.img_height];
			m_velGrad_dudz = NULL;
			m_velGrad_dvdx = new float[mriData3D.img_width*mriData3D.img_height];
			m_velGrad_dvdy = new float[mriData3D.img_width*mriData3D.img_height];
			m_velGrad_dvdz = NULL;
			m_velGrad_dwdx = NULL;
			m_velGrad_dwdy = NULL;
			m_velGrad_dwdz = NULL;

			for(x=0; x<mriData3D.img_width;x++)
				for(y=0; y<mriData3D.img_height; y++)
					m_velGrad_dudx[y*mriData3D.img_width+x]=
					m_velGrad_dudy[y*mriData3D.img_width+x]=
					m_velGrad_dvdx[y*mriData3D.img_width+x]=
					m_velGrad_dvdy[y*mriData3D.img_width+x]=
					0.0f;

			const int RAD = 1;

			for(x=RAD; x<mriData3D.img_width-RAD;x++)
				for(y=RAD; y<mriData3D.img_height-RAD; y++)
					{	

						// compute "derivatives"
						m_velGrad_dudx[y*mriData3D.img_width+x]
							= (mriData3D.u[y*mriData3D.img_width+(x+RAD)]
							- mriData3D.u[y*mriData3D.img_width+(x-RAD)])
							/ 2.0f;
						
						m_velGrad_dudy[y*mriData3D.img_width+x]
							= (mriData3D.u[(y+RAD)*mriData3D.img_width+x]
							- mriData3D.u[(y-RAD)*mriData3D.img_width+x])
							/ 2.0f;
						
						m_velGrad_dvdx[y*mriData3D.img_width+x]
							= (mriData3D.v[y*mriData3D.img_width+(x+RAD)]
							- mriData3D.v[y*mriData3D.img_width+(x-RAD)])
							/ 2.0f;
						
						m_velGrad_dvdy[y*mriData3D.img_width+x]
							= (mriData3D.v[(y+RAD)*mriData3D.img_width+x]
							- mriData3D.v[(y-RAD)*mriData3D.img_width+x])
							/ 2.0f;
											
						
					}
		}
	}

}

void CDlgFindVortex::LmsSetVelocityGradients(int x, int y, int z)
{

	if(mriData3D.img_slices>1)
	{
		// skip over regions having null velocity
		int xc, yc, zc;
		for(xc=x-m_lmsRegionSize; xc <= x+m_lmsRegionSize; xc++)
			for(yc=y-m_lmsRegionSize; yc <= y+m_lmsRegionSize; yc++)
				for(zc=z-m_lmsRegionSize; zc <= z+m_lmsRegionSize; zc++)
					if(  abs(mriData3D.u[zc*mriData3D.img_width*mriData3D.img_height+yc*mriData3D.img_width+xc])
						+abs(mriData3D.v[zc*mriData3D.img_width*mriData3D.img_height+yc*mriData3D.img_width+xc])
						+abs(mriData3D.w[zc*mriData3D.img_width*mriData3D.img_height+yc*mriData3D.img_width+xc])==0)
						return;

		// set up matrix A
		float** nrMatrixA;
		nrMatrixA = matrix(1, 3, 1, 3);

		// compute matrix A
		LmsComputeMatrixA(nrMatrixA, x, y, z);

		// set gradients to A's coeffs
		m_velGrad_dudx[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]=
			nrMatrixA[1][1];
		m_velGrad_dudy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]=
			nrMatrixA[1][2];
		m_velGrad_dudz[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]=
			nrMatrixA[1][3];
		m_velGrad_dvdx[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]=
			nrMatrixA[2][1];
		m_velGrad_dvdy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]=
			nrMatrixA[2][2];
		m_velGrad_dvdz[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]=
			nrMatrixA[2][3];
		m_velGrad_dwdx[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]=
			nrMatrixA[3][1];
		m_velGrad_dwdy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]=
			nrMatrixA[3][2];
		m_velGrad_dwdz[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]=
			nrMatrixA[3][3];

		free_matrix(nrMatrixA,1,3,1,3);

	}
	else
	{

		// skip over regions having null velocity
		int xc, yc;
		for(xc=x-m_lmsRegionSize; xc <= x+m_lmsRegionSize; xc++)
			for(yc=y-m_lmsRegionSize; yc <= y+m_lmsRegionSize; yc++)
				if(abs(mriData3D.u[yc*mriData3D.img_width+xc])
				  +abs(mriData3D.v[yc*mriData3D.img_width+xc])==0)
						return;

		// set up matrix A
		float** nrMatrixA;
		nrMatrixA = matrix(1, 2, 1, 2);

		// compute matrix A
		LmsComputeMatrixA(nrMatrixA, x, y, 0);

		// set gradients to A's coeffs
		m_velGrad_dudx[y*mriData3D.img_width+x] = nrMatrixA[1][1];
		m_velGrad_dudy[y*mriData3D.img_width+x] = nrMatrixA[1][2];
		m_velGrad_dvdx[y*mriData3D.img_width+x] = nrMatrixA[2][1];
		m_velGrad_dvdy[y*mriData3D.img_width+x] = nrMatrixA[2][2];

		free_matrix(nrMatrixA,1,2,1,2);

	}

}

float ErrorVel()
{
	AfxMessageBox("Error: wrong index");
	return 0.0f;
}


void CDlgFindVortex::LmsComputeMatrixA(float **matrix_a, int x, int y, int z)
{

	if(mriData3D.img_slices>1)
	{
		int xc, yc, zc, index;
		float reg_size = pow(m_lmsRegionSize*2+1, 3.0);

		// compute average velocity
		m_lmsAverageV[0] = m_lmsAverageV[1] = m_lmsAverageV[2] = 0.0f;
		for(xc=x-m_lmsRegionSize; xc <= x+m_lmsRegionSize; xc++)
			for(yc=y-m_lmsRegionSize; yc <= y+m_lmsRegionSize; yc++)
				for(zc=z-m_lmsRegionSize; zc <= z+m_lmsRegionSize; zc++)
				{
					index = zc*mriData3D.img_width*mriData3D.img_height+yc*mriData3D.img_width+xc;
					m_lmsAverageV[0] = m_lmsAverageV[0] + ((float)mriData3D.u[index]);
					m_lmsAverageV[1] = m_lmsAverageV[1] + ((float)mriData3D.v[index]);
					m_lmsAverageV[2] = m_lmsAverageV[2] + ((float)mriData3D.w[index]);
				}

		m_lmsAverageV[0] = m_lmsAverageV[0]/reg_size;
		m_lmsAverageV[1] = m_lmsAverageV[1]/reg_size;
		m_lmsAverageV[2] = m_lmsAverageV[2]/reg_size;	

		// compute W, V
		int i,j;
		float w_matrix[3][3], v_matrix[3][3];
		for(i=0; i<3; i++)
			for(j=0; j<3; j++)
				w_matrix[i][j]=v_matrix[i][j]=0.0f;

		float tmp_w;
		for(xc=x-m_lmsRegionSize; xc <= x+m_lmsRegionSize; xc++)
			for(yc=y-m_lmsRegionSize; yc <= y+m_lmsRegionSize; yc++)
				for(zc=z-m_lmsRegionSize; zc <= z+m_lmsRegionSize; zc++)
					for(i=0; i<3; i++)
						for(j=0; j<3; j++)
							{
								index = zc*mriData3D.img_width*mriData3D.img_height+yc*mriData3D.img_width+xc;
																
								tmp_w = 
									((float)(i==0?xc-x:i==1?yc-y:i==2?zc-z:ErrorVel()))*
									(j==0?((float)mriData3D.u[index])-m_lmsAverageV[0]:
									 j==1?((float)mriData3D.v[index])-m_lmsAverageV[1]:
									 j==2?((float)mriData3D.w[index])-m_lmsAverageV[2]:ErrorVel());

								w_matrix[i][j]=w_matrix[i][j]+tmp_w;

								v_matrix[i][j]=v_matrix[i][j]+
									((float)(i==0?xc-x:i==1?yc-y:i==2?zc-z:0)*
									(j==0?xc-x:j==1?yc-y:j==2?zc-z:ErrorVel()));
							}

		for(i=0; i<3; i++)
			for(j=0; j<3; j++)
				{
					//if(w_matrix[i][j]>0.001f)
					//	MessageBeep(0);
					w_matrix[i][j]=w_matrix[i][j]/reg_size;
					v_matrix[i][j]=v_matrix[i][j]/reg_size;
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
		inverter.gaussj(nrMatrixVinv, 3, nrVectorB, 1);

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
		
		free_matrix(nrMatrixVinv,1,3,1,3);

		// set position counter
		// compute A^-1
		float** nrMatrixAinv;
		nrMatrixAinv = matrix(1, 3, 1, 3);
		nrVectorB[1][1]=nrVectorB[1][2]=nrVectorB[1][3]=1.0;

		//if(x==14 && y==13 && z==1)
		//	MessageBeep(0);

		nrMatrixAinv[1][1] = matrix_a[1][1];
		nrMatrixAinv[1][2] = matrix_a[1][2];
		nrMatrixAinv[1][3] = matrix_a[1][3];
		nrMatrixAinv[2][1] = matrix_a[2][1];
		nrMatrixAinv[2][2] = matrix_a[2][2];
		nrMatrixAinv[2][3] = matrix_a[2][3];
		nrMatrixAinv[3][1] = matrix_a[3][1];
		nrMatrixAinv[3][2] = matrix_a[3][2];
		nrMatrixAinv[3][3] = matrix_a[3][3];

		ComputeEigenvalues3D(matrix_a[1][1], matrix_a[1][2], matrix_a[1][3],
			matrix_a[2][1], matrix_a[2][2], matrix_a[2][3], 
			matrix_a[3][1], matrix_a[3][2], matrix_a[3][3], 
			x, y, z);

		if(inverter.gaussj(nrMatrixAinv, 3, nrVectorB, 1))
		{

			index = z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x;

			// x0 ***********************

			float fx, fy, fz;	
			fx = ((float)x) - (nrMatrixAinv[1][1]*m_lmsAverageV[0]
							  +nrMatrixAinv[1][2]*m_lmsAverageV[1]
							  +nrMatrixAinv[1][3]*m_lmsAverageV[2]);
			
			fy = ((float)y) - (nrMatrixAinv[2][1]*m_lmsAverageV[0]
							  +nrMatrixAinv[2][2]*m_lmsAverageV[1]
							  +nrMatrixAinv[2][3]*m_lmsAverageV[2]);

			fz = ((float)z) - (nrMatrixAinv[3][1]*m_lmsAverageV[0]
							  +nrMatrixAinv[3][2]*m_lmsAverageV[1]
							  +nrMatrixAinv[3][3]*m_lmsAverageV[2]);

			// w ************************
			matrix3d A(matrix_a[1][1], matrix_a[1][2], matrix_a[1][3],
				matrix_a[2][1], matrix_a[2][2], matrix_a[2][3], 
				matrix_a[3][1], matrix_a[3][2], matrix_a[3][3]);
			
			vector3d dx(fx, fy, fz);
			
			eig3d eig(A);
			if(!eig.complex_eigenvalues())
				goto Abort;

			double lambda = eig.real_eigenvalue();
			vector3d w(A.eigenvector(lambda));
				
			// y = (x0 - x) + w.(x - x0)w
			float yx, yy, yz;
			float wxx0; // w.(x-x0)

			wxx0 = w[0]*(x-fx) + w[1]*(y-fy) + w[2]*(z-fz);
			
			yx = fx - x + wxx0*w[0];
			yy = fy - y + wxx0*w[1];
			yz = fz - z + wxx0*w[2];

			// vc = x0 + aw = x0 + w.(x - x0)w
			float vcx, vcy, vcz;
			vcx = fx + wxx0*w[0];
			vcy = fy + wxx0*w[1];
			vcz = fz + wxx0*w[2];

			float eig_imag, eig_real;
			int index2;

			if(vcx >= 0.0f && vcx<mriData3D.img_width &&
			   vcy >= 0.0f && vcy<mriData3D.img_height &&
			   vcz >= 0.0f && vcz<mriData3D.img_slices)
			{

				index2 = FloatToInt(vcz)*mriData3D.img_width*mriData3D.img_height
					+FloatToInt(vcy)*mriData3D.img_width
					+FloatToInt(vcx);

				eig_imag = m_eigenImag[index];
				eig_real = m_eigenReal[index];

				if(eig_imag > eig_real )
				{
					m_lmsPositions[index2]++;
				}
			}
		}

Abort:
		free_matrix(nrVectorB,1,3,1,1);
		free_matrix(nrMatrixAinv,1,3,1,3);

	}
	else
	{
		int xc, yc, index;
		float reg_size = pow(m_lmsRegionSize*2+1, 2.0);

		// compute average velocity
		m_lmsAverageV[0] = m_lmsAverageV[1] = m_lmsAverageV[2] = 0.0f;
		for(xc=x-m_lmsRegionSize; xc <= x+m_lmsRegionSize; xc++)
			for(yc=y-m_lmsRegionSize; yc <= y+m_lmsRegionSize; yc++)
				{
					index = yc*mriData3D.img_width+xc;
					m_lmsAverageV[0] = m_lmsAverageV[0] + ((float)mriData3D.u[index]);
					m_lmsAverageV[1] = m_lmsAverageV[1] + ((float)mriData3D.v[index]);
				}

		m_lmsAverageV[0] = m_lmsAverageV[0]/reg_size;
		m_lmsAverageV[1] = m_lmsAverageV[1]/reg_size;

		// compute W, V
		int i,j;
		float w_matrix[2][2], v_matrix[2][2];
		for(i=0; i<2; i++)
			for(j=0; j<2; j++)
				w_matrix[i][j]=v_matrix[i][j]=0.0f;

		float tmp_w;
		for(xc=x-m_lmsRegionSize; xc <= x+m_lmsRegionSize; xc++)
			for(yc=y-m_lmsRegionSize; yc <= y+m_lmsRegionSize; yc++)
				for(i=0; i<2; i++)
					for(j=0; j<2; j++)
						{
							index = yc*mriData3D.img_width+xc;

							tmp_w = 
								((float)(i==0?xc-x:i==1?yc-y:ErrorVel()))*
								(j==0?((float)mriData3D.u[index])-m_lmsAverageV[0]:
								 j==1?((float)mriData3D.v[index])-m_lmsAverageV[1]:ErrorVel());

							w_matrix[i][j]=w_matrix[i][j]+tmp_w;

							v_matrix[i][j]=v_matrix[i][j]+
								((float)(i==0?xc-x:i==1?yc-y:0)*
								(j==0?xc-x:j==1?yc-y:ErrorVel()));
						}

		for(i=0; i<2; i++)
			for(j=0; j<2; j++)
				{
					//if(w_matrix[i][j]>0.001f)
					//	MessageBeep(0);
					w_matrix[i][j]=w_matrix[i][j]/reg_size;
					v_matrix[i][j]=v_matrix[i][j]/reg_size;
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
		inverter.gaussj(nrMatrixVinv, 2, nrVectorB, 1);

		// compute Tr(V^-1)
		float trace_Vinv = nrMatrixVinv[1][1]+nrMatrixVinv[2][2];

		// compute Ahat = W^T V^-1
		float matrix_Ahat[2][2];
		for(i=0; i<2; i++)
			for(j=0; j<2; j++)
				matrix_Ahat[i][j]=
					w_matrix[0][i]*nrMatrixVinv[1][j+1]+
					w_matrix[1][i]*nrMatrixVinv[2][j+1];

		// copy Ahat to A (note it's passed from caller as a numerical recipes matrix)
		// no need to make trace-free in 2D
		for(i=0; i<2; i++)
			for(j=0; j<2; j++)
				matrix_a[i+1][j+1]=matrix_Ahat[i][j];
		
		free_matrix(nrMatrixVinv,1,2,1,2);

		// set position counter
		// compute A^-1
		float** nrMatrixAinv;
		nrMatrixAinv = matrix(1, 2, 1, 2);
		nrVectorB[1][1]=nrVectorB[1][2]=1.0;

		nrMatrixAinv[1][1] = matrix_a[1][1];
		nrMatrixAinv[1][2] = matrix_a[1][2];
		nrMatrixAinv[2][1] = matrix_a[2][1];
		nrMatrixAinv[2][2] = matrix_a[2][2];

		ComputeEigenvalues2D(matrix_a[1][1], matrix_a[1][2],
			matrix_a[2][1], matrix_a[2][2], x, y);

		if(inverter.gaussj(nrMatrixAinv, 2, nrVectorB, 1))
		{

			index = y*mriData3D.img_width+x;
			int fx, fy;	
			fx = ((float)x) - (nrMatrixAinv[1][1]*m_lmsAverageV[0]
							  +nrMatrixAinv[1][2]*m_lmsAverageV[1]);
			
			fy = ((float)y) - (nrMatrixAinv[2][1]*m_lmsAverageV[0]
							  +nrMatrixAinv[2][2]*m_lmsAverageV[1]);


			float eig_imag, eig_real;
			int index2;
			
			if(fx >= 0.0f && fx<mriData3D.img_width &&
			   fy >= 0.0f && fy<mriData3D.img_height )
			   // && LmsPointDistance(fx, fy, 0, x, y, 0) <= m_lmsRegionSize
			{
				index2 = FloatToInt(fy)*mriData3D.img_width+FloatToInt(fx);

				eig_imag = m_eigenImag[index];
				eig_real = m_eigenReal[index];

				if(eig_imag > eig_real)
					m_lmsPositions[index2]++; 
			}
		}

		free_matrix(nrVectorB,1,2,1,1);
		free_matrix(nrMatrixAinv,1,2,1,2);

	}

}

/*
void CDlgFindVortex::LmsComputeMatrixA(float **matrix_a, int x, int y, int z)
{

	if(mriData3D.img_slices>1)
	{
		int xc, yc, zc, index;
		float reg_size = pow(m_lmsRegionSize*2+1, 3.0);

		// compute average velocity
		m_lmsAverageV[0] = m_lmsAverageV[1] = m_lmsAverageV[2] = 0.0f;
		for(xc=x-m_lmsRegionSize; xc <= x+m_lmsRegionSize; xc++)
			for(yc=y-m_lmsRegionSize; yc <= y+m_lmsRegionSize; yc++)
				for(zc=z-m_lmsRegionSize; zc <= z+m_lmsRegionSize; zc++)
				{
					index = zc*mriData3D.img_width*mriData3D.img_height+yc*mriData3D.img_width+xc;
					m_lmsAverageV[0] = m_lmsAverageV[0] + ((float)mriData3D.u[index]);
					m_lmsAverageV[1] = m_lmsAverageV[1] + ((float)mriData3D.v[index]);
					m_lmsAverageV[2] = m_lmsAverageV[2] + ((float)mriData3D.w[index]);
				}

		m_lmsAverageV[0] = m_lmsAverageV[0]/reg_size;
		m_lmsAverageV[1] = m_lmsAverageV[1]/reg_size;
		m_lmsAverageV[2] = m_lmsAverageV[2]/reg_size;	

		// compute W, V
		int i,j;
		float w_matrix[3][3], v_matrix[3][3];
		for(i=0; i<3; i++)
			for(j=0; j<3; j++)
				w_matrix[i][j]=v_matrix[i][j]=0.0f;

		float tmp_w;
		for(xc=x-m_lmsRegionSize; xc <= x+m_lmsRegionSize; xc++)
			for(yc=y-m_lmsRegionSize; yc <= y+m_lmsRegionSize; yc++)
				for(zc=z-m_lmsRegionSize; zc <= z+m_lmsRegionSize; zc++)
					for(i=0; i<3; i++)
						for(j=0; j<3; j++)
							{
								index = zc*mriData3D.img_width*mriData3D.img_height+yc*mriData3D.img_width+xc;
																
								tmp_w = 
									((float)(i==0?xc-x:i==1?yc-y:i==2?zc-z:ErrorVel()))*
									(j==0?((float)mriData3D.u[index])-m_lmsAverageV[0]:
									 j==1?((float)mriData3D.v[index])-m_lmsAverageV[1]:
									 j==2?((float)mriData3D.w[index])-m_lmsAverageV[2]:ErrorVel());

								w_matrix[i][j]=w_matrix[i][j]+tmp_w;

								v_matrix[i][j]=v_matrix[i][j]+
									((float)(i==0?xc-x:i==1?yc-y:i==2?zc-z:0)*
									(j==0?xc-x:j==1?yc-y:j==2?zc-z:ErrorVel()));
							}

		for(i=0; i<3; i++)
			for(j=0; j<3; j++)
				{
					//if(w_matrix[i][j]>0.001f)
					//	MessageBeep(0);
					w_matrix[i][j]=w_matrix[i][j]/reg_size;
					v_matrix[i][j]=v_matrix[i][j]/reg_size;
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
		inverter.gaussj(nrMatrixVinv, 3, nrVectorB, 1);

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
		
		free_matrix(nrMatrixVinv,1,3,1,3);

		// set position counter
		// compute A^-1
		float** nrMatrixAinv;
		nrMatrixAinv = matrix(1, 3, 1, 3);
		nrVectorB[1][1]=nrVectorB[1][2]=nrVectorB[1][3]=1.0;

		//if(x==14 && y==13 && z==1)
		//	MessageBeep(0);

		nrMatrixAinv[1][1] = matrix_a[1][1];
		nrMatrixAinv[1][2] = matrix_a[1][2];
		nrMatrixAinv[1][3] = matrix_a[1][3];
		nrMatrixAinv[2][1] = matrix_a[2][1];
		nrMatrixAinv[2][2] = matrix_a[2][2];
		nrMatrixAinv[2][3] = matrix_a[2][3];
		nrMatrixAinv[3][1] = matrix_a[3][1];
		nrMatrixAinv[3][2] = matrix_a[3][2];
		nrMatrixAinv[3][3] = matrix_a[3][3];

		ComputeEigenvalues3D(matrix_a[1][1], matrix_a[1][2], matrix_a[1][3],
			matrix_a[2][1], matrix_a[2][2], matrix_a[2][3], 
			matrix_a[3][1], matrix_a[3][2], matrix_a[3][3], 
			x, y, z);

		if(inverter.gaussj(nrMatrixAinv, 3, nrVectorB, 1))
		{

			index = z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x;
			int fx, fy, fz;	
			fx = ((float)x) - (nrMatrixAinv[1][1]*m_lmsAverageV[0]
							  +nrMatrixAinv[1][2]*m_lmsAverageV[1]
							  +nrMatrixAinv[1][3]*m_lmsAverageV[2]);
			
			fy = ((float)y) - (nrMatrixAinv[2][1]*m_lmsAverageV[0]
							  +nrMatrixAinv[2][2]*m_lmsAverageV[1]
							  +nrMatrixAinv[2][3]*m_lmsAverageV[2]);

			fz = ((float)z) - (nrMatrixAinv[3][1]*m_lmsAverageV[0]
							  +nrMatrixAinv[3][2]*m_lmsAverageV[1]
							  +nrMatrixAinv[3][3]*m_lmsAverageV[2]);

			float eig_imag, eig_real;
			int index2;

			if(fx >= 0.0f && fx<mriData3D.img_width &&
			   fy >= 0.0f && fy<mriData3D.img_height &&
			   fz >= 0.0f && fz<mriData3D.img_slices)
			{

				index2 = FloatToInt(fz)*mriData3D.img_width*mriData3D.img_height
					+FloatToInt(fy)*mriData3D.img_width
					+FloatToInt(fx);

				eig_imag = m_eigenImag[index];
				eig_real = m_eigenReal[index];

				if(eig_imag > eig_real )
				// && LmsVelocityModulus(m_lmsAverageV)>800.0f
				{
					float apbd;

					apbd = LmsComputeApbDistance3D(matrix_a, 
						fx-((float)x), fy-((float)y), fz-((float)z));

					if(apbd>=0.0f && apbd <= m_lmsRegionSize)
						m_lmsPositions[index]++;

				}
			}
		}
		
		free_matrix(nrVectorB,1,3,1,1);
		free_matrix(nrMatrixAinv,1,3,1,3);

	}
	else
	{
		int xc, yc, index;
		float reg_size = pow(m_lmsRegionSize*2+1, 2.0);

		// compute average velocity
		m_lmsAverageV[0] = m_lmsAverageV[1] = m_lmsAverageV[2] = 0.0f;
		for(xc=x-m_lmsRegionSize; xc <= x+m_lmsRegionSize; xc++)
			for(yc=y-m_lmsRegionSize; yc <= y+m_lmsRegionSize; yc++)
				{
					index = yc*mriData3D.img_width+xc;
					m_lmsAverageV[0] = m_lmsAverageV[0] + ((float)mriData3D.u[index]);
					m_lmsAverageV[1] = m_lmsAverageV[1] + ((float)mriData3D.v[index]);
				}

		m_lmsAverageV[0] = m_lmsAverageV[0]/reg_size;
		m_lmsAverageV[1] = m_lmsAverageV[1]/reg_size;

		// compute W, V
		int i,j;
		float w_matrix[2][2], v_matrix[2][2];
		for(i=0; i<2; i++)
			for(j=0; j<2; j++)
				w_matrix[i][j]=v_matrix[i][j]=0.0f;

		float tmp_w;
		for(xc=x-m_lmsRegionSize; xc <= x+m_lmsRegionSize; xc++)
			for(yc=y-m_lmsRegionSize; yc <= y+m_lmsRegionSize; yc++)
				for(i=0; i<2; i++)
					for(j=0; j<2; j++)
						{
							index = yc*mriData3D.img_width+xc;

							tmp_w = 
								((float)(i==0?xc-x:i==1?yc-y:ErrorVel()))*
								(j==0?((float)mriData3D.u[index])-m_lmsAverageV[0]:
								 j==1?((float)mriData3D.v[index])-m_lmsAverageV[1]:ErrorVel());

							w_matrix[i][j]=w_matrix[i][j]+tmp_w;

							v_matrix[i][j]=v_matrix[i][j]+
								((float)(i==0?xc-x:i==1?yc-y:0)*
								(j==0?xc-x:j==1?yc-y:ErrorVel()));
						}

		for(i=0; i<2; i++)
			for(j=0; j<2; j++)
				{
					//if(w_matrix[i][j]>0.001f)
					//	MessageBeep(0);
					w_matrix[i][j]=w_matrix[i][j]/reg_size;
					v_matrix[i][j]=v_matrix[i][j]/reg_size;
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
		inverter.gaussj(nrMatrixVinv, 2, nrVectorB, 1);

		// compute Tr(V^-1)
		float trace_Vinv = nrMatrixVinv[1][1]+nrMatrixVinv[2][2];

		// compute Ahat = W^T V^-1
		float matrix_Ahat[2][2];
		for(i=0; i<2; i++)
			for(j=0; j<2; j++)
				matrix_Ahat[i][j]=
					w_matrix[0][i]*nrMatrixVinv[1][j+1]+
					w_matrix[1][i]*nrMatrixVinv[2][j+1];

		// copy Ahat to A (note it's passed from caller as a numerical recipes matrix)
		// no need to make trace-free in 2D
		for(i=0; i<2; i++)
			for(j=0; j<2; j++)
				matrix_a[i+1][j+1]=matrix_Ahat[i][j];
		
		free_matrix(nrMatrixVinv,1,2,1,2);

		// set position counter
		// compute A^-1
		float** nrMatrixAinv;
		nrMatrixAinv = matrix(1, 2, 1, 2);
		nrVectorB[1][1]=nrVectorB[1][2]=1.0;

		nrMatrixAinv[1][1] = matrix_a[1][1];
		nrMatrixAinv[1][2] = matrix_a[1][2];
		nrMatrixAinv[2][1] = matrix_a[2][1];
		nrMatrixAinv[2][2] = matrix_a[2][2];

		ComputeEigenvalues2D(matrix_a[1][1], matrix_a[1][2],
			matrix_a[2][1], matrix_a[2][2], x, y);

		if(inverter.gaussj(nrMatrixAinv, 2, nrVectorB, 1))
		{

			index = y*mriData3D.img_width+x;
			int fx, fy;	
			fx = ((float)x) - (nrMatrixAinv[1][1]*m_lmsAverageV[0]
							  +nrMatrixAinv[1][2]*m_lmsAverageV[1]);
			
			fy = ((float)y) - (nrMatrixAinv[2][1]*m_lmsAverageV[0]
							  +nrMatrixAinv[2][2]*m_lmsAverageV[1]);


			float eig_imag, eig_real;
			int index2;
			
			if(fx >= 0.0f && fx<mriData3D.img_width &&
			   fy >= 0.0f && fy<mriData3D.img_height )
			   // && LmsPointDistance(fx, fy, 0, x, y, 0) <= m_lmsRegionSize
			{
				index2 = FloatToInt(fy)*mriData3D.img_width+FloatToInt(fx);

				eig_imag = m_eigenImag[index];
				eig_real = m_eigenReal[index];

				if(eig_imag > eig_real)
					m_lmsPositions[index2]++; 
			}
		}

		free_matrix(nrVectorB,1,2,1,1);
		free_matrix(nrMatrixAinv,1,2,1,2);

	}

}
*/

void CDlgFindVortex::OnVortexSavedata() 
{
	mriData3D.SaveDataHeartCFD("c:\\TextonData.geo");
}

float CDlgFindVortex::LmsPointDistance(float xa, float ya, float za, float xb, float yb, float zb)
{
	return sqrt(pow(xa-xb, 2.0)+pow(ya-yb, 2.0)+pow(za-zb, 2.0));
}

float CDlgFindVortex::LmsComputeApbDistance3D(float **matrix_a, float x0, float y0, float z0)
{
	matrix3d A(matrix_a[1][1], matrix_a[1][2], matrix_a[1][3],
		matrix_a[2][1], matrix_a[2][2], matrix_a[2][3], 
		matrix_a[3][1], matrix_a[3][2], matrix_a[3][3]);

	vector3d dx(x0, y0, z0);

	eig3d eig(A);
	if(eig.complex_eigenvalues())
	{
		double lambda = eig.real_eigenvalue();
		vector3d w(A.eigenvector(lambda));
		double t = dot(dx, w);
		dx -= w.mult(t);
		return dx.norm();
	}
	else
		return -1.0f;

}

float CDlgFindVortex::LmsVelocityModulus(float *vel)
{
	return sqrt(pow(vel[0], 2.0f)+pow(vel[1], 2.0f)+pow(vel[2], 2.0f));
}

void CDlgFindVortex::OnMoreVtx() 
{
	CDlgMoreVtx* dlgMore = new CDlgMoreVtx;
	dlgMore->pFindVortex = this;
	dlgMore->img_width = mriData3D.img_width;
	dlgMore->img_height = mriData3D.img_height;
	dlgMore->img_slices = mriData3D.img_slices;
	dlgMore->Create(IDD_VTX_MORE, this);
	dlgMore->ShowWindow(SW_SHOW);	
}


void CDlgFindVortex::OnVortexSaveData3D() 
{
	static int file_idx = 1;
	CString filename;
	filename.Format("vol_%i.bin", file_idx);

	float* dmy = SeedMask();
	float* data_array = (float*)malloc(sizeof(float)*mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices);

	int k;
	for(k=0; k<mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices; k++)
		data_array[k]=dmy[k];


	FILE* vol_file = fopen(filename, "wb");
/*	int x, y, z;
	for(x=0; x<mriData3D.img_width; x++)
		for(y=0; y<mriData3D.img_height; y++)
			for(z=0; z<mriData3D.img_slices; z++)
			{
				buf = data_array[mriData3D.img_width*mriData3D.img_height*z+mriData3D.img_width*y+x];
				fwrite(&buf, sizeof(float), 1, vol_file);
			}
*/

	fwrite(data_array,  sizeof(float)* 
		mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices, 1, vol_file);

	delete [] dmy;
	free(data_array);

	fclose(vol_file);
	
}

float* CDlgFindVortex::GetVorticityMagn()
{

	if(m_vorticityPlotX == NULL || m_vorticityPlotY == NULL || m_vorticityPlotZ == NULL )
		return NULL;
	
	// compute vorticity modulus
	float* dmy2 = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];
	int x, y, z;
	for(x=0; x<mriData3D.img_width;x++)
		for(y=0; y<mriData3D.img_height; y++)
			for(z=0; z<mriData3D.img_slices; z++)
				dmy2[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
				= sqrt(
					(mriData3D.img_slices > 1?
					(pow(m_vorticityPlotX[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x], 2.0)
					+pow(m_vorticityPlotY[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x], 2.0))
					: 0.0f)
					+pow(m_vorticityPlotZ[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x], 2.0)
					  );

	return dmy2;

}

float* CDlgFindVortex::GetVelocityMagn()
{
	float* dmy = new float[mriData3D.img_width*mriData3D.img_height*mriData3D.img_slices];

	int x, y, z;
	for(x=0; x<mriData3D.img_width;x++)
		for(y=0; y<mriData3D.img_height; y++)
			for(z=0; z<mriData3D.img_slices; z++)
				dmy[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x]
				= m_velMagn[z*mriData3D.img_width*mriData3D.img_height+y*mriData3D.img_width+x];

	return dmy;

}

void CDlgFindVortex::OnFindvortexDicom() 
{
	CDlgVolumeDicom* dlgDicom = new CDlgVolumeDicom;
	dlgDicom->pFindVortex = this;
	dlgDicom->img_width = mriData3D.img_width;
	dlgDicom->img_height = mriData3D.img_height;
	dlgDicom->img_slices = mriData3D.img_slices;
	dlgDicom->Create(IDD_FINDVORTEX_DICOM, this);
	dlgDicom->ShowWindow(SW_SHOW);

}

