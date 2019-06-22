#include "stdafx.h"
#include "IrisTracker.h"

void IrisTracker::initialize(const char* highCfg, const char* lenspath, const char* vfadata, int framerate)
{
	cout << highCfg << endl << lensPath << endl << vfadata << endl;
	cv::waitKey(1000);
	
	// initialize Points for comparation
	for (int i = 0; i < 8; i++) {
		lefteyeCV.push_back(cv::Point(0, 0));
		righteyeCV.push_back(cv::Point(0, 0));
	}
	leftPupilCV = cv::Point(0, 0);
	rightPupilCV = cv::Point(0, 0);
	
	// initialize license and tracker and analyser
	VisageSDK::initializeLicenseManager(".");
	
	tracker = new VisageSDK::VisageTracker(highCfg);
	analyser = new VisageSDK::VisageFaceAnalyser();
	analyser->init(vfadata);

	// initialize lensPath
	const cv::String t(lenspath);
	lensPath = &t;
	cout << *lensPath;
	_lens = cv::imread( t , -1);
	//cv::imshow("lens", _lens);

	// framerate
	wait = frameRate - trackerDelay;
	if (wait <= 0) {
		wait = 1;
	}
	
}

int IrisTracker::irisTrack(cv::Mat frame_, cv::Mat& output)
{
	cv::Mat frame = frame_.clone();
	if (isVertical)
		transpose(frame, frame);
	//imshow("ff", frame);

	IplImage *ipl = &IplImage(frame);

	int* stat;
	stat = tracker->track(ipl->width, ipl->height, ipl->imageData, trackingData, VISAGE_FRAMEGRABBER_FMT_BGR);
	cv::waitKey(trackerDelay);		// tracker may be too slow to be real time
	if (stat[0] != TRACK_STAT_OK) {
		cout << "tracker err" << " " << stat[0] << endl;
		return 2;
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
	leftPupilCV = cv::Point(cvCoor[0], cvCoor[1]);
	
	//right pupil
	pos = trackingData[0].featurePoints2D->getFPPos(rightPupil);
	cvCoor = vis2cv_AxisTransfer(pos, ipl->width, ipl->height);
	rightPupilCV = cv::Point(cvCoor[0], cvCoor[1]);


	// get eyesquare roi
	cv::Rect leftRect = cv::boundingRect(lefteyeCV);
	cv::Rect rightRect = cv::boundingRect(righteyeCV);
	if (leftRect.width == 1 || rightRect.width == 1) {
		cout << "bounding box err";
		return 3;
	}
	if (!(0 <= leftRect.x && leftRect.x + leftRect.width <= frame.cols && 0 <= leftRect.y &&  leftRect.y + leftRect.height <= frame.rows)) {
		return 3;
	}
	if (!(0 <= rightRect.x && rightRect.x + rightRect.width <= frame.cols && 0 <= rightRect.y &&  rightRect.y + rightRect.height <= frame.rows)) {
		return 3;
	}
	cv::Mat leftRoi(frame, leftRect);
	cv::Mat rightRoi(frame, rightRect);


	// draw two eye contour mask(from boundoing box roi) seperately
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


	// shrink the mask 
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
			cv::line(lefteyeMask, cl_[i], cl_[0], cv::Scalar(0), 8);
			cv::line(righteyeMask, cr_[i], cr_[0], cv::Scalar(0), 8);
		}
		else {
			cv::line(lefteyeMask, cl_[i], cl_[i + 1], cv::Scalar(0), 8);
			cv::line(righteyeMask, cr_[i], cr_[i + 1], cv::Scalar(0), 8);
		}
	}


	// calculate Iris radius 
	if (trackingData[0].eyeClosure[0] && trackingData[0].eyeClosure[1]) {  // if eye-closure detected, then continue for next frame

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

		// load lens pic and resize, set seperate lens roi(square) on face
		if (isChanged) {
			_lens = cv::imread(*lensPath, -1);
			isChanged = false;
		}


		std::vector<cv::Mat> bgr(4);
		cv::split(_lens, bgr);

		/// this line error
		cv::Mat transChannel = bgr[3]; //.clone();
		bgr.pop_back();

		cv::Mat lens;
		cv::merge(bgr, lens);

		cv::Rect leftIrisRect(leftPupilCV.x - radius_l, leftPupilCV.y - radius_l, 2 * radius_l, 2 * radius_l),
			rightIrisRect(rightPupilCV.x - radius_r, rightPupilCV.y - radius_r, 2 * radius_r, 2 * radius_r);
		if (!(0 <= leftIrisRect.x && leftIrisRect.x + leftIrisRect.width <= frame.cols && 0 <= leftIrisRect.y &&  leftIrisRect.y + leftIrisRect.height <= frame.rows)) {
			return 4;
		}
		if (!(0 <= rightIrisRect.x && rightIrisRect.x + rightIrisRect.width <= frame.cols && 0 <= rightIrisRect.y &&  rightIrisRect.y + rightIrisRect.height <= frame.rows)) {
			return 4;
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
			//cv::imshow("leftlensMask", leftMask);

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
					v_f += (int)hls.at<cv::Vec3b>(i, j)[1];
				}
			}
			// calculate the mean L if lens  
			for (int i = 0; i < lens.rows; i++) {
				for (int j = 0; j < lens.cols; j++) {
					v_l += (float)lens.at<cv::Vec3b>(i, j)[1] / lens.rows / lens.cols;
				}
			}
			v_f /= hls.rows*hls.cols;

			//std::cout << "v_f" << v_f << endl << "v_l " << v_l << endl;
			float fac = (float)v_f / v_l;
			//std::cout << fac << endl;
			// multiple fac
			for (int i = 0; i < lens.rows; i++) {
				for (int j = 0; j < lens.cols; j++) {
					lens.at<cv::Vec3b>(i, j)[1] *= fac;
					//lens.at<Vec3b>(i, j)[1] -= 10;
				}
			}


			//////// add to the origin frame with transparency /////////
			cv::cvtColor(lens, lens, cv::COLOR_HLS2BGR);
			*/
			// by .mul and transMask
			std::vector<cv::Mat> trans;

			trans.push_back(transMask);
			trans.push_back(transMask);
			trans.push_back(transMask);
			merge(trans, transMask);
			multiply(lens, transMask, lens, 1 / 255.0);
			//imshow("lenstran__", lens);

			//left eye
			cv::Mat transbuffer;
			cv::Mat buffer = leftIrisRoi.clone();
			cv::resize(buffer, buffer, cv::Size(1050, 1050), CV_INTER_CUBIC);
			cv::resize(leftMask, leftMask, cv::Size(1050, 1050), CV_INTER_CUBIC);
			cv::Mat full(buffer.cols, buffer.rows, CV_8UC3, cv::Scalar(255, 255, 255));

			transMask.copyTo(transbuffer, leftMask);
			cv::multiply(buffer, full - transbuffer, buffer, 1 / 255.0);

			cv::add(buffer, lens, buffer, leftMask);
			cv::resize(buffer, buffer, cv::Size(500, 500));
			cv::resize(buffer, buffer, cv::Size(leftIrisRoi.cols, leftIrisRoi.rows), 0, 0, CV_INTER_AREA);

			leftIrisRoi = leftIrisRoi * 0 + buffer * 1;
			cv::GaussianBlur(leftIrisRoi, leftIrisRoi, cv::Size(3, 3), 1);


			// right eye
			buffer = rightIrisRoi.clone();
			transbuffer = cv::Mat(transMask.rows, transMask.cols, CV_8UC1, cv::Scalar(0));

			cv::resize(buffer, buffer, cv::Size(1050, 1050));
			cv::resize(rightMask, rightMask, cv::Size(1050, 1050));
			transMask.copyTo(transbuffer, rightMask);
							
			cv::multiply(buffer, full - transbuffer, buffer, 1 / 255.0);

			cv::add(buffer, lens, buffer, rightMask);

			cv::resize(buffer, buffer, cv::Size(500, 500));
			imshow("test", buffer);

			resize(buffer, buffer, cv::Size(rightIrisRoi.cols, rightIrisRoi.rows), 0, 0, CV_INTER_AREA);

			rightIrisRoi = rightIrisRoi * 0 + buffer * 1;
			cv::GaussianBlur(rightIrisRoi, rightIrisRoi, cv::Size(3, 3), 1);

			cv::Mat lShow(frame, leftRect), rShow(frame, rightRect);//cv::imshow("ll", lShow);
			//cv::imshow("rr", rShow);

		}
		catch (exception err) {
			cout << "lensGrayMask or add err" << endl;
			return 5;
		}

		output = frame.clone();
		//cv::resize(frame, frame, cv::Size(480, 680));
		//cv::imshow("frame", frame);
	}
	else {
		return 1;
	}
	return 0;
}

int IrisTracker::getAge(cv::Mat frame)
{
	VsImage* vsFrame = (VsImage*)&IplImage(frame);
	int age = analyser->estimateAge(vsFrame, &trackingData[0]);
	return age;
}

float* IrisTracker::getEmotion(cv::Mat frame)
{
	VsImage* vsFrame = (VsImage*) &IplImage(frame);
	float emo[7];  //anger, disgust, fear, happiness, sadness, surprise and neutral
	analyser->estimateEmotion(vsFrame, trackingData, emo);

	return emo;
}

int IrisTracker::getGender(cv::Mat frame)  //1 = male and 0 = female.
{
	VsImage* vsFrame = (VsImage*)&IplImage(frame);
	int gender = analyser->estimateGender(vsFrame, &trackingData[0]);

	return gender;
}


// translation between coordinates of sdk and opencv 
int* IrisTracker::vis2cv_AxisTransfer(const float* pos, int width, int height) {
	//cout << pos[0] << " " << pos[1]<<endl;
	int x = pos[0] * width;
	int y = pos[1] * height;
	y = height - y;
	int ans[2] = { x, y };
	return ans;
}

float IrisTracker::pointsDistance(int* cvCoor, cv::Point point) {
	return sqrt(pow((cvCoor[0] - point.x), 2) + pow((cvCoor[1] - point.y), 2));
}

float IrisTracker::pointsDistance(cv::Point2d p1, cv::Point p2) {
	return sqrt(pow((p1.x - p2.x), 2) + pow((p1.y - p2.y), 2));
}

IrisTracker::IrisTracker()
{
}


IrisTracker::~IrisTracker()
{
}


