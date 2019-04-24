using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;


/// <summary>
/// Provides high-level functions for using visage|SDK face tracking capabilities in Unity game engine.
///Additionally, provides high-level functions for camera management such as _openCamera(), _closeCamera() and _getCameraInfo().
///
///It implements high-level wrapper functions around key functionalities from visage|SDK that enable usage of visage|SDK face tracking capabilities in Unity C# scripts.
///
///VisageTracker is a face tracker capable of tracking the head pose and facial features for multiple faces in video coming from a video file, camera or other sources. 
///Maximum number of faces that can be tracked is limited to 20 within native plugin library with MAX_FACES parameter.
///Parametar faceIndex with value between 0 and MAX_FACES is passed to the functions within native plugin library to obtain the data for a particular tracked face.
///All functions that return the data obtained from the tracker, internally check the tracker status as well as the value of the parameter faceIndex.
///
///The tracker is fully configurable through comprehensive tracker configuration files.visage|SDK contains optimal configurations for common uses such as head tracking and facial features tracking.Please refer to the VisageTracker Configuration Manual for the list of available configurations.
///The VisageTracker Configuration Manual provides full detail on all available configuration options, allowing to customize the tracker in terms of performance, quality, and other options.
///
///High-level functions offer the following outputs:
/// - 3D head pose
/// - facial expression
/// - gaze direction information
/// - eye closure
/// - facial feature points
/// - full 3D face model, textured
///
///The tracker can apply a smoothing filter to tracking results to reduce the inevitable tracking noise. Smoothing factors are adjusted separately for different parts of the face.The smoothing settings in the supplied tracker configurations are adjusted conservatively to avoid delay in tracking response, yet provide reasonable smoothing. For further details please see the smoothing_factors parameter array in the VisageTracker Configuration Manual.  
///
///Frames are fetched natively using VideoInput library by calling _grabFrame() function and are applied to the Unity-created texture using OpenGL in _setFrameData() function.
///Function _grabFrame() needs to be called before each sequential call to the _track() function to obtain a new frame.
///
///Afterwords, high-level function such as _getFeaturePoints2D() are called to obtain tracker results for the given frame.
///
///The tracker requires a set of data and configuration files, available in Samples/data folder.
///For most Unity applications usage of Application.streamingAssetsPath folder is encouraged.
///
/// </summary>
/// 

/**
 * \defgroup VisageTrackerUnityPlugin VisageTrackerUnityPlugin
 * @{
 */

/** Initializes the license.
* 
* It is recommended to initialize the license in the Awake () function within the unity script.
* 
* @param license Relative path from the working directory to the directory that contains license file(s) (e.g. Application.streamingAssetsPath + "/" + licenseFileFolder).
*/
void _initializeLicense(string license);

/** Updates the pixel data in Unity with data from tracker.
*
* Before each call of the _setFrameData(), _grabFrame() function needs to be called to obtain a new frame.

* Note that the VisageTrackerUnityPlugin requires the RBGA texture format and assumes that the width and heigh of the texture will be set to the nearest heigher power of 2 from the width and height of the image.  
*
* \code
* if (texture == null)
* { 
*     // create texture with dimensions that are dependent on image dimensions as the nearest heigher power of number 2
*     texture = new Texture2D(TexWidth, TexHeight, TextureFormat.RGBA32, false);
*     
*     // "pin" the pixel array in memory, so we can pass direct pointer to it's data to the plugin,
*     // without costly marshaling of array of structures.
*     texturePixels = ((Texture2D)texture).GetPixels32(0);
*     texturePixelsHandle = GCHandle.Alloc(texturePixels, GCHandleType.Pinned);
* }
* 
* // on every frame
* // send memory address of textures' pixel data to VisageTrackerUnityPlugin
* VisageTrackerNative._setFrameData(texturePixelsHandle.AddrOfPinnedObject());
* \endcode
* 
* @param texIDPointer Pointer to the pixel data obtained from tracker to be used in Unity.
 */
void _setFrameData(IntPtr texIDPointer);

/** Grabs current frame from the camera.
* 
* Frame from the camera is obtained using VideoInput library.
* This function needs to be called before each call of the _track() function.
*/
void _grabFrame();

/** Initializes camera using native VideoInput library with given orientation, camera width and height, deviceID and isMirrored parameter for possible frame mirroring.
 * Device orientation parameter can have the following 4 values:
 * - 0: Portrait
 * - 1: LandscapeRight
 * - 2: PortraitUpsideDown
 * - 3: LandscapeLeft
*
* @param orientation Physical orientation of device.
* @param deviceID ID of the camera device from Windows system. For the first camera device value is 0, for the second 1 an so on.
* @param width Width of the frame.
* @param height Height of the frame.
* @param isMirrored 1 or 0, if set to 1 frames will be mirrored.
*/
void _openCamera(int orientation, int deviceID, int width, int height, int isMirrored);

/** Closes camera and clears memory allocated by the camera API.
*/
void _closeCamera();

/** Initialises the tracker with configuration file and a desired number of tracked faces. Number of tracked faces is limited within native plugin library with MAX_FACES parameter to 20.
* 
* Note that in order for license to be registered, _initializeLicense() method has to be called before.
* 
* @param config Relative path from the working directory to the directory that contains tracker configuration file, including configuration file name (e.g. Application.streamingAssetsPath + "/" + ConfigFileEditor).
* @param numFaces The maximum number of faces that will be tracked (<= 20).
*/
void _initTracker(string config, int numFaces);

/** Releases memory allocated by the tracker in the _initTracker() function.
*
* This function is called after the tracker ceases to be used or before the reinitialization of the tracker.
*/
void _releaseTracker();

/** Performs face tracking on current frame.
*
* Before each call of the _track() function, _grabFrame() function needs to be called to obtain the new frame.
*/
void _track();

/** Returns tracker status.
*
*
* An example of use:
* \code
*
* // initialization of TrackerStatus array
* public int[] TrackerStatus = new int[MAX_FACES];
*
* // call of the _getTrackerStatus() function
* VisageTrackerNative._getTrackerStatus(TrackerStatus);
*
* // an example of applying the obtained values
* if (TrackerStatus[0] != (int)TrackStatus.Off)
* {
*     ....
* }
* \endcode
*
*
* @param tStatus Array that will be filled with tracking statuses for each of the tracked faces.
*/
void _getTrackerStatus(int[] tStatus);

/** Estimated tracking quality for the current frame for the given faceIndex.
*
* @param faceIndex Value between 0 and MAX_FACES-1, used to access the data of a particular tracked face.
* @return Estimated tracking quality level for the current frame. The value is between 0 and 1. 
*/
float _getTrackingQuality(int faceIndex);

/** Returns the frame rate of the tracker, in frames per second, measured over last 10 frames.
*
*/
float _getFrameRate();

/** Returns timestamp of the current video frame, in milliseconds, measured from the moment when tracking started.
*
*/
int _getTimeStamp();

/** Returns size of facial bounding box in pixels for the given faceIndex. 
*
* @param faceIndex Value between 0 and MAX_FACES-1, used to access the data of a particular tracked face.
*/
int _getFaceScale(int faceIndex);

/** 
* Returns the current head translation in meters for the given faceIndex.
*
* Parameter <i>translation</i> is 3-dimensional array which will be filled x, y and z head translation coordinates 
* for the particular face given with <i>faceIndex</i> parameter.
* The coordinate system is such that when looking towards the camera, the direction of x is to the left, y iz up, and z points towards the viewer. 
* The global origin (0,0,0) is placed at the camera. The reference point on the head is in the center between the eyes.
* This variable is set only while tracker is tracking (TRACK_STAT_OK), otherwise it will fill translation array with -10000 values.
* @param translation 3D array in which the head translation values will be stored.
* @param faceIndex Value between 0 and MAX_FACES-1, used to access the data of a particular tracked face.
*/
void _getHeadTranslation(float* translation, int faceIndex);

/** 
* Returns the current head translation in meters for the given faceIndex.
* 
* Parameter <i>rotation</i> is 3-dimensional array which will be filled x, y and z head rotation coordinates 
* for the particular face given with <i>faceIndex</i> parameter.
*
* This is the estimated rotation of the head, in radians. Rotation is expressed with three values determining the rotations around the three axes x, y and z, in radians. 
* This means that the values represent the pitch, yaw and roll of the head, respectively. The zero rotation (values 0, 0, 0) corresponds to the face looking straight ahead along the camera axis. 
* Positive values for pitch correspond to head turning down. Positive values for yaw correspond to head turning right in the input image. 
* Positive values for roll correspond to head rolling to the left in the input image. The values are in radians.
* 
* This variable is set only while tracker is tracking (TRACK_STAT_OK), otherwise it will fill rotation array with 0 values.
* @param rotation 3D array in which the head rotation values will be stored.
* @param faceIndex Value between 0 and MAX_FACES-1, used to access the data of a particular tracked face.
*/
void _getHeadRotation(float* rotation, int faceIndex);

/** Returns number of vertices in the 3D face model.
*
* This variable is set only while tracker is tracking (TRACK_STAT_OK), otherwise it will be set to 0.
* @return Number of vertices. 
*/
int _getFaceModelVertexCount();

/** 
* Returns number of triangles in the 3D face model. 
* This variable is set only while tracker is tracking (TRACK_STAT_OK), otherwise it will be set to 0.
* @return Number of triangles. 
*/
int _getFaceModelTriangleCount();

/** 
* Returns triangles coordinates of the 3D face model.
* 
* Each triangle is described by three indices into the list of vertices.
* 
* Function returns triangles coordinates in the triangleNumber*3-dimensional <i>triangles</i> array, where triangleNumber is value 
* obtained from _getFaceModelTriangleCount() function, for the particular faceIndex.
* @param triangles Array in which values of triangle coordinates will be stored.
* @param faceIndex Value between 0 and MAX_FACES-1, used to access the data of a particular tracked face.
* <br/>
*/
void _getFaceModelTriangles(int* triangles, int faceIndex);

/** 
* Returns vertex coordinates of the 3D face model.
* 
* The coordinates are in the local coordinate system of the face, with the origin (0,0,0) placed at the center between the eyes. 
* The x-axis points laterally towards the side of the face, y-axis points up and z-axis points into the face.
* 
* Function returns vertices position in the vertexNumber*3-dimensional <i>vertices</i> array, where vertexNumber is value 
* obtained from _getFaceModelVertexCount() function, for the particular faceIndex.
*
* An example of use: 
* \code
* 
* //define MaxVertices value that will be larger than the real number of vertices
* public const int MaxVertices = 1024;
* private float[] vertices = new float[MaxVertices * 3];
*	
* //call of _getFaceModelVertices() function
* VisageTrackerNative._getFaceModelVertices(vertices, faceIndex);
*	
* \endcode
* @param vertices Array in which the values of vertex coordinates will be stored.
* @param faceIndex Value between 0 and MAX_FACES-1, used to access the data of a particular tracked face.
*/
void _getFaceModelVertices(float* vertices, int faceIndex);

/** 
* Returns projected (image space) vertex coordinates.
* 
* The 2D coordinates are normalised to image size so that the lower left corner of the image has coordinates 0,0 and upper right corner 1,1.
* 
* Function returns vertices position in the vertexNumber*2-dimensional <i>verticesProjected</i> array, where vertexNumber is value 
* obtained from _getFaceModelVertexCount() function, for the particular faceIndex.
* 
* An example of use: 
* \code
*	
* //define MaxVertices value that will be larger than the real number of vertices
* public const int MaxVertices = 1024;
* private float[] verticesProjected = new float[MaxVertices * 2];
*	
* //call of _getFaceModelVerticesProjected() function
* VisageTrackerNative._getFaceModelVerticesProjected(verticesProjected, faceIndex);
*
* \endcode
*
* @param verticesProjected Array in which the values of projected vertex coordinates will be stored.
* @param faceIndex Value between 0 and MAX_FACES-1, used to access the data of a particular tracked face.
*/
void _getFaceModelVerticesProjected(float* verticesProjected, int faceIndex);

/** 
* Returns texture coordinates.
* Texture coordinates for the 3D face model. 
* A pair of u, v coordinates for each vertex.
* 
* Function returns texture coordinates in the vertexNumber*2-dimensional <i>texCoord</i> array, where vertexNumber is value 
* obtained from _getFaceModelVertices() function, for the particular faceIndex.
* 
* An example of use: 
* \code
* 
* //define MaxVertices value that will be larger than the real number of vertices
* public const int MaxVertices = 1024;
* private float[] texCoords = new float[MaxVertices * 2];
*	
* //call of _getFaceModelTextureCoords() function
* VisageTrackerNative._getFaceModelTextureCoords(texCoord, faceIndex);
*	
* \endcode
* @param texCoord Array in which values of texture coordinates will be stored.
* @param faceIndex Value between 0 and MAX_FACES-1, used to access the data of a particular tracked face.
* <br/>
*/
void _getFaceModelTextureCoords(float* texCoord, int faceIndex);

/**
* Index of the first feature point group. 
* @return 2
* <br/>
*/
int _getFP_START_GROUP_INDEX();


/**
* Index of the last feature point group. 
* @return 15
* <br/>
*/
int _getFP_END_GROUP_INDEX();

	
/**
* Get the size of all feature point groups. 
* @param groupSizeArray Array in which the size of each group will be stored.
* @param length Length of groupSizeArray array.
* <br/>
*/
void _getGroupSizes(int* groupSizeArray, int length);

/** Returns camera info.
*
* This function should be called after _openCamera() and _track() functions are preformed, otherwise it will join -1 value to parametar focus and 0 to parameters width and height.
*
* @param focus Parameter to which focal distance of the camera will be assigned.
* @param width Parameter to which width of the frame will be assigned.
* @param height Parameter to which height of the frame will be assigned.
 */
void _getCameraInfo(out float focus, out int width, out int height);


/** Returns the action unit count.
*
* @return Action unit count
 */
int _getActionUnitCount();

/** Returns the name of the action unit with the specified index.
* 
* @param auIndex Index of the action unit for the given faceIndex.
* @return The name of the action unit at the given index.
*/
string[] _getActionUnitName(int auIndex);

/** Returns true if the action unit is used.
*
* @param auIndex Index of the action unit.
* @return True if action unit is used, false otherwise.
*/
bool _getActionUnitUsed(int auIndex);

/** Returns all action unit values.
*
*
* An example of use:
* \code
*
* int AU_COUNT = VisageTrackerNative._getActionUnitCount();
* 
* float[] AUvalues = new float[AU_COUNT];
* 
* // au values for the first tracked face
* VisageTrackerNative._getActionUnitValues(AUvalues, 0);
* 
* \endcode
* 
*
* @param values Array that will be filled with action unit values.
* @param faceIndex Value between 0 and MAX_FACES-1, used to access the data of a particular tracked face.
*/
void _getActionUnitValues(float[] values, int faceIndex);

/** Returns the gaze direction for the given faceIndex.
* 
* Direction is expressed with two values x and y, in radians. Values (0, 0) correspond to person looking straight. X is the horizontal rotation with positive values corresponding to person looking to his/her left. Y is the vertical rotation with positive values corresponding to person looking down.
* 
* An example of use:
* \code
*
* float[] direction = new float[2];
*
* for (int faceIndex = 0; faceIndex < MAX_FACES; faceIndex++)
* {
*    VisageTrackerNative._getGazeDirection(direction, faceIndex);
* }  
* \endcode
*
* Note that gaze direction is influenced with process_eyes parameter from tracker configuration file. 
*
* @param direction Array that will be filled with direction expressed with two values x and y.
* @param faceIndex Value between 0 and MAX_FACES-1, used to access the data of a particular tracked face.
* @return True on success, false on failure that will occur if faceIndex is greater than MAX_FACES or TrackerStatus for the given faceIndex is different than OK.
*/
bool _getGazeDirection(float[] direction, int faceIndex);

/** Returns the global gaze direction for the given faceIndex.
*
*  Global gaze direction is the current estimated gaze direction relative to the camera axis. Direction is expressed with three values determining the rotations around the three axes x, y and z, i.e. pitch, yaw and roll. 
* 
* An example of use:
* \code
*
* float[] direction = new float[3];
*  
* for (int faceIndex = 0; faceIndex < MAX_FACES; faceIndex++)
* {
*    VisageTrackerNative._getGazeDirectionGlobal(direction, faceIndex);
* }
* \endcode
*
* Note that global gaze direction influenced with process_eyes parameter from tracker configuration file.
*
* @param direction Array that will be filled with direction expressed with three values determining the rotations around the three axes x, y and z.
* @param faceIndex Value between 0 and MAX_FACES-1, used to access the data of a particular tracked face.
* @return True on success, false on failure that will occur if faceIndex is greater than MAX_FACES or TrackerStatus for the given faceIndex is different than OK.
*/
bool _getGazeDirectionGlobal(float[] direction, int faceIndex);

/** Returns eye closure value for the given faceIndex.
* 
* Discrete eye closure value is expressed with two values: index 0 represents closure of left eye. Index 1 represents closure of right eye. Value of 1 represents open eye. Value of 0 represents closed eye. 
*
* An example of use:
* \code
*
* float[] closure = new float[2];
*
* // eye closure value for the first tracked face
* VisageTrackerNative._getEyeClosure(closure, 0);
* 
* \endcode
* 
*
* @param closure Array that will be filled with eye closure values: index 0 represents closure of left eye. Index 1 represents closure of right eye. Value of 1 represents open eye. Value of 0 represents closed eye. 
* @param faceIndex Value between 0 and MAX_FACES-1, used to access the data of a particular tracked face.
* @return True on success, false on failure that will occur if faceIndex is greater than MAX_FACES or TrackerStatus for the given faceIndex is different than OK.
*/
bool _getEyeClosure(float[] closure, int faceIndex);

/**
* Returns for all feature points the position in normalized 2D screen coordinates, indicator whether the point is defined, indicator whether the point is detected or estimated
* and feature point quality for the given faceIndex. 
* 
* The 2D feature point coordinates are normalised to image size so that the lower left corner of the image has coordinates 0,0 and upper right corner 1,1.
* 
* For example, first feature point is 2.1, therefore first five values in featurePointArray array will be filled with the following values:
* \code
* 
* featurePointArray = [x_(2.1), y_(2.1), defined_(2.1), detected_(2.1), quality_(2.1), ...]
* 	
* \endcode
* An example of use in Unity: 
* \code
* 
* //array which will be filled with values
* public float[] featurePointArray = new float[2000];
*	
* //call of the _getAllFeaturePoints2D() function
* VisageTrackerNative._getAllFeaturePoints2D(featurePointArray, featurePointArray.Length, faceIndex);
*
* \endcode
*
* @param featurePointArray array in which the position in normalized 2D screen coordinates, indication whether the point is defined and detected and 
* feature point quality will be stored.
* @param length length of featurePointArray.
* @param faceIndex value between 0 and MAX_FACES-1, used to access the data of a particular tracked face.
*
*/
void _getAllFeaturePoints2D(float* featurePointArray, int length, int faceIndex);

/**
	* Returns for all feature points the global 3D position in meters, indicator whether the point is defined, indicator whether the point is detected or estimated
	* and feature point quality for the given faceIndex. 
	* 
	* The coordinate system is such that when looking towards the camera, the direction of x is to the left, y iz up, and z points towards the viewer. The global origin (0,0,0) is placed at the camera.
	* 
	* For example, first feature point is 2.1, therefore first six values in featurePointArray array will be filled with the following values:
	* \code
    * 
	* 	featurePointArray = [x_(2.1), y_(2.1), z_(2.1), defined_(2.1), detected_(2.1), quality_(2.1), ...]
    * 	
	* \endcode
	* An example of use in Unity: 
	* \code
    * 
	*	//array which will be filled with values
	*	public float[] featurePointArray = new float[2000];
	*	
	*	//call of the _getAllFeaturePoints3D() function
	*	VisageTrackerNative._getAllFeaturePoints3D(featurePointArray, featurePointArray.Length, faceIndex);
	*
	* \endcode
	*
	* @param featurePointArray array in which the position in x, y and z coordinates, indication whether the point is defined and detected and 
	* feature point quality will be stored.
	* @param length length of featurePointArray.
	* @param faceIndex value between 0 and MAX_FACES-1, used to access the data of a particular tracked face.
	*
	*/
void _getAllFeaturePoints3D(float* featurePointArray, int length, int faceIndex);

/**
* Returns for all feature points the position in 3D coordinates relative to the face origin, placed at the center between eyes in meters, indicator whether the point is defined, indicator whether the point is detected or estimated
* and feature point quality for the given faceIndex. 
*
* Returned coordinates are in the local coordinate system of the face, with the origin (0,0,0) placed at the center between the eyes. The x-axis points laterally towards the side of the face, y-xis points up and z-axis points into the face.
* 
* For example, first feature point is 2.1, therefore first six values in featurePointArray array will be filled with the following values:
* \code
* 
* featurePointArray = [x_(2.1), y_(2.1), z_(2.1), defined_(2.1), detected_(2.1), quality_(2.1), ...]
* 	
* \endcode
* 
* An example of use in Unity: 
* \code
* 
* //array which will be filled with values
* public float[] featurePointArray = new float[2000];
*	
* //call of the _getAllFeaturePoints3DRelative() function
* VisageTrackerNative._getAllFeaturePoints3DRelative(featurePointArray, featurePointArray.Length, faceIndex);
*	
* \endcode
* 	 
* @param featurePointArray array in which the position in x, y and z coordinates, indication whether the point is defined and detected and 
* feature point quality will be stored.
* @param length length of featurePointArray.
* @param faceIndex value between 0 and MAX_FACES-1, used to access the data of a particular tracked face.
*
*/
void _getAllFeaturePoints3DRelative(float* featurePointArray, int length, int faceIndex);

/**
* Returns the feature points position in normalized 2D screen coordinates, indicator whether the point is defined, indicator whether the point is detected or estimated
* and feature point quality for the given faceIndex. 
* 
* The 2D feature point coordinates are normalised to image size so that the lower left corner of the image has coordinates 0,0 and upper right corner 1,1.
* 
* Function returns position for N feature points in the N*2-dimensional <i>positions</i> array, an indication of whether the point is defined as 0 and 1 in the N-dimensional <i>defined</i> array and detected as 0 and 1 in the N-dimensional <i>detected</i> array. 
* The quality of each point as value between 0 and 1 is returned in <i>quality</i> array.
* For each point, its group and index are specifically listed in the <i>groups</i> and <i>indices</i> arrays.
* 
* An example of use: 
* \code
* // an example for points 2.1, 8.3 and 8.4
* const int N;
* int[] groups = new int[N] {2, 8, 8};
* int[] indices = new int[N]{ 1, 3, 4 };
* float[] positions2D = new float[2*N];
* int[] defined = new int[N];
* int[] detected = new int[N];
* float[] quality = new float[N];
*
* for (int faceIndex = 0; faceIndex < MAX_FACES; faceIndex++)
* {
*	VisageTrackerNative._getFeaturePoints2D(N, groups, indices, positions3D, defined, detected, quality, faceIndex);
* }
*
* \endcode
* @param N number of feature points. 
* @param groups N-dimensional array of the groups of the feature points. 
* @param indices N-dimensional array of the indices of the feature points within the groups. 
* @param positions N*2-dimensional array filled with resulting facial feature points positions. 
* @param defined N-dimensional array filled for every feature point with a value that indicates whether the point is defined. Value 1 is assigned to defined points, and 0 to undefined points. 
* @param detected N-dimensional array filled for every feature point with a value that indicates whether the point is detected. Value 1 is assigned to detected points, and 0 to undetected points. 
* @param quality N-dimensional array filled for every feature point with a value from 0 to 1, where 0 is the worst and 1 is the best quality. 
* @param faceIndex value between 0 and MAX_FACES-1, used to access the data of a particular tracked face.
*/
void _getFeaturePoints2D(int N, int* groups, int* indices, float* positions, int* defined, int* detected, float* quality, int faceIndex);

/**
* Returns the global 3D feature point position in meters, indicator whether the point is defined, indicator whether the point is detected or estimated
* and feature point quality for the given faceIndex. 
* 
* The coordinate system is such that when looking towards the camera, the direction of x is to the left, y iz up, and z points towards the viewer. The global origin (0,0,0) is placed at the camera.
*
* Function returns position for N feature points in the N*3-dimensional <i>positions</i> array, an indication of whether the point is defined as 0 and 1 in the N-dimensional <i>defined</i> array and detected as 0 and 1 in the N-dimensional <i>detected</i> array. 
* The quality of each point as value between 0 and 1 is returned in <i>quality</i> array.
* For each point, its group and index are specifically listed in the <i>groups</i> and <i>indices</i> arrays.
* 
* An example of use: 
* \code
* // an example for points 2.1, 8.3 and 8.4
* const int N;
* int[] groups = new int[N] {2, 8, 8};
* int[] indices = new int[N]{ 1, 3, 4 };
* float[] positions3D = new float[3*N];
* int[] defined = new int[N];
* int[] detected = new int[N];
* float[] quality = new float[N];
*
* for (int faceIndex = 0; faceIndex < MAX_FACES; faceIndex++)
* {
*	VisageTrackerNative._getFeaturePoints3D(N, groups, indices, positions3D, defined, detected, quality, faceIndex);
* }
*
* \endcode
* 
* @param N number of feature points. 
* @param groups N-dimensional array of the groups of the feature points. 
* @param indices N-dimensional array of the indices of the feature points within the groups. 
* @param positions N*3-dimensional array filled with resulting facial feature points positions. 
* @param defined N-dimensional array filled for every feature point with a value that indicates whether the point is defined. Value 1 is assigned to defined points, and 0 to undefined points. 
* @param detected N-dimensional array filled for every feature point with a value that indicates whether the point is detected. Value 1 is assigned to detected points, and 0 to undetected points. 
* @param quality N-dimensional array filled for every feature point with a value from 0 to 1, where 0 is the worst and 1 is the best quality. 
* @param faceIndex value between 0 and MAX_FACES-1, used to access the data of a particular tracked face.
*/
void _getFeaturePoints3D(int N, int* groups, int* indices, float* positions, int* defined, int* detected, float* quality, int faceIndex);

/** 
* Returns the 3D coordinates relative to the face origin, placed at the center between eyes in meters, indicator whether the point is defined, indicator whether the point is detected or estimated
* and feature point quality for the given faceIndex. 
*
* Returned coordinates are in the local coordinate system of the face, with the origin (0,0,0) placed at the center between the eyes. The x-axis points laterally towards the side of the face, y-xis points up and z-axis points into the face.
*
* Function returns position for N feature points in the N*3-dimensional <i>positions</i> array, an indication of whether the point is defined as 0 and 1 in the N-dimensional <i>defined</i> array and detected as 0 and 1 in the N-dimensional <i>detected</i> array. 
* The quality of each point as value between 0 and 1 is returned in <i>quality</i> array.
* For each point, its group and index are specifically listed in the <i>groups</i> and <i>indices</i> arrays.
* 
* An example of use:
* \code
*
* // an example for points 2.1, 8.3 and 8.4
* const int N;
* int[] groups = new int[N] {2, 8, 8};
* int[] indices = new int[N]{ 1, 3, 4 };
* float[] positions = new float[3*N];
* int[] defined = new int[N];
* int[] detected = new int[N];
* float[] quality = new float[N];
*
*
* for (int faceIndex = 0; faceIndex < MAX_FACES; faceIndex++)
* {
*    VisageTrackerNative._getFeaturePoints3DRelative(N, groups, indices, positions, defined, detected, quality, faceIndex);
* }
* 
* \endcode
*
* @param N Number of feature points.
* @param groups N-dimensional array of the groups of the feature points.
* @param indices N-dimensional array of the indices of the feature points within the groups.
* @param positions N*3-dimensional array filled with resulting facial feature points positions.
* @param defined N-dimensional array filled for every feature point with a value that indicates whether the point is defined. Value 1 is assigned to defined points, and 0 to undefined points.
* @param detected N-dimensional array filled for every feature point with a value that indicates whether the point is detected. Value 1 is assigned to detected points, and 0 to undetected points. 
* @param quality N-dimensional array filled for every feature point with a value from 0 to 1, where 0 is the worst and 1 is the best quality.  
* @param faceIndex Value between 0 and MAX_FACES-1, used to access the data of a particular tracked face.
*/
void _getFeaturePoints3DRelative(int N, int* groups, int* indices, float* positions, int* defined, int* detected, float* quality, int faceIndex);

/** Sets tracking configuration. 
* 
* The tracker configuration file name and other configuration parameters are set and will be used for the next tracking session (i.e. when _track() is called). Default configuration files (.cfg) are provided in Samples/data folder. 
* Please refer to the VisageTracker Configuration Manual for further details on using the configuration files and all configurable options.
*
*
* @param trackerConfigFile the name of the tracker configuration file. 
* @param au_fitting_disabled disables the use of the 3D model used to estimate action units (au_fitting_model configuration parameter). If set to true, estimation of action units shall not be performed, and action units related data will not be available. Disabling will result in a small performance gain. 
* @param mesh_fitting_disabled disables the use of the fine 3D mesh (mesh_fitting_model configuration parameter). If set to true, the 3D mesh shall not be fitted and the related information shall not be available. Disabling will result in a small performance gain. 
*/
void _setTrackerConfiguration(string trackerConfigFile, bool au_fitting_disabled, bool mesh_fitting_disabled);

/** Sets the inter pupillary distance. 
* 
* Inter pupillary distance (IPD) is used by the tracker to estimate the distance of the face from the camera. By default, IPD is set to 0.065 (65 millimeters) which is considered average. 
* If the actual IPD of the tracked person is known, this function can be used to set this IPD. As a result, the calculated distance from the camera will be accurate (as long as the camera focal lenght is also set correctly). This is important for applications that require accurate distance. For example, in Augmented Reality applications objects such as virtual eyeglasses can be rendered at appropriate distance and will thus appear in the image with real-life scale.
* 
* IMPORTANT NOTE: In case of multitracking, same IPD will be set for all tracked faces!
*
* @param IPDvalue the inter pupillary distance (IPD) in meters. 
*/
void _setIPD(float IPDvalue);

/** Returns the current inter pupillary distance (IPD) setting. 
 * 
 * IPD setting is used by the tracker to estimate the distance of the face from the camera. 
* 
*
* @return Current setting of inter pupillary distance (IPD) in meters. 
*/
float _getIPD();

/** @} */

