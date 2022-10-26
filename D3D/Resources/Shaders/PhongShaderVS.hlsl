
cbuffer ModelViewProjection
{
    float4x4 Model;
    float4x4 MVP;
};

struct VSOut
{
    float3 WorldPosition : POSITION;
    float3 Normal        : NORMAL;
    float4 LocalPosition : SV_POSITION;
};

VSOut Main(float3 Position : POSITION, float3 Normal : NORMAL)
{
    const float4 pos = float4(Position, 1.0f);

    VSOut Out;
    Out.LocalPosition = mul(pos, MVP);
    Out.WorldPosition = (float3)mul(pos, Model);
    Out.Normal        = mul(Normal, (float3x3)Model);

    return Out;
}
