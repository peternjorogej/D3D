#pragma once

#include <stdint.h>
#include <math.h>

class Constants
{
public:
	static constexpr double E    = 2.71828182845904523536f;
	static constexpr double Pi   = 3.14159265358979323846f;
	static constexpr double Tau  = 2.0f * Pi;
};

struct Float2
{
	float X = 0.0f;
	float Y = 0.0f;

	constexpr Float2() = default;
	constexpr Float2(float t)          : X(t), Y(t) { }
	constexpr Float2(float x, float y) : X(x), Y(y) { }
	constexpr Float2(const Float2&) = default;

	Float2& operator=(const Float2&) = default;
};

struct Float3
{
	float X = 0.0f;
	float Y = 0.0f;
	float Z = 0.0f;

	constexpr Float3() = default;
	constexpr Float3(float t)                   : X(t),   Y(t),   Z(t) { }
	constexpr Float3(float x, float y, float z) : X(x),   Y(y),   Z(z) { }
	constexpr Float3(const Float2& u, float z)  : X(u.X), Y(u.Y), Z(z) { }
	constexpr Float3(const Float3&) = default;

	Float3& operator=(const Float3&) = default;

	operator Float2() const noexcept;
};

struct Float4
{
	float X = 0.0f;
	float Y = 0.0f;
	float Z = 0.0f;
	float W = 0.0f;

	constexpr Float4() = default;
	constexpr Float4(float t)                            : X(t),   Y(t),   Z(t),   W(t)    { }
	constexpr Float4(float x, float y, float z)          : X(x),   Y(y),   Z(z),   W(1.0f) { }
	constexpr Float4(float x, float y, float z, float w) : X(x),   Y(y),   Z(z),   W(w)    { }
	constexpr Float4(const Float2& u, const Float2& v)   : X(u.X), Y(u.Y), Z(v.X), W(v.Y)  { }
	constexpr Float4(const Float3& u, float w)           : X(u.X), Y(u.Y), Z(u.Z), W(w)    { }
	constexpr Float4(const Float4&) = default;

	Float4& operator=(const Float4&) = default;

	operator Float2() const noexcept;
	operator Float3() const noexcept;
};

struct Float4x4
{
	union
	{
		float  Matrix[4][4];
		Float4 Rows[4];
	};

	Float4x4();
	Float4x4(float t);
	Float4x4(const float m[4][4]);
	Float4x4(const float m[16]);
	constexpr Float4x4(const Float4x4&) = default;
	
	Float4x4& operator=(const Float4x4&) = default;

	static Float4x4 Translate(const Float3& v) noexcept;
	static Float4x4 Rotate(const Float3& v) noexcept;
	static Float4x4 Scale(const Float3& v) noexcept;
	static Float4x4 Orthographic(float Left, float Right, float Top, float Bottom, float Near=-1.0f, float Far=1.0f) noexcept;
	static Float4x4 Perspective(float FoV, float Aspect, float Near=0.1f, float Far=1000.0f) noexcept;

	Float4x4     Transpose()  noexcept;
	float*       Data()       noexcept;
	const float* Data() const noexcept;

	float*       operator[](size_t kIndex)       noexcept { return Matrix[kIndex]; }
	const float* operator[](size_t kIndex) const noexcept { return Matrix[kIndex]; }
	Float4* begin() noexcept { return Rows + 0u; }
	Float4* end()   noexcept { return Rows + 4u; }
	const Float4* begin() const noexcept { return Rows + 0u; }
	const Float4* end()   const noexcept { return Rows + 4u; }
};


struct Pixel
{
	uint8_t Red   = 255u;
	uint8_t Green = 255u;
	uint8_t Blue  = 255u;
	uint8_t Alpha = 255u;

	constexpr Pixel() = default;
	constexpr Pixel(uint8_t x)                                  : Red(x), Green(x), Blue(x), Alpha(255u) { }
	constexpr Pixel(uint8_t r, uint8_t g, uint8_t b)            : Red(r), Green(g), Blue(b), Alpha(255u) { }
	constexpr Pixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : Red(r), Green(g), Blue(b), Alpha(a)    { }

	Float4    ToFloat4() const noexcept;
};

namespace Colors
{

	constexpr Pixel Blank     = Pixel(0u, 0u, 0u, 0u);
	constexpr Pixel White     = Pixel(255u, 255u, 255u);
	constexpr Pixel Black     = Pixel(  0u,   0u,   0u);
	constexpr Pixel Red       = Pixel(255u,   0u,   0u);
	constexpr Pixel Green     = Pixel(  0u, 255u,   0u);
	constexpr Pixel Blue      = Pixel(  0u,   0u, 255u);
	constexpr Pixel Yellow    = Pixel(255u, 255u,   0u);
	constexpr Pixel Magenta   = Pixel(255u,   0u, 255u);
	constexpr Pixel Cyan      = Pixel(0u,   255u, 255u);
	constexpr Pixel LightGray = Pixel(160u, 160u, 160u);
	constexpr Pixel DarkGray  = Pixel( 60u,  60u,  60u);

}

class Random
{
public:
	static float    Float() noexcept;                           // Range (0, 1)
	static float    Float(float First, float Last) noexcept;    // Range (First, Last)
	static double   Double() noexcept;                          // Range (0, 1)
	static double   Double(double First, double Last) noexcept; // Range (First, Last)
	static int64_t  Int() noexcept;                             // Range (0, dist_max)
	static int64_t  Int(int64_t kMax) noexcept;                 // Range (0, kMax)
	static uint64_t UInt() noexcept;                            // Range (0, dist_max)
	static uint64_t UInt(uint64_t kMax) noexcept;               // Range (0, kMax)

	static Float2   Float2F() noexcept;
	static Float2   Float2F(float First, float Last) noexcept;
	static Float2   Float2F(const Float2& First, const Float2& Last) noexcept;
	static Float3   Float3F() noexcept;
	static Float3   Float3F(float First, float Last) noexcept;
	static Float3   Float3F(const Float3& First, const Float3& Last) noexcept;
	static Float4   Float4F() noexcept;
	static Float4   Float4F(float First, float Last) noexcept;
	static Float4   Float4F(const Float4& First, const Float4& Last) noexcept;
	static Pixel    PixelA(bool bRandomAlpha = false) noexcept;
};

Pixel  PixelI(uint32_t kColor) noexcept;
Pixel  PixelF(float r, float g, float b, float a = 1.0f) noexcept;

bool   operator==(const Pixel& p, const Pixel& q) noexcept;
bool   operator!=(const Pixel& p, const Pixel& q) noexcept;

Float2 operator-(const Float2& u) noexcept;
Float2 operator+(const Float2& u, const Float2& v) noexcept;
Float2 operator-(const Float2& u, const Float2& v) noexcept;
Float2 operator*(const Float2& u, const Float2& v) noexcept;
Float2 operator/(const Float2& u, const Float2& v) noexcept;

Float3 operator-(const Float3& u) noexcept;
Float3 operator+(const Float3& u, const Float3& v) noexcept;
Float3 operator-(const Float3& u, const Float3& v) noexcept;
Float3 operator*(const Float3& u, const Float3& v) noexcept;
Float3 operator/(const Float3& u, const Float3& v) noexcept;

Float4 operator-(const Float4& u) noexcept;
Float4 operator+(const Float4& u, const Float4& v) noexcept;
Float4 operator-(const Float4& u, const Float4& v) noexcept;
Float4 operator*(const Float4& u, const Float4& v) noexcept;
Float4 operator/(const Float4& u, const Float4& v) noexcept;

Float4x4 operator-(const Float4x4& a) noexcept;
Float4x4 operator+(const Float4x4& a, const Float4x4& b) noexcept;
Float4x4 operator-(const Float4x4& a, const Float4x4& b) noexcept;
Float4x4 operator*(const Float4x4& a, const Float4x4& b) noexcept;
Float4x4 operator/(const Float4x4& a, const Float4x4& b) noexcept;
Float4x4 operator+(const Float4x4& a, float f) noexcept;
Float4x4 operator-(const Float4x4& a, float f) noexcept;
Float4x4 operator*(const Float4x4& a, float f) noexcept;
Float4x4 operator/(const Float4x4& a, float f) noexcept;
Float4x4 operator+(float f, const Float4x4& a) noexcept;
Float4x4 operator-(float f, const Float4x4& a) noexcept;
Float4x4 operator*(float f, const Float4x4& a) noexcept;
Float4x4 operator/(float f, const Float4x4& a) noexcept;
