using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace VisageCSWrapper
{
	class VisageFeaturesDetector;

	/// <summary> VisageFaceRecognition class contains a face recognition algorithm capable of measuring similarity between human faces and recognizing a person's identity from frontal facial images (yaw angle approximately from -20 to 20 degrees). 
	/// Similarity between two faces is calculated as difference between their face descriptors and returned as a value between 0 (no similarity) and 1 (maximum similarity).
	///
	/// The face descriptor is a condensed representation of a face. It is an array of short values. The dimension of the array is obtained using getDescriptorSize() function, from now on in the text referred to as DESCRIPTOR_SIZE. For any two faces, the distance between descriptors is a measure of their similarity. 
	///
	/// For the purpose of face recognition, faces are stored in a gallery. The gallery is an array of face descriptors, with a corresponding array of names, so that the gallery may contain n descriptors and a corresponding name for each descriptor. To perform recognition, the face descriptor extracted from the input image is then compared with face descriptors from the gallery and the similarity between the input face descriptor and all face descriptors from the gallery is calculated in order to find the face(s) from the gallery that are most similar to the input face. The VisageFaceRecognition class includes a full set of functions for manipulating the gallery, including adding, removing, naming descriptors in the gallery as well as loading and saving the gallery to file.
	///
	/// </summary>
	class VISAGE_DECLSPEC VisageFaceRecognition
{

private:
	THNETWORK *net;
	char* gallery_path;
	vector<pair<short*, string > > descriptors;
	vector<pair<float, string > > result;
	VisageFeaturesDetector *m_Detector;

public:

	/// <summary>
	///	Constructor.
	/// <param name="dataPath"> path to the data file required for face recognition. In the visage|SDK distribution, the file is: Samples/data/bdtsdata/NN/fr.bin. </param>
	/// </summary>
	public VisageFaceRecognition(System::String^ dataPath);
	
	/// <summary>
	/// Destructor.
	/// </summary>
	~VisageFaceRecognition();

	/// <summary>
	/// Gets the descriptor's size.
	///
	/// The function returns size of an array which should be allocated for storing the descriptor.
	/// <returns> descriptor's size. </returns>
	int GetDescriptorSize();

	/// <summary>
	/// Extracts the face descriptor for face recognition from a facial image. Prior to using this function, it is neccessary to process the facial image or video frame using VisageTracker or VisageFeaturesDetector and pass the obtained facial data to this function.
	///
	/// </summary>
	/// <param name="facedata"> Facial data obtained from VisageTracker or VisageFeaturesDetector. </param>
	/// <param name="image"> VsImage pointer to the input RGB image. </param>
	/// <param name="descriptor"> Pointer to a DESCRIPTOR_SIZE-dimensional array of short. The resulting face descriptor is returned in this array. </param>
	/// <returns> Returns 1 on success, 0 on failure. </returns>
	///
	/// See also: FaceData, VisageTracker, VisageFeaturesDetector
	///
	int ExtractDescriptor(FaceData^ facedata, VSImage^ image, short[] descriptor);

	/// <summary>
	/// Calculates similarity between two descriptors.
	///
	/// The function returns a float value between 0 and 1. Two descriptors are equal if the similarity is 1. Two descriptors are completely different if the similarity is 0.
	/// </summary>
	/// <param name="first_descriptor"> Pointer to the first descriptor, an array of DESCRIPTOR_SIZE short values. </param>
	/// <param name="second_descriptor"> Pointer to the second descriptor, an array of DESCRIPTOR_SIZE short values. </param>
	/// <returns> Returns float value between 0 and 1, 1 means full similarity and 0 means full diversity. </returns>
	///
	float DescriptorsSimilarity(short[] first_descriptor, short[] second_descriptor);

	/// <summary>
	/// Extracts a face descriptor from the input RGB image and adds it to the gallery.
	/// </summary>
	/// <param name="image"> VsImage pointer that contains the input RGB image. The image should contain only one face and this face will be added to the gallery. In case of multiple faces in the image, it is not defined which face would be used. </param>
	/// <param name="facedata"> Facial data obtained from VisageTracker or VisageFeaturesDetector. </param>
	/// <param name="name"> Name of the face in the image. </param>
	/// <returns> Returns 1 on success, 0 on failure. The function fails if the face is not found in the image or if the image argument is not a valid RGB image pointer. </returns>
	///
	///
	/// See also: VSImage
	int AddDescriptor(VSImage^ image, FaceData^ facedata, System::String^ name);

	/// <summary>
	/// Adds a face descriptor to the gallery.
	/// </summary>
	/// <param name="descriptor"> Pointer to the face descriptor (an array of DESCRIPTOR_SIZE short values) which will be added to the gallery. </param>
	/// <param name="name"> Name of the face in the image. </param>
	/// <returns> Returns 1 on success, 0 on failure. The function fails if the descriptor is not a valid RGB image pointer. </returns>
	///
	int AddDescriptor(short[] descriptor, System::String^ name);

	/// <summary>
	/// Gets number of descriptors in the gallery.
	/// </summary>	
	/// <returns> Returns number of descriptors in the gallery. </returns>
	///
	int GetDescriptorCount();

	/// <summary>
	/// Gets the name of a descriptor at the given index in the gallery.
	/// </summary>
	/// <param name="name"> Buffer into which the name of a descriptor is filled. </param>
	/// <param name="index"> Index of descriptor in the gallery. </param>
	/// <returns> Returns 1 on success, 0 on failure. The function fails if index is out of range. </returns>
	///
	int GetDescriptorName(char* name, int index);

	/// <summary>
	/// Replaces the name of a descriptor at the given index in the gallery with new name.
	/// </summary>
	/// <param name="name"> New descriptor name. </param>
	/// <param name="index"> Index of descriptor in the gallery. </param>
	/// <returns> Returns 1 on success, 0 on failure. The function fails if index is out of range. </returns>
	///
	int ReplaceDescriptorName(const char* name, int index);

	/// <summary>
	///	Removes a descriptor at the given index from the gallery. The remaining descriptors in the gallery above the given index (if any) are shifted down in the gallery array by one place, filling the gap created by removing the descriptor.
	/// </summary>
	/// <param name="index"> Index of descriptor in the gallery. </param>
	/// <returns> Returns 1 on success, 0 on failure. The function fails if index is out of range. </returns>
	///
	int RemoveDescriptor(int index);
	
	/// <summary>
	/// Save gallery as a binary file.
	/// </summary>
	/// <param name="file_name"> Name of the file (including path if needed) into which the gallery will be saved. </param>
	/// <returns> Returns 1 on success, 0 on failure. The function fails if the file can not be opened. </returns>
	///
	int SaveGallery(System::String^ file_name);

	/// <summary>
	/// Load gallery from a binary file. The entries from the loaded gallery are appended to the current gallery. 
	///  If it is required to replace existing gallery with the loaded one, call resetGallery() first.
	/// </summary>
	/// <param name="file_name"> Name of the file (including path if needed) from which the gallery will be loaded. </param>
	/// <returns> Returns 1 on success, 0 on failure. The function fails if the file can not be opened. </returns>
	///
	int LoadGallery(System::String^ file_name);
	
	/// <summary>
	/// Clear all face descriptors from the gallery.
	/// </summary>	
	void ResetGallery();

	/// <summary>
	/// Compare a face to all faces in the current gallery and return n names of the most similar faces.
	/// </summary>
	/// <param name="descriptor"> Pointer to the face descriptor of the face to be recognized (compared to the gallery). </param>
	/// <param name="n"> Number of names and similarities to be returned. </param>
	/// <param name="names"> Pointer to an array of char arrays. In this array the function will return names of n faces from the gallery that are most similar to the input image ascending by similarity. </param>
	/// <param name="similarities"> Pointer to an array of n floats. The function will return the ascending similarity values for the n most similar faces in this array corresponding to the names array. The values are sorted, with the largest similarity value first. </param>
	/// <returns> Returns number of compared faces. </returns>
	///
	int Recognize(short[] descriptor, int n, ref string[] names, float[] similarities);

	/// <summary>
	/// If the model used in the VisageFaceRecognition is loaded and initialized successfully value of the is_initialized variable is true, otherwise it is false.
    /// Make sure the variable is set to true after instantiation and before using other function of VisageFaceRecognition API.
	///
	/// An example of use:
	/// \code
	/// VisageFaceRecognition *m_VFR = new VisageFaceRecognition("bdtsdata/NN/fr.bin");
	/// if(m_VFR->is_initialized)
	/// {
	/// 		int descriptor_size = m_VFR.GetDescriptorSize();
	/// 		// use VisageFaceRecognition API functions
	/// }
	/// \endcode
	/// Read only property. 
	/// <returns>Return true if VisageFaceRecognition is successfully initialized otherwise returnes false.</returns>
	/// </summary>
	property bool IsInitialized;
}

}