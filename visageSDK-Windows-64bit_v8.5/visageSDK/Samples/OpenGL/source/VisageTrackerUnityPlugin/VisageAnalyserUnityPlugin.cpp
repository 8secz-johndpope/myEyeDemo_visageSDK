//
//	VisageTrackerUnityPlugin
//

#include "VisageAnalyserUnityPlugin.h"
#include "VisageFaceAnalyser.h"



using namespace VisageSDK;

static VisageFaceAnalyser* m_Analyser = 0;
FaceData FAtrackingData[MAX_FACES];
unsigned char* FApixels = 0;
int FAtrackerStatus[MAX_FACES];
int FAframeWidth = 0;
int FAframeHeight = 0;
VsImage* inputImage = 0;


extern "C" 
{
	// initialises face analyser
	int _initAnalyser(char* dataPath)
	{
		if (m_Analyser)
			delete m_Analyser;

		m_Analyser = new VisageFaceAnalyser();
		int is_initialized = m_Analyser->init(dataPath);

		if(is_initialized)
		{
			mtx_fa_init.lock();
			isA_initialized = true; 
			mtx_fa_init.unlock();
		}

		return is_initialized;
	}

	// releases face analyser
	void _releaseAnalyser()
	{
		delete m_Analyser; m_Analyser = 0;
		delete[] FApixels; FApixels = 0;

		if (inputImage)
		{
			vsReleaseImageHeader(&inputImage);
			inputImage = 0;
		}

		mtx_fa_init.lock();
		isA_initialized = false;
		mtx_fa_init.unlock();
	}

	// estimates emotion from a facial image
	int _estimateEmotion(float* emotions, int faceIndex)
	{
		mtx_fa_init.lock();
		if (!isA_initialized)
		{
			mtx_fa_init.unlock();
			return -1;
		}
		mtx_fa_init.unlock();

		if (frameWidthRotated == 0 || frameHeightRotated == 0)
			return -1;

		if (!FApixels)
			return -1;

		mtx_fa_init.lock();
		if (!isA_initialized || FAtrackerStatus[faceIndex] != TRACK_STAT_OK || faceIndex >= NUM_FACES)
		{
			mtx_fa_init.unlock();
			memset(FApixels, 0, FAframeWidth * FAframeHeight * N_CHANNELS);
			return -1;
		}
		mtx_fa_init.unlock();

		vsSetImageData(inputImage, (void*)FApixels, inputImage->widthStep);
			
		int estimationSuccessful = m_Analyser->estimateEmotion(inputImage, &FAtrackingData[faceIndex], emotions);

		return estimationSuccessful;
		
	}

	// estimates age from a facial image
	float _estimateAge(int faceIndex)
	{
		mtx_fa_init.lock();
		if (!isA_initialized)
		{
			mtx_fa_init.unlock();
			return -1;
		}
		mtx_fa_init.unlock();

		if (frameWidthRotated == 0 || frameHeightRotated == 0)
			return -1;

		if (!FApixels)
			return -1;

		mtx_fa_init.lock();
		if (!isA_initialized || FAtrackerStatus[faceIndex] != TRACK_STAT_OK || faceIndex >= NUM_FACES)
		{
			mtx_fa_init.unlock();
			memset(FApixels, 0, FAframeWidth * FAframeHeight * N_CHANNELS);
			return -1;
		}
		mtx_fa_init.unlock();

		vsSetImageData(inputImage, (void*)FApixels, inputImage->widthStep);

		int ageEstimated = m_Analyser->estimateAge(inputImage, &FAtrackingData[faceIndex]);

		return ageEstimated;
	}

	// estimates gender from a facial image
	int _estimateGender(int faceIndex)
	{
		mtx_fa_init.lock();
		if (!isA_initialized)
		{
			mtx_fa_init.unlock();
			return -1;
		}
		mtx_fa_init.unlock();

		if (frameWidthRotated == 0 || frameHeightRotated == 0)
			return -1;

		if (!FApixels)
			return -1;

		mtx_fa_init.lock();
		if (!isA_initialized || FAtrackerStatus[faceIndex] != TRACK_STAT_OK || faceIndex >= NUM_FACES)
		{
			mtx_fa_init.unlock();
			memset(FApixels, 0, FAframeWidth * FAframeHeight * N_CHANNELS);
			return -1;
		}
		mtx_fa_init.unlock();

		vsSetImageData(inputImage, (void*)FApixels, inputImage->widthStep);

		int genderEstimated = m_Analyser->estimateGender(inputImage, &FAtrackingData[faceIndex]);

		return genderEstimated;
	}

	//prepares data obtained by _grabFrame() and track() functions for analysis
	bool _prepareDataForAnalysis()
	{
		mtx_fa_pixels.lock();
		if (!FApixelsBuffer)
		{
			mtx_fa_pixels.unlock();
			return false;
		}

		if (FAframeWidth != frameWidthRotated || FAframeHeight != frameHeightRotated)
		{
			delete[] FApixels;
			FApixels = new unsigned char[frameWidthRotated * frameHeightRotated * N_CHANNELS];
			FAframeWidth = frameWidthRotated;
			FAframeHeight = frameHeightRotated;

			if (inputImage)
			{
				vsReleaseImageHeader(&inputImage);
			}
			inputImage = vsCreateImageHeader(vsSize(FAframeWidth, FAframeHeight), 8, N_CHANNELS);
		}

		memcpy(FApixels, FApixelsBuffer, FAframeWidth * FAframeHeight * N_CHANNELS * sizeof(unsigned char));
		mtx_fa_pixels.unlock();
		
		mtx_fa_faceData.lock();
		for (int i = 0; i < NUM_FACES; ++i)
		{
			FAtrackingData[i] = FAtrackingDataBuffer[i];
		}
		memcpy(FAtrackerStatus, FAtrackerStatusBuffer, NUM_FACES * sizeof(int));
		mtx_fa_faceData.unlock();

		return true;
	}
}
