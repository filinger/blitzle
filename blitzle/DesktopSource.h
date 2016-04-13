#pragma once

#include <opencv2/core.hpp>

#include "DesktopDuplication.h"

class DesktopSource
{
public:
	DesktopSource(UINT adapter, UINT output, UINT timeout);
	virtual ~DesktopSource();

	bool acquireNextFrame(cv::Mat& frameMat);
	bool acquireNextFrame(cv::UMat& frameUMat);
	cv::Point getFramePointer() const;

private:
	DesktopDuplication duplication;
	DXGI_OUTDUPL_FRAME_INFO lastFrameInfo;
};
