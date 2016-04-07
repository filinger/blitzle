#pragma once

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "IPlayerDetector.h"
#include "AdditionalTypes.h"


using namespace cv;
using namespace std;

class DotaPlayerDetector : public IPlayerDetector
{
public:
	DotaPlayerDetector();
	virtual ~DotaPlayerDetector();

	void init(int argc, char** argv, bool debugMode);
	void destroy();
	void processFrame(const Mat& frameIn, vector<Point>& playersOut);
	void processFrameDebug(const Mat& frameIn, Mat& drawingOut);

private:
	HsvRange hsvRange;
	int dilateSize;

	void showPlayers(const vector<vector<Point>>& contoursIn, vector<Point>& playersOut);
	void dilateRect(int dilateSize, const Mat& filteredIn, Mat& dilatedOut);
	void addControls();
};
