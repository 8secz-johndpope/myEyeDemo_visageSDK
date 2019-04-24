// OpenGLWnd.cpp : implementation file
//

#include "stdafx.h"

#include "OpenGLWndFaceTracker.h"
#include "MainFrm.h"
#include <process.h>

#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include <sys/timeb.h>
#include <sys/types.h>

#include <iostream>
#include <fstream>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ANIM_VIEW_SIZE 0.40f
#define SLIDER_RESOLUTION 200


/////////////////////////////////////////////////////////////////////////////
// COpenGLWnd

typedef BOOL (WINAPI * PFNWGLSWAPINTERVALEXTPROC) (int interval);
PFNWGLSWAPINTERVALEXTPROC       wglSwapIntervalEXT = NULL;

/**
* This function is used to render texture on screen.
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
		
		//glTranslatef(mXTranslate, mYTranslate, mZTranslate);
		glRotatef(mXRotAngle, 1.0f, 0.0f, 0.0f);
		glRotatef(mYRotAngle, 0.0f, 1.0f, 0.0f);
	}

	SwapGLBuffers();

	rendererBusy = false;

/*	// display the frame rate
	if(!pMainWnd) pMainWnd = (CMainFrame*)AfxGetMainWnd();
	char tmpmsg[100];
	sprintf(tmpmsg,"%2.2f FPS",renderingFrameRate);
	pMainWnd->m_wndStatusBar.SetPaneText( 0, tmpmsg, TRUE );*/
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

	SwapGLBuffers();
}

IMPLEMENT_DYNCREATE(COpenGLWnd, CView)

COpenGLWnd::COpenGLWnd()

: otvoren_dialog(false)

{
	m_bTimerSet = false;
	m_pDC = NULL;
	m_bSwap = true;
	bgtexture[0] = 0;
	insttexture[0] = 0;

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

	mBackfaceCull = true;

	modelViewMatrix = 0;

	trackerActive=false;
	
	
}

COpenGLWnd::~COpenGLWnd()
{
	wglMakeCurrent(0,0);
	wglDeleteContext(m_hRC);
	wglDeleteContext(m_hRC1);
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
* Perform necessary OpenGL initializations.
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

	m_hRC = ::wglCreateContext( m_pDC->GetSafeHdc() );
	m_hRC1 = ::wglCreateContext( m_pDC->GetSafeHdc() );

	//Failure to Create Rendering Context
	if(m_hRC == 0 )
		return FALSE;
	if(m_hRC1 == 0 )
		return FALSE;

	//Make the RC Current
	if( ::wglMakeCurrent( m_pDC->GetSafeHdc(), m_hRC) == FALSE )
		return FALSE;

	// REMOVE
	if( ::wglMakeCurrent( NULL, NULL ) == FALSE )
			return FALSE;

	rendererBusy = false;

//	StartRenderingTimer();


	return TRUE;
}

BEGIN_MESSAGE_MAP(COpenGLWnd, CView)
	//{{AFX_MSG_MAP(COpenGLWnd)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONUP()
	ON_WM_TIMER()
	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenGLWnd drawing

void COpenGLWnd::OnDraw(CDC* pDC)
{
	if(!trackerActive)   //disable OpenGL calls during tracking so they do not disturb tracking
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
/** Pre-render setup function.
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
	if(!trackerActive) // if tracker is active, do not clear screen
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	
	// INSERT CODE HERE TO RENDER ANY ADDITIONAL OBJECTS, BACKGROUND IMAGE OR SIMILAR
	DrawBackgroundImage();
}

void COpenGLWnd::SwapGLBuffers() 
{
	if(trackerActive) // if tracker is active, do not swap buffers
		return;
	SwapBuffers(m_pDC->GetSafeHdc());
	wglMakeCurrent(NULL, NULL);
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

	IplImage* img = cvLoadImage(fullPath);

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
		
	// So we're not overriding gfx-cards capability
	if(xPowSize > maxSize) xPowSize = maxSize;
	if(yPowSize > maxSize) yPowSize = maxSize;

	// Allocate memory to the scaled image.
	GLubyte* tempScaleImage = new GLubyte[yPowSize * xPowSize * 3];

	// Scale original image to nearest lower power of 2.
	gluScaleImage(GL_RGB, img->width, img->height, GL_UNSIGNED_BYTE, img->imageData, xPowSize, yPowSize, GL_UNSIGNED_BYTE, tempScaleImage);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, xPowSize, yPowSize, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, tempScaleImage);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	// Linear Filtering
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	// Linear Filtering

	delete [] tempScaleImage;
	cvReleaseImage(&img);

	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC) wglGetProcAddress("wglSwapIntervalEXT");
	wglSwapIntervalEXT(0);

	return texture[0];
}

/**
* Draw the background image on the screen.
* Load the background image from the file background.jpg the first time this function is called.
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
			bgtexture[0] = LoadTexture("../OpenGL/data/background.jpg");
		}
		if(bgtexture[0] == -1)
		{
			noImage=1;
			return false;
		}
	}

	int animViewWidth,animViewHeight;

	animViewWidth = (int)(ANIM_VIEW_SIZE*mWindowWidth);
	animViewHeight = (animViewWidth * 3) / 4; // 3/4 aspect ratio
	if(animViewHeight > mWindowHeight)
		animViewHeight = mWindowHeight;

	animViewWidth = mWindowWidth;
	animViewHeight = mWindowHeight;


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
	glBindTexture(GL_TEXTURE_2D, bgtexture[0]);			// Select Our Texture

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();									// Reset The Projection Matrix

	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();									// Reset The Projection Matrix
	glOrtho(0.0f,1.0f,0.0f,animViewHeight/(float)animViewWidth,-10.0f,10.0f);

	const float W = 1.0f;
	const float H = 0.25f;
	const float X = 0.0f;
	float Y = (animViewHeight/(float)animViewWidth - H) / 2.0f;
	const float Z = -5.0f;

	GLfloat vertices[] = {
		X,      Y,      Z,
		X + W,  Y,      Z,
		X + W,  Y + H,  Z,
		X,      Y + H,  Z
	};
	
	GLfloat vertices2[] = {
		0.0f,   0.0f,      Z,
		1.0f,  0.0f,      Z,
		1.0f,  1.0f,  Z,
		0.0f,      1.0f,  Z
	};
	
	static GLfloat tex_coords[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
 
	glEnableClientState(GL_VERTEX_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, vertices2);
	glDrawArrays(GL_QUADS, 0, 4);


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


	return TRUE;
	
	
}

bool COpenGLWnd::DrawInstructionsAndInitStatus(bool face_detected,float yaw)
{
	// draw the instructions image, if any
	static int noImage = 0;

	if(noImage)
		return false;

	if(!insttexture[0])
	{
		insttexture[0] = LoadTexture("InitializationInstructions.jpg");
		if(insttexture[0] == -1)
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
	glBindTexture(GL_TEXTURE_2D, insttexture[0]);				// Select Our Texture

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();									// Reset The Projection Matrix

	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();									// Reset The Projection Matrix
	glOrtho(0.0f,1.0f,0.0f,1.0f,-10.0f,10.0f);

	glColor3ub(255,255,255);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f,0.0f);		
	glVertex3f(0.0f,0.0f,-5.0f);

	glTexCoord2f(1.0f,0.0f);		
	glVertex3f(1.0f,0.0f,-5.0f);

	glTexCoord2f(1.0f,1.0f);		
	glVertex3f(1.0f,0.25f,-5.0f);

	glTexCoord2f(0.0f,1.0f);		
	glVertex3f(0.0f,0.25f,-5.0f);
	
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
	glPopAttrib();

	if(face_detected)
	{
		//draw yaw indicator
		glColor3ub(255,0,0);
		glBegin(GL_QUADS);
		glVertex3f(0.5f,0.25f,-5.0f);
		glVertex3f(0.5f-30.0f*yaw,0.25f,-5.0f);
		glVertex3f(0.5f-30.0f*yaw,0.3f,-5.0f);
		glVertex3f(0.5f,0.3f,-5.0f);

		//draw midline of the yaw indicator
		glColor3ub(0,255,0);
		glBegin(GL_QUADS);
		glVertex3f(0.5f-0.01f,0.25f,-5.0f);
		glVertex3f(0.5f+0.01f,0.25f,-5.0f);
		glVertex3f(0.5f+0.01f,0.3f,-5.0f);
		glVertex3f(0.5f-0.01f,0.3f,-5.0f);

		glEnd();
		//if(yaw > 0.0f)
		//	sprintf(tmpmsg,"------------|               YAW %4.1f mm | ROLL %+5.1f degrees | Velocity %+5.1f mm/s",yaw*1000.0f,roll,velocity*1000.0f);
		//else
		//	sprintf(tmpmsg,"            |------------   YAW %4.1f mm | ROLL %+5.1f degrees | Velocity %+5.1f mm/s",yaw*1000.0f,roll,velocity*1000.0f);
	}

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();



	glClear(GL_DEPTH_BUFFER_BIT);


	return TRUE;
	
	
}

void COpenGLWnd::ReSizeGLScene(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window
{
	if (height==0)										// Prevent A Divide By Zero By
	{
		height=1;										// Making Height Equal One
	}

	mWindowWidth = width;
	mWindowHeight = height;
	int animViewPos,animViewWidth,animViewHeight;

	animViewPos = (int)((1-ANIM_VIEW_SIZE)*width);
	animViewWidth = (int)(ANIM_VIEW_SIZE*width);
	animViewHeight = (animViewWidth * 3) / 4; // 3/4 aspect ratio
	if(animViewHeight > height)
		animViewHeight = height;

	animViewPos = 0;
	animViewWidth = width;
	animViewHeight = height;

	glViewport(animViewPos,0,animViewWidth,animViewHeight);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

//	gluPerspective(mVGLR_PERSPECT_FOVY, (GLfloat)animViewWidth/(GLfloat)animViewHeight, mVGLR_PERSPECT_NEAR_PLANE, mFarPlane);
	float f = 3.0f;
	GLfloat x_offset = 1;
	GLfloat y_offset = 1;
	width = animViewWidth;
	height = animViewHeight;
	if (width > height)
		x_offset = ((GLfloat)width)/((GLfloat)height);
	else if (width < height)
		y_offset = ((GLfloat)height)/((GLfloat)width);
	//set frustum specs
	GLfloat frustum_near = mVGLR_PERSPECT_NEAR_PLANE;
	GLfloat frustum_far = mFarPlane; 
	GLfloat frustum_x = x_offset*frustum_near/f;
	GLfloat frustum_y = y_offset*frustum_near/f;
	//set frustum
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-frustum_x,frustum_x,-frustum_y,frustum_y,frustum_near,frustum_far);
	glMatrixMode(GL_MODELVIEW);
	//clear matrix
	glLoadIdentity();
	//camera in (0,0,0) looking at (0,0,1) up vector (0,1,0)
	gluLookAt(0,0,0,0,0,1,0,1,0);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
}

void COpenGLWnd::displayMessage(char *userMessage,char *caption,int type)
{
	MessageBox(userMessage,caption,type);
}

void COpenGLWnd::setOpenGLContext()
{
	// set the context
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC1); 
	glViewport(0,0,mWindowWidth,mWindowHeight);						// Reset The Current Viewport
	
	// clear the frame buffer before next drawing
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	//verify color depth - FaceTracker needs 8 bits for each color
	static bool verified = false; // verify only once
	if(!verified)
	{
		verified = true;
		GLint redBits=0, greenBits=0, blueBits=0;

		glGetIntegerv (GL_RED_BITS, &redBits);
		glGetIntegerv (GL_GREEN_BITS, &greenBits);
		glGetIntegerv (GL_BLUE_BITS, &blueBits);

		if(redBits < 8 || greenBits < 8 || blueBits < 8)
		{
			MessageBox("The color depth of your graphics system is too low. FaceTracker2 requires 32 bits per pixel (True Color) for correct operation. Please change your graphics settings to True Color or 32 bit color depth.","Error",MB_ICONERROR);
			exit(0);
		}
	}
}

void COpenGLWnd::swapOpenGLBuffers()
{
	SwapBuffers(m_pDC->GetSafeHdc());
	wglMakeCurrent(NULL, NULL);
}

void COpenGLWnd::checkOpenGLSize(unsigned int &width, unsigned int &height)
{
	if(mWindowWidth < (int)width || mWindowHeight < (int)height)
	{
		//MessageBox("The application window is too small for tracking and this may result in degraded tracking performance. It is recommended to restart the tracker and increase the application window size to full screen; alternatively, this warning message can be avoided by decreasing the max_init_image_width and max_work_image_width parameters in the tracker configuration file.","Warning",MB_ICONWARNING);
	}
	width = mWindowWidth;
	height = mWindowHeight;
}


int picker_coordinates_array[10];
int picker_coordinates_count;
int picker_cancel;
IplImage* tmpImage;
IplImage* textImage;

CvFont font;


