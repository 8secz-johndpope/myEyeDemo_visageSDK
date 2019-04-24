//
//  VisageTrackerUnityPluginRecognition.h
//  VisageTrackerUnityPluginRecognition
//
//

#include "VisageTrackerUnityShared.h"

#ifdef WIN32
#ifdef _MSC_VER
// ensures valid dll function export
#define EXPORT_API __declspec(dllexport)
#endif
#endif



extern "C" {

	/** Initialises VisageFaceRecognition.
	*
	* If the VisageFaceRecognition is initialized successfully function returns true, otherwise it returns false. 
	*/
	EXPORT_API bool _initRecognition(char* dataPath);

	/** Releases memory allocated by the face recogntion in the _initRecognition() function.
	*/
	EXPORT_API void _releaseRecognition();

	/** Returns the size of descriptor.
	*
	*/
	EXPORT_API int _getDescriptorSize();

	/** Extracts the face descriptor for face recognition.
	* 
	*/
	EXPORT_API bool _prepareDataForRecog();
	
	/** Extracts the face descriptor for face recognition. Prior to using this function, it is necessary to process the video frame
	* using _track() function and prepare obtained data with _prepareDataForRecog() function.
	*/
	EXPORT_API int _extractDescriptor(short* descriptor, int faceIndex);

	/** Calculates similarity between two descriptors. In caseof multiple trackers running faceIndex specifies tracker ID.
	*
	*/
	EXPORT_API float _descriptorsSimilarity(short* firstDescrtiptor, short* secondDescriptor);

	/** Adds descriptor to the gallery.
	*
	*/
	EXPORT_API int _addDescriptor(short* descriptor, const char* name);

	/** Gets number of descriptors in the gallery.
	*
	*/
	EXPORT_API int _getDescriptorCount();

	/** Gets the name of a descriptor at the given index in the gallery.
	*
	*/
	EXPORT_API const char* _getDescriptorName(int galleryIndex);

	/** Replaces the name of a descriptor at the given index in the gallery with new name.
	*
	*/
	EXPORT_API	int _replaceDescriptorName(const char* name, int galleryIndex);

	/** Removes a descriptor at the given index from the gallery.
	* The remaining descriptors in the gallery above the given index (if any) are shifted down in the gallery array by one place, filling the gap created by removing the descriptor.
	*
	*/
	EXPORT_API int _removeDescriptor(int galleryIndex);

	/** Save gallery as a binary file.
	*
	*/
	EXPORT_API int _saveGallery(const char* file_name);

	/** Loads gallery from a binary file.
	* The entries from the loaded gallery are appended to the current gallery. If it is required to replace existing gallery with the loaded one, call _resetGallery() first.
	*
	*/
	EXPORT_API int _loadGallery(const char* file_name);

	/** Clear all face descriptors from the gallery. 
	*/
	EXPORT_API void _resetGallery();

	/** Compare a face to all faces in the current gallery and return n names of the most similar faces. 
	*
	*/
	EXPORT_API int _recognize(short* descriptor, int n, char** names, int cap, float* similarities);
}
