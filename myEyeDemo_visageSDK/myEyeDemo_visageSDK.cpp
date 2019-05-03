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
void resize(Mat frame);
void hsl(Mat roi);


////// ipad setting//////  
//frame   // 1280*720
int resizeFactors[3] = {3,2,1};
int resizef;
bool isVertical = false;
int trackerDelay = 10;
// Gaussian blur
int blurCount = 4;
int blurKernelSize = 3;
//Canny
int cannyp1 = 60;
int cannyp2 = 100;
//MaskForInside_preprocess line type
int maskLine = 2;
//Iris radius_second calculation bounding, all positive number
int radt1Plus = 2;  //3
int radt1Minus = 2;  // 1
int radChangeNoLessThan = 4;
int borderFactorx = 5;
int borderFactory = 4;
//lens
const cv::String lensPath = "C:\\Users\\jenny\\Desktop\\eye-demo\\pudding.png";
float lensTransparentWeight = 0.4;

int wait = 20-trackerDelay;


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
	cv::VideoCapture cap("D:\\eye-demo\\myEyeDemo_visageSDK\\video-1554968291.mp4");
	cap.read(frame);

	if (!cap.isOpened())
		return 1;

	IrisTracker irisTracker;
	irisTracker.initialize("../visageSDK-Windows-64bit_v8.5/visageSDK/Samples/data/Facial Features Tracker - High.cfg",
		"C:\\Users\\jenny\\Desktop\\eye-demo\\lens-2.png", "../visageSDK-Windows-64bit_v8.5/visageSDK/Samples/data/bdtsdata/LBF/vfadata", 25);
	

	//////////////


	///// IrisTracker testing loop //////
	while(false)
	{
		cap.read(frame);
		if (frame.empty()) {
			break;
		}
		Mat mask[2];
		Point pupil[2];
		int radius = 0;
		int stat = irisTracker.irisTrack(frame, mask, pupil, &radius);
		if (stat == 0) {
			cv::imshow("m", mask[0]);
			cv::imshow("m2", mask[1]);
		}
	}

	///////////

	/*
	/////// Video output ///////
	cv::Size size(frame.rows, frame.cols);
	VideoWriter writer;
	cout << size.height <<endl;
	writer.open("testt.avi", CV_FOURCC('M', 'J', 'P', 'G'), 20, size);
	
	
	for (int i = 0; i < 100; i++) {
		cap.read(frame);
		transpose(frame, frame);
		transpose(frame, frame);

		waitKey(5);
		writer.write(frame);
	}
	*/

	while (true) {
		// read frame and track face by sdk
		//cap.read(frame);   
		frame = cv::imread("C:/Users/jenny/Desktop/eye-demo/1.jpg");
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
		if (leftPupilCV.x == 0 || pointsDistance(cvCoor, leftPupilCV) > 0) {
			leftPupilCV = cv::Point(cvCoor[0], cvCoor[1]);
		}
		//right pupil
		pos = trackingData[0].featurePoints2D->getFPPos(rightPupil);
		cvCoor = vis2cv_AxisTransfer(pos, ipl->width, ipl->height);
		if (rightPupilCV.x == 0 || pointsDistance(cvCoor, rightPupilCV) > 0) {
			rightPupilCV = cv::Point(cvCoor[0], cvCoor[1]);
		}



		// get eyesquare roi 
		cv::Rect leftRect = cv::boundingRect(lefteyeCV);
		cv::Rect rightRect = cv::boundingRect(righteyeCV);
		if (leftRect.width == 1 || rightRect.width == 1) {
			std::cout << "bounding box err";
			continue;
		}
		cv::Mat leftRoi(frame, leftRect);
		cv::Mat rightRoi(frame, rightRect);
		
		//cv::imshow("left", leftRoi);
		//cv::imshow("right", rightRoi);
		//hsl(leftRoi);

		// Blur and get canny
		for (int i = 0; i < blurCount; i++) {
			cv::GaussianBlur(leftRoi, leftRoi, cv::Size(blurKernelSize, blurKernelSize), 1);
			cv::GaussianBlur(rightRoi, rightRoi, cv::Size(blurKernelSize, blurKernelSize), 1);
		}
		cv::Mat leftCanny, rightCanny;
		cv::Canny(leftRoi, leftCanny, cannyp1, cannyp2);
		cv::Canny(rightRoi, rightCanny, cannyp1, cannyp2);
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
		for (int i = 0; i < 8; i++) {
			if (i == 7) {
				cv::line(lefteyeMaskForInside, cl_[i], cl_[0], cv::Scalar(0), maskLine);
				cv::line(righteyeMaskForInside, cr_[i], cr_[0], cv::Scalar(0), maskLine);
			}
			else {
				cv::line(lefteyeMaskForInside, cl_[i], cl_[i + 1], cv::Scalar(0), maskLine);
				cv::line(righteyeMaskForInside, cr_[i], cr_[i + 1], cv::Scalar(0), maskLine);
			}
		}

		// get contours inside eye
		std::vector< std::vector<cv::Point>> leftContours, rightContours;
		
		cv::findContours(leftCanny, leftContours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
		cv::findContours(rightCanny, rightContours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
		std::vector<cv::Point2d> leftInside, rightInside;

		pointsInside(lefteyeMaskForInside, &leftContours, &leftInside, leftRect.x, leftRect.y);
		pointsInside(righteyeMaskForInside, &rightContours, &rightInside, rightRect.x, rightRect.y);
		
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
				if (abs((radius_t2 - radius)) > radChangeNoLessThan) {
					radius = radius_t2;
				}
			}
			else {
				radius *= 0.8;
				isBorder = true;
			}

		///////////////////////////

			
		
		/////////// load lens pic and resize, set seperate lens roi(square) on face///////////
			if (isChanged) {
				_lens = cv::imread(lensPath);
				isChanged = false;
			}
			cv::Mat lens;
			try {
				cv::resize(_lens, lens, cv::Size((int)radius * 2, (int)radius * 2), 0, 0, CV_INTER_AREA);
				cv::imwrite("C:/Users/jenny/Desktop/irisTest/pudding/lens2_interArea_1.png", lens);
			}
			catch (exception err) {
				std::cout << "lens resize err" << endl;
			}
			cv::Rect leftIrisRect(leftPupilCV.x - radius, leftPupilCV.y - radius, 2 * radius, 2 * radius),
				rightIrisRect(rightPupilCV.x - radius, rightPupilCV.y - radius, 2 * radius, 2 * radius);
			cv::Mat leftIrisRoi(frame, leftIrisRect), rightIrisRoi(frame, rightIrisRect);

			// make border on mask for the later roi cutting. The Rect x y need to add the border
			int margin = radius+10;
			cv::copyMakeBorder(lefteyeMask, lefteyeMask, margin, margin, margin, margin, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
			cv::copyMakeBorder(righteyeMask, righteyeMask, margin, margin, margin, margin, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
			//cv::imshow("mask", lefteyeMask);

			cv::Rect leftIrisMaskRect(leftIrisRect.x - leftRect.x + margin, leftIrisRect.y - leftRect.y + margin, leftIrisRect.width, leftIrisRect.height),
				rightIrisMaskRect(rightIrisRect.x - rightRect.x + margin, rightIrisRect.y - rightRect.y + margin, rightIrisRect.width, rightIrisRect.height);
			//try {
				// set lens roi(square) on mask
				cv::Mat leftmaskRoi(lefteyeMask, leftIrisMaskRect), rightmaskRoi(righteyeMask, rightIrisMaskRect);
				
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


				/////// make transparency mask ///////
				cv::Mat transMask(cv::Size(301, 301), CV_8U, Scalar(0));
				cv::Mat smallLenGray;
				cv::resize(_lens, smallLenGray, cv::Size(300, 300));
				cv::cvtColor(smallLenGray, smallLenGray, cv::COLOR_BGR2GRAY);
				// find outer and inner radius
				int d_o, d_i;
				for (int i = 0; i < 151; i++) {
					if (smallLenGray.at<uchar>(i, 151) != 255) {
						std::cout << i << endl;
						//circle(smallLenGray, Point(151, i), 3, Scalar(0), -1);
						d_o = 151 - i;
						break;
					}
				}
				for (int i = 151; i > 0; i--) {
					if (smallLenGray.at<uchar>(i, 151) != 255) {
						std::cout << i << endl;
						//circle(smallLenGray, Point(151, i), 3, Scalar(0), -1);
						d_i = 151 - i -50;
						break;
					}
				}


				// make mask
				int range = (d_o - d_i) / 3;
				int white1 = 0, white2 = 0, white3 = 0;
				for (int i = 0; i < smallLenGray.rows; i++) {
					for (int j = 0; j < smallLenGray.cols; j++) {
						int radius = sqrt((i - 151)*(i - 151) + (j - 151)*(j - 151));
						if (radius > d_o) {
						}
						else if (radius > d_o - range && smallLenGray.at<uchar>(i,j) == 255) {
							white1 += 1;
						}
						else if (radius > d_o - 2*range && smallLenGray.at<uchar>(i, j) == 255) {
							white2 += 1;
						}
						else if (radius > d_o - 3 * range && smallLenGray.at<uchar>(i, j) == 255) {
							white3 += 1;
						}
					}
				}
				cout << "white" << white1 << " " << white2 << " " << white3 << " " << endl;


				for (int i = 0; i < smallLenGray.rows; i++) {
					for (int j = 0; j < smallLenGray.cols; j++) {
						int radius = sqrt((i - 151)*(i - 151) + (j - 151)*(j - 151));
						if (radius > d_o) {
						}
						else if (radius > d_o - range) {
							transMask.at<uchar>(i, j) = 120;
						}
						else if (radius > d_o - 2 * range) {
							transMask.at<uchar>(i, j) = 80;
						}
						else if (radius > d_o - 3 * range) {
							transMask.at<uchar>(i, j) = 60;
						}
						else {
							transMask.at<uchar>(i, j) = 0;
						}
					}
				}
				cv::resize(transMask, transMask, cv::Size((int)radius * 2, (int)radius * 2), 0, 0, CV_INTER_AREA);
				imshow("trans", transMask);

				/////////////////////////////////////

				/////// adjust the color of lens/////////

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

				std::cout << "v_f" << v_f << endl << "v_l " << v_l<< endl;
				float fac = (float)v_f / v_l;
				std::cout << fac << endl;
				// multiple fac
				for (int i = 0; i < lens.rows; i++) {
					for (int j = 0; j < lens.cols; j++) {
						lens.at<Vec3b>(i, j)[1] *= fac - 0.05;
						//lens.at<Vec3b>(i, j)[1] -= 10;
					}
				}
				

				//////// add to the origin frame with transparency /////////
				cv::cvtColor(lens, lens, cv::COLOR_HLS2BGR); 
				
				/*
				// by addWeight
				Mat buffer = rightIrisRoi.clone();
				cv::add(lens, rightIrisRoi, buffer, rightMask);
				cv::addWeighted(rightIrisRoi, 1 - lensTransparentWeight, buffer, lensTransparentWeight, 0, rightIrisRoi);

				buffer = leftIrisRoi.clone();
				cv::add(lens, leftIrisRoi, buffer, leftMask);
				cv::addWeighted(leftIrisRoi, 1 - lensTransparentWeight, buffer, lensTransparentWeight, 0, leftIrisRoi);
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
				Mat full(buffer.cols, buffer.rows, CV_8UC3, Scalar(255, 255, 255));

				multiply(buffer, full - transMask, buffer, 1/255.0);
				imshow("lenstran", buffer);

				cv::add(buffer, lens, buffer, leftMask);
				imshow("f", buffer);

				leftIrisRoi = leftIrisRoi * 0 + buffer * 1;

				
				// right eye
				buffer = rightIrisRoi.clone();
				cv::multiply(buffer, full - transMask, buffer, 1 / 255.0);
				cv::add(buffer, lens, buffer, rightMask);
				rightIrisRoi = rightIrisRoi*0 + buffer*1;

				Mat lShow(frame, leftRect), rShow(frame, rightRect);
				cv::imshow("ll", lShow);
				cv::imshow("rr", rShow);
				if (isBorder) {
					radius /= 0.8;
				}
			//}
			//catch (exception err) {
				//std::cout << "lensGrayMask or add err"<< endl;
			//}
		}

		imwrite("C:/Users/jenny/Desktop/irisTest/pudding/testHLS_1.png", frame);

		resize(frame, frame, Size(480, 680));
		cv::imshow("video", frame);

		cv::waitKey(wait);
		cv::waitKey(0);
		break;
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