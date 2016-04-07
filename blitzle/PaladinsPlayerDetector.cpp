#include "PaladinsPlayerDetector.h"

namespace paladins
{
	const char* control_window = "Paladins Control";
	const char* control_window2 = "Paladins Control 2";
	const char* control_window3 = "Paladins Control 3";
	RNG rng(12345);
}

PaladinsPlayerDetector::PaladinsPlayerDetector()
{
}

PaladinsPlayerDetector::~PaladinsPlayerDetector()
{
}

void PaladinsPlayerDetector::init(int argc, char** argv, bool debugMode)
{
	if (debugMode) 
	{
		addControls();
	}
}

void PaladinsPlayerDetector::destroy()
{
}

void PaladinsPlayerDetector::processFrame(const Mat& frameIn, vector<Point>& playersOut)
{
	Mat filtered;
	applyFilters(frameIn, filtered);

	vector<array<Point2f, 4>> hpBars;
	findHpBars(filtered, hpBars);

	findPlayerPositions(hpBars, playersOut);
}

void PaladinsPlayerDetector::processFrameDebug(const Mat& frameIn, Mat& drawingOut)
{
	Mat filtered;
	applyFilters(frameIn, filtered);

	vector<vector<Point>> contours;
	findContours(filtered, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

	vector<array<Point2f, 4>> hpBars;
	findHpBars(filtered, hpBars);

	frameIn.copyTo(drawingOut);
	for (const auto& bar : hpBars)
	{
		vector<Point2f> barV(bar.begin(), bar.end());
		Scalar color = Scalar(paladins::rng.uniform(0, 255), paladins::rng.uniform(0, 255), paladins::rng.uniform(0, 255));
		rectangle(drawingOut, minAreaRect(barV).boundingRect(), color);
	}
}

void PaladinsPlayerDetector::applyFilters(const Mat& frameIn, Mat& binaryOut)
{
	Mat hsv;
	cvtColor(frameIn, hsv, CV_BGR2HSV);

	Mat lowerHue;
	inRange(hsv, lowerHsvRange.from.asScalar(), lowerHsvRange.to.asScalar(), lowerHue);

	Mat higherHue;
	inRange(hsv, higherHsvRange.from.asScalar(), higherHsvRange.to.asScalar(), higherHue);

	addWeighted(lowerHue, 1.0, higherHue, 1.0, 0.0, binaryOut);
}

void PaladinsPlayerDetector::findHpBars(const Mat& binaryIn, vector<array<Point2f, 4>>& barsOut)
{
	vector<vector<Point>> contours;
	findContours(binaryIn, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
	for (const auto& contour : contours)
	{
		RotatedRect rect = minAreaRect(contour);
		// Note: from horizontal perspective,
		// rect has swapped width and height,
		// because it detects it as vertical rectangle with angle == -90 degree. o_O
		bool correctWidth = rect.size.width > 1 && rect.size.width < 5;
		bool correctHeight = rect.size.height > 4 && rect.size.height < 100;
		bool correctAngle = rect.angle > -91 && rect.angle < -89;
		if (correctAngle && correctHeight && correctWidth)
		{
			array<Point2f, 4> points;
			rect.points(points.data());
			barsOut.push_back(points);
		}
	}
}

void PaladinsPlayerDetector::findPlayerPositions(const vector<array<Point2f, 4>>& barsIn, vector<Point>& positionsOut)
{
	for (const auto& bar : barsIn)
	{
		positionsOut.push_back(Point(int(bar[1].x) + 35, int(bar[1].y) + 60));
	}
}

void PaladinsPlayerDetector::addControls()
{
}
