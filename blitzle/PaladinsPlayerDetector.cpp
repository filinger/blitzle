#include "PaladinsPlayerDetector.h"

namespace paladins
{
	const char* lowerHsvControls = "Lower HSV Controls";
	const char* higherHsvControls = "Higher HSV Controls";
	RNG rng(12345);
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

	filtered.copyTo(drawingOut);
	cvtColor(filtered, drawingOut, CV_GRAY2BGR);

	vector<vector<Point>> contours;
	findContours(filtered, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

	vector<array<Point2f, 4>> hpBars;
	findHpBars(filtered, hpBars);

	for (const auto& bar : hpBars)
	{
		vector<Point2f> barV(bar.begin(), bar.end());
		rectangle(drawingOut, minAreaRect(barV).boundingRect(), Scalar(255, 0, 255), 2);
	}
}

void PaladinsPlayerDetector::applyFilters(const Mat& frameIn, Mat& binaryOut)
{
	Mat hsv;
	//cv::medianBlur(frameIn, hsv, 3);
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
		// the rect has swapped width and height,
		// because it detects it as a vertical rectangle with angle == -90 degree. o_O
		bool correctWidth = rect.size.width > 1 && rect.size.width < 6;
		bool correctHeight = rect.size.height > 8 && rect.size.height < 100;
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
	const int hMax = 180;
	const int svMax = 255;

	namedWindow(paladins::lowerHsvControls, WINDOW_AUTOSIZE);
	createTrackbar("H1", paladins::lowerHsvControls, &lowerHsvRange.from.h, hMax, nopCallback, this);
	createTrackbar("S1", paladins::lowerHsvControls, &lowerHsvRange.from.s, svMax, nopCallback, this);
	createTrackbar("V1", paladins::lowerHsvControls, &lowerHsvRange.from.v, svMax, nopCallback, this);
	createTrackbar("H2", paladins::lowerHsvControls, &lowerHsvRange.to.h, hMax, nopCallback, this);
	createTrackbar("S2", paladins::lowerHsvControls, &lowerHsvRange.to.s, svMax, nopCallback, this);
	createTrackbar("V2", paladins::lowerHsvControls, &lowerHsvRange.to.v, svMax, nopCallback, this);

	namedWindow(paladins::higherHsvControls, WINDOW_AUTOSIZE);
	createTrackbar("H1", paladins::higherHsvControls, &higherHsvRange.from.h, hMax, nopCallback, this);
	createTrackbar("S1", paladins::higherHsvControls, &higherHsvRange.from.s, svMax, nopCallback, this);
	createTrackbar("V1", paladins::higherHsvControls, &higherHsvRange.from.v, svMax, nopCallback, this);
	createTrackbar("H2", paladins::higherHsvControls, &higherHsvRange.to.h, hMax, nopCallback, this);
	createTrackbar("S2", paladins::higherHsvControls, &higherHsvRange.to.s, svMax, nopCallback, this);
	createTrackbar("V2", paladins::higherHsvControls, &higherHsvRange.to.v, svMax, nopCallback, this);
}
