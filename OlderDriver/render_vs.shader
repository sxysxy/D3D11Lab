cbuffer Params:register(b0) {
	float posx, posy, width, height;
};

struct vs_output
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

/*
vs_output main(float4 pos : POSITION, float4 color : COLOR) {
    vs_output opt;
	float2 xy = opt.position;
    opt.position.x = pos.x * 2.0f * width - 1.0f + posx;
	opt.position.y = 1.0f - pos.y * height * 2.0f - posy;
	opt.position.z = opt.position.w = 1.0f;
    opt.color = color;
    return opt;
}
*/

vs_output main(float4 pos : POSITION, float4 color : COLOR) {
	vs_output opt;
	opt.position = pos;
	opt.color = color;
	return opt;
}

