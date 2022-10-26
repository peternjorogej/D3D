#include "Drawable.h"
#include "Image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#ifdef _MSC_VER
  #pragma comment (lib, "assimp-vc140-mt.lib")
#else
  // error "Please link assimp-vc140-mt.lib"
#endif

static const aiScene* LoadSceneFromFile(const char* lpFilepath) noexcept;

IDrawable::~IDrawable() noexcept
{
    for (IBindable*& pBindable : m_Bindables)
    {
        delete pBindable;
        pBindable = nullptr;
    }
}

void IDrawable::EmplaceIndexBuffer(uint32_t kDrawableID, const List<uint16_t>& Indices) noexcept
{
    assert(m_IndexBuffer == nullptr && "Index buffer is already set");
    m_IndexBuffer = Renderer3D::GetIndexBuffer(kDrawableID, Indices);
    m_Bindables.emplace_back(m_IndexBuffer);
}

void IDrawable::Draw() const noexcept
{
    for (IBindable* const& pBindable : m_Bindables)
    {
        pBindable->Bind();
    }
    Renderer3D::DrawIndexed(m_IndexBuffer->GetCount());
}

// PLANE
Plane::Plane()
    : IDrawableChild<Plane>()
{
    const uint32_t kID = GetTypeID<Plane>();

    const List<Vertex> Vertices =
    {
        { { -1.0f, -1.0f, -1.0f }, Colors::Yellow  },
        { {  1.0f, -1.0f, -1.0f }, Colors::Yellow  },
        { { -1.0f,  1.0f, -1.0f }, Colors::Magenta },
        { {  1.0f,  1.0f, -1.0f }, Colors::Magenta },
    };
    const List<uint16_t> Indices =
    {
        0u, 2u, 1u,
        2u, 3u, 1u,
    };
    const List<D3D11_INPUT_ELEMENT_DESC> InputElements =
    {
        { "POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u,  0u, D3D11_INPUT_PER_VERTEX_DATA, 0u },
        { "COLOR",    0u, DXGI_FORMAT_R8G8B8A8_UNORM,  0u, 12u, D3D11_INPUT_PER_VERTEX_DATA, 0u },
    };


    EmplaceBindable<VertexBuffer>(kID, Vertices);
    ID3DBlob* pBlob = EmplaceBindable<VertexShader>(kID, "Resources/Shaders/ColorShaderVS.hlsl")->GetBytecode();
    EmplaceBindable<PixelShader>(kID, "Resources/Shaders/ColorShaderPS.hlsl");
    EmplaceIndexBuffer(kID, Indices);
    EmplaceBindable<InputLayout>(kID, InputElements, pBlob);
    EmplaceBindable<PrimitiveTopology>(kID, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    EmplaceBindable<TransformConstantBuffer>(kID, this);
}

// BOX
Box::Box()
    : IDrawableChild<Box>()
{
    const uint32_t kID = GetTypeID<Box>();

    const List<Vertex> Vertices =
    {
        { { -1.0f, -1.0f, -1.0f }, Colors::Red   },
        { {  1.0f, -1.0f, -1.0f }, Colors::Red   },
        { { -1.0f,  1.0f, -1.0f }, Colors::Red   },
        { {  1.0f,  1.0f, -1.0f }, Colors::Red   },
        { { -1.0f, -1.0f,  1.0f }, Colors::Green },
        { {  1.0f, -1.0f,  1.0f }, Colors::Green },
        { { -1.0f,  1.0f,  1.0f }, Colors::Green },
        { {  1.0f,  1.0f,  1.0f }, Colors::Green },
    };
    const List<uint16_t> Indices =
    {
        0u, 2u, 1u,  2u, 3u, 1u,
        1u, 3u, 5u,  3u, 7u, 5u,
        2u, 6u, 3u,  3u, 6u, 7u,
        4u, 5u, 7u,  4u, 7u, 6u,
        0u, 4u, 2u,  2u, 4u, 6u,
        0u, 1u, 4u,  1u, 5u, 4u,
    };
    const List<D3D11_INPUT_ELEMENT_DESC> InputElements =
    {
        { "POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u,  0u, D3D11_INPUT_PER_VERTEX_DATA, 0u },
        { "COLOR",    0u, DXGI_FORMAT_R8G8B8A8_UNORM,  0u, 12u, D3D11_INPUT_PER_VERTEX_DATA, 0u },
    };


    EmplaceBindable<VertexBuffer>(kID, Vertices);
    ID3DBlob* pBlob = EmplaceBindable<VertexShader>(kID, "Resources/Shaders/ColorShaderVS.hlsl")->GetBytecode();
    EmplaceBindable<PixelShader>(kID, "Resources/Shaders/ColorShaderPS.hlsl");
    EmplaceIndexBuffer(kID, Indices);
    EmplaceBindable<InputLayout>(kID, InputElements, pBlob);
    EmplaceBindable<PrimitiveTopology>(kID, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    EmplaceBindable<TransformConstantBuffer>(kID, this);
}

// PYRAMID
Pyramid::Pyramid()
    : IDrawableChild<Pyramid>()
{
    const uint32_t kID = GetTypeID<Pyramid>();

    const List<Vertex> Vertices =
    {
        { { -1.0f, -1.0f, -1.0f }, Colors::Black },
        { {  1.0f, -1.0f, -1.0f }, Colors::Black },
        { { -1.0f,  1.0f, -1.0f }, Colors::Black },
        { {  1.0f,  1.0f, -1.0f }, Colors::Black },
        { {  0.0f,  0.0f,  2.0f }, Colors::White },
    };
    const List<uint16_t> Indices =
    {
        0u, 2u, 1u, // Floor Tri 0
        2u, 3u, 1u, // Floor Tri 1
        0u, 4u, 2u,
        0u, 1u, 4u,
        1u, 3u, 4u,
        3u, 2u, 4u,
    };
    const List<D3D11_INPUT_ELEMENT_DESC> InputElements =
    {
        { "POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u,  0u, D3D11_INPUT_PER_VERTEX_DATA, 0u },
        { "COLOR",    0u, DXGI_FORMAT_R8G8B8A8_UNORM,  0u, 12u, D3D11_INPUT_PER_VERTEX_DATA, 0u },
    };


    EmplaceBindable<VertexBuffer>(kID, Vertices);
    ID3DBlob* pBlob = EmplaceBindable<VertexShader>(kID, "Resources/Shaders/ColorShaderVS.hlsl")->GetBytecode();
    EmplaceBindable<PixelShader>(kID, "Resources/Shaders/ColorShaderPS.hlsl");
    EmplaceIndexBuffer(kID, Indices);
    EmplaceBindable<InputLayout>(kID, InputElements, pBlob);
    EmplaceBindable<PrimitiveTopology>(kID, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    EmplaceBindable<TransformConstantBuffer>(kID, this);
}

// PRISM
Prism::Prism()
    : IDrawableChild<Prism>()
{
    const uint32_t kID = GetTypeID<Prism>();

    const List<Vertex> Vertices =
    {
        { { -1.0f, -1.0f, -1.0f }, Colors::Blue },
        { {  1.0f, -1.0f, -1.0f }, Colors::Blue },
        { {  0.0f,  1.0f, -1.0f }, Colors::Blue },
        { { -1.0f, -1.0f,  1.0f }, Colors::Cyan },
        { {  1.0f, -1.0f,  1.0f }, Colors::Cyan },
        { {  0.0f,  1.0f,  1.0f }, Colors::Cyan },
    };
    const List<uint16_t> Indices =
    {
        0u, 2u, 1u,

        0u, 1u, 3u,
        1u, 4u, 3u,
        
        1u, 2u, 4u,
        2u, 5u, 4u,
        
        2u, 0u, 5u,
        0u, 3u, 5u,
        
        3u, 4u, 5u,
    };
    const List<D3D11_INPUT_ELEMENT_DESC> InputElements =
    {
        { "POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u,  0u, D3D11_INPUT_PER_VERTEX_DATA, 0u },
        { "COLOR",    0u, DXGI_FORMAT_R8G8B8A8_UNORM,  0u, 12u, D3D11_INPUT_PER_VERTEX_DATA, 0u },
    };

    EmplaceBindable<VertexBuffer>(kID, Vertices);
    ID3DBlob* pBlob = EmplaceBindable<VertexShader>(kID, "Resources/Shaders/ColorShaderVS.hlsl")->GetBytecode();
    EmplaceBindable<PixelShader>(kID, "Resources/Shaders/ColorShaderPS.hlsl");
    EmplaceIndexBuffer(kID, Indices);
    EmplaceBindable<InputLayout>(kID, InputElements, pBlob);
    EmplaceBindable<PrimitiveTopology>(kID, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    EmplaceBindable<TransformConstantBuffer>(kID, this);
}

// SURFACE
Surface::Surface()
    : IDrawableChild<Surface>()
{
    const uint32_t kID = GetTypeID<Surface>();

    struct TexturedVertex
    {
        Float3 Position = {};
        Pixel  Color    = Colors::White;
        Float2 TexCoord = {};
    };

    const List<TexturedVertex> Vertices =
    {
        { { -5.0f, -5.0f, -5.0f }, Colors::White, { 0.0f, 0.0f } },
        { {  5.0f, -5.0f, -5.0f }, Colors::White, { 0.0f, 1.0f } },
        { { -5.0f,  5.0f, -5.0f }, Colors::White, { 1.0f, 0.0f } },
        { {  5.0f,  5.0f, -5.0f }, Colors::White, { 1.0f, 1.0f } },
    };
    const List<uint16_t> Indices =
    {
        0u, 2u, 1u,
        2u, 3u, 1u,
    };
    const List<D3D11_INPUT_ELEMENT_DESC> InputElements =
    {
        { "POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u,  0u, D3D11_INPUT_PER_VERTEX_DATA, 0u },
        { "COLOR",    0u, DXGI_FORMAT_R8G8B8A8_UNORM,  0u, 12u, D3D11_INPUT_PER_VERTEX_DATA, 0u },
        { "TEXCOORD", 0u, DXGI_FORMAT_R32G32_FLOAT,    0u, 16u, D3D11_INPUT_PER_VERTEX_DATA, 0u },
    };
    const Image i = Image("Resources/Images/NjoroLogo.png");


    EmplaceBindable<VertexBuffer>(kID, Vertices);
    ID3DBlob* pBlob = EmplaceBindable<VertexShader>(kID, "Resources/Shaders/TextureShaderVS.hlsl")->GetBytecode();
    EmplaceBindable<PixelShader>(kID, "Resources/Shaders/TextureShaderPS.hlsl");
    EmplaceIndexBuffer(kID, Indices);
    EmplaceBindable<InputLayout>(kID, InputElements, pBlob);
    EmplaceBindable<PrimitiveTopology>(kID, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    EmplaceBindable<TransformConstantBuffer>(kID, this);

    EmplaceSharedBindable<Texture>(kID, i);
    EmplaceSharedBindable<Sampler>(kID);
}

// MESH
Mesh::Mesh(const char* lpFilepath, float Scale)
    : IDrawableChild<Mesh>()
{
    struct MeshVertex
    {
        Float3 Position = {};
        Float3 Normal   = {};

        constexpr MeshVertex() = default;
        constexpr MeshVertex(const Float3& pos, const Float3& n)
            : Position(pos), Normal(n)
        { }
    };

    const uint32_t kID = GetTypeID<Mesh>();

    const aiScene* pModel = LoadSceneFromFile(lpFilepath);
    const aiMesh*  pMesh  = pModel->mMeshes[0];

    List<MeshVertex> Vertices = {};
    Vertices.reserve(pMesh->mNumVertices);
    for (size_t k = 0; k < pMesh->mNumVertices; k++)
    {
        const Float3 position = *reinterpret_cast<const Float3*>(&pMesh->mVertices[k]) * Scale;
        const Float3 normal   = *reinterpret_cast<const Float3*>(&pMesh->mNormals[k]);
        Vertices.emplace_back(position, normal);
    }
    
    List<uint16_t> Indices = {};
    Indices.reserve(pMesh->mNumFaces * 3u);
    for (size_t k = 0; k < pMesh->mNumFaces; k++)
    {
        const aiFace& face = pMesh->mFaces[k];
        assert(face.mNumIndices == 3u);
        Indices.emplace_back(face.mIndices[0]);
        Indices.emplace_back(face.mIndices[1]);
        Indices.emplace_back(face.mIndices[2]);
    }

    const List<D3D11_INPUT_ELEMENT_DESC> InputElements =
    {
        { "POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u,  0u, D3D11_INPUT_PER_VERTEX_DATA, 0u },
        { "NORMAL",   0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 12u, D3D11_INPUT_PER_VERTEX_DATA, 0u },
    };

    EmplaceBindable<VertexBuffer>(kID, Vertices);
    ID3DBlob* pBlob = EmplaceBindable<VertexShader>(kID, "Resources/Shaders/PhongShaderVS.hlsl")->GetBytecode();
    EmplaceBindable<PixelShader>(kID, "Resources/Shaders/PhongShaderPS.hlsl");
    EmplaceIndexBuffer(kID, Indices);
    EmplaceBindable<InputLayout>(kID, InputElements, pBlob);
    EmplaceBindable<PrimitiveTopology>(kID, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    EmplaceBindable<TransformConstantBuffer>(kID, this);
}

// SOLID SPHERE
SolidSphere::SolidSphere(float Radius)
    : IDrawableChild<SolidSphere>()
{
    const uint32_t kID = GetTypeID<SolidSphere>();

    const aiScene* pModel = LoadSceneFromFile("Resources/Models/Sphere.obj");
    const aiMesh* pMesh = pModel->mMeshes[0];

    List<Vertex> Vertices = {};
    Vertices.reserve(pMesh->mNumVertices);
    for (size_t k = 0; k < pMesh->mNumVertices; k++)
    {
        const Float3 pos = *reinterpret_cast<const Float3*>(&pMesh->mVertices[k]);
        Vertices.emplace_back(pos, Colors::White);
    }

    List<uint16_t> Indices = {};
    Indices.reserve(pMesh->mNumFaces * 3u);
    for (size_t k = 0; k < pMesh->mNumFaces; k++)
    {
        const aiFace& face = pMesh->mFaces[k];
        assert(face.mNumIndices == 3u);
        Indices.emplace_back(face.mIndices[0]);
        Indices.emplace_back(face.mIndices[1]);
        Indices.emplace_back(face.mIndices[2]);
    }

    const List<D3D11_INPUT_ELEMENT_DESC> InputElements =
    {
        { "POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u,  0u, D3D11_INPUT_PER_VERTEX_DATA, 0u },
        { "COLOR",    0u, DXGI_FORMAT_R8G8B8A8_UNORM,  0u, 12u, D3D11_INPUT_PER_VERTEX_DATA, 0u },
    };

    EmplaceBindable<VertexBuffer>(kID, Vertices);
    ID3DBlob* pBlob = EmplaceBindable<VertexShader>(kID, "Resources/Shaders/ColorShaderVS.hlsl")->GetBytecode();
    EmplaceBindable<PixelShader>(kID, "Resources/Shaders/ColorShaderPS.hlsl");
    EmplaceIndexBuffer(kID, Indices);
    EmplaceBindable<InputLayout>(kID, InputElements, pBlob);
    EmplaceBindable<PrimitiveTopology>(kID, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    EmplaceBindable<TransformConstantBuffer>(kID, this);
}


static Dictionary<uint64_t, const aiScene*> s_MeshStorage = {};

const aiScene* LoadSceneFromFile(const char* lpFilepath) noexcept
{
    static Assimp::Importer Imp;

    if (!std::filesystem::exists(lpFilepath))
    {
        GfxError(false, "FileNotFoundException: '%s'", lpFilepath);
        return nullptr;
    }
    const uint64_t kHash = HashBytes(lpFilepath, strlen(lpFilepath));
    
    if (auto it = s_MeshStorage.find(kHash); it != s_MeshStorage.end())
    {
        return it->second;
    }
    else
    {
        const aiScene* pScene = Imp.ReadFile(lpFilepath, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
        return (s_MeshStorage[kHash] = pScene);
    }
}
