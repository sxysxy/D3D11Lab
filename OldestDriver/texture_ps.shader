struct ps_input {
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
};

Texture2D colorMap : register(t0);
SamplerState colorSampler : register(s0);

float4 main(ps_input input) : SV_TARGET{
    return colorMap.Sample(colorSampler, input.tex);
    //float4 f = {0.5f, 0.3f, 0.2f, 1.0f};
    //return f;
}
