// DlgVolView.cpp : implementation file
//

#include "stdafx.h"
#include "texton.h"
#include "DlgVolView.h"
#include <math.h>
#include "TextonView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define PLANE_X 1
#define PLANE_Y 2
#define PLANE_S 3


/////////////////////////////////////////////////////////////////////////////
// CDlgVolView dialog


CDlgVolView::CDlgVolView(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgVolView::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgVolView)
	m_SelS = 1;
	m_SelX = 0;
	m_SelY = 0;
	m_imgInfo = _T("");
	m_strVolValue = _T("click image to view value");
	m_sVolumeTitle = _T("");
	m_intMult = 1.0f;
	m_minMaxText = _T("");
	//}}AFX_DATA_INIT

	m_floatRawVolData = NULL;
	m_floatNormVolData = NULL;
	m_intRawVolData = NULL;
	m_intNormVolData = NULL;
	bTakeMouse = FALSE;
	m_colourVolData = NULL;
	iCurrentPlane = PLANE_S;
	iCurrentS = 0;
	iCurrentY = 0;
	iCurrentX = 0;

}

void CDlgVolView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgVolView)
	DDX_Text(pDX, IDC_S_SEL, m_SelS);
	DDX_Text(pDX, IDC_X_SEL, m_SelX);
	DDX_Text(pDX, IDC_Y_SEL, m_SelY);
	DDX_Text(pDX, IDC_STATIC_IMGINFO, m_imgInfo);
	DDX_Text(pDX, IDC_STATIC_VOXVAL, m_strVolValue);
	DDX_Text(pDX, IDC_STATIC_VOLTITLE, m_sVolumeTitle);
	DDX_Text(pDX, IDC_VOLVIEW_MULTIPLIER, m_intMult);
	DDX_Text(pDX, IDC_VOLVIEW_MINMAX, m_minMaxText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgVolView, CDialog)
	//{{AFX_MSG_MAP(CDlgVolView)
	ON_BN_CLICKED(IDC_S_MINUS, OnSMinus)
	ON_BN_CLICKED(IDC_S_PLUS, OnSPlus)
	ON_BN_CLICKED(IDC_X_MINUS, OnXMinus)
	ON_BN_CLICKED(IDC_X_PLUS, OnXPlus)
	ON_BN_CLICKED(IDC_Y_MINUS, OnYMinus)
	ON_BN_CLICKED(IDC_Y_PLUS, OnYPlus)
	ON_BN_CLICKED(IDC_VOLVIEW_SELECT, OnVolviewSelectWindow)
	ON_BN_CLICKED(IDC_VOLVIEW_SAVE, OnVolviewSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgVolView message handlers

void CDlgVolView::OnSMinus() 
{
	UpdateData(TRUE);
	if(m_SelS <= 1 || m_SelS > img_slices)
		m_SelS = img_slices+1;

	m_SelS--;

	if(m_colourVolData == NULL)
	{
		int* slice2D = new int[img_width*img_height];

		int x, y, z;
		z = m_SelS-1;
		for(x=0; x<img_width; x++)
			for(y=0; y<img_height; y++)
				slice2D[img_width*y+x] =
					FloatToInt((float)m_intNormVolData[z*img_width*img_height+y*img_width+x]*m_intMult);

		((CTextonView*)pView)->DrawResultSpecial(slice2D, img_width, img_height);

		delete [] slice2D;
	}
	else
	{
		CImageColour* slice2D = new CImageColour;
		slice2D->AllocateData(img_width, img_height);

		int x, y, z;
		z = m_SelS-1;
		for(x=0; x<img_width; x++)
			for(y=0; y<img_height; y++)
				slice2D->imgData[img_width*y+x] =
					m_colourVolData->imgData[z*img_width*img_height+y*img_width+x];

		((CTextonView*)pView)->DrawColourImageSpecial(slice2D);

		delete slice2D;
	}

	UpdateData(FALSE);

	iCurrentPlane = PLANE_S;
	iCurrentS = m_SelS;
	bTakeMouse = TRUE;


}

void CDlgVolView::OnSPlus() 
{
	UpdateData(TRUE);
	if(m_SelS < 1 || m_SelS >= img_slices)
		m_SelS = 0;

	m_SelS++;

	if(m_colourVolData == NULL)
	{
		int* slice2D = new int[img_width*img_height];

		int x, y, z;
		z = m_SelS-1;
		for(x=0; x<img_width; x++)
			for(y=0; y<img_height; y++)
				slice2D[img_width*y+x] =
					FloatToInt((float)m_intNormVolData[z*img_width*img_height+y*img_width+x]*m_intMult);

		((CTextonView*)pView)->DrawResultSpecial(slice2D, img_width, img_height);

		delete [] slice2D;
	}
	else
	{
		CImageColour* slice2D = new CImageColour;
		slice2D->AllocateData(img_width, img_height);

		int x, y, z;
		z = m_SelS-1;
		for(x=0; x<img_width; x++)
			for(y=0; y<img_height; y++)
				slice2D->imgData[img_width*y+x] =
					m_colourVolData->imgData[z*img_width*img_height+y*img_width+x];

		((CTextonView*)pView)->DrawColourImageSpecial(slice2D);

		delete slice2D;
	}

	UpdateData(FALSE);
	iCurrentPlane = PLANE_S;
	iCurrentS = m_SelS;
	bTakeMouse = TRUE;

}

void CDlgVolView::OnXMinus() 
{
	UpdateData(TRUE);
	if(m_SelX <= 0 || m_SelX > img_width-1)
		m_SelX = img_width;

	m_SelX--;

	if(m_colourVolData == NULL)
	{
		int* slice2D = new int[img_slices*img_height];

		int x, y, z;
		x = m_SelX;
		for(z=0; z<img_slices; z++)
			for(y=0; y<img_height; y++)
				slice2D[img_slices*y+z] =
					FloatToInt((float)m_intNormVolData[z*img_width*img_height+y*img_width+x]*m_intMult);

		((CTextonView*)pView)->DrawResultSpecial(slice2D, img_slices, img_height);

		delete [] slice2D;
	}
	else
	{
		CImageColour* slice2D = new CImageColour;
		slice2D->AllocateData(img_slices, img_height);

		int x, y, z;
		x = m_SelX;
		for(z=0; z<img_slices; z++)
			for(y=0; y<img_height; y++)
				slice2D->imgData[img_slices*y+z] =
					m_colourVolData->imgData[z*img_width*img_height+y*img_width+x];

		((CTextonView*)pView)->DrawColourImageSpecial(slice2D);

		delete slice2D;
	}

	UpdateData(FALSE);
	iCurrentPlane = PLANE_X;
	iCurrentX = m_SelX;
	bTakeMouse = TRUE;
	
}

void CDlgVolView::OnXPlus() 
{
	UpdateData(TRUE);
	if(m_SelX < 0 || m_SelX >= img_width-1)
		m_SelX = -1;

	m_SelX++;

	if(m_colourVolData == NULL)
	{
		int* slice2D = new int[img_slices*img_height];

		int x, y, z;
		x = m_SelX;
		for(z=0; z<img_slices; z++)
			for(y=0; y<img_height; y++)
				slice2D[img_slices*y+z] =
					FloatToInt((float)m_intNormVolData[z*img_width*img_height+y*img_width+x]*m_intMult);

		((CTextonView*)pView)->DrawResultSpecial(slice2D, img_slices, img_height);

		delete [] slice2D;
	}
	else
	{
		CImageColour* slice2D = new CImageColour;
		slice2D->AllocateData(img_slices, img_height);

		int x, y, z;
		x = m_SelX;
		for(z=0; z<img_slices; z++)
			for(y=0; y<img_height; y++)
				slice2D->imgData[img_slices*y+z] =
					m_colourVolData->imgData[z*img_width*img_height+y*img_width+x];

		((CTextonView*)pView)->DrawColourImageSpecial(slice2D);

		delete slice2D;
	}

	UpdateData(FALSE);
	iCurrentPlane = PLANE_X;
	iCurrentX = m_SelX;
	bTakeMouse = TRUE;

}

void CDlgVolView::OnYMinus() 
{
	UpdateData(TRUE);
	if(m_SelY <= 0 || m_SelY > img_height-1)
		m_SelY = img_height;

	m_SelY--;

	if(m_colourVolData == NULL)
	{
		int* slice2D = new int[img_width*img_slices];

		int x, y, z;
		y = m_SelY;
		for(z=0; z<img_slices; z++)
			for(x=0; x<img_width; x++)
				slice2D[img_slices*x+z] =
					FloatToInt((float)m_intNormVolData[z*img_width*img_height+y*img_width+x]*m_intMult);

		((CTextonView*)pView)->DrawResultSpecial(slice2D, img_slices, img_width);

		delete [] slice2D;
	}
	else
	{
		CImageColour* slice2D = new CImageColour;
		slice2D->AllocateData(img_width, img_slices);

		int x, y, z;
		y = m_SelY;
		for(z=0; z<img_slices; z++)
			for(x=0; x<img_width; x++)
				slice2D->imgData[img_slices*x+z] =
					m_colourVolData->imgData[z*img_width*img_height+y*img_width+x];

		((CTextonView*)pView)->DrawColourImageSpecial(slice2D);

		delete slice2D;
	}

	UpdateData(FALSE);
	iCurrentPlane = PLANE_Y;
	iCurrentY = m_SelY;
	bTakeMouse = TRUE;

}

void CDlgVolView::OnYPlus() 
{
	UpdateData(TRUE);
	if(m_SelY < 0 || m_SelY >= img_height-1)
		m_SelY = -1;

	m_SelY++;

	if(m_colourVolData == NULL)
	{
		int* slice2D = new int[img_width*img_slices];

		int x, y, z;
		y = m_SelY;
		for(z=0; z<img_slices; z++)
			for(x=0; x<img_width; x++)
				slice2D[img_slices*x+z] =
					FloatToInt((float)m_intNormVolData[z*img_width*img_height+y*img_width+x]*m_intMult);

		((CTextonView*)pView)->DrawResultSpecial(slice2D, img_slices, img_width);

		delete [] slice2D;
	}
	else
	{
		CImageColour* slice2D = new CImageColour;
		slice2D->AllocateData(img_width, img_slices);

		int x, y, z;
		y = m_SelY;
		for(z=0; z<img_slices; z++)
			for(x=0; x<img_width; x++)
				slice2D->imgData[img_slices*x+z] =
					m_colourVolData->imgData[z*img_width*img_height+y*img_width+x];

		((CTextonView*)pView)->DrawColourImageSpecial(slice2D);

		delete slice2D;
	}

	UpdateData(FALSE);
	iCurrentPlane = PLANE_Y;
	iCurrentY = m_SelY;
	bTakeMouse = TRUE;

}

void CDlgVolView::InitialiseVolume(int* init_vol, int width, int height, int slices)
{
	img_width = width;
	img_height = height;
	img_slices = slices;
	
	if(m_intNormVolData != NULL)
		delete m_intNormVolData;

	if(m_intRawVolData != NULL)
		delete m_intRawVolData;

	if(m_floatNormVolData != NULL)
		delete m_floatNormVolData;

	if(m_floatRawVolData != NULL)
		delete m_floatRawVolData;

	if(m_colourVolData != NULL)
		delete m_colourVolData;
	m_colourVolData = NULL;

	m_intNormVolData = new int[img_width*img_height*img_slices];
	m_intRawVolData = new int[img_width*img_height*img_slices];

	int x, y, z;
	for(x=0; x<img_width;x++)
		for(y=0; y<img_height; y++)
			for(z=0; z<img_slices; z++)
				m_intRawVolData[z*img_width*img_height+y*img_width+x] =
				init_vol[z*img_width*img_height+y*img_width+x];

	NormaliseVolume();

	m_floatNormVolData = NULL;
	m_floatRawVolData = NULL;
	bTakeMouse = FALSE;
	((CTextonView*)pView)->pVolView = this;

}

void CDlgVolView::InitialiseVolume(float* init_vol, int width, int height, int slices)
{
	img_width = width;
	img_height = height;
	img_slices = slices;
	
	if(m_intNormVolData != NULL)
		delete m_intNormVolData;

	if(m_intRawVolData != NULL)
		delete m_intRawVolData;

	if(m_floatNormVolData != NULL)
		delete m_floatNormVolData;

	if(m_floatRawVolData != NULL)
		delete m_floatRawVolData;

	if(m_colourVolData != NULL)
		delete m_colourVolData;
	m_colourVolData = NULL;

	m_intNormVolData = new int[img_width*img_height*img_slices];
	m_intRawVolData = new int[img_width*img_height*img_slices];
	m_floatNormVolData = new float[img_width*img_height*img_slices];
	m_floatRawVolData = new float[img_width*img_height*img_slices];

	int x, y, z;
	for(x=0; x<img_width;x++)
		for(y=0; y<img_height; y++)
			for(z=0; z<img_slices; z++)
				m_floatRawVolData[z*img_width*img_height+y*img_width+x] =
					init_vol[z*img_width*img_height+y*img_width+x];

	NormaliseFloatVolume();

	for(x=0; x<img_width;x++)
		for(y=0; y<img_height; y++)
			for(z=0; z<img_slices; z++)
				m_intNormVolData[z*img_width*img_height+y*img_width+x] =
					FloatToInt(m_floatNormVolData[z*img_width*img_height+y*img_width+x]);

	bTakeMouse = FALSE;
	((CTextonView*)pView)->pVolView = this;

}

void CDlgVolView::InitialiseVolume(CImageColour& init_vol, int width, int height, int slices)
{
	img_width = width;
	img_height = height;
	img_slices = slices;
	
	if(m_intNormVolData != NULL)
		delete m_intNormVolData;

	if(m_intRawVolData != NULL)
		delete m_intRawVolData;

	if(m_floatNormVolData != NULL)
		delete m_floatNormVolData;

	if(m_floatRawVolData != NULL)
		delete m_floatRawVolData;

	if(m_colourVolData != NULL)
		delete m_colourVolData;
	m_colourVolData = NULL;

	m_colourVolData = new CImageColour;
	m_colourVolData->AllocateData3D(img_width, img_height, img_slices);

	int x, y, z;
	for(x=0; x<img_width;x++)
		for(y=0; y<img_height; y++)
			for(z=0; z<img_slices; z++)
				m_colourVolData->imgData[z*img_width*img_height+y*img_width+x] =
					init_vol.imgData[z*img_width*img_height+y*img_width+x];

	bTakeMouse = FALSE;
	((CTextonView*)pView)->pVolView = this;

}

void CDlgVolView::InitialiseVolume(short* init_vol, int width, int height, int slices)
{
	img_width = width;
	img_height = height;
	img_slices = slices;
	
	if(m_intNormVolData != NULL)
		delete m_intNormVolData;

	if(m_intRawVolData != NULL)
		delete m_intRawVolData;

	if(m_floatNormVolData != NULL)
		delete m_floatNormVolData;

	if(m_floatRawVolData != NULL)
		delete m_floatRawVolData;

	if(m_colourVolData != NULL)
		delete m_colourVolData;
	m_colourVolData = NULL;

	m_intNormVolData = new int[img_width*img_height*img_slices];
	m_intRawVolData = new int[img_width*img_height*img_slices];

	int x, y, z;
	for(x=0; x<img_width;x++)
		for(y=0; y<img_height; y++)
			for(z=0; z<img_slices; z++)
				m_intRawVolData[z*img_width*img_height+y*img_width+x] =
				init_vol[z*img_width*img_height+y*img_width+x];

	NormaliseVolume();

	m_floatNormVolData = NULL;
	m_floatRawVolData = NULL;
	bTakeMouse = FALSE;
	((CTextonView*)pView)->pVolView = this;

}

void CDlgVolView::NormaliseVolume()
{
	int min;
	int max;
	int x, y, z;

	min = max = m_intRawVolData[0];

	for(x=0; x<img_width;x++)
		for(y=0; y<img_height; y++)
			for(z=0; z<img_slices; z++)
			{
				if(m_intRawVolData[z*img_width*img_height+y*img_width+x]>max)
					max = m_intRawVolData[z*img_width*img_height+y*img_width+x];
				
				if(m_intRawVolData[z*img_width*img_height+y*img_width+x]<min)
					min = m_intRawVolData[z*img_width*img_height+y*img_width+x];
			}

	int range = max - min;
	float m, b;
	m=255.0f/range;
	b=255.0f-m*(float)max;

	for(x=0; x<img_width;x++)
		for(y=0; y<img_height; y++)
			for(z=0; z<img_slices; z++)
				m_intNormVolData[z*img_width*img_height+y*img_width+x] =
					FloatToInt( m_intRawVolData[z*img_width*img_height+y*img_width+x]*m+b+0.00001f );

	m_minMaxText.Format("Min: %i Max: %i", min, max);

}

int CDlgVolView::FloatToInt(float fnum)
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

void CDlgVolView::NormaliseFloatVolume()
{
	float min;
	float max;
	int x, y, z;

	min = max = m_floatRawVolData[0];

	for(x=0; x<img_width;x++)
		for(y=0; y<img_height; y++)
			for(z=0; z<img_slices; z++)
			{
				if(m_floatRawVolData[z*img_width*img_height+y*img_width+x]>max)
					max = m_floatRawVolData[z*img_width*img_height+y*img_width+x];
				
				if(m_floatRawVolData[z*img_width*img_height+y*img_width+x]<min)
					min = m_floatRawVolData[z*img_width*img_height+y*img_width+x];
			}

	float range = max - min;
	float m, b;
	m=255.0f/range;
	b=255.0f-m*(float)max;

	for(x=0; x<img_width;x++)
		for(y=0; y<img_height; y++)
			for(z=0; z<img_slices; z++)
				m_floatNormVolData[z*img_width*img_height+y*img_width+x] =
					m_floatRawVolData[z*img_width*img_height+y*img_width+x]*m+b;


	m_minMaxText.Format("Min: %.3f Max: %.3f", min, max);

}

void CDlgVolView::TakeMovePoint(CPoint point)
{
	if(!bTakeMouse)
		return;

	// get volume coordinates from current projection
	int x, y, s;
	switch(iCurrentPlane)
	{
	case PLANE_X:
		x = iCurrentX;
		y = point.y;
		s = point.x;
		break;
	case PLANE_Y:
		y = iCurrentY;
		x = point.y;
		s = point.x;
		break;
	case PLANE_S:
		s = iCurrentS;
		y = point.y;
		x = point.x;
		break;
	default:
		AfxMessageBox("Internal error: bad iCurrentPlane.");
		return;
	}

	if((s-1)<0 || (s-1)>=img_slices || x<0 || x >= img_width || y<0 || y>= img_height)
		return;

	UpdateData(TRUE);

	// format string with coordinates and voxel values
	if(m_floatRawVolData==NULL)
		m_strVolValue.Format("(x, y, s) : (%i, %i, %i) int value: %i", x, y, s, 
		m_intRawVolData[(s-1)*img_width*img_height+y*img_width+x]);
	else
		m_strVolValue.Format("(x, y, s) : (%i, %i, %i) float value: %f", x, y, s, 
		m_floatRawVolData[(s-1)*img_width*img_height+y*img_width+x]);

	UpdateData(FALSE);
	
}

void CDlgVolView::OnCancel() 
{

	if(m_floatRawVolData != NULL)
		delete m_floatRawVolData;

	if(m_floatNormVolData != NULL)
		delete m_floatNormVolData;
	
	if(m_intRawVolData != NULL)
		delete m_intRawVolData;

	if(m_intNormVolData != NULL)
		delete m_intNormVolData;
	
	((CTextonView*)pView)->pVolView = NULL;

	CDialog::OnCancel();
}

void CDlgVolView::OnVolviewSelectWindow() 
{
	((CTextonView*)pView)->pVolView = this;
}

void CDlgVolView::GetMouseProjection(int &x, int&y, int& s, CPoint point)
{
	switch(iCurrentPlane)
	{
	case PLANE_X:
		x = iCurrentX;
		y = point.y;
		s = point.x;
		break;
	case PLANE_Y:
		y = iCurrentY;
		x = point.y;
		s = point.x;
		break;
	case PLANE_S:
		s = iCurrentS;
		y = point.y;
		x = point.x;
		break;
	default:
		AfxMessageBox("Internal error: bad iCurrentPlane.");
		return;
	}


	
}

void CDlgVolView::OnVolviewSave() 
{
	((CTextonView*)pView)->SaveResult();
}

void CDlgVolView::GetCurrentPlane(char &cPlane, int& zero_based)
{
	switch(iCurrentPlane)
	{
	case PLANE_X:
		cPlane = 'x';
		zero_based = iCurrentX;
		break;
	case PLANE_Y:
		cPlane = 'y';
		zero_based = iCurrentY;
		break;
	case PLANE_S:
		cPlane = 's';
		zero_based = iCurrentS;
		break;
	default:
		cPlane = 'e';
		AfxMessageBox("Internal error: bad iCurrentPlane.");
		return;
	}
}
