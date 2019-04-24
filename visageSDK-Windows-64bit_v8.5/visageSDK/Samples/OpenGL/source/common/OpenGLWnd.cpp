// OpenGLWnd.cpp : implementation file
//

#include "stdafx.h"
#include "OpenGLWnd.h"
#include "MainFrm.h"
#include "VRMLFitToScreenTraverser.h"
#include <gl\glaux.h>

#include <assert.h>
#include  <math.h>
#include  <stdlib.h>

#include <sys/timeb.h>
#include <sys/types.h>

#include <iostream>
#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




/////////////////////////////////////////////////////////////////////////////
// COpenGLWnd

	// -------------------------------------------------
	// My own code
	// -------------------------------------------------

const int COpenGLWnd::mTIMER_RENDERING_INTERVAL = 20;
const int COpenGLWnd::mTIMER_ROTATE_INTERVAL = 30;


/**
* Initialize the renderer with a VRMLModel to be rendered.
* The VRMLRenderer is initialized, and the camera parameters are automatically adjusted to fit the model to screen.
* The model is then added to the list of models to render. This function can be called several times with different
* models in case more than one model is rendered. To remove the model from the renderer, use VRMLModelRenderer::RemoveModel()
*
* @param theModel the pointer to a VRMLModel to be added to the renderer
*
* @see VRMLModelRenderer
* @see VRMLModelRenderer::RemoveModel()
*/
void COpenGLWnd::Init(VRMLModel *theModel)
{
	SetContext();
	BaseOGLVRMLModelRenderer::Init(theModel);

	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	
	// glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	
	// For performance we set all of these to fastest.
	glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	glHint(GL_POINT_SMOOTH_HINT, GL_FASTEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);
	
	// Can't assume that normals aren't scaled. Therefore I need this.
	glEnable(GL_NORMALIZE);
	
	// Reset modelview matrix before getting coords.
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	// Retrieve max and min in all axes and also the worl-transformed coordinates in a vector.
	VRMLFitToScreenTraverser *tempFitter = new VRMLFitToScreenTraverser;
	
	tempFitter->Reset();
	theModel->traverse(theModel->rootNode, tempFitter);
	GLfloat sceneMinX, sceneMaxX, sceneMinY, sceneMaxY, sceneMinZ, sceneMaxZ;
	tempFitter->GetCoords(sceneMinX, sceneMaxX, sceneMinY, sceneMaxY, sceneMinZ, sceneMaxZ, mMaxRadius);
	
	GLfloat distance = (GLfloat)((mMaxRadius / 2.0) / mVGLR_PERSPECT_TAN_FOVY);
	
	// Make this constant.
	mFarPlane = distance * 10;
	
	mXTranslateFit = - (GLfloat)((sceneMinX + (abs(sceneMaxX - sceneMinX) / 2.0)));
	mYTranslateFit = - (GLfloat)((sceneMinY + (abs(sceneMaxY - sceneMinY) / 2.0)));
	mZTranslateFit = - (GLfloat)((distance + (abs(sceneMinZ - sceneMaxZ) / 2.0)));
	
	mXTranslate = mXTranslateFit;
	mYTranslate = mYTranslateFit;
	mZTranslate = mZTranslateFit;
	
	// Translate and zoom acceleration depends on size of model.
	mXAccel = (GLfloat)(fabs(sceneMinX - sceneMaxX) * 0.005);
	mYAccel = (GLfloat)(fabs(sceneMinX - sceneMaxX) * 0.005);
	mZAccel = (GLfloat)(fabs(sceneMinZ - sceneMaxZ) * 0.005);
	
	
	delete tempFitter;
	
	SetRenderingMode(mRenderingMode);
	
	if(mWindowWidth && mWindowHeight)
		ReSizeGLScene(mWindowWidth, mWindowHeight);
	SwapGLBuffers();
}


/**
* Render all attached VRML models to screen using OpenGL commands.
* The actual rendering is implemented in VRMLRenderer, the parent class. 
*
* This function is called automatically by OnTimer() so only in special occassions it may
* be interesting to call it directly. One such occassion is just after a textured model is loaded
* in order to enforce loading of textures. In fact, when the VRMLModel is loaded from a file the
* texture file names are stored but texture images are not actually loaded. This is done by the 
* renderer when it renders the textured model first time. That can take some time, and it may be desirable
* to call RenderToScreen() and thus force the loading of textures. The function will return after the
* textures are loaded and the models rendered.
*/
void COpenGLWnd::RenderToScreen()
{
	static CMainFrame* pMainWnd = 0;
	struct _timeb timebuffer;
	
	_ftime( &timebuffer );
	
	long current_time = 1000 * (long)timebuffer.time + timebuffer.millitm;	//CHANGED BY I.K. added explicit cast
	static long last_times[50];
	static int cnt = -1;


	if(rendererBusy)   // a simple mechanism to make sure that we do not draw again before last rendering is done
	{
		return;
	}
	rendererBusy = true;

	SetContext();

	
	
	if(cnt == -1)
	{
		cnt = 0;
		for(int i=0;i<10;i++)
			last_times[i]=0;
	}
	cnt++;
	if(cnt == 50) cnt = 0;
	
	renderingFrameRate = 50000.0f / (float)(current_time - last_times[cnt]);
	
	last_times[cnt] = current_time;
	
	glMatrixMode(GL_MODELVIEW);
	if(modelViewMatrix)
	{
		glLoadMatrixf(modelViewMatrix);
	}
	else
	{
		glLoadIdentity();
		
		glTranslatef(mXTranslate, mYTranslate, mZTranslate);
		glRotatef(mXRotAngle, 1.0f, 0.0f, 0.0f);
		glRotatef(mYRotAngle, 0.0f, 1.0f, 0.0f);
	}

	BaseOGLVRMLModelRenderer::RenderToScreen();
	
	
	
	SwapGLBuffers();

	rendererBusy = false;

	// display the frame rate
	if(!pMainWnd) pMainWnd = (CMainFrame*)AfxGetMainWnd();
	char tmpmsg[100];
	sprintf(tmpmsg,"%2.2f FPS",renderingFrameRate);
	pMainWnd->m_wndStatusBar.SetPaneText( 0, tmpmsg, TRUE );
}

/**
* Close down the renderer.
* Tidy up and close the rendering system.
*/
void COpenGLWnd::Close()
{
	// Kill any pending timers.
	KillTimer(ID_ROTATE);
	
	SetContext();

	mXAccel = mYAccel = mZAccel = 0;

	mXTranslate = 0;
	mYTranslate = 0;
	mZTranslate = 0;

	mXRotAngle = 0;
	mYRotAngle = 0;


	BaseOGLVRMLModelRenderer::Close();
	SwapGLBuffers();
}

	
	// -------------------------------------------------
	// VisualC++ added code below
	// -------------------------------------------------

IMPLEMENT_DYNCREATE(COpenGLWnd, CView)

COpenGLWnd::COpenGLWnd()
{
	mLeftButtonDown = false;
	mRightButtonDown = false;
	m_bTimerSet = false;
	m_pDC = NULL;
	m_bSwap = true;
	bgtexture[0] = 0;

	mXTranslate = 0;
	mYTranslate = 0;
	mZTranslate = 0;

	mXRotAngle = 0;
	mYRotAngle = 0;
	// mZRotAngle = 0;

	mVGLR_PERSPECT_FOVY = 45.0f;
	mVGLR_PERSPECT_TAN_FOVY = tan(mVGLR_PERSPECT_FOVY / 2.0);
	mVGLR_PERSPECT_NEAR_PLANE = 0.1f;
	mFarPlane = 100.0f;

	mRenderingMode = VGLR_RENDERING_AS_IS;

	mBackfaceCull = true;

	modelViewMatrix = 0;
}

COpenGLWnd::~COpenGLWnd()
{
	wglMakeCurrent(0,0);
	wglDeleteContext(m_hRC);
	if( m_pDC )
	{
		delete m_pDC;
	}
}

// Standard OpenGL Init Stuff
BOOL COpenGLWnd::SetupPixelFormat()
{
	
  PIXELFORMATDESCRIPTOR pfdMain;

  memset( &pfdMain , 0 , sizeof( PIXELFORMATDESCRIPTOR ) );
  pfdMain.nSize = sizeof( PIXELFORMATDESCRIPTOR );
  pfdMain.nVersion = 1;
  pfdMain.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
  pfdMain.iPixelType = PFD_TYPE_RGBA;
  
	// Set to 16 for performance reasons.
	pfdMain.cColorBits = 16;
  pfdMain.cRedBits = 8;
  pfdMain.cRedShift = 16;
  pfdMain.cGreenBits = 8;
  pfdMain.cGreenShift = 8;
  pfdMain.cBlueBits = 8;
  pfdMain.cBlueShift = 0;
  pfdMain.cAlphaBits = 0;
  pfdMain.cAlphaShift = 0;
  pfdMain.cAccumBits = 64;
  pfdMain.cAccumRedBits = 16;
  pfdMain.cAccumGreenBits = 16;
  pfdMain.cAccumBlueBits = 16;
  pfdMain.cAccumAlphaBits = 0;
  pfdMain.cDepthBits = 32;
  pfdMain.cStencilBits = 8;
  pfdMain.cAuxBuffers = 0;
  pfdMain.iLayerType = PFD_MAIN_PLANE;
  pfdMain.bReserved = 0;
  pfdMain.dwLayerMask = 0;
  pfdMain.dwVisibleMask = 0;
  pfdMain.dwDamageMask = 0;

   // int m_nPixelFormat = ::ChoosePixelFormat( m_pDC->GetSafeHdc(), &pfdMain );
	int m_nPixelFormat = ::ChoosePixelFormat( m_pDC->GetSafeHdc(), &pfdMain );

    if ( m_nPixelFormat == 0 )
        return FALSE;

    // return ::SetPixelFormat( m_pDC->GetSafeHdc(), m_nPixelFormat, &pfdMain );
		return ::SetPixelFormat( m_pDC->GetSafeHdc(), m_nPixelFormat, &pfdMain );
}

/**
* Initialize OpenGL.
* Perform necessary OpenGL initializations, including the startup of the rendering timer through StartRenderingTimer().
*/
BOOL COpenGLWnd::InitOpenGL()
{
	static int init = 0;	//CHANGED BY I.K. added type specifier

	if(init) return true;
	init = 1;

	//Get a DC for the Client Area
	if (m_pDC != NULL) delete m_pDC;
	m_pDC = new CClientDC(this);

	//Failure to Get DC
	if( m_pDC == NULL )
		return FALSE;

	if( !SetupPixelFormat() )
		return FALSE;

	// Only one RC
	m_hRC = ::wglCreateContext( m_pDC->GetSafeHdc() );

    //Failure to Create Rendering Context
    if(m_hRC == 0 )
        return FALSE;

	//Make the RC Current
	if( ::wglMakeCurrent( m_pDC->GetSafeHdc(), m_hRC) == FALSE )
		return FALSE;

	// REMOVE
	if( ::wglMakeCurrent( NULL, NULL ) == FALSE )
			return FALSE;

	rendererBusy = false;

	StartRenderingTimer();

	return TRUE;
}

BEGIN_MESSAGE_MAP(COpenGLWnd, CView)
	//{{AFX_MSG_MAP(COpenGLWnd)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONUP()
	ON_WM_TIMER()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenGLWnd drawing

void COpenGLWnd::OnDraw(CDC* pDC)
{
	RenderToScreen();
}

/////////////////////////////////////////////////////////////////////////////
// COpenGLWnd diagnostics

#ifdef _DEBUG
void COpenGLWnd::AssertValid() const
{
	CView::AssertValid();
}

void COpenGLWnd::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// COpenGLWnd message handlers


/**
* Initialize OpenGL when window is created.
* 
*/
int COpenGLWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	if ( !InitOpenGL() )
	{
		MessageBox( "Error setting up OpenGL!", "Init Error!",
			MB_OK | MB_ICONERROR );
		return -1;
	}

	return 0;
}

// Override the errase background function to
// do nothing to prevent flashing.
BOOL COpenGLWnd::OnEraseBkgnd(CDC* pDC) 
{	
	return true;
}

// Set a few flags to make sure OpenGL only renders
// in its viewport.
BOOL COpenGLWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= (WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CS_OWNDC);

	cs.lpszClass = ::AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS,
	  ::LoadCursor(NULL, IDC_ARROW), NULL, NULL);
		
	return CView::PreCreateWindow(cs);
}

void COpenGLWnd::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	if ( 0 >= cx || 0 >= cy || nType == SIZE_MINIMIZED )
		return;

	SetContext();

	ReSizeGLScene(cx, cy);
}

void COpenGLWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
		if(nFlags & MK_CONTROL)
			LeftButtonControlDown(point.x, point.y);
		else
			LeftButtonDown(point.x, point.y);

		mLeftButtonDown = true;
}

void COpenGLWnd::OnRButtonDown(UINT nFlags, CPoint point) 
{
		RightButtonDown(point.x, point.y);	
		mRightButtonDown = true;
}

void COpenGLWnd::OnMouseMove(UINT nFlags, CPoint point) 
{

	if (!m_bTimerSet &&  mRightButtonDown) {
		SetTimer(ID_ROTATE, mTIMER_ROTATE_INTERVAL, NULL);
		m_bTimerSet = true;
		return;
	}
	
		if(nFlags & MK_LBUTTON)
		{
			// So we don't recieve events when user has not pressed anything.
			if(!mLeftButtonDown) return;
			
			SetContext();
			if(nFlags & MK_CONTROL)
				LeftButtonControlDrag(point.x, point.y);
			else if(!(nFlags & MK_SHIFT))
				LeftButtonDrag(point.x, point.y);
		}
		else if (nFlags & MK_RBUTTON)
		{
			if(!mRightButtonDown) return;
			RightButtonDrag(point.x, point.y);
		}
	
}

void COpenGLWnd::OnRButtonUp(UINT nFlags, CPoint point) 
{
		KillTimer(ID_ROTATE);
		m_bTimerSet = false;
		mRightButtonDown = false;
}

/**
* Start the rendering timer.
* Start the timer for the rendering. After this, the OnTimer() function
* is automatically called every mTIMER_RENDERING_INTERVAL milliseconds, and in turn it calls the render function. Thus we have
* a rendering function call every mTIMER_RENDERING_INTERVAL milliseconds.
*/
void COpenGLWnd::StartRenderingTimer()
{
	SetTimer(ID_RENDERING, mTIMER_RENDERING_INTERVAL, NULL);
}

/**
* Timer callback.
* This function is called by the timer. This is used to implement the rendering cycle.
*/
void COpenGLWnd::OnTimer(UINT_PTR nIDEvent) 
{
	switch(nIDEvent)
	{
		case ID_RENDERING:
			RenderToScreen();
			break;
		case ID_ROTATE:
			RightButtonDownTimerFunc();
			break;
	}
}

void COpenGLWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	SetContext();
		if(nChar == VK_F1)
			SetRenderingMode(VGLR_RENDERING_WIREFRAME);
		else if(nChar == VK_F2)
			SetRenderingMode(VGLR_RENDERING_AS_IS);
		else if(nChar == VK_F11)
			FitToScreen();
		else
			RenderToScreen();
	SwapGLBuffers();
}

void COpenGLWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	mLeftButtonDown = false;
}


/**
* Pre-render setup function.
* This function is called before the rendering in each rendering cycle. It sets up the GL context and clears the screen
* to prepare for the next frame to be rendered. 
*
* This is a good place to insert the rendering of other objects in the scene,
* background image or similar.
*/
void COpenGLWnd::SetContext()
{ 
	// set the context
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC); 
	
	
	// clear the frame buffer before next drawing
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	
	// INSERT CODE HERE TO RENDER ANY ADDITIONAL OBJECTS, BACKGROUND IMAGE OR SIMILAR
	DrawBackgroundImage();
}

int COpenGLWnd::LoadTexture(char *fullPath)
{
	FILE *File=NULL;
	File=fopen(fullPath,"r");

	if(!File)
	{
		return -1;
	}
	fclose(File);

	// Texture varible, return	
	GLuint texture[1];

	IplImage* img = cvLoadImage(fullPath,CV_LOAD_IMAGE_UNCHANGED);

	int nChannels = img->nChannels;

	cvFlip(img);

	glGenTextures(1, &texture[0]);

	texture[0] += 20; //leave some texture names for the tracker because it uses names starting from 1

	// Typical Texture Generation Using Data From The Bitmap
	glBindTexture(GL_TEXTURE_2D, texture[0]);

	// Load texture into GL.
	int maxSize = 0;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxSize);

	// Create a texture of nearest power 2
	int xPowSize = 1;
	int yPowSize = 1;
	while((img->width / xPowSize) > 0) xPowSize <<= 1;
	while((img->height / yPowSize) > 0) yPowSize <<= 1;
		
	// So we're not overriding gfx-cards capabilitys
	if(xPowSize > maxSize) xPowSize = maxSize;
	if(yPowSize > maxSize) yPowSize = maxSize;

	// Allocate memory to the scaled image.
	//IplImage* tempScaleImage = cvCreateImage(cvSize(xPowSize, yPowSize),img->depth, img->nChannels);
	//cvResize(img, tempScaleImage);

	GLubyte* tempScaleImage = new GLubyte[yPowSize * xPowSize * nChannels];

	// Scale original image to nearest lower power of 2.
	gluScaleImage(nChannels == 4 ? GL_RGBA : GL_RGB, img->width, img->height, GL_UNSIGNED_BYTE, img->imageData, xPowSize, yPowSize, GL_UNSIGNED_BYTE, tempScaleImage);

	glTexImage2D(GL_TEXTURE_2D, 0, nChannels, xPowSize, yPowSize, 0, nChannels == 4 ? GL_BGRA_EXT : GL_BGR_EXT, GL_UNSIGNED_BYTE, tempScaleImage);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	// Linear Filtering
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	// Linear Filtering

	delete [] tempScaleImage;
	cvReleaseImage(&img);
	//cvReleaseImage(&tempScaleImage);

	return texture[0];
}


/**
* Draw the background image on the screen.
* Load the background image from the file background.jpg or ../background.jpg the first time this function is called.
* If loading is succesful, the image is drawn on the screen every time the function is called.
* This function is called by SetContext(), so it is executed at the beginning of every frame.
*
* @return true if image is succesfully drawn, 0 otherwise
* @see SetContext()
*/
bool COpenGLWnd::DrawBackgroundImage()
{
	// draw the background image, if any
	static int noImage = 0;

	if(noImage)
		return false;



	if(!bgtexture[0])
	{
		bgtexture[0] = LoadTexture("background.jpg");
		if(bgtexture[0] == -1)
		{
			bgtexture[0] = LoadTexture("../background.jpg");
		}
		if(bgtexture[0] == -1)
		{
			noImage=1;
			return false;
		}
	}


	glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_VIEWPORT_BIT | GL_ENABLE_BIT | GL_FOG_BIT | GL_STENCIL_BUFFER_BIT | GL_TRANSFORM_BIT | GL_TEXTURE_BIT );
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND );
	glDisable(GL_DITHER);
	glDisable(GL_FOG);
	glDisable(GL_SCISSOR_TEST);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, bgtexture[0]);				// Select Our Texture

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();									// Reset The Projection Matrix

	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();									// Reset The Projection Matrix
	glOrtho(0.0f,1.0f,0.0f,mWindowHeight/(float)mWindowWidth,-10.0f,10.0f); // Keep the aspect ratio of the window
/*
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f,0.0f);		
	glVertex3f(0.0f,0.0f,-5.0f);

	glTexCoord2f(1.0f,0.0f);		
	glVertex3f(1.0f,0.0f,-5.0f);

	glTexCoord2f(1.0f,1.0f);		
	glVertex3f(1.0f,1.0f,-5.0f);

	glTexCoord2f(0.0f,1.0f);		
	glVertex3f(0.0f,1.0f,-5.0f);
	
	glEnd();
*/
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	const float W = 1.0f;
	const float H = 0.25f;
	const float X = 0.0f;
	float Y = (mWindowHeight/(float)mWindowWidth - H) / 2.0f;
	const float Z = -5.0f;

	GLfloat vertices[] = {
		X,      Y,      Z,
		X + W,  Y,      Z,
		X + W,  Y + H,  Z,
		X,      Y + H,  Z
	};
	
	static GLfloat tex_coords[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
 
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
 
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glTexCoordPointer(2, GL_FLOAT, 0, tex_coords);
 
	glDrawArrays(GL_QUADS, 0, 4);
 
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);


	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, 0);
	glPopAttrib();
	glClear(GL_DEPTH_BUFFER_BIT);


	return true;
	
	
}


void COpenGLWnd::ReSizeGLScene(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window
{
	if (height==0)										// Prevent A Divide By Zero By
	{
		height=1;										// Making Height Equal One
	}

	mWindowWidth = width;
	mWindowHeight = height;

	glViewport(0,0,width,height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	gluPerspective(mVGLR_PERSPECT_FOVY, (GLfloat)width/(GLfloat)height, mVGLR_PERSPECT_NEAR_PLANE, mFarPlane);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
}


void COpenGLWnd::SetModelViewMatrix(GLfloat *m)
{
	modelViewMatrix = m;
}


void COpenGLWnd::ToggleBackfaceCull() 
{ 
	mBackfaceCull = !mBackfaceCull; 
	SetRenderingMode(mRenderingMode); 
}

void COpenGLWnd::SetRenderingMode(VGLR_RENDERING_MODE newMode)
{
	switch(newMode)
	{
		case VGLR_RENDERING_WIREFRAME:
		{
			// Set wireframe mode.
			if(mBackfaceCull)
				glPolygonMode(GL_FRONT, GL_LINE);
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			
			// Still want lighting tho..
			glEnable(GL_LIGHTING);
			glEnable(GL_LIGHT0);

			// Material
			glDisable(GL_COLOR_MATERIAL);

			// Textures
			glDisable(GL_TEXTURE_2D);
	
			if(mBackfaceCull)
			{
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
			}
			else
				glDisable(GL_CULL_FACE);
			
		}
		break;

		case VGLR_RENDERING_SOLID_NO_MATERIAL:
		{
		}
		break;

		case VGLR_RENDERING_MATERIAL:
		{
		}
		break;

		case VGLR_RENDERING_AS_IS:
		{
			if(mBackfaceCull)
				glPolygonMode(GL_FRONT, GL_FILL);
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			// Lightning.
			glEnable(GL_LIGHTING);
			glEnable(GL_LIGHT0);

			// Textures
			glEnable(GL_TEXTURE_2D);

			if(mBackfaceCull)
			{
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
			}
			else
				glDisable(GL_CULL_FACE);
		}
		break;
	}

	mRenderingMode = newMode;
}


void COpenGLWnd::LeftButtonDown(int x, int y)
{
	mLeftButtonX = (GLfloat)x;
	mLeftButtonY = (GLfloat)y;
}

void COpenGLWnd::RightButtonDown(int x, int y)
{
	mRightButtonX = (GLfloat)x;
	mRightButtonY = (GLfloat)y;
	mRotateXAngleAdd = 0;
	mRotateYAngleAdd = 0;

	// SetTimer(mhWnd, (UINT) IGLW_TIMER_ROTATE, 100, NULL);
	// SetCapture(mhWnd);
}

void COpenGLWnd::LeftButtonControlDown(int x, int y)
{
	// Y gives the zoom factor in Z direction.
	mLeftButtonControlZ = (GLfloat)y;
}

void COpenGLWnd::LeftButtonDrag(int x, int y)
{

	mXTranslate += (x - mLeftButtonX) * mXAccel;

	// Windows y-axis is reversed to openGL.
	mYTranslate -= (y - mLeftButtonY) * mYAccel;

	// Save off current position
	mLeftButtonX = (GLfloat)x;
	mLeftButtonY = (GLfloat)y;

	// Redraw screen.
	RenderToScreen();
}

void COpenGLWnd::RightButtonDrag(int x, int y)
{
	// Rotates object around X or Y axis, distance from first point gives angular rotation speed.
	// Y axis rotation is given by distance in X coords, and vice versa.
	mRotateXAngleAdd = (GLfloat)((y - mRightButtonY) * 0.1);
	mRotateYAngleAdd = (GLfloat)((x - mRightButtonX) * 0.1);
}

void COpenGLWnd::LeftButtonControlDrag(int x, int y)
{
	mZTranslate += (y - mLeftButtonControlZ) * mZAccel;
	mLeftButtonControlZ = (GLfloat)y;

	RenderToScreen();
}

void COpenGLWnd::RightButtonDownTimerFunc()
{
 	mXRotAngle += mRotateXAngleAdd;
	mYRotAngle += mRotateYAngleAdd;

	// Keep it within 360 degrees so that we don't get unpleasant surprises if datatype overflows. 
	if(mXRotAngle > 360.0f)
		mXRotAngle -= 360.0f;
	if(mXRotAngle < 0.0f)
		mXRotAngle += 360.0f;
	
	if(mYRotAngle > 360.0f)
		mYRotAngle -= 360.0f;
	if(mYRotAngle < 0.0f)
		mYRotAngle += 360.0f;

	RenderToScreen();
}

void COpenGLWnd::FitToScreen()
{
	mXTranslate = mXTranslateFit;
	mYTranslate = mYTranslateFit;
	mZTranslate = mZTranslateFit;
	
	mXRotAngle = 0;
	mYRotAngle = 0;

	RenderToScreen();
}

void COpenGLWnd::FitToScreen(float zoom)
{
	mXTranslate = mXTranslateFit;
	mYTranslate = mYTranslateFit;
	mZTranslate = mZTranslateFit * zoom;
	
	mXRotAngle = 0;
	mYRotAngle = 0;

	RenderToScreen();
}
