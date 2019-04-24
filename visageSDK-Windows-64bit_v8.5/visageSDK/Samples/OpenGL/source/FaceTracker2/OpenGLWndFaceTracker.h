#if !defined(AFX_OPENGLWND_H__83310F64_5D53_44AE_9D08_E29D6A961D6F__INCLUDED_)
#define AFX_OPENGLWND_H__83310F64_5D53_44AE_9D08_E29D6A961D6F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OpenGLWndFaceTracker.h : header file
//

#include <windows.h>		// Header File For Windows
#include <gl\gl.h>
#include <gl\glaux.h>

#include "visageVision.h"

#include <cv.h>
#include "highgui.h"

class CExample3Doc;


using namespace VisageSDK;


/** The OpenGL-enabled window with rendering and basic interaction.
* This class provides OpenGL context that is needed to visualize tracking results obtained from VisageTracker through FaceData.
*/
class COpenGLWnd : public CView
{
public:

	bool DrawBackgroundImage();
	bool DrawInstructionsAndInitStatus(bool face_detected,float yaw);
	int LoadTexture(char *fullPath);


	/** Implementation of a virtual function from VisageSDK::TrackerGUIInterface.
	*
	* VisageTracker calls this function when it needs to display a simple
	* text prompt to the user. The function displays the message and
	* lets the user click "OK" to continue. Essentially it is
	* the same as Windows MessageBox function. Indeed, this implementation
	* simply calls the Windows MessageBox function.
	*
	* @param userMessage message to be displayed
	* @param caption the caption to be displayed on the message window
	* @param type type of message
	*/
	void displayMessage(char *userMessage,char *caption,int type);

	/** This function is called by method displayResults when it needs to clear the OpenGL
	* window and set the OpenGL context. The function sets to OpenGL context
	* to a context dedicated to the tracker, then clears the window.
	*
	*/
	void setOpenGLContext();

	/** This function is called by method displayResults when it needs to swap
	* OpenGL buffers. The function calls the OpenGL commands to swap buffers.
	*/
	void swapOpenGLBuffers();

	/* Implementation of a virtual function from VisageSDK::TrackerOpenGLInterface.
	*
	* VisageTracker calls this function to verify the window size
	* of the OpenGL context. It passes the requested size to
	* this function. The function must return the actual size of the
	* window.
	*
	* If the actual size is smaller than the requested size, a warining is issued
	* to the user. The tracker will then use the smaller
	* window but this may result in degradation of tracking performance.
	*
	* An improved implementation of this function may attempt to increase the window to the requested size.
	*
	* @param width input: requested window width, output: actual window width
	* @param height input: requested window height, output: actual window height
	*/
	void checkOpenGLSize(unsigned int &width, unsigned int &height);

protected:

	// ID's to identify callbacks on timers.
	enum ID_TIMER_EVENTS {
		ID_ROTATE = 1, 
		ID_RENDERING = 2 };

	static const int mTIMER_ROTATE_INTERVAL;
	static const int mTIMER_RENDERING_INTERVAL;

	PIXELFORMATDESCRIPTOR pfdMain;

	bool rendererBusy;

	// needed for background image drawing
	GLuint bgtexture[1];

	// needed for instructions image drawing
	GLuint insttexture[1];


	// -------------------------------------------------
	// VisualC++ added code below
	// -------------------------------------------------
	
protected:
	COpenGLWnd();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(COpenGLWnd)

// Attributes
public:
	// void (*m_RenderScene) ( CViewerDoc* doc );	// void function pointer to the rendering
												// function. Used to change to easily
												// change what a viewport displays.
protected:
	/** Rendering context for drawing the 3D model.
	* It is mapped to a viewport in the lower right corner of the main window.
	*
	*/
	HGLRC m_hRC; //Rendering Context
	/** Rendering context for tracking.
	* It is mapped to a viewport that takes the majority of the main window.
	*
	*/
	HGLRC m_hRC1; //Rendering Context
	CDC* m_pDC;  //Device Context

	// HDC m_hDC;

// Operations
public:
	bool m_bSwap;
	// OpenGL init stuff
	BOOL SetupPixelFormat();
	BOOL InitOpenGL();
	
	// Each viewport uses its own context
	// so we need to make sure the correct
	// context is set whenever we make an
	// OpenGL command. Here we also clear the screen.
	void SetContext();
	void SwapGLBuffers();

	bool trackerActive; // set to true while tracking to disable OpenGL calls that would disturb the tracker

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COpenGLWnd)
	public:
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~COpenGLWnd();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(COpenGLWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	bool m_bShowPopup;
	bool m_bTimerSet;

	
private:
	int var;
	int tmp;
	bool side;
	

	/**
	* Initialise.
	* Initialise the renderer with a VRMLModel to render. Standard rendering mode is set.
	* The view is set such that the model is fully visible on screen.
	*
	* This function can be called several times with different
	* models in case more than one model is rendered. Up to 1000 models are supported.
	*/
public:

	/**
	* Resize window.
	* Resize And Initialize The GL Window.
	* @param width the width of the window.
	* @param height the height of the window.
	*/
	void ReSizeGLScene(GLsizei width, GLsizei height);

	/**
	* Render the model to screen.
	* Render the loaded model to screen. If no model is loaded, clears the screen.
	*/
	void RenderToScreen();

	/**
	* Closes down and resets renderer. 
	* Call this before Initing next model.
	*/
	void Close();

	/**
	* Frame rate. Calculated based on the last 50 rendered frames.
	*/
	float renderingFrameRate;

	GLsizei mWindowWidth, mWindowHeight;

private:
	// Current translationvalues.
	GLfloat mXTranslate, mYTranslate, mZTranslate;

	// modelViewMatrix set by the user, if any
	GLfloat *modelViewMatrix;

	GLfloat mXAccel, mYAccel, mZAccel, Accel;

	GLfloat mXRotAngle;
	GLfloat mYRotAngle;

	GLfloat mMaxRadius;

	double mVGLR_PERSPECT_FOVY;
	double mVGLR_PERSPECT_TAN_FOVY;
	double mVGLR_PERSPECT_NEAR_PLANE;

	GLfloat mFarPlane;

	// Backface culling.
	bool mBackfaceCull;
private:
	float *faceShape;

private:
	float* faceScale;
private:
	float* faceTranslation;

private:
	float* faceRotation;
	

private:
	bool otvoren_dialog;

public:
	int zatvoren_dialog;
private:
	int nRet;
	float width;
	float height;
	float image_width;
	float image_height;
	float widthRight;
	float widthLeft;
	float point_x;
	float imageTop;
	float imageBottom;
	float point_y;
	float centarX;
	float centarY;

	VisageTracker *vt2;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPENGLWND_H__83310F64_5D53_44AE_9D08_E29D6A961D6F__INCLUDED_)
