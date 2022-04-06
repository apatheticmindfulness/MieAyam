#include "mieayam_basic_graphics.h"
#include "mieayam_config.h"

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3d11.lib")

typedef struct
{
	ID3D11Device			* pD3DDevice;
	ID3D11DeviceContext		* pD3DContext;
	ID3D11RenderTargetView  * pD3DRenderTarget;
	IDXGISwapChain			* pDXGISwapChain;
} mieayam_directx_internal;

static mieayam_directx_internal			_mieayam_graphics;		// Store DirectX graphics state

uint8_t MieAyam_InitGraphics(const mieayam_window_attributes window_attributes, int32_t index)
{
	HRESULT result;
	int32_t windowWidth = window_attributes.width;
	int32_t windowHeight = window_attributes.height;
	HWND windowHandle = MieAyam_GetWindowHandle(index);

	// Flags
	uint32_t flags = 0;

#if _DEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Feature level
	D3D_FEATURE_LEVEL featureLevels[] = 
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3
	};

	// Swap Chain Description
	DXGI_SWAP_CHAIN_DESC swapChainDesc = 
	{
		.BufferDesc.Width = windowWidth,
		.BufferDesc.Height = windowHeight,
		.BufferDesc.RefreshRate.Numerator = 60,
		.BufferDesc.RefreshRate.Denominator = 1,
		.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM,
		.SampleDesc.Count = 1,
		.SampleDesc.Quality = 0,
		.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
		.BufferCount = 1,
		.OutputWindow = windowHandle,
		.Windowed = true
	};

	// Create device and swap chain
	result = D3D11CreateDeviceAndSwapChain(
		0,
		D3D_DRIVER_TYPE_HARDWARE,
		0,
		flags,
		featureLevels,
		ARRAYSIZE(featureLevels),
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&_mieayam_graphics.pDXGISwapChain,
		&_mieayam_graphics.pD3DDevice,
		0,
		&_mieayam_graphics.pD3DContext);

	// Get backbuffer
	ID3D11Texture2D * backBuffer = 0;
	if (_mieayam_graphics.pDXGISwapChain)
	{
		result = IDXGISwapChain_GetBuffer(_mieayam_graphics.pDXGISwapChain, 0, &IID_ID3D11Texture2D, &backBuffer);
		if (FAILED(result))
		{
			ID3D11Texture2D_Release(backBuffer);
			MieAyam_CleanGraphics(); // Clean DirectX objects
			return false;
		}
	}

	// Create render target
	if (_mieayam_graphics.pD3DDevice)
	{
		result = ID3D11Device_CreateRenderTargetView(_mieayam_graphics.pD3DDevice, (ID3D11Resource*)backBuffer, 0, &_mieayam_graphics.pD3DRenderTarget);
		if (FAILED(result))
		{
			ID3D11Texture2D_Release(backBuffer);
			MieAyam_CleanGraphics(); // Clean DirectX objects
			return false;
		}
	}

	// Release backbuffer
	if (backBuffer)
	{
		ID3D11Texture2D_Release(backBuffer);
	}

	// Set Render target
	if (_mieayam_graphics.pD3DContext)
	{
		ID3D11DeviceContext_OMSetRenderTargets(_mieayam_graphics.pD3DContext, 1, &_mieayam_graphics.pD3DRenderTarget, 0);

		// Set viewport
		D3D11_VIEWPORT viewport =
		{
			.TopLeftY = 0.0f,
			.TopLeftX = 0.0f,
			.Width = (float)windowWidth,
			.Height = (float)windowHeight,
			.MaxDepth = 1.0f,
			.MinDepth = 1.0f
		};
		ID3D11DeviceContext_RSSetViewports(_mieayam_graphics.pD3DContext, 1, &viewport);
	}

	return true;
}

void MieAyam_RenderEnd()
{
	float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	ID3D11DeviceContext_ClearRenderTargetView(_mieayam_graphics.pD3DContext, _mieayam_graphics.pD3DRenderTarget, clearColor);
	IDXGISwapChain_Present(_mieayam_graphics.pDXGISwapChain, 1, 0);
}

void MieAyam_CleanGraphics()
{
	ID3D11RenderTargetView_Release(_mieayam_graphics.pD3DRenderTarget);
	ID3D11DeviceContext_Release(_mieayam_graphics.pD3DContext);
	ID3D11Device_Release(_mieayam_graphics.pD3DDevice);
	IDXGISwapChain_Release(_mieayam_graphics.pDXGISwapChain);
}
