Texture2D colorMap : register(t0);
SamplerState colorSampler : register(s0);

float4 main(float4 pos : SV_POSITION, float2 tex : TEXCOORD) : SV_TARGET{
    return colorMap.Sample(colorSampler, tex);
}
