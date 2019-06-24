#pragma once

#include "visageVision.h"
#include "opencv2\opencv.hpp"
#include <opencv2/core/types_c.h>


//namespace VisageSDK //all visage|SDK calls must be in visageSDK namespace
//{
//	int __declspec(dllimport) initializeLicenseManager(const char *licenseKeyFileFolder);
//
//}

class IrisTracker
{
private:

	////// ipad setting//////  
	//frame   // 1280*720
	int resizef;
	bool isVertical = true;
	int trackerDelay = 10;
	// Gaussian blur
	int blurCount = 5;
	int blurKernelSize = 5;
	//Canny
	int cannyp1 = 80;
	int cannyp2 = 120;
	//MaskForInside_preprocess line type
	int maskLine = 2;
	//Iris radius_second calculation bounding, all positive number
	int radt1Plus = 3;  //3
	int radt1Minus = 1;  // 1
	int radChangeNoLessThan = 3;
	int borderFactorx = 5;
	int borderFactory = 4;
	int radiusAdd = 8;
	//lens
	const cv::String* lensPath;
	float lensTransparentWeight = 0.4;

	int wait = 13 - trackerDelay;

	int frameRate;  // must > 20;
	VisageSDK::VisageTracker* tracker;
	VisageSDK::VisageFaceAnalyser *analyser;
	VisageSDK::FaceData trackingData[3];
	const char* lefteye[8] = { "3.1", "3.3", "3.7", "3.11", "12.5", "12.7", "12.9", "12.11" };  //8 points
	const char* righteye[8] = { "3.2", "3.4", "3.8", "3.12", "12.6", "12.8", "12.10", "12.12" };
	const char* leftPupil = "3.5";
	const char* rightPupil = "3.6";
	std::vector<cv::Point> lefteyeCV, righteyeCV;
	cv::Point leftPupilCV, rightPupilCV;
	int radius_l = 0;
	int radius_r = 0;
	cv::Mat _lens;
	bool isChanged = false;

public:
	// set path: 
	//"Facial Features Tracker - High.cfg",  "lens-1.jpg" , and  "Samples\data\bdtsdata\LBF\vfadata" folder
	// framerate must > 20
	void initialize(const char* highCfg, const char* lensPath, const char* vfadata, int framerate);

	// output will be the picture to show
	// return 0 if tracking success, 1 if eye closure is detect(having no pupil and mask), 2 3 4 5 if error 
	int irisTrack(cv::Mat frame_, cv::Mat& output);

	// get the properties of the face detected
	int getAge(cv::Mat frame);
	float* getEmotion(cv::Mat frame);  // float[7] : anger, disgust, fear, happiness, sadness, surprise and neutral
	int getGender(cv::Mat frame);

private:
	int* vis2cv_AxisTransfer(const float* pos, int width, int height);
	float pointsDistance(int* cvCoor, cv::Point point);
	float pointsDistance(cv::Point2d p1, cv::Point p2);
	cv::Mat face(cv::Mat image);
	void makeup(VisageSDK::FaceData faceData, int width, int height, cv::Mat frame);
public:
	IrisTracker();
	~IrisTracker();
};

