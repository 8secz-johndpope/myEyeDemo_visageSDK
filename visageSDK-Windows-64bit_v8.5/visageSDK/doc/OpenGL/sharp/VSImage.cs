using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace VisageCSWrapper
{
	/// <summary>
	/// Image storage class.
	/// </summary>
	public struct VSImage
	{

		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="width">Image width in pixels.</param>
		/// <param name="height">Image height in pixels.</param>
		/// <param name="depth">Pixel depth in bits.</param>
		/// <param name="nChannels">Number of channels.</param>
		/// <param name="dataOrder">0 - interleaved color channels, 1 - separate color channels.</param>
		/// <param name="origin"> 0 - top-left origin, 1 - bottom-left origin.</param>
		VSImage(int width, int height, int depth, int nChannels, int dataOrder = 0, int origin = 0);

		/// <summary>
		/// Image width in pixels.
		/// </summary>
		public int width;


		/// <summary>
		/// Image height in pixels.
		/// </summary>
		public int height;


		/// <summary>
		/// Pixel depth in bits: VS_DEPTH_8U, VS_DEPTH_8S, VS_DEPTH_16S, VS_DEPTH_32S, VS_DEPTH_32F and VS_DEPTH_64F are supported.
		/// </summary>
		public int depth;


		/// <summary>
		/// Size of aligned image row in bytes.
		/// </summary>
		public int widthStep;


		/// <summary>
		/// 0 - interleaved color channels, 1 - separate color channels.
		/// </summary>
		public int dataOrder;


		/// <summary>
		/// Most functions support 1,2,3 or 4 channels
		/// </summary>
		public int nChannels;


		/// <summary>
		/// 0 - top-left origin, 1 - bottom-left origin.
		/// </summary>
		public int origin;


		/// <summary>
		/// Pointer to aligned image data.
		/// </summary>
		public byte[] imageData;
	}
}
