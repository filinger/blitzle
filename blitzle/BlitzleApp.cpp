#include "BlitzleApp.h"

#include <iostream>
#include <opencv2/imgproc.hpp>

#include "ComHeaders.h"
#include "DesktopSource.h"
#include "Interceptor.h"
#include "PaladinsPlayerDetector.h"
#include "DotaPlayerDetector.h"


const std::string BlitzleDetectorStr[] = {
	"DOTA",
	"PALADINS"
};

// FIXME: interceptor will hang on debugger breakpoint,
// thus stopping all mouse and kb interaction. :Z
BlitzleApp::BlitzleApp(int argc, char** argv)
	: opts(parseCmdOptions(argc, argv))
{
	detector = instantiateDetector(opts.detectorType);
	detector->init(argc, argv, opts.withDebug, opts.withControls);
	desktop = new DesktopSource(opts.duplicationAdapter, opts.duplicationOutput, opts.duplicationTimeout);
#ifndef _DEBUG
	interceptor = new Interceptor();
#endif
}

BlitzleApp::~BlitzleApp()
{
	detector->destroy();
	delete detector;
	delete desktop;
#ifndef _DEBUG
	delete interceptor;
#endif
}

void BlitzleApp::drawNextFrame(cv::Mat& resultOut) const
{
	cv::Mat frame;
	desktop->acquireNextFrame(frame);

	resultOut = frame;
	if (opts.withDebug)
	{
		detector->processFrameDebug(frame, resultOut);
	}
	else
	{
		std::vector<cv::Point> players;
		detector->processFrame(frame, players);
		drawPlayers(players, resultOut);
	}

	if (opts.outputScale != 1.0f)
	{
		resize(resultOut, resultOut, cv::Size(), opts.outputScale, opts.outputScale, cv::INTER_AREA);
	}
}

void BlitzleApp::aimNextFrame() const
{
	cv::Mat frame;
	desktop->acquireNextFrame(frame);

	std::vector<cv::Point> players;
	detector->processFrame(frame, players);

	cv::Point crosshair = cv::Point(int(frame.cols * 0.5f), int(frame.rows * 0.5f));
	cv::Point target;
	bool targetFound = getClosestTarget(players, crosshair, target);

#ifndef _DEBUG
	if (targetFound)
	{
		cv::Point delta = cv::Point(target.x - crosshair.x, target.y - crosshair.y);
		if (delta.x != 0 && delta.y != 0)
		{
			interceptor->setTargetDelta(int(delta.x * opts.sensivity), int(delta.y * opts.sensivity));
		}
	}
	else
	{
		interceptor->unsetTarget();
	}
#endif
}

std::string BlitzleApp::getType() const
{
	return BlitzleDetectorStr[opts.detectorType];
}

IPlayerDetector* BlitzleApp::instantiateDetector(BlitzleDetector detectorType)
{
	switch (detectorType)
	{
	case PALADINS: return new PaladinsPlayerDetector();
	case DOTA: return new DotaPlayerDetector();
	default: return new DotaPlayerDetector();
	}
}

BlitzleOpts BlitzleApp::parseCmdOptions(int argc, char** argv)
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

	if (cmdOptionExists(argv, end, "-cuda"))
	{
		opts.withCuda = true;
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

void BlitzleApp::drawPlayers(const std::vector<cv::Point>& playersIn, cv::Mat& drawingOut)
{
	const int radius = 50;
	for (const auto& player : playersIn)
	{
		cv::circle(drawingOut, player, radius, cv::Scalar(255, 0, 255), 4);
	}
}

bool BlitzleApp::getClosestTarget(const std::vector<cv::Point>& playersIn, const cv::Point& originIn, cv::Point& targetOut)
{
	if (playersIn.empty())
	{
		return false;
	}

	targetOut = playersIn[0];
	float closestDist = euclideanDist(originIn, targetOut);
	for (const auto& player : playersIn)
	{
		float playerDist = euclideanDist(originIn, player);
		if (playerDist < closestDist)
		{
			closestDist = playerDist;
			targetOut = player;
		}
	}

	return true;
}

