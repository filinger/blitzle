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
	void processFrame(Mat& frame, Mat& output);
	void destroy();

private:
	HsvRange hsvRange;
	int dilateSize;

	void drawPlayers(const Size& size, const vector<vector<Point>>& contours_in, Mat& drawing_out);
	void dilateRect(int dilate_size, const Mat& filtered_in, Mat& dilated_out);
	void addControls();
};
