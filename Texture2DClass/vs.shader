cbuffer TextureParam
{
    float posx;
    float posy;
    float width;
    float height;
};

struct vs_input {
    float2 pos : POSITION;
    float2 tex : TEXCOORD0;
};
struct ps_input {
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
};
ps_input main(vs_input input) {
    ps_input output = (ps_input)0;
    output.pos.x = input.pos.x * 2.f / width - 1.f + posx;
    output.pos.y = 1.f - input.pos.y * 2.f / height + posy;
    output.pos.z = 1.f;
    output.pos.w = 1.f;
    output.tex = input.tex;
    return output;
}
