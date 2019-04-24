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



#ifndef _FACEANALYSER_H
#define _FACEANALYSER_H


#ifdef VISAGE_STATIC
	#define VISAGE_ANALYSER_DECLSPEC
#else

	#ifdef VISAGE_ANALYSER_EXPORTS
		#define VISAGE_ANALYSER_DECLSPEC __declspec(dllexport)
	#else
		#define VISAGE_ANALYSER_DECLSPEC __declspec(dllimport)
	#endif

#endif


#include "FaceData.h"

namespace VisageSDK
{

class VFASingleInstance;
class VFAConcatenatedInstance;
class VisageNNRunner;
class VisageNNModel;

/** VFA_FLAGS are used to describe whether age, gender and/or emotion data have been loaded succesfully.
*
* Flags are returned as bitwise combination in function init().
*/
	enum VFA_FLAGS {
		VFA_AGE = 1,
		VFA_GENDER = 2,
		VFA_EMOTION = 4
	};
	
	//face normalization flags
	enum FACE_NORM_FLAGS {
		VS_NORM_POSE = 0x00,
		VS_NORM_SU =   0x01,
		VS_NORM_AU =   0x02
	};

	/** VisageFaceAnalyser contains face analysis algorithms capable of estimating age, gender and emotion from frontal facial images (yaw between -20 and 20 degrees).
	*
	* Before using VisageFaceAnalyser, it must be initialized using the function init(). After the initialization the following types of analysis can be used:
	*
	*<table>
	* <tr><td width="250"><b>ANALYSIS TYPE</b></td><td width="200"><b>FUNCTION</b></td></tr>
	* <tr><td>age estimation</td><td>estimateAge()</td></tr>
	* <tr><td>gender estimation</td><td>estimateGender()</td></tr>
	* <tr><td>emotion estimation</td><td>estimateEmotion()</td></tr>
	* </table>
	*
	* VisageFaceAnalyser uses VsImage structure as a container for the input image. VsImage is the image storage class similar to IplImage from OpenCV; it has the same structure and members so it can be used like IplImage.  
	* Please refer to OpenCV documentation for details of accessing IplImage data members; the basic members are the size of the image (frame->width, frame->height) and the pointer to the actual pixel data of the 
	* image (frame->imageData). Method estimateAge() expects input image to be in the RGB format with bit depth set to 8; estimateEmotion() and estimateGender() expect RGB, RGBA or grayscale image. 
	* \n\n
	* There are two ways to create input image:
	* 1. Create IplImage and set image data using OpenCV. Image should be converted to VsImage before passing it to VisageFaceAnalyser. Please note that 
	* OpenCV loads image (imread, cvLoadImage) by default in BGR format. In that case, the input image should be converted before passing it to VisageFaceAnalyser.

	* 2. visage|SDK provides methods similar to the ones from OpenCV which are used to create input image:
	* \code
	* VsImage *inputImage;
	* inputImage = vsCreateImageHeader(vsSize(frameWidth, frameHeight), 8, 3);
	* vsSetData(image, (void*)frameData, image->widthStep);
	* vsReleaseImageHeader(&inputImage);
	* \endcode
	* The memory should be released as follows:
	* \code
	* vsReleaseImageHeader(&inputImage);
	* \endcode
	*
	*/
	class VISAGE_ANALYSER_DECLSPEC VisageFaceAnalyser
{

private:
	int m_dataLoaded;
	static const int m_eNumOfInstances = 7;
	static const int m_numOfGroups = 5;

	VFASingleInstance *m_ageStage1;
	VFASingleInstance **m_ageGroups;

	VFASingleInstance *m_gender;
	VFAConcatenatedInstance *m_emotion;

	VisageNNRunner* m_ageRunner;
	VisageNNModel* m_ageModel;
	

public:
	/** Constructor.
	*/
	VisageFaceAnalyser();
	/** Destructor.
	*/
	~VisageFaceAnalyser();

	/** Initialise VisageFaceAnalyser. This function must be called before using VisageFaceAnalyser by passing it a path to the folder containing the VisageFaceAnalyser data files. Within the Visage|SDK package, this folder is	
	 Samples/data/bdtsdata/LBF/vfadata. When implementing applications, developers may copy this folder as part of their application. The path is given relative to the current working directory at the time of calling the init() function. The VisageFaceAnalyser data folder contains the following subfolders corresponding to the various types of analysis that can be performed:
	*
	<ul>
		<li>ad: age estimation</li>
		<li>gd: gender estimation</li>
		<li>ed: emotion estimation</li>
	</ul>
	*
	* Note that it is not neccessary for all subfolders to be present, only the ones needed by the types of analysis that will be performed. For example, if only gender estimation will be performed, then only the gd folder is required.
	*
	* The return value of the init() function indicates which types of analysis have been successfully initialised. Subsqeuent attempt to use a type of analysis that was not initialized shall fail; for example, if only gd folder was present at initialization, attempt to use estimateEmotion() shall fail.
	* 
	* The return value is a bitwise combination of flags indicating which types of analysis are successfully initialized.
	*
	* @param dataPath relative path from the working directory to the directory that contains VisageFaceAnalyser data files.
	* @return The return value of 0 indicates that no data files are found and no analysis will be possible. In such case, the placement of data files should be verified. A non-zero value indicates that one or more types of analysis are successfully initialized. The specific types of analysis that have been initialized are indicated by a bitwise combination of the following flags: VFA_GENDER, VFA_GENDER, VFA_EMOTION.
	*/
	int init(const char* dataPath);	

	/** Estimates age from a facial image.
	*
	* The function returns estimated age. Prior to using this function, it is necessary to process the facial image or video frame using VisageTracker or VisageFeaturesDetector and pass the image and obtained data to this function. 
	* * \if WIN_DOXY
	* An example of use, estimate age on the first face detected in an image file:
	* \code
	* VsImage *frame = 0;
	* frame = cvLoadImage(fileName);
	* VisageFaceAnalyser * m_VFA= new VisageFaceAnalyser();
	* const char *dataPath="./bdtsdata/LBF/vfadata";
	* int is_initialized = m_VFA->init(dataPath);
	* 
	* if((is_initialized & VFA_AGE) == VFA_AGE)
	* {
	*	int maxFaces = 100;
	*	FaceData* data = new FaceData[maxFaces];
	*	CFaceDetectorDoc * const CFDD = new CFaceDetectorDoc();
	*	
	*	int n_faces = CFDD->m_Detector->detectFacialFeatures(frame, data, maxFaces);
	*
	*	if (n_faces > 0)
	*		float detectionSuccessful = m_VFA->estimateAge(frame, &data[i]);
	* }
	* \endcode
	*\endif
	*
	* @param frame the input RGB image.
	* @param facedata needs to contain FDP data. FDP data remains unchanged.
	* @return returns estimated age and -1 if it failed.
	*
	* \deprecated Will be removed in next version (8.5).
	*/
	float estimateAge(VsImage* frame, FaceData *facedata);

	/** Estimates gender from a facial image.
	*
	* The function returns 1 if estimated gender is male and 0 if it is a female. Prior to using this function, it is necessary to process the facial image or video frame using VisageTracker or VisageFeaturesDetector and pass the frame and obtained data to this function. 
	* * \if WIN_DOXY
	* An example of use, estimate gender on the first face detected in an image file:
	* \code
	* VsImage *frame = 0;
	* frame = (VsImage*)cvLoadImage(fileName);
	* VisageFaceAnalyser * m_VFA= new VisageFaceAnalyser();
	* const char *dataPath="./bdtsdata/LBF/vfadata";
	* int is_initialized = m_VFA->init(dataPath);
	* 
	* if((is_initialized & VFA_GENDER) == VFA_GENDER)
	* {
	*	int maxFaces = 100;
	*	FaceData* data = new FaceData[maxFaces];
	*	CFaceDetectorDoc * const CFDD = new CFaceDetectorDoc();
	*	
	*	int n_faces = CFDD->m_Detector->detectFacialFeatures(frame, data, maxFaces);
	*
	*	if (n_faces > 0)
	*		int detectionSuccessful = m_VFA->estimateGender(frame, &data[i]);
	* }
	* \endcode
	*\endif
	*
	* @param frame the input RGB, RGBA or grayscale image.
	* @param facedata needs to contain FDP data. FDP data remains unchanged.
	* @return returns 0 if estimated gender is female, 1 if it is a male and -1 if it failed.
	*
	* \deprecated Will be removed in next version (8.5).
	*/
	int estimateGender(VsImage* frame, FaceData *facedata);

	/** @cond */
	/** Estimates age and/or gender from a facial image.
	*
	* The function returns estimated age in years and/or returns 1 if estimated gender is male and 0 if it is a female. Prior to using this function, it is necessary to process the facial image or video frame using VisageTracker or VisageFeaturesDetector and pass the frame and obtained data to this function.
	* Depending on age and gender parameters, age estimation, gender estimation, both or neither is performed.
	* \if WIN_DOXY
	* An example of use, estimate gender on the first face detected in an image file:
	* \code
	* int gender = -1;
	* VsImage *frame = 0;
	* frame = (VsImage*)cvLoadImage(fileName);
	* VisageFaceAnalyser * m_VFA= new VisageFaceAnalyser();
	* const char *dataPath="./bdtsdata/LBF/vfadata";
	* int is_initialized = m_VFA->init(dataPath);
	*
	* if((is_initialized & VFA_GENDER) == VFA_GENDER)
	* {
	*	int maxFaces = 100;
	*	FaceData* data = new FaceData[maxFaces];
	*	CFaceDetectorDoc * const CFDD = new CFaceDetectorDoc();
	*
	*	int n_faces = CFDD->m_Detector->detectFacialFeatures(frame, data, maxFaces);
	* 
	*	if (n_faces > 0)
	*		m_VFA->estimateAgeAndGender(frame, &data[i], NULL, &gender);
	* }
	* \endcode
	* \endif
	*
	* @param frame the input RGB image.
	* @param facedata needs to contain FDP data. FDP data remains unchanged.
	* @param age if not NULL filled with estimated age or -1 if estimation failed.
	* @param gender if not NULL filled with estimated gender as follows: 0 if female, 1 if male or -1 if estimation failed.
	*/
	void estimateAgeAndGender(VsImage* frame, FaceData* facedata, float* age = NULL, int* gender = NULL);
	/** @endcond */

	/** Estimates emotion from a facial image.
	*  
	* The function returns estimated probabilities for basic emotions. Prior to using this function, it is necessary to process the facial image or video frame using VisageTracker or VisageFeaturesDetector and pass the frame and obtained data to this function. 
	* \if WIN_DOXY
	* An example of use, estimate emotions on the first face detected in an image file:
	* \code
	* VsImage *frame = 0;
	* frame = (VsImage*)cvLoadImage(fileName);
	* VisageFaceAnalyser * m_VFA= new VisageFaceAnalyser();
	* const char *dataPath="./bdtsdata/LBF/vfadata";
	* int is_initialized = m_VFA->init(dataPath);
	* 
	* if((is_initialized & VFA_EMOTION) == VFA_EMOTION)
	* {
	*	int maxFaces = 100;
	*	FaceData* data = new FaceData[maxFaces];
	*	CFaceDetectorDoc * const CFDD = new CFaceDetectorDoc();
	*	
	*	int n_faces = CFDD->m_Detector->detectFacialFeatures(frame, data, maxFaces);
	*
	*	if (n_faces > 0)
	*	{
	*		float prob_estimates[7];
	*		int detectionSuccessful = m_VFA->estimateEmotion(frame, &data[i], prob_estimates);
	*	}
	* }
	* \endcode
	* \endif
	* 
	* @param frame the input RGB, RGBA or grayscale image.
	* @param facedata needs to contain FDP data. FDP data remains unchanged.
	* @param prob_estimates is an array of 7 doubles. If successful, the function will fill this array with estimated probabilities for emotions in this order: anger, disgust,
	* fear, happiness, sadness, surprise and neutral. Each probability will have a value between 0 and 1. Sum of probabilities does not have to be 1.
	* @return returns 1 if estimation was successful, 0 otherwise.
	*
	* See also: Facedata, VisageTracker, VisageDetector
	*/
	int estimateEmotion(VsImage* frame, FaceData *facedata, float* prob_estimates);

	/* Get normalized face image.
	*
	* This function returns normalized face image with corresponding feature points.
	* Size of the normalized face in the image is such that interpupillary distance is approximately quarter of the image width.
	* 
	* Face will be normalized to a varying degree depending on normalization type. For example rotated 
	* face with open mouth will only have its pose straightened with normalization type VS_NORM_POSE while
	* with addition of VS_NORM_AU normalized face will also have closed mouth.
	*
	* Note that the face will always have its pose straightened.
	* 
	* Types of normalization are:
	*   - VS_NORM_POSE - face translation and rotation are set to zero thereby normalizing the pose
	*   - VS_NORM_SU - parameters describing the face shape (shape units) are set to zero thereby normalizing the face shape
	*   - VS_NORM_AU - parameters describing facial movements (action units) are set to zero, for example open mouth will be closed
	*
	* Different types of normalization can be combined with "|" operator, for example VS_NORM_POSE | VS_NORM_SU.
	*
	* \b Note: To use this functionality a FaceTrack or FaceDetect license is required.
	*
	* @param frame image containing the face to be normalized, must be grayscale
	* @param normFace image containing the normalized face; it must be allocated before calling the function; face size will depend on this image size
	* @param face_data FaceData structure containing the information about the face that will be normalized
	* @param normFDP features points that correspond to the normalized face; coordinates are normalized to 0-1 range
	* @param norm_type normalization type, a binary combination of VS_NORM_POSE - normalizes pose, VS_NORM_SU - normalizes shape units and VS_NORM_AU - normalizes action units
	* @param dataPath path to the folder where Face Detector.cfg is located, default values is ""
	*/
	void getNormalizedFaceImage (
		VsImage* frame,
		VsImage* normFace,
		FaceData* face_data,
		FDP* normFDP,
		int norm_type = VS_NORM_POSE,
		const char* dataPath = ""
	);

};
}

#endif //_FACEANALYSER_H
