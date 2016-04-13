#pragma once

#include <d3d11_2.h>
#include <dxgi1_3.h>

class DesktopDuplication
{
public:
	DesktopDuplication(UINT adapter, UINT output, UINT acquireTimeout);
	virtual ~DesktopDuplication();

	ID3D11Texture2D* acquireNextFrame(DXGI_OUTDUPL_FRAME_INFO& frameInfo);
	D3D11_MAPPED_SUBRESOURCE mapFrame(ID3D11Texture2D* frameTex);
	void unmapFrame(ID3D11Texture2D* frameTex) const;
	UINT frameHeight() const;
	UINT frameWidth() const;

protected:
	ID3D11Texture2D* copyToStaging(ID3D11Texture2D* tex);
	static D3D11_TEXTURE2D_DESC getStagingTexDesc(D3D11_TEXTURE2D_DESC& originalTexDesc);

	D3D11_TEXTURE2D_DESC stagingTexDesc;

private:
	void reinitialize();
	void init();
	void destroy();

	UINT adapter, output, acquireTimeout = 0;

	HRESULT hr = S_OK;
	UINT subResource = ::D3D11CalcSubresource(0, 0, 1);

	ID3D11Device* d3dDevice = nullptr;
	ID3D11DeviceContext* d3dContext = nullptr;
	IDXGIOutputDuplication* outputDuplication = nullptr;
};

