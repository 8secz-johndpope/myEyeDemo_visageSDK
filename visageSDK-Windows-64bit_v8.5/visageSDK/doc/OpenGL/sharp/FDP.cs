using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace VisageCSWrapper
{
	/// <summary>
	/// Feature point.
	/// This struct represents a feature point definition. If the feature points are defined on a 3D model, each feature point maps to a vertex of the model.
	/// The following information is specified per each feature point:
	/// - vertex position (an array of 3 floats)
	/// - vertex identifier (mesh identifier and vertex index), if the feature point is defined on a 3D model
	/// - normalization factors (useful for algorithms that normalize the face, such as facial motion cloning)
	/// @see FDP
	/// </summary>
	struct FeaturePoint
	{
		/// <summary>
		/// Position.
		/// x, y, z coordinates of the feature point. If the feature point is 2-dimensional (e.g. feature points in an image)
		/// the z coordinate is ignored.
		/// </summary>
		float[] pos;


		/// <summary>
		/// Quality is returned as a value from 0 to 1, where 0 is the worst and 1 is the best quality. If -1 is returned, quality is not estimated. 
		/// </summary>
		int defined;


        /// <summary>
        /// Quality is returned as a value from 0 to 1, where 0 is the worst and 1 is the best quality. 
        /// If -1 is returned, quality is not estimated.
        /// </summary>
        float quality;


		/// <summary>
		/// Identifier of the vertex to which the vertex corresponding to the feature point belongs. This is the
		/// vertex index within the polygon mesh (surface). If
		/// the feature points are not defined on a 3D model (e.g. if they represent points in a 2D image), this is not used.
		/// </summary>
		int vert;
		int animated;
		int normalizing;
	}


	/// <summary>
	/// Feature points of a face.
	/// This class is a container for facial feature points as defined by <a href="../MPEG-4 FBA Overview.pdf">MPEG-4 FBA standard</a>, as well as
	/// some additional feature points. Feature points are identified by their group (for example, feature points of
	/// the nose constitute their own group) and index. So, for example, the tip of the chin belongs to group 2 and has
	/// index 1, so it is identified as point 2.1. The identification of all feature points is
	/// illustrated in the image below:
	/// \image html "mpeg-4_fba.png" "Facial Feature Points (FP)"
	/// \image html "half_profile_physical_2d.png" "Visible/Physical contour"
	/// Groups 2 - 11 contain feature points defined according to the
	/// MPEG-4 FBA standard,
	/// and group 12 and 14 contain additional feature points that are not part of the MPEG-4 standard (12.1, 12.5, 12.6, 12.7, 12.8, 12.9, 12.10, 12.11, 12.12), (14.1, 14.2, 14.3, 14.4).
	/// <h4>Face contour - group 13 and group 15</h4>
	/// Face contour is available in two versions: the visible contour (points 13.1 - 13.17) and the physical contour (points 15.1 - 15.17).
	/// The physical contour is the set of 16 equally spaced points running from one ear to another, following the outer edge of cheeks and the jaw. Depending on face rotation, points on the physical contour may not always be visible; however, our algorithm maps them on the 3D model of the face and estimates the invisible points. Therefore all points of the physical contour are always available, regardless of their visibility, in both 3D and 2D (featurePoints2D, featurePoints3D, featurePoints3DRelative). The 2D points are obtained by projection from the 3D points, and they may not correspond to the points on the visible contour.
	/// 
	/// The visible contour is the set of 16 equally spaced points running along the edges of the visible part of the face in the image, starting at the height of ears. Thus, in a frontal face the visible contour runs from one ear, along the jaw and chin, to the other ear; in a semi-profile face it runs from the visible ear, along the jaw and chin, then along the cheek on the far side of the face; in full profile it runs from the ear, along the jaw, then up along the mouth and nose. The points on the visible contour are obtained directly by detection/tracking, and they are available only in 2D (featurePoints2D). Please note that point 13.17 is exactly identical to point 2.1 (2.1 point exists for MPEG-4 compatibility purposes).
	/// <h4>Stability and accuracy of the contours</h4>
	/// The points on the visible contour are among the most difficult to detect/track, and their accuracy is lower than that of eyes or mouth points; they are also more noisy (jittery). The points on the physical contour are obtained from the fitted 3D model of the face. This stabilises them so there is considerably less jitter. In general, it is recommended to use the physical contour.
	///
	/// 
	/// FDP class stores feature point information. It also provides functions for reading and writing the feature point data as files,
	/// as well as certain auxiliary members that deal with normalization of feature points and their classification according to
	/// facial region.
	///
	/// The actual data for each feature point is stored in the structure FeaturePoint. One such structure is allocated for each feature point. To
	/// access a feature point, use one of the functions getFP() that access a feature point by its group and index expressed either as integer values, or as a string (e.g. "2.1").
	/// Functions getFPPos() are available as a convenience, to access the feature point coordinates directly, without first accessing the FeaturePoint structure.
	///
	/// The feature points may relate to a particular 3D model. In such a case it is interesting to know, for each feature point, to 
	/// which vertex it belongs. For this purpose, the FeaturePoint structure contains the mesh identifier
	/// and vertex index that correspond to the feature point and the FDP class provides functions to access this data.
	/// 
	/// <b><i>Left-right convention</i></b>
	///	 
	/// References to left and right in feature point definitions are given from the perspective of the face itself so "right eye" 
	/// can be imagined as "my right eye". When referring to feature points in an image, it is assumed that image is taken by camera and 
	/// not mirrored so "right eye" is on the left in the image, as shown in feature points illustration above.
	///
	///
	/// @see FeaturePoint
	/// 
	/// </summary>
	class FDP
	{
		/// <summary>
		/// Constructor.
		/// Creates an empty FDP object.
		/// </summary>
		public FDP();


		/// <summary>
		/// Copy constructor.
		/// Makes a copy of FDP object.
		/// </summary>
		/// <param name="featurePoints"></param>
		public FDP(FDP featurePoints);


		/// <summary>
		/// Read from an FDP file.
		/// The FDP file format consists of one line of text for each feature point, in the following format:
		/// \<group\>.\<index\> \<x\> \<y\> \<z\> \<mesh_index\>.\<vertex_index\>
		/// </summary>
		/// <param name="name">Filename.</param>
		/// <returns>0 - failure, 1 - success, -1 - old version, needs re-indexing.</returns>
		public int readFromFile(String name);


		/// <summary>
		/// Write feature point definitions to the FDP file, changing the FDP file name.
		/// </summary>
		/// <param name="fileName">Filename.</param>
		public void saveToFile(String fileName);


		/// <summary>
		/// Get a feature point by its group and index.
		/// </summary>
		/// <param name="group">Feature point group. Valid range is from 2 to 15.</param>
		/// <param name="n">Feature point index. Valid range is from 1 to the size of particular group. Group sizes can be obtained using groupSize().</param>
		/// <returns>Feature point.</returns>
		public FeaturePoint getFP(int group, int n);


		/// <summary>
		/// Get a feature point by its name.
		/// </summary>
		/// <param name="name">Feature point name (e.g. "7.1").</param>
		/// <returns>Feature point.</returns>
		public const FeaturePoint getFP(String name);


		/// <summary>
		/// Set a feature specified by its group and index.
		/// </summary>
		/// <param name="group">Feature point group. Valid range is from 2 to 15.</param>
		/// <param name="n">Feature point index. Valid range is from 1 to the size of particular group. Group sizes can be obtained using groupSize().</param>
		/// <param name="f">The feature point to set.</param>
		public void setFP(int group, int n, FeaturePoint f);


		/// <summary>
		/// Set a feature specified by its name.
		/// </summary>
		/// <param name="name">Feature point name (e.g. "7.1").</param>
		/// <param name="fp">The feature point to set.</param>
		public void setFP (String name, FeaturePoint fp);


		/// <summary>
		/// Get the position of a feature point specified by its group and index.
		/// </summary>
		/// <param name="group">Feature point group. Valid range is from 2 to 15.</param>
		/// <param name="n">Feature point index. Valid range is from 1 to the size of particular group. Group sizes can be obtained using groupSize().</param>
		/// <returns>Vertex position (array of 3 floating point numbers).</returns>
		public float[] getFPPos (int group, int n);


		/// <summary>
		/// Get the position of a feature point specified by its name.
		/// </summary>
		/// <param name="name">Feature point name (e.g. "7.1").</param>
		/// <returns>Vertex position (array of 3 floating point numbers).</returns>
		public float[] getFPPos (String name);


		/// <summary>
		/// Set the position of a feature point specified by its group and index.
		/// </summary>
		/// <param name="group">Feature point group. Valid range is from 2 to 15.</param>
		/// <param name="n">Feature point index. Valid range is from 1 to the size of particular group. Group sizes can be obtained using groupSize().</param>
		/// <param name="pos">Vertex position (array of 3 floating point values).</param>
		public void setFPPos (int group, int n, float[] pos);


		/// <summary>
		/// Set the position of a feature point specified by its name.
		/// </summary>
		/// <param name="name">Feature point name (e.g. "7.1").</param>
		/// <param name="pos">Vertex position (array of 3 floating point numbers).</param>
		public void setFPPos (String name, float[] pos);


		/// <summary>
		/// Set the position of a feature point specified by its group and index.
		/// </summary>
		/// <param name="group">Feature point group. Valid range is from 2 to 15.</param>
		/// <param name="n">Feature point index. Valid range is from 1 to the size of particular group. Group sizes can be obtained using groupSize().</param>
		/// <param name="x">Vertex x position.</param>
		/// <param name="y">Vertex y position.</param>
		/// <param name="z">Vertex z position.</param>
		public void setFPPos (int group, int n, float x, float y, float z);


		/// <summary>
		/// Set the position of a feature point specified by its name.
		/// </summary>
		/// <param name="name">Feature point name (e.g. "7.1").</param>
		/// <param name="x">Vertex x position.</param>
		/// <param name="y">Vertex y position.</param>
		/// <param name="z">Vertex z position.</param>
		public void setFPPos (String name, float x, float y, float z);


		/// <summary>
		/// Get the quality of a feature point specified by its group and index.
		/// </summary>
		/// <param name="group">Feature point group. Valid range is from 2 to 15.</param>
		/// <param name="n">Feature point index. Valid range is from 1 to the size of particular group. Group sizes can be obtained using groupSize().</param>
		/// <returns>quality value.</returns>
		public float getFPQuality(int group, int n);


		/// <summary>
		/// Get the quality of a feature point specified by its name.
		/// </summary>
		/// <param name="name">Feature point name (e.g. "7.1").</param>
		/// <returns>quality value.</returns>
		public float getFPQuality(String name);


		/// <summary>
		/// Set the quality of a feature point specified by its group and index.
		/// </summary>
		/// <param name="group">Feature point group. Valid range is from 2 to 15.</param>
		/// <param name="n">Feature point index. Valid range is from 1 to the size of particular group. Group sizes can be obtained using groupSize().</param>
		/// <param name="quality">quality value.</param>
		public void setFPQuality(int group, int n, float quality);


		/// <summary>
		/// Set the position of a feature point specified by its name.
		/// </summary>
		/// <param name="name">Feature point name (e.g. "7.1").</param>
		/// <param name="quality">Vertex position (array of 3 floating point numbers).</param>
		public void setFPQuality(String name, float quality);


		/// <summary>
		/// Resets all feature points.
		///
		/// The value of all feature points is set to "undefined".
		/// </summary>
		public void reset();


		/// <summary>
		/// Returns true if the feature point is defined.
		/// </summary>
		/// <param name="group">Feature point group. Valid range is from 2 to 15.</param>
		/// <param name="n">Feature point index. Valid range is from 1 to the size of particular group. Group sizes can be obtained using groupSize().</param>
		/// <returns>True if the feature point is defined, false otherwise.</returns>
		public bool FPIsDefined(int group, int n);


		/// <summary>
		/// Returns true if the feature point is defined.
		/// </summary>
		/// <param name="name">Feature point name (e.g. "7.1").</param>
		/// <returns>True if the feature point is defined, false otherwise.</returns>
		public bool FPIsDefined(String name);


		/// <summary>
		/// Vertices that are part of the upper lip region.
		/// </summary>
		public FeaturePoint[] ul;


		/// <summary>
		/// Vertices that are part of the lower lip region. 
		/// </summary>
		public FeaturePoint[] ll;


		/// <summary>
		/// Get feature point group and index from its name.
		/// </summary>
		/// <param name="name">Feature point name (e.g. "7.1").</param>
		/// <param name="group">Returned feature point group.</param>
		/// <param name="n">Returned feature point index.</param>
		public static void parseFPName (String name, ref int group, ref int n);


		/// <summary>
		/// Get feature point name from group and index.
		/// </summary>
		/// <param name="group">Feature point group. Valid range is from 2 to 15.</param>
		/// <param name="n">Feature point index. Valid range is from 1 to the size of particular group. Group sizes can be obtained using groupSize().</param>
		/// <returns>Feature point name.</returns>
		public static String getFPName (int group, int n);


		/// <summary>
		/// Returns true if specified feature point identifier is valid.
		/// </summary>
		/// <param name="group">Feature point group. Valid range is from 2 to 15.</param>
		/// <param name="n">Feature point index. Valid range is from 1 to the size of particular group. Group sizes can be obtained using groupSize().</param>
		/// <returns>True if specified feature point identifier is valid, false otherwise.</returns>
		public static bool FPIsValid(int group, int n);


		/// <summary>
		/// Returns true if specified feature point identifier is valid.
		/// </summary>
		/// <param name="name">Feature point name (e.g. "7.1").</param>
		/// <returns>True if specified feature point identifier is valid, false otherwise.</returns>
		public static bool FPIsValid (String name);


		/// <summary>
		/// Get the size of the specified feature point group.
		/// Valid range for group is from 2 to 15.
		/// </summary>
		/// <param name="group">Feature point group. Valid range is from 2 to 15.</param>
		/// <returns>Size of the specified feature point group.</returns>
		public static int GroupSize (int group);


		/// Index of the first feature point group.
		public static int FP_START_GROUP_INDEX = 2;


		/// Index of the last feature point group.
		public static int FP_END_GROUP_INDEX = 15;


		/// Number of groups.
		public static int FP_NUMBER_OF_GROUPS = 14;
	}
}
