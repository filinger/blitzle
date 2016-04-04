#pragma once

#include "IPlayerDetector.h"
#include "AdditionalTypes.h"

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;

class PaladinsPlayerDetector : public IPlayerDetector
{
public:
	PaladinsPlayerDetector();
	virtual ~PaladinsPlayerDetector();

	void init();
	void destroy();
	void processFrame(const Mat& frameIn, vector<Point>& playersOut);
};
