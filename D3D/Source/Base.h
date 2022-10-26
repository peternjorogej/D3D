#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#ifdef _MSC_VER
  #pragma warning(disable: 26451)
#endif // _MSC_VER

#include <stdio.h>
#include <stdint.h>

#include <Windows.h>
#include <d3d11.h>

#include "Maths.h"
#include <DirectXMath.h>

#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>

#ifndef NDEBUG
  #define GfxError(x, ...)  __Assert((x), __FILE__, __LINE__, __VA_ARGS__)
#else
  #define GfxError(x, ...)  bool(0)
#endif // !NDEBUG

using String = std::string;

template<typename Tp>
using List = std::vector<Tp>;

template<typename K, typename V>
using Dictionary = std::unordered_map<K, V>;

using Matrix4x4 = DirectX::XMMATRIX;


class VertexShader;
class PixelShader;
class VertexBuffer;
class IndexBuffer;


class Renderer3D
{
public:
	static bool                 Initialize(HINSTANCE hInstance);
	static void                 Shutdown();
	static void                 Run();

	static void                 DrawIndexed(uint32_t kIndexCount) noexcept;

	static VertexShader*        GetVertexShader(const std::filesystem::path& Filepath, const char* lpEntryPoint = "Main") noexcept;
	static PixelShader*         GetPixelShader(const std::filesystem::path& Filepath, const char* lpEntryPoint = "Main") noexcept;
	template<typename V>
	static VertexBuffer*        GetVertexBuffer(uint32_t kTypeID, const List<V>& Vertices = {});
	static IndexBuffer*         GetIndexBuffer(uint32_t kTypeID, const List<uint16_t>& Indices = {});

	static Dictionary<uint32_t, VertexBuffer*>& GetVertexBuffers();
	static Dictionary<uint32_t, IndexBuffer*>&  GetIndexBuffers();

	static ID3D11Device*        GetDevice() noexcept;
	static ID3D11DeviceContext* GetDeviceContext() noexcept;

	static Matrix4x4&           GetProjection() noexcept;
	static void                 SetProjection(const Matrix4x4& Projection) noexcept;
	static Matrix4x4            GetCameraView() noexcept;

	static void                 SetViewport() noexcept;
};

class Input
{
public:
	static bool IsKeyPressed(int32_t kKeycode) noexcept;
	static bool IsMouseButtonPressed(int32_t kButton) noexcept;
};


struct Vertex
{
	Float3 Position = {};
	Pixel  Color    = Colors::White;

	constexpr Vertex() = default;
	constexpr Vertex(const Float3& pos)
		: Position(pos)
	{ }
	constexpr Vertex(const Float3& pos, const Pixel& color)
		: Position(pos), Color(color)
	{ }
};


template<typename Tp>
inline void SafeRelease(Tp*& pResource) noexcept
{
	if (pResource)
	{
		pResource->Release();
		pResource = NULL;
	}
}

template<typename V>
inline VertexBuffer* Renderer3D::GetVertexBuffer(uint32_t kTypeID, const List<V>& Vertices)
{
	Dictionary<uint32_t, VertexBuffer*>& VertexBuffers = Renderer3D::GetVertexBuffers();
	if (auto it = VertexBuffers.find(kTypeID); it != VertexBuffers.end())
	{
		return it->second;
	}
	else
	{
		return (VertexBuffers[kTypeID] = new VertexBuffer(Vertices));
	}
}

uint64_t HashBytes(const void* pBytes, size_t kSize) noexcept;
uint32_t NextTypeID() noexcept;

template<typename Tp>
uint32_t GetTypeID() noexcept
{
	static const uint32_t s_TypeID = NextTypeID();
	return s_TypeID;
}

bool __Assert(bool bCondition, const char* lpFile, int kLine, const char* lpMsg = NULL, ...) noexcept;