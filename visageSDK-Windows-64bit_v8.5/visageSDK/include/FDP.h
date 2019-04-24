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


#ifndef __FDP_h__
#define __FDP_h__

// C++
#include <string>

#include "Base.h"

#ifdef VISAGE_STATIC
	#define VISAGE_DECLSPEC
#else

	#ifdef VISAGE_EXPORTS
		#define VISAGE_DECLSPEC __declspec(dllexport)
	#else
		#define VISAGE_DECLSPEC __declspec(dllimport)
	#endif

#endif

namespace VisageSDK
{

/**
* Feature point.
* This struct represents a feature point definition. If the feature points are defined on a 3D model, each feature point maps to a vertex of the model.
* The following information is specified per each feature point:
* - vertex position (an array of 3 floats)
* - vertex identifier (mesh identifier and vertex index), if the feature point is defined on a 3D model
* - normalization factors (useful for algorithms that normalize the face, such as facial motion cloning)
*
* @see FDP
*
*/
struct VISAGE_DECLSPEC FeaturePoint
{

public:

	/**
	* Position.
	* x, y, z coordinates of the feature point. If the feature point is 2-dimensional (e.g. feature points in an image)
	* the z coordinate is ignored.
	*/
	float pos[3];

	/**
	* Indicator whether the feature point is defined. 0 means it is not defined and its values should not be used. 1 means it is defined.
	*/
	int defined;

	/**
	* Indicator whether the feature point is detected or estimated after 3D model fitting. If the point is defined, 1 means the point is detected and 0 means the point is estimated.
	*/
	int detected;

	/**
	* Quality is returned as a value from 0 to 1, where 0 is the worst and 1 is the best quality. If -1 is returned, quality is not estimated.
	*/
	float quality;

	/**
	* Identifier of the vertex to which the vertex corresponding to the feature point belongs. This is the
	* vertex index within the polygon mesh (surface) identified by @ref surf. If
	* the feature points are not defined on a 3D model (e.g. if they represent points in a 2D image), this is not used.
	*/
	int vert;

	/**
	* Identifier of the polygon mesh (surface) to which the vertex corresponding to the feature point belongs. If
	* the feature points are not defined on a 3D model (e.g. if they represent points in a 2D image), this is not used.
	*/
	std::string surf;

	int animated;
	int normalizing;

	FeaturePoint()
	{
		quality = -1;
		defined = 0;
		detected = 0;
		surf = "";
		vert = -1;
		animated = normalizing = 0;
		pos[0] = 0.0f;
		pos[1] = 0.0f;
		pos[2] = 0.0f;
	}

	FeaturePoint( const FeaturePoint& fp )
	{
		quality = fp.quality;
		pos[0] = fp.pos[0];
		pos[1] = fp.pos[1];
		pos[2] = fp.pos[2];
		defined = fp.defined;
		detected = fp.detected;
		animated = fp.animated;
		normalizing = fp.normalizing;
		surf = fp.surf;
		vert = fp.vert;
	}

	FeaturePoint& operator=( const FeaturePoint& fp )
	{
		if( this == &fp )
			return *this;

		pos[0] = fp.pos[0];
		pos[1] = fp.pos[1];
		pos[2] = fp.pos[2];
		quality = fp.quality;
		defined = fp.defined;
		detected = fp.detected;
		animated = fp.animated;
		normalizing = fp.normalizing;
		surf = fp.surf;
		vert = fp.vert;

		return *this;
	}
};

/**
* Feature points of a face.
* This class is a container for facial feature points as defined by <a href="../MPEG-4 FBA Overview.pdf">MPEG-4 FBA standard</a>, as well as
* some additional feature points. Feature points are identified by their group (for example, feature points of
* the nose constitute their own group) and index. So, for example, the tip of the chin belongs to group 2 and has
* index 1, so it is identified as point 2.1. The identification of all feature points is
* illustrated in the image below:
* \image html "mpeg-4_fba.png" "Facial Feature Points (FP)"
* \image html "half_profile_physical_2d.png" "Visible/Physical contour"
* Groups 2 - 11 contain feature points defined according to the
* MPEG-4 FBA standard,
* and groups 12 and 14 contain additional feature points that are not part of the MPEG-4 standard (12.1, 12.5, 12.6, 12.7, 12.8, 12.9, 12.10, 12.11, 12.12, 14.1, 14.2, 14.3, 14.4, 14.21, 14.22, 14.23, 14.24, 14.25).
* <h4>Face contour - group 13 and group 15</h4>
*
* Face contour is available in two versions: the visible contour (points 13.1 - 13.17) and the physical contour (points 15.1 - 15.17).
* The physical contour is the set of 16 equally spaced points running from one ear to another, following the outer edge of cheeks and the jaw. 
* Depending on face rotation, points on the physical contour may not always be visible; however, our algorithm maps them on the 3D model of the face and estimates the invisible points. 
* Therefore all points of the physical contour are always available, regardless of their visibility, in both 3D and 2D 
* (featurePoints2D, featurePoints3D, featurePoints3DRelative). The 2D points are obtained by projection from the 3D points, 
* and they may not correspond to the points on the visible contour.
*
* The visible contour is the set of 16 equally spaced points running along the edges of the visible part of the face in the image, 
* starting at the height of ears. Thus, in a frontal face the visible contour runs from one ear, along the jaw and chin, to the other ear; 
* in a semi-profile face it runs from the visible ear, along the jaw and chin, then along the cheek on the far side of the face; 
* in full profile it runs from the ear, along the jaw, then up along the mouth and nose. 
* The points on the visible contour are obtained directly by detection/tracking, and they are available only in 2D (featurePoints2D). 
* Please note that point 13.17 is exactly identical to point 2.1 (2.1 point exists for MPEG-4 compatibility purposes).
*
* <h4>Stability and accuracy of the contours</h4>
*
* The points on the visible contour are among the most difficult to detect/track, and their accuracy is lower than that of eyes or mouth points; 
* they are also more noisy (jittery). The points on the physical contour are obtained from the fitted 3D model of the face. 
* This stabilises them so there is considerably less jitter. In general, it is recommended to use the physical contour.
*
*
* FDP class stores feature point information. It also provides functions for reading and writing the feature point data as files.
*
* The actual data for each feature point is stored in the structure FeaturePoint. One such structure is allocated for each feature point. To
* access a feature point, use one of the functions getFP() that access a feature point by its group and index expressed either as integer values, or as a string (e.g. "2.1").
* Functions getFPPos() are available as a convenience, to access the feature point coordinates directly, without first accessing the FeaturePoint structure.
*
* The feature points may relate to a particular 3D model. In such a case it is interesting to know, for each feature point, to
* which vertex it belongs. For this purpose, the FeaturePoint structure contains the mesh identifier
* and vertex index that correspond to the feature point and the FDP class provides functions to access this data.
*
* <b><i>Left-right convention</i></b>
*
* References to left and right in feature point definitions are given from the perspective of the face itself so "right eye"
* can be imagined as "my right eye". When refering to feature points in an image, it is assumed that image is taken by camera and
* not mirrored so "right eye" is on the left in the image, as shown in feature points illustration above.
*
*
* @see FeaturePoint
*/
class VISAGE_DECLSPEC FDP
{
public:

	/**
	* Constructor.
	* Creates an empty FDP object.
	*/
	FDP();

	/**
	* Copy constructor.
	* Makes a copy of FDP object.
	*/
	FDP( const FDP& featurePoints );

	/**
	* Constructor.
	* @param fn FDP file.
	*/
	FDP( const char *fn );

	/*
	* Destructor.
	*/
	~FDP();

	/*
	* Assignment operator.
	*/
	FDP& operator=( const FDP& featurePoints );

	/**
	* Read from an FDP file.
	* The FDP file format consists of one line of text for each feature point, in the following format:
	* (group).(index) (x) (y) (z) (mesh_index).(vertex_index)
	* @param name Filename.
	* @return 0 - failure, 1 - success, -1 - old version, needs reindexing.
	*/
	int readFromFile(const char *name);

	/**
	* Create a new (empty) FDP file.
	* @param name Filename.
	*/
	void createNewFile(const char *name);

	/**
	* Write feature point definitions to the current FDP file.
	* File name is the one currently used.
	*/
	void saveToFile() const;

	/**
	* Write feature point definitions to the FDP file, changing the FDP file name.
	* @param fileName Filename.
	*/
	void saveToFile(const char *fileName);

	/**
	* Get a feature point by its group and index.
	* @param group Feature point group. Valid range is from 2 to 15.
	* @param n Feature point index. Valid range is from 1 to the size of particular group. Group sizes can be obtained using groupSize().
	* @return Feature point.
	*/
	const FeaturePoint& getFP(int group, int n) const;

	/**
	* Get a feature point by its name.
	* @param name Feature point name (e.g. "7.1").
	* @return Feature point.
	*/
	const FeaturePoint& getFP(const char *name) const;

	/**
	* Set a feature specified by its group and index.
	* @param group Feature point group. Valid range is from 2 to 15.
	* @param n Feature point index. Valid range is from 1 to the size of particular group. Group sizes can be obtained using groupSize().

	* @param f The feature point to set.
	*/
	void setFP( int group, int n, const FeaturePoint& f );

	/**
	* Set a feature specified by its name.
	* @param name Feature point name (e.g. "7.1").
	* @param f The feature point to set.
	*/
	void setFP( const char* name, const FeaturePoint& f );

	/**
	* Get the position of a feature point specified by its group and index.
	* @param group Feature point group. Valid range is from 2 to 15.
	* @param n Feature point index. Valid range is from 1 to the size of particular group. Group sizes can be obtained using groupSize().

	* @return Vertex position (array of 3 floating point numbers).
	*/
	const float* getFPPos(int group, int n) const;

	/**
	* Get the position of a feature point specified by its name.
	* @param name Feature point name (e.g. "7.1").
	* @return Vertex position (array of 3 floating point numbers).
	*/
	const float* getFPPos(const char *name) const;

	/**
	* Get the quality of a feature point specified by its group and index.
	* @param group Feature point group. Valid range is from 2 to 15.
	* @param n Feature point index. Valid range is from 1 to the size of particular group. Group sizes can be obtained using groupSize().

	* @return quality value.
	*/
	float getFPQuality(int group, int n) const;

	/**
	* Get the quality of a feature point specified by its name.
	* @param name Feature point name (e.g. "7.1").
	* @return quality value.
	*/
	float getFPQuality(const char *name) const;

	/**
	* Set the quality of a feature point specified by its group and index.
	* @param group Feature point group. Valid range is from 2 to 15.
	* @param n Feature point index. Valid range is from 1 to the size of particular group. Group sizes can be obtained using groupSize().
	* @param quality quality value.
	*/
	void setFPQuality(int group, int n, const float quality);

	/**
	* Set the position of a feature point specified by its name.
	* @param name Feature point name (e.g. "7.1").
	* @param quality quality value.
	*/
	void setFPQuality(const char *name, const float quality);

	/**
	* Set the position of a feature point specified by its group and index.
	* @param group Feature point group. Valid range is from 2 to 15.
	* @param n Feature point index. Valid range is from 1 to the size of particular group. Group sizes can be obtained using groupSize().
	* @param pos Vertex position (array of 3 floating point values).
	*/
	void setFPPos(int group, int n, const float *pos);

	/**
	* Set the position of a feature point specified by its name.
	* @param name Feature point name (e.g. "7.1").
	* @param pos Vertex position (array of 3 floating point values).
	*/
	void setFPPos(const char *name, const float *pos);

	/**
	* Set the position of a feature point specified by its group and index.
	* @param group Feature point group. Valid range is from 2 to 15.
	* @param n Feature point index. Valid range is from 1 to the size of particular group. Group sizes can be obtained using groupSize().
	* @param x Vertex x position.
	* @param y Vertex y position.
	* @param z Vertex z position.
	*/
	void setFPPos(int group, int n, float x, float y, float z);

	/**
	* Set the position of a feature point specified by its name.
	* @param name Feature point name (e.g. "7.1").
	* @param x Vertex x position.
	* @param y Vertex y position.
	* @param z Vertex z position.
	*/
	void setFPPos(const char *name, float x, float y, float z);

	/**
	* Get the mesh and vertex mapping for a feature point specified by its group and index.
	* @param group Feature point group. Valid range is from 2 to 15.
	* @param n Feature point index. Valid range is from 1 to the size of particular group. Group sizes can be obtained using groupSize().
	* @param surf Returned mesh identifier.
	* @param vert Returned vertex index.
	* @return True if feature point is defined, false if not.
	*/
	bool getFPSurfVert( int group, int n, std::string& surf, int& vert ) const;

	/**
	* Get the mesh and vertex mapping for a feature point specified by its name.
	* @param name Feature point name (e.g. "7.1").
	* @param surf Returned mesh identifier.
	* @param vert Returned vertex index.
	* @return True if feature point is defined, false if not.
	*/
	bool getFPSurfVert( const char* name, std::string& surf, int& vert ) const;

	/**
	* Assign a feature point, specified by its group and index, to a specific vertex.
	* @param group Feature point group. Valid range is from 2 to 15.
	* @param n Feature point index. Valid range is from 1 to the size of particular group. Group sizes can be obtained using groupSize().
	* @param surf Mesh id.
	* @param vert Vertex index.
	*/
	void setFPSurfVert(int group, int n, const std::string& surf, int vert);

	/**
	* Assign a feature point, specified by its name, to a specific vertex.
	* @param name Feature point name (e.g. "7.1").
	* @param surf Mesh id.
	* @param vert Vertex index.
	*/
	void setFPSurfVert(const char *name, const std::string& surf, int vert);

	/**
	* Resets all feature points.
	*
	* The value of all feature points is set to "undefined".
	*/
	void reset();

	/**
	* Returns true if the feature point is defined.
	*
	* @param group Feature point group. Valid range is from 2 to 15.
	* @param n Feature point index. Valid range is from 1 to the size of particular group. Group sizes can be obtained using groupSize().

	* @return True if the feature point is defined, false otherwise.
	*/
	bool FPIsDefined( int group, int n ) const;

	/**
	* Returns true if the feature point is defined.
	*
	* @param name Feature point name (e.g. "7.1").

	* @return True if the feature point is defined, false otherwise.
	*/
	bool FPIsDefined( const std::string& name ) const;

	/**
	* Returns true if the feature point is detected.
	*
	* @param group Feature point group. Valid range is from 2 to 15.
	* @param n Feature point index. Valid range is from 1 to the size of particular group. Group sizes can be obtained using groupSize().

	* @return True if the feature point is detected, false otherwise.
	*/
	bool FPIsDetected(int group, int n) const;

	/**
	* Returns true if the feature point is detected.
	*
	* @param name Feature point name (e.g. "7.1").

	* @return True if the feature point is detected, false otherwise.
	*/
	bool FPIsDetected(const std::string& name) const;


	/*
	* Returns true if the face is normalized.
	*/
	bool isNormalized() const;

	/*
	* Returns true if the face is initialized.
	*/
	bool isInitialized() const;

	/*
	* Index of the first feature point group. 
	* @return 2
	*/
	static const int FP_START_GROUP_INDEX = 2;

	/*
	* Index of the last feature point group. 
	* @return 15
	*/
	static const int FP_END_GROUP_INDEX = 15;

	/*
	* Number of groups.
	*/
	static const int FP_NUMBER_OF_GROUPS = 14;

private:

	/*
	* FDP file name.
	*/
	char mFilename[192];

	/**
	* Some characteristic distances between feature points (defined in <a href="../MPEG-4 FBA Overview.pdf">MPEG-4 FBA standard</a>).
	*/
	float MNS0,ENS0,ES0,MW0,IRISD0;

	/**
	* True if the face is normalized, false otherwise.
	*/
	bool normalized;

	/**
	* Feature points.
	*/
	FeaturePoint *fp[FP_END_GROUP_INDEX+1];

	/*
	* Print feature point definitions to console.
	*/
	void print() const;

	/**
	* True if FDP is initialized, false otherwise.
	*/
	bool initialized;


public:

	/**
	* Get feature point group and index from its name.
	*
	* @param name Feature point name (e.g. "7.1").
	* @param group Returned feature point group.
	* @param n Returned feature point index.
	*/
	static void parseFPName( const std::string& name, int& group, int& n );

	/**
	* Get feature point name from group and index.
	*
	* @param group Feature point group. Valid range is from 2 to 15.
	* @param n Feature point index. Valid range is from 1 to the size of particular group. Group sizes can be obtained using groupSize().
	*
	* @return Feature point name.
	*/
	static std::string getFPName( int group, int n );

	/**
	* Returns true if specified feature point identifier is valid.
	*
	* @param group Feature point group. Valid range is from 2 to 15.
	* @param n Feature point index. Valid range is from 1 to the size of particular group. Group sizes can be obtained using groupSize().
	*
	* @return True if specified feature point identifier is valid, false otherwise.
	*/
	static bool FPIsValid( int group, int n );

	/**
	* Returns true if specified feature point identifier is valid.
	*
	* @param name Feature point name (e.g. "7.1").
	*
	* @return True if specified feature point identifier is valid, false otherwise.
	*/
	static bool FPIsValid( const std::string& name );

	enum pointTypes { PT_LEFT = 1, PT_CENTRAL = 2, PT_RIGHT = 3, PT_UNDEFINED = -1 };

	static pointTypes getPointType(int group, int index);

private:

	/**
	* Sizes of feature point groups.
	*/
	static const int groupSizes[FP_NUMBER_OF_GROUPS];

public:

	/**
	* Get the size of the specified feature point group.
	* Valid range for group is from 2 to 15.
	*
	* @param group Feature point group. Valid range is from 2 to 15.
	*
	* @return Size of the specified feature point group.
	*/
	static int groupSize( int group );


	/**
	* Get the mirror point index for the point defined by given group and index.
	*
	* @param group Feature point group. Valid range is from 2 to 15.
	* @param index Feature point index. Valid range is from 1 to the size of particular group. Group sizes can be obtained using groupSize().
	*
	* @return Index of the mirror point (group is the same).
	*/
	static int getMirrorPointIndex(int group, int index);
};

}

#endif // __FDP_h__
