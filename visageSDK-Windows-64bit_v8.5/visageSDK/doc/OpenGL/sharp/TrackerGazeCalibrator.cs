using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace VisageCSWrapper
{
	/// <summary>
	/// Screen Space Gaze Data is a container structure used to hold the gaze point location, current state of screen space gaze tracking system and gaze tracking estimation quality parameters for the current frame.
	/// This class is used both to store the calibration points during the calibration phase and the estimation results during the estimation phase.
	/// </summary>
	public struct ScreenSpaceGazeData 
	{
		/// <summary>
		/// Index of the video frame. 
		/// In online mode the frame index corresponds to the sum of the number of calibration frames passed to the tracker and the number of frames processed in the estimation phase. 
		///
		/// In offline mode the index is the frame number in the processed video sequence.
		/// </summary>
		public int index;


		/// <summary>
		/// X coordinate of screen space gaze normalized to screen width. 
		/// 0 corresponds to the left-most point and 1 to the right-most points of the screen. Defaults to 0.5.
		/// </summary>
		public float x;


		/// <summary>
		/// Y coordinate of screen space gaze normalized to screen height.
		/// 0 corresponds to the top-most point and 1 to the bottom-most points of the screen. Defaults to 0.5.
		/// </summary>
		public float y;


		/// <summary>
		/// Flag indicating the state of the gaze estimator for the current frame.
		/// 
		/// If inState is 0 the estimator is off and position is default. 
		///
		/// If inState is 1 the estimator is calibrating and returns calibration data for the current frame. 
		/// 
		/// If inState is 2 the estimator is estimating and returns the estimated screen space gaze coordinates.  
		///
		/// If inState is -1 the estimator is calibrating but the tracking/screen space gaze estimation failed for the current frame (position is default)
		///
		/// If inState is -2 the estimator is estimating but the tracking/screen space gaze estimation failed for the current frame (position is default)
		/// </summary>
		public int inState;


		/// <summary>
		/// The frame level gaze tracking quality.
		/// Quality is returned as a value from 0 to 1, where 0 is the worst and 1 is the best quality. The value is 0 also when the gaze tracking is off or calibrating.
		/// </summary>
		public float quality;
	};
}
