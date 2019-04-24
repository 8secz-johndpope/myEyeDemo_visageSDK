#include "visageVision.h"


using namespace VisageSDK;


/**
* This implementation is used to demonstrate the use of screen space gaze data returned by the tracker.
* It is used to store tracking data passed as a parameter after ViasgeSDK::VisageTracker::track() is called. 
* The stored tracking data is used in the main application loop and passed to the GazeRenderer object to be displayed on the screen.
**/
class Observer
{
public:
	Observer(void);
	~Observer(void);

	/**
	* This function is called once for every video frame. The method recieves the latest VisageSDK::FaceData object as a parameter and stores it in order to be available to main application loop. 
	*/
	void Notify(FaceData& trackingData, int trackerStatus, VsImage* frame);
	int track_stat;

	float scaleX;
	float scaleY;
	float camOffsetX;
	float camOffsetY;
	int resolutionX;
	int resolutionY;
	float scalingFactor;
	
	float gazeScaleX;
	float gazeScaleY;

	long timestamp;
	long recalibTimestamp;

	FaceData trackingData;
	VsImage* image;
	ScreenSpaceGazeData gazeData;
	int notifyCount;

private:

};