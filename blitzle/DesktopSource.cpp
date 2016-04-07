#include "DesktopSource.h"

namespace cv 
{
	DesktopSource::DesktopSource(UINT adapter, UINT output, UINT timeout)
		: duplication(adapter, output, timeout)
	{
	}

	DesktopSource::~DesktopSource()
	{
	}

	bool DesktopSource::acquireNextFrame(Mat& frameMat)
	{
		DXGI_OUTDUPL_FRAME_INFO frameInfo;
		ID3D11Texture2D* frameTex = duplication.acquireNextFrame(frameInfo);
		D3D11_MAPPED_SUBRESOURCE frameMapped = duplication.mapFrame(frameTex);
		
		frameMat = Mat(duplication.frameHeight(), duplication.frameWidth(), CV_8UC4, frameMapped.pData, frameMapped.RowPitch);
		
		duplication.unmapFrame(frameTex);
		frameTex->Release();
		return frameInfo.TotalMetadataBufferSize != 0;
	}
}
