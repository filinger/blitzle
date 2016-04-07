#pragma once

#include <d3d11_2.h>
#include <dxgi1_3.h>

#include "ComHeaders.h"

class DesktopDuplication
{
public:
	DesktopDuplication(UINT adapter, UINT output, UINT acquireTimeout);
	virtual ~DesktopDuplication();

	ID3D11Texture2D* acquireNextFrame(DXGI_OUTDUPL_FRAME_INFO& frameInfo);
	D3D11_MAPPED_SUBRESOURCE mapFrame(ID3D11Texture2D* frameTex);
	void unmapFrame(ID3D11Texture2D* frameTex);
	UINT frameHeight();
	UINT frameWidth();

protected:
	ID3D11Texture2D* copyToStaging(ID3D11Texture2D* tex);
	D3D11_TEXTURE2D_DESC getStagingTexDesc(D3D11_TEXTURE2D_DESC& originalTexDesc);

	D3D11_TEXTURE2D_DESC stagingTexDesc;

private:
	HRESULT hr = S_OK;
	UINT subResource = ::D3D11CalcSubresource(0, 0, 1);
	UINT acquireTimeout;

	ID3D11Device* d3dDevice = nullptr;
	ID3D11DeviceContext* d3dContext = nullptr;
	IDXGIOutputDuplication* outputDuplication = nullptr;
};

