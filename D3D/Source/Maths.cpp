#include "Maths.h"

#include <memory.h>
#include <immintrin.h>

#include <random>

static std::mt19937_64	                        s_RandomEngine(std::random_device{}());
static std::uniform_real_distribution<float>    s_RandomDistributionF32;
static std::uniform_real_distribution<double>   s_RandomDistributionF64;
static std::uniform_int_distribution<int64_t>   s_RandomDistributionI64;
static std::uniform_int_distribution<uint64_t>  s_RandomDistributionU64;


// RANDOM
template<typename Real>
inline static Real RandomReal(const std::uniform_real_distribution<Real>& Dist, Real First, Real Last) noexcept
{
    static const Real Max = Dist.max();
    
    const Real t = Dist(s_RandomEngine) / Max;
    return First + (Last - First)*t;
}

template<typename Int>
inline static Int RandomInt(const std::uniform_int_distribution<Int>& Dist, Int kMax) noexcept
{
    if (kMax == Int(0))
    {
        kMax = Int(1);
    }
    return Dist(s_RandomEngine) % kMax;
}

float Random::Float() noexcept
{
    return RandomReal(s_RandomDistributionF32, 0.0f, 1.0f);
}

float Random::Float(float First, float Last) noexcept
{
    return RandomReal(s_RandomDistributionF32, First, Last);
}

double Random::Double() noexcept
{
    return RandomReal(s_RandomDistributionF64, 0.0, 1.0);
}

double Random::Double(double First, double Last) noexcept
{
    return RandomReal(s_RandomDistributionF64, First, Last);
}

int64_t Random::Int() noexcept
{
    return RandomInt(s_RandomDistributionI64, 1ll);
}

int64_t Random::Int(int64_t kMax) noexcept
{
    return RandomInt(s_RandomDistributionI64, kMax);
}

uint64_t Random::UInt() noexcept
{
    return RandomInt(s_RandomDistributionU64, 1ull);
}

uint64_t Random::UInt(uint64_t kMax) noexcept
{
    return RandomInt(s_RandomDistributionU64, kMax);
}

Float2 Random::Float2F() noexcept
{
    return Float2(Float(), Float());
}

Float2 Random::Float2F(float First, float Last) noexcept
{
    const float X = Float(First, Last);
    const float Y = Float(First, Last);
    return Float2(X, Y);
}

Float2 Random::Float2F(const Float2& First, const Float2& Last) noexcept
{
    const float X = Float(First.X, Last.X);
    const float Y = Float(First.Y, Last.Y);
    return Float2(X, Y);
}

Float3 Random::Float3F() noexcept
{
    return Float3(Float(), Float(), Float());
}

Float3 Random::Float3F(float First, float Last) noexcept
{
    const float X = Float(First, Last);
    const float Y = Float(First, Last);
    const float Z = Float(First, Last);
    return Float3(X, Y, Z);
}

Float3 Random::Float3F(const Float3& First, const Float3& Last) noexcept
{
    const float X = Float(First.X, Last.X);
    const float Y = Float(First.Y, Last.Y);
    const float Z = Float(First.Z, Last.Z);
    return Float3(X, Y, Z);
}

Float4 Random::Float4F() noexcept
{
    return Float4(Float(), Float(), Float(), Float());
}

Float4 Random::Float4F(float First, float Last) noexcept
{
    const float X = Float(First, Last);
    const float Y = Float(First, Last);
    const float Z = Float(First, Last);
    const float W = Float(First, Last);
    return Float4(X, Y, Z, W);
}

Float4 Random::Float4F(const Float4& First, const Float4& Last) noexcept
{
    const float X = Float(First.X, Last.X);
    const float Y = Float(First.Y, Last.Y);
    const float Z = Float(First.Z, Last.Z);
    const float W = Float(First.W, Last.W);
    return Float4(X, Y, Z, W);
}

Pixel Random::PixelA(bool bRandomAlpha) noexcept
{
    uint8_t Red   = uint8_t(UInt(255u));
    uint8_t Green = uint8_t(UInt(255u));
    uint8_t Blue  = uint8_t(UInt(255u));
    uint8_t Alpha = 255u;
    if (bRandomAlpha)
    {
        Alpha = uint8_t(UInt(255u));
    }
    return Pixel(Red, Green, Blue, Alpha);
}

// COLOR
Float4 Pixel::ToFloat4() const noexcept
{
    const float r = float(Red)   / 255.0f;
    const float g = float(Green) / 255.0f;
    const float b = float(Blue)  / 255.0f;
    const float a = float(Alpha) / 255.0f;
    return Float4(r, g, b, a);
}

Pixel PixelI(uint32_t kColor) noexcept
{
    uint8_t red   = uint8_t(kColor & 0x000000FF);
    uint8_t green = uint8_t(kColor & 0x0000FF00);
    uint8_t blue  = uint8_t(kColor & 0x00FF0000);
    uint8_t alpha = uint8_t(kColor & 0xFF000000);
    return Pixel(red, green, blue, alpha);
}

Pixel PixelF(float r, float g, float b, float a) noexcept
{
    uint8_t red   = uint8_t(r * 255.0f);
    uint8_t green = uint8_t(g * 255.0f);
    uint8_t blue  = uint8_t(b * 255.0f);
    uint8_t alpha = uint8_t(a * 255.0f);
    return Pixel(red, green, blue, alpha);
}

bool operator==(const Pixel& p, const Pixel& q) noexcept
{
    const uint32_t& ColorA = *reinterpret_cast<const uint32_t*>(&p);
    const uint32_t& ColorB = *reinterpret_cast<const uint32_t*>(&q);
    return ColorA == ColorB;
}

bool operator!=(const Pixel& p, const Pixel& q) noexcept
{
    return !operator==(p, q);
}

// FLOAT2
Float2 operator-(const Float2& u) noexcept
{
    return Float2(-u.X, -u.Y);
}

Float2 operator+(const Float2& u, const Float2& v) noexcept
{
    return Float2(u.X + v.X, u.Y + v.Y);
}

Float2 operator-(const Float2& u, const Float2& v) noexcept
{
    return Float2(u.X - v.X, u.Y - v.Y);
}

Float2 operator*(const Float2& u, const Float2& v) noexcept
{
    return Float2(u.X * v.X, u.Y * v.Y);
}

Float2 operator/(const Float2& u, const Float2& v) noexcept
{
    return Float2(u.X / v.X, u.Y / v.Y);
}

// FLOAT3
Float3 operator-(const Float3& u) noexcept
{
    return Float3(-u.X, -u.Y, -u.Z);
}

Float3 operator+(const Float3& u, const Float3& v) noexcept
{
    return Float3(u.X + v.X, u.Y + v.Y, u.Z + v.Z);
}

Float3 operator-(const Float3& u, const Float3& v) noexcept
{
    return Float3(u.X - v.X, u.Y - v.Y, u.Z - v.Z);
}

Float3 operator*(const Float3& u, const Float3& v) noexcept
{
    return Float3(u.X * v.X, u.Y * v.Y, u.Z * v.Z);
}

Float3 operator/(const Float3& u, const Float3& v) noexcept
{
    return Float3(u.X / v.X, u.Y / v.Y, u.Z / v.Z);
}

Float3::operator Float2() const noexcept
{
    return Float2(X, Y);
}

// FLOAT4
Float4 operator-(const Float4& u) noexcept
{
    return Float4(-u.X, -u.Y, -u.Z, -u.W);
}

Float4 operator+(const Float4& u, const Float4& v) noexcept
{
    // const __m128 w = _mm_add_ps(*(__m128*)&u, *(__m128*)&v);
    // const float* z = (float*)&w;
    // return Float4(z[0], z[1], z[2], z[3]);
    return Float4(u.X + v.X, u.Y + v.Y, u.Z + v.Z, u.W + v.W);
}

Float4 operator-(const Float4& u, const Float4& v) noexcept
{
    // const __m128 w = _mm_sub_ps(*(__m128*)&u, *(__m128*)&v);
    // const float* z = (float*)&w;
    // return Float4(z[0], z[1], z[2], z[3]);
    return Float4(u.X - v.X, u.Y - v.Y, u.Z - v.Z, u.W - v.W);
}

Float4 operator*(const Float4& u, const Float4& v) noexcept
{
    // const __m128 w = _mm_mul_ps(*(__m128*)&u, *(__m128*)&v);
    // const float* z = (float*)&w;
    // return Float4(z[0], z[1], z[2], z[3]);
    return Float4(u.X * v.X, u.Y * v.Y, u.Z * v.Z, u.W * v.W);
}

Float4 operator/(const Float4& u, const Float4& v) noexcept
{
    // const __m128 w = _mm_div_ps(*(__m128*)&u, *(__m128*)&v);
    // const float* z = (float*)&w;
    // return Float4(z[0], z[1], z[2], z[3]);
    return Float4(u.X / v.X, u.Y / v.Y, u.Z / v.Z, u.W / v.W);
}

Float4::operator Float2() const noexcept
{
    return Float2(X, Y);
}

Float4::operator Float3() const noexcept
{
    return Float3(X, Y, Z);
}

// FLOAT4X4
Float4x4::Float4x4()
: Matrix()
{
    for (Float4& Row : Rows)
    {
        Row = Float4{};
    }
}

Float4x4::Float4x4(float t)
: Float4x4()
{
    for (size_t k = 0; k < 4u; k++)
    {
        Matrix[k][k] = t;
    }
}

Float4x4::Float4x4(const float m[4][4])
: Float4x4()
{
    memcpy(Matrix, m, sizeof(float) * 16u);
}

Float4x4::Float4x4(const float m[16]  )
: Float4x4()
{
    memcpy(Matrix, m, sizeof(float) * 16u);
}

float* Float4x4::Data() noexcept
{
    return &Matrix[0][0];
}

const float* Float4x4::Data() const noexcept
{
    return &Matrix[0][0];
}

Float4x4 Float4x4::Transpose() noexcept
{
    Float4x4 T = Float4x4(0.0f);

    for (uint32_t r = 0; r < 4u; r++)
    {
        for (uint32_t c = 0; c < 4u; c++)
        {
            T.Matrix[c][r] = Matrix[r][c];
        }
    }
    
    return T;
}

Float4x4 Float4x4::Translate(const Float3& v) noexcept
{
    Float4x4 tM = Float4x4(1.0f);
    tM.Matrix[0][3] = v.X;
    tM.Matrix[1][3] = v.Y;
    tM.Matrix[2][3] = v.Z;
    return tM;
}

Float4x4 Float4x4::Rotate(const Float3& v) noexcept
{
    return Float4x4();
}

Float4x4 Float4x4::Scale(const Float3& v) noexcept
{
    Float4x4 sM = Float4x4(1.0f);
    sM.Matrix[0][0] = v.X;
    sM.Matrix[1][1] = v.Y;
    sM.Matrix[2][2] = v.Z;
    return sM;
}

Float4x4 Float4x4::Orthographic(float Left, float Right, float Top, float Bottom, float Near, float Far) noexcept
{
    return Float4x4();
}

Float4x4 Float4x4::Perspective(float FoV, float Aspect, float Near, float Far) noexcept
{
    return Float4x4();
}

Float4x4 operator-(const Float4x4& a) noexcept
{
    Float4x4 m;
    for (size_t k = 0; k < 4u; k++)
    {
        m.Rows[k] = a.Rows[k];
    }
    return m;
}

Float4x4 operator+(const Float4x4& a, const Float4x4& b) noexcept
{
    Float4x4 m;
    for (size_t k = 0; k < 4u; k++)
    {
        m.Rows[k] = a.Rows[k] + b.Rows[k];
    }
    return m;
}

Float4x4 operator-(const Float4x4& a, const Float4x4& b) noexcept
{
    Float4x4 m;
    for (size_t k = 0; k < 4u; k++)
    {
        m.Rows[k] = a.Rows[k] - b.Rows[k];
    }
    return m;
}

Float4x4 operator*(const Float4x4& a, const Float4x4& b) noexcept
{
    Float4x4 m;
    for (uint32_t x = 0; x < 4u; x++)
    {
        for (uint32_t y = 0; y < 4u; y++)
        {
            float Dot = 0.0f;
            for (uint32_t e = 0; e < 4u; e++)
            {
                Dot += a[x][e] * b[e][y];
            }
            m[x][y] = Dot;
        }
    }

    return m;
}

Float4x4 operator/(const Float4x4& a, const Float4x4& b) noexcept
{
    return a;
}

Float4x4 operator+(const Float4x4& a, float f) noexcept
{
    for (uint32_t k = 0; k < 4u * 4u; k++)
    {
        a[k];
    }

    return Float4x4{};
}

Float4x4 operator-(const Float4x4& a, float f) noexcept
{
    return Float4x4{};
}

Float4x4 operator*(const Float4x4& a, float f) noexcept
{
    return Float4x4{};
}

Float4x4 operator/(const Float4x4& a, float f) noexcept
{
    return Float4x4{};
}

Float4x4 operator+(float f, const Float4x4& a) noexcept
{
    return Float4x4{};
}

Float4x4 operator-(float f, const Float4x4& a) noexcept
{
    return Float4x4{};
}

Float4x4 operator*(float f, const Float4x4& a) noexcept
{
    return Float4x4{};
}

Float4x4 operator/(float f, const Float4x4& a) noexcept
{
    return Float4x4{};
}
