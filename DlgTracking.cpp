// DlgTracking.cpp : implementation file
//

#include "stdafx.h"
#include "texton.h"
#include <math.h>
#include "DlgTracking.h"
#include "Condensation.h"
#include <fstream.h>

#ifndef ANSI
#define ANSI
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTracking dialog


CDlgTracking::CDlgTracking(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTracking::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTracking)
	m_denseMean = 0.0;
	m_denseScaling = 0.0;
	m_denseSigma = 0.0f;
	m_trackLoaded = _T("");
	m_trackSpaceRad = 0;
	m_trackTimeRad = 0;
	m_trackTracked = _T("");
	m_denseResX = _T("");
	m_denseresY = _T("");
	//}}AFX_DATA_INIT
}


void CDlgTracking::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTracking)
	DDX_Text(pDX, IDC_DENSE_MEAN, m_denseMean);
	DDX_Text(pDX, IDC_DENSE_SCALING, m_denseScaling);
	DDX_Text(pDX, IDC_DENSE_SIGMA, m_denseSigma);
	DDX_Text(pDX, IDC_TRACKING_LOADED, m_trackLoaded);
	DDX_Text(pDX, IDC_TRACKING_SPACERAD, m_trackSpaceRad);
	DDX_Text(pDX, IDC_TRACKING_TIMERAD, m_trackTimeRad);
	DDX_Text(pDX, IDC_TRACKING_TRACKED, m_trackTracked);
	DDX_Text(pDX, IDC_STATIC_DENSE_X, m_denseResX);
	DDX_Text(pDX, IDC_STATIC_DENSE_Y, m_denseresY);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgTracking, CDialog)
	//{{AFX_MSG_MAP(CDlgTracking)
	ON_BN_CLICKED(IDC_CONDENSATION_GO, OnCondensationGo)
	ON_BN_CLICKED(IDC_FINDVORTEX_FLOWVIS_TRACKING, OnFindvortexFlowvisTracking)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTracking message handlers

void CDlgTracking::OnCondensationGo() 
{
	UpdateData(TRUE);

	const int nopoints = 1;
	const int nosamples = 300;

	CPoint inputPoints;
	CPoint observationPoints;
	double resultx, resulty;

	inputPoints.x=100;
	inputPoints.y=100;

	CCondensation dense(nopoints, nosamples, m_denseMean, m_denseScaling, m_denseSigma, 
		&inputPoints, &observationPoints);

	dense.Process();
	dense.Process();
	dense.Process();

	observationPoints.x = 220;
	observationPoints.y = 110;

	dense.Process(); // first run
	dense.Process(); // second run

	/*
	for(i=0; i<nosamples; i++)
		observationPoints[i].x=observationPoints[i].y=250;
	*/

	resultx=resulty=0;

	int i;
	for(i=0; i<nosamples; i++)
	{
		resultx = resultx + (float)dense.buffer[i].x/nosamples;
		resulty = resulty + (float)dense.buffer[i].y/nosamples;
	}

	m_denseResX.Format("%f", resultx);
	m_denseresY.Format("%f", resulty);

	UpdateData(FALSE);


/*
	// data structures
	const int MAX_VTX = 50;
	// maximum number of vortices

	int detected[MAX_VTX][5];
	// detected vortices as loaded from file
	// [0]-x coord [1]-y coord [2]-frame no. [3]-vorticity [4]-clockwise
	// assuming no more than 50 detected vortices

	int tracked[MAX_VTX][6];
	// tracked vortices after our removal algorithm [5]-vortex serial no.

	int vortex_count; // how many vortices in loaded file

	int tracked_count; // how many vortices in tracked file

	int vortex_serial; // vortex serial number
	
	// load test.txt file
	ifstream inf;
	inf.open("test.txt");

	int i;
	for(i=0; i<MAX_VTX; i++)
	{
		if(inf.eof())
		{
			i++;
			break;
		}

		// typ. 6 130 129 54 1
		inf >> detected[i][2] >> detected[i][0] >> detected[i][1]
			>> detected[i][3] >> detected[i][4];

	}

	vortex_count = i-1;
	m_trackLoaded.Format("%i vortices loaded", vortex_count);

	if(vortex_count == 0)
	{
		AfxMessageBox("No vortices loaded.");
		return;
	}

	// test each vortex for tracking conditions
	int itest;
	tracked_count = 0;
	for(i=0; i<vortex_count; i++)
	{

		for(itest=0; itest<vortex_count; itest++)
		{
			if(itest==i)
				continue;

			// neighbour condition: current vortex must have nearby vortex
			if(PointDistance2D(detected[i][0], detected[i][1], detected[itest][0], detected[itest][1]) 
				< (float)m_trackSpaceRad)
				if(abs(detected[i][2]-detected[itest][2]) < m_trackTimeRad)
					// rotation condition: same direction
					if(detected[i][4] == detected[itest][4])
					{
						tracked[tracked_count][0]=detected[i][0];
						tracked[tracked_count][1]=detected[i][1];
						tracked[tracked_count][2]=detected[i][2];
						tracked[tracked_count][3]=detected[i][3];
						tracked[tracked_count][4]=detected[i][4];
						tracked_count++;
						break;
					}

		}


	}

	vortex_serial=0;
	// set vortex serial number based on same conditions again
	for(i=0; i<tracked_count; i++)
	{
		for(itest=0; itest<tracked_count; itest++)
		{
			if(itest==i)
				continue;

			// neighbour condition: current vortex must have nearby vortex
			if(PointDistance2D(tracked[i][0], tracked[i][1], tracked[itest][0], tracked[itest][1]) 
				< (float)m_trackSpaceRad)
				if(abs(tracked[i][2]-tracked[itest][2]) < m_trackTimeRad)
					// rotation condition: same direction
					if(tracked[i][4] == tracked[itest][4])
						if(i<itest)
						{
							tracked[i][5]=vortex_serial;
							vortex_serial++;
							break;
						}
						else
						{
							tracked[i][5]=tracked[itest][5];
							break;
						}

		}


	}

	// save tracked vortices to tracked.txt
	ofstream outf;
	outf.open("tracked.txt");

	outf << "SPACE_RAD=" << m_trackSpaceRad << " TIME_RAD=" << m_trackTimeRad << "\n";

	for(i=0; i<tracked_count; i++)
	{
		outf << tracked[i][2] << " " << tracked[i][0] << " " << tracked[i][1]
			 << " " << tracked[i][3] << " " << tracked[i][4] << " " << tracked[i][5] << "\n";

	}

	m_trackTracked.Format("%i vortices tracked in %i entries", vortex_serial, tracked_count);

	UpdateData(FALSE);
*/	

}

void CDlgTracking::OnFindvortexFlowvisTracking() 
{
	UpdateData(TRUE);

	// data structures
	const int MAX_VTX = 50;
	// maximum number of vortices

	int detected[MAX_VTX][5];
	// detected vortices as loaded from file
	// [0]-x coord [1]-y coord [2]-frame no. [3]-vorticity [4]-clockwise
	// assuming no more than 50 detected vortices

	int tracked[MAX_VTX][6];
	// tracked vortices after our removal algorithm [5]-vortex serial no.

	int vortex_count; // how many vortices in loaded file

	int tracked_count; // how many vortices in tracked file

	int vortex_serial; // vortex serial number
	
	// load test.txt file
	ifstream inf;
	inf.open("test.txt");

	int i;
	for(i=0; i<MAX_VTX; i++)
	{
		if(inf.eof())
		{
			i++;
			break;
		}

		// typ. 6 130 129 54 1
		inf >> detected[i][2] >> detected[i][0] >> detected[i][1]
			>> detected[i][3] >> detected[i][4];

	}

	vortex_count = i-1;
	m_trackLoaded.Format("%i vortices loaded", vortex_count);

	if(vortex_count == 0)
	{
		AfxMessageBox("No vortices loaded.");
		return;
	}

	// test each vortex for tracking conditions
	int itest;
	tracked_count = 0;
	for(i=0; i<vortex_count; i++)
	{

		for(itest=0; itest<vortex_count; itest++)
		{
			if(itest==i)
				continue;

			// neighbour condition: current vortex must have nearby vortex
			if(PointDistance2D(detected[i][0], detected[i][1], detected[itest][0], detected[itest][1]) 
				< (float)m_trackSpaceRad)
				if(abs(detected[i][2]-detected[itest][2]) < m_trackTimeRad)
					// rotation condition: same direction
					if(detected[i][4] == detected[itest][4])
					{
						tracked[tracked_count][0]=detected[i][0];
						tracked[tracked_count][1]=detected[i][1];
						tracked[tracked_count][2]=detected[i][2];
						tracked[tracked_count][3]=detected[i][3];
						tracked[tracked_count][4]=detected[i][4];
						tracked_count++;
						break;
					}

		}


	}

	vortex_serial=0;
	// set vortex serial number based on same conditions again
	for(i=0; i<tracked_count; i++)
	{
		for(itest=0; itest<tracked_count; itest++)
		{
			if(itest==i)
				continue;

			// neighbour condition: current vortex must have nearby vortex
			if(PointDistance2D(tracked[i][0], tracked[i][1], tracked[itest][0], tracked[itest][1]) 
				< (float)m_trackSpaceRad)
				if(abs(tracked[i][2]-tracked[itest][2]) < m_trackTimeRad)
					// rotation condition: same direction
					if(tracked[i][4] == tracked[itest][4])
						if(i<itest)
						{
							tracked[i][5]=vortex_serial;
							vortex_serial++;
							break;
						}
						else
						{
							tracked[i][5]=tracked[itest][5];
							break;
						}

		}


	}

	// save tracked vortices to tracked.txt
	ofstream outf;
	outf.open("tracked.txt");

	outf << "SPACE_RAD=" << m_trackSpaceRad << " TIME_RAD=" << m_trackTimeRad << "\n";

	for(i=0; i<tracked_count; i++)
	{
		outf << tracked[i][2] << " " << tracked[i][0] << " " << tracked[i][1]
			 << " " << tracked[i][3] << " " << tracked[i][4] << " " << tracked[i][5] << "\n";

	}

	m_trackTracked.Format("%i vortices tracked in %i entries", vortex_serial, tracked_count);

	UpdateData(FALSE);

}

float CDlgTracking::PointDistance2D(int x1, int y1, int x2, int y2)
{
	return (float)sqrt(pow(y2-y1,2.0)+pow(x2-x1,2.0));

}
