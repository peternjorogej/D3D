#pragma once

#include "Maths.h"

class Image
{
public:
	Image(uint32_t Width, uint32_t Height, const Pixel& Color = Colors::Blank);
	Image(const char* lpFilepath);
	~Image() noexcept;

	Image Copy();
	void  Save(const char* lpFilepath);

	Pixel*       GetBufferPointer() noexcept;
	const Pixel* GetBufferPointer() const noexcept;
	size_t       GetBufferSize() const noexcept;
	uint32_t     GetPitch() const noexcept;
	uint32_t     GetWidth() const noexcept;
	uint32_t     GetHeight() const noexcept;

private:
	Image(uint32_t Width, uint32_t Height, const Pixel* const pPixels);

	Image(const Image&) = delete;
	Image& operator=(const Image&) = delete;

private:
	Pixel*   m_Pixels = nullptr;
	uint32_t m_Width  = 0u;
	uint32_t m_Height = 0u;
};