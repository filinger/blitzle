#pragma once

#include "IPlayerDetector.h"
#include "AdditionalTypes.h"

class DotaPlayerDetector : public IPlayerDetector
{
public:
	DotaPlayerDetector();
	virtual ~DotaPlayerDetector();

	void init(int argc, char** argv, bool withDebug = false, bool withControls = false) override;
	void destroy() override;
	void processFrame(const cv::Mat& frameIn, std::vector<cv::Point>& playersOut) override;
	void processFrameDebug(const cv::Mat& frameIn, cv::Mat& drawingOut) override;
	void processFrame(const cv::UMat& frameIn, std::vector<cv::Point>& playersOut) override;;
	void processFrameDebug(const cv::UMat& frameIn, cv::Mat& drawingOut) override;

private:
	static void showPlayers(const std::vector<std::vector<cv::Point>>& contoursIn, std::vector<cv::Point>& playersOut);
	static void dilateRect(int dilateSize, const cv::Mat& filteredIn, cv::Mat& dilatedOut);
	void addControls();

	cv::FilterRange hsvRange = {{6, 255, 240},{8, 255, 255}};
	int dilateSize = 5;
};

