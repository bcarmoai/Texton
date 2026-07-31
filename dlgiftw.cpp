// DlgIFTW.cpp : implementation file
//

#include "stdafx.h"
#include "Texton.h"
#include "TextonView.h"
#include "DlgIFTW.h"
#include "WrapperIftw.h"
#include "OpencvContour.h"
#include <fstream.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgIFTW dialog


CDlgIFTW::CDlgIFTW(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgIFTW::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgIFTW)
	m_staticDots = _T("");
	m_mouseRootTitle = _T("");
	m_mouseRootValue = _T("");
	m_macCCost = 500;
	m_cirRad = 3;
	m_maxIterations = 10;
	//}}AFX_DATA_INIT

	imgIftwAnnotated = NULL;
	imgDots.imgData = NULL;
	imgCosts = NULL;
	bZoom = FALSE;
	m_mouseRootTitle.Format("pixel root:");
	root_map = NULL;
	predec_map = NULL;
	bInteract = FALSE;
	green_paths = NULL;

}


void CDlgIFTW::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgIFTW)
	DDX_Text(pDX, IDC_STATIC_IFTW_DOTS, m_staticDots);
	DDX_Text(pDX, IDC_IFTW_ROOT_TITLE_STATIC, m_mouseRootTitle);
	DDX_Text(pDX, IDC_IFTW_ROOT_VALUE_STATIC, m_mouseRootValue);
	DDX_Text(pDX, IDC_IFTW_MACCOST, m_macCCost);
	DDX_Text(pDX, IDC_IFTW_MAXCIRRAD, m_cirRad);
	DDX_Text(pDX, IDC_IFTW_MAXITS, m_maxIterations);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgIFTW, CDialog)
	//{{AFX_MSG_MAP(CDlgIFTW)
	ON_BN_CLICKED(IDC_PEAKS, OnPeaks)
	ON_BN_CLICKED(IDC_IFTW_SHOW_PEAKS, OnIftwShowPeaks)
	ON_BN_CLICKED(IDC_IFTW_ORIGINAL, OnIftwOriginal)
	ON_BN_CLICKED(IDC_IFTW_WATERSHED_GO, OnIftwWatershed)
	ON_BN_CLICKED(IDC_IFTW_SHOW_W, OnIftwShowW)
	ON_BN_CLICKED(IDC_IFTW_COSTS, OnIftwCosts)
	ON_BN_CLICKED(IDC_IFTW_SHOW_COSTS, OnIftwShowCosts)
	ON_BN_CLICKED(IDOK, OnOk)
	ON_BN_CLICKED(IDC_IFTW_INTERACT, OnIftwInteract)
	ON_BN_CLICKED(IDC_IFTW_PROPAG, OnIftwPropag)
	ON_BN_CLICKED(IDC_IFTW_CHECK_ROOTS, OnIftwCheckRoots)
	ON_BN_CLICKED(IDC_IFTW_SAVEMATLAB, OnIftwSavematlab)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgIFTW message handlers

void CDlgIFTW::OnPeaks() 
{

	CFileDialog* dDlg = new CFileDialog(FALSE,"pgm","peaks",OFN_CREATEPROMPT |
		OFN_HIDEREADONLY | OFN_NOREADONLYRETURN,
		_T("PGM Images (*.pgm)|*.pgm||"), NULL);

	if(dDlg->DoModal()!=IDOK)
		return;

	imgDots.LoadPgm(dDlg->GetPathName());

	m_staticDots = dDlg->GetFileName();
	UpdateData(FALSE);


	if(   imgDots.imgWidth != ((CTextonView*)pView)->pgmImage.imgWidth
	   || imgDots.imgHeight != ((CTextonView*)pView)->pgmImage.imgHeight )
	   {
			char sms[200];
			sprintf(sms, "WARNING: Loaded peaks image (%ix%i) is of different size than the working image (%ix%i).\
\nThis program will either crash or behave incorrectly if you try to use the current peaks image.",
			imgDots.imgWidth, imgDots.imgHeight, ((CTextonView*)pView)->pgmImage.imgWidth,
			((CTextonView*)pView)->pgmImage.imgHeight);
			AfxMessageBox(sms);
	   }

	delete [] dDlg;

}

void CDlgIFTW::OnIftwShowPeaks() 
{
	((CTextonView*)pView)->DrawResult(imgDots.imgData);
}

void CDlgIFTW::OnIftwOriginal() 
{
	((CTextonView*)pView)->DrawPgm();
	
}

void CDlgIFTW::OnIftwWatershed() 
{

	if(green_paths == NULL)
		delete [] green_paths;
	if(imgIftwAnnotated == NULL)
		delete [] imgIftwAnnotated;
	if(root_map == NULL)
		delete [] root_map;
	if(predec_map == NULL)
		delete [] predec_map;

	imgIftwAnnotated = new int[imgDots.imgWidth * imgDots.imgHeight];

	CWrapperIftw cIFTW;

	cIFTW.Watershed(imgDots.imgWidth, imgDots.imgHeight,  // sizes
		((CTextonView*)pView)->pgmImage.imgData, // original image
		imgDots.imgData, // peaks image
		imgCosts,	      // cost function output, see OnIftwCosts()
		imgIftwAnnotated, // target image
		2,  // watershed algorithm version no. (see CWrapperIftw.h)
		&root_map,    // root map
		&predec_map); // predecessor map

	OnIftwShowW();

	bZoom = TRUE;
	bInteract = FALSE;
	
}

void CDlgIFTW::OnIftwShowW() 
{
	int width = imgDots.imgWidth;
	int height = imgDots.imgHeight;

	int* dmy = new int[width * height];

	int x, y;
	for(x=0; x<width; x++)
		for(y=0; y<height; y++)
			if(imgDots.imgData[width*y+x] == 255)
				dmy[width*y+x] = 455; // magic number for green
			else
				if(imgIftwAnnotated[width*y+x] == 255)
					dmy[width*y+x] = 255; // magic number for red
				else
					dmy[width*y+x] = imgIftwAnnotated[width*y+x];
					/*((CTextonView*)pView)->pgmImage.imgData[width*y+x];*/

	CString imginfo;
	imginfo.Format("image: %ix%i", imgDots.imgWidth, imgDots.imgHeight);
	pVolDlg = new CDlgVolView;
	pVolDlg->pView = pView;
	pVolDlg->m_sVolumeTitle.Format("watershed roots");
	pVolDlg->InitialiseVolume(dmy, imgDots.imgWidth, imgDots.imgHeight, 1);
	pVolDlg->m_imgInfo = imginfo;
	pVolDlg->Create(IDD_VOL_VIEW, this);
	pVolDlg->ShowWindow(SW_SHOW);

	delete dmy;
	
}

void CDlgIFTW::OnIftwCosts() 
{

	if(imgCosts != NULL)
	{
		delete imgCosts;
		imgCosts = NULL;
	}

	imgCosts = new int[imgDots.imgWidth*imgDots.imgHeight];

	// for now, assign intensity as costs directly
	int x, y;
	for(x=0; x<imgDots.imgWidth; x++)
		for(y=0; y<imgDots.imgHeight; y++)
			imgCosts[imgDots.imgWidth*y+x] = 
			((CTextonView*)pView)->pgmImage.imgData[imgDots.imgWidth*y+x];
		
}

void CDlgIFTW::OnIftwShowCosts() 
{
	CString imginfo;
	imginfo.Format("image: %ix%i", imgDots.imgWidth, imgDots.imgHeight);
	pVolDlg = new CDlgVolView;
	pVolDlg->pView = pView;
	pVolDlg->m_sVolumeTitle.Format("watershed costs");
	pVolDlg->InitialiseVolume(imgCosts, imgDots.imgWidth, imgDots.imgHeight, 1);
	pVolDlg->m_imgInfo = imginfo;
	pVolDlg->Create(IDD_VOL_VIEW, this);
	pVolDlg->ShowWindow(SW_SHOW);

}

void CDlgIFTW::TakeMovePoint(CPoint point)
{
	static int x, y;
	static CPoint pred_point;

	if(!(point.x >=0 && point.x < imgDots.imgWidth
		&& point.y >=0 && point.y < imgDots.imgHeight))
		return;

	if(bZoom)
	{
		m_mouseRootValue.Format("pixel (%i, %i) - root (%i, %i)", 
			point.x, point.y,
			root_map[imgDots.imgWidth*point.y+point.x].x, 
			root_map[imgDots.imgWidth*point.y+point.x].y);
		
		UpdateData(FALSE);
	}
	else
		if(bInteract)
		{
			for(x=0; x<imgDots.imgWidth; x++)
				for(y=0; y<imgDots.imgHeight; y++)
					green_paths[imgDots.imgWidth*y+x]
						= ((CTextonView*)pView)->pgmImage.imgData[imgDots.imgWidth*y+x];

			// draw root point
			green_paths[imgDots.imgWidth*interactRootPoint.y+interactRootPoint.x]=256;

			// draw clicked point
			green_paths[imgDots.imgWidth*point.y+point.x]=455;

			pred_point = predec_map[imgDots.imgWidth*point.y+point.x];
			while(pred_point.x >= 0 && !(pred_point.x == interactRootPoint.x
				&& pred_point.y == interactRootPoint.y)
				&& pred_point.x < imgDots.imgWidth && pred_point.y < imgDots.imgHeight
				&& pred_point.y >= 0)
			{
				green_paths[imgDots.imgWidth*pred_point.y+pred_point.x]=455;
				pred_point = predec_map[imgDots.imgWidth*pred_point.y+pred_point.x];
			
			}
			
			((CTextonView*)pView)->DrawResult(green_paths);

			m_mouseRootValue.Format("pixel (%i, %i) - cumulative cost %i", 
				point.x, point.y, imgCosts[imgDots.imgWidth*point.y+point.x]);
			UpdateData(FALSE);
			
		}


}

void CDlgIFTW::OnOk() 
{
	OnCancel();		
}

void CDlgIFTW::OnCancel() 
{
	((CTextonView*)pView)->DlgIFTWClosed();
	CDialog::OnCancel();
}

void CDlgIFTW::OnIftwInteract() 
{
	if(imgDots.imgData != NULL)
		delete imgDots.imgData;
	if(green_paths == NULL)
		delete [] green_paths;

	imgDots.imgWidth = ((CTextonView*)pView)->pgmImage.imgWidth;
	imgDots.imgHeight = ((CTextonView*)pView)->pgmImage.imgHeight;
	imgDots.imgData = new int[imgDots.imgWidth*imgDots.imgHeight];
	green_paths = new int[imgDots.imgWidth*imgDots.imgHeight];

	int x, y;
	for(x=0; x<imgDots.imgWidth; x++)
		for(y=0; y<imgDots.imgHeight; y++)
		{
			imgDots.imgData[imgDots.imgWidth*y+x] = 0;
			green_paths[imgDots.imgWidth*y+x] 
				= ((CTextonView*)pView)->pgmImage.imgData[imgDots.imgWidth*y+x];
		}

	if(imgIftwAnnotated == NULL)
		delete [] imgIftwAnnotated;
	imgIftwAnnotated = new int[imgDots.imgWidth * imgDots.imgHeight];
	if(imgCosts == NULL)
		delete [] imgCosts;
	imgCosts = new int[imgDots.imgWidth*imgDots.imgHeight];
	if(root_map == NULL)
		delete [] root_map;
	if(predec_map == NULL)
		delete [] predec_map;

	bZoom = FALSE;
	bInteract = TRUE;	

}

void CDlgIFTW::TakeDownPoint(CPoint point)
{
	if(!(point.x >=0 && point.x < imgDots.imgWidth
		&& point.y >=0 && point.y < imgDots.imgHeight))
		return;

	if(!bInteract)
		return;

	imgDots.imgData[imgDots.imgWidth*point.y+point.x]=255;
	
	CWrapperIftw cIFTW;

	cIFTW.Watershed(imgDots.imgWidth, imgDots.imgHeight,  // sizes
		((CTextonView*)pView)->pgmImage.imgData, // original image
		imgDots.imgData, // peaks image
		imgCosts,	      // cost function output, see OnIftwCosts()
		imgIftwAnnotated, // target image
		2,  // watershed algorithm version no. (see CWrapperIftw.h)
		&root_map,    // root map
		&predec_map); // predecessor map

	int x, y;
	for(x=0; x<imgDots.imgWidth; x++)
		for(y=0; y<imgDots.imgHeight; y++)
			green_paths[imgDots.imgWidth*y+x]
			= ((CTextonView*)pView)->pgmImage.imgData[imgDots.imgWidth*y+x];
		
	((CTextonView*)pView)->DrawResult(green_paths);

	interactRootPoint = point;

}


void CDlgIFTW::OnIftwPropag() 
{

	ofstream of;
	of.open("c:\\iftw_propag_log.txt");

	of << "watershed propagation\n" << flush;

	// expand least cost path from current root point
	// by trying several neighbouring points in a circle

	UpdateData(TRUE);
	// m_macCCost
	// m_cirRad

	CArray<CPoint, CPoint> cir_points;
	COpencvContour circle_draw;
	int i;
	int max_i;
	int min_ccost;
	CPoint min_point;
	CPoint cur_root_point = interactRootPoint;
	CPoint pred_point;
	CPoint prev_root(-1, -1);

	int x, y;
	for(x=0; x<imgDots.imgWidth; x++)
		for(y=0; y<imgDots.imgHeight; y++)
			green_paths[imgDots.imgWidth*y+x]
			= ((CTextonView*)pView)->pgmImage.imgData[imgDots.imgWidth*y+x];

	of << "initial root point: (" << cur_root_point.x << ", " << cur_root_point.y
	   << ")\n" << flush;

	int limit;
	for(limit=0; limit<m_maxIterations; limit++)
	{

		// get circle points
		circle_draw.GetCirclePoints(m_cirRad, cur_root_point.x, 
			cur_root_point.y, cir_points);

		of << "testing " << cir_points.GetSize() << " circle points.\n" << flush;

		// go through each point and determine which has lowest cumulative cost
		max_i = cir_points.GetSize();
		min_point.x=min_point.y=-1;
		min_ccost = INT_MAX;
		for(i=0; i< max_i; i++)
		{

			of << "circle point: (" << cir_points[i].x << ", " << cir_points[i].y
				<< ") - cost:" << imgCosts[imgDots.imgWidth*cir_points[i].y+cir_points[i].x]
				<< "\n" << flush;

			if(imgCosts[imgDots.imgWidth*cir_points[i].y+cir_points[i].x] < min_ccost
				&& cir_points[i] != prev_root)
			{
				min_ccost = imgCosts[imgDots.imgWidth*cir_points[i].y+cir_points[i].x];
				min_point = cir_points[i];
			}
		}
		
		of << "point chosen: (" << min_point.x << ", " << min_point.y
		   << ") - cost:" << min_ccost << "\n" << flush;

		if(min_point.x == -1 || min_point.y == -1 || cir_points.GetSize() == 0)
		{
			AfxMessageBox("Error: could not find minimum cost point.");
			return;
		}

		if(min_ccost > m_macCCost)
		{
			of << "min cost exceeded: " << min_ccost << " > " << m_macCCost
			   << ". propagate exited with " << limit+1 << " iterations.\n" << flush;
			break;
		}

		// draw path from root to minimum cost point
		// draw root point in red
		green_paths[imgDots.imgWidth*cur_root_point.y+cur_root_point.x]=256;

		// draw new root point in red
		green_paths[imgDots.imgWidth*min_point.y+min_point.x]=256;
		
		// draw path in green
		pred_point = predec_map[imgDots.imgWidth*min_point.y+min_point.x];

		of << "drawing path from root (" << cur_root_point.x << ", " << cur_root_point.y 
		   << ") to minimum point (" << min_point.x << ", " << min_point.y 
		   << ")\n" << flush;

		while(pred_point.x >= 0 && !(pred_point.x == cur_root_point.x
			&& pred_point.y == cur_root_point.y)
			&& pred_point.x < imgDots.imgWidth && pred_point.y < imgDots.imgHeight
			&& pred_point.y >= 0)
		{
			green_paths[imgDots.imgWidth*pred_point.y+pred_point.x]=455;
			of << "drawn path point (" << pred_point.x << ", " << pred_point.y 
			   << ")\n" << flush;
			pred_point = predec_map[imgDots.imgWidth*pred_point.y+pred_point.x];
			
		}

		of << "finished drawning path from minimum point\n" << flush;

		// fire watershed centered on new root
		PropagNewWatershed(cur_root_point, min_point);

		// set minimum cost point as the new root
		prev_root = cur_root_point;
		cur_root_point = min_point;


	}

	of << "propagate exited with "<< limit << " iterations.\n" << flush;

	((CTextonView*)pView)->DrawResult(green_paths);


}

void CDlgIFTW::PropagNewWatershed(CPoint oldRoot, CPoint newRoot)
{
	imgDots.imgData[imgDots.imgWidth*oldRoot.y+oldRoot.x]=0;
	imgDots.imgData[imgDots.imgWidth*newRoot.y+newRoot.x]=255;

	// modify original image so already traced paths have very high cost
	int* dmy_original = new int[imgDots.imgWidth*imgDots.imgHeight];
	int x, y;
	for(x=0; x<imgDots.imgWidth; x++)
		for(y=0; y<imgDots.imgHeight; y++)
			if((green_paths[imgDots.imgWidth*y+x] > 255)
			   && (x!=newRoot.x&&y!=newRoot.y))
					dmy_original[imgDots.imgWidth*y+x]=255;
			   else
					dmy_original[imgDots.imgWidth*y+x]=
					((CTextonView*)pView)->pgmImage.imgData[imgDots.imgWidth*y+x];

	CWrapperIftw cIFTW;

	cIFTW.Watershed(imgDots.imgWidth, imgDots.imgHeight,  // sizes
		dmy_original, // original image
		imgDots.imgData, // peaks image
		imgCosts,	      // cost function output, see OnIftwCosts()
		imgIftwAnnotated, // target image
		2,  // watershed algorithm version no. (see CWrapperIftw.h)
		&root_map,    // root map
		&predec_map); // predecessor map

	delete [] dmy_original;
}

void CDlgIFTW::OnIftwCheckRoots() 
{
	int* dmy = new int[imgDots.imgWidth*imgDots.imgHeight];

	int x, y;
	for(x=0; x<imgDots.imgWidth; x++)
		for(y=0; y<imgDots.imgHeight; y++)
			if(predec_map[imgDots.imgWidth*y+x].x==-1 ||
			   predec_map[imgDots.imgWidth*y+x].y==-1)
				dmy[imgDots.imgWidth*y+x]=0;
			else
				dmy[imgDots.imgWidth*y+x]=255;

	((CTextonView*)pView)->DrawResult(dmy);

	delete [] dmy;

	
}

void CDlgIFTW::OnIftwSavematlab() 
{
	ofstream of;
	of.open("c:\\vortex_data.txt");

	int x, y;	
	for(y=0; y<imgDots.imgHeight; y++)
	{
		of << "\n";

		for(x=0; x<imgDots.imgWidth; x++)
		{
			of << ((CTextonView*)pView)->pgmImage.imgData[imgDots.imgWidth*y+x] << " ";

		}
	}

}
