#include "Observer.h"
#include "TrackerGazeCalibrator.h"

Observer::Observer(void)
{
	track_stat = 0;

	image = 0;
	notifyCount = 0;
}

Observer::~Observer()
{
	vsReleaseImage(&image);
}

void Observer::Notify(FaceData& trackingData, int trackerStatus, VsImage* frame)
{
	this->trackingData = trackingData;
	
	gazeData.x = trackingData.gazeData.x;
	gazeData.y = trackingData.gazeData.y;
	gazeData.inState = trackingData.gazeData.inState;

	if(image == 0)
	{
		image= vsCloneImage(frame);
	}

	vsCopyImage(frame, image);

	track_stat = trackerStatus;
	this->timestamp = trackingData.timeStamp;

	notifyCount++;
}

