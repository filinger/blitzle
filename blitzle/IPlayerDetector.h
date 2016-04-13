#pragma once

#include <opencv2/core.hpp>

class IPlayerDetector
{
public:
	IPlayerDetector();
	virtual ~IPlayerDetector() = 0;

	virtual void init(int argc, char** argv, bool withDebug = false, bool withControls = false) = 0;
	virtual void destroy() = 0;
	virtual void processFrame(const cv::Mat& frameIn, std::vector<cv::Point>& playersOut) = 0;
	virtual void processFrameDebug(const cv::Mat& frameIn, cv::Mat& drawingOut) = 0;
	virtual void processFrame(const cv::UMat& frameIn, std::vector<cv::Point>& playersOut) = 0;
	virtual void processFrameDebug(const cv::UMat& frameIn, cv::Mat& drawingOut) = 0;

	static void nopCallback(int, void* context) {
		// Do nothing, we'll process the updates on next processFrame() call.
	};
};
