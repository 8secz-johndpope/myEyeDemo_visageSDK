#include "StdAfx.h"
#include "visageVision.h"
#include "observer.h"
#include "imageServer.h"
#include "LipSync.h"
#include "FolderManager.h"

#if _MSC_VER
// ensures valid dll function export
#define EXPORT_API __declspec(dllexport)
#endif

using namespace VisageSDK;

static const float RAD_TO_DEG = 57.2957795f;
const int MaxVertices = 1024;
const int MaxTriangles = 2048;
static VisageTracker* tracker;
static Observer* observer;
static ImageServer* imageServer = 0;

// prevents name mangling
extern "C"
{
	// initializes the tracker with the given config
	EXPORT_API int Initialize(char* config)
	{

		visageFolderManager.init();
		
		// simple test to check if a valid OpenGL context exists
		GLint redBits=0;
		glGetIntegerv (GL_RED_BITS, &redBits);
		GLenum error = glGetError();

		if (error != GL_NO_ERROR)
			return -2;

		// create tracker
		tracker = new VisageTracker(config);
		
		if (tracker == NULL)
			return -1;

		// create observer
		observer = new Observer(tracker);

		// attach observer to tracker
		tracker->attach(observer);

		return 0;
	}

	// checks if the tracker is active
	EXPORT_API bool IsTracking()
	{
		return tracker != NULL && observer != NULL && observer->TrackStatus != TRACK_STAT_OFF && observer->TrackStatus != TRACK_STAT_RECOVERING;
	}

	// starts tracking from a video file
	EXPORT_API bool TrackFromVideo(char* avi)
	{
		if (tracker == NULL)
			return false;

		return tracker->trackFromAvi(avi);
	}

	// starts tracking from a file
	EXPORT_API bool TrackFromImage(char* image)
	{
		if (tracker == NULL)
			return false;

		if (imageServer == 0)
			imageServer = new ImageServer();

		imageServer->AddFile(image);

		int format = 0;

		switch (imageServer->nChannels) {
			case 3:
				format = VISAGE_FRAMEGRABBER_FMT_RGB;
				break;
			case 1:
				format = VISAGE_FRAMEGRABBER_FMT_LUMINANCE;
				break;
			default:
				return false;
		}

		return tracker->trackFromRawImages(imageServer, imageServer->width, imageServer->height, format, VISAGE_FRAMEGRABBER_ORIGIN_TL);
	}

	// starts tracking from the camera
	EXPORT_API bool TrackFromCam()
	{
		if (tracker == NULL)
			return false;

		return tracker->trackFromCam();
	}

	// stops tracking
	EXPORT_API void StopTracking()
	{
		if (tracker != NULL && tracker->isActive()) {
			tracker->stop();
			observer->ReleaseAndReInitBuffers();
			// comment the following lines to keep the same imageServer for all sessions
			delete imageServer;
			imageServer = 0;
		}
	}

	// gets the current translation
	EXPORT_API void GetTranslation(float* x, float* y, float* z)
	{
		*x = -observer->Translation[0];
		*y = observer->Translation[1];
		*z = observer->Translation[2];
	}

	// gets the current rotation
	EXPORT_API void GetRotation(float* x, float* y, float* z)
	{
		*x = -observer->Rotation[0] * RAD_TO_DEG;
		*y = -observer->Rotation[1] * RAD_TO_DEG;
		*z = observer->Rotation[2] * RAD_TO_DEG;
	}

	// returns the tracker's focus
	EXPORT_API float GetTrackerFocus()
	{
		if (observer == NULL || tracker == NULL)
			return 0.0f;

		return observer->F;
	}

	// gets the current image width
	EXPORT_API int GetImageWidth()
	{
		if (observer == NULL || tracker == NULL)
			return 0;

		return observer->ImageWidth;
	}

	// gets the current image height
	EXPORT_API int GetImageHeight()
	{
		if (observer == NULL || tracker == NULL)
			return 0;

		return observer->ImageHeight;
	}

	// returns the tracker status
	EXPORT_API int GetStatus()
	{
		if (observer == NULL)
			return 0;

		return observer->TrackStatus;
	}

	// gets the current vertex number
	EXPORT_API int GetVertexNumber()
	{
		if (observer == NULL || tracker == NULL)
			return 0;

		return observer->VertexNumber;
	}

	// gets a specified vertex
	EXPORT_API void GetVertex(int n, float vertex[3])
	{
		if (observer == NULL || tracker == NULL || n >= GetVertexNumber())
			return;

		vertex[0] = observer->Vertices[3 * n];
		vertex[1] = observer->Vertices[3 * n + 1];
		vertex[2] = observer->Vertices[3 * n + 2];
	}

	// gets all vertices thru a static array
	EXPORT_API void GetVertices(float vertices[MaxVertices * 3])
	{
		int n = GetVertexNumber();

		if (n == 0)
			return;

		memcpy(vertices, observer->Vertices, n * 3 * sizeof(float));
	}

	// gets the current tri number
	EXPORT_API int GetTriangleNumber()
	{
		if (observer == NULL || tracker == NULL)
			return 0;

		return observer->TriangleNumber;
	}

	// gets all tris thru a static array
	EXPORT_API void GetTriangles(int triangles[MaxTriangles * 3])
	{
		int n = GetTriangleNumber();

		if (n == 0)
			return;

		// get triangles in reverse order
		int *trianglesEnd = &triangles[n * 3 - 1];
		for(int i = 0; i < n * 3; i++)
		{
			*trianglesEnd = observer->Triangles[i];
			trianglesEnd--;
		}
	}

	// a helper function to get all the needed info in one native call
	EXPORT_API bool Get(int* vertexNumber, float* vertices, int* triangleNumber, int* triangles, float* texCoord)
	{
		if (observer == NULL || tracker == NULL || observer->TrackStatus == TRACK_STAT_OFF)
			return false;

		// get vertex number
		*vertexNumber = observer->VertexNumber;

		// get vertices
		memcpy(vertices, observer->Vertices, *vertexNumber * 3 * sizeof(float));

		// get triangle number
		*triangleNumber = observer->TriangleNumber;

		// get triangles in reverse order
		int *trianglesEnd = &triangles[*triangleNumber * 3 - 1];
		for(int i = 0; i < *triangleNumber * 3; i++)
		{
			*trianglesEnd = observer->Triangles[i];
			trianglesEnd--;
		}

		// get texture coordinates
		memcpy(texCoord, observer->TexCoords, *vertexNumber * 2 * sizeof(float));

		return true;
	}

	// binds a texture with the given native hardware texture id thru opengl
	EXPORT_API void BindTexture(int texID)
	{
		observer->InBindTexture = true;
		
		if (observer->CurrentBuffer == 'A' && observer->ImageA != 0)
		{
			observer->ReadBuffer = 'A';
			glBindTexture(GL_TEXTURE_2D, texID);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, observer->ImageWidth, observer->ImageHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, observer->ImageA->imageData);
			observer->ReadBuffer = 'N';
		}
		else if (observer->CurrentBuffer == 'B' && observer->ImageB != 0)
		{
			observer->ReadBuffer = 'B';
			glBindTexture(GL_TEXTURE_2D, texID);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, observer->ImageWidth, observer->ImageHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, observer->ImageB->imageData);
			observer->ReadBuffer = 'N';
		}

		observer->InBindTexture = false;
	}
}

