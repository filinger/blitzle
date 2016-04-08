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
	Mat roi;
	Rect roiRect;
	getRoi(frameIn, roi, roiRect);

	Mat filtered;
	applyFilters(roi, filtered);

	vector<Rect> hpBars;
	findHpBars(filtered, hpBars);

	vector<Point> players;
	findPlayerPositions(hpBars, players);
	const Point& roiOffset = Point(roiRect.x, roiRect.y);
	for (const auto& player : players) {
		playersOut.push_back(player + roiOffset);
	}
}

void PaladinsPlayerDetector::processFrameDebug(const Mat& frameIn, Mat& drawingOut)
{
	Mat roi;
	Rect roiRect;
	getRoi(frameIn, roi, roiRect);

	Mat filtered;
	applyFilters(roi, filtered);

	vector<Rect> hpBars;
	findHpBars(filtered, hpBars);

	frameIn.copyTo(drawingOut);
	rectangle(drawingOut, roiRect, Scalar(0, 255, 0), 2);
	for (const auto& bar : hpBars)
	{
		Rect correctedBar = Rect(bar.x + roiRect.x, bar.y + roiRect.y, bar.width, bar.height);
		rectangle(drawingOut, correctedBar, Scalar(255, 0, 255), 2);
	}
}

void PaladinsPlayerDetector::getRoi(const Mat& frameIn, Mat& roiOut, Rect& roiRectOut)
{
	const int width = frameIn.cols;
	const int height = frameIn.rows;
	const int roiStartX = int(width * (roiRectNorm.x - roiRectNorm.width / 2.0f));
	const int roiStartY = int(height * (roiRectNorm.y - roiRectNorm.height / 2.0f));
	const int roiWidth = int(width * roiRectNorm.width);
	const int roiHeight = int(height * roiRectNorm.height);
	roiRectOut = Rect(roiStartX, roiStartY, roiWidth, roiHeight);
	roiOut = Mat(frameIn, roiRectOut);
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

void PaladinsPlayerDetector::findHpBars(const Mat& binaryIn, vector<Rect>& barsOut)
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
		bool correctHeight = rect.size.height > 15 && rect.size.height < 100;
		bool correctAngle = rect.angle > -91 && rect.angle < -89;
		if (correctAngle && correctHeight && correctWidth)
		{
			array<Point2f, 4> points;
			rect.points(points.data());
			barsOut.push_back(Rect(points[1], points[3]));
		}
	}
}

void PaladinsPlayerDetector::findPlayerPositions(const vector<Rect>& barsIn, vector<Point>& positionsOut)
{
	for (const auto& bar : barsIn)
	{
		positionsOut.push_back(Point(bar.x + 35, bar.y + 60));
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
