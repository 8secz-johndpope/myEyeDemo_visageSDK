#include "TrackerGazeCalibrator.h"

using namespace VisageSDK;

/**
* Class used to provide calibration data to the screen space gaze tracking system.
* Aggregates VisageSDK::ScreenSpaceGazeRepositroy object to hold calibration data.
* Implements method for reading calibration data from file.
* Provided sample calibration data located in ../Samples/OpenGL/data/GazeTracker is written is following format:
* frame_index x_coordinate y_coordinate
*@see VisageSDK::ScreenSpaceGazeRepository
*/
class Repository
{
public:
	
	/**
	* Constructor.
	**/
	Repository();

	/**
	* Reads calibration data from file written in following format: frame_index x y
	**/
	void ReadFromFile(char* filename);
	
	ScreenSpaceGazeData* Get(int index);
	int GetCount();

private:
	ScreenSpaceGazeRepository* calibrator;
	
};