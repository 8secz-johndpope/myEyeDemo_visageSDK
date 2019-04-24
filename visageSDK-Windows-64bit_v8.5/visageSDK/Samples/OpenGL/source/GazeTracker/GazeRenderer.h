#include "visageVision.h"
#include "glut.h"
#include "Window.h"

using namespace VisageSDK;

/** Simple OpenGL graphics renderer.
* Contains methods to draw screen space gaze postion in calibration or estimation phase, as well as video stream from camera and tracking results.
*/
class GazeRenderer
{
public:
	GazeRenderer(void);
	~GazeRenderer(void);
	
	void SetDrawWindow(Window* window);
	
	/** Method called to redraw the gaze tracking and face tracking results in display window.
	* In calibration phase it draws the latest calibration point, video frame from the camera and the latest tracking results. 
	* 
	* In estimation phase it draws the latest estimated screen space gaze position, video frame from the camera and the latest tracking results.
	* 
	* Additionally, it displays instruction messages.
	* Drawing is done from the main application thread.
	*/
	void Redraw(ScreenSpaceGazeData* gazeData, FaceData* data, VsImage* image, int trackingStatus);
	
	void RecalibStarted(int trackingStatus);

	bool inRecalibSequence;
	bool recalibSequenceEnd;

	void DrawPoint(float x, float y, int r, int g, int b, int size);
	void DrawImage(VsImage* image);
	void DrawTrackingData(FaceData* data, VsImage* frame);
	void DisplayText(char* displayText, float x, float y);
	void Clear();
	void Swap();

private:
	Window* window;
	int width, height;
	GLuint frameTexId;
	int glHeight, glWidth;
	float xTexCoord, yTexCoord;
	float glMousePosX, glMousePosY;
	long timestamp;
	long recalibTimestamp;
	bool setRenderingContext;

	void DisplayGazeData(ScreenSpaceGazeData calibData);

	void initFrameTexture(VsImage* image);
	void setFrameTexture(VsImage* image);
	void DrawImage();
	void SetupFrustum();
	void drawFaceFeatures(FDP* featurePoints2D);
	void drawGazeDirection(const float* gazeDirection ,FDP* featurePoints3D);
	void drawPoints2D(int *points, int num, int drawType, FDP* featurePoints2D);
	void SetupFrustum(int width, int height, int focus);
	bool eyesState;

};
