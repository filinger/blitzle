#pragma once

#include <opencv2/core.hpp>

using namespace cv;

class IPlayerDetector
{
public:
	IPlayerDetector();
	virtual ~IPlayerDetector() = 0;

	virtual void init() = 0;
	virtual void destroy() = 0;
	virtual void processFrame(Mat& frame, Mat& output) = 0;
};
