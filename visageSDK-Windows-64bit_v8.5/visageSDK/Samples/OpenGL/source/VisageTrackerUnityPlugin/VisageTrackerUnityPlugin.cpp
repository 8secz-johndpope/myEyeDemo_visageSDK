//
//	VisageTrackerUnityPlugin
//

#include <math.h> 

namespace VisageSDK
{
	void __declspec(dllimport) initializeLicenseManager(const char *licenseKeyFileFolder);
}

#include "VisageTrackerUnityPlugin.h"
//
#include "videoInput.h"

enum VISAGE_ORIENTATION
{
	PORTRAIT = 0,
	LANDSCAPE_RIGHT = 1,
	PORTRAIT_UPSIDE_DOWN = 2,
	LANDSCAPE_LEFT = 3,
};

using namespace VisageSDK;

static VisageTracker* m_Tracker = 0;
static videoInput *VI = 0;
//
FaceData trackingData[MAX_FACES] = {};
FaceData FRtrackingDataBuffer[MAX_FACES] = {};
FaceData FAtrackingDataBuffer[MAX_FACES] = {};
//
int *trackerStatus = NULL;
int *ArrayOfZeros = NULL;
int FRtrackerStatusBuffer[MAX_FACES] = {};
int FAtrackerStatusBuffer[MAX_FACES] = {};
//
unsigned char* pixels = 0;
unsigned char* FRpixelsBuffer = 0;
unsigned char* FApixelsBuffer = 0;
unsigned char* pixelsRaw = 0;
int format = VISAGE_FRAMEGRABBER_FMT_RGB;
// raw width and height as sent to openCamera method
int frameWidth = 0;
int frameHeight = 0;
// width and height with applied rotation
int frameWidthRotated = 0;
int frameHeightRotated = 0;

int NUM_FACES = 0;
//
bool isR_initialized = false;
bool isA_initialized = false;
//
static int cam_device = 0;
static int cameraMirrored = 1;
static int cameraOrientation = 0;
static float xTexScale;
static float yTexScale;
//
FuncPtr Debug = 0;
//
mutex mtx_fr_faceData = {};
mutex mtx_fr_pixels = {};
mutex mtx_fr_init = {};
//
mutex mtx_fa_faceData = {};
mutex mtx_fa_pixels = {};
mutex mtx_fa_init = {};

const int DEFAULT_CAMERA_WIDTH = 800;
const int DEFAULT_CAMERA_HEIGHT = 600;

static unsigned int GetNearestPow2(unsigned int num)
{
	unsigned int n = num > 0 ? num - 1 : 0;

	n |= n >> 1;
	n |= n >> 2;
	n |= n >> 4;
	n |= n >> 8;
	n |= n >> 16;
	n++;

	return n;
}

// Helper method to rotate/mirror RGB image
static void rotateRGB(unsigned char* input, unsigned char* output, int width, int height, int rotation, int cameraMirrored)
{
	bool swap = false;
	bool xflip = false;
	bool yflip = false;

	if (rotation == VISAGE_ORIENTATION::LANDSCAPE_RIGHT)
	{
		swap = true;
		if (cameraMirrored == 1)
			yflip = true;
	}

	else if (rotation == VISAGE_ORIENTATION::PORTRAIT_UPSIDE_DOWN)
	{
		yflip = true;
		if (cameraMirrored == 1)
			xflip = true;
	}

	else if (rotation == VISAGE_ORIENTATION::LANDSCAPE_LEFT)
	{
		swap = true;
		yflip = true;
		if (cameraMirrored == 1)
			yflip = false;
	}
	//default VISAGE_ORIENTATION::PORTRAIT
	else
	{
		swap = false;
		xflip = false;
		if (cameraMirrored == 1)
		{
			xflip = true;
		}
	}


	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			int rIn = j * width * 3 + 3 * i + 0;
			int gIn = j * width * 3 + 3 * i + 1;
			int bIn = j * width * 3 + 3 * i + 2;

			int wOut = swap		? height				: width;
			int hOut = swap		? width					: height;
			int iSwapped = swap ? j						: i;
			int jSwapped = swap ? i						: j;
			int iOut = xflip	? wOut - iSwapped - 1	: iSwapped;
			int jOut = yflip	? hOut - jSwapped - 1	: jSwapped;

			int rOut = jOut * wOut * 3 + 3 * iOut + 0;
			int gOut = jOut * wOut * 3 + 3 * iOut + 1;
			int bOut = jOut * wOut * 3 + 3 * iOut + 2;

			output[rOut] = input[rIn];
			output[gOut] = input[gIn];
			output[bOut] = input[bIn];
		}
	}
}

// Returns index of the first tracker that has status TRACK_STATUS_OK, otherwise returns -1
int firstTrackerStatusOK()
{
	int index = -1;

	if (trackerStatus != NULL)
	{
		for (int i = 0; i < NUM_FACES; ++i)
		{
			if (trackerStatus[i] == TRACK_STAT_OK)
			{
				index = i;
				break;
			}
		}
	}

	return index;
}

// Returns index of the first tracker that has status TRACK_STATUS_OK, TRACK_STATUS_INIT or TRACK_STAT_RECOVERING, otherwise returns -1
int firstTrackerStatusNotOFF()
{
	int index = -1;

	if (trackerStatus != NULL)
	{
		for (int i = 0; i < NUM_FACES; ++i)
		{
			if (trackerStatus[i] != TRACK_STAT_OFF)
			{
				index = i;
				break;
			}
		}
	}
	return index;
}

// When native code plugin is implemented in .mm / .cpp file, then functions
// should be surrounded with extern "C" block to conform C function naming rules

extern "C"
{	
	//initialize licensing
	void _initializeLicense(char* license)
	{
	
		initializeLicenseManager(license);
	}

	// initialises the tracker
	void _initTracker(char* config, int numFaces)
	{
		if (m_Tracker)
			delete m_Tracker;

		if (numFaces > MAX_FACES)
			NUM_FACES = MAX_FACES;
		else
			NUM_FACES = numFaces;

		ArrayOfZeros = new int[NUM_FACES];
		memset(ArrayOfZeros, 0, NUM_FACES * sizeof(int));
		
		m_Tracker = new VisageTracker(config);		
	}

	// grabs current frame
	void _grabFrame()
	{
		if (!VI) return;

		while (!VI->isFrameNew(cam_device))
			Sleep(1);

		VI->getPixels(cam_device, (unsigned char*)pixelsRaw, false, true);

		if(cameraOrientation == 0 && cameraMirrored == 0)
			memcpy(pixels, pixelsRaw, frameWidth * frameHeight  * N_CHANNELS * sizeof(unsigned char));
		else
			rotateRGB(pixelsRaw, pixels, frameWidth, frameHeight, cameraOrientation, cameraMirrored);

		mtx_fr_init.lock();
		if (isR_initialized)
		{
			mtx_fr_pixels.lock();
			memcpy(FRpixelsBuffer, pixels, frameWidthRotated * frameHeightRotated * N_CHANNELS * sizeof(unsigned char));
			mtx_fr_pixels.unlock();
		}
		mtx_fr_init.unlock();

		mtx_fa_init.lock();
		if (isA_initialized)
		{
			mtx_fa_pixels.lock();
			memcpy(FApixelsBuffer, pixels, frameWidthRotated * frameHeightRotated * N_CHANNELS * sizeof(unsigned char));
			mtx_fa_pixels.unlock();
		}
		mtx_fa_init.unlock();
	}

	// release memory allocated by the initTracker function
	void _releaseTracker()
	{
		delete m_Tracker;
		delete[] ArrayOfZeros; ArrayOfZeros = 0;
		m_Tracker = 0;
	}

	// estimated tracking quality
	float _getTrackingQuality(int faceIndex)
	{
		float quality = -1;

		if (trackerStatus != NULL)
			if (trackerStatus[faceIndex] == TRACK_STAT_OK && faceIndex < NUM_FACES)
				quality = trackingData[faceIndex].trackingQuality;

		return quality;
	}

	// returns frame rate of the tracker
	float _getFrameRate()
	{
		float frameRate = -1;
		int faceIndex = firstTrackerStatusNotOFF();

		if (faceIndex != -1)
			frameRate = trackingData[faceIndex].frameRate;

		return frameRate;
	}

	// Timestamp of the current video frame
	long _getTimeStamp()
	{
		long timeStamp = -1;

		int faceIndex = firstTrackerStatusNotOFF();
		
		if (faceIndex != -1)
			timeStamp = trackingData[faceIndex].timeStamp;

		return timeStamp;
	}

	// Scale of facial bounding box
	int _getFaceScale(int faceIndex)
	{
		if (trackerStatus != NULL)
			if (trackerStatus[faceIndex] == TRACK_STAT_OK && faceIndex < NUM_FACES)
				return trackingData[faceIndex].faceScale;
		
		return -1;
	}

	// starts face tracking on current frame
	void _track()
	{
		if (!m_Tracker)
			return;
	
		trackerStatus = m_Tracker->track(frameWidthRotated, frameHeightRotated, (const char *)pixels, trackingData, format, VISAGE_FRAMEGRABBER_ORIGIN_TL, 0, -1, NUM_FACES);
		
		if (trackerStatus[0] == TRACK_STAT_OFF && pixels)
		{
			memset(pixels, 0, frameWidthRotated * frameHeightRotated * N_CHANNELS);
		}

		mtx_fr_init.lock();
		if (isR_initialized)
		{
			mtx_fr_faceData.lock();
			for (int i = 0; i < NUM_FACES; ++i)
			{
				FRtrackingDataBuffer[i] = trackingData[i];
			}
			memcpy(FRtrackerStatusBuffer, trackerStatus, NUM_FACES * sizeof(int));
			mtx_fr_faceData.unlock();

			mtx_fr_pixels.lock();
			if (trackerStatus[0] == TRACK_STAT_OFF && FRpixelsBuffer)
			{
				memset(FRpixelsBuffer, 0, frameWidthRotated * frameHeightRotated * N_CHANNELS);
			}
			mtx_fr_pixels.unlock();
		}
		mtx_fr_init.unlock();

		mtx_fa_init.lock();
		if (isA_initialized)
		{
			mtx_fa_faceData.lock();
			for (int i = 0; i < NUM_FACES; ++i)
			{
				FAtrackingDataBuffer[i] = trackingData[i];
			}
			memcpy(FAtrackerStatusBuffer, trackerStatus, NUM_FACES * sizeof(int));
			mtx_fa_faceData.unlock();

			mtx_fa_pixels.lock();
			if (trackerStatus[0] == TRACK_STAT_OFF && FApixelsBuffer)
			{
				memset(FApixelsBuffer, 0, frameWidthRotated * frameHeightRotated * N_CHANNELS);
			}
			mtx_fa_pixels.unlock();
		}
		mtx_fa_init.unlock();

		return;
	}

	// gets tracker status
	void _getTrackerStatus(int* trStatus)
	{
		if (trackerStatus != NULL)
			memcpy(trStatus, trackerStatus, NUM_FACES * sizeof(int));
		else
			memcpy(trStatus, ArrayOfZeros, NUM_FACES * sizeof(int));
	}

	//  initializes new camera
	void _openCamera(int orientation, int deviceID, int width, int height, int isMirrored)
	{
		int cam_width = DEFAULT_CAMERA_WIDTH;
		int cam_height = DEFAULT_CAMERA_HEIGHT;

		if (orientation == 1 || orientation == 3)
		{
			cam_width = DEFAULT_CAMERA_HEIGHT;
			cam_height = DEFAULT_CAMERA_WIDTH;
		}

		if (width != -1 || height != -1)
		{
			cam_width = width;
			cam_height = height;
		}

		int cam_fps = 30;
		int format = VISAGE_FRAMEGRABBER_FMT_RGB;
		bool r = false;

		int n = videoInput::listDevices();

		if (n > deviceID)
			cam_device = deviceID;
		else
			cam_device = 0;

		// if camera already works, release
		if (VI != 0)
		{
			VI->stopDevice(cam_device);
			delete VI; VI = 0; 
			delete[] pixels; pixels = 0;
			delete[] pixelsRaw; pixelsRaw = 0;
			mtx_fr_pixels.lock();
			delete[] FRpixelsBuffer; FRpixelsBuffer = 0;
			mtx_fr_pixels.unlock();
			mtx_fa_pixels.lock();
			delete[] FApixelsBuffer; FApixelsBuffer = 0;
			mtx_fa_pixels.unlock();
		}
		// ***** Grabbing from camera using Videoinput library *******
		VI = new videoInput();

		VI->setIdealFramerate(cam_device, cam_fps);
		
		r = VI->setupDevice(cam_device, cam_width, cam_height);

		if (!r)
		{
			VI->stopDevice(cam_device);
			delete VI; VI = 0;
			MessageBox(0,"Error opening camera! Check if camera is already running.","ERROR",MB_ICONERROR);
			return;
		}

		cam_width = VI->getWidth(cam_device);
		cam_height = VI->getHeight(cam_device);

		printf("Start tracking from camera %dx%d@%d\n", cam_width, cam_height, cam_fps);

		frameWidth = cam_width;
		frameHeight = cam_height;

		(orientation == 0 || orientation == 2) ? frameWidthRotated = frameWidth : frameWidthRotated = frameHeight;
		(orientation == 0 || orientation == 2) ? frameHeightRotated = frameHeight : frameHeightRotated = frameWidth;

		pixels = new unsigned char[frameWidthRotated * frameHeightRotated * N_CHANNELS];
		VI->getPixels(cam_device, (unsigned char*)pixels, false, true);

		pixelsRaw = new unsigned char[frameWidthRotated * frameHeightRotated * N_CHANNELS];
		mtx_fr_pixels.lock();
		FRpixelsBuffer = new unsigned char[frameWidthRotated * frameHeightRotated * N_CHANNELS];
		mtx_fr_pixels.unlock();
		mtx_fa_pixels.lock();
		FApixelsBuffer = new unsigned char[frameWidthRotated * frameHeightRotated * N_CHANNELS];
		mtx_fa_pixels.unlock();
		cameraMirrored = isMirrored;
		cameraOrientation = orientation;
	}

	// closes camera
	void _closeCamera()
	{
		if (VI != 0)
		{
			VI->stopDevice(cam_device);
			delete VI; VI = 0;	
			delete[] pixels; pixels = 0;
			delete[] pixelsRaw; pixelsRaw = 0;
			mtx_fr_pixels.lock();
			delete[] FRpixelsBuffer; FRpixelsBuffer = 0;
			mtx_fr_pixels.unlock();		
			mtx_fa_pixels.lock();
			delete[] FApixelsBuffer; FApixelsBuffer = 0;
			mtx_fa_pixels.unlock();
		}
	}

	// gets the current translation for the given faceIndex
	void _getHeadTranslation(float* translation, int faceIndex)
	{
		if (trackerStatus == NULL)
			return;
		
		if (trackerStatus[faceIndex] != TRACK_STAT_OK || faceIndex >= NUM_FACES)
		{
			*(translation + 0) = -10000.0f;
			*(translation + 1) = -10000.0f;
			*(translation + 2) = 0.0f;
		}
		else
		{
			*(translation + 0) = trackingData[faceIndex].faceTranslation[0];
			*(translation + 1) = trackingData[faceIndex].faceTranslation[1];
			*(translation + 2) = trackingData[faceIndex].faceTranslation[2];
		}
	}

	// gets the current rotation for the given faceIndex
	void _getHeadRotation(float* rotation, int faceIndex)
	{
		if (trackerStatus == NULL)
			return;

		if (trackerStatus[faceIndex] != TRACK_STAT_OK || faceIndex >= NUM_FACES)
		{
			*(rotation + 0)= 0;
			*(rotation + 1) = 0;
			*(rotation + 2) = 0;
		}
		else
		{
			*(rotation + 0) = trackingData[faceIndex].faceRotation[0];
			*(rotation + 1) = trackingData[faceIndex].faceRotation[1];
			*(rotation + 2) = trackingData[faceIndex].faceRotation[2];
		}
	}

	//returns number of vertices in the 3D face model.
	int _getFaceModelVertexCount()
	{
		int faceIndex = firstTrackerStatusOK();

		if (faceIndex == -1)
			return 0;

		return trackingData[faceIndex].faceModelVertexCount;
	}

	//returns number of triangles in the 3D face model
	int _getFaceModelTriangleCount()
	{
		int faceIndex = firstTrackerStatusOK();

		if (faceIndex == -1)
			return 0;

		return trackingData[faceIndex].faceModelTriangleCount;
	}

	//returns triangles for the 3D face model.
	void _getFaceModelTriangles(int* triangles, int faceIndex)
	{
		if (trackerStatus == NULL)
			return;

		if (trackerStatus[faceIndex] != TRACK_STAT_OK || faceIndex >= NUM_FACES)
		{
			return;
		}
		else
		{
			int triangleCount = trackingData[faceIndex].faceModelTriangleCount;
			for (int i = 0; i < triangleCount * 3; i++)
			{
				triangles[triangleCount * 3 - 1 - i] = trackingData[faceIndex].faceModelTriangles[i];
			}
		}
	}

	//returns vertex coordinates of the 3D face model
	void _getFaceModelVertices(float* vertices, int faceIndex)
	{
		if (trackerStatus == NULL)
			return;

		if (trackerStatus[faceIndex] != TRACK_STAT_OK || faceIndex >= NUM_FACES)
		{
			return;
		}
		else
		{
			int vertexCount = trackingData[faceIndex].faceModelVertexCount;
			memcpy(vertices, trackingData[faceIndex].faceModelVertices, 3 * (vertexCount) * sizeof(float));
		}
	}

	//returns projected (image space) vertex coordinates of the 3D face model
	void _getFaceModelVerticesProjected(float* verticesProjected, int faceIndex)
	{
		if (trackerStatus == NULL)
			return;

		if (trackerStatus[faceIndex] != TRACK_STAT_OK || faceIndex >= NUM_FACES)
		{
			return;
		}
		else
		{
			int vertexCount = trackingData[faceIndex].faceModelVertexCount;
			memcpy(verticesProjected, trackingData[faceIndex].faceModelVerticesProjected, 2 * (vertexCount) * sizeof(float));
		}
	}

	// returns texture coordinates for the 3D face model
	void _getFaceModelTextureCoords(float* texCoord, int faceIndex)
	{
		if (trackerStatus == NULL)
			return;

		if (trackerStatus[faceIndex] != TRACK_STAT_OK || faceIndex >= NUM_FACES)
		{
			return;
		}
		else
		{
			xTexScale = frameWidthRotated / (float)GetNearestPow2(frameWidthRotated);
			yTexScale = frameHeightRotated / (float)GetNearestPow2(frameHeightRotated);
			int vertexCount = trackingData[faceIndex].faceModelVertexCount;

			for (int i = 0; i < vertexCount * 2; i += 2) 
			{
				texCoord[i + 0] = (1.0f - trackingData[faceIndex].faceModelTextureCoords[i + 0]) * xTexScale;
				texCoord[i + 1] = trackingData[faceIndex].faceModelTextureCoords[i + 1] * yTexScale;
			}
		}
	}

	// returns 2
	int _getFP_START_GROUP_INDEX()
	{
		return FDP::FP_START_GROUP_INDEX;
	}

	// returns 15
	int _getFP_END_GROUP_INDEX()
	{
		return FDP::FP_END_GROUP_INDEX;
	}

	// returns size of each feature point group
	void _getGroupSizes(int* groupSize, int length)
	{
		int index = 0;
		for (int i = FDP::FP_START_GROUP_INDEX; i <= FDP::FP_END_GROUP_INDEX; ++i)
		{
			groupSize[index++] = FDP::groupSize(i);
		}
	}

	// returns camera info
	void _getCameraInfo(float *focus, int *width, int *height)
	{
		*focus = -1;
		*width = 0;
		*height = 0;
		
		if (!m_Tracker || VI == 0)
		{
			return;
		}

		int index = firstTrackerStatusNotOFF();
		
		if(index != -1)
		{
			*focus = trackingData[index].cameraFocus;
			*width = frameWidthRotated;
			*height = frameHeightRotated;
		}
		
		return;
	}

	void _getTexCoordsStatic(float* buffer, int * texCoordNumber)
	{
		if (trackerStatus == NULL)
			return;

		for (int i = 0; i < MAX_FACES; i++)
		{
			if (trackerStatus[i] == TRACK_STAT_OK && trackingData[i].faceModelTextureCoordsStatic)
			{
				memcpy(buffer, trackingData[i].faceModelTextureCoordsStatic, trackingData[i].faceModelVertexCount * 2 * sizeof(float));

				*texCoordNumber = trackingData[i].faceModelVertexCount * 2;
				return;
			}
		}
	}

	// returns the action unit count
	int _getActionUnitCount()
	{
		int faceIndex = firstTrackerStatusOK();

		if (faceIndex != -1)
			return trackingData[faceIndex].actionUnitCount;

		return 0;
	}

	// returns all action unit values
	// if AUs au_leye_closed or au_reye_closed are disabled in the configuration file 
	// it will substitute their values with discrete values of eye_closure parameter 
	void _getActionUnitValues(float* values, int faceIndex)
	{
		if (trackerStatus == NULL)
			return;

		if (trackerStatus[faceIndex] != TRACK_STAT_OK || faceIndex >= NUM_FACES)
			return;

		// get eye au indices
		int leftIndex = -1;
		int rightIndex = -1;
		for (int i = 0; i < trackingData[faceIndex].actionUnitCount; i++)
		{
			if (!strcmp(trackingData[faceIndex].actionUnitsNames[i], "au_leye_closed"))
			{
				leftIndex = i;
			}

			if (!strcmp(trackingData[faceIndex].actionUnitsNames[i], "au_reye_closed"))
			{
				rightIndex = i;
			}

			if (leftIndex >= 0 && rightIndex >= 0)
				break;
		}

		// if action units for eye closure are not used by the tracker, map eye closure values to them
		if (leftIndex >= 0 && trackingData[faceIndex].actionUnitsUsed[leftIndex] == 0) {
			trackingData[faceIndex].actionUnits[leftIndex] = trackingData[faceIndex].eyeClosure[0];
		}
		if (rightIndex >= 0 && trackingData[faceIndex].actionUnitsUsed[rightIndex] == 0) {
			trackingData[faceIndex].actionUnits[rightIndex] = trackingData[faceIndex].eyeClosure[1];
		}

		memcpy(values, trackingData[faceIndex].actionUnits, trackingData[faceIndex].actionUnitCount * sizeof(float));
	}

	// returns the name of the action unit with the specified index
	const char* _getActionUnitName(int auIndex)
	{
		int faceIndex = firstTrackerStatusOK();

		if (faceIndex != -1)
		{
			return MakeStringCopy(trackingData[faceIndex].actionUnitsNames[auIndex]);
		}

		return MakeStringCopy("");
	}

	// returns true if the action unit is used
	bool _getActionUnitUsed(int auIndex)
	{
		int faceIndex = firstTrackerStatusOK();

		if (faceIndex != -1)
			return trackingData[faceIndex].actionUnitsUsed[auIndex] == 1;

		return false;
	}

	// returns the gaze direction
	bool _getGazeDirection(float* direction, int faceIndex)
	{
		if (trackerStatus == NULL)
			return false;

		if (trackerStatus[faceIndex] != TRACK_STAT_OK || faceIndex >= NUM_FACES)
			return false;

		memcpy(direction, trackingData[faceIndex].gazeDirection, 2 * sizeof(float));
		return true;
	}

	// returns global gaze direction
	bool _getGazeDirectionGlobal(float* direction, int faceIndex)
	{
		if (trackerStatus == NULL)
			return false;

		if (trackerStatus[faceIndex] != TRACK_STAT_OK || faceIndex >= NUM_FACES)
			return false;

		memcpy(direction, trackingData[faceIndex].gazeDirectionGlobal, 3 * sizeof(float));
		return true;
	}

	// Returns discrete eye closure value. 
	bool _getEyeClosure(float* closure, int faceIndex)
	{
		if (trackerStatus == NULL)
			return false;

		if (trackerStatus[faceIndex] != TRACK_STAT_OK || faceIndex >= NUM_FACES)
			return false;
		
		memcpy(closure, trackingData[faceIndex].eyeClosure, 2 * sizeof(float));
		return true;
	}

	// returns feature point position in normalized 2D screen coordinates, indication of whether the point is defined and detected and quality for all feature points
	void _getAllFeaturePoints2D(float* featurePointArray, int length, int faceIndex)
	{
		if (trackerStatus == NULL)
			return;

		if (trackerStatus[faceIndex] != TRACK_STAT_OK || faceIndex >= NUM_FACES)
			return;

		float defined;
		float detected;

		int index = 0;
		for (int groupIndex = FDP::FP_START_GROUP_INDEX; groupIndex <= FDP::FP_END_GROUP_INDEX; ++groupIndex)
		{
			for (int pointIndex = 1; pointIndex <= FDP::groupSize(groupIndex); ++pointIndex)
			{
				if (index < length)
				{
					const float* positions2 = trackingData[faceIndex].featurePoints2D->getFPPos(groupIndex, pointIndex);
					featurePointArray[index++] = positions2[0];
					featurePointArray[index++] = positions2[1];

					const FeaturePoint fp = trackingData[faceIndex].featurePoints2D->getFP(groupIndex, pointIndex);

					(fp.defined) ? defined = 1.0f : defined = 0.0f;
					(fp.detected) ? detected = 1.0f : detected = 0.0f;

					featurePointArray[index++] = defined;
					featurePointArray[index++] = detected;
					featurePointArray[index++] = fp.quality;
				}
			}
		}
	}

	// returns the global feature point position, indication of whether the point is defined and detected and quality for all feature points
	void _getAllFeaturePoints3D(float* featurePointArray, int length, int faceIndex)
	{
		if (trackerStatus == NULL)
			return;

		if (trackerStatus[faceIndex] != TRACK_STAT_OK || faceIndex >= NUM_FACES)
			return;

		float defined;
		float detected;

		int index = 0;
		for (int groupIndex = FDP::FP_START_GROUP_INDEX; groupIndex <= FDP::FP_END_GROUP_INDEX; ++groupIndex)
		{
			for (int pointIndex = 1; pointIndex <= FDP::groupSize(groupIndex); ++pointIndex)
			{
				if (index < length)
				{
					const float* positions3 = trackingData[faceIndex].featurePoints3D->getFPPos(groupIndex, pointIndex);
					featurePointArray[index++] = positions3[0];
					featurePointArray[index++] = positions3[1];
					featurePointArray[index++] = positions3[2];

					const FeaturePoint fp = trackingData[faceIndex].featurePoints3D->getFP(groupIndex, pointIndex);

					(fp.defined) ? defined = 1.0f : defined = 0.0f;
					(fp.detected) ? detected = 1.0f : detected = 0.0f;

					featurePointArray[index++] = defined;
					featurePointArray[index++] = detected;
					featurePointArray[index++] = fp.quality;
				}
			}
		}
	}

	// returns the relative 3D feature point position, indication of whether the point is defined and detected and quality for all feature points
	void _getAllFeaturePoints3DRelative(float* featurePointArray, int length, int faceIndex)
	{
		if (trackerStatus == NULL)
			return;

		if (trackerStatus[faceIndex] != TRACK_STAT_OK || faceIndex >= NUM_FACES)
			return;

		float defined;
		float detected;

		int index = 0;
		for (int groupIndex = FDP::FP_START_GROUP_INDEX; groupIndex <= FDP::FP_END_GROUP_INDEX; ++groupIndex)
		{
			for (int pointIndex = 1; pointIndex <= FDP::groupSize(groupIndex); ++pointIndex)
			{
				if (index < length)
				{
					const float* positions3 = trackingData[faceIndex].featurePoints3DRelative->getFPPos(groupIndex, pointIndex);
					featurePointArray[index++] = positions3[0];
					featurePointArray[index++] = positions3[1];
					featurePointArray[index++] = positions3[2];

					const FeaturePoint fp = trackingData[faceIndex].featurePoints3DRelative->getFP(groupIndex, pointIndex);

					(fp.defined) ? defined = 1.0f : defined = 0.0f;
					(fp.detected) ? detected = 1.0f : detected = 0.0f;

					featurePointArray[index++] = defined;
					featurePointArray[index++] = detected;
					featurePointArray[index++] = fp.quality;
				}
			}
		}
	}

	// returns the feature point positions in normalized 2D screen coordinates, indication of whether the point is defined and detected and quality for specified feature points
	void _getFeaturePoints2D(int number, int* groups, int* indices, float* positions, int* defined, int* detected, float* quality, int faceIndex)
	{
		if (trackerStatus == NULL)
			return;

		if (trackerStatus[faceIndex] != TRACK_STAT_OK || faceIndex >= NUM_FACES)
			return;

		float def;
		float det;

		for (int i = 0; i < number; i++)
		{
			int group = groups[i];
			int index = indices[i];

			const float* positions2 = trackingData[faceIndex].featurePoints2D->getFPPos(group, index);
			positions[i * 2] = positions2[0];
			positions[i * 2 + 1] = positions2[1];

			const FeaturePoint fp = trackingData[faceIndex].featurePoints2D->getFP(group, index);

			(fp.defined) ? def = 1.0f : def = 0.0f;
			(fp.detected) ? det = 1.0f : det = 0.0f;

			defined[i] = def;
			detected[i] = det;
			quality[i] = fp.quality;
		}

		return;
	}

	// returns the global 3D feature point positions, indication of whether the point is defined and detected and quality for specified feature points
	void _getFeaturePoints3D(int number, int* groups, int* indices, float* positions, int* defined, int* detected, float* quality, int faceIndex)
	{
		if (trackerStatus == NULL)
			return;

		if (trackerStatus[faceIndex] != TRACK_STAT_OK || faceIndex >= NUM_FACES)
			return;

		float def;
		float det;

		for (int i = 0; i < number; i++)
		{
			int group = groups[i];
			int index = indices[i];

			const float* positions3 = trackingData[faceIndex].featurePoints3D->getFPPos(group, index);
			positions[i * 3] = positions3[0];
			positions[i * 3 + 1] = positions3[1];
			positions[i * 3 + 2] = positions3[2];

			const FeaturePoint fp = trackingData[faceIndex].featurePoints3D->getFP(group, index);

			(fp.defined) ? def = 1.0f : def = 0.0f;
			(fp.detected) ? det = 1.0f : det = 0.0f;

			defined[i] = def;
			detected[i] = det;
			quality[i] = fp.quality;
		}

		return;
	}

	// returns the relative 3D feature point positions, indication of whether the point is defined and detected and quality for specified feature points
	void _getFeaturePoints3DRelative(int number, int* groups, int* indices, float* positions, int* defined, int* detected, float* quality, int faceIndex)
	{
		if (trackerStatus == NULL)
			return;

		if (trackerStatus[faceIndex] != TRACK_STAT_OK || faceIndex >= NUM_FACES)
			return;

		float def;
		float det;

		for (int i = 0; i < number; i++)
		{
			int group = groups[i];
			int index = indices[i];

			const float* positions3 = trackingData[faceIndex].featurePoints3DRelative->getFPPos(group, index);
			positions[i * 3] = positions3[0];
			positions[i * 3 + 1] = positions3[1];
			positions[i * 3 + 2] = positions3[2];

			const FeaturePoint fp = trackingData[faceIndex].featurePoints3DRelative->getFP(group, index);

			(fp.defined) ? def = 1.0f : def = 0.0f;
			(fp.detected) ? det = 1.0f : det = 0.0f;

			defined[i] = def;
			detected[i] = det;
			quality[i] = fp.quality;
		}

		return;
	}

	// updates the pixel data in Unity with data from tracker
	void _setFrameData(char* texIDPointer)
	{
		int texWidth = GetNearestPow2(frameWidthRotated);
		int texHeight = GetNearestPow2(frameHeightRotated);
		
		int dim = frameWidthRotated*frameHeightRotated;
		for (int i = 0; i < frameHeightRotated; i++) {
			for (int j = 0; j < frameWidthRotated; j++) {
				texIDPointer[i * 4 * texWidth + j * 4 + 0] = pixels[i * 3 * frameWidthRotated + j * 3 + 0];
				texIDPointer[i * 4 * texWidth + j * 4 + 1] = pixels[i * 3 * frameWidthRotated + j * 3 + 1];
				texIDPointer[i * 4 * texWidth + j * 4 + 2] = pixels[i * 3 * frameWidthRotated + j * 3 + 2];
				texIDPointer[i * 4 * texWidth + j * 4 + 3] = -1;
			}
		}
	}
	
	//sets tracking configuration
	void _setTrackerConfiguration(const char *trackerConfigFile, bool au_fitting_disabled, bool mesh_fitting_disabled)
	{
		if (m_Tracker)
			m_Tracker->setTrackerConfiguration(trackerConfigFile, au_fitting_disabled, mesh_fitting_disabled);
	}

	//sets the inter pupillary distance. 
	void _setIPD(float IPDvalue)
	{
		if (m_Tracker)
			m_Tracker->setIPD(IPDvalue);
	}

	//returns the current inter pupillary distance (IPD) setting. 
	float _getIPD()
	{
		float ipd = -1;

		if (m_Tracker)
			ipd = m_Tracker->getIPD();

		return ipd;
	}

}
