#if !defined(AFX_OPENGLWND_H__83310F64_5D53_44AE_9D08_E29D6A961D6F__INCLUDED_)
#define AFX_OPENGLWND_H__83310F64_5D53_44AE_9D08_E29D6A961D6F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OpenGLWnd.h : header file
//

#include "BaseOGLVRMLModelRenderer.h"

#include <cv.h>
#include <highgui.h>


class CExample3Doc;


using namespace VisageSDK;


/** The OpenGL-enabled view with rendering and basic interaction.
* Handles all the basic OpenGL initializations and, through VRMLRenderer, provides
* the rendering and basic mouse and keyboard interaction (camera control, rendering modes).
* It can render one or more objects of type VRMLModel; these are typically the virtual character models, but
* other objects can be loaded and rendered in the scene.
* 
* Unless there are special needs other than regular real-time rendering
* and interaction, it is recommended to simply reuse
* this class as it is by copying the OpenGLWnd.cpp and OpenGLWnd.h files
* into your new project, and use it in the same way as demonstrated
* in this example. If you are building a new application, the detailed step-by-step
* procedure is given in the 
* <a href="../../../../doc/index.html#building">section on Building a new application from scratch on the visage|SDK main page</a>. 
* In this case, it should not be necessary to enter the
* details of this class.
*
* This class implements the following interactions that are available to the user:
* - Right mouse button + move mouse rotates the model.
* - Left mouse button + move mouse translates the model.
* - Control + left mouse button + move mouse zooms image.
* - F11 repositions model to center of window.
* - F7 toggles rendering to whole model or just parts of it. If F7 is pressed separate pieces of the model can be rendered (assuming that the VRML file consists of separate parts for the eyes, teeth etc.). Which part to render is controlled by pressing F4 for advancing separate part to render and F3 for regressing part to render. This is helpful  for modeling objects which are totally (or partially) obscured by other objects, for example to model teeth and eyes.
* - F1 sets rendering mode to wireframe.
* - F2 sets rendering mode to solid.
* If it is desired to bypass the interactive camera control, it is possible to control the camera using
* the COpenGLWnd::SetModelViewMatrix() function.
*
* In this example it is also demonstrated how to add a background image to the application. This is done in the function 
* SetContext(), which is called in every frame before the rendering. This function is a good place to insert other effects as well.
*
* For the user interested to modify the rendering process or implement their own, here is how it works. First, the renderer is initialized
* by calling COpenGLWnd::InitOpenGL() (this is done in COpenGLWnd::OnCreate() so the application developer does not need to do it). The COpenGLWnd::InitOpenGL() function initializes the rendering, and starts the rendering timer. The timer 
* calls the function COpenGLWnd::RenderToScreen() (through the function OnTimer()) every 20 milliseconds (mTIMER_RENDERING_INTERVAL).
* The COpenGLWnd::RenderToScreen() renders all models that are attached to the renderer. The actual implementation of the rendering
* is in the parent class, VRMLRenderer::RenderToScreen(). The models are attached to the renderer
* using the COpenGLWnd::Init() function. In this example this is done in CExample1Doc::LoadFaceModelFile(). Once the model is attached to
* the renderer, the renderer will keep rendering it until it is removed. Note that the rendering loop (implemented by the
* timer mechanism) and the animation loop(s) implemented by FAPlayer are running asynchronously in separate threads. The FAPlayer
* updates the VRMLModel in its animation loop, and the renderer (COpenGLWnd) renders it in every rendering cycle. To avoid
* rendering partially animated models, the flag VRMLModel::locked is used. This flag is set by both animation and rendering
* loops during critical operation to make sure that the other loop waits until the operation is finished. In this way, multiple
* virtual characters can be animated, each in its own FAPlayer running in a separate thread, and they are all rendered by
* the renderer.
*/
class COpenGLWnd : public CView,  public BaseOGLVRMLModelRenderer
{
friend class VideoExporter;
public:

	bool DrawBackgroundImage();
	int COpenGLWnd::LoadTexture(char *fullPath);

protected:

	// ID's to identify callbacks on timers.
	enum ID_TIMER_EVENTS {
		ID_ROTATE = 1, 
		ID_RENDERING = 2 };

	static const int mTIMER_ROTATE_INTERVAL;
	static const int mTIMER_RENDERING_INTERVAL;

	PIXELFORMATDESCRIPTOR pfdMain;

	// We can recieve mouse move events even though left button has not been pressed.
	// This occurs for example when a dialogbox is opened upon the window.
	bool mLeftButtonDown;

	bool mRightButtonDown;

	bool rendererBusy;

	// needed for background image drawing
	GLuint bgtexture[1];


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
	HGLRC m_hRC; //Rendering Context
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
	void SwapGLBuffers() { SwapBuffers(m_pDC->GetSafeHdc());wglMakeCurrent(NULL, NULL);};
	void StartRenderingTimer();

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
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	bool m_bShowPopup;
	bool m_bTimerSet;








public:

	/**
	* The rendering mode.
	* The rendering mode can be:
	* - VGLR_RENDERING_WIREFRAME for the wireframe mode
	* - VGLR_RENDERING_AS_IS for the "standard" lighted, smooth shaded mode
	*/
	enum VGLR_RENDERING_MODE {
		VGLR_RENDERING_WIREFRAME,
		VGLR_RENDERING_SOLID_NO_MATERIAL,
		VGLR_RENDERING_MATERIAL,
		VGLR_RENDERING_AS_IS };



	/**
	* Initialise.
	* Initialise the renderer with a VRMLModel to render. Standard rendering mode is set.
	* The view is set such that the model is fully visible on screen.
	*
	* This function can be called several times with different
	* models in case more than one model is rendered. Up to 1000 models are supported.
	*
	* @param theModel the model to load in the renderer.
	*/
	void Init(VRMLModel *theModel);

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
	* Set the model view matrix (for camera control).
	* 
	* This can be used to control the camera view. This matrix will be loaded using glLoadMatrixf(). glMatrixMode is set to GL_MODELVIEW.
	* If m is 0, the default camera control mode is used. In the default mode, the renderer first sets the view
	* automatically based on the size of the model, and then the camera is controlled interactively by the user
	* using the mouse.
	*
	* @param m the modelview matrix to be loaded.
	*/
	void SetModelViewMatrix(GLfloat *m);

	/**
	* Sets the renderingmode.
	* The rendering mode can be:
	* - VGLR_RENDERING_WIREFRAME for the wireframe mode
	* - VGLR_RENDERING_AS_IS for the "standard" lighted, smooth shaded mode
	*/
	void SetRenderingMode(VGLR_RENDERING_MODE newMode);


	/**
	* The interaction for pressing the left mouse button.
	* The left mouse button is used for moving the model horizontaly and verticaly.
	* @param x x position of the mouse.
	* @param y y position of the mouse.
	*/
	void LeftButtonDown(int x, int y);

	/**
	* The interaction for pressing the right mouse button.
	* The right mouse button is used for rotating the model.
	* @param x x position of the mouse.
	* @param y y position of the mouse.
	*/
	void RightButtonDown(int x, int y);

	/**
	* The interaction for pressing the left mouse button while CTRL is held.
	* CTRL + left mouse button is used for zooming.
	* @param x x position of the mouse.
	* @param y y position of the mouse.
	*/
	void LeftButtonControlDown(int x, int y);

	/**
	* The interaction for dragging the left mouse button.
	* The left mouse button is used for moving the model horizontaly and verticaly.
	* @param x x position of the mouse.
	* @param y y position of the mouse.
	*/
	void LeftButtonDrag(int x, int y);

	/**
	* The interaction for dragging the right mouse button.
	* The right mouse button is used for rotating the model.
	* @param x x position of the mouse.
	* @param y y position of the mouse.
	*/
	void RightButtonDrag(int x, int y);

	/**
	* The interaction for dragging the left mouse button while CTRL is held.
	* CTRL + left mouse button is used for zooming.
	* @param x x position of the mouse.
	* @param y y position of the mouse.
	*/
	void LeftButtonControlDrag(int x, int y);

	/**
	* The interaction for dragging the right mouse button.
	* The right mouse button is used for rotating the model. This function is called by a timer
	* in order to keep rotating the model when the user drags the mouse with the right button
	* down, then holds it in place while still keeping the button down.
	*/
	void RightButtonDownTimerFunc();


	/**
	* Toggle backface culling.
	* Toggle the backface culling on or off.
	*/
	void ToggleBackfaceCull();

	/**
	* Re-positions model onscreen.
	* Re-positions model onscreen based on the previously obtained 
	* measurements of the model. Basically, this resets the view.
	*/
	void FitToScreen();

	/** Fits the model to screen with a zoom factor.
	* The FitToScreen() function sometimes does not provide a satisfactory result. This function allows
	* to correct by zooming in or out with respect to the original result produced by FitToScreen().
	* @param zoom The zoom factor. 1.0 is the regular fit. Bigger number zooms out (good for smaller windows).
	*
	*/
	void FitToScreen(float zoom);

	/**
	* Closes down and resets renderer. 
	* Call this before Initing next model.
	*/
	void Close();

	/**
	* Frame rate. Calculated based on the last 50 rendered frames.
	*/
	float renderingFrameRate;


private:
	// Current translationvalues.
	GLfloat mXTranslate, mYTranslate, mZTranslate;

	// Original fittingvalues stored to use when user wants FitToScreen()
	GLfloat mXTranslateFit, mYTranslateFit, mZTranslateFit;

	// modelViewMatrix set by the user, if any
	GLfloat *modelViewMatrix;

	GLfloat mXAccel, mYAccel, mZAccel;

	GLfloat mLeftButtonX;
	GLfloat mLeftButtonY;
	GLfloat mRightButtonX;
	GLfloat mRightButtonY;

	GLfloat mRotateXAngleAdd;
	GLfloat mRotateYAngleAdd;

	GLfloat mLeftButtonControlZ;

	GLfloat mXRotAngle;
	GLfloat mYRotAngle;

	GLfloat mMaxRadius;

	double mVGLR_PERSPECT_FOVY;
	double mVGLR_PERSPECT_TAN_FOVY;
	double mVGLR_PERSPECT_NEAR_PLANE;

	GLfloat mFarPlane;

	GLsizei mWindowWidth, mWindowHeight;

	
	// Which rendering-mode are we in?
	VGLR_RENDERING_MODE mRenderingMode;

	// Backface culling.
	bool mBackfaceCull;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPENGLWND_H__83310F64_5D53_44AE_9D08_E29D6A961D6F__INCLUDED_)
