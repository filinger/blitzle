#include "DesktopDuplication.h"

DesktopDuplication::DesktopDuplication(UINT adapterOutput, UINT acquireTimeout)
	: acquireTimeout(acquireTimeout)
{
	CHECKED(hr, D3D11CreateDevice(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&d3dDevice,
		NULL,
		&d3dContext));

	IDXGIDevice* dxgiDevice = nullptr;
	CHECKED(hr, d3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice)));

	IDXGIAdapter* dxgiAdapter = nullptr;
	CHECKED(hr, dxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&dxgiAdapter)));
	dxgiDevice->Release();

	IDXGIOutput* dxgiOutput = nullptr;
	CHECKED(hr, dxgiAdapter->EnumOutputs(adapterOutput, &dxgiOutput));
	dxgiAdapter->Release();

	IDXGIOutput1* dxgiOutput1 = nullptr;
	CHECKED(hr, dxgiOutput->QueryInterface(__uuidof(dxgiOutput1), reinterpret_cast<void**>(&dxgiOutput1)));
	dxgiOutput->Release();

	CHECKED(hr, dxgiOutput1->DuplicateOutput(dxgiDevice, &outputDuplication));
	dxgiOutput1->Release();
}


DesktopDuplication::~DesktopDuplication()
{
	outputDuplication->Release();
	d3dDevice->Release();
	d3dContext->Release();
}

ID3D11Texture2D*  DesktopDuplication::acquireNextFrame(DXGI_OUTDUPL_FRAME_INFO& frameInfoOut)
{
	IDXGIResource* frameResource;
	CHECKED(hr, outputDuplication->AcquireNextFrame(acquireTimeout, &frameInfoOut, &frameResource));

	ID3D11Texture2D* frameTex;
	CHECKED(hr, frameResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&frameTex)));
	frameResource->Release();

	ID3D11Texture2D* frameStagingTex = copyToStaging(frameTex);
	frameTex->Release();

	CHECKED(hr, outputDuplication->ReleaseFrame());

	return frameStagingTex;
}

D3D11_MAPPED_SUBRESOURCE DesktopDuplication::mapFrame(ID3D11Texture2D* frameTex) {
	D3D11_MAPPED_SUBRESOURCE frameMapped;
	CHECKED(hr, d3dContext->Map(frameTex, subResource, D3D11_MAP_READ, 0, &frameMapped));
	return frameMapped;
}

void DesktopDuplication::unmapFrame(ID3D11Texture2D* frameTex)
{
	d3dContext->Unmap(frameTex, subResource);
}

UINT DesktopDuplication::frameHeight()
{
	return stagingTexDesc.Height;
}

UINT DesktopDuplication::frameWidth()
{
	return stagingTexDesc.Width;
}

ID3D11Texture2D* DesktopDuplication::copyToStaging(ID3D11Texture2D* tex) {
	D3D11_TEXTURE2D_DESC texDesc;
	tex->GetDesc(&texDesc);

	stagingTexDesc = getStagingTexDesc(texDesc);

	ID3D11Texture2D* stagingTex;
	CHECKED(hr, d3dDevice->CreateTexture2D(&stagingTexDesc, nullptr, &stagingTex));

	d3dContext->CopyResource(stagingTex, tex);
	
	return stagingTex;
}

D3D11_TEXTURE2D_DESC DesktopDuplication::getStagingTexDesc(D3D11_TEXTURE2D_DESC& originalTexDesc) {
	D3D11_TEXTURE2D_DESC stagingTexDesc = originalTexDesc;
	stagingTexDesc.Usage = D3D11_USAGE_STAGING;
	stagingTexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	stagingTexDesc.BindFlags = 0;
	stagingTexDesc.MiscFlags = 0;
	stagingTexDesc.MipLevels = 1;
	stagingTexDesc.ArraySize = 1;
	stagingTexDesc.SampleDesc.Count = 1;
	return stagingTexDesc;
}
