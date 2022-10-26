#pragma once

#include "Base.h"
#include "Bindable.h"

// DRAWABLE
class IDrawable
{
public:
	IDrawable() = default;
	virtual ~IDrawable() noexcept;

	virtual Matrix4x4 GetTransform() noexcept = 0;
	virtual void      Update(float dt) noexcept = 0;

	void Draw() const noexcept;
	
protected:
	void EmplaceIndexBuffer(uint32_t kDrawableID, const List<uint16_t>& Indices) noexcept;
	template<typename B, typename... TArgs>
	B*   EmplaceBindable(uint32_t kDrawableID, TArgs&&... Args) noexcept;

protected:
	IndexBuffer*     m_IndexBuffer = nullptr;
	List<IBindable*> m_Bindables   = {};
};


template<typename Tp>
class IDrawableChild : public IDrawable
{
public:
	IDrawableChild();
	virtual ~IDrawableChild() noexcept = default;

	virtual void      Update(float dt) noexcept override;
	virtual Matrix4x4 GetTransform() noexcept override;

	const Float3& GetPosition() const noexcept;
	void          SetPosition(const Float3& Position) noexcept;

protected:
	template<typename V>
	IDrawableChild(
		const List<V>&                        Vertices,
		const List<uint16_t>&                 Indices,
		const List<D3D11_INPUT_ELEMENT_DESC>& InputElements
	);

	void EmplaceSharedIndexBuffer(uint32_t kDrawableID, const List<uint16_t>& Indices) noexcept;
	template<typename B, typename... TArgs>
	B*   EmplaceSharedBindable(uint32_t kDrawableID, TArgs&&... Args) noexcept;

protected:
	Float3 m_Radius = {};
	Float3 m_Position = {};
	Float3 m_Rotation = {};
	Float3 m_LocalVelocity = {};
	Float3 m_WorldVelocity = {};

protected:
	static IndexBuffer*     m_SharedIndexBuffer;
	static List<IBindable*> m_SharedBindables;
};

class Plane : public IDrawableChild<Plane>
{
public:
	Plane();
	virtual ~Plane() noexcept = default;
};

class Box : public IDrawableChild<Box>
{
public:
	Box();
	virtual ~Box() noexcept = default;
};

class Pyramid : public IDrawableChild<Pyramid>
{
public:
	Pyramid();
	virtual ~Pyramid() noexcept = default;
};

class Prism : public IDrawableChild<Prism>
{
public:
	Prism();
	virtual ~Prism() noexcept = default;
};

class Surface : public IDrawableChild<Surface>
{
public:
	Surface();
	virtual ~Surface() noexcept = default;
};

class Mesh : public IDrawableChild<Mesh>
{
public:
	Mesh(const char* lpFilepath, float Scale);
	virtual ~Mesh() noexcept = default;
};

class SolidSphere : public IDrawableChild<SolidSphere>
{
public:
	SolidSphere(float Radius);
	virtual ~SolidSphere() noexcept = default;
};


template<typename B, typename... TArgs>
B* IDrawable::EmplaceBindable(uint32_t kDrawableID, TArgs&&... Args) noexcept
{
	// static_assert(std::is_base_of_v<IBindable, B>, "type is not a child of 'IBindable'");
	// TODO: Messy (but efficient?)
	if constexpr (std::is_same<B, VertexShader>::value)
	{
		return (B*)m_Bindables.emplace_back(Renderer3D::GetVertexShader(std::forward<TArgs>(Args)...));
	}
	else if constexpr (std::is_same<B, PixelShader>::value)
	{
		return (B*)m_Bindables.emplace_back(Renderer3D::GetPixelShader(std::forward<TArgs>(Args)...));
	}
	else if constexpr (std::is_same<B, VertexBuffer>::value)
	{
		return (B*)m_Bindables.emplace_back(Renderer3D::GetVertexBuffer(kDrawableID, std::forward<TArgs>(Args)...));
	}
	else
	{
		assert(typeid(B) != typeid(IndexBuffer) && "MUST use EmplaceIndexBuffer() to add an index buffer to a drawable");
		return (B*)m_Bindables.emplace_back(new B(std::forward<TArgs>(Args)...));
	}
}


template<typename Tp>
IndexBuffer* IDrawableChild<Tp>::m_SharedIndexBuffer = nullptr;

template<typename Tp>
List<IBindable*> IDrawableChild<Tp>::m_SharedBindables = {};

template<typename Tp>
inline IDrawableChild<Tp>::IDrawableChild()
{
	m_Radius = Random::Float3F(5.0f, 50.0f);
	m_Position = Random::Float3F(0.1f, Constants::Tau);
	m_Rotation = Random::Float3F(0.1f, Constants::Tau);
	m_LocalVelocity = Random::Float3F(0.1f, Constants::Tau);
	m_WorldVelocity = Random::Float3F(0.1f, Constants::Pi / 4.0f);
}

template<typename Tp>
void IDrawableChild<Tp>::Update(float dt) noexcept
{
	m_Position = m_Position + (m_LocalVelocity * dt);
	m_Rotation = m_Rotation + (m_WorldVelocity * dt);

	for (IBindable* const& pSharedBindable : m_SharedBindables)
	{
		pSharedBindable->Bind();
	}
}

template<typename Tp>
Matrix4x4 IDrawableChild<Tp>::GetTransform() noexcept
{
	/*const DirectX::XMMATRIX LocalRotation = DirectX::XMMatrixRotationX(m_Position.X) *
											DirectX::XMMatrixRotationY(m_Position.Y) *
											DirectX::XMMatrixRotationZ(m_Position.Z);
	const DirectX::XMMATRIX WorldRotation = DirectX::XMMatrixRotationX(m_Rotation.X) *
											DirectX::XMMatrixRotationY(-m_Rotation.Y) *
											DirectX::XMMatrixRotationZ(m_Rotation.Z);

	return LocalRotation * DirectX::XMMatrixTranslation(m_Radius.X, m_Radius.Y, m_Radius.Z) * WorldRotation;*/
	return DirectX::XMMatrixRotationRollPitchYaw(m_Position.X, m_Position.Y, m_Position.Z) *
		   DirectX::XMMatrixTranslation(m_Radius.X, m_Radius.Y, m_Radius.Z) *
		   DirectX::XMMatrixRotationRollPitchYaw(m_Rotation.X, m_Rotation.Y, m_Rotation.Z);
}

template<typename Tp>
const Float3& IDrawableChild<Tp>::GetPosition() const noexcept
{
	return m_Position;
}

template<typename Tp>
void IDrawableChild<Tp>::SetPosition(const Float3& Position) noexcept
{
	m_Radius = Position;
}

template<typename Tp>
template<typename V>
inline IDrawableChild<Tp>::IDrawableChild(const List<V>& Vertices, const List<uint16_t>& Indices, const List<D3D11_INPUT_ELEMENT_DESC>& InputElements)
{
	static_assert(false, "NotImplementedException");
}

template<typename Tp>
inline void IDrawableChild<Tp>::EmplaceSharedIndexBuffer(uint32_t kDrawableID, const List<uint16_t>& Indices) noexcept
{
	assert(m_SharedIndexBuffer == nullptr && "Shared index buffer is already set");
	m_SharedIndexBuffer = Renderer3D::GetIndexBuffer(kDrawableID, Indices);
	m_SharedBindables.emplace_back(m_SharedIndexBuffer);
}

template<typename Tp>
template<typename B, typename ...TArgs>
inline B* IDrawableChild<Tp>::EmplaceSharedBindable(uint32_t kDrawableID, TArgs && ...Args) noexcept
{
	assert(typeid(B) != typeid(IndexBuffer) && "MUST use EmplaceSharedIndexBuffer() to add an index buffer to a drawable");
	return (B*)m_SharedBindables.emplace_back(new B(std::forward<TArgs>(Args)...));
}
