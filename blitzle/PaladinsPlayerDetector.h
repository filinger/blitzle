#pragma once

#include <array>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "ComHeaders.h"
#include "IPlayerDetector.h"
#include "AdditionalTypes.h"

using namespace cv;
using namespace std;

class PaladinsPlayerDetector : public IPlayerDetector
{
public:
	PaladinsPlayerDetector();
	virtual ~PaladinsPlayerDetector();

	void init(int argc, char** argv, bool withDebug=false, bool withControls=false);
	void destroy();
	void processFrame(const Mat& frameIn, vector<Point>& playersOut);
	void processFrameDebug(const Mat& frameIn, Mat& drawingOut);

private:
	void applyFilters(const Mat& frameIn, Mat& binaryOut);
	void findHpBars(const Mat& binaryIn, vector<array<Point2f, 4>>& barsOut);
	void findPlayerPositions(const vector<array<Point2f, 4>>& barsIn, vector<Point>& positionsOut);
	void addControls();

	HsvRange lowerHsvRange = { { 0, 130, 130 }, { 10, 255, 255 } };
	HsvRange higherHsvRange = { { 176, 130, 130 }, { 180, 255, 255 } };
};
