// DlgTemplateMatch.cpp : implementation file
//

#include "stdafx.h"
#include "Texton.h"
#include "DlgTemplateMatch.h"
#include "TextonView.h"
#include <math.h>
//#include <ipl.h>
#include "ImagePgm.h"
#include "DlgTextBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTemplateMatch dialog


CDlgTemplateMatch::CDlgTemplateMatch(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTemplateMatch::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTemplateMatch)
	m_ProgressText = _T("");
	m_initAngle = 90;
	m_initCX = 50;
	m_initCY = 50;
	m_initG = 150;
	m_initLH = 3;
	m_initLW = 4;
	m_initWP = 4;
	m_initW = 250;
	m_initAMAG = 12000.0f;
	m_initGMAG = 0.1f;
	m_initLHMAG = 0.1f;
	m_initLWMAG = 0.1f;
	m_initWMAG = 0.1f;
	m_initWPMAG = 0.1f;
	m_FinalParms = _T("");
	m_weightINT = 1.0f;
	m_weigthWP = 0.0f;
	m_trainWP = 15.0f;
	m_trainLW = 9.0f;
	m_trainLH = 8.0f;
	m_weigthLW = 0.0f;
	m_weigthLH = 0.0f;
	m_positionFixed = TRUE;
	m_bSaveCallParms = FALSE;
	m_trainG = 150.0f;
	m_trainW = 250.0f;
	m_weigthMH = 1.0f;
	m_templFixed = FALSE;
	m_trainANG = 0.0f;
	m_errorThres = 0.0f;
	m_tmplClusters = 0.0f;
	m_tmplDOWNF = 4.0f;
	//}}AFX_DATA_INIT

	binErrorMap = NULL;
	debugText = NULL;
	isActive = FALSE;

}


void CDlgTemplateMatch::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTemplateMatch)
	DDX_Control(pDX, IDC_DRAW_THRES, m_tmplErrorThresCtrl);
	DDX_Control(pDX, IDC_TMPL_FINAL, m_FinalParmsCtrl);
	DDX_Control(pDX, IDC_DRAW_ERROR, m_DrawErrorCtrl);
	DDX_Control(pDX, IDC_TMPL_DRAWFIT, m_DrawFitCtrl);
	DDX_Control(pDX, IDC_PROGRESS1, m_ProgressBar);
	DDX_Control(pDX, IDC_TMPL_PROGRESS, m_ProgressCtrl);
	DDX_Text(pDX, IDC_TMPL_PROGRESS, m_ProgressText);
	DDX_Text(pDX, IDC_TPL_ANGLE, m_initAngle);
	DDX_Text(pDX, IDC_TPL_CX, m_initCX);
	DDX_Text(pDX, IDC_TPL_CY, m_initCY);
	DDX_Text(pDX, IDC_TPL_G, m_initG);
	DDX_Text(pDX, IDC_TPL_LH, m_initLH);
	DDX_Text(pDX, IDC_TPL_LW, m_initLW);
	DDX_Text(pDX, IDC_TPL_WP, m_initWP);
	DDX_Text(pDX, IDC_TPL_W, m_initW);
	DDX_Text(pDX, IDC_TPL_ANGMAG, m_initAMAG);
	DDX_Text(pDX, IDC_TPL_GMAG, m_initGMAG);
	DDX_Text(pDX, IDC_TPL_LHMAG, m_initLHMAG);
	DDX_Text(pDX, IDC_TPL_LWMAG, m_initLWMAG);
	DDX_Text(pDX, IDC_TPL_WMAG, m_initWMAG);
	DDX_Text(pDX, IDC_TPL_WPMAG, m_initWPMAG);
	DDX_Text(pDX, IDC_TMPL_FINAL, m_FinalParms);
	DDX_Text(pDX, IDC_TPL_INTWGTH, m_weightINT);
	DDX_Text(pDX, IDC_TPL_WPWGTH, m_weigthWP);
	DDX_Text(pDX, IDC_TPL_TRAINWP, m_trainWP);
	DDX_Text(pDX, IDC_TPL_TRAINLW, m_trainLW);
	DDX_Text(pDX, IDC_TPL_TRAINLH, m_trainLH);
	DDX_Text(pDX, IDC_TPL_LWWGTH, m_weigthLW);
	DDX_Text(pDX, IDC_TPL_LHWGTH, m_weigthLH);
	DDX_Check(pDX, IDC_TMPL_POSFIX, m_positionFixed);
	DDX_Check(pDX, IDC_TMPL_SAVE, m_bSaveCallParms);
	DDX_Text(pDX, IDC_TPL_TRAING, m_trainG);
	DDX_Text(pDX, IDC_TPL_TRAINW, m_trainW);
	DDX_Text(pDX, IDC_TPL_MHWGTH, m_weigthMH);
	DDX_Check(pDX, IDC_TMPL_TMPFIX, m_templFixed);
	DDX_Text(pDX, IDC_TPL_TRAINANG, m_trainANG);
	DDX_Text(pDX, IDC_TPL_MAXERR, m_errorThres);
	DDX_Text(pDX, IDC_TPL_MAXERR2, m_tmplClusters);
	DDV_MinMaxFloat(pDX, m_tmplClusters, 1.f, 15.f);
	DDX_Text(pDX, IDC_TPL_MAXERR3, m_tmplDOWNF);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgTemplateMatch, CDialog)
	//{{AFX_MSG_MAP(CDlgTemplateMatch)
	ON_BN_CLICKED(IDC_TMPL_START, OnMatchStart)
	ON_BN_CLICKED(IDC_DRAW_ERROR, OnDrawError)
	ON_BN_CLICKED(IDC_TMPL_DRAWFIT, OnTmplDrawfit)
	ON_BN_CLICKED(IDC_TMPL_DRAWINIT, OnTmplDrawinit)
	ON_BN_CLICKED(IDC_TMPL_START2, OnTmplStart2)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_TMPLDSTC, OnTmplDSTC)
	ON_BN_CLICKED(IDC_TMPL_DRAWORI, OnTmplDrawori)
	ON_BN_CLICKED(IDC_DRAW_THRES, OnDrawThres)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTemplateMatch message handlers

CDlgTemplateMatch* pCbckTempl;

void callback_CDlgTemplateMatch(int percent)
{
	pCbckTempl->PrintProgress(percent);
}

void CDlgTemplateMatch::OnMatchStart(BOOL buttonPress, float* final_parms) 
{
	UpdateData(TRUE);

	if(m_trainLW<=m_trainLH)
	{
		AfxMessageBox("Training values must satisfy lw>lh.");
		return;
	}

	if(m_initLW<=m_initLH)
	{
		AfxMessageBox("Initial values must satisfy lw>lh.");
		return;
	}

	if(m_initG>=m_initW)
	{
		AfxMessageBox("Initial values must satisfy w>g.");
		return;
	}

	CTextonView* pTView = (CTextonView*)pView;
	pCbckTempl = this;
	if(buttonPress)
		m_ProgressText = "Working...";
	m_FinalParms = "";

	((CTextonView*)pView)->DrawPgm();

	PrintProgress(0);
	UpdateData(FALSE);

	m_TemplateMatcher.m_errorThres = m_errorThres;
	m_TemplateMatcher.m_trainANG = m_trainANG;
	m_TemplateMatcher.m_templFixed = m_templFixed;
	m_TemplateMatcher.b_saveCalls = m_bSaveCallParms;
	m_TemplateMatcher.m_weigthMH = m_weigthMH;
	m_TemplateMatcher.m_weightINT = m_weightINT;
	m_TemplateMatcher.m_weigthWP = m_weigthWP;
	m_TemplateMatcher.m_trainWP = m_trainWP;
	m_TemplateMatcher.m_trainLW = m_trainLW;
	m_TemplateMatcher.m_trainLH = m_trainLH;
	m_TemplateMatcher.m_weigthLW = m_weigthLW;
	m_TemplateMatcher.m_weigthLH = m_weigthLH;
	m_TemplateMatcher.iInitAngle = m_initAngle;
	m_TemplateMatcher.m_initG = m_initG;
	m_TemplateMatcher.m_initLH = m_initLH;
	m_TemplateMatcher.m_initLW = m_initLW;
	m_TemplateMatcher.m_initWP = m_initWP;
	m_TemplateMatcher.m_initW = m_initW;
	m_TemplateMatcher.m_initAMAG = m_initAMAG;
	m_TemplateMatcher.m_initGMAG = m_initGMAG;
	m_TemplateMatcher.m_initLHMAG = m_initLHMAG;
	m_TemplateMatcher.m_initLWMAG = m_initLWMAG;
	m_TemplateMatcher.m_initWMAG = m_initWMAG;
	m_TemplateMatcher.m_initWPMAG = m_initWPMAG;
	m_TemplateMatcher.m_weightINT = m_weightINT;
	m_TemplateMatcher.m_weigthWP = m_weigthWP;
	m_TemplateMatcher.m_trainG = m_trainG;
	m_TemplateMatcher.m_trainW = m_trainW;
	m_TemplateMatcher.templ_xc = m_initCX;
	m_TemplateMatcher.templ_yc = m_initCY;

	m_TemplateMatcher.match_main(pTView->pgmImage.imgData,
		pTView->pgmImage.imgWidth, pTView->pgmImage.imgHeight,
		callback_CDlgTemplateMatch, m_positionFixed, m_bSaveCallParms, final_parms);

	if(buttonPress)
		m_ProgressText = "Done";
	m_FinalParms = m_TemplateMatcher.txtFinalParms;
	m_DrawFitCtrl.EnableWindow(TRUE);
	m_DrawErrorCtrl.EnableWindow(TRUE);
	UpdateData(FALSE);
}

void CDlgTemplateMatch::PrintProgress(int percent)
{
	m_ProgressBar.SetPos(percent);
	m_FinalParms = m_TemplateMatcher.txtFinalParms;
	m_FinalParmsCtrl.SetWindowText(m_FinalParms);
}

void CDlgTemplateMatch::OnDrawError() 
{
	UpdateData(TRUE);
	if(m_TemplateMatcher.errorImage!=NULL)
	{
		CTextonView* pTView = (CTextonView*)pView;

		// draw error image
		int x,y;

		// normalise
		float max_error = 0;
		const int max_grey = 255;
		/*
		for(x=m_TemplateMatcher.errorCanvas;x<pTView->pgmImage.imgWidth-m_TemplateMatcher.errorCanvas;x++)
			for(y=m_TemplateMatcher.errorCanvas;y<pTView->pgmImage.imgHeight-m_TemplateMatcher.errorCanvas;y++)
		*/
		for(x=0;x<pTView->pgmImage.imgWidth;x++)
			for(y=0;y<pTView->pgmImage.imgHeight;y++)
				if(m_TemplateMatcher.errorImage[pTView->pgmImage.imgWidth*y+x]>max_error)
					max_error = m_TemplateMatcher.errorImage[pTView->pgmImage.imgWidth*y+x];
		
		float norm_ratio = max_grey/max_error;
		int imgHeight, imgWidth;

		// allocate destination memory
		imgHeight = ((CTextonView*)pView)->pgmImage.imgHeight;
		imgWidth = ((CTextonView*)pView)->pgmImage.imgWidth;
		
		if(((CTextonView*)pView)->testImage.imgData != NULL)
			delete ((CTextonView*)pView)->testImage.imgData;
		
		((CTextonView*)pView)->testImage.imgData = new int[imgWidth*imgHeight];
		((CTextonView*)pView)->testImage.imgWidth = imgWidth;
		((CTextonView*)pView)->testImage.imgHeight = imgHeight;

		// copy error map to destination
		for(x = 0; x< imgWidth; x++)
			for(y = 0; y<imgHeight; y++)
				((CTextonView*)pView)->testImage.imgData[imgWidth*y+x] = 
					(int)(m_TemplateMatcher.errorImage[imgWidth*y+x]*norm_ratio);
			
		((CTextonView*)pView)->DrawTest();
		
	}
	
}

void CDlgTemplateMatch::OnTmplDrawfit() 
{
/*	UpdateData(TRUE);
	// draw template over original image
	int xx, yy, imgHeight, imgWidth;
	unsigned char buffer[4];
	imgHeight = ((CTextonView*)pView)->pgmImage.imgHeight;
	imgWidth = ((CTextonView*)pView)->pgmImage.imgWidth;
	
	if(((CTextonView*)pView)->testImage.imgData != NULL)
		delete ((CTextonView*)pView)->testImage.imgData;
	
	((CTextonView*)pView)->testImage.imgData = new int[imgWidth*imgHeight];
	((CTextonView*)pView)->testImage.imgWidth = imgWidth;
	((CTextonView*)pView)->testImage.imgHeight = imgHeight;
	
	for(xx = 0; xx< imgWidth; xx++)
		for(yy = 0; yy<imgHeight; yy++)
		{
			iplGetPixel(m_TemplateMatcher.imgTmpl, xx, yy, &buffer);
			if(buffer[0]!=m_TemplateMatcher.background)
				((CTextonView*)pView)->testImage.imgData[imgWidth*yy+xx] = buffer[0];
			else
				((CTextonView*)pView)->testImage.imgData[imgWidth*yy+xx] =
				((CTextonView*)pView)->pgmImage.imgData[imgWidth*yy+xx] ;
		}
		
	((CTextonView*)pView)->DrawTest();
	*/	
}

void CDlgTemplateMatch::OnTmplDrawinit() 
{
	((CTextonView*)pView)->DrawPgm();
	CTextonView* pTView = (CTextonView*)pView;
	UpdateData(TRUE);
	m_ProgressText = "Working...";
	m_FinalParms = "";
	UpdateData(FALSE);

	m_TemplateMatcher.m_errorThres = m_errorThres;
	m_TemplateMatcher.m_trainANG = m_trainANG;
	m_TemplateMatcher.m_templFixed = m_templFixed;
	m_TemplateMatcher.m_weigthMH = m_weigthMH;
	m_TemplateMatcher.m_weightINT = m_weightINT;
	m_TemplateMatcher.m_weigthWP = m_weigthWP;
	m_TemplateMatcher.m_trainWP = m_trainWP;
	m_TemplateMatcher.m_trainLW = m_trainLW;
	m_TemplateMatcher.m_trainLH = m_trainLH;
	m_TemplateMatcher.m_weigthLW = m_weigthLW;
	m_TemplateMatcher.m_weigthLH = m_weigthLH;
	m_TemplateMatcher.iInitAngle = m_initAngle;
	m_TemplateMatcher.m_initG = m_initG;
	m_TemplateMatcher.m_initLH = m_initLH;
	m_TemplateMatcher.m_initLW = m_initLW;
	m_TemplateMatcher.m_initWP = m_initWP;
	m_TemplateMatcher.m_initW = m_initW;
	m_TemplateMatcher.m_initAMAG = m_initAMAG;
	m_TemplateMatcher.m_initGMAG = m_initGMAG;
	m_TemplateMatcher.m_initLHMAG = m_initLHMAG;
	m_TemplateMatcher.m_initLWMAG = m_initLWMAG;
	m_TemplateMatcher.m_initWMAG = m_initWMAG;
	m_TemplateMatcher.m_initWPMAG = m_initWPMAG;
	m_TemplateMatcher.m_weightINT = m_weightINT;
	m_TemplateMatcher.m_weigthWP = m_weigthWP;
	m_TemplateMatcher.m_trainG = m_trainG;
	m_TemplateMatcher.m_trainW = m_trainW;
	m_TemplateMatcher.templ_xc = m_initCX;
	m_TemplateMatcher.templ_yc = m_initCY;

	m_TemplateMatcher.DrawInitial(pTView->pgmImage.imgData,
		pTView->pgmImage.imgWidth, pTView->pgmImage.imgHeight,
		callback_CDlgTemplateMatch);
	m_ProgressText = "Done";
	m_FinalParms = m_TemplateMatcher.txtFinalParms;
	m_DrawFitCtrl.EnableWindow(TRUE);
	UpdateData(FALSE);
			
}

void CDlgTemplateMatch::OnTmplStart2(BOOL buttonPress) 
{
	UpdateData(TRUE);

	// derive error map

	if(AfxMessageBox("This function will:\n\n1) Slide Fixed Template across image using entered parameters\n\
2) Threshold the resulting error map with Maximum Error Threshold\n\
3) Calculate a centroid for the thresholded points\n\
4) Optimise the Initial Template centered at the centroid.\n\n\
Have you entered all parameters correctly?", MB_YESNO)==IDNO)
		return;

	m_ProgressText = "Deriving error map with static template...";
	m_templFixed = TRUE;
	m_positionFixed = FALSE;
	UpdateData(FALSE);
	OnMatchStart(FALSE);

	// threshold error map
	m_ProgressText = "Thresholding error map...";
	UpdateData(FALSE);

	int x,y;
	int imgWidth, imgHeight;
	imgWidth = ((CTextonView*)pView)->pgmImage.imgWidth;
	imgHeight = ((CTextonView*)pView)->pgmImage.imgHeight;

	if(binErrorMap != NULL)
		delete binErrorMap;
	binErrorMap = new int[imgWidth*imgHeight];   // note this is internal (binary) map

		// set destination to zero
	for(x = 0; x< imgWidth; x++)
		for(y = 0; y<imgHeight; y++)
			binErrorMap[imgWidth*y+x] = 0;
	
		// copy pixels below threshold to our errorMap
	int noPoints=0;
	for(x = m_TemplateMatcher.errorCanvas; x< imgWidth-m_TemplateMatcher.errorCanvas; x++)
		for(y = m_TemplateMatcher.errorCanvas; y<imgHeight-m_TemplateMatcher.errorCanvas; y++)
			if(m_TemplateMatcher.errorImage[imgWidth*y+x] <= m_errorThres)
			{
				binErrorMap[imgWidth*y+x]=1;
				noPoints++;
			}

	// derive centroid position
	m_ProgressText = "Deriving centroid position...";
	UpdateData(FALSE);

	float mean_x=0.0, mean_y=0.0;
	for(x=0; x<imgWidth; x++)
		for(y=0; y<imgHeight; y++)
			if(binErrorMap[imgWidth*y+x]==1)
			{	
				mean_x = mean_x + (float)x / (float)noPoints;
				mean_y = mean_y + (float)y / (float)noPoints;
			}

	// optimise template at centroid
	m_ProgressText = "Optimising template at centroid...";
	m_templFixed = FALSE;
	m_positionFixed = TRUE;
	double dmy;
	m_initCX = (modf(mean_x,&dmy)>0.5?ceil(mean_x):floor(mean_x));
	m_initCY = (modf(mean_y,&dmy)>0.5?ceil(mean_y):floor(mean_y));
	UpdateData(FALSE);

	OnMatchStart(FALSE);

	// The End
	if(buttonPress)
		m_ProgressText = "Done.";
	m_templFixed = FALSE;
	m_positionFixed = FALSE;
	char szPos[100];
	sprintf(szPos," at (%.0f, %.0f)", m_initCX, m_initCY);
	m_FinalParms = m_TemplateMatcher.txtFinalParms + (CString)szPos;
	m_tmplErrorThresCtrl.EnableWindow(TRUE);
	UpdateData(FALSE);

}

void CDlgTemplateMatch::OnButton1() 
{

	/* mean_vector_A elements sequence: 
	0-lw, 1-lh, 2-wp, 3-w, 4-g */
	
	m_errorThres = 6000;
	m_trainANG = 90.0;
	m_weigthMH = 100.0;
	m_weightINT = 1.0;
	m_weigthWP = 0.0;
	m_trainWP = m_TemplateMatcher.mean_vector_A[2];
	m_trainLW = m_TemplateMatcher.mean_vector_A[0];
	m_trainLH = m_TemplateMatcher.mean_vector_A[1];
	m_weigthLW = 0.0;
	m_weigthLH = 0.0;
	m_initAngle = 90.0;
	m_initG = m_TemplateMatcher.mean_vector_A[4];
	m_initLH = m_TemplateMatcher.mean_vector_A[1];
	m_initLW = m_TemplateMatcher.mean_vector_A[0];
	m_initWP = m_TemplateMatcher.mean_vector_A[2];
	m_initW = m_TemplateMatcher.mean_vector_A[3];
	m_weightINT = 1.0;
	m_weigthWP = 0.0;
	m_trainG = m_TemplateMatcher.mean_vector_A[4];
	m_trainW = m_TemplateMatcher.mean_vector_A[3];
	m_initCX = 50;
	m_initCY = 50;
	m_tmplDOWNF = 4.0;
	m_tmplClusters = 9;

	UpdateData(FALSE);
	
	
}
void CDlgTemplateMatch::OnTmplDSTC() 
{

/*
	UpdateData(TRUE);

	// prompt user if they're sure
	char szRatio[20];
	sprintf(szRatio, "%g", m_tmplDOWNF);
	CString sPrompt = "This function will:\n\n1) Downscale image to 1/" + (CString)szRatio + "th its size.\n\
2) Slide Fixed Template across image using 1/"+(CString)szRatio+"th of entered parameters\n\
3) Threshold the resulting error map with Maximum Error Threshold\n\
4) Find up to # clusters for the thresholded points with adjacency\n\
5) Optimise the Initial Template centered at each centroid.\n\n\
Have you entered all parameters correctly?";
	if(AfxMessageBox(sPrompt, MB_YESNO)==IDNO)
		return;

	if(debugText==NULL || isActive == FALSE)
	{
		debugText = new CDlgTextBox(this);
		debugText->Create(IDD_TEXTOUT, this);
		debugText->ShowWindow(SW_SHOW);
		isActive = TRUE;
		debugText->isActive = &isActive;
	}

	// derive error map
	int oriHeight, oriWidth, smlHeight, smlWidth;
	int buffer;

	oriHeight = ((CTextonView*)pView)->pgmImage.imgHeight;
	oriWidth = ((CTextonView*)pView)->pgmImage.imgWidth;
	smlHeight = ceil(oriHeight/m_tmplDOWNF);
	smlWidth = ceil(oriWidth/m_tmplDOWNF);

		// downscale image
		// allocate source
	IplImage *oriImage;
	oriImage = iplCreateImageHeader(1,0,IPL_DEPTH_8U,"GRAY","GRAY", 
		IPL_DATA_ORDER_PIXEL, IPL_ORIGIN_TL, IPL_ALIGN_DWORD,
		oriWidth, oriHeight, NULL, NULL, NULL, NULL );
	if( !oriImage ) return;
	iplAllocateImage( oriImage, 0, 0 );
	if( !oriImage->imageData ) return;
			
		// copy source
	int x,y;
	for(x=0; x<oriWidth; x++)
		for(y=0; y<oriHeight; y++)
		{	
			buffer = ((CTextonView*)pView)->pgmImage.imgData[oriWidth*y+x];
			iplPutPixel(oriImage, x, y, &buffer);
		}

		// allocate target
	IplImage *smlImage;
	smlImage = iplCreateImageHeader(1,0,IPL_DEPTH_8U,"GRAY","GRAY", 
		IPL_DATA_ORDER_PIXEL, IPL_ORIGIN_TL, IPL_ALIGN_DWORD,
		smlWidth, smlHeight, NULL, NULL, NULL, NULL );
	if( !smlImage ) return;
	iplAllocateImage( smlImage, 0, 0 );
	if( !smlImage->imageData ) return;

		// decimate image
	iplDecimate(oriImage, smlImage, 1, m_tmplDOWNF, 1, m_tmplDOWNF, IPL_INTER_NN);

		// compute error map on downscaled image
	m_ProgressText = "Deriving error map with static template...";
	m_templFixed = TRUE;
	m_positionFixed = FALSE;
	UpdateData(FALSE);

	int* iSmlImage = new int[smlWidth*smlHeight];
	for(x=0;x<smlWidth;x++)
		for(y=0;y<smlHeight;y++)
		{
			iplGetPixel(smlImage, x, y, &buffer);
			iSmlImage[smlWidth*y+x] = buffer;
		}

	OnMatchDownsampled(iSmlImage, smlHeight, smlWidth);

	/**** TEST CODE - UPSAMPLE ERROR MAP FOR DRAWING *****/
	/*
	int max_grey=255;
	float max_error, norm_ratio;
	max_error = 0;
	for(x=0;x<smlWidth;x++)
		for(y=0;y<smlHeight;y++)
			if(m_TemplateMatcher.errorImage[smlWidth*y+x]>max_error)
				max_error = m_TemplateMatcher.errorImage[smlWidth*y+x];
	norm_ratio = max_grey/max_error;
	for(x=0;x<smlWidth;x++)
		for(y=0;y<smlHeight;y++)
			m_TemplateMatcher.errorImage[smlWidth*y+x]=
			m_TemplateMatcher.errorImage[smlWidth*y+x]*norm_ratio;
	for(x=0;x<smlWidth;x++)
		for(y=0;y<smlHeight;y++)
		{
			buffer = m_TemplateMatcher.errorImage[smlWidth*y+x];
			iplPutPixel(smlImage, x, y, &buffer);
		}
	iplZoom(smlImage, oriImage, m_tmplDOWNF, 1, m_tmplDOWNF, 1, IPL_INTER_LINEAR);
	delete m_TemplateMatcher.errorImage;
	m_TemplateMatcher.errorImage = new float[oriWidth*oriHeight];
	for(x=0;x<oriWidth;x++)
		for(y=0;y<oriHeight;y++)
		{	
			iplGetPixel(oriImage, x, y, &buffer);
			m_TemplateMatcher.errorImage[oriWidth*y+x] = buffer;
		}
	
	// uncomment this bit ONLY to draw raw error map without zooming
	((CTextonView*)pView)->pgmImage.imgData = iSmlImage;
	((CTextonView*)pView)->pgmImage.imgWidth = smlWidth;
	((CTextonView*)pView)->pgmImage.imgHeight = smlHeight;
	((CTextonView*)pView)->testImage.imgWidth = smlWidth;
	((CTextonView*)pView)->testImage.imgHeight = smlHeight;

	return;
	*/
	/***** END OF TEST CODE ****/
/*
	delete iSmlImage;

	// threshold error map
	m_ProgressText = "Thresholding error map...";
	UpdateData(FALSE);

	if(binErrorMap != NULL)
		delete binErrorMap;
	binErrorMap = new int[smlWidth*smlHeight];

		// set destination to zero
	for(x = 0; x< smlWidth; x++)
		for(y = 0; y<smlHeight; y++)
			binErrorMap[smlWidth*y+x] = 0;
	
		// copy pixels below threshold to our errorMap
	for(x = m_TemplateMatcher.errorCanvas; x< smlWidth-m_TemplateMatcher.errorCanvas; x++)
		for(y = m_TemplateMatcher.errorCanvas; y<smlHeight-m_TemplateMatcher.errorCanvas; y++)
			if(m_TemplateMatcher.errorImage[smlWidth*y+x] <= m_errorThres)
				binErrorMap[smlWidth*y+x]=1;

	// cluster error bits
	m_ProgressText = "Finding error cluster centroids...";
	UpdateData(FALSE);
	float* xPosArr;
	float* yPosArr;
	int actual_clusters = m_tmplClusters;

	int total_clusters;		// for reporting to user
	FindCandidateClusters(binErrorMap, smlWidth, smlHeight, &xPosArr, &yPosArr, &actual_clusters, &total_clusters);
	char sz566[100];
	if(actual_clusters>1)
		sprintf(sz566, "%i clusters found. Using best %i.\r\n", total_clusters, actual_clusters);
	else
		sprintf(sz566, "1 cluster found.\r\n");
	debugText->AddText(sz566);

	// convert each position pair from small to large image coordinates
	CoordsSmallToLarge(xPosArr, yPosArr, m_tmplDOWNF, actual_clusters);

	// optimise template at each centroid position
	int cluster;
	float** final_parms = new float*[actual_clusters];  // angle, lw, lh, w, g, wp, error
	for(cluster=0; cluster<actual_clusters; cluster++)
		final_parms[cluster] = new float[7];
	double dummy;
	char szPos[100];

	if(actual_clusters>1)
		debugText->AddText("Optimising template at each centroid:\r\n");
	else
		debugText->AddText("Optimising template at centroid:\r\n");

	for(cluster=0; cluster<actual_clusters; cluster++)
	{
		m_initCX = (modf(xPosArr[cluster],&dummy)>0.5?ceil(xPosArr[cluster]):floor(xPosArr[cluster]));
		m_initCY = (modf(yPosArr[cluster],&dummy)>0.5?ceil(yPosArr[cluster]):floor(yPosArr[cluster]));
		m_templFixed = FALSE;
		m_positionFixed = TRUE;
		sprintf(szPos, "(%.0f, %.0f)", m_initCX, m_initCY);
		m_ProgressText = "Optimising template at centroid "+(CString)szPos+" ...";
		UpdateData(FALSE);
		OnMatchStart(FALSE, final_parms[cluster]);
		debugText->AddText((CString)szPos+(CString)" ");
		debugText->AddText(m_TemplateMatcher.txtFinalParms);
		debugText->AddText("\r\n");
	}

	// set best fit to lowest error centroid
	int min_index=0;
	float min_error = final_parms[0][6];
	for(cluster=1; cluster<actual_clusters; cluster++)
		if(final_parms[cluster][6]<min_error)
		{
			min_error = final_parms[cluster][6];
			min_index = cluster;
		}

	m_TemplateMatcher.templ_xc = xPosArr[min_index];
	m_TemplateMatcher.templ_yc = yPosArr[min_index];
	float final_error = m_TemplateMatcher.EvaluateTemplate(final_parms[min_index][0], final_parms[min_index][1], final_parms[min_index][2], 
		final_parms[min_index][3], final_parms[min_index][4], final_parms[min_index][5], FALSE);

	sprintf(szPos, "Best match chosen at centroid (%i, %i) with parameters:\r\n", 
		m_TemplateMatcher.templ_xc, m_TemplateMatcher.templ_yc);
	debugText->AddText(szPos);
	sprintf(szPos, "angle=%.0f lw=%.0f lh=%.0f w=%.0f g=%.0f wp=%.0f error = %f\r\n",
		final_parms[min_index][0], final_parms[min_index][1], final_parms[min_index][2], 
		final_parms[min_index][3], final_parms[min_index][4], final_parms[min_index][5], final_error);
	debugText->AddText(szPos);


	// The End
	m_ProgressText = "Done.";
	m_templFixed = FALSE;
	m_positionFixed = FALSE;
	sprintf(szPos," at (%.0f, %.0f)", xPosArr[min_index], yPosArr[min_index]);
	m_FinalParms = m_TemplateMatcher.txtFinalParms + (CString)szPos;
	UpdateData(FALSE);

	for(cluster=0; cluster<actual_clusters; cluster++)
		delete final_parms[cluster];
	delete final_parms;
	delete xPosArr;
	delete yPosArr;

	// plot error map at normal size
	float max_error = 0;
	const int max_grey = 255;
	for(x=0;x<smlWidth;x++)
		for(y=0;y<smlHeight;y++)
			if(m_TemplateMatcher.errorImage[smlWidth*y+x]>max_error)
				max_error = m_TemplateMatcher.errorImage[smlWidth*y+x];
	float norm_ratio = max_grey/max_error;
	iplSet(smlImage, 0);
	for(x=0;x<smlWidth;x++)
		for(y=0;y<smlHeight;y++)
		{
			buffer = m_TemplateMatcher.errorImage[smlWidth*y+x]*norm_ratio;
			iplPutPixel(smlImage, x, y, &buffer);
		}
	iplZoom(smlImage, oriImage, m_tmplDOWNF, 1, m_tmplDOWNF, 1, IPL_INTER_NN);
	delete m_TemplateMatcher.errorImage;
	m_TemplateMatcher.errorImage = new float[oriWidth*oriHeight];
	for(x=0;x<oriWidth;x++)
		for(y=0;y<oriHeight;y++)
		{	
			iplGetPixel(oriImage, x, y, &buffer);
			m_TemplateMatcher.errorImage[oriWidth*y+x] = buffer;
		}

	// plot error threshold at normal size
	for(x=0;x<smlWidth;x++)
		for(y=0;y<smlHeight;y++)
		{
			buffer = binErrorMap[smlWidth*y+x];
			iplPutPixel(smlImage, x, y, &buffer);
		}
	iplZoom(smlImage, oriImage, m_tmplDOWNF, 1, m_tmplDOWNF, 1, IPL_INTER_NN);
	delete binErrorMap;
	binErrorMap = new int[oriWidth*oriHeight];
	for(x=0;x<oriWidth;x++)
		for(y=0;y<oriHeight;y++)
		{	
			iplGetPixel(oriImage, x, y, &buffer);
			binErrorMap[oriWidth*y+x] = buffer;
		}


	m_tmplErrorThresCtrl.EnableWindow(TRUE);
	iplDeallocate( oriImage, IPL_IMAGE_ALL );
	iplDeallocate( smlImage, IPL_IMAGE_ALL );
*/
}

void CDlgTemplateMatch::OnMatchDownsampled(int* image, int height, int width)
{
	UpdateData(TRUE);

	if(m_trainLW<=m_trainLH)
	{
		AfxMessageBox("Training values must satisfy lw>lh.");
		return;
	}

	if(m_initLW<=m_initLH)
	{
		AfxMessageBox("Initial values must satisfy lw>lh.");
		return;
	}

	if(m_initG>=m_initW)
	{
		AfxMessageBox("Initial values must satisfy w>g.");
		return;
	}

	CTextonView* pTView = (CTextonView*)pView;
	pCbckTempl = this;

	((CTextonView*)pView)->DrawPgm();

	m_ProgressText = "Deriving error map with static template...";
	m_templFixed = TRUE;
	m_positionFixed = FALSE;

	UpdateData(FALSE);

	m_TemplateMatcher.m_errorThres = m_errorThres;
	m_TemplateMatcher.m_trainANG = m_trainANG;
	m_TemplateMatcher.m_templFixed = m_templFixed;
	m_TemplateMatcher.b_saveCalls = m_bSaveCallParms;
	m_TemplateMatcher.m_weigthMH = m_weigthMH;
	m_TemplateMatcher.m_weightINT = m_weightINT;
	m_TemplateMatcher.m_weigthWP = m_weigthWP;
	m_TemplateMatcher.m_trainWP = (m_trainWP/m_tmplDOWNF)<1?1:m_trainWP/m_tmplDOWNF;
	m_TemplateMatcher.m_trainLW = (m_trainLW/m_tmplDOWNF)<2?2:m_trainLW/m_tmplDOWNF;
	m_TemplateMatcher.m_trainLH = (m_trainLH/m_tmplDOWNF)<1?1:m_trainLH/m_tmplDOWNF;
	m_TemplateMatcher.m_weigthLW = m_weigthLW;
	m_TemplateMatcher.m_weigthLH = m_weigthLH;
	m_TemplateMatcher.iInitAngle = m_initAngle;
	m_TemplateMatcher.m_initG = m_initG;
	m_TemplateMatcher.m_initLH = (m_initLH/m_tmplDOWNF)<1?1:m_initLH/m_tmplDOWNF;
	m_TemplateMatcher.m_initLW = (m_initLW/m_tmplDOWNF)<2?2:m_initLW/m_tmplDOWNF;
	m_TemplateMatcher.m_initWP = (m_initWP/m_tmplDOWNF)<1?11:m_initWP/m_tmplDOWNF;
	m_TemplateMatcher.m_initW = m_initW;
	m_TemplateMatcher.m_initAMAG = m_initAMAG;
	m_TemplateMatcher.m_initGMAG = m_initGMAG;
	m_TemplateMatcher.m_initLHMAG = m_initLHMAG;
	m_TemplateMatcher.m_initLWMAG = m_initLWMAG;
	m_TemplateMatcher.m_initWMAG = m_initWMAG;
	m_TemplateMatcher.m_initWPMAG = m_initWPMAG;
	m_TemplateMatcher.m_weightINT = m_weightINT;
	m_TemplateMatcher.m_weigthWP = m_weigthWP;
	m_TemplateMatcher.m_trainG = m_trainG;
	m_TemplateMatcher.m_trainW = m_trainW;
	m_TemplateMatcher.templ_xc = (m_initCX/m_tmplDOWNF)<1?1:m_initCX/m_tmplDOWNF;
	m_TemplateMatcher.templ_yc = (m_initCY/m_tmplDOWNF)<1?1:m_initCY/m_tmplDOWNF;

	m_TemplateMatcher.match_main(image, width, height,
		callback_CDlgTemplateMatch, m_positionFixed, m_bSaveCallParms);

	m_DrawFitCtrl.EnableWindow(FALSE);
	m_DrawErrorCtrl.EnableWindow(TRUE);
	UpdateData(FALSE);

}

void CDlgTemplateMatch::CoordsSmallToLarge(float * xpos, float * ypos, int scale, int classes)
{
	int i;
	double dmy;

	for(i=0; i<classes; i++)
	{
		xpos[i] = (modf(xpos[i]*(float)scale,&dmy)>0.5?ceil(xpos[i]*(float)scale):floor(xpos[i]*(float)scale));
		ypos[i] = (modf(ypos[i]*(float)scale,&dmy)>0.5?ceil(ypos[i]*(float)scale):floor(ypos[i]*(float)scale));
	}
}

int ClusterRight(int * error_map, int width, int height, int * class_map, int* rx, const int y)
{
	int x = *rx;
	int current_class;

	if(error_map[width*y+x]==1)
		current_class = class_map[width*y+x];
	else
		AfxMessageBox("Error in ClusterRight(): called with bad x,y coordinates.");

	for( ; x<width; x++)
		if(error_map[width*y+x] == 1)
		{
			if((y-1)>0)
				if(error_map[width*(y-1)+x] == 1)
					break;
		}
		else
			break;

	// we reached end of row and there are no points neighbouring the row above
	if(current_class == -1 && (x>=width))
		return -1;

	// we reached end of connected row and there are no points neighbouring the row above
	if((y-1)>0)
		if(current_class == -1 && error_map[width*(y-1)+x]!=1)
			return -1;

	// the first neighbouring point we found must have a class
	// so find this class and mark all points between this one (inclusive)
	// and that neighbouring point as that point's class
	if((y-1)>0 && (x<width))
		if(error_map[width*(y-1)+x] == 1)
			current_class = class_map[width*(y-1)+x];

	int xx;
	for(xx=*rx; xx<=x && xx<width; xx++)
		if(error_map[width*y+xx]==1)
			class_map[width*y+xx] = current_class;

	if(error_map[width*y+x]==1)
		*rx = x;
	else
		*rx = x-1;

	return current_class;

}

void floodFill8(int *image, int width, int height, int x, int y, int fill, int old)
{
	// adapted from http://www.cs.unc.edu/~mcmillan/comp136/Lecture8/areaFills.html
	// by Leonard McMillan, University of North Carolina at Chapel Hill.

	if ((x < 0) || (x >= width)) return;
	if ((y < 0) || (y >= height)) return;
	if (image[width*y+x] == old) {
		image[width*y+x] = fill;
		floodFill8(image, width, height, x+1, y, fill, old);
		floodFill8(image, width, height, x, y+1, fill, old);
		floodFill8(image, width, height, x-1, y, fill, old);
		floodFill8(image, width, height, x, y-1, fill, old);
		floodFill8(image, width, height, x+1, y+1, fill, old);
		floodFill8(image, width, height, x-1, y+1, fill, old);
		floodFill8(image, width, height, x-1, y-1, fill, old);
		floodFill8(image, width, height, x+1, y-1, fill, old);
	}
}

void ClusterImage(int * class_no, int * error_map, int width, int height, int * class_map)
{
	int x, y;
	int current_class = 0;

	for(x=0; x<width; x++)
		for(y=0; y<height; y++)
			class_map[width*y+x] = (error_map[width*y+x]==1)?-1:-2;


	for(x=0; x<width; x++)
		for(y=0; y<height; y++)
		{
			// if point isn't clustered, cluster it
			if(error_map[width*y+x] == 1)
				if(class_map[width*y+x] == -1)
				{
					floodFill8(class_map, width, height, x, y, current_class, -1);

					// increment class count
					current_class++;
				}
		}

	for(x=0; x<width; x++)
		for(y=0; y<height; y++)
			class_map[width*y+x] = (class_map[width*y+x]==-2)?-1:class_map[width*y+x];

	*class_no = current_class;

}

void ClusterPointOld(int * class_no, int * error_map, int width, int height, int * class_map, int x, int y)
{
	// ********************* OBSOLETE ****** DOES NOT WORK ***********************************
	// this function is designed to be called from a loop that's scanning the image one row at a
	// time i.e.
	//
	// for(y=0; y<width; y++)
	//	for(x=0; x<height; x++)
	//		if(error_map[width*y+x]==1)
	//			ClusterPoint(&class_count, error_map, width, height, classMap, x, y);
	//
	//
	
	int nx, ny; // neighbour coordinates
	int current_class;

	if(error_map[width*y+x]==1)
	{
		// mark point if unmarked
		if(class_map[width*y+x]==-1)
		{
			if((x+1)<width)
			{
				if(class_map[width*y+(x+1)]==-1)
				{
					// point is unmarked, so is 3 o'clock neighbour, so mark point as new class
					// BLOCK A: MATCH CODE to block B below
					class_map[width*y+x]=*class_no;
					current_class = *class_no;
					(*class_no)++;
				}
				else
				{
					// mark point same class as its three o'clock neighbour
					current_class = class_map[width*y+(x+1)];
					class_map[width*y+x]=current_class;
				}
			}
			else
			{
				// point is unmarked, there's no 3 o'clock neighbour, so mark point as new class
				// BLOCK B: MATCH CODE to block A above
				class_map[width*y+x]=*class_no;
				current_class = *class_no;
				(*class_no)++;
			}

		}
		else
			current_class = class_map[width*y+x]; // point already marked
	}
	else
		return;  // point is not marked as having been thresholded
	
	// mark point's sequential eight-neighbours as same class
	// i.e. we're only marking:
	//
	//		      P --> *
	//			/ | \
	//		  /   |   \
	//		*	  *     *
	//
	//

	// mark three o'clock neighbour
	if((x+1)<width)
		if(error_map[width*y+(x+1)]==1)
			class_map[width*y+(x+1)]=current_class;

	// mark five o'clock neighbour
	if((y+1)<height && (x+1)<width)
		if(error_map[width*(y+1)+(x+1)]==1)
			class_map[width*(y+1)+(x+1)]=current_class;

	// mark six o'clock neighbour
	if((y+1)<height)
		if(error_map[width*(y+1)+x]==1)
			class_map[width*(y+1)+x]=current_class;

	// mark seven o'clock neighbour
	if((y+1)<height && (x-1)>=0)
		if(error_map[width*(y+1)+(x-1)]==1)
			class_map[width*(y+1)+(x-1)]=current_class;


}

void FindClassCentroid(int class_no, int point_count, int* class_map, 
					   int width, int height, float* xpos, float* ypos, 
					   int pos_no)
{
	int x, y;

	// go through class map
	float mean_x=0.0;
	float mean_y=0.0;

	for(x=0; x<width; x++)
		for(y=0; y<height; y++)
			if(class_map[width*y+x]==class_no)
			{	
				mean_x = mean_x + (float)x / (float)point_count;
				mean_y = mean_y + (float)y / (float)point_count;
			}

	xpos[pos_no] = mean_x;
	ypos[pos_no] = mean_y;

}

void PickTopClasses(int* top_classes, int* class_points, int class_count, int clusters)
{
	int class_no, class_max;
	int cur_max_points, cur_max_class;
	int old_max_points, old_max_class;

	// there are more classes than clusters so we need to sort best classes into cluster slots
	// find first top class
	cur_max_class = 0;
	cur_max_points = class_points[cur_max_class];
	for(class_max = 1; class_max<class_count; class_max++)
		if(class_points[class_max] > cur_max_points)
		{
			cur_max_points = class_points[class_max];
			cur_max_class = class_max;
		}

	top_classes[0] = cur_max_class;

	for(class_no=1; class_no<clusters; class_no++)
	{
		// for each slot in top_classes, search class_points for the next maximum and set entry
		old_max_points = cur_max_points;
		old_max_class = cur_max_class;

		cur_max_class = 0;
		cur_max_points = 0;

		for(class_max = 0; class_max < class_count; class_max++)
			if(class_points[class_max] > cur_max_points && 
				class_points[class_max] <= old_max_points)
				if(class_max != old_max_class)
				{
					cur_max_class = class_max;
					cur_max_points = class_points[class_max];
				}

		top_classes[class_no] = cur_max_class;
	}
}

void CDlgTemplateMatch::FindCandidateClusters(int * error_map, int width, int height, float** rxpos, 
											  float** rypos, int* clusters, int* total_clusters)
{
	int x, y;
	int class_count;
	int* classMap = new int[width*height];

	class_count = 0;
	
	// go through error map, cluster points and count classes
	ClusterImage(&class_count, error_map, width, height, classMap);

	// uncomment to save class map
	/*
	CImagePgm toSave;
	toSave.imgData = classMap;
	toSave.imgWidth = width;
	toSave.imgHeight = height;
	toSave.SaveImagePgm("ClassMap.pgm", 100);
	char sz266[100];
	sprintf(sz266, "ClassMap.pgm saved. There are %i classes", class_count);
	AfxMessageBox(sz266);
	toSave.imgData = NULL;
	*/
		
	// allocate memory for coordinates
	if(class_count < *clusters)
		*clusters = class_count;

	float* xpos = new float[*clusters];
	float* ypos = new float[*clusters];

	// FOR DEBUG
	for(x=0;x<(*clusters); x++)
		xpos[x]=0.0;

	for(x=0;x<(*clusters); x++)
		ypos[x]=0.0;

	int* top_classes = new int[*clusters];

	// if we have more classes than the user asked for, we only return
	// the classes with the most points in them
	int* class_points = new int[class_count];

	// count points in each class
	for(x=0; x<class_count; x++)
		class_points[x]=0;
	
	for(x=0; x<width; x++)
		for(y=0; y<height; y++)
			if(classMap[width*y+x]>=0 && classMap[width*y+x]<class_count)
				class_points[classMap[width*y+x]]++;

	*total_clusters = class_count;

	if(class_count > *clusters)
	{
		// choose classes with the most points
		PickTopClasses(top_classes, class_points, class_count, *clusters);

	}
	else
	{
		// otherwise we process all available classes
		int i;
		for(i=0; i<*clusters; i++)
			top_classes[i]=i;
	}

	// find centroid for each class
	int iClass;
	for(iClass=0; iClass<*clusters; iClass++)
		FindClassCentroid(top_classes[iClass], class_points[top_classes[iClass]], classMap, 
		width, height, xpos, ypos, iClass);

	// the end
	delete class_points;
	delete classMap;
	delete top_classes;

	*rxpos = xpos;
	*rypos = ypos;


}


void CDlgTemplateMatch::OnTmplDrawori() 
{
	((CTextonView*)pView)->DrawPgm();	
}

void CDlgTemplateMatch::OnDrawThres() 
{
	UpdateData(TRUE);
	if(binErrorMap!=NULL)
	{
		CTextonView* pTView = (CTextonView*)pView;

		// draw error image
		int x,y;

		// allocate destination memory
		int imgHeight = ((CTextonView*)pView)->pgmImage.imgHeight;
		int imgWidth = ((CTextonView*)pView)->pgmImage.imgWidth;
		
		if(((CTextonView*)pView)->testImage.imgData != NULL)
			delete ((CTextonView*)pView)->testImage.imgData;
		
		((CTextonView*)pView)->testImage.imgData = new int[imgWidth*imgHeight];
		((CTextonView*)pView)->testImage.imgWidth = imgWidth;
		((CTextonView*)pView)->testImage.imgHeight = imgHeight;

		// set destination to white
		for(x = 0; x< imgWidth; x++)
			for(y = 0; y<imgHeight; y++)
				((CTextonView*)pView)->testImage.imgData[imgWidth*y+x] = 255;
		
		// copy error map to destination
		for(x = m_TemplateMatcher.errorCanvas; x< imgWidth-m_TemplateMatcher.errorCanvas; x++)
			for(y = m_TemplateMatcher.errorCanvas; y<imgHeight-m_TemplateMatcher.errorCanvas; y++)
				((CTextonView*)pView)->testImage.imgData[imgWidth*y+x] = 
				binErrorMap[imgWidth*y+x]==1?0:255;
			
		((CTextonView*)pView)->DrawTest();

		// uncomment to save image
		/*
		CImagePgm toSave;
		toSave.imgData = ((CTextonView*)pView)->testImage.imgData;
		toSave.imgWidth = imgWidth;
		toSave.imgHeight = imgHeight;
		toSave.SaveImagePgm("TestMap.pgm", 1);
		AfxMessageBox("TestMap.pgm saved.");
		toSave.imgData = NULL;
		*/

		
	}
		
}

void CDlgTemplateMatch::OnCancel() 
{
	((CTextonView*)pView)->DrawPgm();
	CDialog::OnCancel();
}

