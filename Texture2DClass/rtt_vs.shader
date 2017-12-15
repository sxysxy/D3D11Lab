struct vs_output
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

vs_output main(float4 position : POSITION, float4 color : COLOR) {
    vs_output opt;
    opt.position = position;
    opt.color = color;
    return opt;
}

