#include "OVR_CAPI_D3D.h"

#include "openvr.h"
#include <d3d11.h>

#include "REV_Assert.h"
#include "REV_Common.h"

DXGI_FORMAT ovr_TextureFormatToDXGIFormat(ovrTextureFormat format, unsigned int flags)
{
	if (flags & ovrTextureMisc_DX_Typeless)
	{
		switch (format)
		{
			case OVR_FORMAT_UNKNOWN:				return DXGI_FORMAT_UNKNOWN;
			//case OVR_FORMAT_B5G6R5_UNORM:			return DXGI_FORMAT_B5G6R5_TYPELESS;
			//case OVR_FORMAT_B5G5R5A1_UNORM:		return DXGI_FORMAT_B5G5R5A1_TYPELESS;
			//case OVR_FORMAT_B4G4R4A4_UNORM:		return DXGI_FORMAT_B4G4R4A4_TYPELESS;
			case OVR_FORMAT_R8G8B8A8_UNORM:			return DXGI_FORMAT_R8G8B8A8_TYPELESS;
			case OVR_FORMAT_R8G8B8A8_UNORM_SRGB:	return DXGI_FORMAT_R8G8B8A8_TYPELESS;
			case OVR_FORMAT_B8G8R8A8_UNORM:			return DXGI_FORMAT_B8G8R8A8_TYPELESS;
			case OVR_FORMAT_B8G8R8A8_UNORM_SRGB:	return DXGI_FORMAT_B8G8R8A8_TYPELESS;
			case OVR_FORMAT_B8G8R8X8_UNORM:			return DXGI_FORMAT_B8G8R8X8_TYPELESS;
			case OVR_FORMAT_B8G8R8X8_UNORM_SRGB:	return DXGI_FORMAT_B8G8R8X8_TYPELESS;
			case OVR_FORMAT_R16G16B16A16_FLOAT:		return DXGI_FORMAT_R16G16B16A16_TYPELESS;
			case OVR_FORMAT_D16_UNORM:				return DXGI_FORMAT_R16_TYPELESS;
			case OVR_FORMAT_D24_UNORM_S8_UINT:		return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			case OVR_FORMAT_D32_FLOAT:				return DXGI_FORMAT_R32_TYPELESS;
			case OVR_FORMAT_D32_FLOAT_S8X24_UINT:	return DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;
			default: return DXGI_FORMAT_UNKNOWN;
		}
	}
	else
	{
		switch (format)
		{
			case OVR_FORMAT_UNKNOWN:				return DXGI_FORMAT_UNKNOWN;
			case OVR_FORMAT_B5G6R5_UNORM:			return DXGI_FORMAT_B5G6R5_UNORM;
			case OVR_FORMAT_B5G5R5A1_UNORM:			return DXGI_FORMAT_B5G5R5A1_UNORM;
			case OVR_FORMAT_B4G4R4A4_UNORM:			return DXGI_FORMAT_B4G4R4A4_UNORM;
			case OVR_FORMAT_R8G8B8A8_UNORM:			return DXGI_FORMAT_R8G8B8A8_UNORM;
			case OVR_FORMAT_R8G8B8A8_UNORM_SRGB:	return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			case OVR_FORMAT_B8G8R8A8_UNORM:			return DXGI_FORMAT_B8G8R8A8_UNORM;
			case OVR_FORMAT_B8G8R8A8_UNORM_SRGB:	return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
			case OVR_FORMAT_B8G8R8X8_UNORM:			return DXGI_FORMAT_B8G8R8X8_UNORM;
			case OVR_FORMAT_B8G8R8X8_UNORM_SRGB:	return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
			case OVR_FORMAT_R16G16B16A16_FLOAT:		return DXGI_FORMAT_R16G16B16A16_FLOAT;
			case OVR_FORMAT_D16_UNORM:				return DXGI_FORMAT_D16_UNORM;
			case OVR_FORMAT_D24_UNORM_S8_UINT:		return DXGI_FORMAT_D24_UNORM_S8_UINT;
			case OVR_FORMAT_D32_FLOAT:				return DXGI_FORMAT_D32_FLOAT;
			case OVR_FORMAT_D32_FLOAT_S8X24_UINT:	return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
			default: return DXGI_FORMAT_UNKNOWN;
		}
	}
}

UINT ovr_BindFlagsToD3DBindFlags(unsigned int flags)
{
	UINT result = 0;
	if (flags & ovrTextureBind_DX_RenderTarget)
		result |= D3D11_BIND_RENDER_TARGET;
	if (flags & ovrTextureBind_DX_UnorderedAccess)
		result |= D3D11_BIND_UNORDERED_ACCESS;
	if (flags & ovrTextureBind_DX_DepthStencil)
		result |= D3D11_BIND_DEPTH_STENCIL;
	return result;
}

OVR_PUBLIC_FUNCTION(ovrResult) ovr_CreateTextureSwapChainDX(ovrSession session,
                                                            IUnknown* d3dPtr,
                                                            const ovrTextureSwapChainDesc* desc,
                                                            ovrTextureSwapChain* out_TextureSwapChain)
{
	// TODO: DX12 support.
	ID3D11Device* pDevice;
	HRESULT hr = d3dPtr->QueryInterface(&pDevice);
	if (FAILED(hr))
		return ovrError_RuntimeException;

	// TODO: Implement support for texture flags.
	ID3D11Texture2D* texture;
	D3D11_TEXTURE2D_DESC tdesc = { 0 };
	tdesc.Width = desc->Width;
	tdesc.Height = desc->Height;
	tdesc.MipLevels = desc->MipLevels;
	tdesc.ArraySize = desc->ArraySize;
	tdesc.SampleDesc.Count = 1;
	tdesc.SampleDesc.Quality = 0;
	tdesc.Format = ovr_TextureFormatToDXGIFormat(desc->Format, desc->MiscFlags);
	tdesc.Usage = D3D11_USAGE_DEFAULT;
	tdesc.BindFlags = ovr_BindFlagsToD3DBindFlags(desc->BindFlags);
	hr = pDevice->CreateTexture2D(&tdesc, nullptr, &texture);
	if (FAILED(hr))
		return ovrError_RuntimeException;

	// TODO: Should add multiple buffers to swapchain?
	ovrTextureSwapChain swapChain = new ovrTextureSwapChainData();
	swapChain->length = 1;
	swapChain->index = 0;
	swapChain->desc = *desc;
	swapChain->texture.handle = texture;
	swapChain->texture.eType = vr::API_DirectX;
	swapChain->texture.eColorSpace = vr::ColorSpace_Auto; // TODO: Set this from the texture format.
	*out_TextureSwapChain = swapChain;
	return ovrSuccess;
}

OVR_PUBLIC_FUNCTION(ovrResult) ovr_GetTextureSwapChainBufferDX(ovrSession session,
                                                               ovrTextureSwapChain chain,
                                                               int index,
                                                               IID iid,
                                                               void** out_Buffer)
{
	ID3D11Texture2D* texturePtr = (ID3D11Texture2D*)chain->texture.handle;
	HRESULT hr = texturePtr->QueryInterface(iid, out_Buffer);
	if (FAILED(hr))
		return ovrError_RuntimeException;

	return ovrSuccess;
}

OVR_PUBLIC_FUNCTION(ovrResult) ovr_CreateMirrorTextureDX(ovrSession session,
                                                         IUnknown* d3dPtr,
                                                         const ovrMirrorTextureDesc* desc,
                                                         ovrMirrorTexture* out_MirrorTexture)
{
	// TODO: DX12 support.
	ID3D11Device* pDevice;
	HRESULT hr = d3dPtr->QueryInterface(&pDevice);
	if (FAILED(hr))
		return ovrError_RuntimeException;

	// TODO: Implement support for texture flags.
	ID3D11Texture2D* texture;
	D3D11_TEXTURE2D_DESC tdesc = { 0 };
	tdesc.Width = desc->Width;
	tdesc.Height = desc->Height;
	tdesc.MipLevels = 1;
	tdesc.ArraySize = 1;
	tdesc.SampleDesc.Count = 1;
	tdesc.SampleDesc.Quality = 0;
	tdesc.Format = ovr_TextureFormatToDXGIFormat(desc->Format, desc->MiscFlags);
	tdesc.Usage = D3D11_USAGE_DEFAULT;
	tdesc.BindFlags = D3D11_BIND_RENDER_TARGET;
	hr = pDevice->CreateTexture2D(&tdesc, nullptr, &texture);
	if (FAILED(hr))
		return ovrError_RuntimeException;

	// TODO: Should add multiple buffers to swapchain?
	ovrMirrorTexture mirrorTexture = new ovrMirrorTextureData();
	mirrorTexture->desc = *desc;
	mirrorTexture->texture.handle = texture;
	mirrorTexture->texture.eType = vr::API_DirectX;
	mirrorTexture->texture.eColorSpace = vr::ColorSpace_Auto; // TODO: Set this from the texture format.
	*out_MirrorTexture = mirrorTexture;
	return ovrSuccess;
}

OVR_PUBLIC_FUNCTION(ovrResult) ovr_GetMirrorTextureBufferDX(ovrSession session,
                                                            ovrMirrorTexture mirrorTexture,
                                                            IID iid,
                                                            void** out_Buffer)
{
	ID3D11Texture2D* texture = (ID3D11Texture2D*)mirrorTexture->texture.handle;
	ID3D11Device* pDevice;
	texture->GetDevice(&pDevice);
	ID3D11DeviceContext* pContext;
	pDevice->GetImmediateContext(&pContext);

	ovrLayerEyeFov* layer = &session->lastFrame;
	int perEyeWidth = mirrorTexture->desc.Width / 2;
	for (int i = 0; i < ovrEye_Count; i++)
	{
		ID3D11Texture2D* eyeTexture;
		IUnknown* eyeTexturePtr = (IUnknown*)layer->ColorTexture[i]->texture.handle;
		eyeTexturePtr->QueryInterface(&eyeTexture);
		pContext->CopySubresourceRegion(texture, 0, perEyeWidth * i, 0, 0, eyeTexture, 0, nullptr);
	}

	HRESULT hr = texture->QueryInterface(iid, out_Buffer);
	if (FAILED(hr))
		return ovrError_RuntimeException;

	return ovrSuccess;
}
