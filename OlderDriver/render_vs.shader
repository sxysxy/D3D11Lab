struct vs_output {
	float4 position : SV_POSITION;
};

struct vs_input {
	float4 position : POSITION;
};

vs_output main(vs_input input) {
	return input;
}
