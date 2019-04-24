using System;
using System.Windows;
using SharpGL.SceneGraph;
using SharpGL;
using VisageCSWrapper;
using System.Drawing;
using System.Drawing.Imaging;
using System.ComponentModel;
using System.Runtime.InteropServices;
using System.Windows.Threading;
using Emgu.CV;
using Emgu.CV.UI;
using Emgu.CV.Structure;
using Emgu.CV.CvEnum;
using System.Windows.Forms;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Windows.Controls;
using DirectShowLib;

namespace ShowcaseDemo
{
    /*! \mainpage  visage|SDK ShowcaseDemo example project
    *
    * \if WIN64_DOXY
    * \htmlonly
    * <table border="0">
    * <tr>
    * <td width="32"><a href="../../../../../../bin64/DEMO_ShowcaseDemo.exe"><img src="../../../../../../doc/OpenGL/images/run_sample.png" border=0 title="Run Sample (this may not work in all browsers, in such case please run DEMO_ShowcaseDemo.exe from the visageSDK\bin64 folder.)"></a></td>
    * <td width="32"><a href="../../../../../../bin64"><img src="../../../../../../doc/OpenGL/images/open_bin_folder.png" border=0 title="Open Binary Folder (visageSDK\bin64)"></a></td>
    * <td width="32"><a href="../../../../data/Videos"><img src="../../../../../../doc/OpenGL/images/open_folder.png" border=0 title="Open Video Folder"></a></td>
    * <td width="32"><a href="../../../../build/msvc140/ShowcaseDemo"><img src="../../../../../../doc/OpenGL/images/open_project_folder.png" border=0 title="Open Project Folder"></a></td>
    * <td width="32"><a href="../../../../source/ShowcaseDemo"><img src="../../../../../../doc/OpenGL/images/open_source_folder.png" border=0 title="Open Source Code Folder"></a></td>
    * </tr>
    * </table>
    * \endhtmlonly
    * \else
    * \htmlonly
    * <table border="0">
    * <tr>
    * <td width="32"><a href="../../../../../../bin/DEMO_ShowcaseDemo.exe"><img src="../../../../../../doc/OpenGL/images/run_sample.png" border=0 title="Run Sample (this may not work in all browsers, in such case please run DEMO_ShowcaseDemo.exe from the visageSDK\bin folder.)"></a></td>
    * <td width="32"><a href="../../../../../../bin"><img src="../../../../../../doc/OpenGL/images/open_bin_folder.png" border=0 title="Open Binary Folder (visageSDK\bin)"></a></td>
    * <td width="32"><a href="../../../../data/Videos"><img src="../../../../../../doc/OpenGL/images/open_folder.png" border=0 title="Open Video Folder"></a></td>
    * <td width="32"><a href="../../../../build/msvc140/ShowcaseDemo"><img src="../../../../../../doc/OpenGL/images/open_project_folder.png" border=0 title="Open Project Folder"></a></td>
    * <td width="32"><a href="../../../../source/ShowcaseDemo"><img src="../../../../../../doc/OpenGL/images/open_source_folder.png" border=0 title="Open Source Code Folder"></a></td>
    * </tr>
    * </table>
    * \endhtmlonly
    * \endif
    *
    * The ShowcaseDemo example project demonstrates the following capabilities of visage|SDK C# API:
    *
    * - real-time head/facial features tracking, face recognition and age, gender and emotion estimation on multiple faces from video file or camera, based on the VisageCSWrapper.VisageTracker tracker, VisageCSWrapper.VisageFaceRecognition and VisageCSWrapper.VisageFaceAnalyser
    * - detecting facial features, face recognition and age, gender and emotion estimation on multiple faces in still images, based on the VisageCSWrapper.VisageFeaturesDetector detector, VisageCSWrapper.VisageFaceRecognition and VisageCSWrapper.VisageFaceAnalyser
    *
    * Requirements:
    * - Microsoft .NET Framework 4.0
    * - Emgu CV: msvcp120.dll, msvcr120.dll
    * 
    * Prebuilt version of the sample with integrated license key file can be found in 
    * \if WIN64_DOXY 
    * bin64 
    * \else 
    * bin 
    * \endif 
    * folder. In case you wish to rebuild the application, you will need a valid license key file.
    * For more information please take a look at <a href="../../../../../../doc/OpenGL/licensing.html"><em>Licensing</em></a> section of the documentation.
    *
    * The camera tracking starts when you run the application.
    * 
    * The specific classes and methods that demonstrate visage|SDK are:
    * - ShowcaseDemo.MainWindow.runTrackFromVideo(string file): Performs real time tracking from a video file.
    * - ShowcaseDemo.MainWindow.runTrackFromCam(): Performs real time tracking from a camera.
    * - ShowcaseDemo.MainWindow.runTrackFromImage(string file, VISAGE_MODE activeMode): Performs real time tracking from an image.
    * 
    * Drawing is implemented in ShowcaseDemo.VisageRendering which uses SharpGL, an external library which provides a C# wrapper of OpenGL.
    *
    * For frame capturing in video and camera tracking, ShowcaseDemo uses Emgu, an OpenCV wrapper for C#.
    *
    * Further classes and methods that are important for detailed understanding how this
    * sample project is implemented and how it uses visage|SDK are:
    * - VisageCSWrapper.FaceData class that is used for storing all the tracking data 
    *
    */

    /// <summary>
    /// Application can either be in TRACKER mode or in DETECTOR mode.
    /// 
    /// Tracker mode features face tracking from image, video or camera source. 
    /// Detector mode features face detection from an image source.
    /// </summary>
    public enum VISAGE_MODE
    {
        TRACKER = 0,
        DETECTOR
    };

    /// <summary>
    /// Type of input for tracker/detector.
    /// </summary>
    public enum INPUT_MODE
    {
        CAMERA = 0,
        VIDEO,
        IMAGE
    };

    #region Textured model
    struct TexturedModel
    {
        public List<Point3D> v; // geometric vertices
        public List<Point> vt; // texture vertices
        public List<Face> faces; // vertex indexes
    }

    struct Face
    {
        public int[] indexPoints;
        public int[] texIndexPoints;

        public Face(int[] indexPoints, int[] texIndexPoints)
        {
            this.indexPoints = indexPoints;
            this.texIndexPoints = texIndexPoints;
        }
    }

    struct Point
    {
        public float x;
        public float y;

        public Point(float x, float y)
        {
            this.x = x;
            this.y = y;
        }
    };

    struct Point3D
    {
        public float x;
        public float y;
        public float z;

        public Point3D(float x, float y, float z)
        {
            this.x = x;
            this.y = y;
            this.z = z;
        }
    };
    #endregion



    /// <summary>
    /// VisageSDK ShowcaseDemo application MainWindow class.
    /// 
    /// Demonstrates multi-threaded implementation and usage of visage|SDK C# wrapper. 
    /// </summary>
    public partial class MainWindow : Window
    {

        #region Display Constant Variables
        int ACTIVE_DISPLAY = VisageRendering.DISPLAY_FRAME + VisageRendering.DISPLAY_FEATURE_POINTS + VisageRendering.DISPLAY_SPLINES + VisageRendering.DISPLAY_TRACKING_QUALITY + VisageRendering.DISPLAY_POINT_QUALITY;
        bool ACTIVE_MIRROR = false;
        #endregion

        #region Globals
        VisageTracker gVisageTracker;
        VisageFaceAnalyser gVisageFaceAnalyser;
        VisageFeaturesDetector gVisageFeaturesDetector;
        VisageFaceRecognition gVisageFaceRecognition;


        VisageRendering gVisageRendering;
        OpenGL gl;

        public bool isTracked = false;
        public bool isDetected = false;
        public bool inDrawDetection = false;

        VSImage frame;
        VSImage frameRGB;

        //Used for pure tracking timer
        public long elapsedTime;
        public long startTime;
        public long endTime;
        System.Diagnostics.Stopwatch pureTrackStopwatch = new System.Diagnostics.Stopwatch();

        //Lock for draw thread and track thread synchronization
        private Object lockFaceData = new Object();

        private Object lockClearRecognition = new Object();

        //BackgroundWorker object used in tracking (image, camera, video) and detection (image)
        private BackgroundWorker worker;
        //BackgroundWorker object used in face recognition
        private BackgroundWorker fr_worker;
        //BackgroundWorker object used in face analysis
        private BackgroundWorker fa_worker;

        //VisageTracker
        public FaceData[] gFaceData;
        public int[] gTrackerStatus;
        public INPUT_MODE activeInputMode = INPUT_MODE.CAMERA;
        public VISAGE_MODE activeVisageMode = VISAGE_MODE.TRACKER;
        string currentVideo = "";
        string currentImage = "";
        int currentCameraDevice = 0;
        Dictionary<int, string> cameraDevices = new Dictionary<int, string>();
        Capture cameraCapture;
        const int maxFacesTracker = 4;

        //VisageFeaturesDetector
        FaceData[] dataArray;
        public bool detectorActive = false;
        int numFaces = 0;
        const int MAX_FACES_DETECTOR = 30;

        //VisageFaceAnalyser
        int vfaInitialised = 0;
        float[] gAgeArray = { -1.0f };
        int[] gGenderArray = { -1 };
        List<float[]> gEmotionList = new List<float[]>();
        const int NUM_EMOTIONS = 7;

        //VisageFaceRecognition
        List<string> recognizedFaces = new List<string>();
        int[] numInitFrames = new int[MAX_FACES_DETECTOR]; // current number of frames taken in face recognition initialization phase
        const int numInitFramesThreshold = 10;  //determines number of frames needed in face recognition initialization phase
        int[] numFramesNewIdentity = new int[MAX_FACES_DETECTOR]; // current number of subsequent frames needed for new identity to be added
        const int newIdentityFramesThreshold = 5; // determines number of subsequent frames needed for new identity to be added
        //
        int numPersons = 0; // number of new identities added so far
        List<string> gUniqueNames = new List<string>();
        string[] currentDisplayName = new string[MAX_FACES_DETECTOR];
        Dictionary<int, string> indices_to_names = new Dictionary<int, string>();
        float display_threshold = 0.65f;
        List<string> max_faces = new List<string>();
        Dictionary<int, List <short[]>> descBuffer = new Dictionary<int, List <short[]>>();


        //Filter
        int[] ageFilterCount = new int[maxFacesTracker];
        List<List<float>> TrackingQualityList = new List<List<float>>();
        private float[] TrackingQualityDELTA = new float[maxFacesTracker];
        float currentTrackingQuality = 0.0f;
        private float[] currMaxTrackingQuality = new float[maxFacesTracker];
        private int[] ageFilterFramesReached = new int[maxFacesTracker];
        List<List<float>> gAgeFilterList = new List<List<float>>();
        List<List<int>> gGenderFilterList = new List<List<int>>();
        List<List<float[]>> gEmotionFilterList = new List<List<float[]>>();
        //
        int numFilterFrames = 1;
        int emotionFilterTime = 500;
        int genderFilterTime = 1000;
        int ageFilterFrames = 10;
        int[] gGenderSmoothed = { -1 };
        float[] gAgeSmoothed = { -1.0f };
        List<float[]> gEmotionsSmoothed = new List<float[]>();

        //Texture coords for the textured model
		const int VERTEX_NUMBER = 357;        float[] texCoords;
        bool texCoordsStaticLoaded = false;

        //
        string imageInitialDir = @"..\Samples\OpenGL\data\Images";
        string videoInitialDir = @"..\Samples\OpenGL\data\Videos";

        //control flags accessed from face recognition/face analysis background thread as well as from the main UI thread
        //set when the check-box in the UI thread is clicked
        volatile bool FR_CHECKED = false;
        volatile bool FA_AGE_CHECKED = false;
        volatile bool FA_EMO_CHECKED = false;
        volatile bool FA_GEN_CHECKED = false;

        bool freeze_gallery = false;


        #endregion

        /// <summary>
        /// Main entry point of the application. 
        /// 
        /// Creates instances of <see cref="VisageTracker"/>, <see cref="VisageFeaturesDetector"/> and <see cref="VisageFaceAnalyser"/> objects and demonstrates
        /// license initialization. Also initializes OpenGL rendering.
        /// 
        /// Starts tracking from camera, as a default mode.
        /// </summary>
        public MainWindow()
        {
            InitializeComponent();

            VisageLicensing.InitializeLicenseManager(licensePath);

            //Create VisageTracker object
            gVisageTracker = new VisageTracker(@"..\Samples\data\Facial Features Tracker - High.cfg");

            //Create FaceData object used in tracking
            gFaceData = new FaceData[maxFacesTracker];
            for (int i = 0; i < maxFacesTracker; ++i)
            {
                gFaceData[i] = new FaceData();
            }

            //Create and initialize VisageFaceAnalayser object
            gVisageFaceAnalyser = new VisageFaceAnalyser();
            vfaInitialised = gVisageFaceAnalyser.Init(@"..\Samples\data\bdtsdata\LBF\vfadata");

            //Initialize variables for storing and smoothing face analysis results
            InitializeFaceAnalysis();

            //Initialize variables for storing face recognition results
            InitializeFaceRecognition();

            //Create VisageFeaturesDetector object
            gVisageFeaturesDetector = new VisageFeaturesDetector(@"..\Samples\data");

            //Create VisageFeaturesRecognition object
            gVisageFaceRecognition = new VisageFaceRecognition(@"..\Samples\data\bdtsdata\NN\fr.bin");

            //Create the renderer and openGL control objects
            gl = openGLControl.OpenGL;
            gVisageRendering = new VisageRendering(gl);

            //Load and set texture image
            gVisageRendering.SetTextureImage(LoadImage(@"..\Samples\OpenGL\data\ShowcaseDemo\tiger_texture.png"));

            //Populate combox box with connected camera names
            DsDevice[] _SystemCamereas = DsDevice.GetDevicesOfCat(FilterCategory.VideoInputDevice);
            for (int i = 0; i < _SystemCamereas.Length; i++)
            {
                cameraDevices.Add(i, _SystemCamereas[i].Name);
                ComboBoxItem cbi = new ComboBoxItem();
                cbi.Content = _SystemCamereas[i].Name;
                cbCameraChoice.Items.Add(cbi);
            }
            if (cameraDevices.Count > 0)
            {
                cbCameraChoice.SelectedIndex = currentCameraDevice; //Set the selected device the default
            }
        }

        #region FR and FA State Modifiers

        /// <summary>
        /// Reset all the face recognition variables including gallery. Called when gallery is cleared.
        /// </summary>
        private void ClearFaceRecognition()
        {
            lock (lockClearRecognition)
            {
                for (int trackerIndex = 0; trackerIndex < maxFacesTracker; ++trackerIndex)
                {
                    lbxNames.Items.Clear();
                    recognizedFaces = new List<string>();
                    descBuffer[trackerIndex].Clear();
                    numInitFrames[trackerIndex] = 0;
                    numFramesNewIdentity[trackerIndex] = 0;
                    numPersons = 0;
                    gUniqueNames = new List<string>();
                    currentDisplayName[trackerIndex] = "";
                    indices_to_names = new Dictionary<int, string>();
                    gVisageFaceRecognition.ResetGallery();
                }
            }
        }

        /// <summary>
        /// Initialize face recognition buffer variables. Called when the application starts.
        /// </summary>
        private void InitializeFaceRecognition()
        {
            for (int i = 0; i < maxFacesTracker; ++i)
            {
                //For each face, initialize a list that will that face descriptors
                descBuffer.Add(i, new List<short[]>());
                //For each face reset the counters
                numInitFrames[i] = 0;
                numFramesNewIdentity[i] = 0;
                //
                currentDisplayName[i] = "";
            }
        }


        /// <summary>
        /// Reinitialize face recognition variables used during face recognition initialization phase. For example, when the face is lost in the middle of the initialization
        /// process.
        /// </summary>
        /// <param name="index">Index of the face corresponding to the index of the array where face analysis data is stored.</param>
        private void ResetFRInitialization(int index = -1)
        {
            lock (lockClearRecognition)
            {
                if (index == -1)
                {
                    for (int i = 0; i < maxFacesTracker; ++i)
                    {
                        descBuffer[i].Clear(); 
                        numFramesNewIdentity[i] = 0;

                        if (numInitFrames[i] != 0 && numInitFrames[i] < numInitFramesThreshold)
                            numInitFrames[i] = numInitFramesThreshold;
                    }
                }
                else
                {
                    descBuffer[index].Clear(); 
                    numFramesNewIdentity[index] = 0;

                    if (numInitFrames[index] != 0 && numInitFrames[index] < numInitFramesThreshold)
                        numInitFrames[index] = numInitFramesThreshold;
                }
            }
        }


        /// <summary>
        /// Reinitialize face analysis variables for storing data and filtering/smoothing.
        /// </summary>
        private void InitializeFaceAnalysis()
        {
            gAgeArray = new float[MAX_FACES_DETECTOR];
            gGenderArray = new int[MAX_FACES_DETECTOR];
            //
            gAgeSmoothed = new float[maxFacesTracker];
            gGenderSmoothed = new int[maxFacesTracker];
            gEmotionsSmoothed.Clear();

            for (int i = 0; i < MAX_FACES_DETECTOR; ++i)
            {
                gAgeArray[i] = -1.0f;
                gGenderArray[i] = -1;
                gEmotionList.Add(default(float[]));    
            }

            //filter variables only for tracker mode
            for (int i = 0; i < maxFacesTracker; ++i)
            {
                gAgeFilterList.Add(new List<float>());
                gGenderFilterList.Add(new List<int>());
                gEmotionFilterList.Add(new List<float[]>());
                //
                gAgeSmoothed[i] = -1.0f;
                gGenderSmoothed[i] = -1;
                gEmotionsSmoothed.Add(default(float[]));
                //
                TrackingQualityList.Add(new List<float>());
                currMaxTrackingQuality[i] = 0.6f;
                TrackingQualityDELTA[i] = 0.0f;
                ageFilterFramesReached[i] = 0;
                //
                ResetFaceAnalysis(i);
            }
            
        }

        /// <summary>
        /// Clears face analysis data and smoothing data for the specific face.
        /// </summary>
        /// <param name="index">Index of the face corresponding to the index of the array where face analysis data is stored.</param>
        private void ResetFaceAnalysis(int index)
        {
            gAgeSmoothed[index] = -1.0f;
            gGenderSmoothed[index] = -1;
            gEmotionsSmoothed[index] = default(float[]);
            //
            gAgeFilterList[index].Clear();
            gGenderFilterList[index].Clear();
            gEmotionFilterList[index].Clear();

            TrackingQualityList[index].Clear();
            currMaxTrackingQuality[index] = 0.6f;
            TrackingQualityDELTA[index] = 0.0f;
            ageFilterFramesReached[index] = 0;
            ageFilterCount[index] = 0;
        }

        #endregion


        #region BackgroundWorker Tracking Functions

        private static byte[] BitmapToByteArray(Bitmap bitmap)
        {
            int stride = 0;
            return BitmapToByteArray(bitmap, ref stride);
        }

        /// <summary>
        /// Helper function for converting image data from Bitmap class to a raw byte array.
        /// </summary>
        /// <param name="bitmap">Bitmap image.</param>
        /// <param name="stride">Image stride.</param>
        /// <returns></returns>
        private static byte[] BitmapToByteArray(Bitmap bitmap, ref int stride)
        {
            System.Drawing.Imaging.PixelFormat pixelFormat = bitmap.PixelFormat;
            BitmapData bmpdata = bitmap.LockBits(new System.Drawing.Rectangle(0, 0, bitmap.Width, bitmap.Height), ImageLockMode.ReadOnly, pixelFormat);
            int numbytes = bmpdata.Stride * bitmap.Height;
            //((n + (k-1)) / k) * k;
            //modify height to be the value of the next nearest number divisble by 4
            int numbytes_padded = bmpdata.Stride * ((bitmap.Height + 3) / 4 * 4);
            byte[] bytedata = new byte[numbytes_padded];
            stride = bmpdata.Stride;
            IntPtr ptr = bmpdata.Scan0;

            Marshal.Copy(ptr, bytedata, 0, numbytes);

            bitmap.UnlockBits(bmpdata);

            return bytedata;
        }

        /// <summary>
        /// Worker event for tracking/detection from an image source that runs in a separate thread. Used in runTrackFromImage() function.
        /// 
        /// Expects two arguments sent via e parameter: path to the image and currently active VISAGE_MODE (TRACKER, DETECTOR). Based on the current mode initiates tracking or detection.
        /// Depending on the tracker return status runs face analysis and face recognition in a separate thread.
        /// 
        /// Locking mechanism is used to synchronize frame and tracking results rendering. Due to face analysis and face recognition running in separate thread 
        /// rendering of age, gender, emotion and identity is not synchronized with the rendering of the frame.
        /// 
        /// Implemented using <see cref="BackgroundWorker"/>.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void worker_DoWorkImage(object sender, DoWorkEventArgs e)
        {
            BackgroundWorker worker = sender as BackgroundWorker;
            List<object> argumentList = e.Argument as List<object>;
            string file = (string)argumentList[0];
            VISAGE_MODE currentMode = (VISAGE_MODE)argumentList[1];

            //Age estimation enabled by default
            Dispatcher.BeginInvoke((Action)(() => { enableAgeEstimCheckbox(); }));

            //Load image data by filename
            Bitmap bmp = new Bitmap(file);
            int bitmapStride = 0;
            byte[] bmp_byte = BitmapToByteArray(bmp, ref bitmapStride);
            BitmapData bmpdata = bmp.LockBits(new System.Drawing.Rectangle(0, 0, bmp.Width, bmp.Height), ImageLockMode.ReadOnly, bmp.PixelFormat);

            //Determine image format
            int format = (int)VISAGE_FORMAT.BGR;    
            int origin = (int)VISAGE_ORIGIN.TL;
            int nChannels = 3;
            switch (bmp.PixelFormat)
            {
                case PixelFormat.Format24bppRgb:
                    format = (int)VISAGE_FORMAT.BGR;
                    nChannels = 3;
                    break;
                case PixelFormat.Format8bppIndexed:
                    format = (int)VISAGE_FORMAT.LUMINANCE;
                    nChannels = 1;
                    Dispatcher.BeginInvoke((Action)(() => { disableAgeEstimCheckbox(); }));
                    break;
                case PixelFormat.Format32bppRgb:
                    format = (int)VISAGE_FORMAT.BGRA;
                    nChannels = 4;
                    Dispatcher.BeginInvoke((Action)(() => { disableAgeEstimCheckbox(); }));
                    break;
                default:
                    format = -1;
                    break;
            }

            //Clear FA variables
            lock (lockFaceData)
            {
                InitializeFaceAnalysis();
            }

            //Create a VSImage and fill it with data to pass it to the DetectFeatures function
            frame = new VSImage(bmp.Width, bmp.Height, VS_DEPTH._8U, nChannels);
            frame.imageData = bmp_byte;

            byte[] bytedataRGB = new byte[bmp_byte.Length];

            convertFromBGRToRGB(ref bytedataRGB, ref bmp_byte);

            frameRGB = new VSImage(bmp.Width, bmp.Height, VS_DEPTH._8U, nChannels);
            frameRGB.imageData = bytedataRGB;

            while (true)
            {
                //Check if the worker thread was canceled
                if (worker.CancellationPending || format == -1)
                {
                    isTracked = false;
                    e.Cancel = true;
                    break;
                }
                else if (currentMode == VISAGE_MODE.TRACKER)
                {
                    //
                    FaceData[] analysisData = new FaceData[maxFacesTracker];

                    //Lock the shared variables so there can be no read/write concurrency between the tracking thread and the rendering thread
                    lock (lockFaceData)
                    {
                        pureTrackStopwatch.Start();
                        startTime = pureTrackStopwatch.ElapsedMilliseconds;
                        gTrackerStatus = gVisageTracker.Track(bmp.Width, bmp.Height, bmp_byte, gFaceData, format, origin, bmpdata.Stride, -1, maxFacesTracker);
                        pureTrackStopwatch.Stop();
                        endTime = pureTrackStopwatch.ElapsedMilliseconds;
                        elapsedTime = endTime - startTime;

                        //While locked, copy face data from the tracker to face data array for face analysis and recognition
                        for (int i = 0; i < maxFacesTracker; ++i)
                        {
                            analysisData[i] = new FaceData(gFaceData[i]);
                        }

                        //Signalize to the rendering thread that the tracking is complete
                        isTracked = true;
                    }

                    //Run face analysis on each face in a separate thread
                    runFaceAnalysis(frameRGB, analysisData, gTrackerStatus);

                    //Run face recognition on each face in a separate thread
                    runFaceRecognition(frame, analysisData, gTrackerStatus);
                }
                else if (currentMode == VISAGE_MODE.DETECTOR)
                {
                    isDetected = false;

                    //Check that the rendering thread is not rendering, if it is rendering wait
                    while (inDrawDetection)
                    {
                        System.Threading.Thread.Sleep(100);
                    }

                    bmp.UnlockBits(bmpdata);

                    //Create new FaceData array and array objects
                    dataArray = new FaceData[MAX_FACES_DETECTOR];
                    for (int i = 0; i < MAX_FACES_DETECTOR; ++i)
                    {
                        dataArray[i] = new FaceData();
                    }

                    //Check that the detector was initializes successfully, if not cancel worker and return
                    if (gVisageFeaturesDetector.Initialized != true)
                    {
                        worker.CancelAsync();
                        return;
                    }

                    //Detect faces and facial features
                    numFaces = gVisageFeaturesDetector.DetectFacialFeatures(frame, dataArray);
                    if (numFaces > 0)
                    {
                        //Allocate arrays for age, gender and clear the emotion data list
                        gAgeArray = new float[numFaces];
                        gGenderArray = new int[numFaces];
                        gEmotionList.Clear();
                        for (int i = 0; i < numFaces; i++)
                        {
                            //Example of face normalization function
                            //NormalizeFace(frame, dataArray[i]);

                            //Fill the arrays and the list with default values
                            gAgeArray[i] = -1.0f;
                            gGenderArray[i] = -1;
                            gEmotionList.Add(default(float[]));
                            //Run face analysis on this thread
                            AnalyzeFace(frameRGB, dataArray[i], i);
                            //Run face recognition on this thread
                            RecognizeFace(frame, dataArray[i], i); 
                        }

                    }

                    isDetected = true;
                    //Terminate the worker after detection
                    worker.CancelAsync();
                }
            }
            //Stop the tracker
            gVisageTracker.Stop();
        }

        /// <summary>
        /// Worker event for tracking from camera that runs in a separate thread. Used in runTrackFromCam() function.
        /// 
        /// Demonstrates handling of the camera and camera frame data retrieval (using OpenCV). Tracks and analyses face in the current camera frame.
        /// Depending on the tracker return status runs face analysis and face recognition in a separate thread.
        /// 
        /// Locking mechanism is used to synchronize frame and tracking results rendering. Due to face analysis and face recognition running in separate thread 
        /// rendering of age, gender, emotion and identity is not synchronized with the rendering of the frame.
        /// 
        /// Implemented using <see cref="BackgroundWorker"/>.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void worker_DoWorkCam(object sender, DoWorkEventArgs e)
        {
            if (cameraCapture == null)
            {
                //Create new OpenCV camera capture
                cameraCapture = new Capture(currentCameraDevice);
                //Set desired resolution
                cameraCapture.SetCaptureProperty(CapProp.FrameWidth, 800);
                cameraCapture.SetCaptureProperty(CapProp.FrameHeight, 600);
            }

            //Age estimation enabled by default
            Dispatcher.BeginInvoke((Action)(() => { enableAgeEstimCheckbox(); }));

            //Clear FA variables
            lock (lockFaceData)
            {
                InitializeFaceAnalysis();
            }

            while (true)
            {
                //Check if the worker thread was canceled
                if (worker.CancellationPending)
                {
                    isTracked = false;
                    e.Cancel = true;
                    cameraCapture.Dispose();
                    cameraCapture = null;
                    break;
                }
                else
                {
                    //Grab camera frame
                    Bitmap bmp = cameraCapture.QueryFrame().Bitmap;

                    if (ACTIVE_MIRROR == true)
                        bmp.RotateFlip(RotateFlipType.Rotate180FlipY);

                    int nChannels = 3;
                    switch (bmp.PixelFormat)
                    {
                        case PixelFormat.Format24bppRgb:
                            nChannels = 3;
                            break;
                        case PixelFormat.Format8bppIndexed:
                            nChannels = 1;
                            Dispatcher.BeginInvoke((Action)(() => { disableAgeEstimCheckbox(); }));
                            break;
                        case PixelFormat.Format32bppRgb:
                            nChannels = 4;
                            Dispatcher.BeginInvoke((Action)(() => { disableAgeEstimCheckbox(); }));
                            break;
                        default:
                            break;
                    }

                    //
                    FaceData[] analysisData = new FaceData[maxFacesTracker];

                    lock (lockFaceData)
                    {
                        //Create a VSImage and fill it with data to pass it to the DetectFeatures function
                        frame = new VSImage(bmp.Width, bmp.Height, VS_DEPTH._8U, nChannels);
                        byte[] bmp_byte = BitmapToByteArray(bmp);
                        frame.imageData = bmp_byte;

                        byte[] bytedataRGB = new byte[bmp_byte.Length];

                        convertFromBGRToRGB(ref bytedataRGB, ref bmp_byte);

                        frameRGB = new VSImage(bmp.Width, bmp.Height, VS_DEPTH._8U, nChannels);
                        frameRGB.imageData = bytedataRGB;

                        pureTrackStopwatch.Start();
                        startTime = pureTrackStopwatch.ElapsedMilliseconds;
                        gTrackerStatus = gVisageTracker.Track(bmp.Width, bmp.Height, bmp_byte, gFaceData, (int)VISAGE_FORMAT.BGR, (int)VISAGE_ORIGIN.TL, 0, -1, maxFacesTracker);
                        pureTrackStopwatch.Stop();
                        endTime = pureTrackStopwatch.ElapsedMilliseconds;
                        elapsedTime = endTime - startTime;

                        if (!texCoordsStaticLoaded)
                        {
                            texCoords = gFaceData[0].FaceModelTextureCoordsStatic;
                            texCoordsStaticLoaded = true;
                        }

                        //While locked, copy face data from the tracker to face data array for face analysis and recognition //PROVJERITI KASNIJE: prije copya provjeriti da li je ukljucen FA/FR
                        for (int i = 0; i < maxFacesTracker; ++i)
                        {
                            analysisData[i] = new FaceData(gFaceData[i]);
                        }

                        isTracked = true;
                    }

                    //Run face analysis in a separate thread
                    runFaceAnalysis(frameRGB, analysisData, gTrackerStatus);

                    //Run face recognition in a separate thread
                    runFaceRecognition(frame, analysisData, gTrackerStatus);
                }
            }
            //Stop the tracker
            gVisageTracker.Stop();
        }


        /// <summary>
        /// Converts BGR image obtained from camera to RGB image. 
        /// Image has to be converted to RGB format when VisageFaceAnalyser is used.
        /// 
        /// </summary>
        private void convertFromBGRToRGB(ref byte[] outputRGB, ref byte[] inputBGR)
        {
            int length = inputBGR.Length;
            int nChannels = 3;
            int j = 0;
            for (int i = 0; i < length/nChannels; i++)
            {
                outputRGB[j++] = inputBGR[i * nChannels + 2];
                outputRGB[j++] = inputBGR[i * nChannels + 1];
                outputRGB[j++] = inputBGR[i * nChannels];
            }
        }

        /// <summary>
        /// Worker event for tracking from video that runs in a separate thread. Used in runTrackFromVideo() function.
        /// 
        /// Expects one argument sent via e parameter: path to the video file. Demonstrates handling of video files and video frame data retrieval (using OpenCV) and
        /// implements synchronization to the video FPS. Tracks and analyses face in video frames. Depending on the tracker return status runs face analysis 
        /// and face recognition in a separate thread.
        /// 
        /// Locking mechanism is used to synchronize frame and tracking results rendering. Due face analysis and face recognition are running in separate thread 
        /// rendering of age, gender, emotion and identity is not synchronized with the rendering of the frame.
        /// 
        /// Implemented using <see cref="BackgroundWorker"/>.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void worker_DoWorkVideo(object sender, DoWorkEventArgs e)
        {
            BackgroundWorker worker = sender as BackgroundWorker;
            string file = (string)e.Argument;

            // Video capture and frame data initialization
            Capture capture = new Capture(file);
            Bitmap bmp = new Bitmap(capture.QueryFrame().Bitmap);
            int bitmapStride = 0;
            byte[] bmp_byte;

            //Age estimation enabled by default
            Dispatcher.BeginInvoke((Action)(() => { enableAgeEstimCheckbox(); }));

            System.Diagnostics.Stopwatch stopwatch = new System.Diagnostics.Stopwatch();
            stopwatch.Start();

            bool video_file_sync = false;
            float frameTime = 1000 / (float)CvInvoke.cveVideoCaptureGet(capture, Emgu.CV.CvEnum.CapProp.Fps);
            int frameCount = 0;

            //Clear FA variables
            lock (lockFaceData)
            {
                InitializeFaceAnalysis();
            }

            while (true)
            {
                if (worker.CancellationPending)
                {
                    isTracked = false;
                    e.Cancel = true;
                    capture = null;
                    break;
                }
                else
                {
                    long currentTime = stopwatch.ElapsedMilliseconds;

                    while (video_file_sync && currentTime > frameTime * (frameCount + 1)) // skip frames if track() is too slow
                    {
                        frameCount++;
                        currentTime = stopwatch.ElapsedMilliseconds;
                        capture.Grab();
                    }

                    while (video_file_sync && currentTime < frameTime * (frameCount - 5)) // wait if track() is too fast
                    {
                        System.Threading.Thread.Sleep(1);
                        currentTime = stopwatch.ElapsedMilliseconds;
                    }

                    // Frame grabbing
                    try
                    {
                        bmp = capture.QueryFrame().Bitmap;

                        if (ACTIVE_MIRROR == true)
                            bmp.RotateFlip(RotateFlipType.Rotate180FlipY);
                    }
                    catch (NullReferenceException)
                    {
                        worker.CancelAsync();
                    }
                    bmp_byte = BitmapToByteArray(bmp, ref bitmapStride);

                    frameCount++;

                    int format = (int)VISAGE_FORMAT.BGR;
                    int nChannels = 3;
                    switch (bmp.PixelFormat)
                    {
                        case PixelFormat.Format24bppRgb:
                            format = (int)VISAGE_FORMAT.BGR;
                            nChannels = 3;
                            break;
                        case PixelFormat.Format8bppIndexed:
                            format = (int)VISAGE_FORMAT.LUMINANCE;
                            nChannels = 1;
                            Dispatcher.BeginInvoke((Action)(() => { disableAgeEstimCheckbox(); }));
                            break;
                        case PixelFormat.Format32bppRgb:
                            format = (int)VISAGE_FORMAT.BGRA;
                            nChannels = 4;
                            Dispatcher.BeginInvoke((Action)(() => { disableAgeEstimCheckbox(); }));
                            break;
                        default:
                            format = -1;
                            break;
                    }

                    //Create a VSImage and fill it with data to pass it to the track function
                    frame = new VSImage(bmp.Width, bmp.Height, VS_DEPTH._8U, nChannels);
                    frame.imageData = bmp_byte;

                    byte[] bytedataRGB = new byte[bmp_byte.Length];

                    convertFromBGRToRGB(ref bytedataRGB, ref bmp_byte);

                    frameRGB = new VSImage(bmp.Width, bmp.Height, VS_DEPTH._8U, nChannels);
                    frameRGB.imageData = bytedataRGB;

                    FaceData[] analysisData = new FaceData[maxFacesTracker];

                    lock (lockFaceData)
                    {
                        pureTrackStopwatch.Start();
                        startTime = pureTrackStopwatch.ElapsedMilliseconds;
                        gTrackerStatus = gVisageTracker.Track(bmp.Width, bmp.Height, bmp_byte, gFaceData, format, (int)VISAGE_ORIGIN.TL, bitmapStride, -1, maxFacesTracker);
                        pureTrackStopwatch.Stop();
                        endTime = pureTrackStopwatch.ElapsedMilliseconds;
                        elapsedTime = endTime - startTime;

                        //While locked, copy face data from the tracker to face data array for face analysis and recognition //PROVJERITI KASNIJE: prije copya provjeriti da li je ukljucen FA/FR
                        for (int i = 0; i < maxFacesTracker; ++i)
                        {
                            analysisData[i] = new FaceData(gFaceData[i]);
                        }

                        isTracked = true;
                    }

                    //Run face analysis in a separate thread
                    runFaceAnalysis(frameRGB, analysisData, gTrackerStatus);

                    //Run face recognition in a separate thread
                    runFaceRecognition(frame, analysisData, gTrackerStatus);
                }
            }
            //Stop the tracker
            gVisageTracker.Stop();
        }

        /// <summary>
        /// Worker event for face recognition that runs in a separate thread. Used in runFaceRecognition() function.
        /// 
        /// Expects three arguments sent via e parameter: frame (VsImage), array of face data objects (FaceData[]) and array of tracking statuses (int[].
        /// 
        /// Calls RecognizeFace function for each face in the frame and passes frame and corresponding FaceData object.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void worker_DoWorkMatchFace(object sender, DoWorkEventArgs e)
        {
            List<object> argumentList = e.Argument as List<object>;
            //read and cast the parameters
            VSImage frame = (VSImage)argumentList[0];
            FaceData[] faceDatas = (FaceData[])argumentList[1];
            int[] trackerStatus = (int[])argumentList[2];

            //recognize face if the tracker is tracking, otherwise, if tracker state has changed, reset face recognition initialization buffers
            for (int i = 0; i < maxFacesTracker; ++i)
            {
                if (trackerStatus[i] == (int)TRACK_STAT.OK && gVisageRendering.withinConstraints(ref faceDatas[i]))
                {
                    RecognizeFace(frame, faceDatas[i], i);
                }
                else
                {
                    ResetFRInitialization(i); 
                }
            }

        }

        /// <summary>
        /// Worker event for face analysis that runs in a separate thread. Used in runFaceAnalysis() function.
        /// 
        /// Expects three arguments sent via e parameter: frame (VsImage), face data array object (FaceData[]) and array of tracking statuses (int[]).
        /// 
        /// Calls AnalyzeFace function for each face in the frame and passes frame and corresponding FaceData object.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void worker_DoAnalyzeFace(object sender, DoWorkEventArgs e)
        {
            List<object> argumentList = e.Argument as List<object>;
            //read and cast the parameters
            VSImage frame = (VSImage)argumentList[0];
            FaceData[] faceData = (FaceData[])argumentList[1];
            int[] trackerStatus = (int[])argumentList[2];

            //analyze face if the tracker is tracking, otherwise, if tracker state has changed, reset filtering buffers
            for (int i = 0; i < maxFacesTracker; ++i)
            {
                if (trackerStatus[i] == (int)TRACK_STAT.OK)
                {
                    if (gEmotionsSmoothed[i] == default(float[])) gEmotionsSmoothed[i] = new float[NUM_EMOTIONS];
                    AnalyzeFace(frame, faceData[i], i);
                }
                else
                {
                    ResetFaceAnalysis(i);
                }
            }
        }

        /// <summary>
        /// Performs face analysis on an image based on the input face data object. Used in worker_DoAnalyzeFace() function.
        /// 
        /// Performs age, gender or emotion analysis depending on the check-boxes selected and which features of the VisageFaceAnalyser are available.
        /// </summary>
        /// <param name="frame">Input frame on which the face analysis is performed.</param>
        /// <param name="faceData">Input FaceData object on which the face analysis is performed.</param>
        /// <param name="index">The corresponding index of the FaceData object in the FaceData array matching the index of the age, gender array or emotion filter list.</param>
        /// 
        private void AnalyzeFace(VSImage frame, FaceData faceData, int index = 0)
        {
            int emotion_result = -1;

            if ((FA_AGE_CHECKED || FA_GEN_CHECKED || FA_EMO_CHECKED))
            {
                //age estimation
                if (FA_AGE_CHECKED && ((vfaInitialised & (int)VFA.AGE) == (int)VFA.AGE))
                {
                    gAgeArray[index] = gVisageFaceAnalyser.EstimateAge(frame, faceData); 
                }

                //gender estimation
                if (FA_GEN_CHECKED && ((vfaInitialised & (int)VFA.GENDER) == (int)VFA.GENDER))
                {
                    gGenderArray[index] = gVisageFaceAnalyser.EstimateGender(frame, faceData);
                    if (activeVisageMode == VISAGE_MODE.TRACKER)
                        gGenderFilterList[index].Add(gGenderArray[index]);
                }

                //emotion estimation
                //anger, disgust, fear, happiness, sadness, surprise and neutral
                if (FA_EMO_CHECKED && ((vfaInitialised & (int)VFA.EMOTION) == (int)VFA.EMOTION))
                {
                    gEmotionList[index] = new float[NUM_EMOTIONS];
                    emotion_result = gVisageFaceAnalyser.EstimateEmotion(frame, faceData, gEmotionList[index]);
                    if (activeVisageMode == VISAGE_MODE.TRACKER && emotion_result == 1)
                        gEmotionFilterList[index].Add(gEmotionList[index]);
                }

                //filter only if in tracker mode
                if (activeVisageMode == VISAGE_MODE.TRACKER)
                    Filter(ref faceData, index);
            }
        }

        /// <summary>
        /// Performs face recognition on an image based on the input face data array. Used in worker_DoWorkMatchFace() function when INPUT_MODE is set to
        /// CAMERA or IMAGE or can be called separately.
        /// 
        /// Implements additional face recognition logic and descriptor clustering.
        /// </summary>
        /// <param name="frame">Input frame on which the face recognition is performed.</param>
        /// <param name="faceDatas">Input FaceData object on which the face recognition is performed.</param>
        /// <param name="index">The corresponding index of the FaceData object in the FaceData array.</param>
        ///
        private void RecognizeFace(VSImage frame, FaceData faceDatas, int index = 0)
        {
            //Activate lock - wait here until clear gallery event has finished
            lock (lockClearRecognition)
            {
                int DESCRIPTOR_SIZE;

                //Proceed with face recognition only if VisageFaceRecognition has initialized successfully
                //Save the descriptor size for later
                if (!gVisageFaceRecognition.IsInitialized)
                    return;
                else
                    DESCRIPTOR_SIZE = gVisageFaceRecognition.GetDescriptorSize();
                
                //If face recognition is enabled and adding descriptors to the gallery is enabled
                if (FR_CHECKED && !freeze_gallery)
                {
                    //Fetch the current number of descriptors
                    int count = gVisageFaceRecognition.GetDescriptorCount();

                    short[] descriptor = new short[DESCRIPTOR_SIZE];

                    //Extract a face descriptor from the face in the current frame
                    int extract_descriptor_status = gVisageFaceRecognition.ExtractDescriptor(faceDatas, frame, descriptor);

                    //Check if still in initialization phase for [index] face
                    if (numInitFrames[index] < numInitFramesThreshold)
                    {
                        currentDisplayName[index] = "?";

                        if (extract_descriptor_status == 1)
                        {
                            numInitFrames[index] += 1;

                            //Add descriptor to the buffer for face index
                            descBuffer[index].Add(descriptor);
                        }
                        else
                            currentDisplayName[index] = "-";

                        //Check if sufficient number of face descriptors has been collected if it has add them all to the gallery with the unique ID - Person #
                        if (numInitFrames[index] >= numInitFramesThreshold)
                        {
                            for (int i = 0; i < numInitFramesThreshold; i++)
                            {
                                gVisageFaceRecognition.AddDescriptor(descBuffer[index][i], "Person" + numPersons);
                            }

                            //Clear buffer after descriptors have been added to the gallery
                            descBuffer[index].Clear();

                            gUniqueNames.Add("Person" + numPersons);
                            indices_to_names[numPersons] = "Person" + numPersons;
                            numPersons += 1;
                            Dispatcher.BeginInvoke((Action)(() =>
                            { FillNames(); }));
                        }

                    }

                    //Check if the initialization phase is complete
                    else
                    {
                        float[] sim = new float[count];
                        string[] names = new string[count];

                        //Compare the descriptor to all the descriptors in the gallery (count)
                        int recognize_status = 0;
                        if (extract_descriptor_status == 1)
                            recognize_status = gVisageFaceRecognition.Recognize(descriptor, count, ref names, sim);

                        //If face recognition was successful and the recognized face is recognized with high enough similarity, display the name
                        if (recognize_status > 0 && extract_descriptor_status == 1 && sim[0] > display_threshold)
                        {
                            currentDisplayName[index] = names[0];
                            numFramesNewIdentity[index] = 0;
                        }
                        //Case were extract descriptor fails, for example when on edge of the screen, keep the name and do not collect new descriptors
                        else if (extract_descriptor_status == 0)
                        {
                            currentDisplayName[index] = "-";
                            numFramesNewIdentity[index] = 0;
                        }
                        //Otherwise keep count on how many frames in a row similarity has not been high enough
                        //If the number is above threshold we conclude that there is a new face in the frame and we go to the initialisation phase again
                        else 
                        {
                            if (numFramesNewIdentity[index] / (float)newIdentityFramesThreshold > 0)
                                currentDisplayName[index] = "?";
                            else if (recognize_status > 0 && sim[0] > display_threshold)
                                currentDisplayName[index] = names[0];
                            else
                                currentDisplayName[index] = "?";

                            numFramesNewIdentity[index] += 1;

                            if (numFramesNewIdentity[index] > newIdentityFramesThreshold)
                            {
                                numInitFrames[index] = 0;
                                numFramesNewIdentity[index] = 0;
                            }

                        }
                    }
                }
                //If face recognition is enabled and adding descriptors to the gallery is disabled
                if (FR_CHECKED && freeze_gallery)
                {
                    int count = gVisageFaceRecognition.GetDescriptorCount();

                    //Delete descriptors for identities that are not fully processed
                    if (!(numInitFrames[index] == 0 || numInitFrames[index] == numInitFramesThreshold))
                    {
                        descBuffer[index].Clear();
                        numInitFrames[index] = 0;
                    }

                    count = gVisageFaceRecognition.GetDescriptorCount();

                    //If we are not allowed to add to the gallery just extract descriptor and compare it to all the ones that are currently in the gallery
                    if (count > 0)
                    {

                        short[] descriptor = new short[DESCRIPTOR_SIZE];
                        float[] sim = new float[count];
                        string[] names = new string[count];

                        gVisageFaceRecognition.ExtractDescriptor(faceDatas, frame, descriptor);

                        gVisageFaceRecognition.Recognize(descriptor, count, ref names, sim);

                        if (names.Count() > 0 && sim[0] > display_threshold)
                            currentDisplayName[index] = names[0];
                        else
                            currentDisplayName[index] = "?";

                    }
                    else
                        currentDisplayName[index] = "?";

                }
            }
        }

        /// <summary>
        /// Filters emotion, age and gender data.
        /// </summary>
        /// <param name="faceData">Input FaceData object.</param>
        /// <param name="index">The corresponding index of the FaceData object in the FaceData array matching the index of the age, gender array or emotion filter list.</param>
        private void Filter(ref FaceData faceData, int index)
        {
            if (gAgeArray[index] > -1.0f)
            {
                currentTrackingQuality = faceData.TrackingQuality;

                if (currMaxTrackingQuality[index] * (1.0f + TrackingQualityDELTA[index]) <= currentTrackingQuality && ageFilterCount[index] <= ageFilterFrames && gVisageRendering.withinConstraints(ref faceData))
                {
                    TrackingQualityList[index].Add(currentTrackingQuality);
                    gAgeFilterList[index].Add(gAgeArray[index]);
                    //
                    ageFilterCount[index]++;
                    TrackingQualityDELTA[index] += DeltaStep(ageFilterFramesReached[index]);
                }

                if (ageFilterCount[index] == ageFilterFrames)
                {
                    gAgeSmoothed[index] = (int)gAgeFilterList[index].Average();
                    //
                    ageFilterCount[index] = 0;
                    currMaxTrackingQuality[index] = TrackingQualityList[index].Max();
                    TrackingQualityDELTA[index] = 0.0f;
                    ageFilterFramesReached[index]++;
                }
                else
                {
                   gAgeSmoothed[index] = (int)gAgeFilterList[index].Average();
                }
            }

            if (gGenderArray[index] > -1)
            {
                numFilterFrames = (int)Math.Round(genderFilterTime * faceData.FrameRate / 1000);

                gGenderSmoothed[index] = (gGenderFilterList[index].Count != 0) ? (int)Math.Round(gGenderFilterList[index].Average()) : -1;
                if (gGenderFilterList[index].Count > numFilterFrames)
                {
                    gGenderSmoothed[index] = (int)Math.Round(gGenderFilterList[index].Average());
                    gGenderFilterList[index].RemoveAt(0);
                }
            }

            if (gEmotionList[index] != null)
            {
                numFilterFrames = (int)Math.Round(emotionFilterTime * faceData.FrameRate / 1000);
                float avgEmotion = 0;

                if (gEmotionFilterList[index].Count > numFilterFrames)
                {
                    for (int i = 0; i < NUM_EMOTIONS; i++)
                    {
                        avgEmotion = 0;
                        for (int j = 0; j < gEmotionFilterList[index].Count; j++)
                        {
                            avgEmotion += gEmotionFilterList[index][j][i];
                        }

                        avgEmotion = avgEmotion / gEmotionFilterList[index].Count();
                        gEmotionsSmoothed[index][i] = avgEmotion;
                    }

                    gEmotionFilterList[index].RemoveAt(0);
                }
            }
        }

        private float DeltaStep(int ageFilterFramesReached)
        {
            if (ageFilterFramesReached == 0)
            {
                return 0.05f;
            }
            else
                return 0.005f;
        }


        #endregion



        #region OpenGL Tracker And Detector Display

        /// <summary>
        /// Handles the OpenGLDraw event of the openGLControl control.
        /// 
        /// Locking mechanism is used to synchronize frame and tracking results rendering (lockFaceData).
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="args">The <see cref="SharpGL.SceneGraph.OpenGLEventArgs"/> instance containing the event data.</param>
        /// 
        private void openGLControl_OpenGLDraw(object sender, OpenGLEventArgs args)
        {
            lock (lockFaceData)
            {
                if (isTracked == true && gTrackerStatus[0] != 0)
                {
                    isTracked = false;

                    if (activeVisageMode == VISAGE_MODE.TRACKER && gFaceData != null)
                    {
                        trackerDisplay();
                    }
                }
            }

            if (isDetected == true)
            {
                if (activeVisageMode == VISAGE_MODE.DETECTOR && dataArray != null)
                {
                    inDrawDetection = true;
                    detectorDisplay();
                    inDrawDetection = false;
                }
            }
        }

        /// <summary>
        /// Calls the VisageRendering' DisplayResults function.
        /// 
        /// Called in a separate thread controlled by openGLControl_OpenGLDraw() function when the active application mode is set to TRACKER mode.
        /// </summary>
        private void trackerDisplay()
        {
            cbFrame.IsEnabled = true;

            int winWidth = (int)openGLControl.ActualWidth;
            int winHeight = (int)openGLControl.ActualHeight;

            AdjustAspect(ref gFaceData[0], ref winWidth, ref winHeight);

            gl.Viewport(0, 0, winWidth, winHeight); // Reset The Current Viewport

            // clear the frame buffer before next drawing
            gl.Clear(OpenGL.GL_DEPTH_BUFFER_BIT | OpenGL.GL_COLOR_BUFFER_BIT);

            lFPS.Text = "Tracker FPS: " + gFaceData[0].FrameRate.ToString("n2");
            lPureTrackTime.Text = "Track time: " + elapsedTime.ToString() + " ms";
            //var b = gFaceData.FaceRotation.Select(x => { return Math.Round(x, 2); }).ToArray();
            lTranslation.Text = String.Format("Translation: {0}, {1}, {2}",
                Math.Round(gFaceData[0].FaceTranslation[0], 2), Math.Round(gFaceData[0].FaceTranslation[1], 2), Math.Round(gFaceData[0].FaceTranslation[2], 2));
            lRotation.Text = String.Format("Rotation: {0}, {1}, {2}",
                Math.Round(VisageRendering.VS_RAD2DEG(gFaceData[0].FaceRotation[0]), 2),
                Math.Round(VisageRendering.VS_RAD2DEG(gFaceData[0].FaceRotation[1]), 2),
                Math.Round(VisageRendering.VS_RAD2DEG(gFaceData[0].FaceRotation[2]), 2));
            lStatus.Text = "Status: " + GetTrackerStatusString();

            //Display frame separately from tracking results
            if ((bool)cbFrame.IsChecked)
            {
                gVisageRendering.DisplayFrame(frame, winWidth, winHeight);
            }
          
            //For every tracked face, display tracking and analysis results without displaying the frame (^)
            for (int i = 0; i < maxFacesTracker; ++i)
            {
                gVisageRendering.DisplayResults(frame, gFaceData[i], gTrackerStatus[i], winWidth, winHeight, ACTIVE_DISPLAY ^ VisageRendering.DISPLAY_FRAME, gEmotionsSmoothed[i], gGenderSmoothed[i], gAgeSmoothed[i], currentDisplayName[i], texCoords);
            }
        }

        /// <summary>
        /// Calls the Calls the VisageRendering' DisplayResults function for every detected face.
        /// 
        /// Called in a separate thread controlled by openGLControl_OpenGLDraw() function when the active application mode is set to DETECTOR mode.
        /// </summary>
        private void detectorDisplay()
        {
            if (isDetected)
            {
                int winWidth = (int)openGLControl.ActualWidth;
                int winHeight = (int)openGLControl.ActualHeight;

                AdjustAspect(ref dataArray[0], ref winWidth, ref winHeight);

                //Reset the current viewport
                gl.Viewport(0, 0, winWidth, winHeight);

                //Clear the frame buffer before next drawing
                gl.Clear(OpenGL.GL_DEPTH_BUFFER_BIT | OpenGL.GL_COLOR_BUFFER_BIT);

                lFPS.Text = "";
                lPureTrackTime.Text = "";
                lTranslation.Text = "";
                lRotation.Text = "";
                lStatus.Text = "";

                //Display frame separately from detection results
                gVisageRendering.DisplayFrame(frame, winWidth, winHeight);

                //For every detected face, display detection and analysis results without displaying the frame (^)
                for (int i = 0; i < numFaces; i++)
                {
                    gVisageRendering.DisplayResults(frame, dataArray[i], 1, winWidth, winHeight, ACTIVE_DISPLAY ^ VisageRendering.DISPLAY_FRAME, gEmotionList[i], gGenderArray[i], gAgeArray[i], currentDisplayName[i], texCoords);
                }

                if (numFaces == 0)
                {
                    gl.DrawText(winWidth / 2, winHeight / 2, 1.0f, 0.0f, 0.0f, "Arial", 25, "Could not detect face");
                }
            }
        }

        private string GetTrackerStatusString()
        {
            string s = "";

            switch (gTrackerStatus[0])
            {
                case (int)TRACK_STAT.OFF:
                    s = "OFF";
                    break;
                case (int)TRACK_STAT.INIT:
                    s = "INIT";
                    break;
                case (int)TRACK_STAT.OK:
                    s = "OK";
                    break;
                case (int)TRACK_STAT.RECOVERING:
                    s = "RECOVERING";
                    break;
            }

            return s;
        }

        #endregion


        #region BackgroundWorker Control

        /// <summary>
        /// Demonstrates running a new tracking thread for tracking from video.
        /// 
        /// The function is called by clicking on the Track from video button.
        /// </summary>
        /// <param name="file">Video on which the tracking will be performed.</param>
        private void runTrackFromVideo(string file)
        {
            //Disable rendering until worker starts
            isTracked = false;

            //Check if worker exists and if it is currently working, if yes cancel worker
            if (worker != null)
            {
                //
                if (worker.IsBusy)
                {
                    worker.CancelAsync();
                }

                //Wait for the worker to exit before running a new one
                while (worker.IsBusy)
                {
                    Application.Current.Dispatcher.Invoke(DispatcherPriority.Background,
                                                new Action(delegate { }));
                }
            }

            //Change the input mode to video mode
            activeInputMode = INPUT_MODE.VIDEO;

            //Start tracking from video in another thread
            worker = new BackgroundWorker();
            worker.WorkerSupportsCancellation = true;
            worker.DoWork += worker_DoWorkVideo;
            worker.RunWorkerAsync(file);
        }

        /// <summary>
        /// Demonstrates running a new tracking thread for tracking from image.
        /// 
        /// The function is called by clicking on the Track from image icon.
        /// </summary>
        /// <param name="file">Image on which the tracking will be performed.</param>
        /// <param name="activeMode">Current active mode. Can be DETECTOR or TRACKER.</param>
        private void runTrackFromImage(string file, VISAGE_MODE activeMode)
        {
            //Disable rendering until worker starts
            isTracked = false;

            //Check if worker exists and if it is currently working, if yes cancel worker
            if (worker != null)
            {
                if (worker.IsBusy)
                {
                    worker.CancelAsync();
                }

                //Wait for the worker to exit before running a new one
                while (worker.IsBusy)
                {
                    Application.Current.Dispatcher.Invoke(DispatcherPriority.Background,
                                                new Action(delegate { }));
                }
            }

            //Change the input mode to image mode
            activeInputMode = INPUT_MODE.IMAGE;

            //Start tracking from image in another thread
            worker = new BackgroundWorker();
            worker.WorkerSupportsCancellation = true;
            List<object> arguments = new List<object>();
            arguments.Add(file);
            arguments.Add(activeMode);
            worker.DoWork += worker_DoWorkImage;
            worker.RunWorkerAsync(arguments);
        }

        /// <summary>
        /// Demonstrates running a new tracking thread for tracking from camera.
        /// 
        /// The function is called by clicking on the Track from camera icon.
        /// </summary>
        private void runTrackFromCam()
        {
            //Disable rendering until worker starts
            isTracked = false;

            //Check if worker exists and if it is currently working, if yes cancel worker
            if (worker != null)
            {
                if (worker.IsBusy)
                {
                    worker.CancelAsync();
                }

                //Wait for the worker to exit before running a new one
                while (worker.IsBusy)
                {
                    Application.Current.Dispatcher.Invoke(DispatcherPriority.Background,
                                                new Action(delegate { }));
                }
            }

            //Change the input mode to camera mode
            activeInputMode = INPUT_MODE.CAMERA;

            //Start tracking from camera in another thread
            worker = new BackgroundWorker();
            worker.WorkerSupportsCancellation = true;
            worker.DoWork += worker_DoWorkCam;
            worker.RunWorkerAsync();
        }

        /// <summary>
        /// Demonstrates running face recognition in a background thread. In case the worker is busy the function returns effectively skipping that frame.
        /// </summary>
        /// <param name="frame">Input frame on which the recognition is made</param>
        /// <param name="faceDatas">Input face data array</param>
        /// <param name="trackerStatus">tracker status array</param>
        private void runFaceRecognition(VSImage frame, FaceData[] faceDatas, int[] trackerStatus)
        {
            if (!(FR_CHECKED))
                return;

            //Check if worker exists and if it is currently working
            if (fr_worker != null)
            {
                if (fr_worker.IsBusy)
                {
                    return;
                }
            }
            else
            {
                fr_worker = new BackgroundWorker();
                fr_worker.DoWork += worker_DoWorkMatchFace;
                fr_worker.WorkerSupportsCancellation = true;
            }

            //Start tracking from image in another thread
            List<object> arguments = new List<object>();
            arguments.Add(frame);
            arguments.Add(faceDatas);
            arguments.Add(trackerStatus);
            fr_worker.RunWorkerAsync(arguments);
        }

        /// <summary>
        /// Demonstrates running face analysis in a background thread. In case the worker is busy the function returns effectively skipping that frame.
        /// </summary>
        /// <param name="frame">Input frame on which the analysis is made</param>
        /// <param name="faceData">Input face data array</param>
        /// <param name="trackerStatus">tracker status array</param>
        private void runFaceAnalysis(VSImage frame, FaceData[] faceData, int[] trackerStatus)
        {
            if (!(FA_AGE_CHECKED || FA_GEN_CHECKED || FA_EMO_CHECKED))
                return;

            //Check if worker exists and if it is currently working
            if (fa_worker != null)
            {
                if (fa_worker.IsBusy)
                {
                    return;
                }
            }
            else
            {
                fa_worker = new BackgroundWorker();
                fa_worker.DoWork += worker_DoAnalyzeFace;
                fa_worker.WorkerSupportsCancellation = true;
            }

            //Start tracking from image in another thread
            List<object> arguments = new List<object>();
            arguments.Add(frame);
            arguments.Add(faceData);
            arguments.Add(trackerStatus);
            fa_worker.RunWorkerAsync(arguments);
        }
        #endregion


        #region Style Control

        /// <summary>
        /// Change button style (camera, video or image buttons) depending on the chosen INPUT_MODE
        /// </summary>
        /// <param name="mode">chosen INPUT_MODE</param>
        private void changeButtonStyle(INPUT_MODE mode)
        {
            camButton.Content = FindResource("CameraPassive");
            videoButton.Content = FindResource("VideoPassive");
            imageButton.Content = FindResource("ImagePassive");

            lCamera.Foreground = (System.Windows.Media.Brush)FindResource("Passive");
            lVideo.Foreground = (System.Windows.Media.Brush)FindResource("Passive");
            lImage.Foreground = (System.Windows.Media.Brush)FindResource("Passive");

            if (mode == INPUT_MODE.CAMERA)
            {
                camButton.Content = FindResource("CameraActive");
                lCamera.Foreground = (System.Windows.Media.Brush)FindResource("Active");
            }
            else if (mode == INPUT_MODE.VIDEO)
            {
                videoButton.Content = FindResource("VideoActive");
                lVideo.Foreground = (System.Windows.Media.Brush)FindResource("Active");
            }
            else if (mode == INPUT_MODE.IMAGE)
            {
                imageButton.Content = FindResource("ImageActive");
                lImage.Foreground = (System.Windows.Media.Brush)FindResource("Active");
            }
        }

        /// <summary>
        /// Change radio button style (configuration choice) depending on the chose configuration
        /// </summary>
        /// <param name="sender">sender object from the click event</param>
        private void changeConfStyle(object sender)
        {
            rbFFTHigh.Foreground = (System.Windows.Media.Brush)FindResource("Passive");
            rbFFTLow.Foreground = (System.Windows.Media.Brush)FindResource("Passive");

            (sender as RadioButton).Foreground = (System.Windows.Media.Brush)FindResource((sender as RadioButton).Foreground == (System.Windows.Media.Brush)FindResource("Active") ? "Passive" : "Active"); 
        }

        #endregion


        #region Button Control

        private void camButton_Click(object sender, RoutedEventArgs e)
        {
            runTrackFromCam();

            //Set style
            changeButtonStyle(INPUT_MODE.CAMERA);
        }

        private void videoButton_Click(object sender, RoutedEventArgs e)
        {
            // File dialog
            Microsoft.Win32.OpenFileDialog dlg = new Microsoft.Win32.OpenFileDialog();
            dlg.InitialDirectory = System.IO.Path.GetFullPath(videoInitialDir);

            dlg.DefaultExt = ".avi";
            dlg.Filter = "AVI Files|*.avi;*.mp4";

            Nullable<bool> result = dlg.ShowDialog();

            if (result != true)
            {
                return;
            }

            videoInitialDir = System.IO.Path.GetDirectoryName(dlg.FileName);
            currentVideo = dlg.FileName;
            runTrackFromVideo(currentVideo);

            //Set style
            changeButtonStyle(INPUT_MODE.VIDEO);
        }

        private void imageButton_Click(object sender, RoutedEventArgs e)
        {
            // File dialog
            Microsoft.Win32.OpenFileDialog dlg = new Microsoft.Win32.OpenFileDialog();

            // Set filter for file extension and default file extension
            dlg.DefaultExt = ".jpg";
            dlg.Filter = "JPG Files (*.jpg)|*.jpg|JPEG Files (*.jpeg)|*.jpeg|PNG Files (*.png)|*.png|GIF Files (*.gif)|*.gif| BMP Files (*.bmp)|*.bmp";
            dlg.InitialDirectory = System.IO.Path.GetFullPath(imageInitialDir);

            // Display OpenFileDialog by calling ShowDialog method
            Nullable<bool> result = dlg.ShowDialog();

            // Check if file is chosen
            if (result != true)
            {
                return;
            }

            ResetFRInitialization();

            imageInitialDir = System.IO.Path.GetDirectoryName(dlg.FileName);
            currentImage = dlg.FileName;
            runTrackFromImage(currentImage, activeVisageMode);

            //Set style
            changeButtonStyle(INPUT_MODE.IMAGE);
        }

        /// <summary>
        /// Toggles application mode from TRACKER to DETECTOR and vice versa.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void trackerDetectorButton_Click(object sender, RoutedEventArgs e)
        {
            if (activeVisageMode == VISAGE_MODE.TRACKER)
            {
                //toggle detector mode
                activeVisageMode = VISAGE_MODE.DETECTOR;
                //toggle freeze gallery button
                ResetFRInitialization();
                freeze_gallery = true;
                cbFreeze.IsChecked = true;
                //
                manageControls(activeVisageMode);
                trackerDetectorButton.Content = "Switch to Tracker";
                // File dialog
                Microsoft.Win32.OpenFileDialog dlg = new Microsoft.Win32.OpenFileDialog();

                // Set filter for file extension and default file extension
                dlg.DefaultExt = ".jpg";
                dlg.Filter = "JPG Files (*.jpg)|*.jpg|JPEG Files (*.jpeg)|*.jpeg|PNG Files (*.png)|*.png|GIF Files (*.gif)|*.gif| BMP Files (*.bmp)|*.bmp";

                // Display OpenFileDialog by calling ShowDialog method
                Nullable<bool> result = dlg.ShowDialog();

                // Check if file is chosen
                if (result != true)
                {
                    return;
                }
                currentImage = dlg.FileName;
                runTrackFromImage(currentImage, activeVisageMode);

            }

            else if (activeVisageMode == VISAGE_MODE.DETECTOR)
            {
                //
                ResetFRInitialization();
                //toggle freeze gallery button
                freeze_gallery = false;
                cbFreeze.IsChecked = false;
                //toggle tracker mode
                activeVisageMode = VISAGE_MODE.TRACKER;
                manageControls(activeVisageMode);
                trackerDetectorButton.Content = "Switch to Detector";
                dataArray = null;
                numFaces = 0;
                runTrackFromCam();
            }
        }

        private void cameraSettingsButton_Click(object sender, RoutedEventArgs e)
        {
            if (cameraCapture != null)
                cameraCapture.SetCaptureProperty(CapProp.Settings, 0);
        }

        private void cbCameraChoice_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            currentCameraDevice = (sender as ComboBox).SelectedIndex;
            runTrackFromCam();
            //Set style
            changeButtonStyle(INPUT_MODE.CAMERA);
        }

        /// <summary>
        /// Enable age estimation checkbox and change to appropriate color.
        /// </summary>
        private void enableAgeEstimCheckbox()
        {
            cbAge.IsEnabled = true;
            cbAge.ToolTip = null;
            if (cbAge.Foreground == ((System.Windows.Media.Brush)FindResource("Disabled")))
            {
                cbAge.Foreground = ((System.Windows.Media.Brush)FindResource("Passive"));
            }
        }

        /// <summary>
        /// Disable age estimation checkbox and change color for the use case when one channel or four channel is loaded
        /// (age estimation is supported for 3 channel images only).
        /// </summary>
        private void disableAgeEstimCheckbox()
        {
            cbAge.IsEnabled = false;
            cbAge.IsChecked = false;
            ToolTip t = new ToolTip { Content = "Age esimation disabled for images with one channel." };
            cbAge.ToolTip = t;
            cbAge.Foreground = ((System.Windows.Media.Brush)FindResource("Disabled"));
        }
        #endregion


        #region Recognition Controls

        /// <summary>
        /// Load face recognition gallery (.frg) from file.
        /// 
        /// Opens a dialog to load the gallery (.frg) file.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void loadGalleryButton_Click(object sender, RoutedEventArgs e)
        {
            Microsoft.Win32.OpenFileDialog dlg = new Microsoft.Win32.OpenFileDialog();

            // Set filter for file extension and default file extension
            dlg.DefaultExt = ".frg";
            dlg.Filter = "Face Recognition Gallery Files (*.frg)|*.frg";
            // Set the initial directory
            dlg.InitialDirectory = System.IO.Path.GetFullPath(@"\..\Samples\OpenGL\data\ShowcaseDemo");

            if (dlg.ShowDialog() == true)
            {
                string filename = dlg.FileName;

                lock (lockClearRecognition)
                {
                    gVisageFaceRecognition.LoadGallery(filename);

                    int count = gVisageFaceRecognition.GetDescriptorCount();

                    for (int i = 0; i < count; i++)
                    {
                        // fetch all unique names from the FR gallery and store in global array for future use
                        string name = gVisageFaceRecognition.GetDescriptorName(i);
                        if (!gUniqueNames.Contains(name))
                        {
                            gUniqueNames.Add(name);
                            indices_to_names[numPersons] = name;
                            numPersons += 1;
                            FillNames();
                        }
                    }

                    // By setting numInitFrames to numInitFramesThreshold skip initialization phase
                    for (int i = 0; i < MAX_FACES_DETECTOR; i++)
                        numInitFrames[i] = numInitFramesThreshold;
                    // Clear any descriptors stored in the buffer during initialization phase in case the init phase was interrupted
                    for (int trackerIndex = 0; trackerIndex < maxFacesTracker; ++trackerIndex)
                    {
                        descBuffer[trackerIndex].Clear();
                    }
                }
            }
        }

        /// <summary>
        /// Save face recognition gallery (.frg) to a file.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void saveGalleryButton_Click(object sender, RoutedEventArgs e)
        {
            Microsoft.Win32.SaveFileDialog dlg = new Microsoft.Win32.SaveFileDialog();

            dlg.DefaultExt = ".frg";
            dlg.Filter = "Face Recognition Gallery Files (*.frg)|*.frg";

            if (dlg.ShowDialog() == true)
            {
                string filename = dlg.FileName;
                gVisageFaceRecognition.SaveGallery(filename);
            }
        }

        /// <summary>
        /// Clear face recognition GUI element representing the gallery.
        /// 
        /// It will also clear all the buffers for every face.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void clearGalleryButton_Click(object sender, RoutedEventArgs e)
        {
            ClearFaceRecognition();
        }

        /// <summary>
        /// A helper function used for populating the GUI element representing face recognition gallery from a variable.
        /// </summary>
        private void FillNames()
        {
            lbxNames.Items.Clear();
            
            for (int i = 0; i < gUniqueNames.Count(); i++)
            {
                TextBox tbx = new TextBox();

                tbx.Text = gUniqueNames[i];
                tbx.Tag = i.ToString();
                tbx.BorderBrush = System.Windows.Media.Brushes.White;

                tbx.TextChanged += new TextChangedEventHandler(OnNameTextChanged);

                lbxNames.Items.Add(tbx);
            }
        }

        /// <summary>
        /// A helper function for renaming a descriptor once it is added to the GUI element representing face recognition gallery.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnNameTextChanged(object sender, TextChangedEventArgs e)
        {
            TextBox tbx = sender as TextBox;

            int index = lbxNames.Items.IndexOf(tbx);

            string current_name = indices_to_names[index];
            for (int i = 0; i < gVisageFaceRecognition.GetDescriptorCount(); i++)
                if (gVisageFaceRecognition.GetDescriptorName(i) == current_name)
                    gVisageFaceRecognition.ReplaceDescriptorName(tbx.Text, i);

            for (int i = 0; i < gUniqueNames.Count(); i++)
                if (gUniqueNames[i] == current_name)
                    gUniqueNames[i] = tbx.Text;
            indices_to_names[index] = tbx.Text;
        }

        #endregion


        #region Configuration Control

        private void rbFFTHigh_Clicked(object sender, RoutedEventArgs e)
        {
            if (gVisageTracker != null)
            {
                if (activeInputMode == INPUT_MODE.CAMERA)
                {
                    gVisageTracker.SetTrackerConfiguration("../Samples/data/Facial Features Tracker - High.cfg");
                    runTrackFromCam();
                }

                else if (activeInputMode == INPUT_MODE.VIDEO)
                {
                    gVisageTracker.SetTrackerConfiguration("../Samples/data/Facial Features Tracker - High.cfg");
                    runTrackFromVideo(currentVideo);
                }

                else if (activeInputMode == INPUT_MODE.IMAGE)
                {
                    gVisageTracker.SetTrackerConfiguration("../Samples/data/Facial Features Tracker - High.cfg");
                    runTrackFromImage(currentImage, activeVisageMode);
                }

                //set style
                changeConfStyle(sender);

                texCoordsStaticLoaded = true;
            }
        }

        private void rbFFTLow_Clicked(object sender, RoutedEventArgs e)
        {
            if (gVisageTracker != null)
            {
                if (activeInputMode == INPUT_MODE.CAMERA)
                {
                    gVisageTracker.SetTrackerConfiguration("../Samples/data/Facial Features Tracker - Low.cfg");
                    runTrackFromCam();
                }

                else if (activeInputMode == INPUT_MODE.VIDEO)
                {
                    gVisageTracker.SetTrackerConfiguration("../Samples/data/Facial Features Tracker - Low.cfg");
                    runTrackFromVideo(currentVideo);
                }

                else if (activeInputMode == INPUT_MODE.IMAGE)
                {
                    gVisageTracker.SetTrackerConfiguration("../Samples/data/Facial Features Tracker - Low.cfg");
                    runTrackFromImage(currentImage, activeVisageMode);
                }

                //set style
                changeConfStyle(sender);

                texCoordsStaticLoaded = true;
            }
        }

        #endregion


        #region Render Options Control

        private void checkBox_onClick(object sender, RoutedEventArgs e)
        {
            //reset and update display options when checkbox is checked/unchecked
            updateDisplay();

            (sender as CheckBox).Foreground = (System.Windows.Media.Brush)FindResource((sender as CheckBox).Foreground == ((System.Windows.Media.Brush)FindResource("Active")) ? "Passive" : "Active"); 
        }

        private void updateDisplay()
        {
            ACTIVE_DISPLAY = 0;

            if ((bool)cbFrame.IsChecked)
                ACTIVE_DISPLAY += VisageRendering.DISPLAY_FRAME;

            if ((bool)cbFeaturePoints.IsChecked)
                ACTIVE_DISPLAY += VisageRendering.DISPLAY_FEATURE_POINTS;

            if ((bool)cbSplines.IsChecked)
                ACTIVE_DISPLAY += VisageRendering.DISPLAY_SPLINES;

            if ((bool)cbGaze.IsChecked)
                ACTIVE_DISPLAY += VisageRendering.DISPLAY_GAZE;

            if ((bool)cbAxes.IsChecked)
                ACTIVE_DISPLAY += VisageRendering.DISPLAY_AXES;

            if ((bool)cbWireFrame.IsChecked)
                ACTIVE_DISPLAY += VisageRendering.DISPLAY_WIRE_FRAME;

            if ((bool)cbAge.IsChecked)
            {
                ACTIVE_DISPLAY += VisageRendering.DISPLAY_AGE;
                FA_AGE_CHECKED = true;
            }
            else
                FA_AGE_CHECKED = false;

            if ((bool)cbGender.IsChecked)
            {
                ACTIVE_DISPLAY += VisageRendering.DISPLAY_GENDER;
                FA_GEN_CHECKED = true;
            }
            else
                FA_GEN_CHECKED = false;

            if ((bool)cbEmotion.IsChecked)
            {
                ACTIVE_DISPLAY += VisageRendering.DISPLAY_EMOTION;
                FA_EMO_CHECKED = true;
            }
            else
                FA_EMO_CHECKED = false;


            if ((bool)cbTexturedModel.IsChecked)
                ACTIVE_DISPLAY += VisageRendering.DISPLAY_TEXTURED_MODEL;

            if ((bool)cbTrackingQuality.IsChecked)
                ACTIVE_DISPLAY += VisageRendering.DISPLAY_TRACKING_QUALITY + VisageRendering.DISPLAY_POINT_QUALITY;

            if ((bool)cbRecognition.IsChecked)
            {
                ACTIVE_DISPLAY += VisageRendering.DISPLAY_NAME;
                FR_CHECKED = true;
            }
            else
            {
                FR_CHECKED = false;
            }
            ACTIVE_MIRROR = ((bool)cbMirror.IsChecked) ? true : false;

            if ((bool)cbFreeze.IsChecked)
            {
                freeze_gallery = true;
            }
            else
            {
                freeze_gallery = false;
            }
        }

        private void manageControls(VISAGE_MODE activeMode)
        {
            //enable/disable controls for the detector mode
            if (activeMode == VISAGE_MODE.DETECTOR)
            {
                camButton.IsEnabled = false;
                videoButton.IsEnabled = false;
                rbFFTHigh.IsEnabled = false;
                rbFFTLow.IsEnabled = false;
                if (cbFrame.IsChecked != true)
                {
                    cbFrame.IsChecked = true;
                    updateDisplay();
                }
                cbFrame.IsEnabled = false;
                lCamera.IsEnabled = false;
                lVideo.IsEnabled = false;

                //
                changeButtonStyle(INPUT_MODE.IMAGE);
            }

            //enable/disable controls for the tracker mode
            else if (activeMode == VISAGE_MODE.TRACKER)
            {
                camButton.IsEnabled = true;
                videoButton.IsEnabled = true;
                rbFFTHigh.IsEnabled = true;
                rbFFTLow.IsEnabled = true;
                cbFrame.IsEnabled = true;
                cbGender.IsEnabled = true;
                cbEmotion.IsEnabled = true;
                cbMirror.IsEnabled = true;
                lCamera.IsEnabled = true;
                lVideo.IsEnabled = true;
                updateDisplay();

                //
                changeButtonStyle(INPUT_MODE.CAMERA);
            }
        }

        #endregion


        /// <summary>
        /// Adjust aspect of the rendered image on window resize.
        /// </summary>
        /// <param name="faceData">Input FaceData object which contains the frame necessary for adjusting aspect.</param>
        /// <param name="width">Width that will be adjusted.</param>
        /// <param name="height">Height that will be adjusted.</param>
        private void AdjustAspect(ref FaceData faceData, ref int width, ref int height)
        {
            int mWidth = frame.width;
            int mHeight = frame.height;

            float aspect = mWidth / (float)mHeight;
            float tmp;

            float winWidth = (float)openGLControl.ActualWidth;
            float winHeight = (float)openGLControl.ActualHeight;

            if (aspect < 1.0f)
            {
                tmp = winHeight;
                winHeight = winWidth / aspect;
                if (winHeight > tmp)
                {
                    winWidth = winWidth * tmp / winHeight;
                    winHeight = tmp;
                }
            }
            else
            {
                tmp = winWidth;
                winWidth = winHeight * aspect;
                if (winWidth > tmp)
                {
                    winHeight = winHeight * tmp / winWidth;
                    winWidth = tmp;
                }
            }

            width = (int)winWidth;
            height = (int)winHeight;
        }

        /// <summary>
        /// Sample usage of VisageFaceAnalyser::GetNormalizedFaceImage() function.
        /// </summary>
        /// <param name="inputFrame">Frame with face to be normalized.</param>
        /// <param name="faceData">Face data object with face information.</param>
        private void NormalizeFace(VSImage inputFrame, FaceData faceData)
        {
            int normFaceWidth = 512;
            int normFaceHeight = 512;
            int normFaceNChannels = 1;

            //Create output image for normalized face image to be stored into
            VSImage normFaceImage = new VSImage(normFaceWidth, normFaceHeight, VS_DEPTH._8U, normFaceNChannels);

            //Create an empty FDP object for normalized FDP values to be stored into
            FDP fdp = new FDP();

            int grayImageWidthStep = (inputFrame.width + 3) / 4 * 4;
            //Create grayscale input image
            VSImage grayImage = new VSImage(inputFrame.width, inputFrame.height, VS_DEPTH._8U, normFaceNChannels);

            //convert from RGB to grayscale - luminosity method 0.21 R + 0.72 G + 0.07 B.
            for (int v = 0; v < inputFrame.height; v++)
            {
                for (int u = 0; u < inputFrame.width; u++)
                {
                    if (inputFrame.nChannels == 3)
                    {
                        float red = inputFrame.imageData[v * inputFrame.widthStep + u * 3 + 0];
                        float green = inputFrame.imageData[v * inputFrame.widthStep + u * 3 + 1];
                        float blue = inputFrame.imageData[v * inputFrame.widthStep + u * 3 + 2];
                        grayImage.imageData[v * grayImage.widthStep + u] = (byte)(0.21f * red + 0.72f * green + 0.07f * blue);
                    }
                    else if (inputFrame.nChannels == 1)
                    {
                        grayImage.imageData[v * grayImage.widthStep + u] = inputFrame.imageData[v * inputFrame.widthStep + u];
                    }
                }
            }

            //
            gVisageFaceAnalyser.GetNormalizedFaceImage(grayImage, normFaceImage, faceData, ref fdp, VS_NORM.POSE, @"..\Samples\data");

            //Create a 1 channel image of normFaceWidth x normFaceWidth and set it to black
            Mat img = new Mat(normFaceHeight, normFaceWidth, DepthType.Cv8U, 1);
            img.SetTo(new Bgr(0, 0, 0).MCvScalar);
            Image<Gray, Byte> displayImage = img.ToImage<Gray, Byte>();

            //Copy image data from VSImage to EmguCV image
            for (int v = 0; v < displayImage.Height; v++)
            {
                for (int u = 0; u < displayImage.Width; u++)
                {
                    displayImage.Data[v, u, 0] = normFaceImage.imageData[v * displayImage.Width + u];
                }
            }

            //Draw pupils and chin for demonstration
            //Invert y coordinate due to different coordinate system of EmguCV
            float[] leftPupil = fdp.GetFPPos("3.5");
            float[] rightPupil = fdp.GetFPPos("3.6");
            float[] chin = fdp.GetFPPos("2.1");
            displayImage.Draw(new CircleF(new PointF(leftPupil[0] * displayImage.Width, (1 - leftPupil[1]) * displayImage.Height), 2), new Gray(255), 1);
            displayImage.Draw(new CircleF(new PointF(rightPupil[0] * displayImage.Width, (1 - rightPupil[1]) * displayImage.Height), 2), new Gray(255), 1);
            displayImage.Draw(new CircleF(new PointF(chin[0] * displayImage.Width, (1 - chin[1]) * displayImage.Height), 2), new Gray(255), 1);

            //Draw image in a window
            String normWindow = "Normalized face image"; //The name of the window
            CvInvoke.NamedWindow(normWindow); //Create the window using the specific name
            CvInvoke.Imshow(normWindow, displayImage); //Show the image
            CvInvoke.WaitKey(0);  //Wait for the key pressing event
            CvInvoke.DestroyWindow(normWindow); //Destroy the window if key is pressed
        }

        /// <summary>
        /// Helper functions for loading image from image path. Flips image.
        /// </summary>
        /// <param name="path">Image path</param>
        /// <returns></returns>
        private VSImage LoadImage(string path)
        {
            Bitmap bmp = new Bitmap(path);
            bmp.RotateFlip(RotateFlipType.Rotate180FlipX);
            int bitmapStride = 0;
            byte[] bmp_byte = BitmapToByteArray(bmp, ref bitmapStride);
            BitmapData bmpdata = bmp.LockBits(new System.Drawing.Rectangle(0, 0, bmp.Width, bmp.Height), ImageLockMode.ReadOnly, bmp.PixelFormat);
            VSImage image = new VSImage(bmp.Width, bmp.Height, VS_DEPTH._8U, 3); // nChannels ?
            image.imageData = bmp_byte;
            return image;
        }

        /// <summary>
        /// Extracts texture coordinates from the loaded .obj model and reorderes them.
        /// </summary>
        /// <param name="model"></param>
        void getTexCoordsArray(TexturedModel model)
        {
            texCoords = new float[2 * VERTEX_NUMBER];
            int indexPoint;
            int texIndexPoint;
            foreach (Face face in model.faces)
            {
                for (int i = 0; i < 3; i++)
                {
                    indexPoint = face.indexPoints[i];
                    texIndexPoint = face.texIndexPoints[i];

                    texCoords[2 * (indexPoint - 1)] = model.vt[texIndexPoint - 1].x;
                    texCoords[2 * (indexPoint - 1) + 1] = model.vt[texIndexPoint - 1].y;
                }
            }
        }
    }
}
