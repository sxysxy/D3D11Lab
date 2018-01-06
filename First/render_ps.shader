cbuffer Param:register(b0) {
	float4 render_color;
};

struct vs_output {
	float4 position : SV_POSITION;
};

float4 main(vs_output vs_opt) : SV_TARGET {
	return render_color;
} 