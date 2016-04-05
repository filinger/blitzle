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

	void init(char** argv);
	void destroy();
	void processFrame(const Mat& frameIn, vector<Point>& playersOut);
	void processFrameDebug(const Mat& frameIn, Mat& drawingOut);

private:
	void findHpBarContours(const Mat& frameIn, vector<vector<Point>>& contoursOut);

	Mat hpBarTemplate;
	int thresholdValue = 9300;
	int thresholdMax = 10000;
	int minContourArea = 50;
	int maxContourArea = 300;
	int contourAreaMax = 500;
	int contourAreaScale = 1;
	int dilateX = 1;
	int dilateY = 4;
	int dilateMax = 20;
};
