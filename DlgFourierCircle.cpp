// DlgFourierCircle.cpp : implementation file
//

#include "stdafx.h"
#include "Texton.h"
#include "DlgFourierCircle.h"
#include "TextonView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgFourierCircle dialog


CDlgFourierCircle::CDlgFourierCircle(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFourierCircle::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgFourierCircle)
	m_rotateAngle = 0;
	//}}AFX_DATA_INIT
}


void CDlgFourierCircle::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgFourierCircle)
	DDX_Text(pDX, IDC_FFT_DEGREES, m_rotateAngle);
	DDV_MinMaxInt(pDX, m_rotateAngle, 0, 359);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgFourierCircle, CDialog)
	//{{AFX_MSG_MAP(CDlgFourierCircle)
	ON_BN_CLICKED(IDC_FFT_CANNY, OnFftCanny)
	ON_BN_CLICKED(IDC_FFT_CANNYCOPY, OnFftCannycopy)
	ON_BN_CLICKED(IDC_FFT_FFT, OnFftFft)
	ON_BN_CLICKED(IDC_FFT_ROTATE, OnFftRotate)
	ON_BN_CLICKED(IDC_FFT_INVERSE, OnFftInverse)
	ON_BN_CLICKED(IDC_FFT_FFTROT, OnFftFftrot)
	ON_BN_CLICKED(IDC_FFT_ROTINVERSE, OnFftRotinverse)
	ON_BN_CLICKED(IDC_FFT_COMBINE, OnFftCombine)
	ON_BN_CLICKED(IDC_FFT_COMINV, OnFftCominv)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgFourierCircle message handlers

void CDlgFourierCircle::OnRotate() 
{
	UpdateData(TRUE);
	((CTextonView*)pView)->DrawRotated(m_rotateAngle);	
}

void CDlgFourierCircle::OnFftCanny() 
{
	if(!((CTextonView*)pView)->edgeImage)
	{
		AfxMessageBox("Please create an edge image first.");
		return;
	}

	imgWidth = ((CTextonView*)pView)->pgmImage.imgWidth;
	imgHeight = ((CTextonView*)pView)->pgmImage.imgHeight;

	int* img_canny = new int[imgWidth*imgHeight];

	fftWorker.SetCanny(((CTextonView*)pView)->edgeImage, imgWidth, imgHeight);

	fftWorker.GetCanny(img_canny);

	((CTextonView*)pView)->DrawResult(img_canny);

	delete img_canny;

	GetDlgItem(IDC_FFT_FFT)->EnableWindow();


}

void CDlgFourierCircle::OnFftCannycopy() 
{
	((CTextonView*)pView)->CopyOriginalToCanny();	

	imgWidth = ((CTextonView*)pView)->pgmImage.imgWidth;
	imgHeight = ((CTextonView*)pView)->pgmImage.imgHeight;

	int* img_canny = new int[imgWidth*imgHeight];

	fftWorker.SetCanny(((CTextonView*)pView)->edgeImage, imgWidth, imgHeight);

	fftWorker.GetCanny(img_canny);

	((CTextonView*)pView)->DrawResult(img_canny);

	delete img_canny;

	GetDlgItem(IDC_FFT_FFT)->EnableWindow();

}

void CDlgFourierCircle::OnFftFft() 
{

	int* img_fft;

	img_fft = new int[imgWidth*imgHeight];

	fftWorker.StepFFT();

	fftWorker.GetDataFFT(img_fft);

	((CTextonView*)pView)->DrawResult(img_fft);

	delete img_fft;

	GetDlgItem(IDC_FFT_INVERSE)->EnableWindow();
	GetDlgItem(IDC_FFT_ROTATE)->EnableWindow();
	GetDlgItem(IDC_FFT_DEGREES)->EnableWindow();
	GetDlgItem(IDC_FFT_ROTATE_STATIC1)->EnableWindow();
	GetDlgItem(IDC_FFT_ROTATE_STATIC2)->EnableWindow();
	
}

void CDlgFourierCircle::OnFftRotate() 
{
	UpdateData(TRUE);

	int* img_rot = new int[imgWidth*imgHeight];

	fftWorker.StepRotate(m_rotateAngle);

	fftWorker.GetDataRotated(img_rot);

	((CTextonView*)pView)->DrawResult(img_rot);

	delete img_rot;

	GetDlgItem(IDC_FFT_COMBINE)->EnableWindow();
	
}

void CDlgFourierCircle::OnFftInverse() 
{
	int* img_fft = new int[imgWidth*imgHeight];

	fftWorker.InverseFFT();

	fftWorker.GetDataInvFFT(img_fft);

	((CTextonView*)pView)->DrawResult(img_fft);

	delete img_fft;
		
}


void CDlgFourierCircle::OnFftFftrot() 
{
	int* img_fft = new int[imgWidth*imgHeight];

	fftWorker.StepFFTRotated();

	fftWorker.GetDataFFTRotated(img_fft);

	((CTextonView*)pView)->DrawResult(img_fft);

	delete img_fft;


}

void CDlgFourierCircle::OnFftRotinverse() 
{
	int* img_fft = new int[imgWidth*imgHeight];

	fftWorker.InverseRotatedFFT();

	fftWorker.GetDataInvRotatedFFT(img_fft);

	((CTextonView*)pView)->DrawResult(img_fft);

	delete img_fft;
	
}

void CDlgFourierCircle::OnFftCombine() 
{
	int* combined = new int[imgWidth*imgHeight];

	fftWorker.StepCombine();

	fftWorker.GetDataCombined(combined);

	((CTextonView*)pView)->DrawResult(combined);

	delete combined;
	
}

void CDlgFourierCircle::OnFftCominv() 
{
	int* combined = new int[imgWidth*imgHeight];

	fftWorker.StepInvert();

	fftWorker.GetDataInverted(combined);

	((CTextonView*)pView)->DrawResult(combined);

	delete combined;
		
}
