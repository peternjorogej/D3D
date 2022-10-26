
Texture2D    s_Texture;
SamplerState s_Sampler;

float4 Main(float4 Color : COLOR, float2 TexCoord: TEXCOORD) : SV_TARGET
{
    return Color * s_Texture.Sample(s_Sampler, TexCoord);
}