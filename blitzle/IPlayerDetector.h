#pragma once

#include <opencv2/core.hpp>

using namespace cv;
using namespace std;

class IPlayerDetector
{
public:
	IPlayerDetector();
	virtual ~IPlayerDetector() = 0;

	virtual void init() = 0;
	virtual void destroy() = 0;
	virtual void processFrame(const Mat& frameIn, vector<Point>& playersOut) = 0;
};
