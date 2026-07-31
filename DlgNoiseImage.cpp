// DlgNoiseImage.cpp : implementation file
//

#include "stdafx.h"
#include "Texton.h"
#include "DlgNoiseImage.h"
#include <ipl.h>
#include "TextonView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgNoiseImage dialog


CDlgNoiseImage::CDlgNoiseImage(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNoiseImage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgNoiseImage)
	m_Seed = 0;
	m_Std = 0;
	m_Mean = 0;
	//}}AFX_DATA_INIT

}


void CDlgNoiseImage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgNoiseImage)
	DDX_Text(pDX, IDC_NOISE_SEED, m_Seed);
	DDX_Text(pDX, IDC_NOISE_STD, m_Std);
	DDX_Text(pDX, IDC_NOISE_MEAN, m_Mean);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgNoiseImage, CDialog)
	//{{AFX_MSG_MAP(CDlgNoiseImage)
	ON_BN_CLICKED(IDC_NOISE_GO, OnNoiseGo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgNoiseImage message handlers

void CDlgNoiseImage::OnNoiseGo() 
{
	UpdateData(TRUE);

	// check application status
	if( ((CTextonView*)pView)->pgmImage.imgData == NULL)
	{
		AfxMessageBox("Load an Original Image first.");
		return;
	}

	int img_width = ((CTextonView*)pView)->pgmImage.imgWidth;
	int img_height = ((CTextonView*)pView)->pgmImage.imgHeight;

	if(img_width == 0 || img_height == 0)
	{
		AfxMessageBox("Load an Original Image first.");
		return;
	}

	// create ipl source image and set it to zero
	IplImage *nsImage;
	nsImage = iplCreateImageHeader(1,0,IPL_DEPTH_8U,"GRAY","GRAY", 
		IPL_DATA_ORDER_PIXEL, IPL_ORIGIN_TL, IPL_ALIGN_DWORD,
		img_width, img_height, NULL, NULL, NULL, NULL );
	if( !nsImage ) return;
	iplAllocateImage( nsImage, 1, 0 ); //allocate all to zeros
	if( !nsImage->imageData ) return;

	// create noise image
	IplNoiseParam noiseParam;
	iplNoiseGaussianInit(&noiseParam, m_Seed, m_Mean, m_Std);
	IPLStatus result = iplNoiseImage(nsImage, &noiseParam);

	// display noise image
	int* dmy = new int[img_width*img_height];
	char buffer[4];
	int x, y;
	for(x=0;x<img_width;x++)
		for(y=0;y<img_height;y++)
		{
			iplGetPixel(nsImage, x, y, &buffer);
			dmy[img_width*y+x] = buffer[0];
		}
	
	((CTextonView*)pView)->DrawResult(dmy);

	// start DEBUG
	/*
	CImagePgm dmy2;
	dmy2.imgHeight = img_height;
	dmy2.imgWidth = img_width;
	dmy2.imgData = dmy;
	dmy2.SaveImagePgm("testnoise_347020.pgm");
	*/
	// end DEBUG

	delete dmy;

	iplDeallocateImage(nsImage);

}
