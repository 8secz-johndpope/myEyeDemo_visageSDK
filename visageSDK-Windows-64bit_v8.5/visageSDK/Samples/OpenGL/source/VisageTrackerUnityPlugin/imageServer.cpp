#include "imageServer.h"

using namespace VisageSDK;


// destructor
ImageServer::~ImageServer(void)
{
	// delete images
	for (int i=0; i < nImages; i++) {
		cvReleaseImage(&images[i]);
	}
}


void ImageServer::AddFile(char* file)
{
	nImages = 1;
	currentImage = 0;

	images = new IplImage *[1];

	// first image is loaded separately to get image dimensions
	images[currentImage] = cvLoadImage(file);

	width = images[currentImage]->width;
	height = images[currentImage]->height;
	nChannels = images[currentImage]->nChannels;
}

// called when the tracker needs next frame
unsigned char *ImageServer::GrabFrame(long &timeStamp)
{
	timeStamp = -1;
	unsigned char *imageData = (unsigned char *) images[currentImage]->imageData;
	
	currentImage = (currentImage + 1) % nImages;

	return imageData;
	return 0;
}