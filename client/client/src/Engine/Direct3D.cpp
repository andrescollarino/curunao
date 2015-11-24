#include "Engine/Direct3D.h"

namespace Engine
{
  namespace Direct3D
  {
    static ID3D11Device * g_device = nullptr;
    static ID3D11DeviceContext * g_device_context = nullptr;
    static IDXGISwapChain * g_swap_chain = nullptr;
    static ID3D11RenderTargetView * g_render_target_view = nullptr;
    static ID3D11Texture2D * g_depth_stencil_buffer = nullptr;
    static ID3D11DepthStencilState * g_depth_stencil_state = nullptr;
    static ID3D11DepthStencilView * g_depth_stencil_view = nullptr;
    static ID3D11BlendState * g_alpha_enable_blending_state = nullptr;
    static ID3D11BlendState* g_alpha_disable_blending_state = nullptr;
    static ID3D11DepthStencilState * g_depth_disable_stencil_state = nullptr;
    static ID3D11RasterizerState * g_raster_state = nullptr;
    static std::string g_viceo_card_description;
    static int g_video_card_memory = 0;
    static bool g_vsync_enable = false;



    static bool InitializeSwapChain(HWND hwnd, bool fullscreen, int width, int height, unsigned int numerator, unsigned int  denominator)
    {
      DXGI_SWAP_CHAIN_DESC swapChainDesc;
      D3D_FEATURE_LEVEL featureLevel;
      HRESULT result;

      ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

      swapChainDesc.BufferCount = 1; // Un solo back buffer
      swapChainDesc.BufferDesc.Width = width;
      swapChainDesc.BufferDesc.Height = height;
      swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

      if (g_vsync_enable)
      {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
      }
      else
      {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
      }

      swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
      swapChainDesc.OutputWindow = hwnd;

      // Multisampleo desactivado
      swapChainDesc.SampleDesc.Count = 1;
      swapChainDesc.SampleDesc.Quality = 0;

      swapChainDesc.Windowed = !fullscreen;

      // Escalado no Especificado
      swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
      swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

      swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

      swapChainDesc.Flags = 0;

      featureLevel = D3D_FEATURE_LEVEL_11_0;

      // Creo el divace, swapchain y deviceContext
      result = D3D11CreateDeviceAndSwapChain(
        NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        0,
        &featureLevel,
        1,
        D3D11_SDK_VERSION,
        &swapChainDesc,
        &g_swap_chain,
        &g_device,
        NULL,
        &g_device_context
        );

      if (FAILED(result))
      {
        Debug::Log(__FILE__, __LINE__, "Error in D3D11CreateDeviceAndSwapChain", result);
        return false;
      }

      return true;
    }

    static bool InitializeDepthBuffer(int screenWidth, int screenHeight)
    {
      D3D11_TEXTURE2D_DESC depthBufferDesc;
      HRESULT result;

      ZeroMemory(&depthBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));

      depthBufferDesc.Width = screenWidth;
      depthBufferDesc.Height = screenHeight;
      depthBufferDesc.MipLevels = 1;
      depthBufferDesc.ArraySize = 1;
      depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
      depthBufferDesc.SampleDesc.Count = 1;
      depthBufferDesc.SampleDesc.Quality = 0;
      depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
      depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
      depthBufferDesc.CPUAccessFlags = 0;
      depthBufferDesc.MiscFlags = 0;

      result = g_device->CreateTexture2D(&depthBufferDesc, NULL, &g_depth_stencil_buffer);
      if (FAILED(result))
      {
        Debug::Log(__FILE__, __LINE__, "Error in CreateTexture2D", result);
        return false;
      }

      return true;
    }

    static bool InitializeDepthStencilBuffer()
    {
      D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
      HRESULT result;

      ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

      depthStencilDesc.DepthEnable = true;
      depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
      depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
      depthStencilDesc.StencilEnable = true;
      depthStencilDesc.StencilReadMask = 0xFF;
      depthStencilDesc.StencilWriteMask = 0xFF;

      depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
      depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
      depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
      depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

      depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
      depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
      depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
      depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

      result = g_device->CreateDepthStencilState(&depthStencilDesc, &g_depth_stencil_state);
      if (FAILED(result))
      {
        Debug::Log(__FILE__, __LINE__, "Error in CreateDepthStencilState", result);
        return false;
      }

      g_device_context->OMSetDepthStencilState(g_depth_stencil_state, 1);

      return true;
    }
    static bool InitializeStencilView()
    {
      D3D11_DEPTH_STENCIL_VIEW_DESC  depthStencilViewDesc;
      HRESULT result;
      ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

      depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
      depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
      depthStencilViewDesc.Texture2D.MipSlice = 0;

      result = g_device->CreateDepthStencilView(g_depth_stencil_buffer, &depthStencilViewDesc, &g_depth_stencil_view);
      if (FAILED(result))
      {
        Debug::Log(__FILE__, __LINE__, "Error in CreateDepthStencilView", result);
        return false;
      }
      return true;
    }
    static bool InitializeRasterizerStete()
    {
      D3D11_RASTERIZER_DESC rasterDesc;
      HRESULT result;

      rasterDesc.AntialiasedLineEnable = false;
      rasterDesc.CullMode = D3D11_CULL_BACK;
      rasterDesc.DepthBias = 0;
      rasterDesc.DepthBiasClamp = 0.0f;
      rasterDesc.DepthClipEnable = true;
      rasterDesc.FillMode = D3D11_FILL_SOLID;
      rasterDesc.FrontCounterClockwise = false;
      rasterDesc.MultisampleEnable = false;
      rasterDesc.ScissorEnable = false;
      rasterDesc.SlopeScaledDepthBias = 0.0f;

      result = g_device->CreateRasterizerState(&rasterDesc, &g_raster_state);
      if (FAILED(result))
      {
        Debug::Log(__FILE__, __LINE__, "Error in CreateRasterizerState", result);
        return false;
      }

      g_device_context->RSSetState(g_raster_state);
      return true;
    }

    static void InitializeViewport(int screenWidth, int screenHeight)
    {
      D3D11_VIEWPORT viewport;

      viewport.Width = (float)screenWidth;
      viewport.Height = (float)screenHeight;
      viewport.MinDepth = 0.0f;
      viewport.MaxDepth = 1.0f;
      viewport.TopLeftX = 0.0f;
      viewport.TopLeftY = 0.0f;

      g_device_context->RSSetViewports(1, &viewport);
    }
    static bool InitializeAlphaBlending()
    {

      D3D11_BLEND_DESC blendStateDesc;
      HRESULT result;

      ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));

      // TODO: Estuadiar los modos de alpha para mallor compresion
      blendStateDesc.RenderTarget[0].BlendEnable = TRUE;
      blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
      blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
      blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
      blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
      blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
      blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
      blendStateDesc.RenderTarget[0].RenderTargetWriteMask = 0x0F;

      result = g_device->CreateBlendState(&blendStateDesc, &g_alpha_enable_blending_state);
      if (FAILED(result))
      {
        Debug::Log(__FILE__, __LINE__, "Error in CreateBlendState", result);
        return false;
      }

      blendStateDesc.RenderTarget[0].BlendEnable = FALSE;
      result = g_device->CreateBlendState(&blendStateDesc, &g_alpha_disable_blending_state);
      if (FAILED(result))
      {
        Debug::Log(__FILE__, __LINE__, "Error in CreateBlendState", result);
        return false;
      }
      return true;
    }
    static bool InitializeZBuffer()
    {
      D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
      HRESULT result;

      ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

      depthStencilDesc.DepthEnable = false;
      depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
      depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
      depthStencilDesc.StencilEnable = true;
      depthStencilDesc.StencilReadMask = 0xFF;
      depthStencilDesc.StencilWriteMask = 0xFF;

      depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
      depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
      depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
      depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

      depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
      depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
      depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
      depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

      result = g_device->CreateDepthStencilState(&depthStencilDesc, &g_depth_disable_stencil_state);
      if (FAILED(result))
      {
        // TODO: Informar del Error
        return false;
      }
      return true;
    }


    bool Initialize(int width, int height, bool vsync, HWND hwnd, bool fullScreen)
    {
      HRESULT result;

      // Guardo el vsycn
      g_vsync_enable = vsync;

      IDXGIFactory * factory = nullptr;

      // Creo una interfas con DirectX (IDXGIFactory)
      result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
      if (FAILED(result))
      {
        Debug::Log(__FILE__, __LINE__, "Error in CreateDXGIFactory", result);
        return false;
      }

      IDXGIAdapter * adapter = nullptr;

      // Uso el IDXGIFactory para Obtener un adaptador(Placa de Video)
      result = factory->EnumAdapters(0, &adapter);
      if (FAILED(result))
      {
        Debug::Log(__FILE__, __LINE__, "Error in EnumAdapters", result);
        return false;
      }


      IDXGIOutput * adapterOutput = nullptr;

      // Enumero la cantidad de monitores (Adaptador de salida)
      result = adapter->EnumOutputs(0, &adapterOutput);
      if (FAILED(result))
      {
        Debug::Log(__FILE__, __LINE__, "Error in EnumOutputs", result);
        return false;
      }

      unsigned int numModes = 0;
      // Obtengo el numero de modos que se ajustan al formato DXGI_FORMAT_R8G8B8A8 para el monitor
      result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
      if (FAILED(result))
      {
        Debug::Log(__FILE__, __LINE__, "Error in GetDisplayModeList", result);
        return false;
      }

      // Creo una lista con todos los posibles modos para esta combinacion placa de video monitor.
      DXGI_MODE_DESC * displayModeList = new DXGI_MODE_DESC[numModes];
      // Cargo la lista de display Mode
      result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
      if (FAILED(result))
      {
        Debug::Log(__FILE__, __LINE__, "Error in GetDisplayModeList", result);
        return false;
      }

      unsigned int numerator = 0;
      unsigned int denominator = 0;

      // Obtego los valores de refrescos del Monitor
      for (int i = 0; i < (int)numModes; ++i)
      {

        if (displayModeList[i].Width == (unsigned int)width && displayModeList[i].Height == (unsigned int)height)
        {
          numerator = displayModeList[i].RefreshRate.Numerator;
          denominator = displayModeList[i].RefreshRate.Denominator;
          break;
        }

      }

      if (numerator == 0 && denominator == 0)
      {
        Debug::Log(__FILE__, __LINE__, "Error numerator and Denomitar is 0");
        return false;
      }

      DXGI_ADAPTER_DESC adapterDesc;

      // Obtengo la descripcion del adaptador
      result = adapter->GetDesc(&adapterDesc);
      if (FAILED(result))
      {
        Debug::Log(__FILE__, __LINE__, "Error in GetDesc", result);
        return false;
      }

      // Guardo la cantidad de memorya de video
      g_video_card_memory = adapterDesc.DedicatedVideoMemory;

      // Convierto el nombre de la VGA a string
      char tmp_string[128];
      int error;
      unsigned int stringLenht;

      error = wcstombs_s(&stringLenht, tmp_string, 128, adapterDesc.Description, 128);
      if (error != 0)
      {
        Debug::Log(__FILE__, __LINE__, "Error in DXGI_ADAPTER_DESC");
        return false;
      }

      g_viceo_card_description = tmp_string;

      // Released Memory
      delete[] displayModeList;
      displayModeList = nullptr;

      adapterOutput->Release();
      adapterOutput = nullptr;

      adapter->Release();
      adapter = nullptr;

      factory->Release();
      factory = nullptr;


      if (!InitializeSwapChain(hwnd, fullScreen, width, height, numerator, denominator))
      {
        Debug::Log(__FILE__, __LINE__, "Error in InitializeSwapChain");
        return false;
      }

      ID3D11Texture2D * backBuffer = nullptr;

      // Obtengo el puntero al BackBuffer
      result = g_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
      if (FAILED(result))
      {
        Debug::Log(__FILE__, __LINE__, "Error in GetBuffer", result);
        return false;
      }

      // Creo el render Target View
      result = g_device->CreateRenderTargetView(backBuffer, NULL, &g_render_target_view);
      if (FAILED(result))
      {
        Debug::Log(__FILE__, __LINE__, "Error in CreateRenderTargetView", result);
        return false;
      }

      // No te Nesesito MUERE
      backBuffer->Release();
      backBuffer = nullptr;

      if (!InitializeDepthBuffer(width, height))
      {
        Debug::Log(__FILE__, __LINE__, "Error in InitializeDepthBuffer");
        return false;
      }

      if (!InitializeDepthStencilBuffer())
      {
        Debug::Log(__FILE__, __LINE__, "Error in InitializeDepthStencilBuffer");
        return false;
      }

      if (!InitializeStencilView())
      {
        Debug::Log(__FILE__, __LINE__, "Error in InitializeStencilView");
        return false;
      }

      g_device_context->OMSetRenderTargets(1, &g_render_target_view, g_depth_stencil_view);
      if (!InitializeRasterizerStete())
      {
        Debug::Log(__FILE__, __LINE__, "Error in InitializeRasterizerStete");
        return false;
      }

      InitializeViewport(width, height);

      if (!InitializeAlphaBlending())
      {
        Debug::Log(__FILE__, __LINE__, "Error in InitializeAlphaBlending");
        return false;
      }

      if (!InitializeZBuffer())
      {
        Debug::Log(__FILE__, __LINE__, "Error in InitializeZBuffer");
        return false;
      }

      return true;
    }



    void BeginScene(float r, float g, float b, float a)
    {
      float color[4];

      color[0] = r;
      color[1] = g;
      color[2] = b;
      color[3] = a;

      g_device_context->ClearRenderTargetView(g_render_target_view, color);
      g_device_context->ClearDepthStencilView(g_depth_stencil_view, D3D11_CLEAR_DEPTH, 1.0f, 0);
    }

    void EndScene()
    {
      if (g_vsync_enable)
      {
        g_swap_chain->Present(1, 0);
      }
      else
      {
        g_swap_chain->Present(0, 0);
      }
    }

    void EnableZBuffer()
    {
      g_device_context->OMSetDepthStencilState(g_depth_stencil_state, 1);
    }

    void DisableZBuffer()
    {
      g_device_context->OMSetDepthStencilState(g_depth_disable_stencil_state, 1);
    }

    void EnableAlphaBlending()
    {
      float blendFactor[4];

      blendFactor[0] = 0.0f;
      blendFactor[1] = 0.0f;
      blendFactor[2] = 0.0f;
      blendFactor[3] = 0.0f;


      g_device_context->OMSetBlendState(g_alpha_enable_blending_state, blendFactor, 0xFFFFFFFF);
    }
    void DisableAlphaBlending()
    {
      float blendFactor[4];

      blendFactor[0] = 0.0f;
      blendFactor[1] = 0.0f;
      blendFactor[2] = 0.0f;
      blendFactor[3] = 0.0f;

      g_device_context->OMSetBlendState(g_alpha_disable_blending_state, blendFactor, 0xFFFFFFFF);
    }

    void Release()
    {
      if (g_swap_chain != nullptr)
      {
        g_swap_chain->SetFullscreenState(false, NULL);
      }

      if (g_alpha_enable_blending_state != nullptr)
      {
        g_alpha_enable_blending_state->Release();
        g_alpha_enable_blending_state = nullptr;
      }

      if (g_alpha_disable_blending_state != nullptr)
      {
        g_alpha_disable_blending_state->Release();
        g_alpha_disable_blending_state = nullptr;
      }

      if (g_depth_disable_stencil_state != nullptr)
      {
        g_depth_disable_stencil_state->Release();
        g_depth_disable_stencil_state = nullptr;
      }

      if (g_raster_state != nullptr)
      {
        g_raster_state->Release();
        g_raster_state = nullptr;
      }

      if (g_depth_stencil_view != nullptr)
      {
        g_depth_stencil_view->Release();
        g_depth_stencil_view = nullptr;
      }

      if (g_depth_stencil_state != nullptr)
      {
        g_depth_stencil_state->Release();
        g_depth_stencil_state = nullptr;
      }
    }

    ID3D11Device * GetDevice()
    {
      return g_device;
    }

    ID3D11DeviceContext * GetDeviceContext()
    {
      return g_device_context;
    }

    std::string GetVideoCardDesc(void)
    {
      return g_viceo_card_description;
    }

    int GetVideoCardMemory(void)
    {
      return g_video_card_memory;
    }

    bool IsVSYNC()
    {
      return g_vsync_enable;
    }
  }
}