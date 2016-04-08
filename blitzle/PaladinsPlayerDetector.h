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
	void getRoi(const Mat& frameIn, Mat& roiOut, Rect& roiRectOut);
	void applyFilters(const Mat& frameIn, Mat& binaryOut);
	void findHpBars(const Mat& binaryIn, vector<Rect>& barsOut);
	void findPlayerPositions(const vector<Rect>& barsIn, vector<Point>& positionsOut);
	void addControls();

	HsvRange lowerHsvRange = { { 0, 130, 130 }, { 10, 255, 255 } };
	HsvRange higherHsvRange = { { 176, 130, 130 }, { 180, 255, 255 } };
	Rect_<float> roiRectNorm = Rect_<float>(0.5f, 0.5f, 0.2f, 0.3f);
};
