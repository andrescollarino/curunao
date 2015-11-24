#pragma once



#include <d3d11.h>
#include <string>

#include "Debug.h"

// Linkeo las librerias Direct3D ( Instrucciones para el Compilador de Microsft )
#pragma comment(lib , "d3d11.lib" )
#pragma comment(lib , "dxgi.lib")

namespace Engine
{
  namespace Direct3D
  {
    bool Initialize(int width, int height, bool vsync, HWND hwnd, bool fullScreen);

    void BeginScene(float r, float g, float b, float a);
    void EndScene();

    void EnableZBuffer();
    void DisableZBuffer();

    void EnableAlphaBlending();
    void DisableAlphaBlending();

    void Release();

    ID3D11Device * GetDevice();
    ID3D11DeviceContext * GetDeviceContext();
    std::string GetVideoCardDesc(void);
    int GetVideoCardMemory(void);
    bool IsVSYNC();
  }
}
