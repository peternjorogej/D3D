#pragma once

#include "Base.h"

// BINDABLE
class IBindable
{
public:
	virtual ~IBindable() = default;

	virtual void Bind() noexcept = 0;
};

// VERTEX SHADER
class VertexShader : public IBindable
{
public:
	VertexShader(const std::filesystem::path& Filepath, const char* lpEntryPoint = "Main");
	virtual ~VertexShader() noexcept;

	virtual void Bind() noexcept override;

	ID3DBlob* GetBytecode() noexcept;

private:
	ID3D11VertexShader* m_VertexShader = nullptr;
	ID3DBlob* m_Blob = nullptr;
};

// PIXEL SHADER
class PixelShader : public IBindable
{
public:
	PixelShader(const std::filesystem::path& Filepath, const char* lpEntryPoint = "Main");
	virtual ~PixelShader() noexcept;

	virtual void Bind() noexcept override;

private:
	ID3D11PixelShader* m_PixelShader = nullptr;
};

// VERTEX BUFFER
class VertexBuffer : public IBindable
{
public:
	template<typename V>
	VertexBuffer(const V* pVertices, size_t kCount)
		: m_Stride(sizeof(V))
	{
		D3D11_BUFFER_DESC      bd = {};
		D3D11_SUBRESOURCE_DATA sd = {};

		ZeroMemory(&bd, sizeof(bd));
		bd.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
		bd.ByteWidth           = sizeof(V) * UINT(kCount);
		bd.CPUAccessFlags      = 0u;
		bd.MiscFlags           = 0u;
		bd.StructureByteStride = sizeof(V);
		bd.Usage               = D3D11_USAGE_DEFAULT;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem          = pVertices;
		sd.SysMemPitch      = 0u;
		sd.SysMemSlicePitch = 0u;

		Renderer3D::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);
		assert(m_VertexBuffer != nullptr);
	}

	template<typename V>
	VertexBuffer(const List<V>& Vertices)
		: VertexBuffer(Vertices.data(), Vertices.size())
	{ }

	virtual ~VertexBuffer() noexcept;

	virtual void Bind() noexcept override;

private:
	ID3D11Buffer* m_VertexBuffer = nullptr;
	uint32_t      m_Stride = 0u;
};

// INDEX BUFFER
class IndexBuffer : public IBindable
{
public:
	IndexBuffer(const uint16_t* pIndices, size_t kCount);
	IndexBuffer(const List<uint16_t>& Indices)
		: IndexBuffer(Indices.data(), Indices.size())
	{ }

	virtual ~IndexBuffer() noexcept;

	virtual void Bind() noexcept override;

	uint32_t GetCount() const noexcept;

private:
	ID3D11Buffer* m_IndexBuffer = nullptr;
	uint32_t      m_Count = 0u;
};

// CONSTANT BUFFER
template<typename C>
class IConstantBuffer : public IBindable
{
public:
	IConstantBuffer()
	{
		D3D11_BUFFER_DESC      bd = {};
		D3D11_SUBRESOURCE_DATA sd = {};

		ZeroMemory(&bd, sizeof(bd));
		bd.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
		bd.ByteWidth           = sizeof(C);
		bd.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags           = 0u;
		bd.StructureByteStride = 0u;
		bd.Usage               = D3D11_USAGE_DYNAMIC;
		Renderer3D::GetDevice()->CreateBuffer(&bd, nullptr, &m_ConstantBuffer);
		assert(m_ConstantBuffer != nullptr);
	}

	IConstantBuffer(const C& Buffer)
	{
		D3D11_BUFFER_DESC      bd = {};
		D3D11_SUBRESOURCE_DATA sd = {};

		ZeroMemory(&bd, sizeof(bd));
		bd.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
		bd.ByteWidth           = sizeof(C);
		bd.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags           = 0u;
		bd.StructureByteStride = 0u;
		bd.Usage               = D3D11_USAGE_DYNAMIC;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem          = &Buffer;
		sd.SysMemPitch      = 0u;
		sd.SysMemSlicePitch = 0u;
		Renderer3D::GetDevice()->CreateBuffer(&bd, &sd, &m_ConstantBuffer);
		assert(m_ConstantBuffer != nullptr);
	}

	virtual ~IConstantBuffer() noexcept
	{
		m_ConstantBuffer->Release();
		m_ConstantBuffer = nullptr;
	}

	void Update(const C& Buffer)
	{
		D3D11_MAPPED_SUBRESOURCE ms = {};
		ZeroMemory(&ms, sizeof(ms));
		Renderer3D::GetDeviceContext()->Map(m_ConstantBuffer, 0u, D3D11_MAP_WRITE_DISCARD, 0u, &ms);
		CopyMemory(ms.pData, &Buffer, sizeof(C));
		Renderer3D::GetDeviceContext()->Unmap(m_ConstantBuffer, 0u);
	}

protected:
	ID3D11Buffer* m_ConstantBuffer = nullptr;
};

template<typename C>
class VertexConstantBuffer : public IConstantBuffer<C>
{
public:
	using IConstantBuffer<C>::m_ConstantBuffer;

public:
	using IConstantBuffer<C>::IConstantBuffer;

	virtual void Bind() noexcept override
	{
		Renderer3D::GetDeviceContext()->VSSetConstantBuffers(0u, 1u, &m_ConstantBuffer);
	}
};

template<typename C>
class PixelConstantBuffer : public IConstantBuffer<C>
{
public:
	using IConstantBuffer<C>::m_ConstantBuffer;

public:
	using IConstantBuffer<C>::IConstantBuffer;

	virtual void Bind() noexcept override
	{
		Renderer3D::GetDeviceContext()->PSSetConstantBuffers(0u, 1u, &m_ConstantBuffer);
	}
};

class TransformConstantBuffer : public IBindable
{
public:
	TransformConstantBuffer(class IDrawable* const& pParent);
	virtual ~TransformConstantBuffer() noexcept;

	virtual void Bind() noexcept override;

private:
	class IDrawable* m_Parent = nullptr;

private:
	struct ModelViewProjection
	{
		Matrix4x4 Model = {};
		Matrix4x4 MVP   = {};
	};
	static VertexConstantBuffer<ModelViewProjection>* s_TransformCB;
};

// INPUT LAYOUT
class InputLayout : public IBindable
{
public:
	InputLayout(const D3D11_INPUT_ELEMENT_DESC* pInputElements, size_t kCount, ID3DBlob* pVertexShaderBlob);
	InputLayout(const List<D3D11_INPUT_ELEMENT_DESC>& InputElements, ID3DBlob* pVertexShaderBlob);
	virtual ~InputLayout() noexcept;

	virtual void Bind() noexcept override;

private:
	ID3D11InputLayout* m_InputLayout      = nullptr;
	ID3DBlob*          m_VertexShaderBlob = nullptr;
};

// TEXTURE
class Texture : public IBindable
{
public:
	Texture(const class Image& i);

	virtual void Bind() noexcept override;

private:
	ID3D11ShaderResourceView* m_TextureView = nullptr;
};

// SAMPLER
class Sampler : public IBindable
{
public:
	Sampler();

	virtual void Bind() noexcept override;

private:
	ID3D11SamplerState* m_Sampler = nullptr;
};

// TOPOLOGY
class PrimitiveTopology : public IBindable
{
public:
	PrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY Topology);
	virtual ~PrimitiveTopology() = default;

	virtual void Bind() noexcept override;

private:
	D3D11_PRIMITIVE_TOPOLOGY m_Topology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
};
