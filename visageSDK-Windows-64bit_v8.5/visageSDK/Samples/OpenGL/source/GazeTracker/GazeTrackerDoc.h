#include "visageVision.h"
#include "Observer.h"
#include "Repository.h"
#include "GazeRenderer.h"
#include "VisageGazeTracker.h"
#include "IMouseHandler.h"

using namespace VisageSDK;


/*! \mainpage  visage|SDK GazeTracker example project
*
* \if WIN64_DOXY
* \htmlonly
* <table border="0">
* <tr>
* <td width="32"><a href="../../../../../../bin64/DEMO_GazeTracker_x64.exe"><img src="../../../../../../doc/OpenGL/images/run_sample.png" border=0 title="Run Sample (this may not work in all browsers, in such case please run DEMO_GazeTracker_x64.exe from the visageSDK\bin64 folder.)"></a></td>
* <td width="32"><a href="../../../../../../bin64"><img src="../../../../../../doc/OpenGL/images/open_bin_folder.png" border=0 title="Open Binary Folder (visageSDK\bin64)"></a></td>
* <td width="32"><a href="../../../../data/GazeTracker"><img src="../../../../../../doc/OpenGL/images/open_data_folder.png" border=0 title="Open Data Folder"></a></td>
* <td width="32"><a href="../../../../build/msvc140/GazeTracker"><img src="../../../../../../doc/OpenGL/images/open_project_folder.png" border=0 title="Open Project Folder"></a></td>
* <td width="32"><a href="../../../../source/GazeTracker"><img src="../../../../../../doc/OpenGL/images/open_source_folder.png" border=0 title="Open Source Code Folder"></a></td>
* </tr>
* </table>
* \endhtmlonly
* \else
* \htmlonly
* <table border="0">
* <tr>
* <td width="32"><a href="../../../../../../bin/DEMO_GazeTracker.exe"><img src="../../../../../../doc/OpenGL/images/run_sample.png" border=0 title="Run Sample (this may not work in all browsers, in such case please run DEMO_GazeTracker.exe from the visageSDK\bin folder.)"></a></td>
* <td width="32"><a href="../../../../../../bin"><img src="../../../../../../doc/OpenGL/images/open_bin_folder.png" border=0 title="Open Binary Folder (visageSDK\bin)"></a></td>
* <td width="32"><a href="../../../../data/GazeTracker"><img src="../../../../../../doc/OpenGL/images/open_data_folder.png" border=0 title="Open Data Folder"></a></td>
* <td width="32"><a href="../../../../build/msvc140/GazeTracker"><img src="../../../../../../doc/OpenGL/images/open_project_folder.png" border=0 title="Open Project Folder"></a></td>
* <td width="32"><a href="../../../../source/GazeTracker"><img src="../../../../../../doc/OpenGL/images/open_source_folder.png" border=0 title="Open Source Code Folder"></a></td>
* </tr>
* </table>
* \endhtmlonly
* \endif
*
* The GazeTracker example project demonstrates the following capabilities of visage|SDK:
*
* - online mode of screen space gaze estimation (from the camera).
*
* <br/><br/>
* In order to try the sample application, you need to have a valid license key. More information on how to obtain license key and include it in the application can be found in
* <a href="../../../../../../doc/OpenGL/licensing.html"><em>Licensing</em></a> section of the documentation.
* <br/><br/>
* This sample application works in two phases, static calibration and interactive calibration/estimation. 
* 
* In the static calibration phase application displays a calibration point as a red dot on the screen. 
* After the user has clicked on the dot, the next calibration point in the pattern is displayed.
* 
* After all calibration points are clicked, gaze tracking system is calibrated and second phase starts. In the interactive calibration/estimation phase the application draws the estimated gaze location as a blue dot on the screen. Additionally, the application displays calibration points as red dots on random positions on the screen.
* When the user clicks on the calibration dot, the system is recalibrated using all calibration points from the static calibration phase and interactive calibration/estimation phase. 
*
* The application returns to static calibration phase by pressing SPACE button. All previously clicked calibration points are then discarded.
*
* The specific classes and methods that demonstrate visage|SDK are:
* - GazeTrackerDoc::StartGazeTracking(): Contains the main application loop. Initializes the screen space gaze estimation and calibrates screen space gaze estimator. 
* During the static calibration phase, the method reads calibration points from a Repository object and displays them on screen. <br> During the interactive calibration/estimation phase, the method reads the latest estimated screen space gaze position from Observer object and displays additional calibration points.
*
* Further classes and methods that are important for detailed understanding how this
* sample project is implemented and how it used visage|SDK are:
* - Observer::Notify(): Stores gaze estimation results and tracking data for further use.
* - GazeRenderer::Redraw(): Draws face tracking data as well as screen space gaze tracking calibration points and results using OpenGL.
* - Repository::ReadFromFile(): Reads screen space calibration data from the provided file containing the calibration points.
* - GazeTrackerDoc::OnLeftButtonDown(): Handles mouse input from user.
*
*@see Repository, GazeRenderer, Observer
*/

/**
* The document.
* Main application document.
*/
class GazeTrackerDoc : public IMouseHandler
{
public:
	GazeTrackerDoc(void);
	~GazeTrackerDoc(void);

	/** This method contains the main application loop of the GazeTracker sample application. 
	* This method handles initalization of VisageTracker object, initialization of screen space gaze tracking system, calibration of gaze tracking system and rendering of results. 
	* 
	* The method initalizes screen space gaze tracking system by calling VisageSDK::VisageTracker::InitOnlineGazeCalibration and reads calibration data by calling Repository::ReadFromFile().
	* 
	* In each iteration of the main application loop method GazeRenderer::Redraw() is called to display the calibration data or the estimated screen space gaze point. 
	* Additonally, the latest tracking data is aquired from Observer and displayed on the screen.
	**/
	void StartGazeTracking();
	

	/**
	* This method handles user mouse clicks.
	* 
	* The method check if the user has clicked on currently displayed calibration point and passes it to the tracker using VisageSDK::VisageTracker::AddGazeCalibrationPoint(). 
	*
	**/
	void OnLeftButtonDown(float x, float y);

	void ReadConfig();

private:
	VisageGazeTracker* tracker;
	Repository* calibrator;	
	Observer* observer;
	GazeRenderer* renderer;
	Window* resWindow;

	ScreenSpaceGazeData* calibration;
	bool isClicked;
	bool isCalibrated;
	int index;

	void WaitForClick();
	void PointOutOfScreenFix(float& x, float& y, int width, int height, int pointSize);

	HANDLE trackingThreadHandle;
	volatile bool trackerActive;
	static void trackFromCAM(void *thisdoc);
	void startTrackingFromCam();
};

