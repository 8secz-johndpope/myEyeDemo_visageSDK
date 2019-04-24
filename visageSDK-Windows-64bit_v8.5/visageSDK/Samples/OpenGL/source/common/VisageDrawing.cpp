#include "VisageDrawing.h"


namespace VisageSDK
{

void VisageDrawing::drawResults(VsImage* img, FaceData* td, bool drawOnly2DFeatures)
{

		static int contourPoints[] = {
			13,	1,
			13,	3,
			13,	5,
			13,	7,
			13,	9,
			13,	11,
			13,	13,
			13,	15,
			13,	17,
			13,	16,
			13,	14,
			13,	12,
			13,	10,
			13,	8,
			13,	6,
			13,	4,
			13,	2,
		};

		drawSpline2D(img, contourPoints, td->featurePoints2D, 17);
		drawPoints2D(img, contourPoints, td->featurePoints2D, 17, 0, td);

		static int outerUpperLipPoints[] = {
			8, 4,
			8, 6,
			8, 9,
			8, 1,
			8, 10,
			8, 5,
			8, 3,
		};
		drawSpline2D(img, outerUpperLipPoints, td->featurePoints2D, 7);

		static int outerLowerLipPoints[] = {
			8, 4,
			8, 8,
			8, 2,
			8, 7,
			8, 3,
		};
		drawSpline2D(img, outerLowerLipPoints, td->featurePoints2D, 5);

		static int innerUpperLipPoints[] = {
			2, 5,
			2, 7,
			2, 2,
			2, 6,
			2, 4,
		};
		drawSpline2D(img, innerUpperLipPoints, td->featurePoints2D, 5);

		static int innerLowerLipPoints[] = {
			2, 5,
			2, 9,
			2, 3,
			2, 8,
			2, 4,
		};
		drawSpline2D(img, innerLowerLipPoints, td->featurePoints2D, 5);

		static int innerLipPoints[] = {
			2,	2,
			2,	6,
			2,	4,
			2,	8,
			2,	3,
			2,	9,
			2,	5,
			2,	7,
		};

		drawPoints2D(img, innerLipPoints, td->featurePoints2D, 8, 0, td);

		static int outerLipPoints[] = {
			8,	1,
			8,	10,
			8,	5,
			8,	3,
			8,	7,
			8,	2,
			8,	8,
			8,	4,
			8,	6,
			8,	9,
		};

		drawPoints2D(img, outerLipPoints, td->featurePoints2D, 10, 0, td);

		static int noseLinePoints[] = {
			9,	1,
			9,	3,
			9,	2,
		};
		drawSpline2D(img, noseLinePoints, td->featurePoints2D, 3);

		static int noseLinePoints2[] = {
			9,	3,
			14,	22,
			14,	23,
			14, 24,
			14, 25
		};
		drawSpline2D(img, noseLinePoints2, td->featurePoints2D, 5);

		static int nosePoints[] = {
			9,	1,
			9,	2,
			9,	3,
			9,	15,
			14, 22,
			14, 23,
			14, 24,
			14, 25
		};

		drawPoints2D(img, nosePoints, td->featurePoints2D, 8, 0, td);

		//draw eyes
		if (td->eyeClosure[0] > 0.5f){
			static int irisPoints1[] = {
				3,	5
			};
			drawPoints2D(img, irisPoints1, td->featurePoints2D, 1, 0, td);
		}
		if (td->eyeClosure[1] > 0.5f){
			static int irisPoints[] = {
				3,	6
			};
			drawPoints2D(img, irisPoints, td->featurePoints2D, 1, 0, td);
		}

		static int outerUpperEyePointsR[] = {
			3,	12,
			3,	14,
			3,	8
		};

		static int outerLowerEyePointsR[] = {
			3,	8,
			3,	10,
			3,	12
		};

		static int innerUpperEyePointsR[] = {
			3,	12,
			12, 10,
			3,	2,
			12, 6,
			3,	8
		};
		static int innerLowerEyePointsR[] = {
			3,	8,
			12, 8,
			3,	4,
			12, 12,
			3,	12
		};

		static int outerUpperEyePointsL[] = {
			3,	11,
			3,	13,
			3,	7
		};

		static int outerLowerEyePointsL[] = {
			3,	7,
			3,	9,
			3,	11
		};

		static int innerUpperEyePointsL[] = {
			3,	11,
			12, 9,
			3,	1,
			12, 5,
			3,	7
		};

		static int innerLowerEyePointsL[] = {
			3,	7,
			12, 7,
			3,	3,
			12, 11,
			3,	11
		};

		drawSpline2D(img, outerUpperEyePointsR, td->featurePoints2D, 3);
		drawSpline2D(img, outerLowerEyePointsR, td->featurePoints2D, 3);
		drawSpline2D(img, innerUpperEyePointsR, td->featurePoints2D, 5);
		drawSpline2D(img, innerLowerEyePointsR, td->featurePoints2D, 5);

		drawSpline2D(img, outerUpperEyePointsL, td->featurePoints2D, 3);
		drawSpline2D(img, outerLowerEyePointsL, td->featurePoints2D, 3);
		drawSpline2D(img, innerUpperEyePointsL, td->featurePoints2D, 5);
		drawSpline2D(img, innerLowerEyePointsL, td->featurePoints2D, 5);

		static int eyesPoints[] = {
			3,	2,
			3,	4,
			3,	8,
			3,	10,
			3,	12,
			3,	14,
			12, 6,
			12, 8,
			12, 10,
			12, 12
		};
		static int eyesPoints2[] = {
			3,	1,
			3,	3,
			3,	7,
			3,	9,
			3,	11,
			3,	13,
			12, 5,
			12, 7,
			12, 9,
			12, 11
		};

		drawPoints2D(img, eyesPoints, td->featurePoints2D, 10, td->eyeClosure[1] < 0.5f, td);
		drawPoints2D(img, eyesPoints2, td->featurePoints2D, 10, td->eyeClosure[0] < 0.5f, td);

		static int eyebrowLinesPointsR[] = {
			4,	6,
			14,	4,
			4,	4,
			14,	2,
			4,	2
		};
		drawSpline2D(img, eyebrowLinesPointsR, td->featurePoints2D, 5);

		static int eyebrowLinesPointsL[] = {
			4,	1,
			14,	1,
			4,	3,
			14,	3,
			4,	5
		};
		drawSpline2D(img, eyebrowLinesPointsL, td->featurePoints2D, 5);

		static int eyebrowPoints[] = {
			4,	1,
			4,	2,
			4,	3,
			4,	4,
			4,	5,
			4,	6,
			14,	1,
			14,	2,
			14,	3,
			14,	4
		};

		drawPoints2D(img, eyebrowPoints, td->featurePoints2D, 10, 0, td);

		if (!drawOnly2DFeatures)
		{
			drawFaceModelAxes(img, td);
			drawGaze(img, td, 255, 0, 0);
		}
}

void VisageDrawing::drawLogo(VsImage* img, VsImage* logo)
{
	float logoWidth;
	float logoHeight;
	
	//logo aspect
	float logoAspect = logo->width / (float)logo->height;

	logoWidth = img->width/4;
	logoHeight = logoWidth / logoAspect;
	
	IplImage* resizedLogo = cvCreateImage(cvSize(logoWidth,logoHeight),logo->depth,logo->nChannels);
	cvResize(logo,resizedLogo, CV_INTER_LINEAR);

	//set logo ROI diemensions and position in image
	CvRect logoROI;
	logoROI.x = img->width - logoWidth;
	logoROI.y = 0;
	logoROI.width = logoWidth;
	logoROI.height = logoHeight;

	//Set the ROI to the same size as the logo
	cvSetImageROI((IplImage*)img, logoROI);

	//Add the logo to the designated ROI
	int width = img->width;
	int height = resizedLogo->height;
	int nchannels = img->nChannels;
	int step = img->widthStep;
	int logoStep = resizedLogo->widthStep;

	uchar *data = (uchar*)img->imageData;
	uchar *dataLogo = (uchar*)resizedLogo->imageData;
	for (int i = 0; i < height; i++) {
		int k = 0;
		for (int j = 0; j < width; j++) {

			if (j % img->width >= logoROI.x)
			{
				int rImg = data[i*step + j*nchannels + 0];
				int gImg = data[i*step + j*nchannels + 1];
				int bImg = data[i*step + j*nchannels + 2];
				int aImg = data[i*step + j*nchannels + 3];
				//
				int rLogo = dataLogo[i*logoStep + k*nchannels + 0];
				int gLogo = dataLogo[i*logoStep + k*nchannels + 1];
				int bLogo = dataLogo[i*logoStep + k*nchannels + 2];
				int aLogo = dataLogo[i*logoStep + k*nchannels + 3];
				//
				k++;
				//Blend formula
				//dst[r] = src1[r] * alpha + src2[r]*(1-alpha)
				//alpha = src1[a]/255.0f
				float alpha = aLogo / 255.0f;
				//
				int rBlend = rLogo * alpha + rImg * (1.0f - alpha);
				int gBlend = gLogo * alpha + gImg * (1.0f - alpha);
				int bBlend = bLogo * alpha + bImg * (1.0f - alpha);
				data[i*step + j*nchannels + 0] = rBlend;
				data[i*step + j*nchannels + 1] = gBlend;
				data[i*step + j*nchannels + 2] = bBlend;
			}

		}
	}

	//reset ROI
	cvResetImageROI((IplImage*) img);
	//clean up
	cvReleaseImage(&resizedLogo);
}

void VisageDrawing::drawSpline2D(VsImage* img, int* points, FDP* fdp, int num)
{
	if (num < 2)
			return;
		vector<float> pointCoords;
		int n = 0;
		FeaturePoint* fp;
		for (int i = 0; i < num; i++) {
			fp = const_cast<FeaturePoint*>( &fdp->getFP(points[2*i],points[2*i+1]) );
			if(fp->defined)
				if(fp->pos[0]!=0 && fp->pos[1]!=0)
				{
					pointCoords.push_back(fp->pos[0]);
					pointCoords.push_back(fp->pos[1]);
					n++;
				}
		}

		if(pointCoords.size() == 0 || n <= 2)
			return;

		int factor = 10;
		vector<float> pointsToDraw;
		VisageDrawing::calcSpline(pointCoords, factor, pointsToDraw);
		int nVert = (int)pointsToDraw.size() / 2;
		float *vertices = new float[nVert*2];
		int cnt = 0;
		for (int i = 0; i < nVert; ++i)
		{
			vertices[2*i+0] = pointsToDraw.at(cnt++)*img->width;
			vertices[2*i+1] = (1 - pointsToDraw.at(cnt++))*img->height;
		}

		CvScalar color = cvScalar(222,196,176,160);

		for(int j = 0; j < nVert-1; j++)
		{
			cvLine(img, cvPoint(vertices[2 * j + 0], vertices[2 * j + 1]), cvPoint(vertices[2 * (j + 1) + 0], vertices[2 * (j + 1) + 1]), color, 1);
		}

		//clean-up
		delete[] vertices;
}

void VisageDrawing::drawPoints2D(VsImage* img, int* points, FDP* fdp, int num, bool singleColor, FaceData* td)
{
	float radius = MAX(td->faceScale * 0.02f, 2);
	FeaturePoint* fp;
	for (int i = 0; i < num; i++)
	{
		fp = const_cast<FeaturePoint*>( &fdp->getFP(points[2*i],points[2*i+1]) );
		if(fp->defined && fp->pos[0]!=0 && fp->pos[1]!=0)
		{
			float x = fp->pos[0] * img->width;
			float y = (1-fp->pos[1]) * img->height;
			CvScalar color = cvScalar(0, 0, 0, 255);
			cvCircle(img, cvPoint(x, y), cvRound(radius), color, -1);

			if(!singleColor)
			{
				float radius2 = MIN(cvRound(radius * 0.6f), radius - 1);

#if defined(ANDROID) || defined(MAC_OS_X) || defined(EMSCRIPTEN)
				CvScalar color = cvScalar(0, 255, 255, 255);
#else
				CvScalar color = cvScalar(255, 255, 0, 255);
#endif
				cvCircle(img, cvPoint(x, y), cvRound(radius2), color, -1);
			}
		}
	}
}

void VisageDrawing::drawFaceModelAxes(VsImage* img, FaceData* trackingData)
{
	//precalculate sines and cosines for eye rotations
	const float *r = trackingData->faceRotation;
	const float f = trackingData->cameraFocus;

	FDP *fdp = trackingData->featurePoints3D;

	const FeaturePoint &fp1 = fdp->getFP(4, 1);
	const FeaturePoint &fp2 = fdp->getFP(4, 2);

	const float t[] = {
		(fp1.pos[0] + fp2.pos[0]) / 2.0f,
		(fp1.pos[1] + fp2.pos[1]) / 2.0f,
		(fp1.pos[2] + fp2.pos[2]) / 2.0f
	};

	//pre calculate sines and cosines
	float sinrx = sin(r[0]);
	float sinry = sin(r[1] + VS_PI);
	float sinrz = sin(r[2]);
	float cosrx = cos(r[0]);
	float cosry = cos(r[1] + VS_PI);
	float cosrz = cos(r[2]);

	//set the rotation matrix
	float R00 = cosry*cosrz + sinrx*sinry*sinrz;
	float R01 = -cosry*sinrz + sinrx*sinry*cosrz;
	float R02 = cosrx*sinry;
	float R10 = cosrx*sinrz;
	float R11 = cosrx*cosrz;
	float R12 = -sinrx;
	float R20 = -sinry*cosrz + sinrx*cosry*sinrz;
	float R21 = sinry*sinrz + sinrx*cosry*cosrz;
	float R22 = cosrx*cosry;

	static const float vertices[] = {
		0.0f,	0.0f,	0.0f,
		0.07f,	0.0f,	0.0f,
		0.0f,	0.0f,	0.0f,
		0.0f,	0.07f,	0.0f,
		0.0f,	0.0f,	0.0f,
		0.0f,	0.0f,	0.07f,
	};

	float vertices2D[2 * 6] = {0};

	// variables for aspect correction
	float x_offset = 1.0f;
	float y_offset = 1.0f;
	int w = img->width;
	int h = img->height;
	if (w > h)
		x_offset = ((float)w) / ((float)h);
	else if (w < h)
		y_offset = ((float)h) / ((float)w);

	//loop over all vertices
	for (int i = 0; i < 6; i++)
	{
		float x0 = vertices[3*i+0];
		float y0 = vertices[3*i+1];
		float z0 = vertices[3*i+2];

		//scale, rotation and translation added
		float x1 = R00*x0 + R01*y0 + R02*z0 + t[0];
		float y1 = R10*x0 + R11*y0 + R12*z0 + t[1];
		float z1 = R20*x0 + R21*y0 + R22*z0 + t[2];
		float fdz = f / z1;
		//projection
		float x = fdz*x1;
		float y = fdz*y1;
		// to screen space
		vertices2D[2*i+0] = ((x * -0.5f / x_offset) + 0.5f) * w;
		vertices2D[2*i+1] = (1.0f - ((y *  0.5f / y_offset) + 0.5f)) * h;
	}

#if defined(ANDROID) || defined(MAC_OS_X) || defined(EMSCRIPTEN)
	CvScalar color_x = cvScalar(0, 255, 0, 255);
	CvScalar color_y = cvScalar(255, 0, 0, 255);
	CvScalar color_z = cvScalar(0, 0, 255, 255);
#else
	CvScalar color_x = cvScalar(0, 0, 255, 255);
	CvScalar color_y = cvScalar(255, 0, 0, 255);
	CvScalar color_z = cvScalar(0, 255, 0, 255);
#endif

	cvLine(img, cvPoint(vertices2D[0], vertices2D[1]), cvPoint(vertices2D[2],   vertices2D[3]), color_x, 2);
	cvLine(img, cvPoint(vertices2D[4], vertices2D[5]), cvPoint(vertices2D[6],   vertices2D[7]), color_y, 2);
	cvLine(img, cvPoint(vertices2D[8], vertices2D[9]), cvPoint(vertices2D[10], vertices2D[11]), color_z, 2);
}

void VisageDrawing::drawGaze(VsImage* img, FaceData* td, int r, int g, int b)
{
	//precalculate sines and cosines for eye rotations
	const float *eye_r = td->gazeDirectionGlobal;
	const float f = td->cameraFocus;

	float t[6] = { 0 };

	FDP *fdp = td->featurePoints3D;

	const FeaturePoint &leye = fdp->getFP(3, 5);
	const FeaturePoint &reye = fdp->getFP(3, 6);

	if (leye.defined && reye.defined)
	{
		t[0] = leye.pos[0];
		t[1] = leye.pos[1];
		t[2] = leye.pos[2];
		t[3] = reye.pos[0];
		t[4] = reye.pos[1];
		t[5] = reye.pos[2];
	}

	//pre calculate sines and cosines
	float sinrx = sin(eye_r[0]);
	float sinry = sin(eye_r[1] + VS_PI);
	float sinrz = sin(eye_r[2]);
	float cosrx = cos(eye_r[0]);
	float cosry = cos(eye_r[1] + VS_PI);
	float cosrz = cos(eye_r[2]);

	//set the rotation matrix
	float R00 = cosry*cosrz + sinrx*sinry*sinrz;
	float R01 = -cosry*sinrz + sinrx*sinry*cosrz;
	float R02 = cosrx*sinry;
	float R10 = cosrx*sinrz;
	float R11 = cosrx*cosrz;
	float R12 = -sinrx;
	float R20 = -sinry*cosrz + sinrx*cosry*sinrz;
	float R21 = sinry*sinrz + sinrx*cosry*cosrz;
	float R22 = cosrx*cosry;

	static const float vertices[] = {
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.04f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.04f
	};

	float vertices2D[2 * 4] = { 0 };

	// variables for aspect correction
	float x_offset = 1.0f;
	float y_offset = 1.0f;
	int w = img->width;
	int h = img->height;
	if (w > h)
		x_offset = ((float)w) / ((float)h);
	else if (w < h)
		y_offset = ((float)h) / ((float)w);

	//loop over all vertices
	for (int i = 0; i < 4; i++)
	{
		float x0 = vertices[3*i+0];
		float y0 = vertices[3*i+1];
		float z0 = vertices[3*i+2];

		//scale, rotation and translation added
		float x1 = R00*x0 + R01*y0 + R02*z0 + t[3*(i/2)+0]; // integer divide, parenthesis are important!
		float y1 = R10*x0 + R11*y0 + R12*z0 + t[3*(i/2)+1];
		float z1 = R20*x0 + R21*y0 + R22*z0 + t[3*(i/2)+2];
		float fdz = f / z1;
		//projection
		float x = fdz*x1;
		float y = fdz*y1;
		// to screen space
		vertices2D[2*i+0] = ((x * -0.5f / x_offset) + 0.5f) * w;
		vertices2D[2*i+1] = (1.0f - ((y *  0.5f / y_offset) + 0.5f)) * h;
	}

#if defined(ANDROID) || defined(MAC_OS_X) || defined(EMSCRIPTEN)
	CvScalar color = cvScalar(r, g, b);
#else
	CvScalar color = cvScalar(b, g, r);
#endif

	cvLine(img, cvPoint(vertices2D[0], vertices2D[1]), cvPoint(vertices2D[2], vertices2D[3]), color, 2);
	cvLine(img, cvPoint(vertices2D[4], vertices2D[5]), cvPoint(vertices2D[6], vertices2D[7]), color, 2);
}

void VisageDrawing::drawLines(VsImage* img, int* points, FDP* fdp, int num, int r, int g, int b)
{
	float* vertices = new float[2 * num];
	int faces = 1;

	for(int i = 0; i < faces; i++)
	{
		FeaturePoint* fp;
		int n = 0;
		for(int j = 0; j < num; j++)
		{
			fp = const_cast<FeaturePoint*>(&fdp->getFP(points[2 * j], points[2 * j + 1]));
			if(fp->defined)
			{
				if(fp->pos[0] != 0 && fp->pos[1] != 0)
				{
					vertices[2 * n + 0] = fp->pos[0] * img->width;
					vertices[2 * n + 1] = (1-fp->pos[1]) * img->height;
					n++;
				}
			}
		}

		CvScalar color = cvScalar(b, g, r);

		for(int j = 0; j < n ; j+=2)
		{
			cvLine(img, cvPoint(vertices[2 * j + 0], vertices[2 * j + 1]), cvPoint(vertices[2 * (j + 1) + 0], vertices[2 * (j + 1) + 1]), color, 1);
		}
	}

	delete[] vertices;
}

void VisageDrawing::drawGender(IplImage* frame, const FaceData &faceData, int gender, bool writeText)
{
	FeaturePoint head = faceData.featurePoints2D->getFP(12, 1);

	if (!head.defined)
		return;

	float x = 0.0f;
	float y = 0.0f;
	float radius = 0.0f;

	const FeaturePoint &nose1 = faceData.featurePoints2D->getFP(9, 1);
	const FeaturePoint &nose2 = faceData.featurePoints2D->getFP(9, 2);

	if (!nose1.defined || !nose2.defined)
	{
		x = head.pos[0] * frame->width;
		y = (1 - head.pos[1]) * frame->height;
		radius = faceData.faceScale / 1.7f;
	}
	else
	{
		x = (nose1.pos[0] * frame->width + nose2.pos[0] * frame->width) / 2.0f;
		y = ((1 - nose1.pos[1]) * frame->height + (1 - nose2.pos[1]) * frame->height) / 2.0f;
		radius = faceData.faceScale / 1.7f;
	}
#ifdef ANDROID
	cvCircle(frame, cvPoint(x, y), radius, gender ? cv::Scalar(0, 0, 255) : cv::Scalar(255, 0, 255), 2);
#else
	cvCircle(frame, cvPoint(x, y), radius, gender ? CV_RGB(0, 0, 255) : CV_RGB(255, 0, 255), 2);
#endif

	if (writeText)
	{
		CvFont font, outline;
		cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.4, 0.4, 0, 1, 8);
		cvInitFont(&outline, CV_FONT_HERSHEY_SIMPLEX, 0.4, 0.4, 0, 3, 8);

		//string gender_txt = gender ? "Male" : "Female";
		//cvPutText(frame, gender_txt.c_str(), cvPoint(x - (gender ? 14 : 22), y), &outline, CV_RGB(0,0,0));
		//cvPutText(frame, gender_txt.c_str(), cvPoint(x - (gender ? 14 : 22), y), &font, gender ? CV_RGB(0,128,255) : CV_RGB(255,0,255));
	}
}

void VisageDrawing::drawAge(IplImage* frame, const FaceData &faceData, int age)
{
	CvFont font;
	CvFont outline;
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.4, 0.4, 0, 1, 8);
	cvInitFont(&outline, CV_FONT_HERSHEY_SIMPLEX, 0.6, 0.6, 0, 2, 8);

	ostringstream intToString;
	intToString << age;
	string age_txt = intToString.str();


	FeaturePoint head = faceData.featurePoints2D->getFP(4, 2);

	if (head.defined)
	{
		cvPutText(frame, age_txt.c_str(), cvPoint(head.pos[0] * frame->width - faceData.faceScale / 1.7f + faceData.faceScale*1.176f / 2, (1 - head.pos[1])*frame->height - faceData.faceScale / 1.7f + faceData.faceScale*1.176f / 2), &outline, cv::Scalar(0, 255, 0));
	}
}

void VisageDrawing::drawEmotions(IplImage* frame, const FaceData &faceData, float* estimates)
{
	int x = 20;
	int y = frame->height - 100;
	int verticalStep = 12;
	int emotionsBarOffset = 70;

	float estimates_[8] = { 0 };

	estimates_[0] = 1.0f;

	for (int j = 0; j < 7; j++) estimates_[j + 1] = estimates[j];

	CvFont font;
	CvFont outline;
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.4, 0.4, 0, 1, 8);
	cvInitFont(&outline, CV_FONT_HERSHEY_SIMPLEX, 0.4, 0.4, 0, 3, 8);

	string emotions[8] = { "Emotion", "Anger", "Disgust", "Fear", "Happiness", "Sadness", "Surprise", "Neutral" };

	for (int j = 0; j < 8; j++)
	{
		cvPutText(frame, emotions[j].c_str(), cvPoint(x, y + j * verticalStep), &outline, cv::Scalar(0, 0, 0));
	}

	for (int j = 0; j < 8; j++)
	{
		int length = estimates_[j] * 100;
		cvPutText(frame, emotions[j].c_str(), cvPoint(x, y + j * verticalStep), &font, (j == 0) ? cv::Scalar(255, 255, 255) : cv::Scalar(0, 255, 0));
		cvLine(frame, cvPoint(x + emotionsBarOffset, y - 3 + j * verticalStep), cvPoint(x + emotionsBarOffset + length, y - 3 + j * verticalStep), cv::Scalar(0, 0, 0), 6);
		cvLine(frame, cvPoint(x + emotionsBarOffset, y - 3 + j * verticalStep), cvPoint(x + emotionsBarOffset + length, y - 3 + j * verticalStep), (j == 0) ? cv::Scalar(255, 255, 255) : cv::Scalar(0, 255, 0), 4);
	}

	FeaturePoint head = faceData.featurePoints2D->getFP(12, 1);

	if (head.defined)
	{
		cvRectangleR(frame, cvRect(head.pos[0] * frame->width - faceData.faceScale / 1.7f, (1 - head.pos[1])*frame->height - faceData.faceScale / 1.7f, faceData.faceScale*1.176f, faceData.faceScale*1.176f), cvScalar(0, 255, 0));
	}
}

void VisageDrawing::drawMatchingIdentity(IplImage* frame, const FaceData &faceData, const char* matchName)
{
	CvFont font;

	float font_size = faceData.faceScale < 185 ? 0.4 : 0.6;

	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, font_size, font_size, 0, 1, 8);

	float x = faceData.featurePoints2D->getFP(12, 1).pos[0] * frame->width;
	float y = frame->height - faceData.featurePoints2D->getFP(12, 1).pos[1] * frame->height;
	int scale = faceData.faceScale;

	cvPutText(frame, matchName, cvPoint(x - 0.6*scale, y + scale - 5), &font, cvScalar(0, 255, 0, 0));
	cvRectangle(frame, cvPoint(x - 0.6*scale, y + scale), cvPoint(x + 0.6*scale, y - scale), cvScalar(0, 255, 0, 0), 1, 8, 0);
}

void VisageDrawing::multiplyMatrix(float* m1, float* m2, float* res, int M, int N, int P)
{
	for (int i = 0; i < M; ++i)
	{
		for (int j = 0; j < P; ++j)
		{
			float sum = 0;
			for (int k = 0; k < N; ++k)
			{
				sum += m1[i*N+k] * m2[k*P+j];
			}
			res[i*P+j] = sum;
		}
	}
}

void VisageDrawing::calcSpline(vector <float>& inputPoints, int ratio, vector <float>& outputPoints)
{

	int nPoints, nPointsToDraw, nLines;


	nPoints = (int)inputPoints.size()/2 + 2;
	nPointsToDraw = (int)inputPoints.size()/2 + ((int)inputPoints.size()/2 - 1) * ratio;
	nLines = nPoints - 1 - 2;

	//adding fake point's coordinates
	inputPoints.insert(inputPoints.begin(), inputPoints[1] + (inputPoints[1] - inputPoints[3]));
	inputPoints.insert(inputPoints.begin(), inputPoints[1] + (inputPoints[1] - inputPoints[3]));
	inputPoints.insert(inputPoints.end(), inputPoints[inputPoints.size()/2-2] + (inputPoints[inputPoints.size()/2-2] - inputPoints[inputPoints.size()/2-4]));
	inputPoints.insert(inputPoints.end(), inputPoints[inputPoints.size()/2-1] + (inputPoints[inputPoints.size()/2-1] - inputPoints[inputPoints.size()/2-3]));


	//calculate spline

	Vec2D p0(0,0), p1(0,0), p2(0,0), p3(0,0);
	CubicPoly px, py;


	outputPoints.resize(2*nPointsToDraw);

	for(int i = 0; i < nPoints - 2; i++) {
			outputPoints[i*2*(ratio+1)] = inputPoints[2*i+2];
			outputPoints[i*2*(ratio+1)+1] = inputPoints[2*i+1+2];
	}

	for(int i = 0; i < 2*nLines; i=i+2) {
		p0.x = inputPoints[i];
		p0.y = inputPoints[i+1];
		p1.x = inputPoints[i+2];
		p1.y = inputPoints[i+3];
		p2.x = inputPoints[i+4];
		p2.y = inputPoints[i+5];
		p3.x = inputPoints[i+6];
		p3.y = inputPoints[i+7];

		InitCentripetalCR(p0, p1, p2, p3, px, py);


		for(int j = 1; j <= ratio; j++) {
			outputPoints[i*(ratio+1)+2*j] = (px.eval(1.00f/(ratio+1)*(j)));
			outputPoints[i*(ratio+1)+2*j+1] =(py.eval(1.00f/(ratio+1)*(j)));
		}

	}

	//deleting fake points
	inputPoints.erase(inputPoints.begin(), inputPoints.begin()+2);
	inputPoints.erase(inputPoints.end()-2, inputPoints.end());

//	int i = 0;

}

void VisageDrawing::InitCentripetalCR(const Vec2D& p0, const Vec2D& p1, const Vec2D& p2, const Vec2D& p3, CubicPoly &px, CubicPoly &py)
{
	float dt0 = powf(VecDistSquared(p0, p1), 0.25f);
	float dt1 = powf(VecDistSquared(p1, p2), 0.25f);
	float dt2 = powf(VecDistSquared(p2, p3), 0.25f);

	// safety check for repeated points
	if (dt1 < 1e-4f)    dt1 = 1.0f;
	if (dt0 < 1e-4f)    dt0 = dt1;
	if (dt2 < 1e-4f)    dt2 = dt1;

	InitNonuniformCatmullRom(p0.x, p1.x, p2.x, p3.x, dt0, dt1, dt2, px);
	InitNonuniformCatmullRom(p0.y, p1.y, p2.y, p3.y, dt0, dt1, dt2, py);
}

float VisageDrawing::VecDistSquared(const Vec2D& p, const Vec2D& q)
{
	float dx = q.x - p.x;
	float dy = q.y - p.y;
	return dx*dx + dy*dy;
}

void VisageDrawing::InitNonuniformCatmullRom(float x0, float x1, float x2, float x3, float dt0, float dt1, float dt2, CubicPoly &p)
{
	// compute tangents when parameterized in [t1,t2]
	float t1 = (x1 - x0) / dt0 - (x2 - x0) / (dt0 + dt1) + (x2 - x1) / dt1;
	float t2 = (x2 - x1) / dt1 - (x3 - x1) / (dt1 + dt2) + (x3 - x2) / dt2;

	// rescale tangents for parametrization in [0,1]
	t1 *= dt1;
	t2 *= dt1;

	InitCubicPoly(x1, x2, t1, t2, p);
}

void VisageDrawing::InitCubicPoly(float x0, float x1, float t0, float t1, CubicPoly &p)
{
	p.c0 = x0;
	p.c1 = t0;
	p.c2 = -3*x0 + 3*x1 - 2*t0 - t1;
	p.c3 = 2*x0 - 2*x1 + t0 + t1;
}
}

