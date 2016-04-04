#pragma once

#include "IPlayerDetector.h"
#include "AdditionalTypes.h"

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;

class DotaPlayerDetector : public IPlayerDetector
{
public:
	DotaPlayerDetector();
	virtual ~DotaPlayerDetector();

	static void controlsCallback(int, void* context);

	void init();
	void processFrame(const Mat& frameIn, vector<Point>& playersOut);
	void destroy();

private:
	HsvRange hsvRange;
	int dilateSize;

	void showPlayers(const vector<vector<Point>>& contoursIn, vector<Point>& playersOut);
	void dilateRect(int dilateSize, const Mat& filteredIn, Mat& dilatedOut);
	void addControls();
};
