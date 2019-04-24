using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace VisageCSWrapper
{
	/// <summary>
	/// VISAGE_FORMAT is used to signalize what kind of image will be sent to the tracker, specifically how many channels the image will have.
	/// 
	/// Used in the VisageTracker.Track() function.
	/// </summary>
	public enum class VISAGE_FORMAT
	{
		RGB=0,
		BGR,
		LUMINANCE,
		RGBA,
		BGRA
	};


	/// <summary>
	/// VISAGE_ORIGIN is used to signalize what kind of image will be sent to the tracker, specifically about origin.
	/// 
	/// 0 - top-left origin, 1 - bottom-left origin
	///
	/// Used in the VisageTracker.Track() function.
	/// </summary>
	public enum class VISAGE_ORIGIN
	{
		TL = 0,
		BL = 1
	};


	/// <summary>
	/// TRACK_STAT describes the state the tracker is in.
	/// 
	/// Used as return value of VisageTracker.Track() function.
	/// </summary>
	public enum class TRACK_STAT
	{
		OFF=0,
		OK=1,
		RECOVERING=2,
		INIT=3
	};


	/// <summary>
	/// Visage Tracker is a face tracker capable of tracking the head pose, facial features and gaze for multiple faces in video coming from a
	/// video file, camera or other sources.
	/// 
	/// The tracker is fully configurable through comprehensive tracker configuration files. visage|SDK contains optimal configurations 
	/// for common uses such as head tracking and facial features tracking.
	/// Please refer to the <a href="../VisageTracker Configuration Manual.pdf">VisageTracker Configuration Manual</a> for the list of available configurations.
	/// 
	/// The <a href="../VisageTracker Configuration Manual.pdf">VisageTracker Configuration Manual</a> provides full detail
	/// on all available configuration options, allowing to customize the tracker in terms of performance, quality,
	/// and other options.
	/// 
	/// The tracker offers the following outputs as a part of FaceData object:
	/// - 3D head pose,
	/// - facial expression,
	/// - gaze information,
	/// - eye closure,
	/// - facial feature points,
	/// - full 3D face model, textured.
	/// 
	/// The tracker can apply a smoothing filter to tracking results to reduce the inevitable tracking noise. Smoothing factors 
	/// are adjusted separately for different parts of the face. The smoothing settings 
	/// in the supplied tracker configurations are adjusted conservatively to avoid delay in tracking response, yet provide 
	/// reasonable smoothing. For further details please see the smoothing_factors parameter array in the 
	/// <a href="../VisageTracker Configuration Manual.pdf">VisageTracker Configuration Manual</a>.
	/// 
    /// Frames (images) need to be passed sequentially to the VisageTracker::track() method, which 
    /// immediately returns results for the given frame.
	/// 
	/// The tracker requires a set of data and configuration files, available in Samples/data.
	/// 
	/// Please either copy the complete contents of this folder into your application's working folder, or consult <a href="../VisageTracker Configuration Manual.pdf">VisageTracker Configuration Manual</a> for detailed settings.
	/// 
	/// 
	/// 
	/// </summary>
	public class VisageTracker
	{
		/// <summary>
		/// Constructor.
		/// <param name="trackerConfigFile"> the name of the tracker configuration file (.cfg), relative to the current working directory, e.g. "visageSDK\Samples\data\Facial Features Tracker - High.cfg", considering Visage Technologies as a working directory. Default configuration files are provided in Samples/data folder.
		/// Other data files required by the tracker are located in the same folder as the configuration files, so their path is determined from the configuration file's path given here.
		/// For further details see <a href="../VisageTracker Configuration Manual.pdf">VisageTracker Configuration Manual</a>. </param>
		/// </summary>
		
		public VisageTracker(String trackerConfigFile);


		/// <summary>
		/// Performs face tracking (one or more faces) in the given image and returns tracking results and status.
		/// This function should be called repeatedly on a series of images in order to perform continuous tracking.
		/// 
		/// If the tracker needs to be initialized, this will be done automatically before tracking is performed on the given image. 
		/// Initialization means loading the tracker configuration file, required data files and allocating various data buffers to the given image size. 
		/// This operation may take several seconds.
		/// This happens in the following cases:
		///   - In the first frame (first call to track() function).
		///   - When frameWidth or frameHeight are changed, i.e. when they are different from the ones used in the last call to Track() function.
		///   - If SetTrackerConfiguration() function was called after the last call to Track() function.
		///   - When maxFaces is changed, i.e. when it its different from the one used in the last call to track() function.
		///
		/// The tracker results are returned in an array of FaceData objects, one FaceData object for each tracked face.
		/// Contents of each FaceData element depend on the corresponding tracker status (tracker statuses are returned as return value from the function).
		///
		/// The tracking of multiple faces is performed in parallel ( using OpenMP) and performance (speed) may vary depending on the number of CPU cores,
		/// the number of faces in the current image and the value of maxFaces argument.
		///
		/// </summary>
		/// <param name="frameWidth">Width of the frame</param>
		/// <param name="frameHeight">Height of the frame</param>
		/// <param name="imageData">Pointer to image pixel data; size of the array must correspond to frameWidth and frameHeight</param>
		/// <param name="faceData">FaceData instance that will receive the tracking results. No tracking results will be returned if NULL pointer is passed.
		/// On first call of this function, the memory for the required member variables of the passed FaceData object will be allocated and initialized automatically.
		/// </param>
		/// <param name="format">Format of input images passed in imageData. It can not change during tracking. Format can be one of the following:
		/// - VISAGE_FORMAT.RGB: each pixel of the image is represented by three bytes representing red, green and blue channels, respectively.
		/// - VISAGE_FORMAT.BGR: each pixel of the image is represented by three bytes representing blue, green and red channels, respectively.
		/// - VISAGE_FORMAT.RGBA: each pixel of the image is represented by four bytes representing red, green, blue and alpha (ignored) channels, respectively.
		/// - VISAGE_FORMAT.BGRA: each pixel of the image is represented by four bytes representing blue, green, red and alpha (ignored) channels, respectively.
		/// - VISAGE_FORMAT.LUMINANCE: each pixel of the image is represented by one byte representing the luminance (gray level) of the image.
		/// </param>
		/// <param name="origin">No longer used, therefore, passed value will not have an effect on this function. However, the parameter is left to avoid API changes. </param>
		/// <param name="widthStep">Width of the image data buffer, in bytes.</param>
		/// <param name="timeStamp">The timestamp of the the input frame in milliseconds. The passed value will be returned with the tracking data for that frame (FaceData.timeStamp). 
		/// Alternatively, the value of -1 can be passed, in which case the tracker will return time, in milliseconds, measured from the moment when tracking started.</param>
		/// <param name="maxFaces">The maximum number of faces that will be tracked (tracker currently supports up to 20 faces).</param>
		/// <returns>Tracking status (TRACK_STAT.OFF, TRACK_STAT.OK, TRACK_STAT.RECOVERING and TRACK_STAT.INIT, see @ref FaceData for more details)</returns>
		/// @see FaceData
		public int[] Track(int frameWidth, int frameHeight, Byte[] imageData, FaceData[] faceData, int format = VISAGE_FORMAT.RGB, int origin = VISAGE_ORIGIN.TL, int widthStep = 0, long timeStamp = -1, int maxFaces = -1);


		/// <summary>
		/// Set configuration file name.
		/// 
		/// The tracker configuration file name is set and this configuration file will be used for next tracking session (i.e. track() is called). 
		/// Default configuration files (.cfg) are provided in Samples/data folder.
		/// Please refer to the  <a href="../VisageTracker Configuration Manual.pdf">VisageTracker Configuration Manual</a> for further details
		/// on using the configuration files and all configurable options.
		/// </summary>
		/// <param name="trackerConfigFile">The name of the tracker configuration file.</param>
		/// <param name="au_fitting_disabled">Disables the use of the 3D model used to estimate action units (au_fitting_model configuration parameter). If set to true, estimation of Action Units shall not be performed, and Action Units released data in the returned FaceData structure will not be available (FaceData::ActionUnits etc.). Disabling will result in a small performance gain.</param>
		/// <param name="mesh_fitting_disabled">Disables the use of the fine 3D mesh (mesh_fitting_model configuration parameter). If set to true, the 3D mesh shall not be fitted and the related information shall not be available in the returned FaceData structure (FaceData::FaceModelVertices etc.). Disabling will result in small performance gain.</param>
		public void SetTrackerConfiguration(String trackerConfigFile, bool au_fitting_disabled = false, bool mesh_fitting_disabled = false);


		/// <summary>
		/// Stops the tracking.
		/// </summary>
		public void Stop();


		/// <summary>
		/// Reset tracking.
		/// 
		/// Resets the tracker. Tracker will reinitialise.
		/// </summary>
		public void Reset();


		/// <summary>
		/// Inter pupillary distance.
		/// 
		/// Inter pupillary distance (IPD) is used by the tracker to estimate the distance of the face from the camera. 
		/// By default, IPD is set to 0.065 (65 millimetres) which is considered average. If the actual IPD of the tracked person is known, this function can be used to set this IPD. As a result, the calculated distance 
		/// from the camera will be accurate (as long as the camera focal length is also set correctly).
		/// This is important for applications that require accurate distance. For example, in Augmented Reality applications objects such as virtual eyeglasses can be rendered at appropriate distance and will thus appear 
		/// in the image with real-life scale.
		/// 
		/// </summary>
		property float InterPupillaryDistance;
		};
}
