#include "GazeTrackerDoc.h"

#include <process.h>
#include <cv.h>
#include <highgui.h>
#include <videoInput.h>

#define lock_mutex(m) EnterCriticalSection(m)
#define unlock_mutex(m) LeaveCriticalSection(m)

CRITICAL_SECTION trackingData_mutex;

GazeTrackerDoc::GazeTrackerDoc()
{
	tracker = new VisageGazeTracker("Facial Features Tracker - High.cfg");
	observer = new Observer();
	trackingThreadHandle = 0;
	trackerActive = false;
	InitializeCriticalSection(&trackingData_mutex);
}

void GazeTrackerDoc::StartGazeTracking()
{

	srand (time(NULL));

	int width = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);
		
	resWindow = new Window();
	resWindow->Init(width, height, 32, _T("ResWin"));

	resWindow->mouseHandler = this;
	renderer = new GazeRenderer();
	renderer->SetDrawWindow(resWindow);

	calibrator = new Repository();

	calibrator->ReadFromFile("..//OpenGL//data//GazeTracker//calibration.txt");

	startTrackingFromCam();
	
	while(observer->track_stat != TRACK_STAT_OK && observer->track_stat != TRACK_STAT_OFF)
	{
		Sleep(40);
	}

	calibration = 0;

	while(!resWindow->exit)
	{
		tracker->InitOnlineGazeCalibration();
		resWindow->recalibStarted = false;
		index = 0;
		isCalibrated = false;
		isClicked = false;

		std::string displayText;

		while(index < calibrator->GetCount())
		{
			calibration = calibrator->Get(index);
			calibration->inState = 1;

			WaitForClick();

			if(resWindow->exit || resWindow->recalibStarted)
			{
				break;
			}
		}

		float quality = 0;
		if(!resWindow->exit && !resWindow->recalibStarted)
		{
			tracker->FinalizeOnlineGazeCalibration();
		}

		calibration = new ScreenSpaceGazeData();
		calibration->inState = 2;
		while(!isCalibrated && !resWindow->recalibStarted)
		{
			float x = (float)rand() / (float)RAND_MAX;
			float y = (float)rand() / (float)RAND_MAX;
	
			calibration->x = x;
			calibration->y = y;

			WaitForClick();

			if(resWindow->exit || resWindow->recalibStarted)
			{
				break;
			}

			if(!resWindow->exit && !resWindow->recalibStarted && calibration->inState == 1)
			{
				tracker->FinalizeOnlineGazeCalibration();
			}

			calibration->inState = calibration->inState == 2 ? 1 : 2;
		}

		delete calibration;
		calibration = 0;

		while(!resWindow->recalibStarted)
		{
			resWindow->ProcessWinMsg();

			if(resWindow->exit)
			{
				break;
			}

			renderer->Clear();
			
			displayText = "Press space to restart the calibration";
			int len = displayText.length();
			int size = glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, 'I');
			renderer->DisplayText((char*)displayText.c_str(), -1.0f + 2.0f * ((float)(width / 2) - (len * size / 2)) / (float)width, -0.9);

			lock_mutex(&trackingData_mutex);

			if(observer->track_stat != TRACK_STAT_OFF)
			{
				renderer->DrawImage(observer->image);
			}

			if(observer->track_stat == TRACK_STAT_OK)
			{
				renderer->DrawTrackingData(&observer->trackingData, observer->image);
			}


			if(observer->gazeData.inState == 0 || observer->track_stat == TRACK_STAT_OFF || observer->track_stat == TRACK_STAT_INIT || observer->track_stat == TRACK_STAT_RECOVERING)
			{
				renderer->DrawPoint(0.5f, 0.5f, 0, 0, 1, 15.0f);
			}

			if(observer->gazeData.inState == 2 && observer->track_stat == TRACK_STAT_OK)
			{
				renderer->DrawPoint(observer->gazeData.x, observer->gazeData.y, 0, 0, 1, 15.0f);
			}

			unlock_mutex(&trackingData_mutex);

			renderer->Swap();

			Sleep(40);
		}
	}

	// stop thread if active previously
	trackerActive = false;

	WaitForSingleObject(trackingThreadHandle, INFINITE);

	delete tracker;
	delete observer;
	delete resWindow;
	delete renderer;
	delete calibrator;
}

void GazeTrackerDoc::OnLeftButtonDown(float x, float y)
{
	if(calibration == 0)
	{
		return;
	}

	int width = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);

	if(calibration->inState == 1)
	{
		if(calibration->x * width < x + 20.0f && calibration->x * width > x - 20.0f &&
		height - calibration->y * height < y + 20.0f && height - calibration->y * height > y - 20.0f)
		{
			tracker->AddGazeCalibrationPoint((float)x / (float)width, 1.0f - (float)y / (float)height);
			index++;
			isClicked = true;
		}
	}
	else
	{
		if(calibration->x * width < x + 20.0f && calibration->x * width > x - 20.0f &&
		height - calibration->y * height < y + 20.0f && height - calibration->y * height > y - 20.0f)
		{
			float error_x = abs(observer->gazeData.x - x / width);
			float error_y = abs(observer->gazeData.y - y / height);

			if(error_x < 0.06f && error_y < 0.06f)
			{
				//isCalibrated = true;
			}
			else
			{
				tracker->AddGazeCalibrationPoint((float)x / (float)width, 1.0f - (float)y / (float)height);
				index++;
			}

			//tracker->AddGazeCalibrationPoint((float)x / (float)width, 1.0f - (float)y / (float)height);
			//index++;

			isClicked = true;
		}
	}
}

void GazeTrackerDoc::WaitForClick()
{
	std::string displayText;
	int width = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);

	int pointSize = 15.0f;

	while(!isClicked)
	{
		if(resWindow->exit || resWindow->recalibStarted)
		{
			break;
		}

		renderer->Clear();

		if(observer->track_stat == TRACK_STAT_OFF || observer->track_stat == TRACK_STAT_INIT || observer->track_stat == TRACK_STAT_RECOVERING)
		{
			displayText = "Please wait while tracker initializes";
			int len = displayText.length();
			int size = glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, 'I');
			renderer->DisplayText((char*)displayText.c_str(), -1.0f + 2.0f * ((float)(width / 2) - (len * size / 2)) / (float)width, -0.9);
		}

		lock_mutex(&trackingData_mutex);

		if(observer->track_stat != TRACK_STAT_OFF)
		{
			renderer->DrawImage(observer->image);
		}

		if(observer->track_stat == TRACK_STAT_OK)
		{
			if(index < calibrator->GetCount())
			{
				displayText = "Please click on red calibration dot";
				
			}
			else
			{
				displayText = "Initial calibration is finished, additional calibration can be done by clicking the red dot";
			}
			int len = displayText.length();
			int size = glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, 'I');
			renderer->DisplayText((char*)displayText.c_str(), -1.0f + 2.0f * ((float)(width / 2) - (len * size / 2)) / (float)width, -0.9);

			renderer->DrawTrackingData(&observer->trackingData, observer->image);
		}

		PointOutOfScreenFix(calibration->x, calibration->y, width, height, pointSize);

		renderer->DrawPoint(calibration->x, calibration->y, 1, 0, 0, pointSize);

		if(observer->gazeData.inState == 0 || observer->track_stat == TRACK_STAT_OFF || observer->track_stat == TRACK_STAT_INIT || observer->track_stat == TRACK_STAT_RECOVERING)
		{
			renderer->DrawPoint(0.5f, 0.5f, 0, 0, 1, pointSize);
		}

		if(observer->gazeData.inState == 2 && observer->track_stat == TRACK_STAT_OK)
		{
			float x = observer->gazeData.x;
			float y = observer->gazeData.y;

			PointOutOfScreenFix(x, y, width, height, pointSize);

			renderer->DrawPoint(x, y, 0, 0, 1, 15.0f);
		}

		unlock_mutex(&trackingData_mutex);

		renderer->Swap();

		Sleep(40);
		resWindow->ProcessWinMsg();
	}
	
	isClicked = false;
}

void GazeTrackerDoc::PointOutOfScreenFix(float& x, float& y, int width, int height, int pointSize)
{
	//possible fix for calibration points near the screen edges, under menus
	pointSize += 50;

	if(x >= 1 - ((float)pointSize / (float)( width)))
	{
		x = (float)(width - (float)pointSize) / (float)width;
	}

	if(y >= 1 - ((float)pointSize /(float)(height)))
	{
		y = (float)(height - (float)pointSize) / (float)height;
	}

	if(x <= ((float)pointSize / (float)(width)))
	{
		x = (float)(pointSize / (float)(width));
	}

	if(y <= ((float)pointSize / (float)(height)))
	{
		y = (float)(pointSize / (float)(height));
	}
}

void GazeTrackerDoc::trackFromCAM(void *thisdoc)
{
	GazeTrackerDoc *td = (GazeTrackerDoc *)thisdoc;

	FaceData trackingData;

	int track_stat = 0;

	int cam_device = 0;
	int cam_width = 800;
	int cam_height = 600;
	int cam_fps = 30;
int format = VISAGE_FRAMEGRABBER_FMT_RGB;

#define USE_VIDEOINPUT
#ifdef USE_VIDEOINPUT
	// ***** Grabbing from camera using Videoinput library *******
	videoInput *VI = new videoInput();

	VI->setIdealFramerate(cam_device, cam_fps);
	bool r = VI->setupDevice(cam_device, cam_width, cam_height);
	cam_width = VI->getWidth(cam_device);
	cam_height = VI->getHeight(cam_device);

	IplImage *framePtr = cvCreateImage(cvSize(cam_width, cam_height), IPL_DEPTH_8U, 3);

	while (td->trackerActive) {

		while(!VI->isFrameNew(cam_device))
			Sleep(1);

		VI->getPixels(cam_device, (unsigned char*)framePtr->imageData, false, true);
#else
	// ***** Grabbing from camera using OpenCV library *******
	CvCapture* capture = cvCaptureFromCAM( cam_device );

	int ret = cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, cam_width);
	ret = cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, cam_height);
	ret = cvSetCaptureProperty(capture, CV_CAP_PROP_FPS, cam_fps);

	cam_width = (int)cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH );
	cam_height = (int)cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT );

	while (td->trackerActive && cvGrabFrame(capture)) {

		const IplImage *framePtr = cvRetrieveFrame(capture); // grab latest frame
#endif

		track_stat = td->tracker->track(framePtr->width,framePtr->height,framePtr->imageData, &trackingData, format, framePtr->origin);

		lock_mutex(&trackingData_mutex);

		td->observer->Notify(trackingData, track_stat, (VsImage*)framePtr);

		unlock_mutex(&trackingData_mutex);
	}

	td->tracker->stop();

#ifdef USE_VIDEOINPUT
	VI->stopDevice(cam_device);
	delete VI; VI = 0;
	cvReleaseImage(&framePtr);
#else
	cvReleaseCapture(&capture);
#endif

	_endthread();
}

void GazeTrackerDoc::startTrackingFromCam()
{
	// stop thread if active previously
	trackerActive = false;
	
	WaitForSingleObject(trackingThreadHandle, INFINITE);

	trackerActive = true;

	trackingThreadHandle = (HANDLE)_beginthread(trackFromCAM,0,(void*)this);
}
