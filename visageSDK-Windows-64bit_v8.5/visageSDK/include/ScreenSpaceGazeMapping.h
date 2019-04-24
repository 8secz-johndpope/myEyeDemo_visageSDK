#ifndef SCREENSPACEGAZEMAPPING_H
#define SCREENSPACEGAZEMAPPING_H

#include "FaceData.h"
#include "InputGazeData.h"
#include "TrackerGazeCalibrator.h"

namespace VisageSDK
{
	struct EyeData
	{
	public:

		EyeData();
		EyeData(const FaceData& faceData, VsImage* frame_input);
		EyeData(const EyeData& data);
		~EyeData();

		EyeData& operator= (const EyeData& data); 

		void resize(int rw, int rh, int lw, int lh);

		VsImage* leftEye;
		VsImage* rightEye;	

	private:
		VsImage* crop(const VsRect& rectangle, VsImage* frame);
		void filter(VsImage* image);
	};

	struct TrainingGazeData : public ScreenSpaceGazeData
	{
	public:

		TrainingGazeData();

		void addCalibData(float x, float y, int type, int frameIndex);
		void addEyeData(EyeData eyeData);
		void addInputData(InputGazeData data);


		EyeData eyeData;
		InputGazeData data;
		int trackerStatus;
	};


	struct GaussianProcessParams
	{
		float sigma;
		float lscale;
		float noise;
	};

	class CovarianceFunction
	{
		public:
			GaussianProcessParams params;
			double run(VsMat* x, VsMat* y);
	};

	class GaussianProcessMapping
	{
		public:

			GaussianProcessMapping() {};

			GaussianProcessMapping(const GaussianProcessMapping& mapping);
			
			~GaussianProcessMapping();

			GaussianProcessMapping& operator=(const GaussianProcessMapping& mapping);

			GaussianProcessMapping(std::vector<VsMat*> samples, const CovarianceFunction& function);

			VsMat* GetMatrix(std::vector<VsMat*> input);

			CovarianceFunction GetCovarianceFunction();
		
		private:
			CovarianceFunction function;
			std::vector<VsMat*> samples;
	};
}
#endif