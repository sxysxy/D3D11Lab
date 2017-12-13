cbuffer Params {
    float width, height;
    float posx, posy;
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
    output.pos.x = input.pos.x * 2.0f * width - 1.0f + posx;
    output.pos.y = 1.0f - input.pos.y * height * 2.0f - posy;
    output.pos.z = 1.0f;
    output.pos.w = 1.0f;
    output.tex = input.tex;
    return output;
}
