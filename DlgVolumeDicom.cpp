// DlgVolumeDicom.cpp : implementation file
//

#include "stdafx.h"
#include "texton.h"
#include "DlgVolumeDicom.h"
#include "DlgFindVortex.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgVolumeDicom dialog


CDlgVolumeDicom::CDlgVolumeDicom(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgVolumeDicom::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgVolumeDicom)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgVolumeDicom::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgVolumeDicom)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgVolumeDicom, CDialog)
	//{{AFX_MSG_MAP(CDlgVolumeDicom)
	ON_BN_CLICKED(IDC_DICOM_SAVE_GO, OnDicomSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgVolumeDicom message handlers

void CDlgVolumeDicom::OnDicomSave() 
{
	AfxMessageBox("Please select the directory to save the file to.\nThe files will be named \"0001.dcm\", \"0002.dcm\", etc.");

	CFileDialog* dDlg = new CFileDialog(FALSE,"dcm","0001",OFN_CREATEPROMPT |
		OFN_HIDEREADONLY | OFN_NOREADONLYRETURN,
		_T("DICOM files (*.dcm)|*.dcm||"),
		NULL);
	if(dDlg->DoModal()!=IDOK)
		return;

	SaveDicomFile(dDlg->GetPathName());

	delete dDlg;
	
}

int CDlgVolumeDicom::FloatToInt(float fnum)
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


void CDlgVolumeDicom::SaveDicomFile(CString filename)
{

	// load DICOM header source file
	CString srcPath = AfxGetApp()->m_pszHelpFilePath;
	srcPath = srcPath.Left(srcPath.ReverseFind('\\')+1);
	srcPath += "texton.dcm";

	FILE* srcFile;
	srcFile = fopen(srcPath, "rb");

	// load target file
	CString trgPath;
	char trgFileName[100];
	int trgFileIdx;
	FILE* trgFile;
	const int header_length = 0x7AE;
	unsigned char* szCopyChar = (unsigned char*)malloc(header_length);	
	fread(szCopyChar, header_length, 1, srcFile);
	fclose(srcFile);
	float* volData;
	int x, y, s, index;
	unsigned char volDataChar;
	int volDataBuf;

	// compute volume data
	volData = ((CDlgFindVortex*)pFindVortex)->SeedMask();
	NormaliseFloat16bit(volData, img_width, img_height, img_slices);

	// for header changing
	char numBuf[10], numBuf2[10]; 
	int iNumBuf;

	for(trgFileIdx = 1; trgFileIdx<=img_slices; trgFileIdx++)
	{

		trgPath = filename.Left(filename.ReverseFind('\\')+1);
		sprintf(trgFileName, "%s%04i.dcm", trgPath, trgFileIdx);
		trgFile = fopen(trgFileName, "wb");

		// change header slice number tags
		const int InstanceNumberPos = 0x3F8;
		const int ImagePositionPos = 0x632;

		sprintf(numBuf, "%.3i", trgFileIdx);
		if(trgFileIdx<10)
			sprintf(numBuf2, "%i.00", trgFileIdx);
		else
			if(trgFileIdx<100)
				sprintf(numBuf2, "%i.0", trgFileIdx);
			else
				sprintf(numBuf2, "%i.", trgFileIdx);

		szCopyChar[InstanceNumberPos] = numBuf[0];
		szCopyChar[InstanceNumberPos+1] = numBuf[1];
		szCopyChar[InstanceNumberPos+2] = numBuf[2];

		szCopyChar[ImagePositionPos] = numBuf2[0];
		szCopyChar[ImagePositionPos+1] = numBuf2[1];
		szCopyChar[ImagePositionPos+2] = numBuf2[2];
		szCopyChar[ImagePositionPos+3] = numBuf2[3];

		//change header image sizes
		const int rows_pos = 0x496;
		const int cols_pos = 0x4A0;
		const int size_pos = 0x7AA;

		// image height
		iNumBuf = img_height;
		iNumBuf = (iNumBuf & 0xff);
		szCopyChar[cols_pos] = iNumBuf;
		iNumBuf = img_height;
		iNumBuf = iNumBuf>>8;
		szCopyChar[cols_pos+1] = iNumBuf;

		//image width
		iNumBuf = img_width;
		iNumBuf = (iNumBuf & 0xff);
		szCopyChar[rows_pos] = iNumBuf;
		iNumBuf = img_width;
		iNumBuf = iNumBuf>>8;
		szCopyChar[rows_pos+1] = iNumBuf;

		// image data length (4 bytes)
		iNumBuf = img_width*img_height*2;
		iNumBuf = (iNumBuf & 0xff);
		szCopyChar[size_pos] = iNumBuf;
		iNumBuf = img_width*img_height*2;
		iNumBuf = (iNumBuf>>8 & 0xff);
		szCopyChar[size_pos+1] = iNumBuf;
		iNumBuf = img_width*img_height*2;
		iNumBuf = iNumBuf>>16;
		iNumBuf = (iNumBuf & 0xff);
		szCopyChar[size_pos+2] = iNumBuf;
		iNumBuf = img_width*img_height*2;
		iNumBuf = iNumBuf>>24;
		iNumBuf = (iNumBuf & 0xff);
		szCopyChar[size_pos+3] = iNumBuf;
/*		iNumBuf = img_width*img_height*2;
		iNumBuf = iNumBuf>>32;
		iNumBuf = (iNumBuf & 0xff);
		szCopyChar[size_pos+4] = iNumBuf;
		iNumBuf = img_width*img_height*2;
		iNumBuf = iNumBuf>>40;
		iNumBuf = (iNumBuf & 0xff);
		szCopyChar[size_pos+5] = iNumBuf;
*/
		// copy header from source to target file
		fwrite(szCopyChar, header_length, 1, trgFile);

		//write volume data to target file
		s = trgFileIdx-1;
		for(x = 0; x<img_width; x++)
			for(y = 0; y<img_height; y++)
			{
				index = s*img_height*img_width + y*img_width + x;

				volDataBuf = (int)volData[index];
					
				// save the 2 bytes
				volDataChar = (volDataBuf & 0xff);
				fputc(volDataChar, trgFile);
				volDataBuf = (int)volData[index];
				volDataChar = volDataBuf>>8;
				fputc(volDataChar, trgFile);
				
			}			

		fclose(trgFile);

	}

	delete [] volData;
	free(szCopyChar);
	

}

void CDlgVolumeDicom::NormaliseFloat16bit(float * in_out, int width, int height, int slices)
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
	m=4000.0f/range;
	b=4000.0f-m*(float)max;

	for(x=0; x<width; x++)
		for(y=0; y<height; y++)
			for(z=0; z<slices; z++)
				in_out[z*width*height+y*width+x]
				=(float)FloatToInt( in_out[z*width*height+y*width+x]*m+b+0.00001f );

}
