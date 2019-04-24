#pragma once

#include "visageVision.h"

using namespace VisageSDK;

class ImageServer
{
	private:
		IplImage **images;
		int nImages;
		int currentImage;

	public:
		int width;
		int height;
		int nChannels;

		void AddFile(char *file);
		~ImageServer(void);
		unsigned char *GrabFrame(long &timeStamp);
};
