using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;


/// <summary>
/// Provides high-level functions for using visage|SDK face recognition capabilities in Unity game engine.
/// It implements high-level wrapper functions from visage|SDK that enable usage of visage|SDK face recognition capabilities in Unity C# scripts.
/// 
/// VisageFaceRecognition contains a face recognition algorithm capable of measuring similarity between human faces and recognizing a person's identity from frontal facial images (yaw angle approximately from -20 to 20 degrees).
/// 
/// Similarity between two faces is calculated as difference between their face descriptors and returned as a value between 0 (no similarity) and 1 (maximum similarity).
/// 
/// The face descriptor is a condensed representation of a face.It is an array of short values. 
/// The dimension of the array is obtained using getDescriptorSize() function, from now on in the text referred to as DESCRIPTOR_SIZE.
/// For any two faces, the distance between descriptors is a measure of their similarity.
/// 
/// For the purpose of face recognition, faces are stored in a gallery.The gallery is an array of face descriptors, with a corresponding array of names,
/// so that the gallery may contain n descriptors and a corresponding name for each descriptor. 
/// To perform recognition, the face descriptor extracted from the input image is then compared with face descriptors from the gallery and the similarity between the input face descriptor
/// and all face descriptors from the gallery is calculated in order to find the face(s) from the gallery that are most similar to the input face.
/// The VisageFaceRecognition class includes a full set of functions for manipulating the gallery, including adding, removing, naming descriptors in the gallery as well as loading and saving the gallery to file.
/// 
/// The functionality of VisageFaceRecognition is dependent on the VisageTracker and it is expected that prior to the call of VisageFaceRecognition's functions frame is processed with _track() function 
/// and the data thus obtained is prepared and forwarded to VisageFaceRecognition.
/// 
/// Before using VisageFaceRecognition, it must be initialized by calling the _initRecognition() function.
/// Afterward, in each frame, the _prepareDataForRecog() function is called first and then all other VisageFaceRecognition's functions can be called
/// otherwise they will return -1 as an indication that the necessary data is missing.
/// 
/// VisageFaceRecognition's functionality can be used in a separate thread.
/// </summary>
/// 

/**
 * \defgroup VisageRecognitionUnityPlugin VisageRecognitionUnityPlugin
 * @{
 */

/** Initialises VisageFaceRecognition.
*
* If the VisageFaceRecognition is initialized successfully function returns true, otherwise it returns false. 
*
* Please note that in order for license to be registered, _initializeLicense() method has to be called before.
* 
* @param dataPath Relative path to the data file required for face recognition, including data file name. In the visage|SDK distribution, the file is: Samples/data/bdtsdata/NN/fr.bin. 
* @return True on success, false otherwise,
*/
bool _initRecognition(string dataPath);

/** Releases memory allocated by the face recogntion in the _initRecognition() function.
 * 
 * This function is called after the recognition ceases to be used or before the reinitialization of the recognition.
*/
void _releaseRecognition();

/** Returns the size of descriptor.
*
* @return DESCRIPTOR_SIZE.
*/
 int _getDescriptorSize();

/** Extracts the face descriptor for face recognition.
* 
*/
 bool _prepareDataForRecog();

/** Extracts the face descriptor for face recognition. Prior to using this function, it is necessary to process the video frame
* using _track() function and prepare obtained data with _prepareDataForRecog() function.
* In case of multiple trackers running faceIndex specifies tracker ID.
*
* @param descriptor DESCRIPTOR_SIZE-dimensional array that will be filled with the resulting face descriptor.
* @param faceIndex Value between 0 and MAX_FACES-1, used to access the data of a particular tracked face.
* @return 1 on success, 0 or -1 on failure.
*
* See also: FaceData, VisageTracker, VisageFeaturesDetector
*/
int _extractDescriptor(short[] descriptor, int faceIndex);

/** Calculates similarity between two descriptors.
*
* The function returns a float value between 0 and 1. Two descriptors are equal if the similarity is 1. Two descriptors are completely different if the similarity is 0.
*
* @param firstDescrtiptor An array of DESCRIPTOR_SIZE short values.
* @param secondDescriptor An array of DESCRIPTOR_SIZE short values.
* @return Float value between 0 and 1 on success, 1 means full similarity and 0 means full diversity or -1 on failure.
*/
float _descriptorsSimilarity(short[] firstDescrtiptor, short[] secondDescriptor);

/** Adds descriptor to the gallery.
*
* @param descriptor Descriptor to be added to the gallery obtained by calling _extractDescriptor().
* @param name Name of the descriptor.
* @return 1 on success, 0 or -1 on failure.
*/
int _addDescriptor(short[] descriptor, string name);

/** Gets number of descriptors in the gallery.
*
* @return Number of descriptors in the gallery.
*/
int _getDescriptorCount();

/** Gets the name of a descriptor at the given index in the gallery.
*
* @param galleryIndex Index of descriptor in the gallery.
* @return Name on success, empty string on failure.
*/
string _getDescriptorName(int galleryIndex);

/** Replaces the name of a descriptor at the given index in the gallery with new name.
*
* @param name New descriptor name.
* @param galleryIndex Index of descriptor in the gallery.
* @return 1 on success, 0 or -1 on failure. The function may fail if index is out of range.
*/
int _replaceDescriptorName(string name, int galleryIndex);

/** Removes a descriptor at the given index from the gallery.
* The remaining descriptors in the gallery above the given index (if any) are shifted down in the gallery array by one place, filling the gap created by removing the descriptor.
*
* @param galleryIndex Index of descriptor in the gallery.
* @return 1 on success, 0 or -1 on failure. The function may fail if index is out of range.
*/
int _removeDescriptor(int galleryIndex);

/** Save gallery as a binary file.
*
* @param fileName Name of the file (including path if needed) into which the gallery will be saved.
* @return 1 on success, 0 or -1 on failure. The function fails if the file can not be opened.
*/
int _saveGallery(string fileName);

/** Loads gallery from a binary file.
* The entries from the loaded gallery are appended to the current gallery. If it is required to replace existing gallery with the loaded one, call _resetGallery() first.
*
* @param fileName Name of the file (including path if needed) from which the gallery will be loaded.
* @return 1 on success, 0 or -1 on failure. The function fails if the file can not be opened.
*/
int _loadGallery(string fileName);

/** Clear all face descriptors from the gallery. 
*/
 void _resetGallery();

/** Compare a face to all faces in the current gallery and return n names of the most similar faces. 
* 
* Names array is passed as StringBuilder array to the function. The following code is an example of how the StringBuilder array is created, passed to the function and used after it is filled with strings:
* \code
* 
* //maximum number of characters per name
* const int SB_CAPACITY = 50;
* 
* //prepare stringbuilder
* System.Text.StringBuilder[] sbArray = new System.Text.StringBuilder[n];
* for (int i = 0; i < n; ++i)
* {
*    sbArray[i] = new System.Text.StringBuilder(SB_CAPACITY);
* }
* //call of the _recognize function
* int ret = _recognize(descriptor, n, sbArray, SB_CAPACITY, similarities);
* 
* List<string> names = new List<string>();
* 
* //fill list of strings
* for (int i = 0; i < n; ++i)
* {
*    names.Add(sbArray[i].ToString());
* }
* 
* \endcode
* 
* The example can be found in the VisageTrackerNative.Windows.cs (_recognizeWrapper() function) file within the VisageTrackerUnityDemo project.
* 
* @param descriptor Face descriptor of the face to be recognized (compared to the gallery).
* @param n Number of names and similarities to be returned.
* @param names List type of <a href="https://msdn.microsoft.com/en-us/library/system.text.stringbuilder(v=vs.110).aspx">StringBuilder</a> that will be filled with the names of n faces from the gallery that are most similar to the input image ascending by similarity. Maximum number of characters per name is set to 256, for larger name only first 256 characters will be returned.
* @param capacity Number representing the desired capacity of a particular name in a string of names.
* @param similarities Array that will be filled with the similarity values for the n most similar faces, corresponding to the names array. The values are sorted, with the largest similarity value first.
* @return Number of compared faces.
*/
int _recognize(short[] descriptor, int n, System.Text.StringBuilder[] names, int capacity, float[] similarities);

/** @} */

