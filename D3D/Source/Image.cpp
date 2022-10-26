#include "Image.h"
#include "Maths.h"
#include "Base.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <memory.h>

Image::Image(uint32_t Width, uint32_t Height, const Pixel& Color)
    : m_Pixels(nullptr), m_Width(Width), m_Height(Height)
{
    const size_t kSize = size_t(m_Width) * size_t(m_Height);
    m_Pixels = new Pixel[kSize]{};

    if (Color == Colors::Blank)
    {
        for (size_t k = 0; k < kSize; k++)
        {
            m_Pixels[k] = Random::PixelA();
        }
    }
    else
    {
        const int32_t& kColor = *reinterpret_cast<const int32_t*>(&Color);
        memset(m_Pixels, kColor, GetBufferSize());
    }
}

Image::Image(const char* lpFilepath)
    : m_Width(0u), m_Height(0u)
{
    int32_t kWidth    = 0;
    int32_t kHeight   = 0;
    int32_t kChannels = 0;
    stbi_uc* pPixels = stbi_load(lpFilepath, &kWidth, &kHeight, &kChannels, 4);
    if (pPixels == nullptr || kWidth < 1 || kHeight < 1)
    {
        assert(false && "Failed to load image");
        return;
    }

    m_Width  = uint32_t(kWidth);
    m_Height = uint32_t(kHeight);

    const size_t kSize = size_t(m_Width) * size_t(m_Height);
    m_Pixels = new Pixel[kSize]{};

    memcpy(m_Pixels, pPixels, GetBufferSize());
    stbi_image_free(pPixels);
    pPixels = nullptr;
}

Image::Image(uint32_t Width, uint32_t Height, const Pixel* const pPixels)
    : m_Width(Width), m_Height(Height)
{
    const size_t kSize = size_t(m_Width) * size_t(m_Height);
    m_Pixels = new Pixel[kSize]{};

    memcpy(m_Pixels, pPixels, GetBufferSize());
}

Image::~Image() noexcept
{
    delete m_Pixels;
    m_Pixels = nullptr;

    m_Width = m_Height = 0u;
}

Image Image::Copy()
{
    return Image(m_Width, m_Height, m_Pixels);
}

void Image::Save(const char* lpFilepath)
{
    assert(false && "NotImplementedException");
}

Pixel* Image::GetBufferPointer() noexcept
{
    return m_Pixels;
}

const Pixel* Image::GetBufferPointer() const noexcept
{
    return m_Pixels;
}

size_t Image::GetBufferSize() const noexcept
{
    return size_t(m_Width) * size_t(m_Height) * sizeof(Pixel);
}

uint32_t Image::GetPitch() const noexcept
{
    return m_Width * sizeof(Pixel);
}

uint32_t Image::GetWidth() const noexcept
{
    return m_Width;
}

uint32_t Image::GetHeight() const noexcept
{
    return m_Height;
}
