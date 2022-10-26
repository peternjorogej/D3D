#include "Bindable.h"
#include "Drawable.h"
#include "Image.h"

#include <d3dcompiler.h>

#ifdef _MSC_VER
  #pragma comment (lib, "d3d11.lib")
  #pragma comment (lib, "d3dcompiler.lib")
#else
  // error "Please link d3d11.lib"
  // error "Please link d3dcompiler.lib"
#endif

// VERTEX BUFFER
VertexBuffer::~VertexBuffer() noexcept
{
    m_VertexBuffer->Release();
    m_VertexBuffer = nullptr;
    m_Stride = 0u;
}

void VertexBuffer::Bind() noexcept
{
    const UINT kOffset = 0;
    Renderer3D::GetDeviceContext()->IASetVertexBuffers(0u, 1u, &m_VertexBuffer, &m_Stride, &kOffset);
}

// INDEX BUFFER
IndexBuffer::IndexBuffer(const uint16_t* pIndices, size_t kCount)
    : m_Count(uint32_t(kCount))
{
    D3D11_BUFFER_DESC      bd = {};
    D3D11_SUBRESOURCE_DATA sd = {};

    ZeroMemory(&bd, sizeof(bd));
    bd.BindFlags           = D3D11_BIND_INDEX_BUFFER;
    bd.ByteWidth           = sizeof(uint16_t) * UINT(kCount);
    bd.CPUAccessFlags      = 0u;
    bd.MiscFlags           = 0u;
    bd.StructureByteStride = sizeof(uint16_t);
    bd.Usage               = D3D11_USAGE_DEFAULT;
    ZeroMemory(&sd, sizeof(sd));
    sd.pSysMem          = pIndices;
    sd.SysMemPitch      = 0u;
    sd.SysMemSlicePitch = 0u;

    Renderer3D::GetDevice()->CreateBuffer(&bd, &sd, &m_IndexBuffer);
    assert(m_IndexBuffer != nullptr);
}

IndexBuffer::~IndexBuffer() noexcept
{
    m_IndexBuffer->Release();
    m_IndexBuffer = nullptr;
    m_Count = 0u;
}

void IndexBuffer::Bind() noexcept
{
    Renderer3D::GetDeviceContext()->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R16_UINT, 0u);
}

uint32_t IndexBuffer::GetCount() const noexcept
{
    return m_Count;
}

// VERTEX SHADER
VertexShader::VertexShader(const std::filesystem::path& Filepath, const char* lpEntryPoint)
{
    D3DCompileFromFile(Filepath.c_str(), nullptr, nullptr, lpEntryPoint, "vs_5_0", 0u, 0u, &m_Blob, nullptr);
    assert(m_Blob != nullptr);
    Renderer3D::GetDevice()->CreateVertexShader(m_Blob->GetBufferPointer(), m_Blob->GetBufferSize(), nullptr, &m_VertexShader);
    assert(m_VertexShader != nullptr);
}

VertexShader::~VertexShader() noexcept
{
    m_Blob->Release();
    m_Blob = nullptr;
    m_VertexShader->Release();
    m_VertexShader = nullptr;
}

void VertexShader::Bind() noexcept
{
    Renderer3D::GetDeviceContext()->VSSetShader(m_VertexShader, nullptr, 0u);
}

ID3DBlob* VertexShader::GetBytecode()  noexcept
{
    return m_Blob;
}

// PIXEL SHADER
PixelShader::PixelShader(const std::filesystem::path& Filepath, const char* lpEntryPoint)
{
    ID3DBlob* pBlob = nullptr;

    D3DCompileFromFile(Filepath.c_str(), nullptr, nullptr, lpEntryPoint, "ps_5_0", 0u, 0u, &pBlob, nullptr);
    assert(pBlob != nullptr);
    Renderer3D::GetDevice()->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_PixelShader);
    assert(m_PixelShader != nullptr);
}

PixelShader::~PixelShader() noexcept
{
    m_PixelShader->Release();
    m_PixelShader = nullptr;
}

void PixelShader::Bind() noexcept
{
    Renderer3D::GetDeviceContext()->PSSetShader(m_PixelShader, nullptr, 0u);
}

InputLayout::InputLayout(const D3D11_INPUT_ELEMENT_DESC* pInputElements, size_t kCount, ID3DBlob* pVertexShaderBlob)
    : m_VertexShaderBlob(pVertexShaderBlob)
{
    ID3DBlob* pBlob = m_VertexShaderBlob;

    Renderer3D::GetDevice()->CreateInputLayout(pInputElements, UINT(kCount), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &m_InputLayout);
    assert(m_InputLayout != nullptr);
    Renderer3D::GetDeviceContext()->IASetInputLayout(m_InputLayout);
}

InputLayout::InputLayout(const List<D3D11_INPUT_ELEMENT_DESC>& InputElements, ID3DBlob* pVertexShaderBlob)
    : InputLayout(InputElements.data(), InputElements.size(), pVertexShaderBlob)
{ }

void InputLayout::Bind() noexcept
{
    Renderer3D::GetDeviceContext()->IASetInputLayout(m_InputLayout);
}

InputLayout::~InputLayout() noexcept
{
    m_InputLayout->Release();
    m_InputLayout = nullptr;
}

PrimitiveTopology::PrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY Topology)
    : m_Topology(Topology)
{ }

void PrimitiveTopology::Bind() noexcept
{
    Renderer3D::GetDeviceContext()->IASetPrimitiveTopology(m_Topology);
}

VertexConstantBuffer<TransformConstantBuffer::ModelViewProjection>* TransformConstantBuffer::s_TransformCB = nullptr;

TransformConstantBuffer::TransformConstantBuffer(IDrawable* const& pParent)
    : m_Parent(pParent)
{
    if (s_TransformCB == nullptr)
    {
        s_TransformCB = new VertexConstantBuffer<ModelViewProjection>();
    }
}

TransformConstantBuffer::~TransformConstantBuffer() noexcept
{
    if (s_TransformCB != nullptr)
    {
        delete s_TransformCB;
        s_TransformCB = nullptr;
    }
}

void TransformConstantBuffer::Bind() noexcept
{
    const Matrix4x4 Model = m_Parent->GetTransform();
    s_TransformCB->Update(
    {
        DirectX::XMMatrixTranspose(Model),
        DirectX::XMMatrixTranspose(Model * Renderer3D::GetCameraView() * Renderer3D::GetProjection())
    });
    s_TransformCB->Bind();
}

Texture::Texture(const Image& i)
{
    D3D11_TEXTURE2D_DESC td = {};
    ZeroMemory(&td, sizeof(td));
    td.BindFlags          = D3D11_BIND_SHADER_RESOURCE;
    td.CPUAccessFlags     = 0u;
    td.MiscFlags          = 0u;
    td.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
    td.Usage              = D3D11_USAGE_DEFAULT;
    td.ArraySize          = 1u;
    td.MipLevels          = 1u;
    td.SampleDesc.Count   = 1u;
    td.SampleDesc.Quality = 0u;
    td.Height             = i.GetHeight();
    td.Width              = i.GetWidth();
    D3D11_SUBRESOURCE_DATA sd = {};
    ZeroMemory(&sd, sizeof(sd));
    sd.pSysMem          = i.GetBufferPointer();
    sd.SysMemPitch      = i.GetPitch();
    sd.SysMemSlicePitch = 0u;
    ID3D11Texture2D* pTexture = nullptr;
    Renderer3D::GetDevice()->CreateTexture2D(&td, &sd, &pTexture);
    assert(pTexture != nullptr);

    D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
    ZeroMemory(&srvd, sizeof(srvd));
    srvd.Format                    = td.Format;
    srvd.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvd.Texture2D.MipLevels       = 1u;
    srvd.Texture2D.MostDetailedMip = 0u;
    Renderer3D::GetDevice()->CreateShaderResourceView(pTexture, &srvd, &m_TextureView);
    assert(m_TextureView != nullptr);
}

void Texture::Bind() noexcept
{
    Renderer3D::GetDeviceContext()->PSSetShaderResources(0u, 1u, &m_TextureView);
}

Sampler::Sampler()
{
    D3D11_SAMPLER_DESC sd = {};
    ZeroMemory(&sd, sizeof(sd));
    sd.Filter   = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    Renderer3D::GetDevice()->CreateSamplerState(&sd, &m_Sampler);
    assert(m_Sampler != nullptr);
}

void Sampler::Bind() noexcept
{
    Renderer3D::GetDeviceContext()->PSSetSamplers(0u, 1u, &m_Sampler);
}
