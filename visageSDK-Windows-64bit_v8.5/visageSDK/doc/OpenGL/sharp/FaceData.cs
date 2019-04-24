using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace VisageCSWrapper
{
	/// <summary>
	/// Face data structure, used as container for all face tracking and detection results.
	/// This structure is passed as parameter to the VisageTracker.Track() or VisageFeaturesDetector.DetectFacialFeatures() method. 
	/// Any of these methods copies latest tracking or detection results into it.
	///
	/// When filling the structure with data some members are filled while some are left undefined depending on tracking/detection status. 
	///
	/// <h2>Obtaining tracking data</h2>
	///
	/// The tracker returns these main classes of data:
	///  - 3D head pose
	///  - facial expression
	///  - gaze direction
	///  - eye closure
	///  - facial feature points
	///  - full 3D face model, textured
	///  - screen space gaze position (if calibrated)
	///
	/// The tracker status is the return value of the VisageTracker.Track() functions. 
	/// The following table describes the possible states of the tracker, and lists active member variables (those that are filled with data) for each status.
	///  <table>
	///  <tr><td width="100"><b>TRACKER STATUS</b></td><td><b>DESCRIPTION</b></td><td><b>ACTIVE VARIABLES</b></td></tr>
	///  <tr><td>TRACK_STAT.OFF</td><td>Tracker is not active, i.e. it has not yet been started, or it has been stopped.</td>
	///  <td>N/A</td></tr>
	///  <tr><td>TRACK_STAT.OK</td><td>Tracker is tracking normally.</td>
	///  <td>
	///  TrackingQuality,
	///  FrameRate,
	///  CameraFocus,
	///  FaceScale,
	///  FaceTranslation,
	///  FaceRotation,
	///  ActionUnitCount,
	///  ActionUnitsUsed,
	///  ActionUnits,
	///  ActionUnitsNames,
	///  FeaturePoints3D,
	///  FeaturePoints3DRelative,
	///  FeaturePoints2D,
	///  FaceModelVertexCount,
	///  FaceModelVertices,
	///  FaceModelVerticesProjected,
	///  FaceModelTriangleCount,
	///  FaceModelTriangles,
	///  FaceModelTextureCoords
	/// </td></tr>
	///  <tr><td>TRACK_STAT.RECOVERING</td><td>Tracker has lost the face and is attempting to recover and continue tracking. 
	/// If it can not recover within the time defined by the parameter recovery_timeout in the <a href="../VisageTracker Configuration Manual.pdf">tracker configuration file</a>, 
	/// the tracker will fully re-initialize (i.e. it will assume that a new user may be present).</td>
	///  <td>
	///  FrameRate,
	///  CameraFocus</td></tr>
	///  <tr><td>TRACK_STAT.INIT</td><td>Tracker is initializing. The tracker enters this state immediately when it is started, or when it has lost the face and failed to recover (see TRACK_STAT.RECOVERING above). 
	///  The initialization process is configurable through a number of parameters in the <a href="../VisageTracker Configuration Manual.pdf">tracker configuration file.</a></td>
	///  <td>
	///  FrameRate,
	///  CameraFocus
	/// </td></tr>
	///  </table>
	///
	///  <h3>Smoothing</h3>
	///
	///  The tracker can apply a smoothing filter to tracking results to reduce the inevitable tracking noise. Smoothing factors are adjusted separately for different parts of the face. 
	///  The smoothing settings in the supplied tracker configurations are adjusted conservatively to avoid delay in tracking response, yet provide reasonable smoothing. 
	///  For further details please see the smoothing_factors parameter array in the <a href="../VisageTracker Configuration Manual.pdf">VisageTracker Configuration Manual</a>.
	///  
	///
	///  <h2>Obtaining detection data</h2>
	///
	/// The detector returns these main classes of data for each detected face:
	///  - 3D head pose
	///  - gaze direction
	///  - eye closure
	///  - facial feature points
	///  - full 3D face model, textured.
	///
	/// Detection result is returned from VisageFeaturesDetector.DetectFacialFeatures() function.
	/// The following table describes possible output from the detector and the list of active variables (those that are filled with data). All other variables are left undefined.
	/// <table>
	/// <tr><td width="100"><b>DETECTION RESULT</b></td><td><b>DESCRIPTION</b></td><td><b>ACTIVE VARIABLES</b></td></tr>
	/// <tr><td> 0 </td><td>Detector did not find any faces in the image</td>
	/// <td>N/A</td></tr>
	/// <tr><td> N > 0 </td><td>Detector detected N faces in the image.</td>
	/// <td>
	/// <b>For first N FaceData objects in the array:</b>
	///  CameraFocus,
	///  FaceTranslation,
	///  FaceRotation,
	///  FeaturePoints3D,
	///  FeaturePoints3DRelative,
	///  FeaturePoints2D,
	///  FaceModelVertexCount,
	///  FaceModelVertices,
	///  FaceModelVerticesProjected,
	///  FaceModelTriangleCount,
	///  FaceModelTriangles,
	///  FaceModelTextureCoords
	///  <br>
	///  <b>For other FaceData objects in the array:</b>
	///  N/A
	///  </td></tr>
	/// </table>
	/// 
	/// <h2>Returned data</h2>
	/// 
	/// The following sections give an overview of main classes of data that may be returned in FaceData by the tracker or the detector, and pointers to specific data members.
	/// 
	/// <h3>3D head pose</h3>
	/// 
	/// The 3D head pose consists of head translation and rotation. It is available as absolute pose of the head with respect to the camera.
	/// 
	///  The following member variables return the head pose:
	///  - #FaceTranslation
	///  - #FaceRotation 
	/// 
	/// Both face tracker and face detector return the 3D head pose.
	/// 
	/// <h3>Facial expression</h3>
	/// 
	/// Facial expression is available in a form of Action Units (AUs), which describe the basic motions of the face, such as mouth opening, eyebrow raising etc.
	///
	/// AUs are <a href="../VisageTracker Configuration Manual.pdf">fully configurable</a> in the tracker configuration files. For the full list of default AUs, and for other information about the use and configuration of AUs, please refer to the <a href="../VisageTracker Configuration Manual.pdf">Tracker Configuration Manual</a>.
	/// Please note that the AUs are similar, but not identical to the FACS Action Units.
	///
	///  The following member variables return Action Units data:
	///  - #ActionUnitCount
	///  - #ActionUnitsUsed
	///  - #ActionUnits
	///  - #ActionUnitsNames
	///
	///  Only face tracker returns the facial expression; face detector leaves these variables undefined. Furthermore, it is returned only if the au_fitting_model parameter is defined in the configuration file. 
	///  An example of such file is Facial Features Tracker - High.cfg. Please see the <a href="../VisageTracker Configuration Manual.pdf">VisageTracker Configuration Manual</a> for further details.
	///
	///  <h3>Gaze direction and eye closure</h3>
	///
	///  Gaze direction is available in local coordinate system of the person's face or global coordinate system of the camera. Eye closure is available as binary information (OPEN/CLOSED).
	///
	///  The following member variables return gaze direction and eye closure:
	///  - #GazeDirection
	///  - #GazeDirectionGlobal
	///  - #EyeClosure
	///
	///  Both face tracker and face detector return gaze direction and eye closure.
	///
	///  <h3>Facial feature points</h3>
	///
	/// 2D or 3D coordinates of facial feature points, including the ones defined by the <a href="../MPEG-4 FBA Overview.pdf">MPEG-4 FBA standard</a> and some additional points are available.
	/// 
	/// 3D coordinates are available in global coordinate system or relative to the origin of the face (i.e. the point in the centre between the eyes in the input image).
	///
	///  Facial features are available through the following member variables:
	///  - #FeaturePoints3D
	///  - #FeaturePoints3DRelative
	///  - #FeaturePoints2D
	///
	///  Both face tracker and face detector return facial feature points.
	/// 
	///  <h3>3D face model</h3>
	///
	/// The 3D face model is fitted in 3D to the face in the current image/video frame. The model is a single textured 3D triangle mesh. The texture of the model is the current image/video frame.
	/// This means that, when the model is drawn using the correct perspective it exactly recreates the facial part of the image. The correct perspective is defined by camera focal length (#CameraFocus), 
	/// width and height of the input image or the video frame, model rotation (#FaceRotation) and translation (#FaceTranslation).
	///
	/// The 3D face model is fully configurable and can even be replaced by a custom model; it can also be disabled for performance reasons if not required. Please see the <a href="../VisageTracker Configuration Manual.pdf">VisageTracker Configuration Manual</a> for further details. The default model is illustrated in the following image:
	///
	///  \image html "coord3.png" "3D face model"
	///
	///  There are multiple potential uses for the face model. Some ideas include, but are not limited to:
	///  - Draw textured model to achieve face paint or mask effect.
	///  - Draw the 3D face model into the Z buffer to achieve correct occlusion of virtual objects by the head in AR applications.
	///  - Use texture coordinates to cut out the face from the image.
	///  - Draw the 3D face model from a different perspective than the one in the actual video.
	///  - Insert the 3D face model into another video or 3D scene.
	///
	///  The 3D face model is contained in the following members:
	///  - #FaceModelVertexCount
	///  - #FaceModelVertices
	///  - #FaceModelTriangleCount
	///  - #FaceModelTriangles
	///  - #FaceModelTextureCoords
	///
	///  Both face tracker and face detector return the 3D face model, if the mesh_fitting_model parameter in the configuration file is set (see <a href="../VisageTracker Configuration Manual.pdf">VisageTracker Configuration Manual</a> for details).
	///
	///  <h3>Screen space gaze position</h3>
	///  Screen space gaze position is available if the tracker was provided with calibration repository and screen space gaze estimator is working in real time mode. 
	///  Otherwise tracker returns default screen space gaze data. Default gaze position is centre of screen. Default estimator state is off (ScreenSpaceGazeData.inState == 0). Please refer to VisageGazeTracker documentation for instructions on usage of screen space gaze estimator.
	///  
	///   Screen space gaze position is contained in member gazeData.
	/// 
	///   Only face tracker returns screen space gaze position.
	///
	/// </summary>
	public struct FaceData
	{
		/// <summary>
		/// Constructor.
		/// </summary>
		FaceData();

		/// <summary>
		/// Destructor.
		/// </summary>
		~FaceData();


		/// <summary>
		/// Tracking quality level.
		/// <i>This variable is set only while tracker is tracking (TRACK_STAT_OK) or if the detector has detected a face.</i>
		/// 
		/// Estimated tracking quality level for the current frame. The value is between 0 and 1.
		///
		float trackingQuality;


		/// <summary>
		/// The frame rate of the tracker, in frames per second, measured over last 10 frames.
		/// <i>This variable is set while tracker is running., i.e. while tracking status is not TRACK_STAT.OFF. Face detector leaves this variable undefined.</i>
		/// </summary>
		float FrameRate;


		/// <summary>
		/// Timestamp of the current video frame.
		/// <i>This variable is set while tracker is running., i.e. while tracking status is not TRACK_STAT.OFF.
		/// Face detector leaves this variable undefined.</i>
		/// 
		/// If timeStamp argument passed to VisageTracker.Track() function is different than -1 TimeStamp returns the same value passed 
		/// as a parameter to the VisageTracker.Track() function, otherwise it returns time, in milliseconds, 
		/// measured from the moment when tracking started.
		/// 
		/// </summary>
		long TimeStamp;


		/// <summary>
		/// Translation of the head from the camera.
		/// <i>This variable is set only while tracker is tracking (TRACK_STAT.OK) or if the detector has detected a face.</i>
		/// 
		/// 
		/// Translation is expressed with three coordinates x, y, z.
		/// The coordinate system is such that when looking towards the camera, the direction of x is to the
		/// left, y iz up, and z points towards the viewer - see illustration below. The global origin (0,0,0) is placed at the camera. The reference point on the head is in the centre between the eyes.
		/// 
		/// \image html "coord-camera.png"
		/// 
		/// If the value set for the camera focal length in the <a href="../VisageTracker Configuration Manual.pdf">tracker/detector configuration</a> file
		/// corresponds to the real camera used, the returned coordinates shall be in meters; otherwise the scale of the translation values is not known, but the relative values are still correct 
		/// (i.e. moving towards the camera results in smaller values of z coordinate).
		/// 
		/// <b>Aligning 3D objects with the face</b>
		/// 
		/// The translation, rotation and the camera focus value together form the 3D coordinate system of the head in its current position
		/// and they can be used to align 3D rendered objects with the head for AR or similar applications.
		/// 
		/// The relative facial feature coordinates (FeaturePoints3DRelative)
		/// can then be used to align rendered 3D objects to the specific features of the face, like putting virtual eyeglasses on the eyes.
		/// 
		/// @see FaceRotation
		/// </summary>
		float[] FaceTranslation;


        /// <summary>
        /// Rotation of the head.
        /// <i>This variable is set only while tracker is tracking (TRACK_STAT.OK) or if the detector has detected a face.</i>
        /// 
        /// This is the estimated rotation of the head, in radians.
        /// Rotation is expressed with three values determining the rotations
        /// around the three axes x, y and z, in radians. This means that the values represent
        /// the pitch, yaw and roll of the head, respectively. The zero rotation
        /// (values 0, 0, 0) corresponds to the face looking straight ahead along the camera axis.
        /// Positive values for pitch correspond to head turning down.
        /// Positive values for yaw correspond to head turning right in the input image.
        /// Positive values for roll correspond to head rolling to the left in the input image, see illustration below.
        /// The values are in radians.
        /// 
        /// Note: The order to properly apply these rotations is y-x-z.
        /// 
        /// \image html "coord-rotation.png"
        /// 
        /// @see FaceTranslation
        /// </summary>
        float[] FaceRotation;


		/// <summary>
		/// Gaze direction.
		/// <i>This variable is set only while tracker is tracking (TRACK_STAT_OK) or if the detector has detected a face.</i>
		/// 
		/// This is the current estimated gaze direction relative to the person's head.
		/// Direction is expressed with two values x and y, in radians. Values (0, 0) correspond to person looking straight.
		/// X is the horizontal rotation with positive values corresponding to person looking to his/her left.
		/// Y is the vertical rotation with positive values corresponding to person looking down.
		/// 
		/// @see GazeDirectionGlobal
		/// </summary>
		float[] GazeDirection;


		/// <summary>
		/// Global gaze direction, taking into account both head pose and eye rotation.
		/// <i>This variable is set only while tracker is tracking (TRACK_STAT_OK) or if the detector has detected a face.</i>
		/// 
		/// This is the current estimated gaze direction relative to the camera axis.
		/// Direction is expressed with three values determining the rotations
		/// around the three axes x, y and z, i.e. pitch, yaw and roll. Values (0, 0, 0) correspond to the gaze direction parallel to the camera axis.
		/// Positive values for pitch correspond to gaze turning down.
		/// Positive values for yaw correspond to gaze turning right in the input image.
		/// Positive values for roll correspond to face rolling to the left in the input image, see illustration below.
		/// 
		/// The values are in radians.
		/// 
		/// \image html "coord-rotation-eye.png"
		/// 
		/// The global gaze direction can be combined with eye locations to determine the line(s) of sight in the real-world coordinate system with the origin at the camera.
		/// To get eye positions use #FeaturePoints3D and FDP.GetFP() function, e.g.:
		/// 
		/// @see GazeDirection, FeaturePoints3D
		/// </summary>
		float[] GazeDirectionGlobal;


		/// <summary>
		/// Discrete eye closure value. 
		/// <i>This variable is set only while tracker is tracking (TRACK_STAT.OK) or if the detector has detected a face.</i>
		/// 
		/// Index 0 represents closure of left eye. Index 1 represents closure of right eye.
		/// Value of 1 represents open eye. Value of 0 represents closed eye.
		/// </summary>
		float[] EyeClosure;


		/// <summary>
		/// Number of facial Shape Units.
		/// <i>This variable is set while tracker is running., i.e. while tracking status is not TRACK_STAT_OFF or if the detector has detected a face and only if au_fitting_model parameter in the configuration file is set (see <a href="../VisageTracker Configuration Manual.pdf">VisageTracker Configuration Manual</a> for details).</i>
		/// 
		/// Number of shape units that are defined for current face model.
		/// 
		/// @see ShapeUnits 
		/// </summary>
		int ShapeUnitCount;


		/// <summary>
		/// List of current values for facial Shape Units, one value for each shape unit.
		/// <i>This variable is set only while tracker is tracking (TRACK_STAT_OK) or if the detector has detected a face and only if au_fitting_model parameter in the configuration file is set (see <a href="../VisageTracker Configuration Manual.pdf">VisageTracker Configuration Manual</a> for details).</i>
		/// 
		/// Shape units can be described as static parameters of the face that are specific for each individual (e.g. shape of the nose).
		/// 
		/// 
		/// The shape units used by the tracker and detector are defined in the
		/// 3D face model file , specified by the au_fitting_model in the configuration file (see the <a href="../VisageTracker Configuration Manual.pdf">VisageTracker Configuration Manual</a> for details).
		/// 
		/// @see ShapeUnitCount
		/// 
		/// </summary>
		float[] ShapeUnits;


		/// <summary>
		/// Number of facial Action Units.
		/// <i>This variable is set only while tracker is tracking (TRACK_STAT.OK) and only if au_fitting_model parameter in the configuration file is set (see <a href="../VisageTracker Configuration Manual.pdf">VisageTracker Configuration Manual</a> for details).
		/// Face detector leaves this variable undefined.</i>
		/// 
		/// Number of action units that are defined for current face model.
		/// 
		/// @see ActionUnits, ActionUnitsUsed, ActionUnitsNames
		/// 
		/// </summary>
		int ActionUnitCount;


		/// <summary>
		/// Used facial Action Units.
		/// <i>This variable is set only while tracker is tracking (TRACK_STAT.OK) and only if au_fitting_model parameter in the configuration file is set (see <a href="../VisageTracker Configuration Manual.pdf">VisageTracker Configuration Manual</a> for details).
		/// Face detector leaves this variable undefined.</i>
		/// 
		/// List of values, one for each action unit, to determine if specific action unit is actually used in the current tracker configuration.
		/// Values are as follows: 1 if action unit is used, 0 if action unit is not used.
		/// 
		/// @see ActionUnits, ActionUnitCount, ActionUnitsNames
		/// 
		/// </summary>
		int[] ActionUnitsUsed;


		/// <summary>
		/// List of current values for facial Action Units, one value for each Action Unit. Action Units are the basic motions of the face, such as mouth opening, eyebrow raising etc.
		/// <i>This variable is set only while tracker is tracking (TRACK_STAT.OK) and only if au_fitting_model parameter in the configuration file is set (see <a href="../VisageTracker Configuration Manual.pdf">VisageTracker Configuration Manual</a> for details).
		/// Face detector leaves this variable undefined.</i>
		/// 
		/// The Action Units used by the tracker are defined in the
		/// 3D face model file specified by au_fitting_model parameter in the configuration file (see the <a href="../VisageTracker Configuration Manual.pdf">VisageTracker Configuration Manual</a> for details).
		/// Furthermore, the tracker configuration file defines the names of Action Units and these names can be accessed through ActionUnitsNames.
		/// Please refer to section or <a href="../VisageTracker Configuration Manual.pdf">VisageTracker Configuration Manual</a> for full list of Action Units.
		/// 
		/// @see ActionUnitsUsed, ActionUnitCount, ActionUnitsNames
		/// 
		/// </summary>
		float[] ActionUnits;


		/// <summary>
		/// List of facial Action Units names.
		/// 
		/// <i>This variable is set only while tracker is tracking (TRACK_STAT.OK) and only if au_fitting_model parameter in the configuration file is set (see <a href="../VisageTracker Configuration Manual.pdf">VisageTracker Configuration Manual</a> for details).
		/// Face detector leaves this variable undefined.</i>
		/// 
		/// @see ActionUnitsUsed, ActionUnitCount, ActionUnits
		/// 
		/// </summary>
		String[] ActionUnitsNames;


		/// <summary>
		/// Facial feature points (global 3D coordinates).
		/// <i>This variable is set only while tracker is tracking (TRACK_STAT.OK) or if the detector has detected a face.</i>
		/// 
		/// The coordinate system is such that when looking towards the camera, the direction of x is to the
		/// left, y iz up, and z points towards the viewer.  The global origin (0,0,0) is placed at the camera, see illustration.
		/// 
		/// \image html "coord-camera.png"
		/// 
		/// If the value set for the camera focal length in the <a href="../VisageTracker Configuration Manual.pdf">tracker/detector configuration</a> file
		/// corresponds to the real camera used, the returned coordinates shall be in meters; otherwise the scale is not known, but the relative values are still correct 
		/// (i.e. moving towards the camera results in smaller values of z coordinate).
		/// 
		/// The feature points are identified
		/// according to the MPEG-4 standard (with extension for additional points), so each feature point is identified by its group and index. For example, the tip of the chin
		/// belongs to group 2 and its index is 1, so this point is identified as point 2.1. The identification of all feature points is
		/// illustrated in the following image:
		///
		/// \image html "mpeg-4_fba.png"
		/// \image html "half_profile_physical_2d.png" "Visible/Physical contour"
		/// 
		/// Certain feature points, like the ones on the tongue and teeth, can not be reliably detected so they are not returned
		/// and their coordinates are always set to zero. These points are:
		/// 6.1, 6.2, 6.3, 6.4, 9.8, 9.9, 9.10, 9.11, 10.1, 10.2, 10.3, 10.4, 10.5, 10.6, 11.4, 11.5, 11.6.
		/// 
		/// Several other points are estimated, rather than accurately detected, due to their specific locations. These points are:
		/// 2.10, 2.11, 2.12, 2.13, 2.14, 5.1, 5.2, 5.3, 5.4, 7.1, 9.1, 9.2, 9.6, 9.7, 9.12, 9.13, 9.14, 10.7, 10.8, 10.9, 10.10, 11.1,
		/// 11.2, 11.3, 12.1.
		/// 
		/// Face contour - group 13 and group 15. Face contour is available in two versions: the visible contour (points 13.1 - 13.17) and the physical contour (points 15.1 - 15.17). 
        /// For more details regarding face contour please refer to the documentation of FDP.
		///
        /// Nose contour - group 14, points: 14.21, 14.22, 14.23, 14.24, 14.25 
        ///
		/// The resulting feature point coordinates are returned in form of an FDP object. This is a container class used for storage of MPEG-4 feature points.
		/// It provides functions to access each feature point by its group and index and to read its coordinates.
		/// 
		/// @see FeaturePoints3DRelative, FeaturePoints2D, FDP
		/// 
		/// </summary>
		FDP[] FeaturePoints3D;


		/// <summary>
		///  Facial feature points (3D coordinates relative to the face origin, placed at the centre between eyes).
		/// 
		/// <i>This variable is set only while tracker is tracking (TRACK_STAT.OK) or if the detector has detected a face.</i>
		/// 
		/// The coordinates are in the local coordinate system of the face, with the origin (0,0,0) placed at the centre between the eyes.
		/// The x-axis points laterally towards the side of the face, y-axis points up and z-axis points into the face - see illustration below.
		/// 
		/// \image html "coord3.png" "Coordinate system"
		/// 
		/// The feature points are identified
		/// according to the MPEG-4 standard (with extension for additional points), so each feature point is identified by its group and index. For example, the tip of the chin
		/// belongs to group 2 and its index is 1, so this point is identified as point 2.1. The identification of all feature points is
		/// illustrated in the following image:
		///
		/// \image html "mpeg-4_fba.png"
		/// \image html "half_profile_physical_2d.png" "Visible/Physical contour"
		/// 
		/// Certain feature points, like the ones on the tongue and teeth, can not be reliably detected so they are not returned
		/// and their coordinates are always set to zero. These points are:
		/// 6.1, 6.2, 6.3, 6.4, 9.8, 9.9, 9.10, 9.11, 10.1, 10.2, 10.3, 10.4, 10.5, 10.6, 11.4, 11.5, 11.6.
		/// 
		/// Several other points are estimated, rather than accurately detected, due to their specific locations. These points are:
		/// 2.10, 2.11, 2.12, 2.13, 2.14, 5.1, 5.2, 5.3, 5.4, 7.1, 9.1, 9.2, 9.6, 9.7, 9.12, 9.13, 9.14, 10.7, 10.8, 10.9, 10.10, 11.1,
		/// 11.2, 11.3, 12.1.
		/// 
		/// Face contour - group 13 and group 15. Face contour is available in two versions: the visible contour (points 13.1 - 13.17) and the physical contour (points 15.1 - 15.17). 
        /// For more details regarding face contour please refer to the documentation of FDP.
		///
        /// Nose contour - group 14, points: 14.21, 14.22, 14.23, 14.24, 14.25 
        ///
		/// The resulting feature point coordinates are returned in form of an FDP object. This is a container class used for storage of MPEG-4 feature points.
		/// It provides functions to access each feature point by its group and index and to read its coordinates.
		/// 
		/// @see FeaturePoints3D, FeaturePoints2D, FDP
		/// 
		/// </summary>
		FDP[] FeaturePoints3DRelative;


		/// <summary>
		/// Facial feature points (2D coordinates).
		/// <i>This variable is set only while tracker is tracking (TRACK_STAT.OK) or if the detector has detected a face.</i>
		/// 
		/// The 2D feature point coordinates are normalized to image size so that the lower left corner of the image has coordinates 0,0 and upper right corner 1,1.
		/// 
		/// The feature points are identified
		/// according to the MPEG-4 standard (with extension for additional points), so each feature point is identified by its group and index. For example, the tip of the chin
		/// belongs to group 2 and its index is 1, so this point is identified as point 2.1. The identification of all feature points is
		/// illustrated in the following image:
		///
		/// \image html "mpeg-4_fba.png"
		/// \image html "half_profile_physical_2d.png" "Visible/Physical contour"
		/// 
		/// Certain feature points, like the ones on the tongue and teeth, can not be reliably detected so they are not returned
		/// and their coordinates are always set to zero. These points are:
		/// 6.1, 6.2, 6.3, 6.4, 9.8, 9.9, 9.10, 9.11, 10.1, 10.2, 10.3, 10.4, 10.5, 10.6, 11.4, 11.5, 11.6.
		/// 
		/// Several other points are estimated, rather than accurately detected, due to their specific locations. These points are: 
		/// 2.10, 2.11, 2.12, 2.13, 2.14, 5.1, 5.2, 5.3, 5.4, 7.1, 9.1, 9.2, 9.6, 9.7, 9.12, 9.13, 9.14, 10.7, 10.8, 10.9, 10.10, 11.1,
		/// 11.2, 11.3, 12.1.
		/// 
		/// Face contour - group 13 and group 15. Face contour is available in two versions: the visible contour (points 13.1 - 13.17) and the physical contour (points 15.1 - 15.17). 
        /// For more details regarding face contour please refer to the documentation of FDP.
		///
        /// Nose contour - group 14, points: 14.21, 14.22, 14.23, 14.24, 14.25 
        ///
		/// The resulting feature point coordinates are returned in form of an FDP object. This is a container class used for storage of MPEG-4 feature points.
		/// It provides functions to access each feature point by its group and index and to read its coordinates. Note that FDP stores 3D points and in the case of 2D feature points only the x and y coordinates of each point are used.
		/// 
		/// @see FeaturePoints3D, FeaturePoints3DRelative, FDP
		/// </summary>
		FDP[] FeaturePoints2D;


		/// <summary>
		/// Number of vertices in the 3D face model.
		/// <i>This variable is set only while tracker is tracking (TRACK_STAT.OK) or if the detector has detected a face, providing that the mesh_fitting_model parameter is set in the configuration file (see <a href="../VisageTracker Configuration Manual.pdf">VisageTracker Configuration Manual</a> for details).</i>
		/// 
		/// @see FaceModelVertices, FaceModelVerticesProjected, FaceModelTriangleCount, FaceModelTriangles, FaceModelTextureCoords
		/// </summary>
		int FaceModelVertexCount;


		/// <summary>
		/// List of vertex coordinates of the 3D face model.
		/// <i>This variable is set only while tracker is tracking (TRACK_STAT.OK) or if the detector has detected a face, providing that the mesh_fitting_model parameter is set in the configuration file (see <a href="../VisageTracker Configuration Manual.pdf">VisageTracker Configuration Manual</a> for details).</i>
		/// 
		/// The format of the list is x, y, z coordinate for each vertex.
		/// 
		/// The coordinates are in the local coordinate system of the face, with the origin (0,0,0) placed at the center between the eyes.
		/// The x-axis points laterally  towards the side of the face, y-axis points up and z-axis points into the face - see illustration below.
		/// 
		/// \image html "coord3.png" "Coordinate system"
		/// 
		/// To transform the coordinates into the coordinate system of the camera, use faceTranslation and faceRotation.
		/// 
		/// If the value set for the camera focal length in the <a href="../VisageTracker Configuration Manual.pdf">tracker/detector configuration</a> file
		/// corresponds to the real camera used, the scale of the coordinates shall be in meters; otherwise the scale is not known.
		/// 
		/// 
		/// @see FaceModelVertexCount, FaceModelVerticesProjected, FaceModelTriangleCount, FaceModelTriangles, FaceModelTextureCoords
		/// </summary>
		float[] FaceModelVertices;


		/// <summary>
		/// List of projected (image space) vertex coordinates of the 3D face model.
		/// <i>This variable is set only while tracker is tracking (TRACK_STAT.OK) or if the detector has detected a face, providing that the mesh_fitting_model parameter is set in the configuration file (see <a href="../VisageTracker Configuration Manual.pdf">VisageTracker Configuration Manual</a> for details).</i>
		/// 
		/// The format of the list is x, y coordinate for each vertex.
		/// The 2D coordinates are normalised to image size so that the lower left corner of the image has coordinates 0,0 and upper right corner 1,1.
		/// 
		/// 
		/// @see FaceModelVertexCount, FaceModelVertices, FaceModelTriangleCount, FaceModelTriangles, FaceModelTextureCoords
		/// </summary>
		float[] FaceModelVerticesProjected;


		/// <summary>
		/// Number of triangles in the 3D face model. 
		/// <i>This variable is set only while tracker is tracking (TRACK_STAT.OK) or if the detector has detected a face, providing that the mesh_fitting_model parameter is set in the configuration file (see <a href="../VisageTracker Configuration Manual.pdf">VisageTracker Configuration Manual</a> for details).</i>
		/// 
		/// @see FaceModelVertexCount, FaceModelVertices, FaceModelVerticesProjected, FaceModelTriangles, FaceModelTextureCoords
		/// </summary>
		int FaceModelTriangleCount;


		/// <summary>
		/// Triangles list for the 3D face model.
		/// <i>This variable is set only while tracker is tracking (TRACK_STAT.OK), or if the detector has detected a face, providing that the mesh_fitting_model parameter is set in the configuration file (see <a href="../VisageTracker Configuration Manual.pdf">VisageTracker Configuration Manual</a> for details).</i>
		/// 
		/// Each triangle is described by three indices into the list of vertices @ref FaceModelVertices (counter-clockwise convention is used for normals direction).
		/// 
		/// @see FaceModelVertexCount, FaceModelVertices, FaceModelVerticesProjected, FaceModelTriangleCount, FaceModelTextureCoords
		/// </summary>
		int[] FaceModelTriangles;


		/// <summary>
		/// Texture coordinates for the 3D face model.
		/// <i>This variable is set only while tracker is tracking (TRACK_STAT.OK) or if the detector has detected a face, providing that the mesh_fitting_model parameter is set in the configuration file (see <a href="../VisageTracker Configuration Manual.pdf">VisageTracker Configuration Manual</a> for details).</i>
		/// 
		/// A pair of u, v coordinates for each vertex. When FaceData is obtained from the tracker, the texture image is the current video frame.
		/// When FaceData is obtained from detector, the texture image is the input image of the detector.
		/// 
		/// @see FaceModelVertexCount, FaceModelVertices, FaceModelVerticesProjected, FaceModelTriangleCount, FaceModelTriangles
		/// </summary>
		float[] FaceModelTextureCoords;
		
		/// <summary>
		/// Static texture coordinates of the mesh from the configuration file parameter mesh_fitting_model.
		/// They can be used to apply textures based on the texture template for the unwrapped mesh to the face model. The texture template for these coordinates is provided in jk_300_textureTemplate.png
		/// <i>This variable is set only while tracker is tracking (TRACK_STAT.OK) or if the detector has detected a face, providing that the mesh_fitting_model parameter is set in the configuration file (see <a href="../VisageTracker Configuration Manual.pdf">VisageTracker Configuration Manual</a> for details).</i>
		/// 
		/// A pair of u, v coordinates for each vertex.
		/// 
		/// @see FaceModelVertexCount, FaceModelVertices, FaceModelVerticesProjected, FaceModelTriangleCount, FaceModelTriangles
		/// </summary>
		float[] FaceModelTextureCoordsStatic;


		/// <summary>
		/// Scale of facial bounding box.
		/// </summary>
		int FaceScale;


		/// <summary>
		/// Focal distance of the camera, as configured in the <a href="../VisageTracker Configuration Manual.pdf">tracker/detector configuration</a> file.
		/// <i>This variable is set while tracker is running (any status other than TRACK_STAT.OFF), or if the detector has detected a face.</i>
		/// 
		/// The camera focal distance mainly used for 3D scene set-up and accurate interpretation of tracking data.
		/// This member corresponds to the camera_focus parameter in the <a href="../VisageTracker Configuration Manual.pdf">tracker/detector configuration</a> file.
		/// </summary>
		float CameraFocus;


		/// <summary>
		/// Structure holding screen space gaze position and quality for current frame.
		/// <i>This variable is set only while tracker is tracking (TRACK_STAT_OK).
		/// Face detector leaves this variable undefined.</i>
		///
		/// Position values are dependent on estimator state. Please refer to VisageGazeTracker and ScreenSpaceGazeData documentation for more details. 
		///
		/// </summary>
		ScreenSpaceGazeData GazeData;


		/// <summary>
		/// The session level gaze tracking quality in online mode. 
		/// Quality is returned as discrete grade from 0 to 1, where 0 is the worst and 1 is the best quality. The grade is 0 also when the gaze tracking is off or calibrating.
		/// </summary>
		float GazeQuality;
	}
}

