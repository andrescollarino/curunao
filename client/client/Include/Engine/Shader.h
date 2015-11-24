#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

#include <string>
#include <hash_map>

using namespace DirectX;


namespace Engine
{
  class Shader
  {
  public:



    std::string GetName();
    std::hash<std::string> GetId();

  private:
    
    struct MatrixBufferType
    {
      XMMATRIX worldMatrix;
      XMMATRIX viewMatrix;
      XMMATRIX projectionMatrix;
    };

    ID3D11VertexShader * m_VS; // Vertex Shader
    ID3D11PixelShader * m_PS; // Pixel Shader
    ID3D11InputLayout * m_layaout;
    ID3D11Buffer * m_matrix_buffer;

    bool m_initialized;
    std::hash<std::string> m_id;
    std::string m_name;
  };
}