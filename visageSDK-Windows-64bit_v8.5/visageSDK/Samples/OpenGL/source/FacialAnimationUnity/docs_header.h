namespace VisageSDK
{
    
    /*! \mainpage  visage|SDK FacialAnimationUnity example project
     *
     * \if WIN64_DOXY
     * \htmlonly
     * <table border="0">
     * <tr>
     * <td width="32"><a href="../../../../../../bin64/FacialAnimationUnityDemo.exe"><img src="../../../../../../doc/OpenGL/images/run_sample.png" border=0 title="Run Sample (this may not work in all browsers, in such case please run FacialAnimationUnityDemo.exe from the visageSDK\bin64 folder.)"></a></td>
     * <td width="32"><a href="../../../../../../bin64"><img src="../../../../../../doc/OpenGL/images/open_bin_folder.png" border=0 title="Open Binary Folder (visageSDK\bin64)"></a></td>
     * <td width="32"><a href="../../../../data/FacialAnimationUnity"><img src="../../../../../../doc/OpenGL/images/open_data_folder.png" border=0 title="Open Data Folder"></a></td>
     * </tr>
     * </table>
     * \endhtmlonly
     * \else
     * \htmlonly
     * <table border="0">
     * <tr>
     * <td width="32"><a href="../../../../../../bin/FacialAnimationUnityDemo.exe"><img src="../../../../../../doc/OpenGL/images/run_sample.png" border=0 title="Run Sample (this may not work in all browsers, in such case please run FacialAnimationUnityDemo.exe from the visageSDK\bin folder.)"></a></td>
     * <td width="32"><a href="../../../../../../bin"><img src="../../../../../../doc/OpenGL/images/open_bin_folder.png" border=0 title="Open Binary Folder (visageSDK\bin)"></a></td>
     * <td width="32"><a href="../../../../data/FacialAnimationUnity"><img src="../../../../../../doc/OpenGL/images/open_data_folder.png" border=0 title="Open Data Folder"></a></td>
     * </tr>
     * </table>
     * \endhtmlonly
     * \endif
     * 
     * The FacialAnimationUnityDemo sample project demonstrates the integration of visage|SDK with Unity game engine 
     * and is aimed at developers starting to use face tracking functionality of visage|SDK to drive facial animation 
     * using blendshapes and bones in the Unity game engine.
     * 
     * 
     * \section using Using the sample application
     *
     * <br/><br/>
     * In order to try the sample application, you need to have a valid license key. More information on how to obtain license key and include it in the application can be found in 
     * <a href="../../../../../../doc/OpenGL/licensing.html"><em>Licensing</em></a> section of the documentation.
     * <br/><br/>
     *
     * To start the application, double-click on FacialAnimationUnity.exe located in \if WIN64_DOXY <i>visageSDK/bin64 </i>\else <i>visageSDK/bin </i>\endif folder.
     *
     * As soon as the application starts, tracking will begin. When a face is found in the camera frame, the character will
     * start moving - mimicking the user's facial expressions. Try opening your mouth, smiling or moving your eyebrows. Available buttons are: Play/Pause in the lower left corner, Switch camera in the lower right corner and Normal/Mask in the upper right corner of the screen.
     * If no license key is present the tracking will not work - please refer to the <a href="../../../../../../doc/OpenGL/licensing.html">licensing section</a> for details.
     * 
     * 
     * \section building Building the project
     * 
     * The files related to FacialAnimationUnity sample project are located in the Samples/OpenGL/data/FacialAnimationUnity subfolder of the visage|SDK folder.
     * 
     * The application was built and tested with version 2018.3.0f2.
     * 
     * To build FacialAnimationUnity sample two steps are involved: creating the Unity project from the provided Unity package and building the application.
     * 
     * \subsection creatingUnity Creating the Unity project from Unity package
     * 
     * To create the Unity project from provided Unity package follow these steps:
     * -# Create a new Unity project
     * -# Import FacialAnimationUnity.unitypackage (provided in Samples/OpenGL/data/FacialAnimationUnity folder) by selecting Assets->Import Package->Custom Package... item from the menu. That way all the assets that are used by the example project will be imported into the new project.
     * -# Select and open the "Visage Tracker/Jones/jones" scene.
     * 
     * \subsection generateApp Building the application
     * 
     * To build the application follow these steps:
     * -# Build the application by selecting File->Build settings... and under Platform choose PC and Mac Standalone and set Windows as target platform. Check if the correct Architecture is set: \if WIN64_DOXY <i>x86_64</i>\else <i>x86</i>\endif.
     * -# Press the Build button to generate the project, a dialog window will appear. In this window, locate and set destination folder to \if WIN64_DOXY <i>visageSDK/bin64</i>\else <i>visageSDK/bin</i>\endif.
     * 
     * \subsection editor Running the project through Unity editor
     *
     * -# Follow steps on creating a Unity project.
     * -# Application is dependent on libraries located in \if WIN64_DOXY <i>visageSDK/bin64 </i>\else <i>visageSDK/bin </i>\endif folder. Copy libraries to the Unity project root folder (e.g. <i>C:/MyUnityProjects/FacialAnimationUnity</i>):
     * \if WIN64_DOXY
     *   - opencv_core2411.dll
     *   - opencv_imgproc2411.dll
     *   - VisageTrackerUnityPlugin64.dll
     *    - libVisageVision64.dll
     *     - libopenblas.dll
     *     - libgfortran-3.dll</i>
     *     - libquadmath-0.dll</i>
     *     - libgcc_s_seh-1.dll</i>
	 *    - libVisageAnalyser64.dll
     * \else
     *   - opencv_core2411.dll
     *   - opencv_imgproc2411.dll
     *   - VisageTrackerUnityPlugin.dll
     *    - libVisageVision.dll
     *     - libopenblas.dll
     *     - libgfortran-3.dll</i>
     *     - libquadmath-0.dll</i>
     *     - libgcc_s_sjlj-1.dll</i>
	 *    - libVisageAnalyser.dll
     * \endif
     * -# Create a <a href="https://docs.unity3d.com/Manual/StreamingAssets.html" target="New">StreamingAssets/Visage Tracker</a> folder in your Unity project Assets folder
     * -# Copy following files and folders from the <i>visageSDK/Samples/data</i> folder to the <i>StreamingAssets/Visage Tracker</i> folder:
     *   - bdtsdata folder
     *   - candide3.wfm
     *   - candide3.fdp
     *   - jk_300.wfm
     *   - jk_300.fdp
     *   - configuration files (Head Tracker.cfg, Facial Features Tracker - High.cfg and Facial Features Tracker - Low.cfg) 
	 *
     * NOTE: In case you have license key file, add it also to the <i>StreamingAssets/Visage Tracker</i> folder. Additionally, set the <i>licenseString</i> member of the LicenseString class (script LicenseString.cs) so it contains name of the license key file.
	 *
     * \section implementation Implementation overview
     * 
     * The project consists of a main scene with the Unity’s GameObjects that provide different functionalities. A GameObject
     * is the main building block of a scene in Unity. It consists of different Components and can parent other GameObjects.
     * Manipulation of GameObjects is done with Scripts. It is recommended for developers to get familiar with Unity basics
     * before continuing. The integration with visage|SDK is done through a plugin (VisageTrackerUnityPlugin) that wraps
     * native code calls to visage|SDK and provides functions that can be called from Unity scripts. The source code for the plugin can be found in the VisageTrackerUnityPlugin project in the samples folder.
     * 
     *
     * \subsection trackerGO Tracker GameObject
     * 
     * The core of the example is the Visage Tracker GameObject. It handles communication with the tracker
     * (starts it and gets results from it). The behaviour of the script can be modified by changing its properties and the script
     * code. Other properties can be added as well. The existing Visage Tracker script properties of interest are as follows:<br/>
     * • Config File Standalone: filename of the tracker configuration file (see the <a href="../../../../../../doc/OpenGL/VisageTracker Configuration Manual.pdf">VisageTracker Configuration manual</a> for information on modifying the tracker configuration) that will be used by the tracker for standalone platforms<br/>
     * • Config File Editor: filename of the tracker configuration file (see the <a href="../../../../../../doc/OpenGL/VisageTracker Configuration Manual.pdf">VisageTracker Configuration manual</a> for information on modifying the tracker configuration) that will be used by the tracker in the editor<br/>
     * • BindingConfigurations: list of binding configurations (see the Binding Configuration section on this page) used in the scene<br/>
     * 
     * 
     * \subsection cameras Cameras
     * 
     * There are two cameras in the scene, one for displaying the character (Main Camera) and other for the background (Background Camera).
     * The main camera "Field of View" is automatically set according to the Visage Tracker focus (camera_focus parameter in
     * configuration file).     
     * 
     *
     * \subsection jonesObject Jones object
     * 
     * Jones is the character with defined blendshape animations which are controlled using the a binding configuration and bones
     * which are controlled using the <i>Visage Tracker/Jones/Scripts/Movement.cs</i> script. The visage|SDK provides relevant information about facial expressions through action units. Their values are then mapped to specific blendshapes using 
     * binding configurations in the <i>VisageTracker.cs</i> script. For more information on creating characters with such blendshapes, consult the <a href="../../../../../../doc/OpenGL/modeling_guide.pdf" target="_blank">Modeling Guide</a>.
     *
     *
     * \subsection bindingConfiguration Binding configuration
     * 
     * Binding configurations map action unit values to specific blendshapes and animate them. A reference binding configuration used to animate Jones is located in <i>Visage Tracker/Jones/jones.bind.txt</i>. <br/>
     * <i>ActionUnitBinding.cs</i> components are created based on the <i>VisageTracker.cs</i> Binding Configuration list in the current scene at runtime. They receive action unit data from the Visage Tracker component and map them to [0,1] values based on the info given in the binding configuration. 
     * You can omit the binding configuration and create the <i>ActionUnitBinding.cs</i> components manually for more control.
     *
     * 
     * \subsection plugin VisageTrackerUnityPlugin
     * The integration of visage|SDK face tracking with Unity is done through a plugin that wraps native code calls and
     * provides functions that can be called from Unity scripts. For more information about the plugin including it's source code, see the VisageTrackerUnityPlugin project and its documentation.
     * 
     */
}