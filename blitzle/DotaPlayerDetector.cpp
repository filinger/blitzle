#include "DotaPlayerDetector.h"

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

namespace dota
{
	const char* controlsWindow = "Filtering Controls";
}

DotaPlayerDetector::DotaPlayerDetector()
{
}

DotaPlayerDetector::~DotaPlayerDetector()
{
}

void DotaPlayerDetector::init(int argc, char** argv, bool withDebug, bool withControls)
{
	if (withControls)
	{
		addControls();
	}
}

void DotaPlayerDetector::destroy()
{
}

void DotaPlayerDetector::processFrame(const cv::Mat& frameIn, std::vector<cv::Point>& playersOut)
{
	cv::Mat hsv;
	cv::cvtColor(frameIn, hsv, cv::COLOR_BGR2HSV);

	cv::Mat filtered;
	inRange(hsv, hsvRange.from.asScalar(), hsvRange.to.asScalar(), filtered);

	cv::Mat dilated;
	dilateRect(dilateSize, filtered, dilated);

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(dilated, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

	showPlayers(contours, playersOut);
}

void DotaPlayerDetector::processFrameDebug(const cv::Mat& frameIn, cv::Mat& drawingOut)
{
	// We could've actually draw some contours for debugging here....
	frameIn.copyTo(drawingOut);
}

void DotaPlayerDetector::processFrame(const cv::UMat& frameIn, std::vector<cv::Point>& playersOut)
{
	// Does not implement GPU processing.
}

void DotaPlayerDetector::processFrameDebug(const cv::UMat& frameIn, cv::Mat& drawingOut)
{
	// Does not implement GPU processing.	
}

void DotaPlayerDetector::showPlayers(const std::vector<std::vector<cv::Point>>& contoursIn, std::vector<cv::Point>& playersOut)
{
	const cv::Point offset = cv::Point(50, 80);
	for (const auto& contour : contoursIn)
	{
		playersOut.push_back(contour[0] + offset);
	}
}

void DotaPlayerDetector::dilateRect(int dilateSize, const cv::Mat& filteredIn, cv::Mat& dilatedOut)
{
	int kernelSize = dilateSize == 0 ? 1 : dilateSize;
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kernelSize, 1));
	cv::dilate(filteredIn, dilatedOut, kernel);
}

void DotaPlayerDetector::addControls()
{
	const int hMax = 180;
	const int svMax = 255;
	const int dilateMax = 100;

	namedWindow(dota::controlsWindow, cv::WINDOW_AUTOSIZE);
	cv::createTrackbar("H1", dota::controlsWindow, &hsvRange.from.h, hMax, nopCallback, this);
	cv::createTrackbar("S1", dota::controlsWindow, &hsvRange.from.s, svMax, nopCallback, this);
	cv::createTrackbar("V1", dota::controlsWindow, &hsvRange.from.v, svMax, nopCallback, this);
	cv::createTrackbar("H2", dota::controlsWindow, &hsvRange.to.h, hMax, nopCallback, this);
	cv::createTrackbar("S2", dota::controlsWindow, &hsvRange.to.s, svMax, nopCallback, this);
	cv::createTrackbar("V2", dota::controlsWindow, &hsvRange.to.v, svMax, nopCallback, this);
	cv::createTrackbar("Dilate Size", dota::controlsWindow, &dilateSize, dilateMax, nopCallback, this);
}

