#include "PaladinsPlayerDetector.h"

#include <array>
#include <vector>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

namespace paladins
{
	const char* lowerHsvControls = "Lower HSV Controls";
	const char* higherHsvControls = "Higher HSV Controls";
	cv::RNG rng(12345);
}

PaladinsPlayerDetector::PaladinsPlayerDetector()
{
}

PaladinsPlayerDetector::~PaladinsPlayerDetector()
{
}

void PaladinsPlayerDetector::init(int argc, char** argv, bool withDebug, bool withControls)
{
	if (withControls)
	{
		addControls();
	}
}

void PaladinsPlayerDetector::destroy()
{
}

void PaladinsPlayerDetector::processFrame(const cv::Mat& frameIn, std::vector<cv::Point>& playersOut)
{
	cv::UMat gpuFrameIn;
	frameIn.copyTo(gpuFrameIn);
	processFrame(gpuFrameIn, playersOut);
}

void PaladinsPlayerDetector::processFrameDebug(const cv::Mat& frameIn, cv::Mat& drawingOut)
{
	cv::UMat gpuFrameIn;
	frameIn.copyTo(gpuFrameIn);
	processFrameDebug(gpuFrameIn, drawingOut);

	//cvtColor(frameIn, drawingOut, CV_BGR2Lab);
	//inRange(drawingOut, hpBarLabRange.from.asScalar(), hpBarLabRange.to.asScalar(), drawingOut);
}

void PaladinsPlayerDetector::processFrame(const cv::UMat& frameIn, std::vector<cv::Point>& playersOut)
{
	cv::UMat roi;
	cv::Rect roiRect;
	getRoi(frameIn, roi, roiRect);

	cv::UMat filtered;
	filterHpColors(roi, filtered);

	std::vector<cv::Rect> hpBars;
	findHpBars(filtered, hpBars);

	std::vector<cv::Point> players;
	findPlayerPositions(hpBars, players);
	const cv::Point& roiOffset = cv::Point(roiRect.x, roiRect.y);
	for (const auto& player : players)
	{
		playersOut.push_back(player + roiOffset);
	}
}

void PaladinsPlayerDetector::processFrameDebug(const cv::UMat& frameIn, cv::Mat& drawingOut)
{
	cv::UMat filtered;
	inRange(frameIn, cv::Scalar(200, 200, 200), cv::Scalar(255, 255, 255), filtered);

	cv::UMat dilated;
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(12, 12));
	dilate(filtered, dilated, kernel);
	cv::erode(dilated, dilated, kernel);

	const cv::Point2f hpRectOffset = cv::Point(0, 3);
	const cv::Size hpRectSize = cv::Size(20, 4);

	std::vector<std::vector<cv::Point>> contours;
	findContours(dilated, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
	for (int i = 0; i < contours.size(); ++i)
	{
		cv::RotatedRect areaRect = cv::minAreaRect(contours[i]);
		cv::Size rectSize = areaRect.size;
		if (areaRect.angle < -45.0f)
		{
			cv::swap(rectSize.width, rectSize.height);
		}

		cv::Rect contourRect = cv::Rect(areaRect.center - cv::Point2f(rectSize.width / 2, rectSize.height / 2), rectSize);
		cv::Rect hpRect = cv::Rect(cv::Point2f(contourRect.x, contourRect.y + contourRect.height) + hpRectOffset, hpRectSize);
		rectangle(drawingOut, hpRect, cv::Scalar(0, 255, 0), 1);
		rectangle(drawingOut, contourRect, cv::Scalar(255, 0, 0), 2);
		drawContours(drawingOut, contours, i, cv::Scalar(255, 0, 255), 2);
	}
}

void PaladinsPlayerDetector::getRoi(const cv::UMat& frameIn, cv::UMat& roiOut, cv::Rect& roiRectOut) const
{
	const int width = frameIn.cols;
	const int height = frameIn.rows;
	const int roiStartX = int(width * (roiRectNorm.x - roiRectNorm.width / 2.0f));
	const int roiStartY = int(height * (roiRectNorm.y - roiRectNorm.height / 2.0f));
	const int roiWidth = int(width * roiRectNorm.width);
	const int roiHeight = int(height * roiRectNorm.height);
	roiRectOut = cv::Rect(roiStartX, roiStartY, roiWidth, roiHeight);
	roiOut = cv::UMat(frameIn, roiRectOut);
}

void PaladinsPlayerDetector::filterHpColors(const cv::UMat& frameIn, cv::UMat& binaryOut) const
{
	cv::UMat hsv;
	cv::cvtColor(frameIn, hsv, CV_BGR2HSV);

	cv::UMat lowerHue;
	inRange(hsv, lowerHsvRange.from.asScalar(), lowerHsvRange.to.asScalar(), lowerHue);

	cv::UMat higherHue;
	inRange(hsv, higherHsvRange.from.asScalar(), higherHsvRange.to.asScalar(), higherHue);

	cv::addWeighted(lowerHue, 1.0, higherHue, 1.0, 0.0, binaryOut);
}

void PaladinsPlayerDetector::findHpBars(const cv::UMat& binaryIn, std::vector<cv::Rect>& barsOut) const
{
	std::vector<std::vector<cv::Point>> contours;
	findContours(binaryIn, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
	for (const auto& contour : contours)
	{
		cv::RotatedRect rect = minAreaRect(contour);
		// Note: from horizontal perspective,
		// the rect has swapped width and height,
		// because it detects it as a vertical rectangle with angle == -90 degree. o_O
		bool correctWidth = rect.size.width > 1 && rect.size.width < 6;
		bool correctHeight = rect.size.height > 15 && rect.size.height < 100;
		bool correctAngle = rect.angle > -91 && rect.angle < -89;
		if (correctAngle && correctHeight && correctWidth)
		{
			std::array<cv::Point2f, 4> points;
			rect.points(points.data());
			barsOut.push_back(cv::Rect(points[1], points[3]));
		}
	}
}

void PaladinsPlayerDetector::findPlayerPositions(const std::vector<cv::Rect>& barsIn, std::vector<cv::Point>& positionsOut)
{
	for (const auto& bar : barsIn)
	{
		positionsOut.push_back(cv::Point(bar.x + 35, bar.y + 60));
	}
}

void PaladinsPlayerDetector::addControls()
{
	const int hMax = 180;
	const int svMax = 255;

	cv::namedWindow(paladins::lowerHsvControls, cv::WINDOW_AUTOSIZE);
	cv::createTrackbar("H1", paladins::lowerHsvControls, &hpBarLabRange.from.h, svMax, nopCallback, this);
	cv::createTrackbar("S1", paladins::lowerHsvControls, &hpBarLabRange.from.s, svMax, nopCallback, this);
	cv::createTrackbar("V1", paladins::lowerHsvControls, &hpBarLabRange.from.v, svMax, nopCallback, this);
	cv::createTrackbar("H2", paladins::lowerHsvControls, &hpBarLabRange.to.h, svMax, nopCallback, this);
	cv::createTrackbar("S2", paladins::lowerHsvControls, &hpBarLabRange.to.s, svMax, nopCallback, this);
	cv::createTrackbar("V2", paladins::lowerHsvControls, &hpBarLabRange.to.v, svMax, nopCallback, this);
}

