// myEyeDemo_visageSDK.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "visageVision.h"
#include "opencv2\opencv.hpp"
#include "IrisTracker.h"


using namespace cv;


//namespace VisageSDK //all visage|SDK calls must be in visageSDK namespace
//{
//	int __declspec(dllimport) initializeLicenseManager(const char *licenseKeyFileFolder);
//}


int* vis2cv_AxisTransfer(const float* pos, int width, int height);
void pointsInside(cv::Mat eyeMask, std::vector< std::vector<cv::Point>>* contours, std::vector<cv::Point2d>* insidePoints, int defx, int defy);
//bool isNearBorder(cv::Point point, cv::Rect rect);
float pointsDistance(int* cvCoor, cv::Point point);
float pointsDistance(cv::Point2d p1, cv::Point p2);
Mat faceMask(VisageSDK::FaceData faceData, int width, int height, Mat frame);
Mat face2(Mat image);
void makeup(VisageSDK::FaceData faceData, int width, int height, Mat frame);

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
	VisageSDK::VisageTracker tracker("../visageSDK/Samples/data/Facial Features Tracker - High.cfg");
	VisageSDK::VisageFaceAnalyser *analyser = new VisageSDK::VisageFaceAnalyser();
	analyser->init("../visageSDK/Samples/data/bdtsdata/LBF/vfadata");
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

	int radius_l = 0;
	int radius_r = 0;
	cv::Mat _lens;
	bool isChanged = true;
	VsImage* vsFrame;
	cv::Mat frame;
	cv::VideoCapture cap("D:\\eye-demo\\myEyeDemo_visageSDK\\IMG_0093.MOV");
	cap.read(frame);

	if (!cap.isOpened())
		return 1;

	IrisTracker irisTracker;
	irisTracker.initialize("../visageSDK/Samples/data/Facial Features Tracker - High.cfg",
		"C:\\Users\\jenny\\Desktop\\eye-demo\\pudding_blank.png", "../visageSDK-Windows-64bit_v8.5/visageSDK/Samples/data/bdtsdata/LBF/vfadata", 25);
	

	//////////////

	/*
	for (int i = 0; i < 30 * 10; i++) {
		cap.read(frame);
	}
	for (int i = 0; i < 30 * 15; i++) {
		cap.read(frame);
	}
	*/
	///// IrisTracker testing loop //////
	while(true)
	{
		cap.read(frame);
		if (frame.empty()) {
			break;
		}

		cv::Mat output;
		int stat = irisTracker.irisTrack(frame, output);
		if (stat == 0) {
			cv::resize(output, output, cv::Size(400, 500));
			cv::imshow("output", output);
		}
	}

	///////////

	
	/////// Video output ///////
	if (isVertical)
		transpose(frame, frame);
	cv::Size size( frame.cols*2, frame.rows);
	VideoWriter writer;
	//cout << size.height <<endl;
	writer.open("origin.avi", CV_FOURCC('M', 'J', 'P', 'G'), 30, size);
	
	/*
	for (int i = 0; i < 100; i++) {
		cap.read(frame);
		transpose(frame, frame);
		transpose(frame, frame);

		waitKey(5);
		writer.write(frame);
	}
	*/


	while (false) {
		double start = static_cast<double>(getTickCount());
		////// read frame and track face by sdk ///////
		cap.read(frame);   
		if(isVertical)
			transpose(frame, frame);

		/*
		Mat dst(frame.rows, frame.cols * 2, CV_8UC3, Scalar(0,0,0));
		Mat te;
		te = dst.colRange(0, frame.cols);
		frame.copyTo(te);
		*/
		//cv::resize(te, te, Size(480, 680));
		//imshow("origin", te);

		int a;
		IplImage *ipl = &IplImage(frame);
		
		const char* dat = (char*)frame.data;

		int* stat;
		stat = tracker.track(frame.cols, frame.rows, dat, trackingData, VISAGE_FRAMEGRABBER_FMT_BGR);
		cv::waitKey(trackerDelay);		// tracker may be too slow to be real time
		
		if (stat[0] != TRACK_STAT_OK) {
			std::cout << "tracker err" << stat[0] << endl;
			continue;
		}
		/////////////////////

		//faceMask(trackingData[0], frame.cols, frame.rows, frame);
		//frame = face2(frame.clone());
		//makeup(trackingData[0], frame.cols, frame.rows, frame);

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
		leftPupilCV = cv::Point(cvCoor[0], cvCoor[1]);

		//right pupil
		pos = trackingData[0].featurePoints2D->getFPPos(rightPupil);
		cvCoor = vis2cv_AxisTransfer(pos, ipl->width, ipl->height);
		rightPupilCV = cv::Point(cvCoor[0], cvCoor[1]);


		// get eyesquare roi 
		cv::Rect leftRect = cv::boundingRect(lefteyeCV);
		cv::Rect rightRect = cv::boundingRect(righteyeCV);
		if (leftRect.width == 1 || rightRect.width == 1) {
			std::cout << "bounding box err";
			continue;
		}
		if (!(0 <= leftRect.x && leftRect.x + leftRect.width <= frame.cols && 0 <= leftRect.y &&  leftRect.y + leftRect.height <= frame.rows)) {
			continue;
			cout << "a";
		}
		if (!(0 <= rightRect.x && rightRect.x + rightRect.width <= frame.cols && 0 <= rightRect.y &&  rightRect.y + rightRect.height <= frame.rows)) {
			continue;
			cout << "b";

		}
		cv::Mat leftRoi(frame, leftRect);
		cv::Mat rightRoi(frame, rightRect);


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
		// shrink the mask for function "pointsInside()"

		////// add (change) shrink the mask //////
		// lower
		for (int i = 4; i < 8; i++) {
			if (i == 7) {
				cv::line(lefteyeMask, cl_[i], cl_[0], cv::Scalar(0), maskLine);
				cv::line(righteyeMask, cr_[i], cr_[0], cv::Scalar(0), maskLine);
			}
			else {
				cv::line(lefteyeMask, cl_[i], cl_[i + 1], cv::Scalar(0), maskLine);
				cv::line(righteyeMask, cr_[i], cr_[i + 1], cv::Scalar(0), maskLine);
			}
		}

		// upper
		for (int i = 0; i < 4; i++) {
			if (i == 7) {
				cv::line(lefteyeMask, cl_[i], cl_[0], cv::Scalar(0), 3);
				cv::line(righteyeMask, cr_[i], cr_[0], cv::Scalar(0), 3);
			}
			else {
				cv::line(lefteyeMask, cl_[i], cl_[i + 1], cv::Scalar(0), 3);
				cv::line(righteyeMask, cr_[i], cr_[i + 1], cv::Scalar(0), 3);
			}
		}
		////////


		// if eye-closure detected, then continue for next frame
		if (trackingData[0].eyeClosure[0]==1 && trackingData[0].eyeClosure[1]==1) {  
			
			if (trackingData[0].irisRadius[0] != -1 && trackingData[0].irisRadius[1] != -1) {
				if (abs(trackingData[0].irisRadius[0] - radius_l) > 1) {
					radius_l = round(trackingData[0].irisRadius[0]);
					radius_l *= 1.3;
				}
				if (abs(trackingData[0].irisRadius[1] - radius_r) > 1) {
					radius_r = round(trackingData[0].irisRadius[1]);
					radius_r *= 1.3;
				}
			}

		///////////////////////////

			
		
		/////////// load lens pic and resize, set seperate lens roi(square) on face///////////
			if (isChanged) {
				_lens = cv::imread(lensPath, -1);
				isChanged = false;
			}

			//imshow("len", _lens);
			std::vector<cv::Mat> bgr(4);
			cv::split(_lens, bgr);

			cv::Mat transChannel = bgr[3];
			bgr.pop_back();

			cv::Mat lens;
			cv::merge(bgr, lens);
			
			cv::Rect leftIrisRect(leftPupilCV.x - radius_l , leftPupilCV.y - radius_l, 2 * radius_l, 2 * radius_l),
				rightIrisRect(rightPupilCV.x - radius_r, rightPupilCV.y - radius_r, 2 * radius_r, 2 * radius_r);


			if (!(0 <= leftIrisRect.x && leftIrisRect.x + leftIrisRect.width <= frame.cols && 0 <= leftIrisRect.y &&  leftIrisRect.y + leftIrisRect.height <= frame.rows) ){
				continue;
				cout << "c";

			}
			if (!(0 <= rightIrisRect.x && rightIrisRect.x + rightIrisRect.width <= frame.cols && 0 <= rightIrisRect.y &&  rightIrisRect.y + rightIrisRect.height <= frame.rows)) {
				continue;
				cout << "d";

			}
			cv::Mat leftIrisRoi(frame, leftIrisRect), rightIrisRoi(frame, rightIrisRect);

			// make border on mask for the later roi cutting. The Rect x y need to add the border
			int margin = radius_l + 10;
			cv::copyMakeBorder(lefteyeMask, lefteyeMask, margin, margin, margin, margin, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
			
			margin = radius_r + 10;
			cv::copyMakeBorder(righteyeMask, righteyeMask, margin, margin, margin, margin, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
			//cv::imshow("mask", lefteyeMask);

			cv::Rect leftIrisMaskRect(leftIrisRect.x - leftRect.x + margin, leftIrisRect.y - leftRect.y + margin, leftIrisRect.width, leftIrisRect.height),
				rightIrisMaskRect(rightIrisRect.x - rightRect.x + margin, rightIrisRect.y - rightRect.y + margin, rightIrisRect.width, rightIrisRect.height);
			try {
				// set lens roi(square) on mask
				cv::Mat leftmaskRoi(lefteyeMask, leftIrisMaskRect), rightmaskRoi(righteyeMask, rightIrisMaskRect);
				cv::Mat leftMask = leftmaskRoi.clone(), rightMask = rightmaskRoi.clone();

				// make transparent mask				
				cv::Mat transMask;
				
				transMask = transChannel.clone();


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
				//imshow("lenstran__", lens);


				//left eye
				cv::Mat transbufffer;
				Mat buffer = leftIrisRoi.clone();

				resize(buffer, buffer, Size(1050, 1050), CV_INTER_CUBIC);
				resize(leftMask, leftMask, Size(1050, 1050), CV_INTER_CUBIC);
				Mat full(buffer.cols, buffer.rows, CV_8UC3, Scalar(255, 255, 255));

				transMask.copyTo(transbufffer, leftMask);
				multiply(buffer, full - transbufffer, buffer, 1/255.0);

				cv::add(buffer, lens, buffer, leftMask);
				resize(buffer, buffer, Size(500, 500));
				resize(buffer, buffer, Size(leftIrisRoi.cols, leftIrisRoi.rows), 0,0, CV_INTER_AREA);

				leftIrisRoi = leftIrisRoi * 0 + buffer * 1;
				cv::GaussianBlur(leftIrisRoi, leftIrisRoi, cv::Size(3, 3), 1);

				
				// right eye
				buffer = rightIrisRoi.clone();
				transbufffer = cv::Mat(transMask.rows, transMask.cols, CV_8UC1, cv::Scalar(0));

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
			}
			catch (exception err) {
				//cout << "e";

				//std::cout << "lensGrayMask or add err"<< endl;
			}
		}

		double time = ((double)getTickCount() - start) / getTickFrequency();
		//cout << " " <<time ;
		/*
		Mat te2;
		te2 = dst.colRange(frame.cols, dst.cols);
		frame.copyTo(te2);
		*/
		imwrite("origin eye.png", frame);

		resize(frame, frame, Size(480, 680));  // 480 680
		cv::imshow("video", frame);
		//writer.write(dst);
		break;
		cv::waitKey(wait);
	}
	return 0;
}

// translation between coordinates of sdk and opencv 
int* vis2cv_AxisTransfer(const float* pos, int width, int height) {
	//cout << pos[0] << " " << pos[1]<<endl;
	int x = pos[0] * width;
	int y = pos[1] * height;
	y = height - y;
	if (x < 0)
		x = 0;
	if (y < 0)
		y = 0;
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

Mat faceMask(VisageSDK::FaceData faceData, int width, int height, Mat frame) {
	// 8
	const char* lefteyePoints[] = { "3.11", "12.9", "3.1", "12.5", "3.7", "12.7", "3.3", "12.11" };  //8 points
	const char* righteyePoints[] = { "3.12", "12.10", "3.2", "12.6", "3.8", "12.8", "3.4", "12.12" };

	// 10  8
	const char* mouthPoints_outer[] = { "8.4", "8.6", "8.9", "8.1", "8.10", "8.5", "8.3", "8.7", "8.2", "8.8" };
	const char* mouthPoints_inner[] = { "2.5", "2.7", "2.2", "2.6", "2.4", "2.8", "2.3", "2.9" };

	const char* leftBrowPoints[] = { "4.6", "14.4", "4.4", "14.2", "4.2"  };  //5 points
	const char* rightBrowPoints[] = { "4.1", "14.1", "4.3", "14.3", "4.5" };

	// 20
	//const char* facePoints[] = {"10.10", "11.2", "11.1", "11.3", "10.9", "10.7", "2.13", "2.11", "2.1", "2.12", "2.14"};
	const char* facePoints[] = { "13.2", "13.4", "13.6", "13.8", "13.10", "13.12", "13.14", "13.16", "13.17", "13.15", "13.13"
		, "13.11", "13.9", "13.7", "13.5", "13.3", "13.1" , "11.3", "11.1", "11.2"};

	cv::Point lefteye[8], righteye[8], mouth_outer[10], mouth_inner[8], face[20], leftBrow[5], rightBrow[5];
	
	
	Mat mask;
	cvtColor(frame, mask, CV_BGR2YCrCb);
	std::vector<Mat> ycrcb;
	split(mask, ycrcb);
	threshold(ycrcb[2], mask, 100, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);
	Mat erodeStruct = getStructuringElement(MORPH_RECT, Size(7, 7));
	morphologyEx(mask, mask, MORPH_OPEN, erodeStruct);
	//imshow("bb", mask);
	

	//Mat mask(height, width, CV_8UC1, Scalar(0));

	
	//face
	for (int i = 0; i < 20; i++) {
		const float* pos = faceData.featurePoints2D->getFPPos(facePoints[i]);
		int* cvcoor = vis2cv_AxisTransfer(pos, width, height);
		if(i == 18)
			cout << cvcoor[0] << " " << cvcoor[1] << endl;
		face[i] = Point(cvcoor[0], cvcoor[1]);
	}
	//cv::fillConvexPoly(mask, face, 20, cv::Scalar(255));

	// eye mask
	for (int i = 0; i < 8; i++) {
		const float* pos = faceData.featurePoints2D->getFPPos(lefteyePoints[i]);
		int* cvcoor = vis2cv_AxisTransfer(pos, width, height);
		lefteye[i] = Point(cvcoor[0], cvcoor[1]);
		const float* pos2 = faceData.featurePoints2D->getFPPos(righteyePoints[i]);
		cvcoor = vis2cv_AxisTransfer(pos2, width, height);
		righteye[i] = Point(cvcoor[0], cvcoor[1]);
	}

	cv::fillConvexPoly(mask, lefteye, 8, cv::Scalar(0));
	cv::fillConvexPoly(mask, righteye, 8, cv::Scalar(0));

	
	// mouth
	for (int i = 0; i < 10; i++) {
		const float* pos = faceData.featurePoints2D->getFPPos(mouthPoints_outer[i]);
		int* cvcoor = vis2cv_AxisTransfer(pos, width, height);
		mouth_outer[i] = Point(cvcoor[0], cvcoor[1]);
	}
	cv::fillConvexPoly(mask, mouth_outer, 10, cv::Scalar(0));

	for (int i = 0; i < 10; i++) {
		const float* pos = faceData.featurePoints2D->getFPPos(mouthPoints_inner[i]);
		int* cvcoor = vis2cv_AxisTransfer(pos, width, height);
		mouth_inner[i] = Point(cvcoor[0], cvcoor[1]);
	}
	cv::fillConvexPoly(mask, mouth_inner, 8, cv::Scalar(255));
	
	// eyebrow
	for (int i = 0; i < 5; i++) {
		const float* pos = faceData.featurePoints2D->getFPPos(leftBrowPoints[i]);
		int* cvcoor = vis2cv_AxisTransfer(pos, width, height);
		leftBrow[i] = Point(cvcoor[0], cvcoor[1]);

		const float* pos2 = faceData.featurePoints2D->getFPPos(rightBrowPoints[i]);
		cvcoor = vis2cv_AxisTransfer(pos2, width, height);
		rightBrow[i] = Point(cvcoor[0], cvcoor[1]);
	}
	cv::fillConvexPoly(mask, leftBrow, 5, cv::Scalar(0));
	cv::fillConvexPoly(mask, rightBrow, 5, cv::Scalar(0));


	// set Roi
	Mat test = mask.clone();
	cv::resize(test, test, Size(500, 500));

	std::vector<cv::Point> faceVector(std::begin(face), std::end(face));
	Rect roiRect = boundingRect(faceVector);

	Mat maskRoi(mask, roiRect), frameRoi(frame, roiRect);
	imshow("mask", maskRoi);
	/*   shapen did not make the margin more clear, but makes a strange margin in the margin of hair
	float k[] = { 0, -1, 0, -1 , 5, -1, 0, -1, 0 };
	Mat kernel(3, 3, CV_32F, k);
	imshow("kernel", kernel);
	filter2D(frameRoi, frameRoi, CV_8UC3, kernel);
	*/
	Mat blurbuffer2 = frameRoi.clone();
	Mat blurbuffer;
	for (int i = 0; i < 7; i++) {
		//GaussianBlur(blurbuffer, blurbuffer, Size(5,5), 1);
		bilateralFilter(blurbuffer2, blurbuffer, 5, 200, 150);
		blurbuffer2 = blurbuffer.clone();
	}
	
	for (int i = 0; i < frameRoi.rows; i++) {
		uchar* frameptr = frameRoi.ptr<uchar>(i);
		const uchar* maskptr = maskRoi.ptr<uchar>(i);
		const uchar* blurptr = blurbuffer.ptr<uchar>(i);
		for (int j = 0; j < frameRoi.cols*3; j+=3) {
			if (maskptr[j/3] == 255) {
				frameptr[j] = blurptr[j];
				frameptr[j+1] = blurptr[j+1];
				frameptr[j+2] = blurptr[j+2];
			}
		}
	}

	//GaussianBlur(frameRoi, frameRoi, Size(3, 3), 1);

	imshow("beautymask", mask);

	return mask;
}

Mat face2(Mat image) {
	Mat dst;

	// int value1 = 3, value2 = 1; 磨皮程度與細節程度
	int value1 = 3, value2 = 1;
	int dx = 15; 
	double fc = value1 * 12.5; 
	double p = 0.0f; // 透明度
	Mat temp1, temp2, temp3, temp4;

	// 雙邊濾波
	bilateralFilter(image, temp1, dx, fc, fc);

	// temp2 = (temp1 - image + 128);
	Mat temp22;
	subtract(temp1, image, temp22);
	// Core.subtract(temp22, new Scalar(128), temp2);
	add(temp22, Scalar(128, 128, 128, 128), temp2);
	// 高斯模糊

	//GaussianBlur(temp2, temp3, Size(2 * value2 - 1, 2 * value2 - 1), 0, 0);
	temp3 = temp2;

	// temp4 = image + 2 * temp3 - 255;
	Mat temp44;
	//temp3.convertTo(temp44, temp3.type(), 4, -510);
	temp3.convertTo(temp44, temp3.type(), 2, -256);

	add(image, temp44, temp4);
	// dst = (image*(100 - p) + temp4*p) / 100;
	addWeighted(image, p, temp4, 1 - p, 0.0, dst);

	add(dst, Scalar(10, 10, 10), dst);
	return dst;

}

void makeup(VisageSDK::FaceData faceData, int width, int height, Mat frame) {
	// 8
	const char* lefteyePoints[] = { "3.11", "12.9", "3.1", "12.5", "3.7", "12.7", "3.3", "12.11" };  //8 points
	const char* righteyePoints[] = { "3.12", "12.10", "3.2", "12.6", "3.8", "12.8", "3.4", "12.12" };

	// 10  8
	const char* mouthPoints_outer[] = { "8.4", "8.6", "8.9", "8.1", "8.10", "8.5", "8.3", "8.7", "8.2", "8.8" };
	const char* mouthPoints_inner[] = { "2.5", "2.7", "2.2", "2.6", "2.4", "2.8", "2.3", "2.9" };

	const char* leftBrowPoints[] = { "4.6", "14.4", "4.4", "14.2", "4.2" };  //5 points
	const char* rightBrowPoints[] = { "4.1", "14.1", "4.3", "14.3", "4.5" };

	// 20
	//const char* facePoints[] = {"10.10", "11.2", "11.1", "11.3", "10.9", "10.7", "2.13", "2.11", "2.1", "2.12", "2.14"};
	const char* facePoints[] = { "13.2", "13.4", "13.6", "13.8", "13.10", "13.12", "13.14", "13.16", "13.17", "13.15", "13.13"
		, "13.11", "13.9", "13.7", "13.5", "13.3", "13.1" , "11.3", "11.1", "11.2" };

	cv::Point lefteye[8], righteye[8], mouth_outer[10], mouth_inner[8], face[20], leftBrow[5], rightBrow[5];


	Mat mask;
	cvtColor(frame, mask, CV_BGR2YCrCb);
	std::vector<Mat> ycrcb;
	split(mask, ycrcb);
	threshold(ycrcb[2], mask, 100, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);
	Mat erodeStruct = getStructuringElement(MORPH_RECT, Size(7, 7));
	morphologyEx(mask, mask, MORPH_OPEN, erodeStruct);
	//imshow("bb", mask);


	//Mat mask(height, width, CV_8UC1, Scalar(0));


	//face
	for (int i = 0; i < 20; i++) {
		const float* pos = faceData.featurePoints2D->getFPPos(facePoints[i]);
		int* cvcoor = vis2cv_AxisTransfer(pos, width, height);
		if (i == 18)
			cout << cvcoor[0] << " " << cvcoor[1] << endl;
		face[i] = Point(cvcoor[0], cvcoor[1]);
	}
	//cv::fillConvexPoly(mask, face, 20, cv::Scalar(255));

	// eye mask
	for (int i = 0; i < 8; i++) {
		const float* pos = faceData.featurePoints2D->getFPPos(lefteyePoints[i]);
		int* cvcoor = vis2cv_AxisTransfer(pos, width, height);
		lefteye[i] = Point(cvcoor[0], cvcoor[1]);
		const float* pos2 = faceData.featurePoints2D->getFPPos(righteyePoints[i]);
		cvcoor = vis2cv_AxisTransfer(pos2, width, height);
		righteye[i] = Point(cvcoor[0], cvcoor[1]);
	}

	cv::fillConvexPoly(mask, lefteye, 8, cv::Scalar(0));
	cv::fillConvexPoly(mask, righteye, 8, cv::Scalar(0));


	// mouth
	for (int i = 0; i < 10; i++) {
		const float* pos = faceData.featurePoints2D->getFPPos(mouthPoints_outer[i]);
		int* cvcoor = vis2cv_AxisTransfer(pos, width, height);
		mouth_outer[i] = Point(cvcoor[0], cvcoor[1]);
	}
	cv::fillConvexPoly(mask, mouth_outer, 10, cv::Scalar(0));

	for (int i = 0; i < 8; i++) {
		const float* pos = faceData.featurePoints2D->getFPPos(mouthPoints_inner[i]);
		int* cvcoor = vis2cv_AxisTransfer(pos, width, height);
		mouth_inner[i] = Point(cvcoor[0], cvcoor[1]);
	}
	cv::fillConvexPoly(mask, mouth_inner, 8, cv::Scalar(255));

	// eyebrow
	for (int i = 0; i < 5; i++) {
		const float* pos = faceData.featurePoints2D->getFPPos(leftBrowPoints[i]);
		int* cvcoor = vis2cv_AxisTransfer(pos, width, height);
		leftBrow[i] = Point(cvcoor[0], cvcoor[1]);

		const float* pos2 = faceData.featurePoints2D->getFPPos(rightBrowPoints[i]);
		cvcoor = vis2cv_AxisTransfer(pos2, width, height);
		rightBrow[i] = Point(cvcoor[0], cvcoor[1]);
	}
	cv::fillConvexPoly(mask, leftBrow, 5, cv::Scalar(0));
	cv::fillConvexPoly(mask, rightBrow, 5, cv::Scalar(0));


	// set Roi
	Mat test = mask.clone();
	cv::resize(test, test, Size(500, 500));

	std::vector<cv::Point> mouthVector(std::begin(mouth_outer), std::end(mouth_outer));
	Rect roiRect = boundingRect(mouthVector);

	Mat maskRoi(mask, roiRect), frameRoi(frame, roiRect);
	imshow("mask", maskRoi);
	
	cvtColor(frameRoi, frameRoi, CV_BGR2HSV);
	for (int i = 0; i < frameRoi.rows; i++) {
		unsigned char* framePtr = frameRoi.ptr<uchar>(i);
		unsigned char* maskPtr = maskRoi.ptr<uchar>(i);

		int t = 10;
		for (int j = 0; j < frameRoi.cols * 3; j+=3) {
			if (maskPtr[j / 3] == 0) {
				if (framePtr[j + 1] + t > 255) {
					framePtr[j + 1] = 255;
				}
				else {
					framePtr[j + 1] += t;
				}
			}
		}
	}
	cvtColor(frameRoi, frameRoi, CV_HSV2BGR);
}