namespace VisageSDK
{
    
    /*! \mainpage  visage|SDK VisageTrackerUnity example project
     *
     * \if WIN64_DOXY
     * \htmlonly
     * <table border="0">
     * <tr>
     * <td width="32"><a href="../../../../../../bin64/VisageTrackerUnityDemo.exe"><img src="../../../../../../doc/OpenGL/images/run_sample.png" border=0 title="Run Sample (this may not work in all browsers, in such case please run VisageTrackerUnityDemo.exe from the visageSDK\bin64 folder.)"></a></td>
     * <td width="32"><a href="../../../../../../bin64"><img src="../../../../../../doc/OpenGL/images/open_bin_folder.png" border=0 title="Open Binary Folder (visageSDK\bin64)"></a></td>
     * <td width="32"><a href="../../../../data/VisageTrackerUnity"><img src="../../../../../../doc/OpenGL/images/open_data_folder.png" border=0 title="Open Data Folder"></a></td>
     * <td width="32"><a href="../../../../data/Videos"><img src="../../../../../../doc/OpenGL/images/open_folder.png" border=0 title="Open Video Folder"></a></td>
     * <td width="32"><a href="../../../../build/msvc140/VisageTrackerUnity"><img src="../../../../../../doc/OpenGL/images/open_project_folder.png" border=0 title="Open Project Folder"></a></td>
     * <td width="32"><a href="../../../../source/VisageTrackerUnity"><img src="../../../../../../doc/OpenGL/images/open_source_folder.png" border=0 title="Open Source Code Folder"></a></td>
     * </tr>
     * </table>
     * \endhtmlonly
     * \else
     * \htmlonly
     * <table border="0">
     * <tr>
     * <td width="32"><a href="../../../../../../bin/VisageTrackerUnityDemo.exe"><img src="../../../../../../doc/OpenGL/images/run_sample.png" border=0 title="Run Sample (this may not work in all browsers, in such case please run VisageTrackerUnityDemo.exe from the visageSDK\bin folder.)"></a></td>
     * <td width="32"><a href="../../../../../../bin"><img src="../../../../../../doc/OpenGL/images/open_bin_folder.png" border=0 title="Open Binary Folder (visageSDK\bin)"></a></td>
     * <td width="32"><a href="../../../../data/VisageTrackerUnity"><img src="../../../../../../doc/OpenGL/images/open_data_folder.png" border=0 title="Open Data Folder"></a></td>
     * <td width="32"><a href="../../../../data/Videos"><img src="../../../../../../doc/OpenGL/images/open_folder.png" border=0 title="Open Video Folder"></a></td>
     * <td width="32"><a href="../../../../build/msvc140/VisageTrackerUnity"><img src="../../../../../../doc/OpenGL/images/open_project_folder.png" border=0 title="Open Project Folder"></a></td>
     * <td width="32"><a href="../../../../source/VisageTrackerUnity"><img src="../../../../../../doc/OpenGL/images/open_source_folder.png" border=0 title="Open Source Code Folder"></a></td>
     * </tr>
     * </table>
     * \endhtmlonly
     * \endif
     * 
     * The VisageTrackerUnity sample project demonstrates the integration of visage|SDK with Unity game engine and is aimed at developers 
     * starting to use face tracking, face analysis and face recognition functionalities of visage|SDK in Unity game engine and development tool.
     * 
     * The sample project shows how face tracking is used to put a virtual object (glasses) on the user's face in Unity, and how to access 
     * the full 3D face model of the face. Sample also demonstrates how to use face tracking results to obtain face analysis and face recognition results.
     * Developers may use parts of this project to speed up their own development in Unity using visage|SDK 
     * or use this application as a starting point for own projects.
     * 
     * \section using Using the sample applications
     *
     * <br/><br/>
     * In order to try the sample application, you need to have a valid license key. More information on how to obtain license key and include it in the application can be found in 
     * <a href="../../../../../../doc/OpenGL/licensing.html"><em>Licensing</em></a> section of the documentation.
     * <br/><br/>
     * To start the application, double-click on VisageTrackerUnity.exe located in \if WIN64_DOXY <i>visageSDK/bin64 </i>\else <i>visageSDK/bin </i>\endif folder.
     *
     * Tracking will start automatically when a face is found in the camera frame. Available buttons are: 
     * - Play/Pause in the lower left corner (resumes/pauses tracking).
     * - Switch camera in the lower right corner.
     * - Switch effect in the upper right corner of the screen (switches between rendering glasses or a tiger texture)
     * - Enable face recognition in upper left corner of the screen
     *
     * On top of the video frames is a 3D model of glasses or tiger model (depending on which effect is chosen) that is transformed with the information about rotation and translation from the tracker. It blends seamlessly with the background video.
     * 
     * 
     * \section building Building the project
     * 
     * The files related to VisageTrackerUnity sample project are located in the Samples/OpenGL/data/VisageTrackerUnity subfolder of the visage|SDK folder.
     * 
     * The application was built and tested with version 2018.3.0f2.
     * 
     * To build VisageTrackerUnity sample two steps are involved: creating the Unity project from the provided Unity package and building the application.
     * 
     * \subsection creatingUnity Creating the Unity project from Unity package
     * 
     * To create the Unity project from provided Unity package follow these steps:
     * -# Create a new Unity project
     * -# Import VisageTrackerUnity.unitypackage (provided in Samples/OpenGL/data/VisageTrackerUnity folder) by selecting Assets->Import Package->Custom Package... item from the menu. That way all the assets that are used by the example project will be imported into the new project.
     * -# Select and open the Main scene.
     * 
     * \subsection generateApp Building the application
     * 
     * To build the application follow these steps:
     * -# Build the application by selecting File->Build settings and under Platform choose PC and Mac Standalone and set Windows as target platform. Check if the correct Architecture is set: \if WIN64_DOXY <i>x86_64</i>\else <i>x86</i>\endif.
     * -# Press the Build button to generate the project, a dialog window will appear. In this window, locate and set destination folder to \if WIN64_DOXY <i>visageSDK/bin64</i>\else <i>visageSDK/bin</i>\endif.
     * 
     * \subsection editor Running the project through Unity editor
     *
     * -# Follow steps on creating a Unity project.
     * -# Application is dependent on libraries located in \if WIN64_DOXY <i>visageSDK/bin64 </i>\else <i>visageSDK/bin </i>\endif folder. Copy libraries to the Unity project root folder (e.g. <i>C:/MyUnityProjects/VisageTrackerUnity</i>):
     * \if WIN64_DOXY
     *   - opencv_core2411.dll
     *   - opencv_imgproc2411.dll
     *   - VisageTrackerUnityPlugin64.dll
     *    - libVisageVision64.dll
     *     - libopenblas.dll
     *      - libgfortran-3.dll</i>
     *      - libquadmath-0.dll</i>
     *      - libgcc_s_seh-1.dll</i>
     *    - libVisageAnalyser64.dll
     * \else
     *   - opencv_core2411.dll
     *   - opencv_imgproc2411.dll
     *   - VisageTrackerUnityPlugin.dll
     *    - libVisageVision.dll
     *     - libopenblas.dll
     *      - libgfortran-3.dll</i>
     *      - libquadmath-0.dll</i>
     *      - libgcc_s_sjlj-1.dll</i>
     *    - libVisageAnalyser.dll
     * \endif
     * -# Create a <a href="https://docs.unity3d.com/Manual/StreamingAssets.html" target="New">StreamingAssets/Visage Tracker</a> folder in your Unity project Assets folder
     * -# Copy following files and folders from the <i>visageSDK/Samples/data</i> folder to the <i>StreamingAssets/Visage Tracker</i> folder:
     *   - bdtsdata folder
     *   - candide3.wfm
     *   - candide3.fdp
     *   - jk_300.wfm
     *   - jk_300.fdp
     *   - configuration file (e.g. Head Tracker.cfg)
     *
     * NOTE: In case you have license key file, add it also to the <i>StreamingAssets/Visage Tracker</i> folder. Additionally, set the <i>licenseString</i> member of the LicenseString class (script LicenseString.cs) so it contains name of the license key file.
	 *
     * \section implementation Implementation overview
     * 
     * The project consists of a main scene with the Unity’s GameObjects that provide different functionalities. A GameObject is the main building block of a scene in Unity. It consists of different Components and can parent other GameObjects. Manipulation of GameObjects is done with Scripts. It is recommended for developers to get familiar with Unity basics before continuing.
     * The integration with visage|SDK is done through a plugin (VisageTrackerUnityPlugin) that wraps native code calls to visage|SDK and provides functions that can be called from Unity scripts.
     * 
     * \subsection trackerGO Tracker GameObject
     * 
     * The core of the example is the Tracker GameObject. It consists of different components attached to it. The main component of the Tracker GameObject is the Tracker script component that handles the communication with the tracker (starts it and gets results from it). The behaviour of the script can be modified by changing its properties and the script code. Other properties can be added as well.
     * The existing Tracker script properties of interest are as follows:
     * - Controllable Objects: list of objects that will be transformed automatically while the face tracking is performed
     * - Config File: filename of the configuration file that will be used by the tracker
     * 
     * <b>Note:</b> Because of the different coordinate systems used by the visage|SDK face tracker and the Unity game engine mirroring around x-axis is applied to the relevant GameObjects.
     * 
     * Objects in the scene that are also in the Controllable Objects list are the Glasses Model GameObjects.
     * 
     * \subsection analyserGO Analyser GameObject
     * 
     * Analyser GameObject is used to obtain and display face analysis results. It relies on the functionality of the Tracker object, i.e. Tracker script component. 
     * The GameObject consists of different components attached to it. The main component of the Analyser GameObject is the Analyser script component that handles the communication with the face analyser 
     * (initializes it and gets results from it).
     * The behaviour of the script can be modified by changing its properties and the script code. Other properties can be added as well. The existing Analyser script properties of interest are as follows:
     * - Data Path Analysis: path to data files used to initialize face analysis.
     * - Analysis Data Element: prefab attached to the Analyser GameObject which contains the components needed to display information about the person's age, gender and emotions.
     * - Analysis list: list of Analysis Data Elements instances for each tracked face that will be transformed while the face tracking is performed.
     * 
     * \subsection recognitionGO Recognition GameObject
     * 
     * Recognition GameObject is used to obtain and display face recognition results. 
     * It relies on the functionality of the Tracker object, i.e. Tracker script component. The GameObject consists of different components attached to it.  
     * The main component of the Recognition GameObject is the Recognition script component that handles the communication with the face recognition (initializes it and gets results from it).
     * The behaviour of the script can be modified by changing its properties and the script code.
     * Other properties can be added as well. The existing Recognition script properties of interest are as follows: 
     * - Data R Folder: path to data files used to initialize face recognition.
     * - Recognition Gallery: UI panel object used to hold all collected indentities.
     * - Identity: prefab attached to the Recognition GameObject which contains text mesh used to display indentity of a particular person that will be transformed while face tracking is performed.
     * 
     * \subsection cameras Cameras
     * 
     * There are two cameras in the scene, one for 3D scene (Main Camera) and other for video display (Video Camera). The main camera "Field of View" is automatically set according to the Tracker focus (camera_focus parameter in configuration file) and input image aspect by the Tracker script. The Video Camera renders objects in the Video layer by using orthographic projection. 
     * 
     * \subsection videoObject Video object
     * 
     * Video GameObject contains Video Plane that is used for displaying video and is automatically scaled to input frame aspect ratio by the VideoController script attached to it. In this project the material used by the Video Plane is tinted green to differentiate it from the textured 3D face model that uses the same texture.
     * 
     * \subsection glassesObject Glasses object
     * 
     * The rotation and translation information from the tracker are applied to the Glasses object as they are in the Tracker script Controllable Objects list. This enables overlaying virtual objects to the tracked face that transform correctly with it. Other custom objects can be used instead of the glasses object.
     * For the correct size and positioning of the glasses objects and also on how to achieve occlusion refer to the <a href="../../../../../../doc/OpenGL/modeling_guide.pdf">Animation & AR modeling guide</a>.
     * 
     * 
     * \subsection plugin VisageTrackerUnityPlugin
     * The integration of visage|SDK face tracking with Unity is done through a plugin that wraps native code calls and provides functions that can be called from Unity scripts. The provided prebuilt plugin VisageTrackerUnityPlugin.dll can be used as-is. 
     * For more information about the plugin including it's source code, see the VisageTrackerUnityPlugin project and its documentation.
     */
}