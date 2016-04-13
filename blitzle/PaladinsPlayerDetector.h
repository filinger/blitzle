#pragma once

#include <opencv2/core.hpp>

#include "IPlayerDetector.h"
#include "AdditionalTypes.h"

class PaladinsPlayerDetector : public IPlayerDetector
{
public:
	PaladinsPlayerDetector();
	virtual ~PaladinsPlayerDetector();

	void init(int argc, char** argv, bool withDebug = false, bool withControls = false) override;
	void destroy() override;
	void processFrame(const cv::Mat& frameIn, std::vector<cv::Point>& playersOut) override;
	void processFrameDebug(const cv::Mat& frameIn, cv::Mat& drawingOut) override;
	void processFrame(const cv::UMat& frameIn, std::vector<cv::Point>& playersOut) override;
	void processFrameDebug(const cv::UMat& frameIn, cv::Mat& drawingOut) override;

private:
	void getRoi(const cv::UMat& frameIn, cv::UMat& roiOut, cv::Rect& roiRectOut) const;
	void filterHpColors(const cv::UMat& frameIn, cv::UMat& binaryOut) const;
	void findHpBars(const cv::UMat& binaryIn, std::vector<cv::Rect>& barsOut) const;
	static void findPlayerPositions(const std::vector<cv::Rect>& barsIn, std::vector<cv::Point>& positionsOut);
	void addControls();

	cv::FilterRange hpBarLabRange = {{200, 120, 120},{225, 140, 140}};
	cv::FilterRange lowerHsvRange = {{0, 130, 130},{10, 255, 255}};
	cv::FilterRange higherHsvRange = {{176, 130, 130},{180, 255, 255}};
	cv::Rect_<float> roiRectNorm = cv::Rect_<float>(0.5f, 0.5f, 0.2f, 0.3f);
};

