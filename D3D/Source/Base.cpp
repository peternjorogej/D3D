#include "Base.h"

#pragma warning (disable: 4267)

#include "Bindable.h"
#include "Drawable.h"
#include "Camera.h"
#include "Light.h"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_dx11.h>
#include <imgui/backends/imgui_impl_win32.h>

#ifndef NDEBUG
  #define GfxErrorH(h)      __AssertHresult((h), __FILE__, __LINE__)
#else
  #define GfxErrorH(x, ...) bool(0)
#endif // !NDEBUG

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace dxm = DirectX;

bool __Assert(bool bCondition, const char* lpFile, int kLine, const char* lpMsg, ...) noexcept
{
    if (bCondition)
    { }
    else
    {
        static char lpFmt[1024]  = { 0 };
        static char lpText[2048] = { 0 };
        
        ZeroMemory(lpFmt, sizeof(lpFmt));
        _snprintf_s(lpFmt, 2048, "Assertion Failure:\n  File: %s\n  Line: %d\n  Reason: %s", lpFile, kLine, lpMsg ? lpMsg : "[None]");
        
        ZeroMemory(lpText, sizeof(lpText));
        va_list vArgs;
        va_start(vArgs, lpMsg);
        (void)vsnprintf_s(lpText, 2048ull, lpFmt, vArgs);
        va_end(vArgs);

        MessageBoxA(NULL, lpText, "Error", MB_ICONERROR);

    #ifndef NDEBUG
        __debugbreak();
    #else
        exit(-1);
    #endif // !NDEBUG
    }
    return bCondition;
}

static bool __AssertHresult(HRESULT hResult, const char* lpFile, int kLine) noexcept
{
    const bool bFailed = FAILED(hResult);
    if (bFailed)
    {
        static char lpText[2048] = { 0 };

        char* lpMsg = NULL;
        DWORD dwFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
        DWORD dwMsgLength = FormatMessageA(dwFlags, NULL, hResult, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&lpMsg, 0, NULL);

        if (dwMsgLength == 0u)
        {
            return false;
        }

        ZeroMemory(lpText, sizeof(lpText));
        _snprintf_s(lpText, 2048, "Assertion Failure:\n  File: %s\n  Line: %d\n  Reason: %s", lpFile, kLine, lpMsg);
        LocalFree(lpMsg);

        MessageBoxA(NULL, lpText, "Error", MB_ICONERROR);

    #ifndef NDEBUG
        __debugbreak();
    #else
        exit(-1);
    #endif // !NDEBUG
    }

    return bFailed;
}


struct RendererContext
{
    // Window Data
    HINSTANCE               pInstance      = nullptr;
    HWND                    pWnd           = nullptr;
    uint32_t                Width          = 0;
    uint32_t                Height         = 0;
    // Core Renderer Data
    IDXGISwapChain*         pSwapChain        = nullptr;
    ID3D11Device*           pDevice           = nullptr;
    ID3D11DeviceContext*    pDeviceContext    = nullptr;
    ID3D11RenderTargetView* pRenderTargetView = nullptr;
    ID3D11DepthStencilView* pDepthStencilView = nullptr;

    Dictionary<String, VertexShader*>   VertexShaders   = {};
    Dictionary<String, PixelShader*>    PixelShaders    = {};
    Dictionary<uint32_t, VertexBuffer*> VertexBuffers    = {};
    Dictionary<uint32_t, IndexBuffer*>  IndexBuffers     = {};

    // User Runtime Renderer Data
    Matrix4x4                           Projection      = DirectX::XMMatrixIdentity();
    SceneCamera                         Camera          = {};
    float                               Frametime       = 0.0f;
    List<IDrawable*>                    Drawables       = {};
    PointLight*                         Light           = nullptr;

    bool                                bMouse[7]       = {};
    bool                                bKeys[256]      = {};
};

static constexpr const wchar_t* s_ClassName = L"D3D";

static RendererContext s_Context = {};


static float            Clock() noexcept;
static bool             InitializeD3D();
static void             ShutdownD3D();
static void             BeginFrame(const Float4& ClearColor = Float4(1.0f));
static void             RenderFrame(float dt);
static void             EndFrame();
static void             DrawTestTriangle() noexcept;
static LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT kMsg, WPARAM wParam, LPARAM lParam);

// RENDERER
bool Renderer3D::Initialize(HINSTANCE hInstance)
{
    if (!hInstance)
        return GfxError(false, "hInstance is NULL");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    WNDCLASSEXW wc = {};
    ZeroMemory(&wc, sizeof(wc));
    wc.cbSize        = sizeof(wc);
    wc.hbrBackground = (HBRUSH)COLOR_BACKGROUND;
    wc.hCursor       = LoadCursorW(hInstance, IDC_ARROW);
    wc.hIcon         = LoadIconW(hInstance, IDI_APPLICATION);
    wc.hIconSm       = LoadIconW(hInstance, IDI_APPLICATION);
    wc.hInstance     = hInstance;
    wc.lpfnWndProc   = WindowProcedure;
    wc.lpszClassName = s_ClassName;
    
    if (!RegisterClassExW(&wc))
        return GfxError(false, "Failed to register class");

    s_Context.pInstance = hInstance;

    DWORD kFlags = 0u;

    s_Context.Width = 1248u;
    s_Context.Height = 702u;
    RECT r = { 0, 0, LONG(s_Context.Width), LONG(s_Context.Height) };
    AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, FALSE);

    s_Context.pWnd = CreateWindowExW(
        0u, wc.lpszClassName, L"D3D", WS_OVERLAPPEDWINDOW,
        30, 0, r.right - r.left, r.bottom - r.top,
        NULL, NULL, hInstance, NULL
    );
    if (!s_Context.pWnd)
        return GfxError(false, "CreateWindowExW(...) failed");

    ShowWindow(s_Context.pWnd, SW_SHOW);
    UpdateWindow(s_Context.pWnd);

    {
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 2.5f;
        style.FrameRounding  = 2.5f;
    }

    ImGui_ImplWin32_Init(s_Context.pWnd);

    bool bIsD3DInitialized = InitializeD3D();
    if (!bIsD3DInitialized)
    {
        return false;
    }
    ImGui_ImplDX11_Init(s_Context.pDevice, s_Context.pDeviceContext);

    return true;
}

void Renderer3D::Shutdown()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();

    ShutdownD3D();
    UnregisterClassW(s_ClassName, s_Context.pInstance);
    DestroyWindow(s_Context.pWnd);

    ImGui::DestroyContext();
}

void Renderer3D::Run()
{
    MSG  Msg = {};
    while (true)
    {
        while (PeekMessageW(&Msg, NULL, 0, 0, PM_REMOVE) > 0)
        {
            TranslateMessage(&Msg);
            DispatchMessageW(&Msg);
        }

        if (Msg.message == WM_QUIT)
        {
            break;
        }

        float Now = Clock();
        RenderFrame(s_Context.Frametime);
        s_Context.Frametime = Clock() - Now;
    };
}

void Renderer3D::DrawIndexed(uint32_t kIndexCount) noexcept
{
    s_Context.pDeviceContext->DrawIndexed(kIndexCount, 0u, 0);
}

VertexShader* Renderer3D::GetVertexShader(const std::filesystem::path& Filepath, const char* lpEntryPoint) noexcept
{
    String Name = Filepath.string();
    if (size_t kIndex = Name.find_last_of('/', 0u); kIndex != String::npos)
    {
        Name = Name.substr(kIndex);
    }

    if (auto it = s_Context.VertexShaders.find(Name); it != s_Context.VertexShaders.end())
    {
        return it->second;
    }
    else
    {
        VertexShader* pVS = new VertexShader(Filepath, lpEntryPoint);
        return (s_Context.VertexShaders[Name] = pVS);
    }
}

PixelShader* Renderer3D::GetPixelShader(const std::filesystem::path& Filepath, const char* lpEntryPoint) noexcept
{
    String Name = Filepath.string();
    if (size_t kIndex = Name.find_last_of('/', 0u); kIndex != String::npos)
    {
        Name = Name.substr(kIndex);
    }

    if (auto it = s_Context.PixelShaders.find(Name); it != s_Context.PixelShaders.end())
    {
        return it->second;
    }
    else
    {
        PixelShader* pPS = new PixelShader(Filepath, lpEntryPoint);
        return (s_Context.PixelShaders[Name] = pPS);
    }
}

IndexBuffer* Renderer3D::GetIndexBuffer(uint32_t kTypeID, const List<uint16_t>& Indices)
{
    Dictionary<uint32_t, IndexBuffer*>& IndexBuffers = s_Context.IndexBuffers;
    if (auto it = IndexBuffers.find(kTypeID); it != IndexBuffers.end())
    {
        return it->second;
    }
    else
    {
        return (IndexBuffers[kTypeID] = new IndexBuffer(Indices));
    }
}

Dictionary<uint32_t, VertexBuffer*>& Renderer3D::GetVertexBuffers()
{
    return s_Context.VertexBuffers;
}

Dictionary<uint32_t, IndexBuffer*>& Renderer3D::GetIndexBuffers()
{
    return s_Context.IndexBuffers;
}

ID3D11Device* Renderer3D::GetDevice() noexcept
{
    return s_Context.pDevice;
}

ID3D11DeviceContext* Renderer3D::GetDeviceContext() noexcept
{
    return s_Context.pDeviceContext;
}

Matrix4x4& Renderer3D::GetProjection() noexcept
{
    return s_Context.Projection;
}

void Renderer3D::SetProjection(const Matrix4x4& Projection) noexcept
{
    s_Context.Projection = Projection;
}

Matrix4x4 Renderer3D::GetCameraView() noexcept
{
    return s_Context.Camera.GetMatrix();
}

void Renderer3D::SetViewport() noexcept
{
    D3D11_VIEWPORT vp = {};
    ZeroMemory(&vp, sizeof(vp));
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    vp.Width    = float(s_Context.Width);
    vp.Height   = float(s_Context.Height);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    
    s_Context.pDeviceContext->RSSetViewports(1u, &vp);
    
    const float Width  = 1.0f;
    const float Height = vp.Height / vp.Width;
    SetProjection(DirectX::XMMatrixPerspectiveLH(Width, Height, 0.5f, 100.0f));
}

// INPUT
bool Input::IsKeyPressed(int32_t kKeycode) noexcept
{
    return s_Context.bKeys[static_cast<uint8_t>(kKeycode)];
}

bool Input::IsMouseButtonPressed(int32_t kButton) noexcept
{
    return s_Context.bMouse[static_cast<uint8_t>(kButton)];
}


uint64_t HashBytes(const void* pBytes, size_t kSize) noexcept
{
#ifdef _MSC_VER
    return std::_Hash_array_representation(static_cast<const uint8_t*>(pBytes), kSize);
#else
    GfxError(false, "Not hash function specified");
    return uint64_t();
#endif // _MSC_VER
}

uint32_t NextTypeID() noexcept
{
    static uint32_t s_TypeID = 0ul;
    return ++s_TypeID;
}

float Clock() noexcept
{
    double now = double(clock()) / double(CLOCKS_PER_SEC);
    return float(now); // Is in seconds
}

bool InitializeD3D()
{
    HRESULT hResult = HRESULT(0);
    // Create Device & SwapChain
    DXGI_SWAP_CHAIN_DESC scd = {};
    ZeroMemory(&scd, sizeof(scd));
    scd.BufferCount                        = 1u;
    scd.BufferDesc.Format                  = DXGI_FORMAT_B8G8R8A8_UNORM;
    scd.BufferDesc.Width                   = 0u;
    scd.BufferDesc.Height                  = 0u;
    scd.BufferDesc.RefreshRate.Numerator   = 0u;
    scd.BufferDesc.RefreshRate.Denominator = 0u;
    scd.BufferDesc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;
    scd.BufferDesc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    scd.SampleDesc.Count                   = 1u;
    scd.SampleDesc.Quality                 = 0u;
    scd.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow                       = s_Context.pWnd;
    scd.SwapEffect                         = DXGI_SWAP_EFFECT_DISCARD;
    scd.Windowed                           = TRUE;
    scd.Flags                              = 0u;

    UINT kFlags = 0u;
#ifndef NDEBUG
    kFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // !NDEBUG

    hResult = D3D11CreateDeviceAndSwapChain(
        NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, kFlags,
        NULL, 0u, D3D11_SDK_VERSION,
        &scd, &s_Context.pSwapChain, &s_Context.pDevice, NULL, &s_Context.pDeviceContext
    );
    if (FAILED(hResult))
        return GfxErrorH(hResult);
    
    {
        // Create Render Target View
        ID3D11Texture2D* pBackBuffer = NULL;
        hResult = s_Context.pSwapChain->GetBuffer(0u, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
        if (FAILED(hResult))
            return GfxErrorH(hResult);

        hResult = s_Context.pDevice->CreateRenderTargetView(pBackBuffer, NULL, &s_Context.pRenderTargetView);
        SafeRelease(pBackBuffer);
        if (FAILED(hResult))
            return GfxErrorH(hResult);
    }
    {
        // Create Depth-Stencil View
        D3D11_DEPTH_STENCIL_DESC dsd = {};
        ZeroMemory(&dsd, sizeof(dsd));
        dsd.DepthEnable    = TRUE;
        dsd.DepthFunc      = D3D11_COMPARISON_LESS;
        dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

        ID3D11DepthStencilState* pDepthStencilState = NULL;
        hResult = s_Context.pDevice->CreateDepthStencilState(&dsd, &pDepthStencilState);
        if (FAILED(hResult))
            return GfxErrorH(hResult);

        ID3D11Texture2D* pDepthBuffer = NULL;
        D3D11_TEXTURE2D_DESC td = {};
        ZeroMemory(&td, sizeof(td));
        td.Width              = s_Context.Width;
        td.Height             = s_Context.Height;
        td.MipLevels          = 1u;
        td.ArraySize          = 1u;
        td.Format             = DXGI_FORMAT_D32_FLOAT;
        td.SampleDesc.Count   = 1u;
        td.SampleDesc.Quality = 0u;
        td.Usage              = D3D11_USAGE_DEFAULT;
        td.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
        hResult = s_Context.pDevice->CreateTexture2D(&td, NULL, &pDepthBuffer);
        if (FAILED(hResult))
            return GfxErrorH(hResult);

        D3D11_DEPTH_STENCIL_VIEW_DESC dsvd = {};
        ZeroMemory(&dsvd, sizeof(dsvd));
        dsvd.Format             = DXGI_FORMAT_D32_FLOAT;
        dsvd.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
        dsvd.Texture2D.MipSlice = 0u;
        hResult = s_Context.pDevice->CreateDepthStencilView(pDepthBuffer, &dsvd, &s_Context.pDepthStencilView);
        if (FAILED(hResult))
            return GfxErrorH(hResult);
    }
    {
        // Set Viewport
        Renderer3D::SetViewport();
    }
    {
        // Blending
        ID3D11BlendState* pBlendState = NULL;
        D3D11_BLEND_DESC bd = {};
        ZeroMemory(&bd, sizeof(bd));
        bd.AlphaToCoverageEnable                 = FALSE;
        bd.IndependentBlendEnable                = FALSE;
        bd.RenderTarget[0].BlendEnable           = TRUE;
        bd.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;
        bd.RenderTarget[0].DestBlend             = D3D11_BLEND_INV_SRC_ALPHA;
        bd.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
        bd.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_INV_SRC_ALPHA;
        bd.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
        bd.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
        bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        hResult = s_Context.pDevice->CreateBlendState(&bd, &pBlendState);
        if (FAILED(hResult))
            return GfxErrorH(hResult);

        float pBlendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        s_Context.pDeviceContext->OMSetBlendState(pBlendState, pBlendFactor, 0xFFFFFFFF);
    }

    s_Context.pDeviceContext->OMSetRenderTargets(1u, &s_Context.pRenderTargetView, s_Context.pDepthStencilView);

    Renderer3D::SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 9.0f / 16.0f, 0.5f, 1000.0f));

    DrawTestTriangle();

    return true;
}

void ShutdownD3D()
{
    ImGui_ImplDX11_Shutdown();

    for (auto&[Name, pShader] : s_Context.VertexShaders)
    {
        delete pShader;
        pShader = nullptr;
    }
    for (auto&[Name, pShader] : s_Context.PixelShaders)
    {
        delete pShader;
        pShader = nullptr;
    }
    for (auto&[kID, pBuffer] : s_Context.VertexBuffers)
    {
        delete pBuffer;
        pBuffer = nullptr;
    }
    for (auto&[kID, pBuffer] : s_Context.IndexBuffers)
    {
        delete pBuffer;
        pBuffer = nullptr;
    }

    SafeRelease(s_Context.pDepthStencilView);
    SafeRelease(s_Context.pRenderTargetView);
    SafeRelease(s_Context.pDeviceContext);
    SafeRelease(s_Context.pDevice);
    SafeRelease(s_Context.pSwapChain);
}

void BeginFrame(const Float4& ClearColor)
{
    s_Context.pDeviceContext->ClearRenderTargetView(s_Context.pRenderTargetView, &ClearColor.X);
    s_Context.pDeviceContext->ClearDepthStencilView(s_Context.pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0u);

    // ImGui
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void RenderFrame(float dt)
{
    static constexpr Float4 BGColor = Float4(0.2f, 0.2f, 0.2f, 1.0f);
    BeginFrame(BGColor);

    static float s_SpeedFactor = 0.21f;
    static float s_WindowAlpha = 0.35f;
    static bool  s_Paused = false;

    // Input
    if (Input::IsKeyPressed(VK_SPACE))
    {
        s_Paused = !s_Paused;
    }

    // 3D Rendering Goes Here...
    s_Context.Light->Bind();
    for (IDrawable* const& pDrawable : s_Context.Drawables)
    {
        pDrawable->Update(dt * s_SpeedFactor);
        pDrawable->Draw();
    }
    s_Context.Light->Draw();

    static float s_TmpSpeed = s_SpeedFactor;
    if (ImGui::Begin("Settings"))
    {
        ImGui::SliderFloat("Simulation Speed", &s_TmpSpeed, 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat("Window Transparency", &s_WindowAlpha, 0.0f, 1.0f, "%.2f");
    }
    ImGui::End();

    s_SpeedFactor = s_Paused ? 0.0f : s_TmpSpeed;
    ImGui::GetStyle().Colors[ImGuiCol_WindowBg].w = s_WindowAlpha;

    s_Context.Camera.SpawnControlWindow();
    s_Context.Light->SpawnControlWindow();

    EndFrame();
}

void EndFrame()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    s_Context.pSwapChain->Present(1u, 0u);

    memset(s_Context.bKeys,  0, sizeof(s_Context.bKeys));
    memset(s_Context.bMouse, 0, sizeof(s_Context.bMouse));
}


void DrawTestTriangle() noexcept
{
    enum class ShapeKind
    {
        Plane,
        Box,
        Pyramid,
        Prism,
        Surface,
        Mesh,
        COUNT
    };
    constexpr uint64_t kShapeCount = uint64_t(ShapeKind::COUNT);

    for (size_t k = 0u; k < kShapeCount * 25u; k++)
    {
        /*const ShapeKind Shape = ShapeKind(Random::UInt(kShapeCount));
        switch (Shape)
        {
            case ShapeKind::Box:
            case ShapeKind::Plane:
            case ShapeKind::Pyramid:
            case ShapeKind::Prism:
            case ShapeKind::Surface:
                break;
            case ShapeKind::Mesh:*/
                s_Context.Drawables.emplace_back(new Mesh{ "Resources/Models/Suzzane.obj", 1.0f });
                /*break;
            default:
                assert(false && "Error: Invalid ShapeKind");
                break;
        }*/
    }

    s_Context.Light = new PointLight{ 5.0f };
}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT kMsg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, kMsg, wParam, lParam))
    {
        return TRUE;
    }
    ImGuiIO& io = ImGui::GetIO();

    switch (kMsg)
    {
        case WM_CLOSE:
        case WM_DESTROY:
        case WM_QUIT:
            PostQuitMessage(0);
            return 0;

        case WM_SIZE:
        {
            RECT r = {};
            GetClientRect(hWnd, &r);
            s_Context.Width  = r.right  - r.left;
            s_Context.Height = r.bottom - r.top;

            if (s_Context.pDeviceContext != nullptr && s_Context.Width > 0u && s_Context.Height > 0u)
            {
                Renderer3D::SetViewport();
            }
            
            break;
        }

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            if (io.WantCaptureKeyboard)
            {
                break;
            }
            if (!(lParam & 0x40000000))
            {
                s_Context.bKeys[static_cast<uint8_t>(wParam)] = true;
            }
            break;
        case WM_KEYUP:
        case WM_SYSKEYUP:
            if (io.WantCaptureKeyboard)
            {
                break;
            }
            s_Context.bKeys[static_cast<uint8_t>(wParam)] = false;
            break;

        default:
            break;
    }

    return DefWindowProcW(hWnd, kMsg, wParam, lParam);
}
