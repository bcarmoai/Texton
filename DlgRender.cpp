// DlgRender.cpp : implementation file
//

#include "stdafx.h"
#include "Texton.h"
#include "DlgRender.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <GL\gl.h>
#include <GL\glu.h>
#include <GL\glaux.h>

/////////////////////////////////////////////////////////////////////////////
// CDlgRender dialog


CDlgRender::CDlgRender(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRender::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgRender)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	// Mouse
	m_LeftButtonDown = FALSE;
	m_RightButtonDown = FALSE;

	InitGeometry();

}

void CDlgRender::InitGeometry(void)
{
	m_xRotation = 0.0f;
	m_yRotation = 0.0f;
	m_zRotation = 0.0f;

	m_xTranslation = 0.0f;
	m_yTranslation = 0.0f;
	m_zTranslation = -5.0f;

	m_xScaling = 1.0f;
	m_yScaling = 1.0f;
	m_zScaling = 1.0f;

	m_SpeedRotation = 1.0f / 3.0f;
	m_SpeedTranslation = 1.0f / 100.0f;

	m_xyRotation = 1;
}

void CDlgRender::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRender)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgRender, CDialog)
	//{{AFX_MSG_MAP(CDlgRender)
	ON_WM_PAINT()
	ON_WM_CANCELMODE()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CAPTURECHANGED()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgRender message handlers

void CDlgRender::OnPaint() 
{
	CDialog::OnPaint();

	CClientDC	dc (this);
	CDC* pDC = &dc;

    // Select the palette.
    CPalette* ppalOld = NULL;
	if (m_pPal)
	{
    	ppalOld = pDC->SelectPalette(m_pPal, 0);
    	pDC->RealizePalette();
	}
	
	// Make the HGLRC current
    BOOL bResult = wglMakeCurrent(pDC->m_hDC, m_hrc);
	if (!bResult)
	{
		TRACE("wglMakeCurrent Failed %x\r\n", GetLastError() ) ;
	}

	// Draw	
	DrawScene() ; 

	//Swap Buffers
	SwapBuffers(pDC->m_hDC) ;

    // select old palette if we altered it
    if (ppalOld) pDC->SelectPalette(ppalOld, 0); 

	wglMakeCurrent(NULL, NULL) ;


}

void CDlgRender::OnCancelMode() 
{
	CDialog::OnCancelMode();	
}

BOOL CDlgRender::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN ;

	return CDialog::PreCreateWindow(cs);
}


int CDlgRender::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
    CClientDC dc(this) ;
	//
	// Fill in the Pixel Format Descriptor
	//
    PIXELFORMATDESCRIPTOR pfd ;
	memset(&pfd,0, sizeof(PIXELFORMATDESCRIPTOR)) ;

    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);   
    pfd.nVersion = 1 ;                           // Version number
	pfd.dwFlags =  PFD_DOUBLEBUFFER |            // Use double buffer
	               PFD_SUPPORT_OPENGL |          // Use OpenGL
	               PFD_DRAW_TO_WINDOW ;          // Pixel format is for a window.
	pfd.iPixelType = PFD_TYPE_RGBA ;
    pfd.cColorBits = 24;                         // 8-bit color
	pfd.cDepthBits = 32 ;					   	 // 32-bit depth buffer
    pfd.iLayerType = PFD_MAIN_PLANE ;            // Layer type

    int nPixelFormat = ChoosePixelFormat(dc.m_hDC, &pfd);
	if (nPixelFormat == 0)
	{
		TRACE("ChoosePixelFormat Failed %d\r\n",GetLastError()) ;
		return -1 ;
	}
	TRACE("Pixel Format %d\r\n", nPixelFormat) ;

    BOOL bResult = SetPixelFormat(dc.m_hDC, nPixelFormat, &pfd);
	if (!bResult)
	{
		TRACE("SetPixelFormat Failed %d\r\n",GetLastError()) ;
		return -1 ;
	}
	
	//
    // Create a rendering context.
    //
    m_hrc = wglCreateContext(dc.m_hDC);
	if (!m_hrc)
	{
		TRACE("wglCreateContext Failed %x\r\n", GetLastError()) ;
		return -1;
	}

	// Create the palette
	CreateRGBPalette(dc.m_hDC) ;
	
	return 0;
}

BOOL CDlgRender::CreateRGBPalette(HDC hDC)
{
	//
	// Check to see if we need a palette
	//
    PIXELFORMATDESCRIPTOR pfd;
    int n = GetPixelFormat(hDC);
    DescribePixelFormat(hDC, n, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
	if (!(pfd.dwFlags & PFD_NEED_PALETTE)) return FALSE ;

	TRACE0("Creating palette\r\n") ;
    // allocate a log pal and fill it with the color table info
    LOGPALETTE* pPal = (LOGPALETTE*) malloc(sizeof(LOGPALETTE) 
                     + 256 * sizeof(PALETTEENTRY));
    if (!pPal) 
    {
        TRACE("Out of memory for logpal");
        return FALSE;
    }
    pPal->palVersion = 0x300; // Windows 3.0
    pPal->palNumEntries = 256; // table size

	//
	// Create RGB Palette
	//
	ASSERT( pfd.cColorBits == 8) ;
     n = 1 << pfd.cColorBits;
    for (int i=0; i<n; i++)
    {
        pPal->palPalEntry[i].peRed =
                ComponentFromIndex(i, pfd.cRedBits, pfd.cRedShift);
        pPal->palPalEntry[i].peGreen =
                ComponentFromIndex(i, pfd.cGreenBits, pfd.cGreenShift);
        pPal->palPalEntry[i].peBlue =
                ComponentFromIndex(i, pfd.cBlueBits, pfd.cBlueShift);
        pPal->palPalEntry[i].peFlags = 0;
    }

	//
	// Fix up color table with system colors.
	//
    if ((pfd.cColorBits == 8)                           &&
        (pfd.cRedBits   == 3) && (pfd.cRedShift   == 0) &&
        (pfd.cGreenBits == 3) && (pfd.cGreenShift == 3) &&
        (pfd.cBlueBits  == 2) && (pfd.cBlueShift  == 6)
       )
    {
	    for (int j = 1 ; j <= 12 ; j++)
	          pPal->palPalEntry[m_defaultOverride[j]] = m_defaultPalEntry[j];
	}

    // Delete any existing GDI palette
	if (m_pPal) delete m_pPal ;
	m_pPal = new CPalette ;

    BOOL bResult = m_pPal->CreatePalette(pPal);
    free (pPal);

    return bResult;
}

unsigned char CDlgRender::ComponentFromIndex(int i, UINT nbits, UINT shift)
{
    unsigned char val;

    val = (unsigned char) (i >> shift);
    switch (nbits) {

    case 1:
        val &= 0x1;
        return m_oneto8[val];

    case 2:
        val &= 0x3;
        return m_twoto8[val];

    case 3:
        val &= 0x7;
        return m_threeto8[val];

    default:
        return 0;
    }
}

//////////////////////////////////////////////////////////////////////////////////////
//
// OpenGLpalette
//
unsigned char CDlgRender::m_threeto8[8] = {
    0, 0111>>1, 0222>>1, 0333>>1, 0444>>1, 0555>>1, 0666>>1, 0377
};
unsigned char CDlgRender::m_twoto8[4] = {
   0, 0x55, 0xaa, 0xff
};
unsigned char CDlgRender::m_oneto8[2] = {
    0, 255
};

int CDlgRender::m_defaultOverride[13] = {
    0, 3, 24, 27, 64, 67, 88, 173, 181, 236, 247, 164, 91
};

PALETTEENTRY CDlgRender::m_defaultPalEntry[20] = {
    { 0,   0,   0,    0 }, //0
    { 0x80,0,   0,    0 }, 
    { 0,   0x80,0,    0 }, 
    { 0x80,0x80,0,    0 }, 
    { 0,   0,   0x80, 0 },
    { 0x80,0,   0x80, 0 },
    { 0,   0x80,0x80, 0 },
    { 0xC0,0xC0,0xC0, 0 }, //7

    { 192, 220, 192,  0 }, //8
    { 166, 202, 240,  0 },
    { 255, 251, 240,  0 },
    { 160, 160, 164,  0 }, //11

    { 0x80,0x80,0x80, 0 }, //12
    { 0xFF,0,   0,    0 },
    { 0,   0xFF,0,    0 },
    { 0xFF,0xFF,0,    0 },
    { 0,   0,   0xFF, 0 },
    { 0xFF,0,   0xFF, 0 },
    { 0,   0xFF,0xFF, 0 },
    { 0xFF,0xFF,0xFF, 0 }  //19
  };


void CDlgRender::OnSize(UINT nType, int cx, int cy) 
{
	
	CDialog::OnSize(nType, cx, cy);

	TRACE0("OnSize\r\n") ;

	if ( (cx <= 0) || (cy <= 0) ) return ;
	
    CClientDC dc(this) ;

	//
	// Make the rendering context m_hrc current
	//
    BOOL bResult = wglMakeCurrent(dc.m_hDC, m_hrc);
	if (!bResult)
	{
		TRACE("wglMakeCurrent Failed %x\r\n", GetLastError() ) ;
		return ;
	}

    //
    // Set up the mapping of 3-space to screen space
    //
    GLdouble gldAspect = (GLdouble) cx/ (GLdouble) cy;
    glMatrixMode(GL_PROJECTION); OutputGlError("MatrixMode") ;
   	glLoadIdentity();
   	gluPerspective(40.0, gldAspect, 1.0, 10.0);
    glViewport(0, 0, cx, cy);

	//
	// No rendering context will be current.
	//
    wglMakeCurrent(NULL, NULL);	
	OnPaint();
	
}

void CDlgRender::OutputGlError(char* label) 
{
	GLenum errorno = glGetError() ;
	if (errorno != GL_NO_ERROR)
		TRACE("%s had error: #(%d) %s\r\n", label, errorno, gluErrorString(errorno)) ;
}

BOOL CDlgRender::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	PrepareScene() ;		
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgRender::OnCaptureChanged(CWnd *pWnd) 
{	
	CDialog::OnCaptureChanged(pWnd);
}

void CDlgRender::PrepareScene() 
{					   
	// Faster than using glScale.
	GLdouble size = 0.5 ;
	GLdouble dsize = 0.35 ; // Size for Dodecadedron

	//
	// Attach the window dc to OpenGL.
	//
	CClientDC dc(this) ;
	BOOL bResult = wglMakeCurrent(dc.m_hDC, m_hrc);	 
	if (!bResult)
	{
		TRACE("wglMakeCurrent Failed %x\r\n", GetLastError() ) ;
	}

	// Build a display list for the cube.
	//    
	glNewList(Box, GL_COMPILE) ; 
	 	//
	    // Draw the six faces of the cube.
	    //
	    glBegin(GL_POLYGON); // right
			glNormal3d(  0.0,  0.0,  size);
	        glVertex3d( size,  size, size); 
	        glVertex3d(-size,  size, size);            
	        glVertex3d(-size, -size, size);           
	        glVertex3d( size, -size, size);            
	    glEnd();

	    glBegin(GL_POLYGON);	//left
			glNormal3d( 0.0,  0.0, -size);
	        glVertex3d( size,  size, -size);
	        glVertex3d( size, -size, -size);           
	        glVertex3d(-size, -size, -size);          
	        glVertex3d(-size,  size, -size);           
	    glEnd();

	    glBegin(GL_POLYGON); //Font
			glNormal3d(-size,  0.0,  0.0);
	        glVertex3d(-size,  size,  size);
	        glVertex3d(-size,  size, -size);           
	        glVertex3d(-size, -size, -size);          
	        glVertex3d(-size, -size,  size);           
	    glEnd();

	    glBegin(GL_POLYGON);  //Back
			glNormal3d(size,  0.0,  0.0);
	        glVertex3d(size,  size,  size);
	        glVertex3d(size, -size,  size);            
	        glVertex3d(size, -size, -size);           
	        glVertex3d(size,  size, -size);            
	    glEnd();

	    glBegin(GL_POLYGON);  //Top
			glNormal3d( 0.0, size,  0.0);
	        glVertex3d(-size, size, -size);
	        glVertex3d(-size, size,  size);            
	        glVertex3d( size, size,  size);             
	        glVertex3d( size, size, -size);            
	    glEnd();

	    glBegin(GL_POLYGON);	//Bottom
			glNormal3d( 0.0, -size,  0.0);
	        glVertex3d(-size, -size, -size);
	        glVertex3d( size, -size, -size);           
	        glVertex3d( size, -size,  size);            
	        glVertex3d(-size, -size,  size);           
	    glEnd();
	glEndList() ;			   

	//
	// Build a display list for the pyramid.
	//
	glNewList(Pyramid, GL_COMPILE) ; OutputGlError("glNewList: Pyramid") ;
 	//
    // Draw the pyramid
    //
    	glBegin(GL_POLYGON); 
			glNormal3d(-size,-size,-size) ;
			glVertex3d( size,-size,-size) ; //2
			glVertex3d(-size,-size, size) ;	//3
			glVertex3d(-size, size,-size) ;	//4
		glEnd() ;

    	glBegin(GL_POLYGON); 
			glNormal3d(-size, size, size) ;
			glVertex3d( size, size, size) ; //1
			glVertex3d(-size, size,-size) ;	//4
			glVertex3d(-size,-size, size) ;	//3
		glEnd() ;

    	glBegin(GL_POLYGON); 
			glNormal3d( size, size,-size) ;
			glVertex3d( size, size, size) ; //1
			glVertex3d( size,-size,-size) ; //2
			glVertex3d(-size, size,-size) ;	//4
		glEnd() ;

    	glBegin(GL_POLYGON); 
			glNormal3d( size,-size, size) ;
			glVertex3d( size, size, size) ; //1
			glVertex3d(-size,-size, size) ;	//3
			glVertex3d( size,-size,-size) ; //2
		glEnd() ;

	glEndList() ;

	//
	// Build display list for Dodecahedron
	//
	GLdouble t = ((sqrt(5) - 1.0) / 2.0) ; 
	GLdouble tt = t*t ;
	t *= dsize ;
	tt *= dsize ;

	glNewList(Dodec, GL_COMPILE) ; OutputGlError("glNewList: Dodec") ;
    	glBegin(GL_POLYGON); // Face 0
			glNormal3d(0, dsize, t) ;
			glVertex3d(  t,    t,    t) ; // Vertex  0
			glVertex3d( tt,dsize,    0) ; // Vertex  7
			glVertex3d(-tt,dsize,    0) ; // Vertex  8
			glVertex3d( -t,    t,    t) ; // Vertex 15
			glVertex3d(  0,   tt,dsize) ; // Vertex  4
		glEnd() ;

    	glBegin(GL_POLYGON); // Face 1
			glNormal3d(0, dsize, -t) ;
			glVertex3d(  0,   tt,-dsize) ; // Vertex 10
			glVertex3d( -t,    t,    -t) ; // Vertex  9
			glVertex3d(-tt,dsize,     0) ; // Vertex  8
			glVertex3d(	tt,dsize,     0) ; // Vertex  7
			glVertex3d(  t,    t,    -t) ; // Vertex  6
		glEnd() ;

    	glBegin(GL_POLYGON); // Face 2
			glNormal3d(dsize, t, 0) ;
			glVertex3d(    t,    t,   t) ; // Vertex  0
			glVertex3d(dsize,    0,  tt) ; // Vertex  1
			glVertex3d(dsize,    0, -tt) ; // Vertex  5
			glVertex3d(	   t,    t,  -t) ; // Vertex  6
			glVertex3d(   tt,dsize,   0) ; // Vertex  7
		glEnd() ;

    	glBegin(GL_POLYGON); // Face 3
			glNormal3d(dsize, -t, 0) ;
			glVertex3d(dsize,     0,  tt) ; // Vertex  1
			glVertex3d(    t,    -t,   t) ; // Vertex  2
			glVertex3d(   tt,-dsize,   0) ; // Vertex 12
			glVertex3d(	   t,    -t,  -t) ; // Vertex 11
			glVertex3d(dsize,     0, -tt) ; // Vertex  5
		glEnd() ;

    	glBegin(GL_POLYGON); // Face 4
			glNormal3d(0, -dsize, -t) ;
			glVertex3d(  t,    -t,    -t) ; // Vertex 11
			glVertex3d( tt,-dsize,     0) ; // Vertex 12
			glVertex3d(-tt,-dsize,     0) ; // Vertex 13
			glVertex3d( -t,    -t,    -t) ; // Vertex 16
			glVertex3d(  0,   -tt,-dsize) ; // Vertex 17
		glEnd() ;

    	glBegin(GL_POLYGON); // Face 5
			glNormal3d(0, -dsize, t) ;
			glVertex3d( -t,    -t,    t) ; // Vertex 18
			glVertex3d(-tt,-dsize,    0) ; // Vertex 13
			glVertex3d( tt,-dsize,    0) ; // Vertex 12
			glVertex3d(  t,    -t,    t) ; // Vertex  2
			glVertex3d(  0,   -tt,dsize) ; // Vertex  3
		glEnd() ;

    	glBegin(GL_POLYGON); // Face 6
			glNormal3d(t, 0, dsize) ;
			glVertex3d(    0,  tt,dsize) ; // Vertex  4
			glVertex3d(    0, -tt,dsize) ; // Vertex  3
			glVertex3d(    t,  -t,    t) ; // Vertex  2
			glVertex3d(dsize,   0,   tt) ; // Vertex  1
			glVertex3d(    t,   t,    t) ; // Vertex  0
		glEnd() ;

    	glBegin(GL_POLYGON); // Face 7
			glNormal3d(-t, 0, dsize) ;
			glVertex3d(     0, -tt,dsize) ; // Vertex  3
			glVertex3d(     0,  tt,dsize) ; // Vertex  4
			glVertex3d(    -t,   t,    t) ; // Vertex 15
			glVertex3d(-dsize,   0,   tt) ; // Vertex 14
			glVertex3d(    -t,  -t,    t) ; // Vertex 18
		glEnd() ;

    	glBegin(GL_POLYGON); // Face 8
			glNormal3d(t, 0, -dsize) ;
			glVertex3d(    0, -tt,-dsize) ; // Vertex 17
			glVertex3d(    0,  tt,-dsize) ; // Vertex 10
			glVertex3d(    t,   t,    -t) ; // Vertex  6
			glVertex3d(dsize,   0,   -tt) ; // Vertex  5
			glVertex3d(    t,  -t,    -t) ; // Vertex 11
		glEnd() ;

    	glBegin(GL_POLYGON); // Face 9
			glNormal3d(-t, 0, -dsize) ;
			glVertex3d(    -t,   t,    -t) ; // Vertex  9
			glVertex3d(     0,  tt,-dsize) ; // Vertex 10
			glVertex3d(     0, -tt,-dsize) ; // Vertex 17
			glVertex3d(    -t,    -t,  -t) ; // Vertex 16
			glVertex3d(-dsize,   0,   -tt) ; // Vertex 19
		glEnd() ;

    	glBegin(GL_POLYGON); // Face 10
			glNormal3d(-dsize, t, 0) ;
			glVertex3d(   -tt,dsize,   0) ; // Vertex  8
			glVertex3d(    -t,    t,  -t) ; // Vertex  9
			glVertex3d(-dsize,    0, -tt) ; // Vertex 19
			glVertex3d(-dsize,    0,  tt) ; // Vertex 14
			glVertex3d(    -t,    t,   t) ; // Vertex 15
		glEnd() ;

    	glBegin(GL_POLYGON); // Face 11
			glNormal3d(-dsize, -t, 0) ;
			glVertex3d(-dsize,     0, -tt) ; // Vertex 19
			glVertex3d(    -t,    -t,  -t) ; // Vertex 16
			glVertex3d(   -tt,-dsize,   0) ; // Vertex 13
			glVertex3d(    -t,    -t,   t) ; // Vertex 18
			glVertex3d(-dsize,     0,  tt) ; // Vertex 14
		glEnd() ;
	glEndList() ;

	glNewList(Dots, GL_COMPILE) ; OutputGlError("glNewList: Dodec") ;
		glBegin(GL_POINTS);
			glVertex3d(-dsize,     0, -tt) ;
			glVertex3d(    -t,    -t,  -t) ;
			glVertex3d(   -tt,-dsize,   0) ;
			glVertex3d(    -t,    -t,   t) ;
			glVertex3d(-dsize,     0,  tt) ;

			glVertex3d(0, 0, 0);
			glVertex3d(.1, 0, .1);
			glVertex3d(.2, 0, .2);
			glVertex3d(.3, 0, .3);
			glVertex3d(.4, 0, .4);

		glEnd();
	glEndList() ;


	wglMakeCurrent(NULL, NULL) ;
}

void CDlgRender::DrawScene() 
{
	// Set up some colors
	static GLdouble red[3] = {0.8, 0.0, 0.0 } ; 
	static GLdouble green[3] = {0.0, 0.75, 0.0} ;
	static GLdouble purple[3] = {1.0, 0.14, 0.6667} ; 

	// Enable lighting calculations
	glEnable(GL_LIGHTING) ; 		OutputGlError("glEnable (GL_LIGHTING) ;") ;
	glEnable(GL_LIGHT0) ;			OutputGlError("glEnable (GL_LIGHT0) ;") ;
	
	// Enable depth calculations
    glEnable(GL_DEPTH_TEST);  		OutputGlError("glEnable (GL_DEPTH_TEST);") ;

	// Clear the color and depth buffers
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f) ;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set the material color to follow the current color
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE) ;
	glEnable(GL_COLOR_MATERIAL) ;
    //
    // Draw the box.
    //		
   glMatrixMode(GL_MODELVIEW); 	OutputGlError("MatrixMode") ;
	
	// Translate and rotate the axis.
	glLoadIdentity();
    glTranslated(0.5, 0.5, -4.5 ); 
    glRotated(m_angle[Box].cx, 1.0, 0.0, 0.0);
    glRotated(m_angle[Box].cy, 0.0, 1.0, 0.0);

	// Change the current color to green.
	glColor3dv(green) ;

	// Draw the box.
	glCallList(Box) ;

	//
	// Draw the pyramid.
	//
	glLoadIdentity();
    glTranslated(-0.7, 0.5, -4.5 );
 	glRotated(m_angle[Pyramid].cx, 1.0, 0.0, 0.0);
    glRotated(m_angle[Pyramid].cy, 0.0, 1.0, 0.0);

	glColor3dv(purple) ;
	glCallList(Pyramid) ;

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

  	//
	// Draw the Dodecahedron.
	//
/*	glLoadIdentity();
    glTranslated(m_xTranslation,m_yTranslation,m_zTranslation);
 	glRotated( m_xRotation, 1.0, 0.0, 0.0);
    glRotated( m_yRotation, 0.0, 1.0, 0.0);

	glColor3dv(red) ;
	glCallList(Dodec) ;
*/
	// Draw the dots
	glLoadIdentity();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA);
	glPointSize(20.0);
	glTranslated(m_xTranslation,m_yTranslation,m_zTranslation);
	glRotated(/*m_angle[Dodec].cx*/ m_xRotation, 1.0, 0.0, 0.0);
    glRotated(/*m_angle[Dodec].cy*/ m_yRotation, 0.0, 1.0, 0.0);
	
	glColor4d(1.0, 0.0, 0.0, .5);
	glCallList(Dots) ;

    //
    // Flush the drawing pipeline.
    //
    glFlush();	
}

BOOL CDlgRender::OnEraseBkgnd(CDC *pDC)
{
	return TRUE;
}

void CDlgRender::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_LeftButtonDown = TRUE;
	m_LeftDownPos = point;
	SetCapture();
	
	CDialog::OnLButtonDown(nFlags, point);
}

void CDlgRender::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_RightButtonDown = FALSE;
	m_LeftButtonDown = FALSE;
	ReleaseCapture();
	
	CDialog::OnLButtonUp(nFlags, point);
}

void CDlgRender::OnRButtonDown(UINT nFlags, CPoint point) 
{
	m_RightButtonDown = TRUE;
	m_RightDownPos = point;
	SetCapture();
	
	CDialog::OnRButtonDown(nFlags, point);
}

void CDlgRender::OnRButtonUp(UINT nFlags, CPoint point) 
{
	m_RightButtonDown = FALSE;
	m_LeftButtonDown = FALSE;
	ReleaseCapture();
	
	CDialog::OnRButtonUp(nFlags, point);
}

void CDlgRender::OnMouseMove(UINT nFlags, CPoint point) 
{

	// Both : rotation
	if(m_LeftButtonDown && m_RightButtonDown)
	{
		m_xTranslation -= (float)(m_LeftDownPos.x - point.x) * m_SpeedTranslation;
		m_yTranslation += (float)(m_LeftDownPos.y - point.y) * m_SpeedTranslation;
		m_LeftDownPos = point;
		OnPaint();

	}

	else

	// Left : x / y translation
	if(m_LeftButtonDown)
	{
		if(m_xyRotation)
		{
			m_yRotation -= (float)(m_LeftDownPos.x - point.x) * m_SpeedRotation;
			m_xRotation -= (float)(m_LeftDownPos.y - point.y) * m_SpeedRotation;
		}
		else
		{
			m_zRotation -= (float)(m_LeftDownPos.x - point.x) * m_SpeedRotation;
			m_xRotation -= (float)(m_LeftDownPos.y - point.y) * m_SpeedRotation;
		}
		m_LeftDownPos = point;
		m_RightDownPos = point;
		OnPaint();
	}

	else

	// Right : z translation
	if(m_RightButtonDown)
	{
		m_zTranslation += (float)(m_RightDownPos.y - point.y) * m_SpeedTranslation;
		m_RightDownPos = point;
		OnPaint();
	}

	/*
	TRACE("\nPosition\n");
	TRACE("Translation : %g %g %g\n",m_xTranslation,m_yTranslation,m_zTranslation);
	TRACE("Rotation    : %g %g %g\n",m_xRotation,m_yRotation,m_zRotation);
	*/
		
	CDialog::OnMouseMove(nFlags, point);
}
