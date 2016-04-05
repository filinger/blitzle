#pragma once

#include <opencv2/core.hpp>

using namespace cv;
using namespace std;

class IPlayerDetector
{
public:
	IPlayerDetector();
	virtual ~IPlayerDetector() = 0;

	virtual void init(int argc, char** argv, bool debugMode) = 0;
	virtual void destroy() = 0;
	virtual void processFrame(const Mat& frameIn, vector<Point>& playersOut) = 0;
	virtual void processFrameDebug(const Mat& frameIn, Mat& drawingOut) = 0;

	static void nopCallback(int, void* context) {
		// Do nothing, we'll process the updates on next processFrame() call.
	};
};
