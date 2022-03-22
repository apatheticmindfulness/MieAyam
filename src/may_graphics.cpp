#include "may_graphics.h"

#define ENABLE_DIRECT2D

#pragma comment(lib, "d3d11.lib")
#ifdef ENABLE_DIRECT2D
#pragma comment(lib, "d2d1.lib")
#endif

static void _MAY_LocalRelease(IUnknown * object)
{
	if (object)
	{
		object->Release();
		object = 0;
	}
}

struct MAY_Graphics
{
	uint32_t				memoryWidth;
	uint32_t				memoryHeight;
	uint32_t				totalMemorySize;
	int8_t					isValid;

	ID3D11Device			* pD3DDevice;
	ID3D11DeviceContext		* pD3DContext;
	ID3D11RenderTargetView	* pD3DRenderTargetView;
	IDXGISwapChain			* pDXGISwapChain;

	/*ID2D1Device				* pD2DDevice;
	ID2D1DeviceContext		* pD2DDeviceContext;
	ID2D1Factory1			* pD2DFactory;
	ID2D1Bitmap1			* pD2DBitmap;*/

	MAY_Color				* pColorBuffer;
};

MAY_Graphics * MAY_CreateGraphics(MAY_Window * window)
{
	HRESULT result;

	MAY_Graphics * graphics = (MAY_Graphics*)malloc(sizeof(MAY_Graphics));

	if (graphics)
	{
		memset(graphics, 0, sizeof(MAY_Graphics));
		graphics->memoryWidth = MAY_GetWindowWidth(window);
		graphics->memoryHeight = MAY_GetWindowHeight(window);
		graphics->totalMemorySize = graphics->memoryWidth * graphics->memoryHeight * sizeof(MAY_Color);

		// ===============================================================
		// ===============================================================
		// Initialize Direct3D
		D3D_FEATURE_LEVEL featureLevels[] = {
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3
		};

		int32_t featureLevelsCount = ARRAYSIZE(featureLevels);

		DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
		swapChainDesc.BufferDesc.Width = MAY_GetWindowWidth(window);
		swapChainDesc.BufferDesc.Height = MAY_GetWindowHeight(window);
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.OutputWindow = (HWND)MAY_GetWindowHandle(window);
		swapChainDesc.Windowed = true;

		uint32_t deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT; // Direct2D support

#if _DEBUG
		deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		result = D3D11CreateDeviceAndSwapChain(
			0,
			D3D_DRIVER_TYPE_HARDWARE,
			0,
			deviceFlags,
			featureLevels,
			featureLevelsCount,
			D3D11_SDK_VERSION,
			&swapChainDesc,
			&graphics->pDXGISwapChain,
			&graphics->pD3DDevice,
			0,
			&graphics->pD3DContext);

		ID3D11Texture2D * pBackBuffer = 0;
		result = graphics->pDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		result = graphics->pD3DDevice->CreateRenderTargetView(pBackBuffer, 0, &graphics->pD3DRenderTargetView);

		_MAY_LocalRelease(pBackBuffer);

		graphics->pD3DContext->OMSetRenderTargets(1, &graphics->pD3DRenderTargetView, 0);

		// Create viewport
		D3D11_VIEWPORT viewport = { 0 };
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		viewport.Width = (float)MAY_GetWindowWidth(window);
		viewport.Height = (float)MAY_GetWindowHeight(window);
		viewport.MaxDepth = 1.0f;
		viewport.MinDepth = 1.0f;

		graphics->pD3DContext->RSSetViewports(1, &viewport);

		return graphics;
	}

	return 0;
}

void MAY_RenderEnd(MAY_Graphics * graphics)
{
	if (graphics)
	{
		float clearColor[] = {0.5f, 0.5f, 0.5f, 1.0f};
 		graphics->pD3DContext->ClearRenderTargetView(graphics->pD3DRenderTargetView, clearColor);
		graphics->pDXGISwapChain->Present(1, 0);
	}
}

void MAY_RenderStart(MAY_Graphics * graphics)
{
}

void MAY_FreeGraphics(MAY_Graphics * graphics)
{
	_MAY_LocalRelease(graphics->pDXGISwapChain);
	_MAY_LocalRelease(graphics->pD3DRenderTargetView);
	_MAY_LocalRelease(graphics->pD3DContext);
	_MAY_LocalRelease(graphics->pD3DDevice);

	if (graphics)
	{
		free(graphics);
		graphics = 0;
	}
}
