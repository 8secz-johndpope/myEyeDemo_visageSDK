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

#ifndef VSDK_VISAGEFACERECOGNITION_H
#define VSDK_VISAGEFACERECOGNITION_H

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
	class VisageNNModel;
	class VisageNNRunner;

	/** VisageFaceRecognition class contains a face recognition algorithm capable of measuring similarity between human 
	* faces and recognizing a person's identity from frontal facial images (yaw angle approximately from -20 to 20 degrees). 
	*
	* Similarity between two faces is calculated as difference between their face descriptors and returned as a value between 0 (no similarity) and 1 (maximum similarity).
	*
	* The face descriptor is a condensed representation of a face. It is an array of short values. The dimension of the array is obtained using getDescriptorSize() function, from now on in the text referred to as 
	* DESCRIPTOR_SIZE. For any two faces, the distance between descriptors is a measure of their similarity.
	*
	* For the purpose of face recognition, faces are stored in a gallery. The gallery is an array of face descriptors, with a corresponding array of names, 
	* so that the gallery may contain n descriptors and a corresponding name for each descriptor. To perform recognition, the face descriptor extracted from 
	* the input image is then compared with face descriptors from the gallery and the similarity between the input face descriptor and all face descriptors from 
	* the gallery is calculated in order to find the face(s) from the gallery that are most similar to the input face. The VisageFaceRecognition class includes 
	* a full set of functions for manipulating the gallery, including adding, removing, naming descriptors in the gallery as well as loading and saving the gallery to file. 
	*/
	class VISAGE_ANALYSER_DECLSPEC VisageFaceRecognition
{

private:
	VisageNNModel* m_model;
	VisageNNRunner* m_runner;
	char* m_galleryPath;
	vector<pair<short*, string > > m_descriptors;
	vector<pair<float, string > > m_result;
	int m_descriptorSize;

public:

	/**
	* If the model used in the VisageFaceRecognition is loaded and initialized successfully value of the is_initialized variable is true, otherwise it is false. Make sure the variable is set to true after instantiation 
	and before using other function of VisageFaceRecognition API.
	* An example of use:
	* \code
	* VisageFaceRecognition *m_VFR = new VisageFaceRecognition("bdtsdata/NN/fr.bin");
	* //
	* if(m_VFR->is_initialized)
	* {
	*		int descriptor_size = m_VFR->getDescriptorSize();
	*		// use VisageFaceRecognition API functions
	* }
	* \endcode
	*/
	bool is_initialized;

#ifdef EMSCRIPTEN
	/** Constructor.
	*/
	VisageFaceRecognition();
#endif
	/** Constructor.
	* @param dataPath path to the data file required for face recognition. 
	In the visage|SDK distribution, the file is: Samples/data/bdtsdata/NN/fr.bin.
	*/
	VisageFaceRecognition(const char* dataPath);
	/** Destructor.
	*/
	~VisageFaceRecognition();

	/** Gets the descriptor's size.
	*
	* The function returns size of an array which should be allocated for storing the descriptor.
	*
	* @return descriptor's size.
	*/
	int getDescriptorSize();

	/** Extracts the face descriptor for face recognition from a facial image. Prior to using this function, it is necessary to process the facial image or video frame 
	* using VisageTracker or VisageFeaturesDetector and pass the obtained facial data to this function. 
	* An example of use:
	* \code
	* IplImage* iplImage = cvLoadImage(imageFile.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
	* VsImage* image = vsCreateImageHeader(vsSize(iplImage->width, iplImage->height), iplImage->depth, iplImage->nChannels);
	*
	* std::string dataPathFeaturesDetector("../Samples/data/bdtsdata");
	* VisageFeaturesDetector *m_Detector = new VisageFeaturesDetector();
	*
	* std::string dataPathFaceRecognition("../Samples/data/bdtsdata/NN/fr.bin");
	* VisageFaceRecognition *m_Recognition = new VisageFaceRecognition(dataPathFaceRecognition.c_str());
	* 
	* if(m_Detector->Initialize(dataPathFeaturesDetector.c_str()) && m_Recognition->is_initialized)
	* {
	*	int DESCRIPTOR_SIZE = m_Recognition->getDescriptorSize();
	* 	FaceData* data = new FaceData[MAX_FACES];
	*	int n_faces = m_Detector->detectFacialFeatures(image, data, MAX_FACES);	
	*	for (int i = 0; i < n_faces; i++){
	*		short* descriptor = new short[DESCRIPTOR_SIZE];
	*		int status = m_Recognition->extractDescriptor(&data[i], image, descriptor);
	* 	}
	* }
	*
	* vsReleaseImageHeader(&image);
	* cvReleaseImage(&iplImage);
	* \endcode
	*
	* @param facedata facial data obtained from VisageTracker or VisageFeaturesDetector.
	* @param image VsImage pointer to the input RGB or grayscale image.
	* @param descriptor pointer to a DESCRIPTOR_SIZE-dimensional array of short. The resulting face descriptor is returned in this array.
	*
	* @return 1 on success, 0 on failure.
	*
	* See also: FaceData, VisageTracker, VisageFeaturesDetector
	*/
	int extractDescriptor(FaceData *facedata, VsImage *image, short *descriptor);

	/** Calculates similarity between two descriptors.
	*
	* The function returns a float value between 0 and 1. Two descriptors are equal if the similarity is 1. Two descriptors are completely different if the similarity is 0.
	*
	* @param first_descriptor pointer to the first descriptor, an array of DESCRIPTOR_SIZE short values.
	* @param second_descriptor pointer to the second descriptor, an array of DESCRIPTOR_SIZE short values.
	* @return float value between 0 and 1, 1 means full similarity and 0 means full diversity.
	*/
	float descriptorsSimilarity(short *first_descriptor, short *second_descriptor);

	/** Extracts a face descriptor from the input RGB image and adds it to the gallery.
	*
	* @param image VsImage pointer that contains the input RGB image. The image should contain only one face and this face will be added to the gallery. In case of multiple faces in the image, it is not defined which face would be used.
	* @param facedata facial data obtained from VisageTracker or VisageFeaturesDetector.
	* @param name name of the face in the image.
	* @return 1 on success, 0 on failure. The function fails if the face is not found in the image or if the image argument is not a valid RGB image pointer.
	*
	* See also: VsImage
	*/
	int addDescriptor(VsImage *image, FaceData *facedata, const char *name);

	/** Adds face descriptor to the gallery.
	*
	* @param descriptor descriptor to be added to the gallery obtained by calling extractDescriptor.
	* @param name name of the descriptor.
	* @return 1 on success, 0 on failure. The function fails if the descriptor is not a valid RGB image pointer.
	*/
	int addDescriptor(short *descriptor, const char *name);

	/** Gets number of descriptors in the gallery.
	* @return returns number of descriptors in the gallery.
	*/
	int getDescriptorCount();

	/** Gets the name of a descriptor at the given index in the gallery.
	* @param name buffer into which the name of a descriptor is filled.
	* @param index index of descriptor in the gallery.
	* @return 1 on success, 0 on failure. The function fails if index is out of range.
	*/
	int getDescriptorName(char* name, int index);

	/** Replaces the name of a descriptor at the given index in the gallery with new name.
	* @param name new descriptor name.
	* @param index index of descriptor in the gallery.
	* @return 1 on success, 0 on failure. The function fails if index is out of range.
	*/
	int replaceDescriptorName(const char* name, int index);

	/** Removes a descriptor at the given index from the gallery. The remaining descriptors in the gallery above the given index (if any) are shifted down in the gallery array by one place, filling the gap created by removing the descriptor.
	* @param index index of descriptor in the gallery.
	* @return 1 on success, 0 on failure. The function fails if index is out of range.
	*/
	int removeDescriptor(int index);

	/** Save gallery as a binary file.
	*
	* @param file_name name of the file (including path if needed) into which the gallery will be saved
	* @return 1 on success, 0 on failure. The function fails if the file can not be opened.
	*/
	int saveGallery(const char *file_name);

	/** Load gallery from a binary file. The entries from the loaded gallery are appended to the current gallery. If it is required to replace existing gallery with the loaded one, call resetGallery() first.
	*
	* @param file_name name of the file (including path if needed) from which the gallery will be loaded.
	* @return 1 on success, 0 on failure. The function fails if the file can not be opened.
	*/	
	int loadGallery(const char *file_name);
	
	/** Clear all face descriptors from the gallery.
	*/	
	void resetGallery();

	/** Compare a face to all faces in the current gallery and return n names of the most similar faces.
	*
	* @param descriptor pointer to the face descriptor of the face to be recognized (compared to the gallery).
	* @param n number of names and similarities to be returned.
	* @param names pointer to an array of char arrays. In this array the function will return names of n faces from the gallery that are most similar to the input image ascending by similarity.
	* @param similarities pointer to an array of n floats. The function will return the similarity values for the n most similar faces in this array, corresponding to the names array. The values are sorted, with the largest similarity value first.
	* @return number of compared faces. 
	*/

	int recognize(short *descriptor, int n, const char** names, float* similarities);
};

}

#endif
