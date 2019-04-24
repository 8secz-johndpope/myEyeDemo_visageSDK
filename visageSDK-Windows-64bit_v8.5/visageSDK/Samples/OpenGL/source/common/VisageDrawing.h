#include "WrapperOpenCV.h"
#include "FaceData.h"
#include <vector>
#include <stdio.h>

#if defined (MAC_OS_X)
	#include "cv.h"
#else
	#include "opencv/cv.h"
#endif

//#include <EGL/egl.h>
//#include <GLES/gl.h>

namespace VisageSDK
{

	class VisageDrawing
	{
	public:

		typedef struct Vec2D
		{
			Vec2D(float _x, float _y) : x(_x), y(_y) {}
			float x, y;
		} Vec2D;

		typedef struct CubicPoly
		{
			float c0, c1, c2, c3;

			float eval(float t)
			{
				float t2 = t*t;
				float t3 = t2 * t;
				return c0 + c1*t + c2*t2 + c3*t3;
			}
		} CubicPoly;

		static void drawResults(VsImage* img, FaceData* td, bool drawOnly2DFeatures = false);

        static void drawLogo(VsImage* img, VsImage* logo);
        
		static void drawSpline2D(VsImage* img, int* points, FDP* fdp, int num);

		static void drawPoints2D(VsImage* img, int* points, FDP* fdp, int num, bool singleColor, FaceData* td);

		static void drawFaceModelAxes(VsImage* img, FaceData* trackingData);

		static void drawGaze(VsImage* img, FaceData* td, int r, int g, int b);

		static void drawLines(VsImage* img, int* points, FDP* fdp, int num, int r, int g, int b);

		static void drawGender(IplImage* frame, const FaceData &faceData, int gender, bool writeText = true);

		static void drawAge(IplImage* frame, const FaceData &faceData, int age);

		static void drawEmotions(IplImage* frame, const FaceData &faceData, float* estimates);

		static void drawMatchingIdentity(IplImage* frame, const FaceData &faceData, const char* matchName);

		static void multiplyMatrix(float* m1, float* m2, float* res, int M, int N, int P);

		static void calcSpline(vector <float>& inputPoints, int ratio, vector <float>& outputPoints);

		static void InitCentripetalCR(const Vec2D& p0, const Vec2D& p1, const Vec2D& p2, const Vec2D& p3, CubicPoly &px, CubicPoly &py);

		static float VecDistSquared(const Vec2D& p, const Vec2D& q);

		static void InitNonuniformCatmullRom(float x0, float x1, float x2, float x3, float dt0, float dt1, float dt2, CubicPoly &p);

		static void InitCubicPoly(float x0, float x1, float t0, float t1, CubicPoly &p);

	};
}
