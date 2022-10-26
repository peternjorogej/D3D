
cbuffer LightData
{
    float3 LightPosition;
    float3 MaterialColor;
    float3 AmbientColor;
    float3 DiffuseColor;
    float  DiffuseIntensity;
    float  AttenConstant;
    float  AttenLinear;
    float  AttenQuadratic;
};

float4 Main(float3 WorldPosition : POSITION, float3 Normal : NORMAL) : SV_TARGET
{
    const float3 vToL = LightPosition - WorldPosition;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;
    const float attenuation = 1.0f / (AttenConstant + AttenLinear*distToL + AttenQuadratic*(distToL*distToL));
    const float3 diffuse = DiffuseColor * DiffuseIntensity * attenuation * max(0.0f, dot(dirToL, Normal));

    return float4(saturate((diffuse + AmbientColor) * MaterialColor), 1.0f);
}