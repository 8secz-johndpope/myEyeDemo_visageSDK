using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;


/// <summary>
/// Provides high-level functions for using visage|SDK face analysis capabilities in Unity game engine. 
/// It implements high-level wrapper functions from visage|SDK that enable usage of visage|SDK face analysis capabilities in Unity C# scripts.
/// 
/// VisageFaceAnalyser contains face analysis algorithms for estimating age, gender and emotion from frontal facial images(yaw between -20 and 20 degrees).
/// 
/// The functionality of VisageFaceAnalyser is dependent on the VisageTracker and it is expected that prior to the call of VisageFaceAnalyser's functions frame is processed with _track() function 
/// and the data thus obtained is prepared and forwarded to VisageFaceAnalyser.
/// 
/// Before using VisageFaceAnalyser, it must be initialized by calling the _initAnalyser() function.
/// Afterward, in each frame, the _prepareDataForAnalysis() function is called first and then the functions for estimating the age, gender and emotions,
/// otherwise -1 will be returned as an indication that the estimation failed.
/// 
/// VisageFaceAnalyser's functionality can be used in a separate thread.
///
/// </summary>
/// 

/**
 * \defgroup VisageAnalyserUnityPlugin VisageAnalyserUnityPlugin
 * @{
 */

/** Initialises VisageFaceAnalyser.
*This function must be called before using VisageFaceAnalyser by passing it a path to the folder containing the VisageFaceAnalyser data files. Within the Visage|SDK package, this folder is Samples/data/bdtsdata/LBF/vfadata. The VisageFaceAnalyser data folder contains the following subfolders corresponding to the various types of analysis that can be performed:
*	
<ul>
    <li>ad: age estimation</li>
    <li>gd: gender estimation</li>
    <li>ed: emotion estimation</li>
</ul>
*
* An example of use:
* \code
* 
* int analysisDataLoaded = 0;
* int analyserInited = VisageTrackerNative._initAnalyser(dataPath);
* 
* if ((analyserInited & (int)VFA_FLAGS.VFA_AGE) == (int)VFA_FLAGS.VFA_AGE)
* {
*    analysisDataLoaded |= (int)VFA_FLAGS.VFA_AGE;
* }
*
* if ((analyserInited & (int)VFA_FLAGS.VFA_EMOTION) == (int)VFA_FLAGS.VFA_EMOTION)
* {
*    analysisDataLoaded |= (int)VFA_FLAGS.VFA_EMOTION;
* }
*  
* if ((analyserInited & (int)VFA_FLAGS.VFA_GENDER) == (int)VFA_FLAGS.VFA_GENDER)
* {
*    analysisDataLoaded |= (int)VFA_FLAGS.VFA_GENDER;
* }
*
* for (int faceIndex = 0; faceIndex < MAX_FACES; faceIndex++)
* {
*    if ((analysisDataLoaded & (int)VFA_FLAGS.VFA_AGE) == (int)VFA_FLAGS.VFA_AGE)
*        age[faceIndex] = VisageTrackerNative._estimateAge(faceIndex);
* }
* \endcode
* 
*Note that it is not necessary for all subfolders to be present, only the ones needed by the types of analysis that will be performed. For example, if only gender estimation will be performed, then only the gd folder is required.
*The return value of the function indicates which types of analysis have been successfully initialised. Subsequent attempt to use a type of analysis that was not initialized shall fail; for example, if only gd folder was present at initialization, attempt to use _estimateEmotion() shall fail.
*The return value is a bitwise combination of flags indicating which types of analysis are successfully initialized.
*
*Please note that in order for license to be registered, _initializeLicense() method has to be called before.
* 
* @param dataPath Relative path from the working directory to the directory that contains VisageFaceAnalyser data files (e.g. Application.streamingAssetsPath + /Visage Tracker/bdtsdata/LBF/vfadata").
* @return The return value of 0 indicates that no data files are found and no analysis will be possible. In such case, the placement of data files should be verified. A non-zero value indicates that one or more types of analysis are successfully initialized. The specific types of analysis that have been initialized are indicated by a bitwise combination of the following flags: VFA_AGE, VFA_GENDER, VFA_EMOTION. 
*/
int _initAnalyser(string dataPath);

/** Releases memory allocated by the face analyser in the _initAnalyser() function.
 * 
 * This function is called after the analyser ceases to be used or before the reinitialization of the analyser.
 */
void _releaseAnalyser();

/** Estimates emotion from a facial image. In case of multiple trackers running faceIndex specifies tracker ID.
*
* The function returns estimated probabilities for basic emotions. Prior to using this function, it is necessary to process the video frame using _track() function and prepare obtained data with _prepareDataForAnalysis() function.
*
* An example of use:
* \code
*
* public float[,] Emotions = new float[MAX_FACES, NUM_EMOTIONS];
* public float[] emotions = new float[7];
*
* for(int i = 0; i < MAX_FACES; ++i)
* {
*    int success = VisageTrackerNative._estimateEmotion(emotions, i);
*
*    if(success)
*    {
*        for (int j = 0; j < emotions.Length; j++)
*        {
*            Emotions[i, j] = emotions[i];
*        }
*    }
* }  
* \endcode
* 
* @param emotions An array of 7 floats. If successful, the function will fill this array with estimated probabilities for emotions in this order: anger, disgust,
* fear, happiness, sadness, surprise and neutral. Each probability will have a value between 0 and 1. Sum of probabilities does not have to be 1.
* @param faceIndex Value between 0 and MAX_FACES-1, used to access the data of a particular tracked face.
* @return 1 if estimation was successful or -1 if it failed.
*
*/
int _estimateEmotion(float[] emotions, int faceIndex);

/** Estimates age from a facial image. In case of multiple trackers running faceIndex specifies tracker ID.
*
* The function returns estimated age. Prior to using this function, it is necessary to process the video frame using _track() function and prepare obtained data with _prepareDataForAnalysis() function.
*
* @param faceIndex Value between 0 and MAX_FACES-1, used to access the data of a particular tracked face.
* @return Estimated age or -1 if it failed.
*/
float _estimateAge(int faceIndex);

/** Estimates gender from a facial image. In case of multiple trackers running faceIndex specifies tracker ID.
*
* The function returns 1 if estimated gender is male and 0 if it is a female. Prior to using this function, it is necessary to process the video frame using _track() function and prepare obtained data with _prepareDataForAnalysis() function.
*
* @param faceIndex Value between 0 and MAX_FACES-1, used to access the data of a particular tracked face.
* @return 0 if estimated gender is female, 1 if it is a male and -1 if it failed.
*/
 int _estimateGender(int faceIndex);

/** Prepares data obtained by the track function.
*/
 bool _prepareDataForAnalysis();

/** @} */

