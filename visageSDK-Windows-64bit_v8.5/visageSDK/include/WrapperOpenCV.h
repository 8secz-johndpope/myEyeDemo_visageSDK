/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
// Copyright (C) 2009, Willow Garage Inc., all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#pragma once

#ifdef VISAGE_STATIC
	#define VISAGE_DECLSPEC
#else

	#ifdef VISAGE_EXPORTS
		#define VISAGE_DECLSPEC __declspec(dllexport)
	#else
		#define VISAGE_DECLSPEC __declspec(dllimport)
	#endif

#endif

#include "vs_main.h"
//forward declarations

//#include "cv.h"
//#include "highgui.h"

#if !defined(EMSCRIPTEN)
//#include <highgui.h>
#endif

typedef struct CvCapture CvCapture;

enum
{
	VS_LOAD_IMAGE_UNCHANGED  =-1,
	VS_LOAD_IMAGE_GRAYSCALE  =0,
	VS_LOAD_IMAGE_COLOR      =1,
	VS_LOAD_IMAGE_ANYDEPTH   =2,
	VS_LOAD_IMAGE_ANYCOLOR   =4
};

// ---------  YV ---------
enum
{
	//These 3 flags are used by vsSet/GetWindowProperty
	VS_WND_PROP_FULLSCREEN = 0, //to change/get window's fullscreen property
	VS_WND_PROP_AUTOSIZE   = 1, //to change/get window's autosize property
	VS_WND_PROP_ASPECTRATIO= 2, //to change/get window's aspectratio property
	VS_WND_PROP_OPENGL     = 3, //to change/get window's opengl support

	//These 2 flags are used by cvNamedWindow and vsSet/GetWindowProperty
	VS_WINDOW_NORMAL       = 0x00000000, //the user can resize the window (no constraint)  / also use to switch a fullscreen window to a normal size
	VS_WINDOW_AUTOSIZE     = 0x00000001, //the user cannot resize the window, the size is constrainted by the image displayed
	VS_WINDOW_OPENGL       = 0x00001000, //window with opengl support

	//Those flags are only for Qt
	VS_GUI_EXPANDED         = 0x00000000, //status bar and tool bar
	VS_GUI_NORMAL           = 0x00000010, //old fashious way

	//These 3 flags are used by cvNamedWindow and vsSet/GetWindowProperty
	VS_WINDOW_FULLSCREEN   = 1,//change the window to fullscreen
	VS_WINDOW_FREERATIO    = 0x00000100,//the image expends as much as it can (no ratio constraint)
	VS_WINDOW_KEEPRATIO    = 0x00000000//the ration image is respected.
};

enum
{
	// modes of the controlling registers (can be: auto, manual, auto single push, absolute Latter allowed with any other mode)
	// every feature can have only one mode turned on at a time
	VS_CAP_PROP_DC1394_OFF         = -4,  //turn the feature off (not controlled manually nor automatically)
	VS_CAP_PROP_DC1394_MODE_MANUAL = -3, //set automatically when a value of the feature is set by the user
	VS_CAP_PROP_DC1394_MODE_AUTO = -2,
	VS_CAP_PROP_DC1394_MODE_ONE_PUSH_AUTO = -1,
	VS_CAP_PROP_POS_MSEC       =0,
	VS_CAP_PROP_POS_FRAMES     =1,
	VS_CAP_PROP_POS_AVI_RATIO  =2,
	VS_CAP_PROP_FRAME_WIDTH    =3,
	VS_CAP_PROP_FRAME_HEIGHT   =4,
	VS_CAP_PROP_FPS            =5,
	VS_CAP_PROP_FOURCC         =6,
	VS_CAP_PROP_FRAME_COUNT    =7,
	VS_CAP_PROP_FORMAT         =8,
	VS_CAP_PROP_MODE           =9,
	VS_CAP_PROP_BRIGHTNESS    =10,
	VS_CAP_PROP_CONTRAST      =11,
	VS_CAP_PROP_SATURATION    =12,
	VS_CAP_PROP_HUE           =13,
	VS_CAP_PROP_GAIN          =14,
	VS_CAP_PROP_EXPOSURE      =15,
	VS_CAP_PROP_CONVERT_RGB   =16,
	VS_CAP_PROP_WHITE_BALANCE_U =17,
	VS_CAP_PROP_RECTIFICATION =18,
	VS_CAP_PROP_MONOCROME     =19,
	VS_CAP_PROP_SHARPNESS     =20,
	VS_CAP_PROP_AUTO_EXPOSURE =21, // exposure control done by camera,
								   // user can adjust refernce level
								   // using this feature
	VS_CAP_PROP_GAMMA         =22,
	VS_CAP_PROP_TEMPERATURE   =23,
	VS_CAP_PROP_TRIGGER       =24,
	VS_CAP_PROP_TRIGGER_DELAY =25,
	VS_CAP_PROP_WHITE_BALANCE_V =26,
	VS_CAP_PROP_ZOOM          =27,
	VS_CAP_PROP_FOCUS         =28,
	VS_CAP_PROP_GUID          =29,
	VS_CAP_PROP_ISO_SPEED     =30,
	VS_CAP_PROP_MAX_DC1394    =31,
	VS_CAP_PROP_BACKLIGHT     =32,
	VS_CAP_PROP_PAN           =33,
	VS_CAP_PROP_TILT          =34,
	VS_CAP_PROP_ROLL          =35,
	VS_CAP_PROP_IRIS          =36,
	VS_CAP_PROP_SETTINGS      =37,
	VS_CAP_PROP_BUFFERSIZE    =38,

	VS_CAP_PROP_AUTOGRAB      =1024, // property for highgui class CvCapture_Android only
	VS_CAP_PROP_SUPPORTED_PREVIEW_SIZES_STRING=1025, // readonly, tricky property, returns cpnst char* indeed
	VS_CAP_PROP_PREVIEW_FORMAT=1026, // readonly, tricky property, returns cpnst char* indeed

	// OpenNI map generators
	VS_CAP_OPENNI_DEPTH_GENERATOR = 1 << 31,
	VS_CAP_OPENNI_IMAGE_GENERATOR = 1 << 30,
	VS_CAP_OPENNI_GENERATORS_MASK = VS_CAP_OPENNI_DEPTH_GENERATOR + VS_CAP_OPENNI_IMAGE_GENERATOR,

	// Properties of cameras available through OpenNI interfaces
	VS_CAP_PROP_OPENNI_OUTPUT_MODE     = 100,
	VS_CAP_PROP_OPENNI_FRAME_MAX_DEPTH = 101, // in mm
	VS_CAP_PROP_OPENNI_BASELINE        = 102, // in mm
	VS_CAP_PROP_OPENNI_FOCAL_LENGTH    = 103, // in pixels
	VS_CAP_PROP_OPENNI_REGISTRATION    = 104, // flag
	VS_CAP_PROP_OPENNI_REGISTRATION_ON = VS_CAP_PROP_OPENNI_REGISTRATION, // flag that synchronizes the remapping depth map to image map
																		  // by changing depth generator's view point (if the flag is "on") or
																		  // sets this view point to its normal one (if the flag is "off").
	VS_CAP_PROP_OPENNI_APPROX_FRAME_SYNC = 105,
	VS_CAP_PROP_OPENNI_MAX_BUFFER_SIZE   = 106,
	VS_CAP_PROP_OPENNI_CIRCLE_BUFFER     = 107,
	VS_CAP_PROP_OPENNI_MAX_TIME_DURATION = 108,

	VS_CAP_PROP_OPENNI_GENERATOR_PRESENT = 109,

	VS_CAP_OPENNI_IMAGE_GENERATOR_PRESENT         = VS_CAP_OPENNI_IMAGE_GENERATOR + VS_CAP_PROP_OPENNI_GENERATOR_PRESENT,
	VS_CAP_OPENNI_IMAGE_GENERATOR_OUTPUT_MODE     = VS_CAP_OPENNI_IMAGE_GENERATOR + VS_CAP_PROP_OPENNI_OUTPUT_MODE,
	VS_CAP_OPENNI_DEPTH_GENERATOR_BASELINE        = VS_CAP_OPENNI_DEPTH_GENERATOR + VS_CAP_PROP_OPENNI_BASELINE,
	VS_CAP_OPENNI_DEPTH_GENERATOR_FOCAL_LENGTH    = VS_CAP_OPENNI_DEPTH_GENERATOR + VS_CAP_PROP_OPENNI_FOCAL_LENGTH,
	VS_CAP_OPENNI_DEPTH_GENERATOR_REGISTRATION    = VS_CAP_OPENNI_DEPTH_GENERATOR + VS_CAP_PROP_OPENNI_REGISTRATION,
	VS_CAP_OPENNI_DEPTH_GENERATOR_REGISTRATION_ON = VS_CAP_OPENNI_DEPTH_GENERATOR_REGISTRATION,

	// Properties of cameras available through GStreamer interface
	VS_CAP_GSTREAMER_QUEUE_LENGTH   = 200, // default is 1
	VS_CAP_PROP_PVAPI_MULTICASTIP   = 300, // ip for anable multicast master mode. 0 for disable multicast

	// Properties of cameras available through XIMEA SDK interface
	VS_CAP_PROP_XI_DOWNSAMPLING  = 400,      // Change image resolution by binning or skipping.
	VS_CAP_PROP_XI_DATA_FORMAT   = 401,       // Output data format.
	VS_CAP_PROP_XI_OFFSET_X      = 402,      // Horizontal offset from the origin to the area of interest (in pixels).
	VS_CAP_PROP_XI_OFFSET_Y      = 403,      // Vertical offset from the origin to the area of interest (in pixels).
	VS_CAP_PROP_XI_TRG_SOURCE    = 404,      // Defines source of trigger.
	VS_CAP_PROP_XI_TRG_SOFTWARE  = 405,      // Generates an internal trigger. PRM_TRG_SOURCE must be set to TRG_SOFTWARE.
	VS_CAP_PROP_XI_GPI_SELECTOR  = 406,      // Selects general purpose input
	VS_CAP_PROP_XI_GPI_MODE      = 407,      // Set general purpose input mode
	VS_CAP_PROP_XI_GPI_LEVEL     = 408,      // Get general purpose level
	VS_CAP_PROP_XI_GPO_SELECTOR  = 409,      // Selects general purpose output
	VS_CAP_PROP_XI_GPO_MODE      = 410,      // Set general purpose output mode
	VS_CAP_PROP_XI_LED_SELECTOR  = 411,      // Selects camera signalling LED
	VS_CAP_PROP_XI_LED_MODE      = 412,      // Define camera signalling LED functionality
	VS_CAP_PROP_XI_MANUAL_WB     = 413,      // Calculates White Balance(must be called during acquisition)
	VS_CAP_PROP_XI_AUTO_WB       = 414,      // Automatic white balance
	VS_CAP_PROP_XI_AEAG          = 415,      // Automatic exposure/gain
	VS_CAP_PROP_XI_EXP_PRIORITY  = 416,      // Exposure priority (0.5 - exposure 50%, gain 50%).
	VS_CAP_PROP_XI_AE_MAX_LIMIT  = 417,      // Maximum limit of exposure in AEAG procedure
	VS_CAP_PROP_XI_AG_MAX_LIMIT  = 418,      // Maximum limit of gain in AEAG procedure
	VS_CAP_PROP_XI_AEAG_LEVEL    = 419,       // Average intensity of output signal AEAG should achieve(in %)
	VS_CAP_PROP_XI_TIMEOUT       = 420,       // Image capture timeout in milliseconds

	// Properties for Android cameras
	VS_CAP_PROP_ANDROID_FLASH_MODE = 8001,
	VS_CAP_PROP_ANDROID_FOCUS_MODE = 8002,
	VS_CAP_PROP_ANDROID_WHITE_BALANCE = 8003,
	VS_CAP_PROP_ANDROID_ANTIBANDING = 8004,
	VS_CAP_PROP_ANDROID_FOCAL_LENGTH = 8005,
	VS_CAP_PROP_ANDROID_FOCUS_DISTANCE_NEAR = 8006,
	VS_CAP_PROP_ANDROID_FOCUS_DISTANCE_OPTIMAL = 8007,
	VS_CAP_PROP_ANDROID_FOCUS_DISTANCE_FAR = 8008,
	VS_CAP_PROP_ANDROID_EXPOSE_LOCK = 8009,
	VS_CAP_PROP_ANDROID_WHITEBALANCE_LOCK = 8010,

	// Properties of cameras available through AVFOUNDATION interface
	VS_CAP_PROP_IOS_DEVICE_FOCUS = 9001,
	VS_CAP_PROP_IOS_DEVICE_EXPOSURE = 9002,
	VS_CAP_PROP_IOS_DEVICE_FLASH = 9003,
	VS_CAP_PROP_IOS_DEVICE_WHITEBALANCE = 9004,
	VS_CAP_PROP_IOS_DEVICE_TORCH = 9005,

	// Properties of cameras available through Smartek Giganetix Ethernet Vision interface
	/* --- Vladimir Litvinenko (litvinenko.vladimir@gmail.com) --- */
	VS_CAP_PROP_GIGA_FRAME_OFFSET_X = 10001,
	VS_CAP_PROP_GIGA_FRAME_OFFSET_Y = 10002,
	VS_CAP_PROP_GIGA_FRAME_WIDTH_MAX = 10003,
	VS_CAP_PROP_GIGA_FRAME_HEIGH_MAX = 10004,
	VS_CAP_PROP_GIGA_FRAME_SENS_WIDTH = 10005,
	VS_CAP_PROP_GIGA_FRAME_SENS_HEIGH = 10006,

	VS_CAP_PROP_INTELPERC_PROFILE_COUNT               = 11001,
	VS_CAP_PROP_INTELPERC_PROFILE_IDX                 = 11002,
	VS_CAP_PROP_INTELPERC_DEPTH_LOW_CONFIDENCE_VALUE  = 11003,
	VS_CAP_PROP_INTELPERC_DEPTH_SATURATION_VALUE      = 11004,
	VS_CAP_PROP_INTELPERC_DEPTH_CONFIDENCE_THRESHOLD  = 11005,
	VS_CAP_PROP_INTELPERC_DEPTH_FOCAL_LENGTH_HORZ     = 11006,
	VS_CAP_PROP_INTELPERC_DEPTH_FOCAL_LENGTH_VERT     = 11007,

	// Intel PerC streams
	VS_CAP_INTELPERC_DEPTH_GENERATOR = 1 << 29,
	VS_CAP_INTELPERC_IMAGE_GENERATOR = 1 << 28,
	VS_CAP_INTELPERC_GENERATORS_MASK = VS_CAP_INTELPERC_DEPTH_GENERATOR + VS_CAP_INTELPERC_IMAGE_GENERATOR
};

enum {
	VS_CAP_ANDROID_GREY_FRAME,
	VS_CAP_ANDROID_COLOR_FRAME_RGB,
	VS_CAP_ANDROID
};

#define vsSetImageData      vsSetData
#define vsCopyImage( src, dst )         vsCopy( src, dst, 0 )

#define vsGetImageRawData   vsGetRawData

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(EMSCRIPTEN) && !defined(FLASH) 
VISAGE_DECLSPEC CvCapture* vsCreateCameraCapture( int index );
VISAGE_DECLSPEC CvCapture* vsCreateFileCapture( const char* filename );
VISAGE_DECLSPEC VsImage* vsLoadImage( const char* filename, int iscolor VS_DEFAULT(VS_LOAD_IMAGE_COLOR));
VISAGE_DECLSPEC VsImage* vsRetrieveFrame( CvCapture* capture, int streamIdx VS_DEFAULT(0) );
VISAGE_DECLSPEC double vsGetCaptureProperty( CvCapture* capture, int property_id );
VISAGE_DECLSPEC int    vsSetCaptureProperty( CvCapture* capture, int property_id, double value );
VISAGE_DECLSPEC int  vsGrabFrame( CvCapture* capture );
VISAGE_DECLSPEC void  vsReleaseCapture( CvCapture** capture );
VISAGE_DECLSPEC int  vsNamedWindow( const char* name, int flags VS_DEFAULT(VS_WINDOW_AUTOSIZE) );
#define vsCaptureFromFile vsCreateFileCapture
#define vsCaptureFromCAM vsCreateCameraCapture
#endif


VISAGE_DECLSPEC VsMat*  vs2DRotationMatrix( VsPoint2D32f center, double angle, double scale, VsMat* map_matrix );
//VISAGE_DECLSPEC void  vsCalcOpticalFlowPyrLK( const VsArr*  prev, const VsArr*  curr, VsArr*  prev_pyr, VsArr*  curr_pyr, const VsPoint2D32f* prev_features, VsPoint2D32f* curr_features, int       count, VsSize    win_size, int       level, char*     status, float*    track_error, VsTermCriteria criteria, int       flags );
VISAGE_DECLSPEC double  vsThreshold( const VsArr*  src, VsArr*  dst, double  threshold, double  max_value, int threshold_type );
VISAGE_DECLSPEC int vsWaitKey(int delay VS_DEFAULT(0));
VISAGE_DECLSPEC void  vsDestroyWindow( const char* name );
VISAGE_DECLSPEC void  vsEqualizeHist( const VsArr* src, VsArr* dst );
//VISAGE_DECLSPEC void  vsCvtColor( const VsArr* src, VsArr* dst, int code );
//VISAGE_DECLSPEC void  vsCircle( VsArr* img, VsPoint center, int radius, VsScalar color, int thickness VS_DEFAULT(1), int line_type VS_DEFAULT(8), int shift VS_DEFAULT(0));
//VISAGE_DECLSPEC void  vsInitFont( VsFont* font, int font_face, double hscale, double vscale, double shear VS_DEFAULT(0), int thickness VS_DEFAULT(1), int line_type VS_DEFAULT(8));
//VISAGE_DECLSPEC void  vsWarpAffine( const VsArr* src, VsArr* dst, const VsMat* map_matrix, int flags VS_DEFAULT(VS_INTER_LINEAR+VS_WARP_FILL_OUTLIERS), VsScalar fillval VS_DEFAULT(vsScalarAll(0)) );


/*

//VISAGE_DECLSPEC VsMat*  vsCloneMat( const VsMat* mat );
//VISAGE_DECLSPEC VsMat*  vsCreateMat( int rows, int cols, int type );
VISAGE_DECLSPEC VsMat* vsGetMat(const VsArr* arr, VsMat* header, int* coi VS_DEFAULT(NULL), int allowND VS_DEFAULT(0));
VISAGE_DECLSPEC VsRect vsGetImageROI( const VsImage* image );
VISAGE_DECLSPEC VsScalar  vsAvg( const VsArr* arr, const VsArr* mask VS_DEFAULT(NULL) );
VISAGE_DECLSPEC VsScalar  vsGet1D( const VsArr* arr, int idx0 );
VISAGE_DECLSPEC VsScalar  vsGet2D( const VsArr* arr, int idx0, int idx1 );
VISAGE_DECLSPEC VsScalar  vsSum( const VsArr* arr );
VISAGE_DECLSPEC VsSize vsGetSize( const VsArr* arr );
VISAGE_DECLSPEC VsImage*  vsCreateImage( VsSize size, int depth, int channels );
VISAGE_DECLSPEC VsImage*  vsCreateImageHeader( VsSize size, int depth, int channels );
VISAGE_DECLSPEC VsImage* vsCloneImage( const VsImage* image );
VISAGE_DECLSPEC double  vsInvert( const VsArr* src, VsArr* dst, int method VS_DEFAULT(VS_LU));
VISAGE_DECLSPEC double  vsNorm( const VsArr* arr1, const VsArr* arr2 VS_DEFAULT(NULL), int norm_type VS_DEFAULT(VS_L2), const VsArr* mask VS_DEFAULT(NULL) );

VISAGE_DECLSPEC int  vsSolve( const VsArr* src1, const VsArr* src2, VsArr* dst, int method VS_DEFAULT(VS_LU));

VISAGE_DECLSPEC void  vsAddS( const VsArr* src, VsScalar value, VsArr* dst, const VsArr* mask VS_DEFAULT(NULL));
VISAGE_DECLSPEC void  vsAddWeighted( const VsArr* src1, double alpha, const VsArr* src2, double beta, double gamma, VsArr* dst );
VISAGE_DECLSPEC void  vsAvgSdv( const VsArr* arr, VsScalar* mean, VsScalar* std_dev, const VsArr* mask VS_DEFAULT(NULL) );


VISAGE_DECLSPEC void  vsConvertScale( const VsArr* src, VsArr* dst, double scale VS_DEFAULT(1), double shift VS_DEFAULT(0) );
VISAGE_DECLSPEC void  vsConvertScaleAbs( const VsArr* src, VsArr* dst, double scale VS_DEFAULT(1), double shift VS_DEFAULT(0) );
VISAGE_DECLSPEC void  vsCopy( const VsArr* src, VsArr* dst, const VsArr* mask VS_DEFAULT(NULL) );

VISAGE_DECLSPEC void  vsFlip( const VsArr* src, VsArr* dst VS_DEFAULT(NULL), int flip_mode VS_DEFAULT(0));
VISAGE_DECLSPEC void  vsGEMM( const VsArr* src1, const VsArr* src2, double alpha, const VsArr* src3, double beta, VsArr* dst, int tABC VS_DEFAULT(0));


VISAGE_DECLSPEC void  vsMinMaxLoc( const VsArr* arr, double* min_val, double* max_val, VsPoint* min_loc VS_DEFAULT(NULL), VsPoint* max_loc VS_DEFAULT(NULL), const VsArr* mask VS_DEFAULT(NULL) );
VISAGE_DECLSPEC void  vsReleaseImage( VsImage** image );
VISAGE_DECLSPEC void  vsReleaseImageHeader( VsImage** image );
VISAGE_DECLSPEC void  vsReleaseMat( VsMat** mat );
VISAGE_DECLSPEC void  vsResetImageROI( VsImage* image );

VISAGE_DECLSPEC void  vsScaleAdd( const VsArr* src1, VsScalar scale, const VsArr* src2, VsArr* dst );
VISAGE_DECLSPEC void  vsSet( VsArr* arr, VsScalar value, const VsArr* mask VS_DEFAULT(NULL) );
VISAGE_DECLSPEC void  vsSetData( VsArr* arr, void* data, int step );
VISAGE_DECLSPEC void  vsSetIdentity( VsArr* mat, VsScalar value VS_DEFAULT(vsRealScalar(1)) );
VISAGE_DECLSPEC void  vsSetImageROI( VsImage* image, VsRect rect );
VISAGE_DECLSPEC void  vsSetZero( VsArr* arr );
VISAGE_DECLSPEC void  vsTranspose( const VsArr* src, VsArr* dst );
VISAGE_DECLSPEC void vsAnd( const VsArr* src1, const VsArr* src2, VsArr* dst, const VsArr* mask VS_DEFAULT(NULL));
VISAGE_DECLSPEC void vsGetRawData( const VsArr* arr, uchar** data, int* step VS_DEFAULT(NULL), VsSize* roi_size VS_DEFAULT(NULL));
VISAGE_DECLSPEC void vsSet1D( VsArr* arr, int idx0, VsScalar value );
VISAGE_DECLSPEC void vsSet2D( VsArr* arr, int idx0, int idx1, VsScalar value );

VISAGE_DECLSPEC double vsmGet( const VsMat* mat, int row, int col );
VISAGE_DECLSPEC void   vsmSet( VsMat* mat, int row, int col, double value );
*/

//VISAGE_DECLSPEC void  vsLine( VsArr* img, VsPoint pt1, VsPoint pt2, VsScalar color, int thickness VS_DEFAULT(1), int line_type VS_DEFAULT(8), int shift VS_DEFAULT(0) );
//VISAGE_DECLSPEC void  vsPutText( VsArr* img, const char* text, VsPoint org, const VsFont* font, VsScalar color );
//VISAGE_DECLSPEC void  vsRectangle( VsArr* img, VsPoint pt1, VsPoint pt2, VsScalar color, int thickness VS_DEFAULT(1), int line_type VS_DEFAULT(8), int shift VS_DEFAULT(0));
//VISAGE_DECLSPEC void  vsResize( const VsArr* src, VsArr* dst, int interpolation VS_DEFAULT( VS_INTER_LINEAR ));
VISAGE_DECLSPEC void vsSobel( const VsArr* src, VsArr* dst, int xorder, int yorder, int aperture_size VS_DEFAULT(3));
VISAGE_DECLSPEC void vsShowImage( const char* name, const VsArr* image );
VISAGE_DECLSPEC void vsSmooth( const VsArr* src, VsArr* dst, int smoothtype VS_DEFAULT(VS_GAUSSIAN), int size1 VS_DEFAULT(3), int size2 VS_DEFAULT(0), double sigma1 VS_DEFAULT(0), double sigma2 VS_DEFAULT(0));

#undef VS_INTER_LINEAR
#undef VS_WARP_FILL_OUTLIERS
#undef VS_GAUSSIAN

#ifdef __cplusplus
}
#endif

