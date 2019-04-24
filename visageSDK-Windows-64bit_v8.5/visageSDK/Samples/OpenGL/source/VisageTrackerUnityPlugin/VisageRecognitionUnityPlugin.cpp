//
//	VisageTrackerUnityPluginRecognition
//

#include "VisageRecognitionUnityPlugin.h"
#include "VisageFaceRecognition.h"
//
using namespace VisageSDK;

static VisageFaceRecognition* m_Recognition = 0;
FaceData FRtrackingData[MAX_FACES];
unsigned char* FRpixels = 0;
int FRtrackerStatus[MAX_FACES];
int FRframeWidth = 0;
int FRframeHeight = 0;
VsImage* inputImg = 0;

mutex mtx_fr_gall = {};

extern "C" 
{
	// initialises face recognition
	bool _initRecognition(char* dataPath)
	{
		if (m_Recognition)
			delete m_Recognition;

		m_Recognition = new VisageFaceRecognition(dataPath);

		mtx_fr_init.lock();
		isR_initialized = m_Recognition->is_initialized;
		mtx_fr_init.unlock();
	
		return isR_initialized;
	}

	// release memory allocated by the initRecognition function
	void _releaseRecognition()
	{
		delete m_Recognition; m_Recognition = 0;
		delete[] FRpixels; FRpixels = 0;

		if (inputImg)
		{
			vsReleaseImageHeader(&inputImg);
			inputImg = 0;
		}

		mtx_fr_init.lock();
		isR_initialized = false;
		mtx_fr_init.unlock();
	}

	// gets the descriptor's size
	int _getDescriptorSize()
	{
		if (m_Recognition->is_initialized)
			return  m_Recognition->getDescriptorSize();
		else 
			return -1;
	}

	// tracker status, FaceData and pixels
	bool _prepareDataForRecog()
	{
		mtx_fr_pixels.lock();
		if (!FRpixelsBuffer)
		{
			mtx_fr_pixels.unlock();
			return false;
		}

		if (FRframeWidth != frameWidthRotated || FRframeHeight != frameHeightRotated)
		{
			delete[] FRpixels;
			FRpixels = new unsigned char[frameWidthRotated * frameHeightRotated * N_CHANNELS];
			FRframeWidth = frameWidthRotated;
			FRframeHeight = frameHeightRotated;

			if (inputImg)
			{
				vsReleaseImageHeader(&inputImg);
			}
			inputImg = vsCreateImageHeader(vsSize(FRframeWidth, FRframeHeight), 8, N_CHANNELS);
		}

		memcpy(FRpixels, FRpixelsBuffer, FRframeWidth * FRframeHeight * N_CHANNELS * sizeof(unsigned char));
		vsSetImageData(inputImg, (void*)FRpixels, inputImg->widthStep);

		mtx_fr_pixels.unlock();

		mtx_fr_faceData.lock();
		for (int i = 0; i < NUM_FACES; ++i)
		{
			FRtrackingData[i] = FRtrackingDataBuffer[i];
		}
		memcpy(FRtrackerStatus, FRtrackerStatusBuffer, NUM_FACES * sizeof(int));
		mtx_fr_faceData.unlock();

		return true;
	}

	// extracts the face descriptor for face recognition from a facial image
	int _extractDescriptor(short* descriptor, int faceIndex)
	{
		mtx_fr_init.lock();
		if (!isR_initialized)
		{
			mtx_fr_init.unlock();
			return -1;
		}
		mtx_fr_init.unlock();

		if (frameWidthRotated == 0 || frameHeightRotated == 0)
			return -1;

		if (!FRpixels)
			return -1;

		mtx_fr_init.lock();
		if (!isR_initialized || FRtrackerStatus[faceIndex] != TRACK_STAT_OK || faceIndex >= NUM_FACES)
		{
			memset(FRpixels, 0, FRframeWidth * FRframeHeight * N_CHANNELS);
			mtx_fr_init.unlock();
			return -1;
		}
		mtx_fr_init.unlock();
		
		int success = m_Recognition->extractDescriptor(&FRtrackingData[faceIndex], inputImg, descriptor);
		
		return success;
	}

	// calculates similarity between two descriptors
	float _descriptorsSimilarity(short* firstDescrtiptor, short* secondDescriptor) 
	{
		mtx_fr_init.lock();
		if (isR_initialized)
		{
			mtx_fr_init.unlock();
			return m_Recognition->descriptorsSimilarity(firstDescrtiptor, secondDescriptor);
		}
		else
		{
			mtx_fr_init.unlock();
			return -1;
		}
	}

	// adds face descriptor to the gallery
	int _addDescriptor(short* descriptor, const char* name)
	{
		mtx_fr_init.lock();
		if (isR_initialized)
		{
			mtx_fr_init.unlock();

			mtx_fr_gall.lock();
			int succ = m_Recognition->addDescriptor(descriptor, name);
			mtx_fr_gall.unlock();

			return succ;
		
		}
		else
		{
			mtx_fr_init.unlock();
			return -1;
		}
	}

	// gets number of descriptors in the gallery
	int _getDescriptorCount()
	{
		mtx_fr_init.lock();
		if (isR_initialized)
		{
			mtx_fr_init.unlock();
			return m_Recognition->getDescriptorCount();
		}
		else
		{
			mtx_fr_init.unlock();
			return -1;
		}
	}

	// gets the name of a descriptor at the given index in the gallery
	const char* _getDescriptorName(int index)
	{
		mtx_fr_init.lock();
		if (isR_initialized)
		{
			char name[20];
			int success = m_Recognition->getDescriptorName(name, index);
			if (success)
			{
				mtx_fr_init.unlock();
				return MakeStringCopy(name);
			}
		}
		mtx_fr_init.unlock();
		return MakeStringCopy("");;	
	}

	// replaces the name of a descriptor at the given index in the gallery with new name
	int _replaceDescriptorName(const char* name, int index)
	{
		mtx_fr_init.lock();
		if (isR_initialized)
		{
			mtx_fr_init.unlock();

			mtx_fr_gall.lock();
			int succ = m_Recognition->replaceDescriptorName(name, index);
			mtx_fr_gall.unlock();

			return succ;
		}
		else
		{
			mtx_fr_init.unlock();
			return -1;
		}
	}

	// removes a descriptor at the given index from the gallery
	int _removeDescriptor(int index)
	{
		mtx_fr_init.lock();
		if (isR_initialized)
		{
			mtx_fr_init.unlock();

			mtx_fr_gall.lock();
			int succ = m_Recognition->removeDescriptor(index);
			mtx_fr_gall.unlock();

			return succ;
			
		}
		else
		{
			mtx_fr_init.unlock();
			return -1;
		}
	}

	// save gallery as a binary file
	int _saveGallery(const char* file_name)
	{
		mtx_fr_init.lock();
		if (isR_initialized)
		{
			mtx_fr_init.unlock();

			mtx_fr_gall.lock();
			int succ = m_Recognition->saveGallery(file_name);
			mtx_fr_gall.unlock();

			return succ;
		}
		else
		{
			mtx_fr_init.unlock();
			return -1;
		}		
	}

	// load gallery from a binary file
	int _loadGallery(const char* file_name)
	{
		mtx_fr_init.lock();
		if (isR_initialized)
		{
			mtx_fr_init.unlock();

			mtx_fr_gall.lock();
			int succ = m_Recognition->loadGallery(file_name);
			mtx_fr_gall.unlock();

			return succ;
			
		}
		else
		{
			mtx_fr_init.unlock();
			return -1;
		}
	}

	// clear all face descriptors from the gallery
	void _resetGallery()
	{
		mtx_fr_init.lock();
		if (isR_initialized)
		{
			mtx_fr_gall.lock();
			m_Recognition->resetGallery();
			mtx_fr_gall.unlock();
		}
		mtx_fr_init.unlock();
	}

	// compares a face to all faces in the current gallery and return n names of the most similar faces
	int _recognize(short* descriptor, int n, char** names, int cap, float* similarities)
	{
		mtx_fr_init.lock();
		if (isR_initialized)
		{
			mtx_fr_init.unlock();
			const char **namesarray = new const char*[n];
			int numOfFaces = m_Recognition->recognize(descriptor, n, namesarray, similarities);

			for (int i = 0; i < numOfFaces; ++i)
			{
				if (namesarray[i] != NULL)
				{
					if (cap > strlen(namesarray[i]))
						strcpy(names[i], namesarray[i]);
					else
						strncpy(names[i], namesarray[i], cap);
				}
			}

			delete[] namesarray;

			return numOfFaces;
		}
		mtx_fr_init.unlock();
		return 0;

	}

}
