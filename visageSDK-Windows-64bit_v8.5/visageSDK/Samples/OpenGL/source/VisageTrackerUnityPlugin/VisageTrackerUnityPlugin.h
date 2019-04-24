//
//  VisageTrackerUnityPlugin.h
//  VisageTrackerUnityPlugin
//
//
#ifndef __VisageTrackerUnityPlugin_h__
#define __VisageTrackerUnityPlugin_h__

#include "VisageTrackerUnityShared.h"


#ifdef WIN32
#ifdef _MSC_VER
// ensures valid dll function export
#define EXPORT_API __declspec(dllexport)
#endif
#endif


extern "C" {

	EXPORT_API void SetDebugFunction(FuncPtr fp) { Debug = fp; };

	/** Initializes the license.
	* 
	*/
	EXPORT_API void _initializeLicense(char* license);

	/** Updates the pixel data in Unity with data from tracker.
	*
	* Before each call of the _setFrameData(), _grabFrame() function needs to be called to obtain a new frame.
	*
	 */
	EXPORT_API void _setFrameData(char* texIDPointer);
	
	/** Grabs current frame from the camera.
	* 
	* Frame from the camera is obtained using VideoInput library.
	*/
	EXPORT_API void _grabFrame();

	/** Initializes camera using native VideoInput library with given orientation, camera width and height, device ID and isMirrored parameter for possible frame mirroring.
	*
	*/
	EXPORT_API void _openCamera(int orientation, int deviceID, int width, int height, int isMirrored);

	/** Closes camera and clears memory allocated by the camera API.
	*/
	EXPORT_API void _closeCamera();
	
	/** Initialises the tracker with configuration file and a desired number of tracked faces. Number of tracked faces is limited with MAX_FACES parameter to 20.
	* 
	*/
	EXPORT_API void _initTracker(char* config, int numFaces);

	/** Releases memory allocated by the tracker in the _initTracker() function.
	*
	*/
	EXPORT_API void _releaseTracker();

	/** Performs face tracking on current frame.
	*
	* Before each call of the _track() function, _grabFrame() function needs to be called to obtain the new frame.
	*/
	EXPORT_API void _track();

	/** Returns tracker status.
	*
	*/
	EXPORT_API void _getTrackerStatus(int* tStatus);

	/** Estimated tracking quality for the current frame for the given faceIndex.
	*
	*/
	EXPORT_API float _getTrackingQuality(int faceIndex);

	/** Returns the frame rate of the tracker, in frames per second, measured over last 10 frames.
	*
	*/
	EXPORT_API float _getFrameRate();

	/** Returns timestamp of the current video frame, in milliseconds, measured from the moment when tracking started.
	*
	*/
	EXPORT_API long _getTimeStamp();

	/** Returns index of the first FP group.
	*
	*/
	EXPORT_API int _getFP_START_GROUP_INDEX();

	/** Returns index of the last FP group.
	*
	*/
	EXPORT_API int _getFP_END_GROUP_INDEX();

	/** Returns size of each FP group.
	*
	*/
	EXPORT_API void _getGroupSizes(int* groupSize, int length);

	/** Returns scale in pixels of facial bounding box for the given faceIndex. 
	*
	*/
	EXPORT_API int _getFaceScale(int faceIndex);

	/** Returns the current head translation in meters for the given faceIndex.
	*
	*/
	EXPORT_API void _getHeadTranslation(float* translation, int faceIndex);

	/** Returns the current head translation in meters for the given faceIndex.
	*
	*/
	EXPORT_API  void _getHeadRotation(float* rotation, int faceIndex);

	/** Returns number of vertices in the 3D face model.
	*
	*/
	EXPORT_API  int _getFaceModelVertexCount();

	/** Returns number of triangles in the 3D face model. 
	*
	*/
	EXPORT_API  int _getFaceModelTriangleCount();

	/** Returns triangles coordinates.
	*
	*/
	EXPORT_API  void _getFaceModelTriangles(int* triangles, int faceIndex);

	/** Returns vertex coordinates.
	*
	*/
	EXPORT_API  void _getFaceModelVertices(float* vertices, int faceIndex);

	/** Returns projected (image space) vertex coordinates.
	*
	*/
	EXPORT_API  void _getFaceModelVerticesProjected(float* verticesProjected, int faceIndex);

	/** Returns texture coordinates.
	*
	*/
	EXPORT_API void _getFaceModelTextureCoords(float* texCoord, int faceIndex);

	/** Returns camera info.
	*
	*/
	EXPORT_API void _getCameraInfo(float *focus, int *width, int *height);
	
	/** Returns static texture coordinates of the mesh.
	*
	*/
	EXPORT_API void _getTexCoordsStatic(float* buffer, int* texCoordNumber);

	/** Returns the action unit count.
	*
	*/
	EXPORT_API int _getActionUnitCount();

	/** Returns the name of the action unit with the specified index.
	* 
	*/
	EXPORT_API const char* _getActionUnitName(int au_index);

	/** Returns true if the action unit is used.
	*
	*/
	EXPORT_API bool _getActionUnitUsed(int au_index);

    /** Returns all action unit values.
    *
    */
	EXPORT_API void _getActionUnitValues(float* values, int faceIndex);

	/** Returns the gaze direction for the given faceIndex.
	* 
    */
	EXPORT_API bool _getGazeDirection(float* direction, int faceIndex);

    /** Returns the global gaze direction for the given faceIndex.
    *
    */
	EXPORT_API bool _getGazeDirectionGlobal(float* direction, int faceIndex);

	/** Returns eye closure value for the given faceindex.
	*
	*/
	EXPORT_API bool _getEyeClosure(float* closure, int faceIndex);

    /** Returns the feature point positions in normalized 2D screen coordinates, indication of whether the point is defined and detected and quality for specified feature points
    *
    */
	EXPORT_API void _getFeaturePoints2D(int N, int* groups, int* indices, float* positions, int* defined, int* detected, float* quality, int faceIndex);

	/** Returns the global 3D feature point positions, indication of whether the point is defined and detected and quality for specified feature points
	*
	*/
	EXPORT_API void _getFeaturePoints3D(int N, int* groups, int* indices, float* positions, int* defined, int* detected, float* quality, int faceIndex);

    /** Returns the relative 3D feature point positions, indication of whether the point is defined and detected and quality for specified feature points
    *
    */
	EXPORT_API void _getFeaturePoints3DRelative(int N, int* groups, int* indices, float* positions, int* defined, int* detected, float* quality, int faceIndex);

	/** Returns feature point position in normalized 2D screen coordinates, indication of whether the point is defined and detected and quality for all feature points
	*
	*/
	EXPORT_API void _getAllFeaturePoints2D(float* positions, int len, int faceIndex);

	/** Returns the global feature point position, indication of whether the point is defined and detected and quality for all feature points
	*
	*/
	EXPORT_API void _getAllFeaturePoints3D(float* positions, int len, int faceIndex);

	/** Returns the relative 3D feature point position, indication of whether the point is defined and detected and quality for all feature points
	*
	*/
	EXPORT_API void _getAllFeaturePoints3DRelative(float* positions, int len, int faceIndex);

	/** Sets tracking configuration. 
	*
	*/
	EXPORT_API void _setTrackerConfiguration(const char *trackerConfigFile, bool au_fitting_disabled = false, bool mesh_fitting_disabled = false);

	/** Sets the inter pupillary distance. 
	*
	*/
	EXPORT_API void _setIPD(float value);

	/**Returns the current inter pupillary distance (IPD) setting.
	*
	*/
	EXPORT_API float _getIPD();
}
#endif //__VisageTrackerUnityPlugin_h__