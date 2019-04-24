#pragma once
#include "VisageTracker.h"
#include <mutex>
#include <thread> 

const int MAX_FACES = 20;
const int N_CHANNELS = 3;
extern VisageSDK::FaceData trackingData[MAX_FACES];
extern VisageSDK::FaceData FRtrackingDataBuffer[MAX_FACES];
extern VisageSDK::FaceData FAtrackingDataBuffer[MAX_FACES];
extern int *trackerStatus;
extern int *ArrayOfZeros;
extern int FRtrackerStatusBuffer[MAX_FACES];
extern int FAtrackerStatusBuffer[MAX_FACES];
extern unsigned char* pixels;
extern unsigned char* FRpixelsBuffer;
extern unsigned char* FApixelsBuffer;
extern int frameWidthRotated;
extern int frameHeightRotated;
extern int NUM_FACES;
//
extern mutex mtx_fr_pixels;
extern mutex mtx_fr_faceData;
extern mutex mtx_fr_init;
//
extern mutex mtx_fa_pixels;
extern mutex mtx_fa_faceData;
extern mutex mtx_fa_init;
//
extern bool isR_initialized;
extern bool isA_initialized;
//
typedef void(*FuncPtr)(const char *);
extern FuncPtr Debug;

//
// Helper method to create C string copy
static char* MakeStringCopy(const char* val)
{
	if (val == NULL)
		return NULL;

	char* res = (char*)malloc(strlen(val) + 1);
	strcpy(res, val);
	return res;
}