#include "DesktopSource.h"
#include "DotaPlayerDetector.h"
#include "PaladinsPlayerDetector.h"
#include "Interceptor.h"

#include <opencv2/highgui.hpp>

#include <iostream>
#include <string>
#include <algorithm>
#include <conio.h>

using namespace cv;

enum BlitzleDetector {
	DOTA, PALADINS
};

const std::string BlitzleDetectorStr[] = {
	"DOTA", "PALADINS"
};

struct BlitzleOpts {
	BlitzleDetector detectorType = DOTA;
	bool withDebug = false;
	bool withControls = false;
	float outputScale = 0.5f;
	float sensivity = 1.0f;
	int duplicationAdapter = 0;
	int duplicationOutput = 0;
	int duplicationTimeout = 2000;
};

IPlayerDetector* instantiateDetector(BlitzleDetector detectorType);
BlitzleOpts parseCmdOptions(int argc, char** argv);
void drawPlayers(const vector<Point>& playersIn, Mat& drawingOut);
bool getClosestTarget(const vector<Point>& playersIn, const Point& originIn, Point& targetOut);
void performAiming(const vector<Point>& playersIn, const Point& originIn, const float sensivity, Interceptor& interceptor);

RNG rng(12345);

int main(int argc, char** argv)
{
#ifdef _DEBUG
	std::cout << "Waiting for debug attach..." << std::endl;
	while (!_kbhit()) { }
#else
	// FIXME: interceptor will hang on debugger breakpoint,
	// thus stopping all mouse and kb interaction. :Z
	Interceptor interceptor;
#endif

	BlitzleOpts opts = parseCmdOptions(argc, argv);
	DesktopSource desktop(opts.duplicationAdapter, opts.duplicationOutput, opts.duplicationTimeout);
	IPlayerDetector* detector = instantiateDetector(opts.detectorType);

	detector->init(argc, argv, opts.withDebug, opts.withControls);

	std::string mainWindow = std::string("Blitzle: ") + BlitzleDetectorStr[opts.detectorType];
	namedWindow(mainWindow, WINDOW_AUTOSIZE);

	int keyPressed = 0;
	while (keyPressed != 0x1B) // Exit on 'Esc'
	{
		Mat frame;
		desktop.acquireNextFrame(frame);

		Mat result = Mat(frame);
		if (opts.withDebug) 
		{
			detector->processFrameDebug(frame, result);
		}
		else
		{
			vector<Point> players;
			detector->processFrame(frame, players);
			drawPlayers(players, result);
			Point crosshair = Point(int(frame.cols * 0.5f), int(frame.rows * 0.5f));
			performAiming(players, crosshair, opts.sensivity, interceptor);
		}

		if (opts.outputScale != 1.0f) 
		{
			resize(result, result, Size(), opts.outputScale, opts.outputScale, INTER_AREA);
		}

		imshow(mainWindow, result);
		keyPressed = waitKey(1);
	}

	destroyAllWindows();
	detector->destroy();
	delete detector;
	return 0;
}

IPlayerDetector* instantiateDetector(BlitzleDetector detectorType)
{
	switch (detectorType) 
	{
	case PALADINS: return new PaladinsPlayerDetector();
	case DOTA: return new DotaPlayerDetector();
	default: return new DotaPlayerDetector();
	}
	return nullptr;
}

BlitzleOpts parseCmdOptions(int argc, char** argv)
{
	BlitzleOpts opts;
	char** end = argv + argc;

	if (cmdOptionExists(argv, end, "-dota"))
	{
		opts.detectorType = DOTA;
	}

	if (cmdOptionExists(argv, end, "-paladins"))
	{
		opts.detectorType = PALADINS;
	}

	if (cmdOptionExists(argv, end, "-debug"))
	{
		opts.withDebug = true;
	}

	if (cmdOptionExists(argv, end, "-controls"))
	{
		opts.withControls = true;
	}

	char* outputScaleOpt = getCmdOption(argv, end, "-scale");
	if (outputScaleOpt)
	{
		float outputScale = std::stof(std::string(outputScaleOpt));
		opts.outputScale = max(min(outputScale, 1.0f), 0.2f);
	}

	char* sensivityOpt = getCmdOption(argv, end, "-sensivity");
	if (sensivityOpt)
	{
		float sensivity = std::stof(std::string(sensivityOpt));
		opts.sensivity = max(min(sensivity, 10.0f), 0.0f);
	}

	char* duplicationOutputOpt = getCmdOption(argv, end, "-output");
	if (duplicationOutputOpt)
	{
		int duplicationOutput = std::stoi(std::string(duplicationOutputOpt));
		opts.duplicationOutput = max(duplicationOutput, 0);
	}

	char* duplicationAdapterOpt = getCmdOption(argv, end, "-adapter");
	if (duplicationAdapterOpt)
	{
		int duplicationAdapter = std::stoi(std::string(duplicationAdapterOpt));
		opts.duplicationAdapter = max(duplicationAdapter, 0);
	}

	return opts;
}

void drawPlayers(const vector<Point>& playersIn, Mat& drawingOut) 
{
	const int radius = 50;
	for (const auto& player : playersIn)
	{
		circle(drawingOut, player, radius, Scalar(255, 0, 255), 4);
	}
}

bool getClosestTarget(const vector<Point>& playersIn, const Point& originIn, Point& targetOut)
{
	if (playersIn.empty()) {
		return false;
	}

	targetOut = playersIn[0];
	float closestDist = euclideanDist(originIn, targetOut);
	for (const auto& player : playersIn)
	{
		float playerDist = euclideanDist(originIn, player);
		if (playerDist < closestDist) {
			closestDist = playerDist;
			targetOut = player;
		}
	}

	return true;
}

void performAiming(const vector<Point>& playersIn, const Point& originIn, const float sensivity, Interceptor& interceptor)
{
	Point target;
	bool targetFound = getClosestTarget(playersIn, originIn, target);

#ifndef _DEBUG
	if (targetFound)
	{
		Point delta = Point(target.x - originIn.x, target.y - originIn.y);
		if (delta.x != 0 && delta.y != 0) 
		{
			std::cout << "Delta: " << delta.x << ":" << delta.y << std::endl;
			interceptor.setTargetDelta(int(delta.x * sensivity), int(delta.y * sensivity));
		}
	}
	else
	{
		interceptor.unsetTarget();
	}
#endif
}
