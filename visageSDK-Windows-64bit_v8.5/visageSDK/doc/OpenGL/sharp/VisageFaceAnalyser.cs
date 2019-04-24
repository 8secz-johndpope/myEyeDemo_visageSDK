using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace VisageCSWrapper
{
    /// <summary>
    /// VFA_FLAGS are used to describe whether age, gender and/or emotion data have been loaded successfully.
    /// 
    /// Flags are returned as bitwise combination in function Init().
    /// </summary>
    public enum class VFA 
	{
		AGE = 1,
		GENDER = 2,
		EMOTION = 4
	};
    
	/// <summary>
	/// Visage Face Analyser contains face analysis algorithms for estimating gender and emotion from frontal facial images (yaw between -20 and 20 degrees).
	/// 
	/// Before using VisageFaceAnalyser, it must be initialized using the function Init(). Alternatively, data path can be passed to the constructor. Data path represent a relative path from 
	/// the working directory to the directory that contains VisageFaceAnalyser data files. See the code example in EstimateGender() function.
	/// 
	/// After the initialization the following types of analysis can be used:
	/// 
	/// <table>
	///  <tr><td width="250"><b>ANALYSIS TYPE</b></td><td width="200"><b>FUNCTION</b></td></tr>
	///  <tr><td>age estimation</td><td>EstimateAge()</td></tr>
	///  <tr><td>gender estimation</td><td>EstimateGender()</td></tr>
	///  <tr><td>emotion estimation</td><td>EstimateEmotion()</td></tr>
	/// </table>
	/// 
	/// </summary>
	public class VisageFaceAnalyser
	{
		/// <summary>
		/// Constructor.
		/// </summary>
		public VisageFaceAnalyser();
		
		/// <summary>
		/// Constructor.
		/// <param name="dataPath">Relative path from the working directory to the directory that contains VisageFaceAnalyser data files.</param>
		/// </summary>
		public VisageFaceAnalyser(System::String^ dataPath);
		
		
		/// <summary>
		/// Destructor.
		/// </summary>
		public ~VisageFaceAnalyser();
		

		/// <summary>
		/// Initialise VisageFaceAnalyser. This function must be called before using VisageFaceAnalyser by passing it a path to the folder containing the VisageFaceAnalyser data files. 
		/// Within the Visage|SDK package, this folder is Samples/data/bdtsdata/LBF/vfadata. When implementing applications, developers may copy this folder as part of their application. 
		/// The path is given relative to the current working directory at the time of calling the Init() function. 
		/// The VisageFaceAnalyser data folder contains the following subfolders corresponding to the various types of analysis that can be performed:
		/// 
		/// <ul>
		///  <li>ad: age estimation</li>
		///  <li>gd: gender estimation</li>
		///  <li>ed: emotion estimation</li>
		/// </ul>
		/// 
		/// Note that it is not necessary for all subfolders to be present, only the ones needed by the types of analysis that will be performed. For example, if only gender estimation will be performed, 
		/// then only the gd folder is required.
		/// 
		/// The return value of the Init() function indicates which types of analysis have been successfully initialised. Subsequent attempt to use a type of analysis that was not initialized shall fail; for example, 
		/// if only gd folder was present at initialization, attempt to use EstimateEmotion() shall fail.
		/// 
		/// The return value is a bitwise combination of flags indicating which types of analysis are successfully initialized.
		///
		/// 
		/// </summary>
		/// <param name="dataPath">Relative path from the working directory to the directory that contains VisageFaceAnalyser data files.</param>
		/// <returns>
		/// The return value of 0 indicates that no data files are found and no analysis will be possible. In such case, the placement of data files should be verified. 
		/// A non-zero value indicates that one or more types of analysis are successfully initialized. 
		/// The specific types of analysis that have been initialized are indicated by a bitwise combination of the following flags: VFA_AGE, VFA_GENDER, VFA_EMOTION.
		/// </returns>
		public int Init(String dataPath);


		/// <summary>
		/// Estimates gender from a facial image.
		///
		/// \if WIN_DOXY
		/// 
		/// The function returns 1 if estimated gender is male and 0 if it is a female. Prior to using this function, it is necessary to process the facial image or video frame using 
		/// VisageTracker or VisageFeaturesDetector and pass the obtained data to this function. 
		/// An example of use, estimate gender on the first face detected in an image file:
		/// 
		/// \code
		/// 
		/// // load an image and store it in VSImage object
		/// VisageFaceAnalyser vfa = new VisageFaceAnalyser();
		/// int isInitialised = vfa.init("./bdtsdata/LBF/vfadata");
		/// 
		/// int maxFaces = 100;
		/// FaceData[] dataArray = new FaceData[maxFaces];
		/// 
		/// if (isInitialised)
		/// {
		///    int numFaces = vfd.detectFacialFeatures(frame, dataArray, maxFaces, 0.1f);
		/// 
		///     if (numFaces > 0)
		///         int gender = vfa.estimateGender(data);
		/// }
		/// \endcode
		/// \endif
		///
		/// </summary>
		/// <param name="facedata">Needs to contain FDP data. FDP data remains unchanged.</param>
		/// <returns>Returns 0 if estimated gender is female, 1 if it is a male and -1 if it failed.</returns>
		public int EstimateGender(FaceData facedata);


		/// <summary>
		/// Estimates emotion from a facial image.
		/// The function returns estimated probabilities for basic emotions. Prior to using this function, it is necessary to process the facial image or video frame using 
		/// VisageTracker or VisageFeaturesDetector and pass the obtained data to this function. 
		/// </summary>
		/// <param name="facedata">Needs to contain FDP data. FDP data remains unchanged.</param>
		/// <param name="probEstimates">Array of 7 doubles. If successful, the function will fill this array with estimated probabilities for emotions in this order: anger, disgust, fear, happiness, sadness, surprise and neutral. 
		/// Each probability will have a value between 0 and 1. Sum of probabilities does not have to be 1.</param>
		/// <returns>Returns 1 if estimation was successful.</returns>
		/// 
		/// See also: Facedata, VisageTracker, VisageDetector
		/// 
		public int EstimateEmotion(FaceData facedata, float[] probEstimates);


		/// <summary>
		/// Estimates age from a facial image.
		/// 
		/// The function returns estimated age. Prior to using this function, it is necessary to process the facial image or video frame using 
		/// VisageTracker or VisageFeaturesDetector and pass the obtained data to this function. 
		/// An example of use, estimate age on the first face detected in an image file:
		/// </summary>
		/// \code
		/// VsImage ///frame = 0;
		/// frame = cvLoadImage(fileName);
		/// VisageFaceAnalyser /// m_VFA= new VisageFaceAnalyser();
		/// const char ///dataPath="./bdtsdata/LBF/vfadata";
		/// int is_initialized = m_VFA->init(dataPath);
		/// 
		/// if((is_initialized & VFA_AGE) == VFA_AGE)
		/// {
		/// int maxFaces = 100;
		/// FaceData/// data = new FaceData[maxFaces];
		/// 
		/// int n_faces = m_Detector->detectFacialFeatures(frame, data, maxFaces);
		/// 
		/// if (n_faces > 0)
		/// {
		/// 	float detectionSuccessful = m_VFA->estimateAge(&data[i]);
		/// }
		/// }
		/// \endcode
		///
		/// <param name="facedata">Needs to contain FDP data. FDP data remains unchanged.</param>
		/// <returns>Returns estimated age and -1 if it failed.</returns>
		/// 
		public float EstimateAge(FaceData facedata);

		/// <summary>
		/// Get normalized face image.
		///
		/// This function returns normalized face image with corresponding feature points.
		/// Size of the normalized face in the image is such that inter-pupillary distance is approximately quarter of the image width.
		/// 
		/// Face will be normalized to a varying degree depending on normalization type. For example rotated 
		/// face with open mouth will only have its pose straightened with normalization type VS_NORM.POSE while
		/// with addition of VS_NORM.AU normalized face will also have closed mouth.
		///
		/// Note that the face will always have its pose straightened.
		/// 
		/// Types of normalization are:
		///   - VS_NORM.POSE - face translation and rotation are set to zero thereby normalizing the pose
		///   - VS_NORM.SU - parameters describing the face shape (shape units) are set to zero thereby normalizing the face shape
		///   - VS_NORM.AU - parameters describing facial movements (action units) are set to zero, for example open mouth will be closed
		///
		/// Different types of normalization can be combined with "|" operator, for example VS_NORM.POSE | VS_NORM.SU.
		/// </summary>
		/// <param name="frame">Image containing the face to be normalized, must be grey-scale</param>
		/// <param name="normFace">Image containing the normalized face; it must be allocated before calling the function; face size will depend on this image size</param>
		/// <param name="face_data">FaceData structure containing the information about the face that will be normalized</param>
		/// <param name="normFDP">Features points that correspond to the normalized face; coordinates are normalized to 0-1 range</param>
		/// <param name="norm_type">Normalization type, a binary combination of VS_NORM.POSE - normalizes pose, VS_NORM.SU - normalizes shape units and VS_NORM.AU - normalizes action units</param>
		/// <param name="dataPath">Path to the folder where Face Detector.cfg is located, default values is ""</param>
		///
		public void GetNormalizedFaceImage(VSImage^ frame, VSImage^ normFace, FaceData^ face_data, FDP^% normFDP, VS_NORM norm_type, System::String^ dataPath);
	}
}
