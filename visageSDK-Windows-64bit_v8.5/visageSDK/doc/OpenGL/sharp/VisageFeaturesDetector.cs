using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace VisageCSWrapper
{
	/// <summary>
	/// Faces and facial features detector implementation.
	/// 
	/// This class detects one or more faces and their facial features in an image. The input is an image bitmap or an image file in one of the supported file formats: JPEG, PNG, BMP or PPM.
	/// The results are, for each detected face, the 3D head pose, gaze direction, eye closure, the coordinates of facial feature points, e.g. chin tip, nose tip, lip corners etc. and 3D face model fitted to the face.
	/// The results are returned in one or more FaceData objects, one for each detected face. Please refer to the FaceData documentation for detailed description of returned data.
	/// 
	/// 
	/// To use the detector, first initialise it by calling the function @ref Initialize(). Alternatively, data path can be passed to the constructor. Data path represent a relative path from 
	/// the working directory to the "bdtsdata" directory that contains VisageFeaturesDetector data files. Finally, call the function @ref DetectFacialFeatures() to perform facial features detection on the image. 
	/// 
	/// The path to the folder "bdtsdata" (located in Samples/data) must be passed to the initialization function, for example: 
	/// 
	/// \code
	/// gVisageFeaturesDetector = new VisageFeaturesDetector(@"..\Samples\data\bdtsdata");
	/// \endcode
	/// 
	/// Additionally, the following files (located in Samples/data) are expected to be located in the same folder as the /bdtsdata folder: candide3.fdp, candide3.wfm, Face Detector.cfg.
	/// The whole "bdtsdata" folder and 3 additional files must be distributed with an application using VisageFeaturesDetector, and its path passed to the initialization function.
	/// 
	/// </summary>
	public class VisageFeaturesDetector
	{
		/// <summary>
		/// Constructor.
		/// </summary>
		public VisageFeaturesDetector();
		
		
		/// <summary>
		/// Constructor that receives path to the "bdtsdata" folder. Using this method, there is no need to call @ref Initialize(). For more information about data files required for facial features detection, please
		/// take a look at @ref Initialize().
		/// </summary>
		/// <param name="path">The path to the detector data files.</param>
		public VisageFeaturesDetector(String path);


		/// <summary>
		/// Destructor.
		/// </summary>
		public ~VisageFeaturesDetector();


		/// <summary>
		/// Initialise the feature detector. 
		///
		/// The path to the folder "bdtsdata" (located in Samples/data) must be passed to the initialization function, for example:
		/// \code
		/// std::string dataPath(@"..\Samples\data\bdtsdata");
		///
		/// gVisageFeaturesDetector.Initialize(dataPath.c_str());
		/// \endcode
		///
		/// The data folder must contain:
		/// - a subfolder "bdtsdata" with complete contents as provided in visage|SDK;
		/// - files: candide3.fdp, candide3.wfm, jk_300.fdp, jk_300.wfm and FaceDetector.cfg.
		///
		/// In visage|SDK, the data folder with necessary files is Samples/data. These files must be distributed with any application using VisageFeaturesDetector.
		///
		/// </summary>
		/// <param name="path">The path to the detector data files.</param>
		/// <returns>True if successful.</returns>
		public bool Initialize(String path);

		/// <summary>
		///
		/// Performs faces and facial features detection in a still image.
		/// 
		/// The algorithm detects one or more faces and their features. The results are, for each detected face, the 3D head pose, gaze direction, eye closure, the coordinates of facial feature points (e.g. chin tip, nose tip, lip corners etc.) and 3D face model fitted to the face.
		///
		/// The results are returned in form of FaceData objects. An array of FaceData objects passed to this method as output parameter should be allocated to maxFaces size. 
		/// For example:
		/// 
		/// \code
		/// dataArray = new FaceData[MAX_FACES];
		/// for (int i = 0; i < MAX_FACES; ++i)
		/// {
		///     dataArray[i] = new FaceData();
		/// }
		/// 
		/// if (gVisageFeaturesDetector.Initialized)
		/// {
		///     numFaces = gVisageFeaturesDetector.DetectFacialFeatures(frame, dataArray);
		/// }
		/// \endcode
		///
		/// After this call, n contains the number of faces actually detected. The first n members of the data array are filled with resulting data for each detected face.
		/// Please refer to the FaceData documentation for detailed description of returned parameters.
		///
		/// Following image formats are supported:
		/// - VISAGE_FORMAT.RGB: each pixel of the image is represented by three bytes representing red, green and blue channels, respectively.
		/// - VISAGE_FORMAT.RGBA: each pixel of the image is represented by four bytes representing red, green, blue and alpha (ignored) channels, respectively.
		/// - VISAGE_FORMAT.LUMINANCE: each pixel of the image is represented by one byte representing the luminance (gray level) of the image.
		/// Origin must be:
		/// - VISAGE_ORIGIN.TL: Origin is the top left pixel of the image. Pixels are ordered row-by-row starting from top left.
		///
		/// Note that the input image is internally converted to grayscale.
		///
		/// @see FaceData
		///
		/// </summary>
		/// <param name="frame">The input image.</param>
		/// <param name="output">Pointer to an array of FaceData objects in which the results will be returned.</param>
		/// <param name="minFaceScale">Scale of smallest face to be searched for, defined as decimal fraction [0.0 - 1.0] of input image size (min(width, height))</param>
		/// <param name="maxFaceScale">Scale of smallest face to be searched for, defined as decimal fraction [0.0 - 1.0] of input image size (min(width, height))</param>
		/// <param name="outputOnly2DFeatures">If set, detection time will be reduced and only featurePoints2D will be returned.</param>
		/// <returns>Number of detected faces (0 or more)</returns>
		public int DetectFacialFeatures(VSImage frame, FaceData[] output, float minFaceScale=0.1f, float maxFaceScale=1.0f, bool outputOnly2DFeatures = false);

		/// <summary>
		///
		/// Performs face detection in a still image.
		/// 
		/// The algorithm detects one or more faces. For each detected face a square facial bounding box is returned.
		/// 
		/// The results are returned in form of VsRect objects. An array of VsRect objects passed to this method as output parameter should be allocated to maxFaces size.
		/// For example:
		/// 
		/// \code
		/// boundingBoxArray = new VSRect[MAX_FACES];
		/// for (int i = 0; i < MAX_FACES; ++i)
		/// {
		///     boundingBoxArray[i] = new VSRect();
		/// }
		/// 
		/// if (gVisageFeaturesDetector.Initialized)
		/// {
		///     numFaces = gVisageFeaturesDetector.DetectFaces(frame, boundingBoxArray);
		/// }
		/// \endcode
		/// 
		/// After this call, n contains the number of detected faces. The first n members of the faces array are filled with resulting bounding boxes for each detected face.
		/// 
		/// VsImage is the image storage class similar to IplImage from OpenCV, it has the same structure and members so it can be used like IplImage. Please refer to OpenCV documentation for details of accessing IplImage data members; the basic members are the size of the image (frame->width, frame->height) and the pointer to the actual pixel data of the image (frame->imageData).
		/// 
		/// Following image formats are supported:
		/// - VISAGE_FORMAT.RGB: each pixel of the image is represented by three bytes representing red, green and blue channels, respectively.
		/// - VISAGE_FORMAT.RGBA: each pixel of the image is represented by four bytes representing red, green, blue and alpha (ignored) channels, respectively.
		/// - VISAGE_FORMAT.LUMINANCE: each pixel of the image is represented by one byte representing the luminance (gray level) of the image.
		/// Origin must be:
		/// - VISAGE_ORIGIN.TL: Origin is the top left pixel of the image. Pixels are ordered row-by-row starting from top left.
		/// 
		/// Note that the input image is internally converted to grayscale.
		/// 
		/// <param name="frame">The input image.</param>
		/// <param name="faces">Pointer to an array of VSRect objects in which the results will be returned.</param>
		/// <param name="minFaceScale">Scale of smallest face to be searched for, defined as decimal fraction [0.0 - 1.0] of input image size (min(width, height))</param>
		/// <param name="maxFaceScale">Scale of largest face to be searched for, defined as decimal fraction [0.0 - 1.0] of input image size (min(width, height))</param>
		/// <param name="useRefinementStep">If set to true, additional refinement algorithm will be used resulting with more precise facial bounding boxes and lower FPR, but higher detection time </param>
		/// <returns>Number of detected faces (0 or more)</returns>
		public int DetectFaces(VSImage frame, VSRect[] faces, float minFaceScale=0.1f, float maxFaceScale=1.0f, bool useRefinementStep=true);
		
		/// <summary>
		/// VisageFeaturesDetector status
		/// </summary>
		property bool Initialised {
			bool get() { return m_Impl->initialised; }
		}
	}
}
