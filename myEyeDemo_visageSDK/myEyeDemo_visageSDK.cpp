// myEyeDemo_visageSDK.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "visageVision.h"
#include "opencv2\opencv.hpp"
#include "IrisTracker.h"


using namespace cv;

namespace VisageSDK //all visage|SDK calls must be in visageSDK namespace
{
	void __declspec(dllimport) initializeLicenseManager(const char *licenseKeyFileFolder);
}

int* vis2cv_AxisTransfer(const float* pos, int width, int height);
void pointsInside(cv::Mat eyeMask, std::vector< std::vector<cv::Point>>* contours, std::vector<cv::Point2d>* insidePoints, int defx, int defy);
bool isNearBorder(cv::Point point, cv::Rect rect);
float pointsDistance(int* cvCoor, cv::Point point);
float pointsDistance(cv::Point2d p1, cv::Point p2);

void resize(Mat frame);
void hsl(Mat roi);


////// ipad setting//////  
//frame   // 1280*720
int resizeFactors[3] = {3,2,1};
int resizef;
bool isVertical = true;
int trackerDelay = 5;
// Gaussian blur
int blurCount = 4;
int blurKernelSize = 5;
//Canny
int cannyp1 = 80;
int cannyp2 = 120;
//MaskForInside_preprocess line type
int maskLine = 8;
//Iris radius_second calculation bounding, all positive number
int radt1Plus = 3;  //3
int radt1Minus = 1;  // 1
//add //
int pupilChangeNoLessThan = 1; 
//
int radChangeNoLessThan = 4;
int borderFactorx = 5;
int borderFactory = 4;
int radiusAdd = 0;
//lens
const cv::String lensPath = "C:\\Users\\jenny\\Desktop\\eye-demo\\pudding_blank.png";
float lensTransparentWeight = 0.4;

int wait = 10-trackerDelay;


/*
   //////my webcam setting//////
// frame = 480*640 for my webcam
int resizeFactors[3] = { 3,2,1 };
int resizef;
// Gaussian blur
int blurCount = 1; 
int blurKernelSize = 5;
bool isVertical = false;
int trackerDelay = 10;

//Canny
int cannyp1 = 60;
int cannyp2 = 100;
//MaskForInside_preprocess line type
int maskLine = 2;
//Iris radius_second calculation bounding, all positive number
int radt1Plus = 4;
int radt1Minus = 0;
int radChangeNoLessThan = 2;
int borderFactorx = 5;
int borderFactory = 4;
//lens
const cv::String lensPath = "C:\\Users\\jenny\\Desktop\\eye-demo\\lens-1.jpg";
float lensTransparentWeight = 0.4;

int wait = 33-trackerDelay;
*/

int main()
{
	VisageSDK::initializeLicenseManager(".");
	VisageSDK::VisageTracker tracker("../visageSDK-Windows-64bit_v8.5/visageSDK/Samples/data/Facial Features Tracker - High.cfg");
	VisageSDK::VisageFaceAnalyser *analyser = new VisageSDK::VisageFaceAnalyser();
	analyser->init("../visageSDK-Windows-64bit_v8.5/visageSDK/Samples/data/bdtsdata/LBF/vfadata");
	VisageSDK::FaceData trackingData[3];
	const char* lefteye[] = { "3.1", "3.3", "3.7", "3.11", "12.5", "12.7", "12.9", "12.11" };  //8 points
	const char* righteye[] = { "3.2", "3.4", "3.8", "3.12", "12.6", "12.8", "12.10", "12.12" };
	const char* leftPupil = "3.5";
	const char* rightPupil = "3.6";
	std::vector<cv::Point> lefteyeCV, righteyeCV;
	cv::Point leftPupilCV(0,0), rightPupilCV(0,0);

	/////// Initiallization////////

	// initiallize Points for comparation
	for (int i = 0; i < 8; i++) {
		lefteyeCV.push_back(cv::Point(0, 0));
		righteyeCV.push_back(cv::Point(0, 0));
	}

	int radius = 0;
	cv::Mat _lens;
	bool isChanged = true;
	VsImage* vsFrame;
	cv::Mat frame;
	cv::VideoCapture cap("D:\\eye-demo\\myEyeDemo_visageSDK\\IMG_0093.MOV");
	cap.read(frame);

	if (!cap.isOpened())
		return 1;

	IrisTracker irisTracker;
	irisTracker.initialize("../visageSDK-Windows-64bit_v8.5/visageSDK/Samples/data/Facial Features Tracker - High.cfg",
		"C:\\Users\\jenny\\Desktop\\eye-demo\\black_blank.png", "../visageSDK-Windows-64bit_v8.5/visageSDK/Samples/data/bdtsdata/LBF/vfadata", 25);
	

	//////////////


	///// IrisTracker testing loop //////
	while(false)
	{
		cap.read(frame);
		if (frame.empty()) {
			break;
		}

		cv::Mat output;
		int stat = irisTracker.irisTrack(frame, output);
		if (stat == 0) {
			resize(output, output, cv::Size(400, 500));
			cv::imshow("output", output);
		}
	}

	///////////

	
	/////// Video output ///////
	cv::Size size(frame.rows, frame.cols);
	VideoWriter writer;
	cout << size.height <<endl;
	writer.open("test_pudding_30fps_0.2_Area.avi", CV_FOURCC('M', 'J', 'P', 'G'), 30, size);
	
	/*
	for (int i = 0; i < 100; i++) {
		cap.read(frame);
		transpose(frame, frame);
		transpose(frame, frame);

		waitKey(5);
		writer.write(frame);
	}
	*/

	for (int i = 0; i < 30 * 10; i++) {
		cap.read(frame);
	}


	while (true) {
		// read frame and track face by sdk
		cap.read(frame);   
		//cap.read(frame);
		//frame = cv::imread("C:/Users/jenny/Desktop/eye-demo/1.jpg");
		if(isVertical)
			transpose(frame, frame);

		int a;
		IplImage *ipl = &IplImage(frame);
		vsFrame = (VsImage*)ipl;
		
		int* stat;
		stat = tracker.track(ipl->width, ipl->height, ipl->imageData, trackingData, VISAGE_FRAMEGRABBER_FMT_BGR);
		cv::waitKey(trackerDelay);		// tracker may be too slow to be real time
		
		if (stat[0] != TRACK_STAT_OK) {
			std::cout << "tracker err" << stat[0] << endl;
			continue;
		}

		// get eye feature points and convert to openCV coordinate (upper-left is(0,0) )
		const float *pos;
		int* cvCoor;
		for (int i = 0; i < 8; i++) {  
			// left eye
			pos = trackingData[0].featurePoints2D->getFPPos(lefteye[i]);
			cvCoor = vis2cv_AxisTransfer(pos, ipl->width, ipl->height);
			if (lefteyeCV[i].x == 0 || pointsDistance(cvCoor, lefteyeCV[i]) > 3) {
				lefteyeCV[i].x = cvCoor[0];
				lefteyeCV[i].y = cvCoor[1];
			}
			//right eye
			pos = trackingData[0].featurePoints2D->getFPPos(righteye[i]);
			cvCoor = vis2cv_AxisTransfer(pos, ipl->width, ipl->height);
			if (righteyeCV[i].x == 0 || pointsDistance(cvCoor, righteyeCV[i]) > 3) {
				righteyeCV[i].x = cvCoor[0];
				righteyeCV[i].y = cvCoor[1];
			}
		}
		//left pupil
		pos = trackingData[0].featurePoints2D->getFPPos(leftPupil);
		cvCoor = vis2cv_AxisTransfer(pos, ipl->width, ipl->height);

		// add (change)//
		if (leftPupilCV.x == 0 || pointsDistance(cvCoor, leftPupilCV) > pupilChangeNoLessThan) {
			leftPupilCV = cv::Point(cvCoor[0], cvCoor[1]);
		}
		//right pupil
		pos = trackingData[0].featurePoints2D->getFPPos(rightPupil);
		cvCoor = vis2cv_AxisTransfer(pos, ipl->width, ipl->height);
		if (rightPupilCV.x == 0 || pointsDistance(cvCoor, rightPupilCV) > pupilChangeNoLessThan) {
			rightPupilCV = cv::Point(cvCoor[0], cvCoor[1]);
		}
		///


		// get eyesquare roi 
		cv::Rect leftRect = cv::boundingRect(lefteyeCV);
		cv::Rect rightRect = cv::boundingRect(righteyeCV);
		if (leftRect.width == 1 || rightRect.width == 1) {
			std::cout << "bounding box err";
			continue;
		}
		if (!(0 <= leftRect.x && leftRect.x + leftRect.width <= frame.cols && 0 <= leftRect.y &&  leftRect.y + leftRect.height <= frame.rows)) {
			continue;
		}
		if (!(0 <= rightRect.x && rightRect.x + rightRect.width <= frame.cols && 0 <= rightRect.y &&  rightRect.y + rightRect.height <= frame.rows)) {
			continue;
		}
		cv::Mat leftRoi(frame, leftRect);
		cv::Mat rightRoi(frame, rightRect);


		// Blur and get canny
		cv::Mat leftblurbuffer = leftRoi.clone(), rightblurbuffer = rightRoi.clone();
		for (int i = 0; i < blurCount; i++) {
			cv::GaussianBlur(leftblurbuffer, leftblurbuffer, cv::Size(blurKernelSize, blurKernelSize), 1);
			cv::GaussianBlur(rightblurbuffer, rightblurbuffer, cv::Size(blurKernelSize, blurKernelSize), 1);
		}
		cv::Mat leftCanny, rightCanny;
		cv::Canny(leftblurbuffer, leftCanny, cannyp1, cannyp2);
		cv::Canny(rightblurbuffer, rightCanny, cannyp1, cannyp2);
		cv::imshow("rightC", rightCanny);
		cv::imshow("leftC", leftCanny);


		/////////get outer and inner eye roi////////////

		// draw two eye contour mask(from bounding box roi) seperately
		cv::Mat lefteyeMask(leftRect.height, leftRect.width, CV_8UC1, cv::Scalar(0)),
			righteyeMask(rightRect.height, rightRect.width, CV_8UC1, cv::Scalar(0));
		cv::Point cl_[8], cr_[8];
		int order[8] = { 3, 6, 0, 4, 2, 5, 1, 7 };  // counter Clockwise

		for (int i = 0; i < lefteyeCV.size(); i++) {
			cl_[i] = cv::Point(lefteyeCV[order[i]].x - leftRect.x, lefteyeCV[order[i]].y - leftRect.y);
			cr_[i] = cv::Point(righteyeCV[order[i]].x - rightRect.x, righteyeCV[order[i]].y - rightRect.y);
		}
		cv::fillConvexPoly(lefteyeMask, cl_, 8, cv::Scalar(255, 255, 255));
		cv::fillConvexPoly(righteyeMask, cr_, 8, cv::Scalar(255, 255, 255));
		cv::Mat lefteyeMaskForInside = lefteyeMask.clone(), righteyeMaskForInside = righteyeMask.clone();
		// shrink the mask for function "pointsInside()"

		////// add (change) //////
		for (int i = 4; i < 8; i++) {
			if (i == 7) {
				cv::line(lefteyeMaskForInside, cl_[i], cl_[0], cv::Scalar(0), 5);
				cv::line(righteyeMaskForInside, cr_[i], cr_[0], cv::Scalar(0), 5);
				cv::line(lefteyeMask, cl_[i], cl_[0], cv::Scalar(0), maskLine);
				cv::line(righteyeMask, cr_[i], cr_[0], cv::Scalar(0), maskLine);
			}
			else {
				cv::line(lefteyeMaskForInside, cl_[i], cl_[i + 1], cv::Scalar(0), 5);
				cv::line(righteyeMaskForInside, cr_[i], cr_[i + 1], cv::Scalar(0), 5);
				cv::line(lefteyeMask, cl_[i], cl_[i + 1], cv::Scalar(0), maskLine);
				cv::line(righteyeMask, cr_[i], cr_[i + 1], cv::Scalar(0), maskLine);
			}
		}
		for (int i = 0; i < 4; i++) {
			if (i == 7) {
				//cv::line(lefteyeMaskForInside, cl_[i], cl_[0], cv::Scalar(0), maskLine);
				//cv::line(righteyeMaskForInside, cr_[i], cr_[0], cv::Scalar(0), maskLine);
				cv::line(lefteyeMask, cl_[i], cl_[0], cv::Scalar(0), 4);
				cv::line(righteyeMask, cr_[i], cr_[0], cv::Scalar(0), 4);
			}
			else {
				//cv::line(lefteyeMaskForInside, cl_[i], cl_[i + 1], cv::Scalar(0), maskLine);
				//cv::line(righteyeMaskForInside, cr_[i], cr_[i + 1], cv::Scalar(0), maskLine);
				cv::line(lefteyeMask, cl_[i], cl_[i + 1], cv::Scalar(0), 4);
				cv::line(righteyeMask, cr_[i], cr_[i + 1], cv::Scalar(0), 4);
			}
		}
		////////

		// get contours inside eye
		std::vector< std::vector<cv::Point>> leftContours, rightContours;
		
		cv::findContours(leftCanny, leftContours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
		cv::findContours(rightCanny, rightContours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
		std::vector<cv::Point2d> leftInside, rightInside;

		///// add /////
		//pointsInside(lefteyeMask, &leftContours, &leftInside, leftRect.x, leftRect.y);
		//pointsInside(righteyeMask, &rightContours, &rightInside, rightRect.x, rightRect.y);
		pointsInside(lefteyeMaskForInside, &leftContours, &leftInside, leftRect.x, leftRect.y);
		pointsInside(righteyeMaskForInside, &rightContours, &rightInside, rightRect.x, rightRect.y);
		////////

		/////////////////////



		////////// calculate Iris radius/////////

		bool isBorder = false;
		// if eye-closure detected, then continue for next frame
		if (trackingData[0].eyeClosure[0]==1 && trackingData[0].eyeClosure[1]==1) {  
			if (!isNearBorder(leftPupilCV, leftRect)
				&& !isNearBorder(rightPupilCV, rightRect)) {

				int radius_t1 = 0, radius_t2 = 0;
				// first calculate
				for (int i = 0; i < leftInside.size(); i++) {
					radius_t1 += sqrt(pow((leftInside.at(i).x + leftRect.x - leftPupilCV.x), 2) +
						pow((leftInside.at(i).y + leftRect.y - leftPupilCV.y), 2));
				}
				for (int i = 0; i < rightInside.size(); i++) {
					radius_t1 += sqrt(pow((rightInside.at(i).x + rightRect.x - rightPupilCV.x), 2) +
						pow((rightInside.at(i).y + rightRect.y - rightPupilCV.y), 2));
				}
				if (rightInside.size() + leftInside.size() != 0)
					radius_t1 /= rightInside.size() + leftInside.size();

				// delete unqulified points
				int count = 0;
				for (int i = 0; i < leftInside.size(); i++) {
					float dis = sqrt(pow((leftInside.at(i).x + leftRect.x - leftPupilCV.x), 2) +
						pow((leftInside.at(i).y + leftRect.y - leftPupilCV.y), 2));
					if (dis < radius_t1 + radt1Plus && dis > radius_t1 - radt1Minus) {
						count++;
						radius_t2 += dis;
					}
				}
				for (int i = 0; i < rightInside.size(); i++) {
					float dis = sqrt(pow((rightInside.at(i).x + rightRect.x - rightPupilCV.x), 2) +
						pow((rightInside.at(i).y + rightRect.y - rightPupilCV.y), 2));
					if (dis < radius_t1 + radt1Plus && dis > radius_t1 - radt1Minus) {
						count++;
						radius_t2 += dis;
					}
				}

				if (count != 0)
					radius_t2 /= count;
				if (abs((radius_t2 - (radius- radiusAdd))) > radChangeNoLessThan) {
					radiusAdd = radius * 0.2; //0.32;
					radius = radius_t2 + radiusAdd;
				}
			}
			else {
				radius *= 0.8;
				radius += 0;
				isBorder = true;
			}

		///////////////////////////

			
		
		/////////// load lens pic and resize, set seperate lens roi(square) on face///////////
			if (isChanged) {
				_lens = cv::imread(lensPath, -1);
				isChanged = false;
			}

			std::vector<cv::Mat> bgr;
			cv::split(_lens, bgr);
			cv::Mat transChannel = bgr[3];
			//cv::imshow("transtestett", transChannel);
			bgr.pop_back();

			cv::Mat lens;
			cv::merge(bgr, lens);

			try {
				//cv::resize(lens, lens, cv::Size((int)radius * 2, (int)radius * 2), 0, 0, CV_INTER_AREA);
				
				//cv::imwrite("C:/Users/jenny/Desktop/irisTest/blacktea/blank/lens2_interArea_1.png", lens);
			}
			catch (exception err) {
				std::cout << "lens resize err" << endl;
			}
			cv::Rect leftIrisRect(leftPupilCV.x - radius, leftPupilCV.y - radius, 2 * radius, 2 * radius),
				rightIrisRect(rightPupilCV.x - radius, rightPupilCV.y - radius, 2 * radius, 2 * radius);
			if (!(0 <= leftIrisRect.x && leftIrisRect.x + leftIrisRect.width <= frame.cols && 0 <= leftIrisRect.y &&  leftIrisRect.y + leftIrisRect.height <= frame.rows) ){
				continue;
			}
			if (!(0 <= rightIrisRect.x && rightIrisRect.x + rightIrisRect.width <= frame.cols && 0 <= rightIrisRect.y &&  rightIrisRect.y + rightIrisRect.height <= frame.rows)) {
				continue;
			}
			cv::Mat leftIrisRoi(frame, leftIrisRect), rightIrisRoi(frame, rightIrisRect);

			/*
			//// add ////
			Mat colorMaskLeft, colorMaskRight;
			cvtColor(leftIrisRoi, colorMaskLeft, CV_BGR2GRAY);
			cvtColor(rightIrisRoi, colorMaskRight, CV_BGR2GRAY);
			threshold(colorMaskLeft, colorMaskLeft, 100, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
			threshold(colorMaskRight, colorMaskRight, 100, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
			imshow("th", colorMaskLeft);
			//////
			*/
			// make border on mask for the later roi cutting. The Rect x y need to add the border
			int margin = radius+10;
			cv::copyMakeBorder(lefteyeMask, lefteyeMask, margin, margin, margin, margin, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
			cv::copyMakeBorder(righteyeMask, righteyeMask, margin, margin, margin, margin, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
			//cv::imshow("mask", lefteyeMask);

			cv::Rect leftIrisMaskRect(leftIrisRect.x - leftRect.x + margin, leftIrisRect.y - leftRect.y + margin, leftIrisRect.width, leftIrisRect.height),
				rightIrisMaskRect(rightIrisRect.x - rightRect.x + margin, rightIrisRect.y - rightRect.y + margin, rightIrisRect.width, rightIrisRect.height);
			try {
				// set lens roi(square) on mask
				cv::Mat leftmaskRoi(lefteyeMask, leftIrisMaskRect), rightmaskRoi(righteyeMask, rightIrisMaskRect);
				
				/*
				//// add //// 會把放大效果的眼白處也刪掉
				for (int i = 0; i < leftmaskRoi.rows; i++) {
					for (int j = 0; j < leftmaskRoi.cols; j++) {
						if (colorMaskLeft.at<uchar>(i,j) == 255) {
							leftmaskRoi.at<uchar>(i, j) = 0;
						}
					}
				}
				for (int i = 0; i < rightmaskRoi.rows; i++) {
					for (int j = 0; j < rightmaskRoi.cols; j++) {
						if (colorMaskRight.at<uchar>(i, j) == 255) {
							rightmaskRoi.at<uchar>(i, j) = 0;
						}
					}
				}
				////
				*/

				/*
				// turn mask to black on mask roi if lens[i,j] is white(average > 250)
				//////// commented this part if you need no mask on the center of lens ///////
				cv::Mat lensGray;
				cv::cvtColor(lens, lensGray, cv::COLOR_BGR2GRAY);
				for (int i = 0; i < lens.rows; i++) {
					for (int j = 0; j < lens.cols; j++) {
						if (lensGray.at<uchar>(i, j) >= 250)
						{
							leftmaskRoi.at<uchar>(i, j) = 0;
							rightmaskRoi.at<uchar>(i, j) = 0;
						}
					}
				}
				//////////////
				*/

				cv::Mat leftMask = leftmaskRoi.clone(), rightMask = rightmaskRoi.clone();
		////////////////

				// make transparent mask				
				cv::Mat transMask;
				//cv::resize(transChannel, transMask, cv::Size((int)radius * 2, (int)radius * 2), 0, 0, CV_INTER_AREA);
				
				transMask = transChannel.clone();

				/*
				cv::Mat grayIris;
				cv::cvtColor(rightIrisRoi, grayIris, CV_BGR2GRAY);
				cv::threshold(grayIris, grayIris, 150, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

				imshow("binaryIris", transMask);
				*/

				//cv::imwrite("C:/Users/jenny/Desktop/irisTest/blacktea/blank/lens_trans2_interArea_1.png", transMask);

				//imshow("trans", transMask);

				/////////////////////////////////////

				/////// adjust the color of lens/////////

				/*
				// HLS, assimilate the L channel
				cv::Mat hls;
				cv::cvtColor(leftRoi, hls, cv::COLOR_BGR2HLS);
				cv::cvtColor(lens, lens, cv::COLOR_BGR2HLS);
				// calculate the mean v if Iris
				int v_f = 0;
				float v_l = 0;
				for (int i = 0; i < hls.rows; i++) {
					for (int j = 0; j < hls.cols; j++) {
						v_f += (int)hls.at<Vec3b>(i, j)[1];
					}
				}
				// calculate the mean L if lens  
				for (int i = 0; i < lens.rows; i++) {
					for (int j = 0; j < lens.cols; j++) {
						v_l += (float)lens.at<Vec3b>(i, j)[1] /lens.rows / lens.cols;
					}
				}
				v_f /= hls.rows*hls.cols;

				//std::cout << "v_f" << v_f << endl << "v_l " << v_l<< endl;
				float fac = (float)v_f / v_l;
				//std::cout << fac << endl;
				// multiple fac
				for (int i = 0; i < lens.rows; i++) {
					for (int j = 0; j < lens.cols; j++) {
						lens.at<Vec3b>(i, j)[1] *= fac ;
						lens.at<Vec3b>(i, j)[1] += 20;
					}
				}
				

				//////// add to the origin frame with transparency /////////
				cv::cvtColor(lens, lens, cv::COLOR_HLS2BGR); 
				*/
				// by .mul and transMask
				std::vector<Mat> trans;
				
				trans.push_back(transMask);
				trans.push_back(transMask);
				trans.push_back(transMask);
				merge(trans, transMask);
				multiply(lens, transMask, lens, 1 / 255.0);
				imshow("lenstran__", lens);




				//left eye
				Mat buffer = leftIrisRoi.clone();

				resize(buffer, buffer, Size(1050, 1050));
				resize(leftMask, leftMask, Size(1050, 1050));


				Mat full(buffer.cols, buffer.rows, CV_8UC3, Scalar(255, 255, 255));

				cv::Mat transbufffer;
				transMask.copyTo(transbufffer, leftMask);
				multiply(buffer, full - transbufffer, buffer, 1/255.0);
				//imshow("lenstran", buffer);

				cv::add(buffer, lens, buffer, leftMask);
				resize(buffer, buffer, Size(500, 500));
				imshow("f", buffer);
				resize(buffer, buffer, Size(leftIrisRoi.cols, leftIrisRoi.rows), 0,0, CV_INTER_AREA);

				leftIrisRoi = leftIrisRoi * 0 + buffer * 1;
				cv::GaussianBlur(leftIrisRoi, leftIrisRoi, cv::Size(3, 3), 1);

				
				// right eye
				buffer = rightIrisRoi.clone();

				resize(buffer, buffer, Size(1050, 1050));
				resize(rightMask, rightMask, Size(1050, 1050));
				transMask.copyTo(transbufffer, rightMask);
				cv::multiply(buffer, full - transbufffer, buffer, 1 / 255.0);
				cv::add(buffer, lens, buffer, rightMask);
				resize(buffer, buffer, Size(rightIrisRoi.cols, rightIrisRoi.rows), 0, 0, CV_INTER_AREA);

				rightIrisRoi = rightIrisRoi*0 + buffer*1;
				cv::GaussianBlur(rightIrisRoi, rightIrisRoi, cv::Size(3, 3), 1);
				
				Mat lShow(frame, leftRect), rShow(frame, rightRect);
				cv::imshow("ll", lShow);
				cv::imshow("rr", rShow);
				if (isBorder) {
					radius /= 0.8;
				}
			}
			catch (exception err) {
				//std::cout << "lensGrayMask or add err"<< endl;
			}
		}

		imwrite("C:/Users/jenny/Desktop/irisTest/blacktea/blank/testHLS_3.png", frame);
		//writer.write(frame);

		resize(frame, frame, Size(550, 880));  // 480 680
		cv::imshow("video", frame);
		cv::waitKey(wait);
		//cv::waitKey(0);
		//break;
	}
	return 0;
}

// translation between coordinates of sdk and opencv 
int* vis2cv_AxisTransfer(const float* pos, int width, int height) {
	//cout << pos[0] << " " << pos[1]<<endl;
	int x = pos[0] * width;
	int y = pos[1] * height;
	y = height - y;
	int ans[2] = { x, y };
	return ans;
}

// collect contour points inside the eyeMask (input *contours and output *insidePoints)
void pointsInside(cv::Mat eyeMask, std::vector< std::vector<cv::Point>>* contours, std::vector<cv::Point2d>* insidePoints, int defx, int defy) {
	for(int i=0; i<contours->size(); i++)
	{
		for (int j = 0; j < contours->at(i).size(); j++) {
			cv::Point t = contours->at(i).at(j);
			if (eyeMask.at<uchar>(t.y, t.x) == 255) {
				insidePoints->push_back(t);
			}
		}
	}
}

bool isNearBorder(cv::Point point, cv::Rect rect) {
	int xborder = rect.width / borderFactorx;  //12
	int yborder = rect.height / borderFactory;  //5
	//cout << xborder << " " << yborder<< endl;
	return point.x - rect.x < xborder || rect.x + rect.width - point.x < xborder
		|| point.y - rect.y < yborder || rect.y + rect.height - point.y < yborder;
}

float pointsDistance(int* cvCoor, cv::Point point) {
	return sqrt(pow((cvCoor[0] - point.x), 2) + pow((cvCoor[1] - point.y), 2));
}

float pointsDistance(cv::Point2d p1, cv::Point p2) {
	return sqrt(pow((p1.x - p2.x), 2) + pow((p1.y - p2.y), 2));
}

void resize(Mat frame) {

	for (int i = 0; i < sizeof(resizeFactors) / sizeof(resizeFactors[0]); i++) {
		if (frame.rows % resizeFactors[i] == 0 && frame.cols%resizeFactors[i] == 0) {
			resizef = resizeFactors[i];
			break;
		}
	}
	std::cout << resizef;
}

void hsl(Mat frame) {
	Mat roi;
	cv::cvtColor(frame, roi, CV_BGR2HLS);
	std::vector<Mat> hls;
	cv::split(roi, hls);
	cv::imshow("leee", hls[1]);

}