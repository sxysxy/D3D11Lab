struct vs_output {
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

float4 main(vs_output vs_opt) : SV_TARGET{
	return vs_opt.color;
}