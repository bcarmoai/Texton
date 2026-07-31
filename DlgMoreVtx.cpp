// DlgMoreVtx.cpp : implementation file
//

#include "stdafx.h"
#include "texton.h"
#include "DlgMoreVtx.h"
#include "DlgFindVortex.h"
#include "Vector3D.h"
#include "FlowCluster.h"
#include "DlgVolView.h"
#include <math.h>
#include <fstream.h>
#include "NumrecEigenvalues.h"
#include "DlgStreamlines.h"
#include "TextonView.h"
#include "VtkUtilities.h"
#include "IntArray.h"

#ifndef ANSI
#define ANSI
#endif
#include "nrutil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgMoreVtx dialog


CDlgMoreVtx::CDlgMoreVtx(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMoreVtx::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgMoreVtx)
	m_RegionCompression = 90.0f;
	m_RegionClusters = 10;
	m_loadedTxt = _T("");
	m_sStrmCountTxt = _T("");
	m_xLengthThres = 0;
	m_scorrTxt = _T("");
	m_nCluSizeThres = 0;
	m_nCluStreamTxt = _T("");
	m_minCurlThres = 0.0f;
	m_magnStrmThres = 0;
	m_sInfoTxt = _T("");
	m_sInfoRanges = _T("");
	//}}AFX_DATA_INIT

	m_pFlowCluster = NULL;
	m_nCluster = 0;
	m_clusterMap = NULL;
	m_CrPts = NULL;
	m_compressRatio = 90.0f;
	m_pFlowAbstraction = NULL;
	iClustersCompression = 1;
	m_fitVec = NULL;
	m_StrmPointsArray = NULL;
	m_nSelectedStrmCount = 0;
	m_SelectedStrmPointsArray = NULL;
	m_magCurlSelectedArray = NULL;
	m_nMagCurlSelectedCount = 0;

}


void CDlgMoreVtx::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMoreVtx)
	DDX_Text(pDX, IDC_REGIONS_COMPRESSION, m_RegionCompression);
	DDX_Text(pDX, IDC_REGIONS_CLUSTERS, m_RegionClusters);
	DDX_Text(pDX, IDC_VTX_MORE_LOADED, m_loadedTxt);
	DDX_Text(pDX, IDC_REGIONS_STRM_STATIC, m_sStrmCountTxt);
	DDX_Text(pDX, IDC_REGIONS_XLENGTH_THRES, m_xLengthThres);
	DDX_Text(pDX, IDC_REGIONS_XCORR_STATIC, m_scorrTxt);
	DDX_Text(pDX, IDC_REGIONS_SIZE_THRES, m_nCluSizeThres);
	DDX_Text(pDX, IDC_REGIONS_XCORR_STATIC2, m_nCluStreamTxt);
	DDX_Text(pDX, IDC_REGIONS_MINCURL, m_minCurlThres);
	DDX_Text(pDX, IDC_REGIONS_MINMAGN, m_magnStrmThres);
	DDX_Text(pDX, IDC_REGIONS_INFO_RESULT, m_sInfoTxt);
	DDX_Text(pDX, IDC_REGIONS_INFO_RANGES, m_sInfoRanges);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMoreVtx, CDialog)
	//{{AFX_MSG_MAP(CDlgMoreVtx)
	ON_BN_CLICKED(IDC_REGIONS_COMPRESSION_SELECT, OnRegionSelectCompression)
	ON_BN_CLICKED(IDC_REGIONS_CLUSTERS_SELECT, OnRegionSelectClusters)
	ON_BN_CLICKED(IDC_REGIONS_GO, OnRegionGo)
	ON_BN_CLICKED(IDC_REGION_SAVE_ARROWS, OnRegionSaveArrows)
	ON_BN_CLICKED(IDC_REGIONS_SAVE_POINTS, OnRegionSavePoints)
	ON_BN_CLICKED(IDC_REGION_VIEW_CLUSTERS, OnRegionViewClusters)
	ON_BN_CLICKED(IDC_VTXMORE_VIEWBIN, OnViewBin)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_REGIONS_SAVE_VELOCITY, OnRegionsSaveVelocity)
	ON_BN_CLICKED(IDC_REGION_SAVE_CLUSTERS, OnRegionSaveClusters)
	ON_BN_CLICKED(IDC_REGIONS_LOAD_CLUSTERS, OnRegionsLoadClusters)
	ON_BN_CLICKED(IDC_REGIONS_SAVE_LMSDIST, OnRegionsSeedDistances)
	ON_BN_CLICKED(IDC_REGIONS_SAVE_MATLAB_VEL, OnSaveMatlabVelocity)
	ON_BN_CLICKED(IDC_REGIONS_SAVE_MATLAB_ORIGINAL, OnSaveMatlabOriginal)
	ON_BN_CLICKED(IDC_REGIONS_STREAMLINES, OnStreamlinesGo)
	ON_BN_CLICKED(IDC_REGION_RENDER_ALL, OnRegionStrmRenderAll)
	ON_BN_CLICKED(IDC_REGIONS_XCORR_GO, OnRegionsXCorrGo)
	ON_BN_CLICKED(IDC_REGION_RENDER_XCORR, OnRegionRenderXcorr)
	ON_BN_CLICKED(IDC_REGION_RENDER_CURL, OnRegionRenderMinCurl)
	ON_BN_CLICKED(IDC_REGION_RENDER_MAG, OnRegionRenderMinMag)
	ON_BN_CLICKED(IDC_REGION_RENDER_MAGN_CURL, OnRegionRenderMagnCurl)
	ON_BN_CLICKED(IDC_REGIONS_MATLAB_SAVE_XCORR, OnSaveMatlabXcorr)
	ON_BN_CLICKED(IDC_REGION_SAVE_VTK_XCORR_VEL, OnRegionSaveVtkXcorrVel)
	ON_BN_CLICKED(IDC_REGIONS_CLUSTER_INFORMATION, OnRegionsClusterInformation)
	ON_BN_CLICKED(IDC_REGIONS_INFORMATION_BATCH_GO, OnRegionsInformationBatchGo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgMoreVtx message handlers

void CDlgMoreVtx::OnRegionSelectCompression() 
{
	iClustersCompression = 2;
	
}

void CDlgMoreVtx::OnRegionSelectClusters() 
{
	iClustersCompression = 1;
	
}

void CDlgMoreVtx::OnRegionGo() 
{
	img_width = ((CDlgFindVortex*)pFindVortex)->mriData3D.img_width;
	img_height = ((CDlgFindVortex*)pFindVortex)->mriData3D.img_height;
	img_slices = ((CDlgFindVortex*)pFindVortex)->mriData3D.img_slices;

	CVector3D *m_pVec;
	/*	BSC 14/11/03
	float *m_pVecMag;
	float *m_pVecAng;
	
	CVector3D *m_pOriVec;
	float *m_pOriVecMag;
	float *m_pOriVecAng;
	*/	

	m_compressRatio = m_RegionCompression;

	CData3DMRI* mriData3D = &((CDlgFindVortex*)pFindVortex)->mriData3D;
	
	CVector3D *pVec = new CVector3D[img_width * img_height * img_slices];

	int x, y, z, index;
	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
			for(z=0; z<img_slices; z++)
			{
				index = z*img_width*img_height+y*img_width+x;
				pVec[index].x = mriData3D->u[index];
				pVec[index].y = mriData3D->v[index];
				pVec[index].z = mriData3D->w[index];
				
			}

	// initialise vectors
	int vecSize = img_width * img_height * img_slices;
	
	/* BSC 14/11/03	
	m_pOriVec = new CVector3D[vecSize];
	m_pOriVecMag = new float[vecSize];
	m_pOriVecAng = new float[vecSize];
	*/
	m_pVec = new CVector3D[vecSize];

	/*	BSC 14/11/03
	m_pVecMag = new float[vecSize];
	m_pVecAng = new float[vecSize];
	*/
	m_clusterMap = new int[vecSize];

	/* BSC 14/11/03
	int		i;
	for ( i = 0; i < vecSize; i++){
		m_pOriVec[i] = pVec[i];
		m_pOriVecMag[i] = (float) pVec[i].Norm();
		m_pOriVecAng[i] = (float) pVec[i].Angle();
	}
	*/
	
	if(m_pFlowAbstraction != NULL)
		delete m_pFlowAbstraction;
	m_pFlowAbstraction = new FlowAbstraction;

	if ( !m_pFlowAbstraction->Initialise(pVec, img_width, img_height, img_slices)) 
	{
		AfxMessageBox("FlowAbstraction initialisation failed");
	}

	bool abstr_result = m_pFlowAbstraction->StartClustering();

	if (abstr_result) 
	{
		UpdateData(TRUE);
		m_compressRatio = m_RegionCompression;
		m_nCluster = (int) ((1 - m_compressRatio/100.0) * vecSize + 0.5);
		
		m_RegionClusters = m_nCluster;
		if(m_pFlowCluster!=NULL)
			delete [] m_pFlowCluster;
		m_pFlowCluster = new FlowCluster[m_nCluster];
		// comment for MemSave 27/1/04
		m_nCluster = m_pFlowAbstraction->RetrieveClusters(m_compressRatio, m_pFlowCluster, 		
			m_nCluster, m_clusterMap, m_pVec);
		m_RegionClusters = m_nCluster;
		
		/* BSC 14/11/03
		for ( i = 0; i < vecSize; i++) 
		{
			m_pVecMag[i] = (float) m_pVec[i].Norm();
			m_pVecAng[i] = (float) m_pVec[i].Angle();
		}
		*/

	//	DetectCriticalPoints();

		UpdateData(FALSE);
		
	}
	else 
		AfxMessageBox("ERROR :  Failed to cluster the flow field.");

	delete [] pVec;
	delete [] m_pVec;
	/* BSC 14/11/03
	delete [] m_pOriVec;
	delete [] m_pOriVecMag;
	delete [] m_pOriVecAng;
	delete [] m_pVecMag;
	delete [] m_pVecAng;
	*/

	m_loadedTxt.Format("%i nodes, image: %i voxels (%ix%ix%i)", 
		m_pFlowAbstraction->GetClusterCount(), img_width*img_height*img_slices,
		img_width, img_height, img_slices);

	UpdateData(FALSE);

	((CButton*)GetDlgItem(IDC_REGIONS_SAVE_POINTS))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_REGION_VIEW_CLUSTERS))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_REGIONS_SAVE_VELOCITY))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_REGIONS_LOAD_CLUSTERS))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_REGION_SAVE_CLUSTERS))->EnableWindow(TRUE);

	
}

void CDlgMoreVtx::OnRegionSaveArrows() 
{
	// TODO: Add your control notification handler code here
	
}

void CDlgMoreVtx::OnRegionSavePoints() 
{
	UpdateClusters();
	DetectCriticalPoints();

	int* crPoints = new int[img_width*img_height*img_slices];
	int i, x, y, z;

	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
			for(z=0; z<img_slices; z++)
				crPoints[z*img_width*img_height+y*img_width+x]=0;

	for(i=0; i<m_nCrPts; i++)
	{
		if(	m_CrPts[i].x >= 0 && m_CrPts[i].x < img_width &&
			m_CrPts[i].y >= 0 && m_CrPts[i].y < img_height &&
			m_CrPts[i].z >= 0 && m_CrPts[i].z < img_slices )
		{
			crPoints[m_CrPts[i].z*img_width*img_height+m_CrPts[i].y*img_width+m_CrPts[i].x]=1;
		}
	}

	((CDlgFindVortex*)pFindVortex)->m_RegionCriticalPoints = crPoints;
}

void CDlgMoreVtx::OnRegionViewClusters() 
{
	UpdateClusters();

	CString s_imgSizesTxt;
	s_imgSizesTxt.Format("%ix%ix%i", img_width, img_height, img_slices);

	// display cluster colours
	CDlgVolView* pPosDlg = new CDlgVolView;
	pPosDlg->pView = ((CDlgFindVortex*)pFindVortex)->pView;
	pPosDlg->m_sVolumeTitle.Format("clusters");
	pPosDlg->InitialiseVolume(m_clusterMap, img_width, img_height, img_slices);
	pPosDlg->m_imgInfo = s_imgSizesTxt;
	pPosDlg->Create(IDD_VOL_VIEW, this);
	pPosDlg->ShowWindow(SW_SHOW);
	
}

void CDlgMoreVtx::ComputeEigenvalues(BOOL dim3D, float** matrix_a, float& real, float& imag)
{

	if(dim3D == TRUE)
	{


		float* nrEigenReal = vector(1, 3);
		float* nrEigenImag = vector(1, 3);

		CNumrecEigenvalues nrEigen;
		nrEigen.ComputeEigenvalues(matrix_a, nrEigenReal, nrEigenImag, 3);

		real = imag = 0.0f;

		int i;
		for(i=0; i<3 ; i++)
		{
			imag += fabs(nrEigenImag[i+1]);
			real += fabs(nrEigenReal[i+1]);
		}
		
		free_vector(nrEigenReal,1,3);
		free_vector(nrEigenImag,1,3);
	}
	else
	{

		float* nrEigenReal = vector(1, 2);
		float* nrEigenImag = vector(1, 2);

		CNumrecEigenvalues nrEigen;
		nrEigen.ComputeEigenvalues(matrix_a, nrEigenReal, nrEigenImag, 2);

		real = imag = 0.0f;

		int i;
		for(i=0; i<2 ; i++)
		{
			imag += fabs(nrEigenImag[i+1]);
			real += fabs(nrEigenReal[i+1]);
		}

		free_vector(nrEigenReal,1,2);
		free_vector(nrEigenImag,1,2);

	}

}


void CDlgMoreVtx::DetectCriticalPoints()
{

	if ( m_CrPts != NULL)
	{
		delete [] m_CrPts;
		m_CrPts = NULL;
		m_nCrPts = 0;
	}

	int i;
	// m_boundRect = CRect(0, 0, img_width, img_height);
	// BSC 14/11/03

	if ( m_pFlowCluster == NULL)
		return;

	const	int	minClusterSize = 0;  
	int		    clusterSize;
	// CRect			boundRect;
	// BSC 14/11/03
//	double		a1, a2, b1, b2, c1, c2, d1, d2;
//	double		det, det1, det2;
//	double		z1, z2;
	CPlane		p1, p2;
	CPoint3D	aPt;
//	double		a11, a12, a21, a22;
//	double		delta, alpha, beta;

	bool			boolPtInCluster = false;

	m_CrPts = new CPoint3D[m_nCluster];
	m_nCrPts = 0;

	CPlane* fitPlane;
	float eig_real, eig_imag;
	float** matrix_a;

	// extract CrPt from each cluster
	for ( i = 0; i < m_nCluster; i++)
	{
		
		clusterSize = m_pFlowCluster[i].GetNPts();		
		
		if ( clusterSize >= minClusterSize)
		{ 
			
			// get plane data
			fitPlane = m_pFlowCluster[i].GetFitPlanes();
			matrix_a = fitPlane->GetMatrixA();
			
			// get eigenvalues from plane data
			float** temp_ma;
			if(((CDlgFindVortex*)pFindVortex)->mriData3D.img_slices == 1)
			{
				temp_ma = matrix(1, 2, 1, 2);
				temp_ma[1][1]=matrix_a[1][1];
				temp_ma[1][2]=matrix_a[1][2];
				temp_ma[2][1]=matrix_a[2][1];
				temp_ma[2][2]=matrix_a[2][2];
	
			}

			if(((CDlgFindVortex*)pFindVortex)->mriData3D.img_slices > 1)
				ComputeEigenvalues(TRUE, matrix_a, eig_real, eig_imag);
			else
				ComputeEigenvalues(FALSE, temp_ma, eig_real, eig_imag);

			// if imaginary part is non-zero, extract critical point
			if(eig_imag > 0)
			{
				
				// critical point is at zero velocity
				if(((CDlgFindVortex*)pFindVortex)->mriData3D.img_slices > 1)
					aPt = ComputeVortexCentre(TRUE, matrix_a, fitPlane->GetAverageX(), 
					fitPlane->GetAverageV());
				else
					aPt = ComputeVortexCentre(FALSE, temp_ma, fitPlane->GetAverageX(), 
					fitPlane->GetAverageV());

				if(m_nCrPts < m_nCluster)
					m_CrPts[m_nCrPts] = aPt;		
				else
					{
						AfxMessageBox("Error in DetectCriticalPoints(): m_nCrPts >= m_nCluster.");
						break;
					}

				m_nCrPts++;
			}

			if(((CDlgFindVortex*)pFindVortex)->mriData3D.img_slices == 1)
			{
				free_matrix(temp_ma, 1, 2, 1, 2);
				free_matrix(matrix_a, 1, 2, 1, 2);
			}
			else
				free_matrix(matrix_a, 1, 3, 1, 3);

		}
	}
}

void CDlgMoreVtx::UpdateClusters()
{
	if ( m_pFlowCluster != NULL)
	{
		delete [] m_pFlowCluster;
		m_pFlowCluster = NULL;
		m_nCluster = 0;
	}

	if ( m_fitVec != NULL)
	{
		delete [] m_fitVec;
		m_fitVec = NULL;
	}

	UpdateData(TRUE);

	int vecSize = img_width*img_height*img_slices;

	if (iClustersCompression == 1) 
	{
		if ( m_RegionClusters < 1 || m_RegionClusters > ( vecSize - 1))
		{
			CString mesg;
			mesg.Format("No. of Clusters must be between 1 and %d", vecSize); 
			AfxMessageBox(mesg);
			m_RegionClusters = m_nCluster;
			m_RegionCompression = m_compressRatio;
			return;
		}
		else
		{
			m_RegionCompression = (1.0f - (float)m_RegionClusters / vecSize) * 100;
		}
	} 
	else 
		if (iClustersCompression == 2) 
		{ 
			if ( m_RegionCompression >= 100 || m_RegionCompression <= 0) 
			{
				AfxMessageBox("Invalid compression ratio.  Please re-enter.");
				m_RegionCompression = m_compressRatio;
				m_RegionClusters = m_nCluster;
				return;
			}
			else 
			{
				m_RegionClusters = (int) ((100 - m_RegionCompression) / 100 * vecSize + 0.5);
				if (m_RegionClusters == 0)
					m_RegionClusters = 1;
			}
		}

	m_nCluster = m_RegionClusters;
	m_compressRatio = m_RegionCompression;

	if ( m_pFlowAbstraction != NULL)
	{
		m_fitVec = new CVector3D[img_width * img_height * img_slices];
		m_pFlowCluster	= new FlowCluster[m_nCluster];
		// comment for MemSave 27/1/04
		m_nCluster = m_pFlowAbstraction->RetrieveClusters(m_compressRatio, m_pFlowCluster, m_nCluster, 
			m_clusterMap, m_fitVec);
	}

	UpdateData(FALSE);
				
}

void CDlgMoreVtx::OnCancel() 
{
	if(m_clusterMap != NULL)
		delete [] m_clusterMap;
	if(m_pFlowCluster != NULL)
		delete [] m_pFlowCluster;
	if(m_CrPts != NULL)
		delete [] m_CrPts;
	if(m_pFlowAbstraction != NULL)
		delete [] m_pFlowAbstraction;
	if(m_StrmPointsArray != NULL)
		delete [] m_StrmPointsArray;
	
	CDialog::OnCancel();
}

void CDlgMoreVtx::OnViewBin() 
{
	img_width = ((CDlgFindVortex*)pFindVortex)->mriData3D.img_width;
	img_height = ((CDlgFindVortex*)pFindVortex)->mriData3D.img_height;
	img_slices = ((CDlgFindVortex*)pFindVortex)->mriData3D.img_slices;

	CString filename;
	CFileDialog* dDlg = new CFileDialog(FALSE,"*","vol",OFN_CREATEPROMPT |
		OFN_HIDEREADONLY | OFN_NOREADONLYRETURN,
		_T("Volume data (*.bin)|*.bin||"), NULL);

	if(dDlg->DoModal()!=IDOK)
		return;

	filename = dDlg->GetPathName();
	delete dDlg;

	// load data into dmy
	float* dmy = (float*) malloc(sizeof(float)*img_width*img_height*img_slices);

	FILE* vol_file = fopen(filename, "rb");
/*
	int x, y, z;
	float buf;
	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
			for(z=0; z<img_slices; z++)
			{
				fread(&buf, sizeof(float), 1, vol_file);
				dmy[img_width*img_height*z+img_width*y+x]=buf;
			}
*/

	fread(dmy, sizeof(float)*img_width*img_height*img_slices, 1, vol_file);

	// view dmy data
	CString s_imgSizesTxt;
	s_imgSizesTxt.Format("%ix%ix%i", img_width, img_height, img_slices);

	// display dmy
	CDlgVolView* pPosDlg = new CDlgVolView;
	pPosDlg->pView = ((CDlgFindVortex*)pFindVortex)->pView;
	pPosDlg->m_sVolumeTitle.Format("loaded volume data");
	pPosDlg->InitialiseVolume(dmy, img_width, img_height, img_slices);
	pPosDlg->m_imgInfo = s_imgSizesTxt;
	pPosDlg->Create(IDD_VOL_VIEW, this);
	pPosDlg->ShowWindow(SW_SHOW);

	free(dmy);

	fclose(vol_file);
	
}

BOOL CDlgMoreVtx::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	((CButton*)GetDlgItem(IDC_REGIONS_CLUSTERS_SELECT))->SetCheck(BST_CHECKED);
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CDlgMoreVtx::FloatToInt(float fnum)
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

CPoint3D CDlgMoreVtx::ComputeVortexCentre(BOOL b3D, float **matrix_a, 
										  CPoint3D average_x, CVector3D average_v)
{

	float** nrMatrixAinv;
	float** nrVectorB;

	if(b3D)
	{
		nrMatrixAinv = matrix(1, 3, 1, 3);
		nrVectorB = matrix(1,3,1,1);

		nrVectorB[1][1]=nrVectorB[1][2]=nrVectorB[1][3]=1.0;

		nrMatrixAinv[1][1] = matrix_a[1][1];
		nrMatrixAinv[1][2] = matrix_a[1][2];
		nrMatrixAinv[1][3] = matrix_a[1][3];
		nrMatrixAinv[2][1] = matrix_a[2][1];
		nrMatrixAinv[2][2] = matrix_a[2][2];
		nrMatrixAinv[2][3] = matrix_a[2][3];
		nrMatrixAinv[3][1] = matrix_a[3][1];
		nrMatrixAinv[3][2] = matrix_a[3][2];
		nrMatrixAinv[3][3] = matrix_a[3][3];

		int fx, fy, fz;	
		fx=fy=fz=0;

		CNumrecEigenvalues inverter;
		if(inverter.gaussj(nrMatrixAinv, 3, nrVectorB, 1))
		{

			fx = FloatToInt((float)average_x.x - (nrMatrixAinv[1][1]*(float)average_v.x
				 			 +nrMatrixAinv[1][2]*(float)average_v.y
				 			 +nrMatrixAinv[1][3]*(float)average_v.z));
			
			fy = FloatToInt((float)average_x.y - (nrMatrixAinv[2][1]*(float)average_v.x
				 			 +nrMatrixAinv[2][2]*(float)average_v.y
				 			 +nrMatrixAinv[2][3]*(float)average_v.z));

			fz = FloatToInt((float)average_x.z - (nrMatrixAinv[3][1]*(float)average_v.x
							  +nrMatrixAinv[3][2]*(float)average_v.y
							  +nrMatrixAinv[3][3]*(float)average_v.z));
		}
		else
		{
			// AfxMessageBox("Warning: bad matrix inversion in CDlgMoreVtx::ComputeVortexCentre()");
			fx = fy = fz = -1;
		}


		free_matrix(nrVectorB,1,3,1,1);
		free_matrix(nrMatrixAinv,1,3,1,3);

		return CPoint3D(fx, fy, fz);
	}
	else
	{
		nrMatrixAinv = matrix(1, 2, 1, 2);
		nrVectorB = matrix(1,2,1,1);

		nrVectorB[1][1]=nrVectorB[1][2]=1.0;

		nrMatrixAinv[1][1] = matrix_a[1][1];
		nrMatrixAinv[1][2] = matrix_a[1][2];
		nrMatrixAinv[2][1] = matrix_a[2][1];
		nrMatrixAinv[2][2] = matrix_a[2][2];

		int fx, fy;	
		fx=fy=0;

		CNumrecEigenvalues inverter;
		if(inverter.gaussj(nrMatrixAinv, 2, nrVectorB, 1))
		{

			fx = FloatToInt((float)average_x.x - ( nrMatrixAinv[1][1]*(float)average_v.x
				 + nrMatrixAinv[1][2]*average_v.y));
			
			fy = FloatToInt((float)average_x.y - ( nrMatrixAinv[2][1]*(float)average_v.x
				 + nrMatrixAinv[2][2]*(float)average_v.y));

		}
		else
		{
			// AfxMessageBox("Warning: bad matrix inversion in CDlgMoreVtx::ComputeVortexCentre()");
			fx = fy =-1;
		}

		free_matrix(nrVectorB,1,2,1,1);
		free_matrix(nrMatrixAinv,1,2,1,2);

		return CPoint3D(fx, fy, 0);

	}


}

void CDlgMoreVtx::OnButton1() 
{

//	for(int i = 0; i<((CDlgFindVortex*)pFindVortex)->mriData3D.img_height; i++)
		((CDlgFindVortex*)pFindVortex)->mriData3D.SaveDataSur(10, 12);	
}

void CDlgMoreVtx::OnRegionsSaveVelocity() 
{

	// fill a CData3DMRI with clusters' velocity data
	// prompt user for filename
	// save data to file

	CData3DMRI dataToSave;
	dataToSave.img_width = ((CDlgFindVortex*)pFindVortex)->mriData3D.img_width;
	dataToSave.img_height = ((CDlgFindVortex*)pFindVortex)->mriData3D.img_height;
	dataToSave.img_slices = ((CDlgFindVortex*)pFindVortex)->mriData3D.img_slices;

	dataToSave.mr = new short[dataToSave.img_width*dataToSave.img_height*dataToSave.img_slices];
	dataToSave.u = new short[dataToSave.img_width*dataToSave.img_height*dataToSave.img_slices];
	dataToSave.v = new short[dataToSave.img_width*dataToSave.img_height*dataToSave.img_slices];
	dataToSave.w = new short[dataToSave.img_width*dataToSave.img_height*dataToSave.img_slices];

	GetClustersVelocity(dataToSave);

	CFileDialog* dDlg = new CFileDialog(FALSE,"*.geo","MriData",OFN_CREATEPROMPT |
		OFN_HIDEREADONLY | OFN_NOREADONLYRETURN,
		_T("MRI Fudgebox Data (*.geo)|*.geo|All Files (*.*)|*.*||"), NULL);

	if(dDlg->DoModal()!=IDOK)
		return;

	dataToSave.SaveDataHeartCFD(dDlg->GetPathName());
	delete dDlg;


}

void CDlgMoreVtx::GetClustersVelocity(CData3DMRI &data3D)
{
	UpdateClusters();

	int x, y, z, index;

	for(x=0; x<data3D.img_width; x++)
		for(y=0; y<data3D.img_height; y++)
			for(z=0; z<data3D.img_slices; z++)
			{
				index = img_width*img_height*z + img_width*y + x;
				data3D.mr[index] = FloatToInt(m_fitVec[index].x);
				data3D.u[index] = FloatToInt(m_fitVec[index].x);
				data3D.v[index] = FloatToInt(m_fitVec[index].y);
				data3D.w[index] = FloatToInt(m_fitVec[index].z);
			}

}

CDlgMoreVtx::~CDlgMoreVtx()
{
	if(m_pFlowCluster!=NULL)
		delete m_pFlowCluster;

	if(m_SelectedStrmPointsArray!=NULL)
		delete m_SelectedStrmPointsArray;

	if(m_magCurlSelectedArray!=NULL)
		delete m_magCurlSelectedArray;

}

void CDlgMoreVtx::OnRegionSaveClusters() 
{
	CString filename;
	CFileDialog* dDlg = new CFileDialog(TRUE,"*","vol",OFN_CREATEPROMPT |
		OFN_HIDEREADONLY | OFN_NOREADONLYRETURN,
		_T("Cluster data (*.clu)|*.clu||"), NULL);

	if(dDlg->DoModal()!=IDOK)
		return;

	filename = dDlg->GetPathName();
	delete dDlg;

	if(m_pFlowAbstraction != NULL)
		m_pFlowAbstraction->SaveClusterData(filename);
	
}

void CDlgMoreVtx::OnRegionsLoadClusters() 
{

	CString filename;
	CFileDialog* dDlg = new CFileDialog(FALSE,"*","vol",OFN_CREATEPROMPT |
		OFN_HIDEREADONLY | OFN_NOREADONLYRETURN,
		_T("Cluster data (*.clu)|*.clu||"), NULL);

	if(dDlg->DoModal()!=IDOK)
		return;

	filename = dDlg->GetPathName();
	delete dDlg;
	
	if(m_pFlowAbstraction != NULL)
		delete m_pFlowAbstraction;

	m_pFlowAbstraction = new FlowAbstraction;

	m_pFlowAbstraction->LoadClusterData(filename);
	m_pFlowAbstraction->GetVecSizes(img_width, img_height, img_slices);
	m_clusterMap = new int[img_width*img_height*img_slices];
	
	((CDlgFindVortex*)pFindVortex)->mriData3D.img_width = img_width;
	((CDlgFindVortex*)pFindVortex)->mriData3D.img_height = img_height;
	((CDlgFindVortex*)pFindVortex)->mriData3D.img_slices = img_slices;

	m_loadedTxt.Format("%i nodes, image: %i voxels (%ix%ix%i)", 
		m_pFlowAbstraction->GetClusterCount(), img_width*img_height*img_slices,
		img_width, img_height, img_slices);

	UpdateData(FALSE);

	((CButton*)GetDlgItem(IDC_REGIONS_SAVE_POINTS))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_REGION_VIEW_CLUSTERS))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_REGIONS_SAVE_VELOCITY))->EnableWindow(TRUE);
	
}

void CDlgMoreVtx::OnRegionsSeedDistances() 
{
/*	UpdateClusters();

	float* dists = new float[img_width*img_height*img_slices];
	int x, y, z;
	float x0, y0, z0;
	FlowCluster* pVoxelCluster;

	CDlgFindVortex dlgVortex;
	
	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
			for(z=0; z<img_slices; z++)
			{
				// retrieve cluster index at each voxel
				if(m_clusterMap[z*img_width*img_height+y*img_width+x]!=-1)
				{
					pVoxelCluster = &m_pFlowCluster[m_clusterMap[z*img_width*img_height+y*img_width+x]];

					// compute distance from voxel cluster's info
					dists[z*img_width*img_height+y*img_width+x]=-1.0*
						dlgVortex.LmsComputeApbDistance3D(
						pVoxelCluster->GetFitPlanes()->GetMatrixA(), 
						pVoxelCluster->GeoCentroid().x, 
						pVoxelCluster->GeoCentroid().y, 
						pVoxelCluster->GeoCentroid().z);
				}
				else
					dists[z*img_width*img_height+y*img_width+x]=-100;
			}
	((CDlgFindVortex*)pFindVortex)->m_lmsDistances = dists;
	*/
/*
	float* dists = new float[img_width*img_height*img_slices];

	int x, y, z;
	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
			for(z=0; z<img_slices; z++)
				dists[z*img_width*img_height+y*img_width+x]=0;

	x=2;
	y=1;
	z=2;
	dists[z*img_width*img_height+y*img_width+x]=1;
	x=0+1;
	y=1+1;
	dists[z*img_width*img_height+y*img_width+x]=1;
	x=0+1;
	y=2+1;
	dists[z*img_width*img_height+y*img_width+x]=1;
	x=0+1;
	y=3+1;
	dists[z*img_width*img_height+y*img_width+x]=1;
	x=1+1;
	y=3+1;
	dists[z*img_width*img_height+y*img_width+x]=1;
	x=2+1;
	y=3+1;
	dists[z*img_width*img_height+y*img_width+x]=1;
	x=3+1;
	y=3+1;
	dists[z*img_width*img_height+y*img_width+x]=1;
	x=3+1;
	y=2+1;
	dists[z*img_width*img_height+y*img_width+x]=1;
	x=3+1;
	y=1+1;
	dists[z*img_width*img_height+y*img_width+x]=1;
	x=4+1;
	y=1+1;
	dists[z*img_width*img_height+y*img_width+x]=1;
	x=5+1;
	y=1+1;
	dists[z*img_width*img_height+y*img_width+x]=1;

	((CDlgFindVortex*)pFindVortex)->m_lmsDistances = dists;
*/

}

void CDlgMoreVtx::OnSaveMatlabVelocity() 
{

	// fill a CData3DMRI with clusters' velocity data
	// prompt user for filename
	// save data to file

	CData3DMRI dataToSave;
	dataToSave.img_width = ((CDlgFindVortex*)pFindVortex)->mriData3D.img_width;
	dataToSave.img_height = ((CDlgFindVortex*)pFindVortex)->mriData3D.img_height;
	dataToSave.img_slices = ((CDlgFindVortex*)pFindVortex)->mriData3D.img_slices;

	dataToSave.mr = new short[dataToSave.img_width*dataToSave.img_height*dataToSave.img_slices];
	dataToSave.u = new short[dataToSave.img_width*dataToSave.img_height*dataToSave.img_slices];
	dataToSave.v = new short[dataToSave.img_width*dataToSave.img_height*dataToSave.img_slices];
	dataToSave.w = new short[dataToSave.img_width*dataToSave.img_height*dataToSave.img_slices];

	GetClustersVelocity(dataToSave);

	CFileDialog* dDlg = new CFileDialog(FALSE,"*.mat","Matlab script",
		OFN_HIDEREADONLY | OFN_NOREADONLYRETURN,
		_T("Matlab script (*.m)|*.m|All Files (*.*)|*.*||"), NULL);

	if(dDlg->DoModal()!=IDOK)
		return;

	dataToSave.SaveDataMatlab(dDlg->GetPathName());
	delete dDlg;

	
}

void CDlgMoreVtx::OnSaveMatlabOriginal() 
{
	// prompt user for filename
	// save data to file

	CFileDialog* dDlg = new CFileDialog(FALSE,"*.mat","Matlab script",
		OFN_HIDEREADONLY | OFN_NOREADONLYRETURN,
		_T("Matlab script (*.m)|*.m|All Files (*.*)|*.*||"), NULL);

	if(dDlg->DoModal()!=IDOK)
		return;

	((CDlgFindVortex*)pFindVortex)->mriData3D.SaveDataMatlab(dDlg->GetPathName());
	delete dDlg;
	
}

void CDlgMoreVtx::OnStreamlinesGo() 
{

	// generate velocity data
	CData3DMRI dataToStream;
	dataToStream.img_width  = img_width;
	dataToStream.img_height = img_height;
	dataToStream.img_slices = img_slices;

	dataToStream.mr = new short[dataToStream.img_width*dataToStream.img_height*dataToStream.img_slices];
	dataToStream.u = new short[dataToStream.img_width*dataToStream.img_height*dataToStream.img_slices];
	dataToStream.v = new short[dataToStream.img_width*dataToStream.img_height*dataToStream.img_slices];
	dataToStream.w = new short[dataToStream.img_width*dataToStream.img_height*dataToStream.img_slices];

	UpdateClusters();
	GetClustersOriginalVelocity(dataToStream);

	// generate seed points and skip null voxels
	int* seeds = new int[img_width*img_height*img_slices];
	int nseeds = 0;
/*
	nseeds = 6;
	seeds[10] = 1;
	seeds[45] = 1;
	seeds[100] = 1;
	seeds[145] = 1;
	seeds[200] = 1;
	seeds[245] = 1;
*/

	int x, y, z;
	nseeds = 0;
	for(x=0; x<img_width; x=x+5)
		for(y=0; y<img_height; y=y+5)
			for(z=0; z<img_slices; z=z+4)
			{
				if(fabs(  dataToStream.u[z*img_width*img_height+y*img_width+x]+
					 dataToStream.v[z*img_width*img_height+y*img_width+x]+
					 dataToStream.w[z*img_width*img_height+y*img_width+x])>0.0)
				{
					seeds[z*img_width*img_height+y*img_width+x]=1;
					nseeds++;
				}
			}


/*	x=12;
	y=13;
	z=11;
	seeds[z*img_width*img_height+y*img_width+x]=1;
	x=14;
	y=11;
	z=10;
	seeds[z*img_width*img_height+y*img_width+x]=1;
	x=10;
	y=13;
	z=11;
	seeds[z*img_width*img_height+y*img_width+x]=1;
	x=12;
	y=11;
	z=13;
	seeds[z*img_width*img_height+y*img_width+x]=1;
	x=14;
	y=16;
	z=13;
	seeds[z*img_width*img_height+y*img_width+x]=1;
	x=18;
	y=13;
	z=14;
	seeds[z*img_width*img_height+y*img_width+x]=1;
*/

	// run streamlines 
	CDlgStreamlines dlgStrm;
	dlgStrm.pView = ((CDlgFindVortex*)pFindVortex)->pView;
	((CTextonView*)((CDlgFindVortex*)pFindVortex)->pView)->dlgStrm = &dlgStrm;
	dlgStrm.NoLoadInitialisePointslist(&dataToStream, seeds, nseeds);
	dlgStrm.DoModal();

	// collect line points
	if(m_StrmPointsArray != NULL)
		delete [] m_StrmPointsArray;
	m_StrmPointsArray = new CPointFloatArray[nseeds];
	m_nStrmCount = dlgStrm.GetPointlist(m_StrmPointsArray);
	UpdateData(TRUE);
	if(m_nStrmCount==1)
		m_sStrmCountTxt.Format("generated 1 streamline");
	else
		m_sStrmCountTxt.Format("generated %i streamlines", m_nStrmCount);
	UpdateData(FALSE);

	delete [] seeds;


}

void CDlgMoreVtx::OnRegionStrmRenderAll() 
{
	if(m_StrmPointsArray==NULL)
		return;

	CVtkUtilities vtkRender;
	vtkRender.RenderStreamlineList(m_StrmPointsArray, m_nStrmCount);
	
}

void CDlgMoreVtx::OnRegionsXCorrGo() 
{

	UpdateData(TRUE);
	m_nCluStreamTxt = "";
	m_scorrTxt = "";
	UpdateData(FALSE);

	if(m_nStrmCount <= 0 || m_nCluster <= 0)
		return;

	if(m_magCurlSelectedArray == NULL || m_nMagCurlSelectedCount <= 0)
	{

		if(m_magCurlSelectedArray!=NULL)
			delete [] m_magCurlSelectedArray;
		m_nMagCurlSelectedCount = m_nStrmCount;
		m_magCurlSelectedArray = m_StrmPointsArray; /*new CPointFloatArray[m_nStrmCount];
	
		for(int iStream=0; iStream<m_nStrmCount; iStream++)
		{
			m_magCurlSelectedArray[iStream].pointArray.Copy(m_StrmPointsArray[iStream].pointArray);
			m_nMagCurlSelectedCount++;
		}*/
	}

	// streamline-cluster array
	CIntArray* strmAllCluArray = new CIntArray[m_nStrmCount]; // for building cluStreamArray
	CIntArray* strmCluArray = new CIntArray[m_nStrmCount]; // for building crossCluster

	// cross-cluster matrix
	int nCrossSize = m_nStrmCount*m_nStrmCount;
	int* crossCluster = new int[nCrossSize];

	// cross-length matrix
	float* crossLength = new float[nCrossSize];

	for(int i=0; i<nCrossSize; i++)
	{
		crossLength[i]=0.0f;
		crossCluster[i]=0;
	}

	// cluster-streamline matrix
	CIntArray* cluStreamArray = new CIntArray[m_nCluster];

	// parse each streamline and fill in clusters array
	int iStream, iPoint, iClusterUsed;

	BOOL* clusterUsed = new BOOL[m_nCluster];
	for(i=0; i<m_nCluster; i++)
		clusterUsed[i]=FALSE;

	int nAddedStream = 0;
	for(iStream = 0; iStream < m_nStrmCount; iStream++)
	{
		for(i=0; i<m_nCluster; i++)
			clusterUsed[i]=FALSE;

		// set used cluster's entry to TRUE
		for(iPoint = 0; iPoint < m_magCurlSelectedArray[iStream].pointArray.GetSize(); iPoint++)
		{
			iClusterUsed = m_clusterMap[
					FloatToInt(m_magCurlSelectedArray[iStream].pointArray[iPoint].z) *img_width*img_height +
					FloatToInt(m_magCurlSelectedArray[iStream].pointArray[iPoint].y) *img_width +
					FloatToInt(m_magCurlSelectedArray[iStream].pointArray[iPoint].x)
 					        ];

			if(iClusterUsed>=0 && iClusterUsed<m_nCluster)
			{
				clusterUsed[iClusterUsed] = TRUE;

				// add starting streamline number to cluster-streamline array
			//	if(iPoint==0)
			//		cluStreamArray[iClusterUsed].arrayData.Add(CInt(iStream));
			}

		}

		// for each TRUE entry, add cluster number to all-streamline-cluster array
		for(i=0; i<m_nCluster; i++)
			if(clusterUsed[i] == TRUE)
			{
				strmAllCluArray[iStream].arrayData.Add(CInt(i));
				// add starting streamline number to cluster-streamline array
				cluStreamArray[i].arrayData.Add(CInt(iStream));

/*				if(m_StrmPointsArray[iStream].pointArray.GetSize() > m_nCluSizeThres)
				{
					strmCluArray[iStream].arrayData.Add(CInt(i));
					nAddedStream++;
				}
*/
			}

	}

	for(iStream = 0; iStream < m_nStrmCount; iStream++)
		if(strmAllCluArray[iStream].arrayData.GetSize() > m_nCluSizeThres)
		{
			strmCluArray[iStream].arrayData.Copy(strmAllCluArray[iStream].arrayData);
			nAddedStream++;

		}


	/*
	// keep the cluster-longest streamlines started in each cluster
	int maxLength = 0, curLength, maxStream = -1, iTestStream;
	for(i=0; i<m_nCluster; i++)
	{
		maxLength = 0;
		maxStream = -1;

		for(iTestStream=0; iTestStream<cluStreamArray[i].arrayData.GetSize(); iTestStream++)
		{
			curLength = strmAllCluArray[cluStreamArray[i].arrayData[iTestStream].n].arrayData.GetSize();
			if(curLength > maxLength)
			{
				maxLength = curLength;
				maxStream = iTestStream;
			}

			// TODO: move this somewhere else
			strmCluArray[iTestStream].arrayData.Copy(strmAllCluArray[iTestStream].arrayData);
			nAddedStream++;
		}
		
		// add cluster-longest streamline to streamline-cluster array
		if(//maxStream>=0
		FALSE)
		{
			// strmCluArray[maxStream].arrayData.Copy(strmAllCluArray[maxStream].arrayData);
			strmCluArray[maxStream].arrayData.Copy(strmAllCluArray[maxStream].arrayData);
			nAddedStream++;
		}
	}
	*/

	UpdateData(TRUE);
	if(nAddedStream==1)
		m_nCluStreamTxt.Format("1 long streamline");
	else
		m_nCluStreamTxt.Format("%i long streamlines", nAddedStream);
	UpdateData(FALSE);

	// set crossCluster entries by searching for common clusters in strmCluArray
	int iCurStream, iSearchStream, iSearchCluster, iCurCluster;
	for(iCurStream=0; iCurStream<m_nStrmCount; iCurStream++)
		for(iCurCluster=0; 
			iCurCluster<strmCluArray[iCurStream].arrayData.GetSize(); 
			iCurCluster++)
				for(iSearchStream=iCurStream+1; iSearchStream<m_nStrmCount; iSearchStream++)
					for(iSearchCluster=0; 
						iSearchCluster<strmCluArray[iSearchStream].arrayData.GetSize(); 
						iSearchCluster++)
						if(strmCluArray[iSearchStream].arrayData[iSearchCluster].n ==
						   strmCluArray[iCurStream].arrayData[iCurCluster].n)							
							crossCluster[iCurStream*m_nStrmCount+iSearchStream]++;

	// set crossLength entries to crossCluster divided by streamline length in #clusters
	int row, col;
	for(row=0; row<m_nStrmCount; row++)
		for(col=0; col<m_nStrmCount; col++)
		{
			if(row==col)
				continue;

			if(strmCluArray[col].arrayData.GetSize()>0)
				crossLength[col*m_nStrmCount+row] = (float)crossCluster[col*m_nStrmCount+row] 
												  / (float)strmCluArray[col].arrayData.GetSize();
												  /// (float)m_StrmPointsArray[col].pointArray.GetSize();
			else
				crossLength[col*m_nStrmCount+row] = HUGE_VAL;

		}

	// eliminate streamlines with total crossLength below threshold
	UpdateData(TRUE);
	float totalXLength;

	if(m_SelectedStrmPointsArray!=NULL)
		delete [] m_SelectedStrmPointsArray;
	m_SelectedStrmPointsArray = new CPointFloatArray[m_nStrmCount];
	m_nSelectedStrmCount = 0;
	for(col=0; col<m_nStrmCount; col++)
	{
		// calculate total crossLength
		totalXLength = 0;

		for(row=0; row<m_nStrmCount; row++)
		{
			if(row==col)
				continue;

			totalXLength = totalXLength + crossLength[col*m_nStrmCount+row];
		}

		if(totalXLength<=m_xLengthThres /*&& strmCluArray[col].arrayData.GetSize()>=m_nCluSizeThres*/)
		{
			m_SelectedStrmPointsArray[m_nSelectedStrmCount].pointArray.
				Copy(m_magCurlSelectedArray[col].pointArray);

			m_nSelectedStrmCount++;

		}
	}

//	for(col=0; col<m_nStrmCount; col++)
//	{
//
//		if(strmCluArray[col].arrayData.GetSize()>=m_xLengthThres)
//		{
//			m_SelectedStrmPointsArray[m_nSelectedStrmCount].pointArray.
//				Copy(m_StrmPointsArray[col].pointArray);
//
//			m_nSelectedStrmCount++;
//
//		}
//	}
	
	
	delete [] crossLength;
	delete [] strmCluArray;
	delete [] clusterUsed;
	delete [] cluStreamArray;

	UpdateData(TRUE);
	if(m_nSelectedStrmCount==1)
		m_scorrTxt.Format("selected 1 streamline");
	else
		m_scorrTxt.Format("selected %i streamlines", m_nSelectedStrmCount);
	UpdateData(FALSE);

}

/*
void CDlgMoreVtx::OnRegionsXCorrGo() 
{

	UpdateData(TRUE);
	m_nCluStreamTxt = "";
	m_scorrTxt = "";
	UpdateData(FALSE);

	if(m_nStrmCount <= 0 || m_nCluster <= 0)
		return;

	// streamline-cluster array
	CIntArray* strmAllCluArray = new CIntArray[m_nStrmCount]; // for building cluStreamArray
	CIntArray* strmCluArray = new CIntArray[m_nStrmCount]; // for building crossCluster

	// cross-cluster matrix
	int nCrossSize = m_nStrmCount*m_nStrmCount;
	int* crossCluster = new int[nCrossSize];

	// cross-length matrix
	float* crossLength = new float[nCrossSize];

	for(int i=0; i<nCrossSize; i++)
	{
		crossLength[i]=0.0f;
		crossCluster[i]=0;
	}

	// cluster-streamline matrix
	CIntArray* cluStreamArray = new CIntArray[m_nCluster];

	// parse each streamline and fill in clusters array
	int iStream, iPoint, iClusterUsed;

	BOOL* clusterUsed = new BOOL[m_nCluster];
	for(i=0; i<m_nCluster; i++)
		clusterUsed[i]=FALSE;

	for(iStream = 0; iStream < m_nStrmCount; iStream++)
	{
		for(i=0; i<m_nCluster; i++)
			clusterUsed[i]=FALSE;

		// set used cluster's entry to TRUE
		for(iPoint = 0; iPoint < m_StrmPointsArray[iStream].pointArray.GetSize(); iPoint++)
		{
			iClusterUsed = m_clusterMap[
					FloatToInt(m_StrmPointsArray[iStream].pointArray[iPoint].z) *img_width*img_height +
					FloatToInt(m_StrmPointsArray[iStream].pointArray[iPoint].y) *img_width +
					FloatToInt(m_StrmPointsArray[iStream].pointArray[iPoint].x)
 					        ];

			if(iClusterUsed>=0 && iClusterUsed<m_nCluster)
			{
				clusterUsed[iClusterUsed] = TRUE;

				// add starting streamline number to cluster-streamline array
			//	if(iPoint==0)
			//		cluStreamArray[iClusterUsed].arrayData.Add(CInt(iStream));
			}

		}

		// for each TRUE entry, add cluster number to all-streamline-cluster array
		for(i=0; i<m_nCluster; i++)
			if(clusterUsed[i] == TRUE)
			{
				strmAllCluArray[iStream].arrayData.Add(CInt(i));
				// add starting streamline number to cluster-streamline array
				cluStreamArray[i].arrayData.Add(CInt(iStream));
			}

	}

	// keep the cluster-longest streamlines started in each cluster
	int maxLength = 0, curLength, maxStream = -1, iTestStream, nAddedStream=0;
	for(i=0; i<m_nCluster; i++)
	{
		maxLength = 0;
		maxStream = -1;

		for(iTestStream=0; iTestStream<cluStreamArray[i].arrayData.GetSize(); iTestStream++)
		{
			curLength = strmAllCluArray[cluStreamArray[i].arrayData[iTestStream].n].arrayData.GetSize();
			if(curLength > maxLength)
			{
				maxLength = curLength;
				maxStream = iTestStream;
			}
		}
		
		// add cluster-longest streamline to streamline-cluster array
		if(maxStream>=0)
		{
			strmCluArray[maxStream].arrayData.Copy(strmAllCluArray[maxStream].arrayData);
			nAddedStream++;
		}
	}

	UpdateData(TRUE);
	if(nAddedStream==1)
		m_nCluStreamTxt.Format("1 long streamline");
	else
		m_nCluStreamTxt.Format("%i long streamlines", nAddedStream);
	UpdateData(FALSE);

	// set crossCluster entries by searching for common clusters in strmCluArray
	int iCurStream, iSearchStream, iSearchCluster, iCurCluster;
	for(iCurStream=0; iCurStream<m_nStrmCount; iCurStream++)
		for(iCurCluster=0; 
			iCurCluster<strmCluArray[iCurStream].arrayData.GetSize(); 
			iCurCluster++)
				for(iSearchStream=iCurStream+1; iSearchStream<m_nStrmCount; iSearchStream++)
					for(iSearchCluster=0; 
						iSearchCluster<strmCluArray[iSearchStream].arrayData.GetSize(); 
						iSearchCluster++)
						if(strmCluArray[iSearchStream].arrayData[iSearchCluster].n ==
						   strmCluArray[iCurStream].arrayData[iCurCluster].n)							
							crossCluster[iCurStream*m_nStrmCount+iSearchStream]++;

	// set crossLength entries to crossCluster divided by streamline length in #clusters
	int row, col;
	for(row=0; row<m_nStrmCount; row++)
		for(col=0; col<m_nStrmCount; col++)
		{
			if(row==col)
				continue;

			if(strmCluArray[col].arrayData.GetSize()>0)
				crossLength[col*m_nStrmCount+row] = (float)crossCluster[col*m_nStrmCount+row] 
												  / (float)strmCluArray[col].arrayData.GetSize();
			else
				crossLength[col*m_nStrmCount+row] = HUGE_VAL;

		}

	// eliminate streamlines with total crossLength below threshold
	UpdateData(TRUE);
	float totalXLength;

	if(m_SelectedStrmPointsArray!=NULL)
		delete [] m_SelectedStrmPointsArray;
	m_SelectedStrmPointsArray = new CPointFloatArray[m_nStrmCount];
	m_nSelectedStrmCount = 0;
	for(col=0; col<m_nStrmCount; col++)
	{
		// calculate total crossLength
		totalXLength = 0;

		for(row=0; row<m_nStrmCount; row++)
		{
			if(row==col)
				continue;

			totalXLength = totalXLength + crossLength[col*m_nStrmCount+row];
		}

		if(totalXLength<=m_xLengthThres && strmCluArray[col].arrayData.GetSize()>=m_nCluSizeThres)
		{
			m_SelectedStrmPointsArray[m_nSelectedStrmCount].pointArray.
				Copy(m_StrmPointsArray[col].pointArray);

			m_nSelectedStrmCount++;

		}
	}

//	for(col=0; col<m_nStrmCount; col++)
//	{
//
//		if(strmCluArray[col].arrayData.GetSize()>=m_xLengthThres)
//		{
//			m_SelectedStrmPointsArray[m_nSelectedStrmCount].pointArray.
//				Copy(m_StrmPointsArray[col].pointArray);
//
//			m_nSelectedStrmCount++;
//
//		}
//	}
	
	
	delete [] crossLength;
	delete [] strmCluArray;
	delete [] clusterUsed;
	delete [] cluStreamArray;

	UpdateData(TRUE);
	if(m_nSelectedStrmCount==1)
		m_scorrTxt.Format("selected 1 streamline");
	else
		m_scorrTxt.Format("selected %i streamlines", m_nSelectedStrmCount);
	UpdateData(FALSE);

}
*/

void CDlgMoreVtx::OnRegionRenderXcorr() 
{
	if(m_SelectedStrmPointsArray==NULL)
		return;

	CVtkUtilities vtkRender;
	vtkRender.RenderStreamlineList(m_SelectedStrmPointsArray, m_nSelectedStrmCount);
	
}

void CDlgMoreVtx::GetClustersOriginalVelocity(CData3DMRI &data3D)
{
	CVector3D* oriVec = new CVector3D[img_width*img_height*img_slices];
	m_pFlowAbstraction->RetrieveOriginalVelocities(oriVec);

	int x, y, z, index;

	for(x=0; x<data3D.img_width; x++)
		for(y=0; y<data3D.img_height; y++)
			for(z=0; z<data3D.img_slices; z++)
			{
				index = img_width*img_height*z + img_width*y + x;
				data3D.mr[index] = FloatToInt(oriVec[index].x);
				data3D.u[index] = FloatToInt(oriVec[index].x);
				data3D.v[index] = FloatToInt(oriVec[index].y);
				data3D.w[index] = FloatToInt(oriVec[index].z);
			}

	delete [] oriVec;
}


void CDlgMoreVtx::OnRegionRenderMinCurl() 
{
	UpdateData(TRUE);

	float* curlMap = ((CDlgFindVortex*)pFindVortex)->GetVorticityMagn();
	if(curlMap == NULL)
		return;

	CPointFloatArray* renderArray = new CPointFloatArray[m_nStrmCount];

	float totalCurl = 0;
	int iStream, iPoint, iAddedStreams = 0;
	for(iStream=0; iStream<m_nStrmCount; iStream++)
	{
		totalCurl = 0.0f;

		for(iPoint=0; iPoint<m_StrmPointsArray[iStream].pointArray.GetSize(); iPoint++)
			totalCurl += curlMap[
					FloatToInt(m_StrmPointsArray[iStream].pointArray[iPoint].z) *img_width*img_height +
					FloatToInt(m_StrmPointsArray[iStream].pointArray[iPoint].y) *img_width +
					FloatToInt(m_StrmPointsArray[iStream].pointArray[iPoint].x)
								];

		totalCurl = totalCurl/(float)iPoint;
		if(totalCurl >= m_minCurlThres)
		{
			renderArray[iAddedStreams].pointArray.Copy(m_StrmPointsArray[iStream].pointArray);
			iAddedStreams++;
		}

	}

	CVtkUtilities vtkRender;
	vtkRender.RenderStreamlineList(renderArray, iAddedStreams);

	delete [] renderArray;
	delete [] curlMap;

}

void CDlgMoreVtx::OnRegionRenderMinMag() 
{
	UpdateData(TRUE);

	CPointFloatArray* renderArray = new CPointFloatArray[m_nStrmCount];

	float* magnMap = ((CDlgFindVortex*)pFindVortex)->GetVelocityMagn();
	float totalMagn = 0;
	int iStream, iPoint, iAddedStreams = 0;
	for(iStream=0; iStream<m_nStrmCount; iStream++)
	{
		totalMagn = 0.0f;

		for(iPoint=0; iPoint<m_StrmPointsArray[iStream].pointArray.GetSize(); iPoint++)
			totalMagn += magnMap[
					FloatToInt(m_StrmPointsArray[iStream].pointArray[iPoint].z) *img_width*img_height +
					FloatToInt(m_StrmPointsArray[iStream].pointArray[iPoint].y) *img_width +
					FloatToInt(m_StrmPointsArray[iStream].pointArray[iPoint].x)
								];

		totalMagn = totalMagn/(float)iPoint;
		if(totalMagn >= m_magnStrmThres)
		{
			renderArray[iAddedStreams].pointArray.Copy(m_StrmPointsArray[iStream].pointArray);
			iAddedStreams++;
		}

	}

	CVtkUtilities vtkRender;
	vtkRender.RenderStreamlineList(renderArray, iAddedStreams);

	delete [] renderArray;
	delete [] magnMap;
	
}

void CDlgMoreVtx::OnRegionRenderMagnCurl() 
{
	UpdateData(TRUE);

	float* curlMap = ((CDlgFindVortex*)pFindVortex)->GetVorticityMagn();
	if(curlMap == NULL)
		return;
	float totalCurl;

	float* magnMap = ((CDlgFindVortex*)pFindVortex)->GetVelocityMagn();
	if(magnMap == NULL)
		return;
	float totalMagn;

	CPointFloatArray* renderArray = new CPointFloatArray[m_nStrmCount];

	int iStream, iPoint, iAddedStreams = 0;
	for(iStream=0; iStream<m_nStrmCount; iStream++)
	{
		totalCurl = 0.0f;
		totalMagn = 0.0f;

		for(iPoint=0; iPoint<m_StrmPointsArray[iStream].pointArray.GetSize(); iPoint++)
		{
			totalCurl += curlMap[
					FloatToInt(m_StrmPointsArray[iStream].pointArray[iPoint].z) *img_width*img_height +
					FloatToInt(m_StrmPointsArray[iStream].pointArray[iPoint].y) *img_width +
					FloatToInt(m_StrmPointsArray[iStream].pointArray[iPoint].x)
								];

			totalMagn += magnMap[
					FloatToInt(m_StrmPointsArray[iStream].pointArray[iPoint].z) *img_width*img_height +
					FloatToInt(m_StrmPointsArray[iStream].pointArray[iPoint].y) *img_width +
					FloatToInt(m_StrmPointsArray[iStream].pointArray[iPoint].x)
								];
		}

		totalCurl = totalCurl/(float)iPoint;
		totalMagn = totalMagn/(float)iPoint;
		if(totalCurl >= m_minCurlThres || totalMagn >= m_magnStrmThres)
		{
			renderArray[iAddedStreams].pointArray.Copy(m_StrmPointsArray[iStream].pointArray);
			iAddedStreams++;
		}

	}


	CVtkUtilities vtkRender;
	vtkRender.RenderStreamlineList(renderArray, iAddedStreams);

	if(m_magCurlSelectedArray!=NULL)
		delete [] m_magCurlSelectedArray;
	m_magCurlSelectedArray = renderArray;
	m_nMagCurlSelectedCount = iAddedStreams;
	delete [] curlMap;
	
}

void CDlgMoreVtx::OnSaveMatlabXcorr() 
{
	// prompt user for filename
	// save data to file

	CFileDialog* dDlg = new CFileDialog(FALSE,"*.mat","Matlab script",
		OFN_HIDEREADONLY | OFN_NOREADONLYRETURN,
		_T("Matlab script (*.m)|*.m|All Files (*.*)|*.*||"), NULL);

	if(dDlg->DoModal()!=IDOK)
		return;

	CData3DMRI dataToSave;
	dataToSave.img_width = img_width;
	dataToSave.img_height = img_height;
	dataToSave.img_slices = img_slices;

	dataToSave.mr = new short[img_width*img_height*img_slices];
	dataToSave.u = new short[img_width*img_height*img_slices];
	dataToSave.v = new short[img_width*img_height*img_slices];
	dataToSave.w = new short[img_width*img_height*img_slices];

	CData3DMRI dataToCopy;
	dataToCopy.img_width = img_width;
	dataToCopy.img_height = img_height;
	dataToCopy.img_slices = img_slices;

	dataToCopy.mr = new short[img_width*img_height*img_slices];
	dataToCopy.u = new short[img_width*img_height*img_slices];
	dataToCopy.v = new short[img_width*img_height*img_slices];
	dataToCopy.w = new short[img_width*img_height*img_slices];

	GetClustersOriginalVelocity(dataToCopy);

	for(int i=0; i<img_width*img_height*img_slices; i++)
		dataToSave.mr[i] = dataToSave.u[i] = dataToSave.v[i] = dataToSave.w[i] = 0;

	int iStream, iPoint, index, maxIndex = img_width*img_height*img_slices;
	for(iStream = 0; iStream < m_nSelectedStrmCount; iStream++)
		for(iPoint = 0; iPoint < m_SelectedStrmPointsArray[iStream].pointArray.GetSize(); iPoint++)
		{
			index = FloatToInt(m_SelectedStrmPointsArray[iStream].pointArray[iPoint].z) *img_width*img_height +
					FloatToInt(m_SelectedStrmPointsArray[iStream].pointArray[iPoint].y) *img_width +
					FloatToInt(m_SelectedStrmPointsArray[iStream].pointArray[iPoint].x);

			if(!(index < maxIndex && index >= 0))
				continue;

			dataToSave.u[index] = dataToCopy.u[index];
			dataToSave.v[index] = dataToCopy.v[index];
			dataToSave.w[index] = dataToCopy.w[index];

		}

	dataToSave.SaveDataMatlab(dDlg->GetPathName());
	delete dDlg;


}

void CDlgMoreVtx::OnRegionSaveVtkXcorrVel() 
{
	// prompt user for filename
	// save data to file

	CFileDialog* dDlg = new CFileDialog(FALSE,"*.mat","Matlab script",
		OFN_HIDEREADONLY | OFN_NOREADONLYRETURN,
		_T("Matlab script (*.m)|*.m|All Files (*.*)|*.*||"), NULL);

	if(dDlg->DoModal()!=IDOK)
		return;

	CData3DMRI dataToSave;
	dataToSave.img_width = img_width;
	dataToSave.img_height = img_height;
	dataToSave.img_slices = img_slices;

	dataToSave.mr = new short[img_width*img_height*img_slices];
	dataToSave.u = new short[img_width*img_height*img_slices];
	dataToSave.v = new short[img_width*img_height*img_slices];
	dataToSave.w = new short[img_width*img_height*img_slices];

	CData3DMRI dataToCopy;
	dataToCopy.img_width = img_width;
	dataToCopy.img_height = img_height;
	dataToCopy.img_slices = img_slices;

	dataToCopy.mr = new short[img_width*img_height*img_slices];
	dataToCopy.u = new short[img_width*img_height*img_slices];
	dataToCopy.v = new short[img_width*img_height*img_slices];
	dataToCopy.w = new short[img_width*img_height*img_slices];

	GetClustersOriginalVelocity(dataToCopy);

	for(int i=0; i<img_width*img_height*img_slices; i++)
		dataToSave.mr[i] = dataToSave.u[i] = dataToSave.v[i] = dataToSave.w[i] = 0;

	int iStream, iPoint, index, maxIndex = img_width*img_height*img_slices;
	for(iStream = 0; iStream < m_nSelectedStrmCount; iStream++)
		for(iPoint = 0; iPoint < m_SelectedStrmPointsArray[iStream].pointArray.GetSize(); iPoint++)
		{
			index = FloatToInt(m_SelectedStrmPointsArray[iStream].pointArray[iPoint].z) *img_width*img_height +
					FloatToInt(m_SelectedStrmPointsArray[iStream].pointArray[iPoint].y) *img_width +
					FloatToInt(m_SelectedStrmPointsArray[iStream].pointArray[iPoint].x);

			if(!(index < maxIndex && index >= 0))
				continue;

			dataToSave.u[index] = dataToCopy.u[index];
			dataToSave.v[index] = dataToCopy.v[index];
			dataToSave.w[index] = dataToCopy.w[index];

		}

	CVtkUtilities vtkSaver;
	vtkSaver.SaveArrowData(dataToSave);

	delete dDlg;
	
}

float CDlgMoreVtx::OnRegionsClusterInformation(float epsilon=0.0f, float* argTerm1 = NULL, 
											   float* argTerm2 = NULL, float* argTerm3 = NULL) 
{
	UpdateClusters();

	float result = 0.0f;

	// make error map
	CPoint3D* error_map = new CPoint3D[img_width*img_height*img_slices];
	CVector3D* oriVecs = new CVector3D[img_width*img_height*img_slices];

	m_pFlowAbstraction->RetrieveOriginalVelocities(oriVecs);

	int x, y, z, index;
	int min_error=INT_MAX, max_error=INT_MIN;
	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
			for(z=0; z<img_slices; z++)
			{
				index = z*img_width*img_height+y*img_width+x;

				if(!(fabs(oriVecs[index].x + oriVecs[index].y + oriVecs[index].z) > 0.0))
					continue;

				error_map[index].x = FloatToInt(oriVecs[index].x-m_fitVec[index].x);
				error_map[index].y = FloatToInt(oriVecs[index].y-m_fitVec[index].y);
				error_map[index].z = FloatToInt(oriVecs[index].z-m_fitVec[index].z);

				if(error_map[index].x > max_error)
					max_error = error_map[index].x;
				if(error_map[index].x < min_error)
					min_error = error_map[index].x;
				if(error_map[index].y > max_error)
					max_error = error_map[index].y;
				if(error_map[index].y < min_error)
					min_error = error_map[index].y;
				if(error_map[index].z > max_error)
					max_error = error_map[index].z;
				if(error_map[index].z < min_error)
					min_error = error_map[index].z;

			}

	// make error histogram
	const int MAX_ERR_RANGE = 10000;
	int error_range = max_error - min_error + 1;
	if(error_range>MAX_ERR_RANGE || error_range<=0)
	{
		CString sms133;
		sms133.Format("Error: error range = %i", error_range);
		AfxMessageBox(sms133);
		delete [] error_map;
		delete [] oriVecs;
		return 0.0f;
	}

	CVector3D* histogram = new CVector3D[error_range];
	for(int i=0; i<error_range; i++)
		histogram[i].x=histogram[i].y=histogram[i].z=0.0f;

	int index_offset = min_error < 0? abs(FloatToInt(min_error)) : 0;
	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
			for(z=0; z<img_slices; z++)
			{
				index = z*img_width*img_height+y*img_width+x;

				if(!(fabs(oriVecs[index].x + oriVecs[index].y + oriVecs[index].z) > 0.0))
					continue;

				if(error_map[index].x+index_offset<error_range)
					histogram[error_map[index].x+index_offset].x++;

				if(error_map[index].y+index_offset<error_range)
					histogram[error_map[index].y+index_offset].y++;

				if(error_map[index].z+index_offset<error_range)
					histogram[error_map[index].z+index_offset].z++;
			}

	int img_size = img_width*img_height*img_slices;
	float Nvox = /*img_size*/0.0f;
	for(i=0; i<img_size; i++)
		if(fabs(oriVecs[i].x + oriVecs[i].y + oriVecs[i].z) > 0.0)
			Nvox++;

	// divide count by total voxels to get probabilities
	for(i=0; i<error_range; i++)
	{
		histogram[i].x=histogram[i].x/ /*(float)img_size*/Nvox;
		histogram[i].y=histogram[i].y/ /*(float)img_size*/Nvox;
		histogram[i].z=histogram[i].z/ /*(float)img_size*/Nvox;
	}

	// compute information measure
	if(epsilon <=0.0f)
		epsilon = 1.0f/300.0f;

	const float Cclu = m_nCluster;
	float cluster_term, voxel_term, error_term;

	// term 1 - cluster data term
	if(img_slices > 1)
		cluster_term = (-18.0*Cclu)*(float)(log(epsilon)/log(2.0));
	else
		cluster_term = (-10.0*Cclu)*(float)(log(epsilon)/log(2.0));

	// term 2 - voxel data term
	// get no points for each cluster, work out circle perimeter or sphere surface area
/*	double* nClusterSize = new double[m_nCluster];
	for(i=0; i<m_nCluster; i++)
		nClusterSize[i]=0.0;

	for(i=0; i<img_size; i++)
		if(m_clusterMap[i]>=0 && m_clusterMap[i]<m_nCluster)
			nClusterSize[m_clusterMap[i]]++;

	double totalBorderVoxels = 0.0;
	double pi = 3.14159265359;
	if(img_slices > 1)
	{
		for(i=0; i<m_nCluster; i++)
			totalBorderVoxels += 4.0*pi*pow((3.0/(4.0*pi))*nClusterSize[i], 2.0/3.0);

	}
	else
	{
		for(i=0; i<m_nCluster; i++)
			totalBorderVoxels += 2.0*pi*sqrt(nClusterSize[i]/pi);

	}
*/

	double totalBorderVoxels = 1.0;	// start at one to send the first voxel
	int max_length = 0, cur_length=0;

	for(i=0; i<img_size-1; i++)
//		if(!(m_clusterMap[i]==-1 && m_clusterMap[i+1]==-1))
		{
			cur_length++;

			if(m_clusterMap[i] != m_clusterMap[i+1])
			{
				if(cur_length>max_length)
					max_length = cur_length;

				totalBorderVoxels++;
				cur_length = 0;

			}

		}

	voxel_term = totalBorderVoxels* ( (float)(log(Cclu)/log(2.0))
									 +(float)(log(max_length)/log(2.0)));

	if(m_nCluster == 1359)
		MessageBeep(0);

	/*
	voxel_term = Cclu * (log(Nvox)/log(2));
	*/
/*
	voxel_term = 6.0*pow(Nvox/Cclu, 2.0/3.0)*(log(Nvox/Cclu)/log(2.0)) + Cclu*(log(Nvox)/log(2.0));
*/
	// term 3 - error term
	error_term = 0.0f;

	for(i=0; i<error_range; i++)
	{
		if(histogram[i].x > 0.0)
			error_term += histogram[i].x * (float)(log(histogram[i].x)/log(2.0));
		if(histogram[i].y > 0.0)
			error_term += histogram[i].y * (float)(log(histogram[i].y)/log(2.0));
		if(img_slices>1)
			if(histogram[i].z > 0.0)
				error_term += histogram[i].z * (float)(log(histogram[i].z)/log(2.0));
	}

	if(img_slices > 1)
		error_term = error_term * -1.0f * Nvox;
	else
		error_term = error_term * -1.0f * Nvox;

	result = cluster_term + voxel_term + error_term;
	
	delete [] error_map;
	delete [] oriVecs;
	delete [] histogram;

	m_sInfoTxt.Format("%.3f = %.3f + %.3f + %.3f", result, cluster_term, voxel_term, error_term);
	m_sInfoRanges.Format("eps=%f err_r=%i #clu=%i", epsilon, error_range, m_nCluster);
	UpdateData(FALSE);

	if(argTerm1 != NULL && argTerm2 != NULL && argTerm3 != NULL)
	{
		*argTerm1 = cluster_term; 
		*argTerm2 = voxel_term; 
		*argTerm3 = error_term; 
	}

	return result;
	
}

void CDlgMoreVtx::OnRegionsInformationBatchGo() 
{
	const int MIN_CLUSTERS = 1;
	const int MAX_CLUSTERS = img_width*img_height*img_slices-1;

	ofstream of;
	of.open("c:\\testINFO.txt", ios::out);

	CVector3D* oriVecs = new CVector3D[img_width*img_height*img_slices];

	int nClusters;
	float cluster_term, voxel_term, error_term, total_info;
	int x, y, z, index;
	float nVoxels;
	for(nClusters=MIN_CLUSTERS; nClusters<=400; nClusters++)
	{
		UpdateData(TRUE);
		m_RegionClusters = nClusters;
		UpdateData(FALSE);

/*		
		total_info = OnRegionsClusterInformation(1.0/1800.0, &cluster_term, &voxel_term, &error_term);

		of	<< nClusters << " " << total_info 
			<< " " << cluster_term << " " << voxel_term << " " 
			<< error_term << "\n" << flush;
*/		


		UpdateClusters();
		m_pFlowAbstraction->RetrieveOriginalVelocities(oriVecs);

		total_info = 0;
		nVoxels=0;
		for(x=2; x<img_width-2; x++)
			for(y=2; y<img_height-2; y++)
				for(z=0; z<img_slices; z++)
				{
					index = z*img_width*img_height+y*img_width+x;

					if(m_clusterMap[index] == -1)
						continue;

					total_info += pow(oriVecs[index].x-m_fitVec[index].x, 2.0) +
								  pow(oriVecs[index].y-m_fitVec[index].y, 2.0) +
								  pow(oriVecs[index].z-m_fitVec[index].z, 2.0);

					nVoxels++;


				}

		of	<< sqrt(total_info/nVoxels) << "\n" << flush;

	}

	delete [] oriVecs;


}
