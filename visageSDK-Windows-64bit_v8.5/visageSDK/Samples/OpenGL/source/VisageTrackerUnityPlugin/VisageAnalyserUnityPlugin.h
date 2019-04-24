//
//  VisageTrackerUnityPluginAnalyser.h
//  VisageTrackerUnityPluginAnalyser
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

	/** Initialises  VisageFaceAnalyser.
	*
	* This function must be called before using VisageFaceAnalyser by passing it a path to the folder containing the VisageFaceAnalyser data files.
	*/
	EXPORT_API int _initAnalyser(char* config);

	/** Releases memory allocated by the face analyser in the _initAnalyser() function.
	*/
	EXPORT_API void _releaseAnalyser();

	/** Estimates emotion from a facial image. In case of multiple trackers running faceIndex specifies tracker ID.
	*
	*/
	EXPORT_API int _estimateEmotion(float* emotions, int faceIndex);

	/** Estimates age from a facial image. In case of multiple trackers running faceIndex specifies tracker ID.
	*
	* The function returns estimated age. Prior to using this function, it is necessary to process the video frame using _track() function and prepare obtained data with _prepareDataForAnalysis() function.
	*/
	EXPORT_API float _estimateAge(int faceIndex);

	/** Estimates gender from a facial image. In case of multiple trackers running faceIndex specifies tracker ID.
	*
	* The function returns 1 if estimated gender is male and 0 if it is a female. Prior to using this function, it is necessary to process the video frame using _track() function and prepare obtained data with _prepareDataForAnalysis() function.
	*/
	EXPORT_API int _estimateGender(int faceIndex);

	/** Prepares data obtained by the track function.
	*/
	EXPORT_API bool _prepareDataForAnalysis();

}

