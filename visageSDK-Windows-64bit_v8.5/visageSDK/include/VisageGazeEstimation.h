#ifndef VISAGEGAZEESTIMATION_H
#define VISAGEGAZEESTIMATION_H

#include "ScreenSpaceGazeMapping.h"

namespace VisageSDK
{

class GazeEstimationPredictor
{
	public:
		virtual float Predict(std::vector<VsMat*> sample) = 0;
		virtual ~GazeEstimationPredictor() {};
};

class GazeEstimationCalibrator
{
	public:
		virtual GazeEstimationPredictor* Calibrate(std::vector<VsMat*> samples, const VsMat* observations, GaussianProcessMapping& mapping) = 0;
		virtual ~GazeEstimationCalibrator() {};
};

class GaussianProcessPredictor : public GazeEstimationPredictor
{
	public:

		GaussianProcessPredictor(const GaussianProcessMapping& mapping, VsMat* coeficient);

		GaussianProcessPredictor(const GaussianProcessPredictor& predictor);

		GaussianProcessPredictor& operator=(const GaussianProcessPredictor& predictor);

		virtual ~GaussianProcessPredictor();

		float Predict(std::vector<VsMat*> sample);
	
		private:
			GaussianProcessMapping mapping;
			VsMat* coeficient;
};

class GaussianProcessCalibrator : public GazeEstimationCalibrator
{
	public:
		GazeEstimationPredictor* Calibrate(std::vector<VsMat*> samples, const VsMat* observations, GaussianProcessMapping& mapping);
};

class GeometricPredictor : GazeEstimationPredictor
{

};

class GeometricCalibrator : GazeEstimationCalibrator
{

};


}
#endif