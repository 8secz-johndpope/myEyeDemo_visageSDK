///////////////////////////////////////////////////////////////////////////////
//
// (c) Visage Technologies AB 2002 - 2016  All rights reserved.
//
// This file is part of visage|SDK(tm).
// Unauthorized copying of this file, via any medium is strictly prohibited.
//
// No warranty, explicit or implicit, provided.
//
// This is proprietary software. No part of this software may be used or
// reproduced in any form or by any means otherwise than in accordance with
// any written license granted by Visage Technologies AB.
//
/////////////////////////////////////////////////////////////////////////////


#ifndef __VisageTracker_h__
#define __VisageTracker_h__

#ifdef VISAGE_STATIC
	#define VISAGE_DECLSPEC
#else

	#ifdef VISAGE_EXPORTS
		#define VISAGE_DECLSPEC __declspec(dllexport)
	#else
		#define VISAGE_DECLSPEC __declspec(dllimport)
	#endif

#endif

#include "FaceData.h"
#include "SmoothingFilter.h"

#ifdef WIN32
#include <windows.h>
#include <process.h>
#endif

#ifdef ANDROID
extern "C" {
	VISAGE_DECLSPEC void _loadVisageVision();
}
#endif

#ifndef WIN32
#include <pthread.h>
#include <sys/types.h>
#define HANDLE pthread_t*
#endif


namespace VisageSDK
{

#define TRACK_STAT_OFF 0
#define TRACK_STAT_OK 1
#define TRACK_STAT_RECOVERING 2
#define TRACK_STAT_INIT 3

#define VISAGE_CAMERA_UP 0
#define VISAGE_CAMERA_DOWN 1
#define VISAGE_CAMERA_LEFT 2
#define VISAGE_CAMERA_RIGHT 3

#define VISAGE_FRAMEGRABBER_FMT_RGB 0
#define VISAGE_FRAMEGRABBER_FMT_BGR 1
#define VISAGE_FRAMEGRABBER_FMT_LUMINANCE 2
#define VISAGE_FRAMEGRABBER_FMT_RGBA 3
#define VISAGE_FRAMEGRABBER_FMT_BGRA 4

#define VISAGE_FRAMEGRABBER_ORIGIN_TL 0
#define VISAGE_FRAMEGRABBER_ORIGIN_BL 1

class VisageTrackerInternal;
class VisageTrackerCore;
class Candide3Model;
class TrackerInternalInterface;
class TrackerGUIInterface;
class VisageDetector;
class ModelFitter;
class PoseEstimator;

/** VisageTracker is a face tracker capable of tracking the head pose, facial features and gaze for multiple faces in video coming from a
* video file, camera or other sources.
*
* The tracker is fully configurable through comprehensive tracker configuration files. visage|SDK contains optimal configurations
* for common uses such as head tracking and facial features tracking.
* Please refer to the <a href="../VisageTracker Configuration Manual.pdf">VisageTracker Configuration Manual</a> for the list of available configurations.
* \if IOS_DOXY
* Please read more details about configuration selection in the section <a href="../../doc/creatingxc.html#config_selection">Device-specific configuration selection</a>
* \endif
*
* The <a href="../VisageTracker Configuration Manual.pdf">VisageTracker Configuration Manual</a> provides full detail
* on all available configuration options, allowing to customize the tracker in terms of performance, quality and other options.
*
* \if ANDROID_DOXY
* Tracking from raw image input is enabled by track() and allows tracking
* from any source.
* \endif
*
* The tracker offers the following outputs as a part of FaceData object:
* - 3D head pose,
* - facial expression,
* - gaze information,
* - eye closure,
* - facial feature points,
* - full 3D face model, textured.
*
*
* The tracker can apply a smoothing filter to tracking results to reduce the inevitable tracking noise. Smoothing factors
* are adjusted separately for different parts of the face. The smoothing settings
* in the supplied tracker configurations are adjusted conservatively to avoid delay in tracking response, yet provide
* reasonable smoothing. For further details please see the smoothing_factors parameter array in the
* <a href="../VisageTracker Configuration Manual.pdf">VisageTracker Configuration Manual</a>.
*
* Frames (images) need to be passed sequentially to the VisageTracker::track() method, which immediately returns results for the given frame.
*
* The tracker requires a set of data and configuration files, available in Samples/data
*
* Please either copy the complete contents of this folder into your application's working folder, or consult <a href="../VisageTracker Configuration Manual.pdf">VisageTracker Configuration Manual</a> for detailed settings.
*
*/
class VISAGE_DECLSPEC VisageTracker
{
public:
	/** Constructor.
	*
	* @param trackerConfigFile the name of the tracker configuration file (.cfg), relative to the current working directory, e.g. "visageSDK\Samples\data\Facial Features Tracker - High.cfg", considering Visage Technologies as a working directory. Default configuration files are provided in Samples/data folder.
	* Other data files required by the tracker are located in the same folder as the configuration files, so their path is determined from the configuration file's path given here.
	* For further details see <a href="../VisageTracker Configuration Manual.pdf">VisageTracker Configuration Manual</a>\if IOS_DOXY and section on <a href="../../doc/creatingxc.html#config_selection">device-specific configuration selection</a>.\endif).
	*/
	VisageTracker(const char* trackerConfigFile);

	/** Destructor.
	*/
	virtual ~VisageTracker();

	/**
	* Performs face tracking (one or more faces) in the given image and returns tracking results and status.
	* This function should be called repeatedly on a series of images in order to perform continuous tracking.
	*
	* If the tracker needs to be initialized, this will be done automatically before tracking is performed on the given image.
	* Initialization means loading the tracker configuration file, required data files and allocating various data buffers to the given image size.
	* This operation may take several seconds.
	* This happens in the following cases:
	*   - In the first frame (first call to track() function).
	*   - When frameWidth or frameHeight are changed, i.e. when they are different from the ones used in the last call to track() function.
	*   - If setTrackerConfiguration() function was called after the last call to track() function.
	*   - When maxFaces is changed, i.e. when it its different from the one used in the last call to track() function.
	*
	* The tracker results are returned in an array of FaceData objects, one FaceData object for each tracked face.
	* Contents of each FaceData element depend on the corresponding tracker status (tracker statuses are returned as return value from the function).
	*
	* The tracking of multiple faces is performed in parallel ( using \if IOS_DOXY Dispatch Framework \else OpenMP \endif) and performance (speed) may vary depending on the number of CPU cores,
	* the number of faces in the current image and the value of maxFaces argument.
	*
	* @param frameWidth Width of the frame
	* @param frameHeight Height of the frame
	* @param p_imageData Pointer to image pixel data; size of the array must correspond to frameWidth and frameHeight
	* @param facedata Array of FaceData instances that will receive the tracking results. No tracking results will be returned if NULL pointer is passed.
	* On first call of this function, the memory for the required member variables of the passed array of FaceData objects will be allocated and initialized
	* automatically. The FacaData array must have the size equal to maxFaces argument.
	* @param format Format of input images passed in p_imageData. It can not change during tracking. Format can be one of the following:
	* - VISAGE_FRAMEGRABBER_FMT_RGB: each pixel of the image is represented by three bytes representing red, green and blue channels, respectively.
	* - VISAGE_FRAMEGRABBER_FMT_BGR: each pixel of the image is represented by three bytes representing blue, green and red channels, respectively.
	* - VISAGE_FRAMEGRABBER_FMT_RGBA: each pixel of the image is represented by four bytes representing red, green, blue and alpha (ignored) channels, respectively.
	* - VISAGE_FRAMEGRABBER_FMT_BGRA: each pixel of the image is represented by four bytes representing blue, green, red and alpha (ignored) channels, respectively.
	* - VISAGE_FRAMEGRABBER_FMT_LUMINANCE: each pixel of the image is represented by one byte representing the luminance (gray level) of the image.
	* @param origin No longer used, therefore, passed value will not have an effect on this function. However, the parameter is left to avoid API changes. 
	* @param widthStep Width of the image data buffer, in bytes.
	* @param timeStamp The timestamp of the the input frame in milliseconds. The passed value will be returned with the tracking data for that frame (FaceData::timeStamp). Alternatively, the value of -1 can be passed, in which case the tracker will return time, in milliseconds, measured from the moment when tracking started.
	* @param maxFaces The maximum number of faces that will be tracked (tracker currently supports up to 20 faces).
	* @returns array of tracking statuses for each of the tracked faces (TRACK_STAT_OFF, TRACK_STAT_OK, TRACK_STAT_RECOVERING and TRACK_STAT_INIT, see @ref FaceData for more details).
	*
	* @see FaceData
	*/
	virtual int* track(int frameWidth, int frameHeight, const char* p_imageData, FaceData* facedata, int format = VISAGE_FRAMEGRABBER_FMT_RGB, int origin = VISAGE_FRAMEGRABBER_ORIGIN_TL, int widthStep = 0, long timeStamp = -1, int maxFaces = 1);

	/** Set tracking configuration.
	*
	* The tracker configuration file name and other configuration parameters are set and will be used for the next tracking session (i.e. when track() is called).
	* Default configuration files (.cfg) are provided in Samples/data folder.
	* Please refer to the  <a href="../VisageTracker Configuration Manual.pdf">VisageTracker Configuration Manual</a> for further details
	* on using the configuration files and all configurable options. \if IOS_DOXY Also, please read the section on automatic device-specific configuration selection.\endif
	* @param trackerConfigFile the name of the tracker configuration file.
	* @param au_fitting_disabled disables the use of the 3D model used to estimate action units (au_fitting_model configuration parameter). If set to true, estimation of action units shall not be performed, and action units related data in the returned FaceData structure will not be available (FaceData::ActionUnits etc.). Disabling will result in a small performance gain.
	* @param mesh_fitting_disabled disables the use of the fine 3D mesh (mesh_fitting_model configuration parameter). If set to true, the 3D mesh shall not be fitted and the related information shall not be available in the returned FaceData structure (FaceData::FaceModelVertices etc.). Disabling will result in a small performance gain.
	*/
	void setTrackerConfiguration(const char* trackerConfigFile, bool au_fitting_disabled = false, bool mesh_fitting_disabled = false);

	/** Sets the inter pupillary distance
	*
	* Inter pupillary distance (IPD) is used by the tracker to estimate the distance of the face from the camera.
	* By default, IPD is set to 0.065 (65 millimeters) which is considered average. If the actual IPD of the tracked person is known,
	* this function can be used to set this IPD. As a result, the calculated distance from the camera will be accurate (as long as the 
	* camera focal lenght is also set correctly).
	* This is important for applications that require accurate distance. For example, in Augmented Reality applications objects such as 
	* virtual eyeglasses can be rendered at appropriate distance and will thus appear in the image with real-life scale.
	*
	* IMPORTANT NOTE: In case of multitracking, same IPD will be set for all tracked faces!
	*
	* @param value the inter pupillary distance (IPD) in meters.
	* @see getIPD()
	*/
	void setIPD(float value);

	/** Returns the current inter pupillary distance (IPD) setting.
	* IPD setting is used by the tracker to estimate the distance of the face from the camera. See setIPD() for further details.
	*
	* @return current setting of inter pupillary distance (IPD) in meters.
	* @see setIPD()
	*/
	float getIPD();

	/** Stop tracking.
	*
	* Stops the tracker.
	*
	*/
	void stop();

	/** Reset tracking
	*
	* Resets the tracker. Tracker will reinitialise.
	*
	*/
	void reset();

	/** Returns visageSDK version and revision number.
	*
	*/
	const char* getSDKVersion();

#ifdef IOS
	/** \if IOS_DOXY\ Set data bundle
	 *
	 * Used to set bundle from which data files will be read. Default is main bundle.
	 *
	 * \endif
	 */
	void setDataBundle(NSBundle *bundle);
#endif


private:

	int Init(void);
	void Finish(void);

	static void processEyesClosure(VsImage* frame, FDP* fdp, float* t, float* r, float* out);
	static void GetPupilCoords(FDP* points, VsMat* eyes_coords, int w, int h);

	TrackerGUIInterface *guiIface;
	TrackerInternalInterface *internalIface;

	float recovery_timeout; /* This value is used only in automatic initialisation mode. It is used when the tracker looses the face and can not detect any face in the frame. This value tells the tracker how long it should wait before considering that the current user is gone and initialising the full re-initialisation procedure.  If the face is detected before this time elapses, the tracker considers that it is the same person and it recovers, i.e. continues tracking it using the previous settings. The time is expressed in milliseconds. */

	friend class VisageFeaturesDetector;
	friend class VisageTrackerInternal;

	VsImage* frame_gray; // Current video frame converted to grayscale; all processing is done on grayscale images
	VsImage* frame_gray_prev; // Previous video frame converted to grayscale; used for optical flow tracking

protected:
	SmoothingFilter sf;
	VsMat *smoothing_factors;

	VsImage* frame_input; // Current video frame, input; may be color or grayscale

	void prepareFramesForMT();
	int calculateOccludeForMT();
	void terminateMT();

	int frameCount; // frame count from beginning of tracking
	double frameTime; // duration of one frame in milliseconds
	long pts; // presentation time stamp
	int pts_frame;

	unsigned long initialTime;
	unsigned long last_times[10];
	int cnt;

	volatile bool active;

private:
	long getCurrentTimeMs(bool init);
	bool grabFrame(bool init);

	static void AugmentFDP(FDP* src, FDP* dst);
	static void unProjectStuff(FDP* fp2D, FDP* fp3D, int w, int h, float f);
	static void removeTransform(FDP* fp3D, FDP* fp3DR, const float* r, const float* t);
	static void setPose(float pose[6], float t[3], float r[3]);
	static float getAvgQuality(const FDP &fdp);

	bool init_successful;

	std::string configuration_filename; //tracker configuration file

#ifdef IOS
	NSBundle *dataBundle;
#endif

	volatile bool tracking_ok; //tracking status OK
	float trackingFrameRate; // measured tracking frame rate

	int m_width; //frame width
	int m_height; //frame height
	int m_format; //frame format for the frame image (RGB, BGR or LUMINANCE)

	void loadConfig();  // read application settings from configuration file

	bool loadModel(const string &settings, const char* cfgDirPat, Candide3Model **model, int modelType);

	float m_IPD;

#if defined(IOS) || defined(ANDROID) || defined(MAC_OS_X) || defined(LINUX)
	pthread_mutex_t track_mutex;
#endif

#ifdef WIN32
	CRITICAL_SECTION track_mutex;
#endif

	std::string bdts_data_path; // path to the folder containing boosted decision trees
#ifdef EMSCRIPTEN
	public: VisageDetector* m_DetectorBDFS;
	private:
#else
	VisageDetector* m_DetectorBDFS;
#endif

	// pose estimator model
	Candide3Model* model;
	// au fitting model
	Candide3Model* au_model;
	// mesh fitting model
	Candide3Model* mesh_model;

	bool use_single_model;
	bool use_pose_model;
	bool use_au_model;
	bool use_mesh_model;
	bool enable_fitting;

	bool au_fitting_disable;
	bool mesh_fitting_disable;

	bool inited;

	long fg_pts;

	bool configChanged;

	char m_trackCfgDataPath[1024];

	FILE *log_file; //log file - used for debugging, set via log_filename configuration parameter

	int m_maxFaces;

	VisageTrackerInternal **tracker;
	int* trackStatus;
	VsImage **frames;
	VsImage **frames_prev;
	VsRect* face;
};

}

#endif // __VisageTracker_h__

