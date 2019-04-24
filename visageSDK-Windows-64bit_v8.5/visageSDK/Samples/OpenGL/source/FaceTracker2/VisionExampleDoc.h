// Example7Doc.h : interface of the CVisionExampleDoc class
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#if !defined(AFX_EXAMPLE7DOC_H__53884DF7_67A9_4F97_9BB2_031E50E618B5__INCLUDED_)
#define AFX_EXAMPLE7DOC_H__53884DF7_67A9_4F97_9BB2_031E50E618B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "visageVision.h"
#include "VisageLivenessAction.h"
#include "VisageLivenessBrowRaise.h"
#include "VisageLivenessSmile.h"
#include "VisageLivenessBlink.h"
#include "OpenGLWndFaceTracker.h"
#include "FolderManager.h"
#include <string>
#include <videoInput.h>

using namespace VisageSDK;

#define WM_MYMESSAGE (WM_USER + 1)


class CMainFrame;

/*! \mainpage  visage|SDK FaceTracker2 example project
*
* \if WIN64_DOXY
* \htmlonly
* <table border="0">
* <tr>
* <td width="32"><a href="../../../../../../bin64/DEMO_FaceTracker2_x64.exe"><img src="../../../../../../doc/OpenGL/images/run_sample.png" border=0 title="Run Sample (this may not work in all browsers, in such case please run DEMO_FaceTracker2_x64.exe from the visageSDK\bin64 folder.)"></a></td>
* <td width="32"><a href="../../../../../../bin64"><img src="../../../../../../doc/OpenGL/images/open_bin_folder.png" border=0 title="Open Binary Folder (visageSDK\bin64)"></a></td>
* <td width="32"><a href="../../../../data/FaceTracker2"><img src="../../../../../../doc/OpenGL/images/open_data_folder.png" border=0 title="Open Data Folder"></a></td>
* <td width="32"><a href="../../../../data/Videos"><img src="../../../../../../doc/OpenGL/images/open_folder.png" border=0 title="Open Video Folder"></a></td>
* <td width="32"><a href="../../../../build/msvc140/FaceTracker2"><img src="../../../../../../doc/OpenGL/images/open_project_folder.png" border=0 title="Open Project Folder"></a></td>
* <td width="32"><a href="../../../../source/FaceTracker2"><img src="../../../../../../doc/OpenGL/images/open_source_folder.png" border=0 title="Open Source Code Folder"></a></td>
* </tr>
* </table>
* \endhtmlonly
* \else
* \htmlonly
* <table border="0">
* <tr>
* <td width="32"><a href="../../../../../../bin/DEMO_FaceTracker2.exe"><img src="../../../../../../doc/OpenGL/images/run_sample.png" border=0 title="Run Sample (this may not work in all browsers, in such case please run DEMO_FaceTracker2.exe from the visageSDK\bin folder.)"></a></td>
* <td width="32"><a href="../../../../../../bin"><img src="../../../../../../doc/OpenGL/images/open_bin_folder.png" border=0 title="Open Binary Folder (visageSDK\bin)"></a></td>
* <td width="32"><a href="../../../../data/FaceTracker2"><img src="../../../../../../doc/OpenGL/images/open_data_folder.png" border=0 title="Open Data Folder"></a></td>
* <td width="32"><a href="../../../../data/Videos"><img src="../../../../../../doc/OpenGL/images/open_folder.png" border=0 title="Open Video Folder"></a></td>
* <td width="32"><a href="../../../../build/msvc140/FaceTracker2"><img src="../../../../../../doc/OpenGL/images/open_project_folder.png" border=0 title="Open Project Folder"></a></td>
* <td width="32"><a href="../../../../source/FaceTracker2"><img src="../../../../../../doc/OpenGL/images/open_source_folder.png" border=0 title="Open Source Code Folder"></a></td>
* </tr>
* </table>
* \endhtmlonly
* \endif
*
* The FaceTracker2 example project demonstrates the following capabilities of visage|SDK:
*
* - real-time head/facial features tracking on multiple faces from video file or camera, based on the VisageSDK::VisageTracker tracker.
* - liveness verification from a camera
* 
* <br/><br/>
* In order to try the sample application, you need to have a valid license key. More information on how to obtain license key and include it in the application can be found in 
* <a href="../../../../../../doc/OpenGL/licensing.html"><em>Licensing</em></a> section of the documentation.
* <br/><br/>
*
* For a quick start just run the demo, and select "Track From Camera"; or select "Track From Avi video" from the "Tracking" menu and select one of the video files available in the <a href="../../../../data/Videos">Samples/OpenGL/data/Videos folder</a>
* which may require, depending on the system, installation of <a href="http://www.xvidmovies.com/codec/" target="_blank">Xvid video codec</a>.
* \n\n For liveness verification demonstration, please select Liveness -> Perform liveness.
* 
* The specific classes and methods that demonstrate visage|SDK are:
* - CVisionExampleDoc::OnTrackingTrackfromavivideo(): Performs real time tracking from a video file.
* - CVisionExampleDoc::OnTrackingTrackfromcam(): Performs real time tracking from a camera.
* - CVisionExampleDoc::OnTrackingTrackinimage(): Performs real time tracking from an image.
* - CVisionExampleDoc::OnTrackingStop(): Stops the real time face tracking.
* - CVisionExampleDoc::OnLivenessStart() Performs liveness verification from a camera.
*
* Further classes and methods that are important for detailed understanding how this
* sample project is implemented and how it used visage|SDK are:
* - VisageSDK::FaceData class that is used for storing all the tracking data 
*
* \section S1_export_to_file Tracking data into a file
*
* This example project includes code that implements writing of tracking data into a text file. The functionality is disabled by default and can be enabled by choosing Tracking from menu and selecting Write tracking data.
* In VisionExampleDoc.cpp close to the top are the lines defining VS_WRITE_AU and VS_WRITE_HEADPOSE. Each of these constants controls the writing of one type of data, 
* specifically Action Units and head pose. For example, to disable writing of head pose and leave only Action Units, simply comment the line defining VS_WRITE_HEADPOSE.
* 
* When activated, the data is written into a text file with values delimited by colon (;). The first line is the header.
* 
* When tracking from an image, the output data file is created in the same folder with the image and has the same name as the image with extension .txt.
*
* When tracking from a video file, the output data file is created in the same folder with the video file and has the same name as the video file with extension .txt.
* 
* When tracking from camera, the output file is created in the tracker working folder (by default this is "Visage Technologies\visageSDK\Samples\data").
* The name of the file is cam-data-\<time\>.txt, where "time" is the date and time when data was recorded.
*/

/**
* The document.
*/
class CVisionExampleDoc : public CDocument
{
protected: // create from serialization only
	CVisionExampleDoc();
	DECLARE_DYNCREATE(CVisionExampleDoc)

// Attributes
public:
	
	/**
	* Initialisation.
	* 
	* Initializes the tracker. Called from CVisionExampleApp::InitInstance().
	*/
	void init();

	/**
	* Initializes liveness detection.
	* Sets array of actions that will need to be verified with accompanying messages and images that will be displayed to user during liveness verification.
	*/
	void SetLivenessActions();

	/**
	* Shuffles array of actions.
	*/
	void RandomizeActions();

	/**
	* Releases objects alocated for liveness detection.
	*/
	void ReleaseActions();

	/**
	* The Face Tracker object (VisageTracker).
	* It is used to track the user's face from video and encode the tracked movements into MPEG-4 FAPs.
	*/
	VisageTracker* m_Tracker;

	/**
	* Maximal number of states that the actions will have.
	* It should be set to the number of states that the action with maximal number of states has.
	*/
	static const int MAX_ACTION_STATES = 5;
	/**
	* Number of actions that will need to be verified in order to confirm liveness.
	*/
	static const int NUM_ACTIONS = 3;

	/**
	* Structure that holds the action and its accompanying messages and images that will be displayed to user during action verification
	*/
	struct Action
	{
		VisageLivenessAction *action;
		string images[MAX_ACTION_STATES];
		string messages[MAX_ACTION_STATES];
	};

	/**
	* Array of actions that will need to be verified.
	*/
	Action actions[NUM_ACTIONS];
	
	/**
	* VisageLivenessBrowRaise object.
	* Used to verify that the user has raised its eyebrows.
	*/
	VisageLivenessAction *BrowRaiseAction;

	/**
	* VisageLivenessSmile object.
	* Used to verify that the user has smiled.
	*/
	VisageLivenessAction *SmileAction;

	/**
	* VisageLivenessBlink object.
	* Used to verify that the user has blinked.
	*/
	VisageLivenessAction *BlinkAction;

	/**
	* Test image used for demonstrating raw image tracking interface.
	*/
	IplImage *testImage;

	/**
	* The OpenGL window.
	* An OpenGL window with camera manipulation facilities. It is used by VisageTracker as an TrackerOpenGLInterface and TrackerGUIInterface, and also to render the animated 3D face model.
	*/
	COpenGLWnd *m_OpenGLWnd;

	CMainFrame* m_MainWnd;

	/**
	* fbaFileName.
	* This string contains the name of the .fba file where the facial movements will be encoded.
	*/
	std::string fbaFileName;

	std::string videoFileName;

	/**
	* Data file.
	* This file will contain written data for each frame of the video.
	* Data is written as CSV file where each line contains data for one frame separated with ';'.
	* The first line contains data header describing meaning of that column.
	*
	*/
	FILE *datafile;
	bool headerSet;
	int frameNum;
	bool writeDataEnabled;

	void writeResults(int track_stat, FaceData &trackingData);
	void finishWriteResults();

// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVisionExampleDoc)
	public:
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:

	virtual ~CVisionExampleDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
	
public:

	void OnCloseDocument();

	afx_msg void OnTrackingStop();

private:
	/**
	* A flag that is false initially, and set to true when the tracker is used the first time.
	* Used to avoid loading a 3D model after tracking has been used, because this causes problems.
	*/
	bool trackerUsed;
	/**
	* A flag used to indicate that actions have been set (setLivenessActions())
	*/
	bool livenessInited = false;
	volatile bool threadActive;
	HANDLE trackingThreadHandle; // Handle to the thread used for tracking
	int cam_device;
	videoInput *VI;

public:
	afx_msg void OnTrackingTrackfromavivideo();
	afx_msg void OnTrackingTrackfromcam();
	afx_msg void OnTrackingCamerasettings();
	static void testingThread(void *thisdoc);
	static void trackFromImage(void *thisdoc);
	static void trackFromCAM(void *thisdoc);
	static void trackFromVideo(void *thisdoc);
	static void detectLivenessOnCamera(void *thisdoc);
	afx_msg void OnTrackingBatchprocessing();
	afx_msg void OnTrackingTrackinimage();
	afx_msg void OnTrackingWritedata();
	afx_msg void OnLivenessStart();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXAMPLE7DOC_H__53884DF7_67A9_4F97_9BB2_031E50E618B5__INCLUDED_)
