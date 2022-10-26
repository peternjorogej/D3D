
cbuffer ModelViewProjection
{
    float4x4 Model;
    float4x4 MVP;
};

struct VSOut
{
    float4 Color    : COLOR;
    float2 TexCoord : TEXCOORD;
    float4 Position : SV_POSITION;
};

VSOut Main(float3 Position : POSITION, float4 Color : COLOR, float2 TexCoord : TEXCOORD)
{
    VSOut Out;
    Out.Position = mul(float4(Position, 1.0f), MVP);
    Out.TexCoord = TexCoord;
    Out.Color    = Color;

    return Out;
}
