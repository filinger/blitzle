#include "DesktopSource.h"

#include <opencv2/core/directx.hpp>

DesktopSource::DesktopSource(UINT adapter, UINT output, UINT timeout)
	: duplication(adapter, output, timeout)
{
}

DesktopSource::~DesktopSource()
{
}

bool DesktopSource::acquireNextFrame(cv::Mat& frameMat)
{
	DXGI_OUTDUPL_FRAME_INFO frameInfo;
	ID3D11Texture2D* frameTex = duplication.acquireNextFrame(frameInfo);
	D3D11_MAPPED_SUBRESOURCE frameMapped = duplication.mapFrame(frameTex);

	frameMat = cv::Mat(duplication.frameHeight(), duplication.frameWidth(), CV_8UC4, frameMapped.pData, frameMapped.RowPitch);

	duplication.unmapFrame(frameTex);
	frameTex->Release();
	lastFrameInfo = frameInfo;
	return frameInfo.TotalMetadataBufferSize != 0;
}

bool DesktopSource::acquireNextFrame(cv::UMat& frameUMat)
{
	DXGI_OUTDUPL_FRAME_INFO frameInfo;
	ID3D11Texture2D* frameTex = duplication.acquireNextFrame(frameInfo);

	cv::directx::convertFromD3D11Texture2D(frameTex, frameUMat);

	frameTex->Release();
	lastFrameInfo = frameInfo;
	return frameInfo.TotalMetadataBufferSize != 0;
}

cv::Point DesktopSource::getFramePointer() const
{
	return cv::Point(lastFrameInfo.PointerPosition.Position.x, lastFrameInfo.PointerPosition.Position.y);
}

