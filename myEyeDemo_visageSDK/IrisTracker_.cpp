#include "IrisTracker.h"
#include "stdafx.h"
#include "visageVision.h"


void IrisTracker::initialize(const char* highCfg, const char* lensPath, const char* vfadata, int framerate)
{
	// initialize Points for comparation
	for (int i = 0; si < 8; i++) {
		lefteyeCV.push_back(cv::Point(0, 0));
		righteyeCV.push_back(cv::Point(0, 0));
	}
	leftPupilCV = cv::Point(0, 0);
	rightPupilCV = cv::Point(0, 0);

	// initialize license and tracker and analyser
	VisageSDK::initializeLicenseManager(".");
	VisageSDK::VisageTracker tracker(highCfg);
	VisageSDK::VisageFaceAnalyser *analyser = new VisageSDK::VisageFaceAnalyser();
	analyser->init(vfadata);

	// initialize lensPath
	lensPath = lensPath;

	// framerate
	wait = 20 - trackerDelay;
}

int IrisTracker::irisTrack(cv::Mat frame, cv::Mat mask, cv::Point * pupil)
{
	if (isVertical)
		transpose(frame, frame);
	//imshow("ff", frame);

	IplImage *ipl = &IplImage(frame);

	int* stat;
	stat = tracker->track(ipl->width, ipl->height, ipl->imageData, trackingData, VISAGE_FRAMEGRABBER_FMT_BGR);
	cv::waitKey(trackerDelay);		// tracker may be too slow to be real time
	if (stat[0] != TRACK_STAT_OK) {
		cout << "tracker err" << " " << stat[0] << endl;
		return 0;
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
	//pupil
	pos = trackingData[0].featurePoints2D->getFPPos(leftPupil);
	cvCoor = vis2cv_AxisTransfer(pos, ipl->width, ipl->height);
	if (leftPupilCV.x == 0 || pointsDistance(cvCoor, leftPupilCV) > 0) {
		leftPupilCV = cv::Point(cvCoor[0], cvCoor[1]);
	}
	//cout << leftPupilCV.x << endl;
	//circle(frame, leftPupilCV, 3, Scalar(255, 0, 0), -1);
	//circle(frame, rightPupilCV, 3, Scalar(255, 0, 0), -1);

	pos = trackingData[0].featurePoints2D->getFPPos(rightPupil);
	cvCoor = vis2cv_AxisTransfer(pos, ipl->width, ipl->height);
	if (rightPupilCV.x == 0 || pointsDistance(cvCoor, rightPupilCV) > 0) {
		rightPupilCV = cv::Point(cvCoor[0], cvCoor[1]);
	}

	//cout << 2;

	// get eyesquare roi
	cv::Rect leftRect = cv::boundingRect(lefteyeCV);
	cv::Rect rightRect = cv::boundingRect(righteyeCV);
	if (leftRect.width == 1 || rightRect.width == 1) {
		cout << "bounding box err";
		return 1;
	}
	cv::Mat leftRoi(frame, leftRect);
	cv::Mat rightRoi(frame, rightRect);

	//cv::imshow("left", leftRoi);
	//cv::imshow("right", rightRoi);


	// Blur and get canny
	for (int i = 0; i < blurCount; i++) {
		cv::GaussianBlur(leftRoi, leftRoi, cv::Size(blurKernelSize, blurKernelSize), 1);
		cv::GaussianBlur(rightRoi, rightRoi, cv::Size(blurKernelSize, blurKernelSize), 1);
	}

	cv::Mat leftCanny, rightCanny;
	cv::Canny(leftRoi, leftCanny, cannyp1, cannyp2);
	cv::Canny(rightRoi, rightCanny, cannyp1, cannyp2);
	imshow("rightC", rightCanny);
	imshow("leftC", leftCanny);


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


	// calculate Iris radius 
	if (trackingData[0].eyeClosure[0] && trackingData[0].eyeClosure[1]) {  // if eye-closure detected, then continue for next frame

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

		// load lens pic and resize, set seperate lens roi(square) on face
		if (isChanged) {
			lens = cv::imread(lensPath);
		}
		try {
			cv::resize(lens, lens, cv::Size((int)radius * 2, (int)radius * 2));
		}
		catch (exception err) {
			cout << "lens resize err" << endl;
			return 3;
		}
		cv::Rect leftIrisRect(leftPupilCV.x - radius, leftPupilCV.y - radius, 2 * radius, 2 * radius),
			rightIrisRect(rightPupilCV.x - radius, rightPupilCV.y - radius, 2 * radius, 2 * radius);
		cv::Mat leftIrisRoi(frame, leftIrisRect), rightIrisRoi(frame, rightIrisRect);

		// make border on mask for the later roi cutting. The Rect x y need to add the border
		int margin = radius + 10;
		cv::copyMakeBorder(lefteyeMask, lefteyeMask, margin, margin, margin, margin, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
		cv::copyMakeBorder(righteyeMask, righteyeMask, margin, margin, margin, margin, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
		//cv::imshow("mask", lefteyeMask);

		cv::Rect leftIrisMaskRect(leftIrisRect.x - leftRect.x + margin, leftIrisRect.y - leftRect.y + margin, leftIrisRect.width, leftIrisRect.height),
			rightIrisMaskRect(rightIrisRect.x - rightRect.x + margin, rightIrisRect.y - rightRect.y + margin, rightIrisRect.width, rightIrisRect.height);
		try {
			// set lens roi(square) on mask
			cv::Mat leftmaskRoi(lefteyeMask, leftIrisMaskRect), rightmaskRoi(righteyeMask, rightIrisMaskRect);

			// turn mask to black on mask roi if lens[i,j] is white(average > 250)
			//////// commented this part if you need no mask on the center of lens ///////

			/**/
			cv::Mat lensGray;
			cv::cvtColor(lens, lensGray, cv::COLOR_BGR2GRAY);
			for (int i = 0; i < lens.rows; i++) {
				for (int j = 0; j < lens.cols; j++) {
					if (lensGray.at<uchar>(i, j) >= 235)
					{
						leftmaskRoi.at<uchar>(i, j) = 0;
						rightmaskRoi.at<uchar>(i, j) = 0;
					}
				}
			}
			/**/

			cv::Mat leftMask = leftmaskRoi.clone(), rightMask = rightmaskRoi.clone();
			//cv::imshow("leftlensMask", leftMask);


			// add lens pic on face lensRoi with mask and transparency
			cv::Mat buffer = leftIrisRoi.clone();
			cv::add(lens, leftIrisRoi, buffer, leftMask);
			cv::addWeighted(leftIrisRoi, 1 - lensTransparentWeight, buffer, lensTransparentWeight, 0, leftIrisRoi);

			buffer = rightIrisRoi.clone();
			cv::add(lens, rightIrisRoi, buffer, rightMask);
			cv::addWeighted(rightIrisRoi, 1 - lensTransparentWeight, buffer, lensTransparentWeight, 0, rightIrisRoi);


			cv::Mat lShow(frame, leftRect), rShow(frame, rightRect);
			cv::imshow("ll", lShow);
			cv::imshow("rr", rShow);

			//mask = leftIrisMaskRect;  //???
			// mask, cv::Point* pupil deliver
		}
		catch (exception err) {
			cout << "lensGrayMask or add err" << endl;
			return 4;
		}
}


int IrisTracker::getAge(cv::Mat frame)
{
	VsImage* vsFrame;

	return 0;
}

int IrisTracker::getEmotion(cv::Mat frame)
{
	VsImage* vsFrame;

	return 0;
}

int IrisTracker::getGender(cv::Mat frame)
{
	VsImage* vsFrame;

	return 0;
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

// collect contour points inside the eyeMask (input *contours and output *insidePoints)
void IrisTracker::pointsInside(cv::Mat eyeMask, std::vector< std::vector<cv::Point>>* contours, std::vector<cv::Point2d>* insidePoints, int defx, int defy) {
	for (int i = 0; i<contours->size(); i++)
	{
		for (int j = 0; j < contours->at(i).size(); j++) {
			cv::Point t = contours->at(i).at(j);
			if (eyeMask.at<uchar>(t.y, t.x) == 255) {
				insidePoints->push_back(t);
			}
		}
	}
}

bool IrisTracker::isNearBorder(cv::Point point, cv::Rect rect) {
	int xborder = rect.width / borderFactorx;  //12
	int yborder = rect.height / borderFactory;  //5
	cout << xborder << " " << yborder << endl;
	return point.x - rect.x < xborder || rect.x + rect.width - point.x < xborder
		|| point.y - rect.y < yborder || rect.y + rect.height - point.y < yborder;
}

float IrisTracker::pointsDistance(int* cvCoor, cv::Point point) {
	return sqrt(pow((cvCoor[0] - point.x), 2) + pow((cvCoor[1] - point.y), 2));
}

float IrisTracker::pointsDistance(cv::Point2d p1, cv::Point p2) {
	return sqrt(pow((p1.x - p2.x), 2) + pow((p1.y - p2.y), 2));
}