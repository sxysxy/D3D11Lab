#include "sprite.h"
#include "global.h"
#include "bitmap.h"

Sprite::Sprite(Bitmap *bmp) {
	bitmap = bmp;
	zoomx = zoomy = 1.0f;
	x = y = ox = oy = 0;
	angle = 0;
	static Vertex inital[] = {
		{ { 1.0f, 1.0f },{ 1.0f, 1.0f } },
	{ { 0.0f, 0.0f },{ 0.0f, 0.0f } },
	{ { 1.0f, 0.0f },{ 1.0f, 0.0f } },
	{ { 0.0f, 0.0f },{ 0.0f, 0.0f } },
	{ { 1.0f, 1.0f },{ 1.0f, 1.0f } },
	{ { 0.0f, 1.0f },{ 0.0f, 1.0f } },
	};
	memcpy(vecs, inital, sizeof(inital));
	D3D11_BUFFER_DESC desc;
	RtlZeroMemory(&desc, sizeof(desc));
	desc.ByteWidth = sizeof(Vertex) * ARRAYSIZE(inital);
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA data;
	RtlZeroMemory(&data, sizeof data);
	data.pSysMem = vecs;
	if (FAILED(g_renderer->device->CreateBuffer(&desc, &data, &vertex_buffer))) {
		throw std::runtime_error("�������㻺��ʧ�ܣ�");
	}
}

void Sprite::Render() {
	if (!Renderer2D::render_texture2d_pipeline.current) {
		throw std::runtime_error("�����˵���Ⱦ����!");
	}
	
	g_renderer->context->PSSetShaderResources(0, 1, bitmap->shader_resource_view.GetAddressOf());
	ShaderParam param = { zoomx * bitmap->width / g_renderer->viewport.right,   //width
		zoomy * bitmap->height / g_renderer->viewport.bottom,  //height
		2.0f * x / g_renderer->viewport.right, 2.0f * y / g_renderer->viewport.bottom,       //x, y
		sin(angle), cos(angle) };  //rotation
	g_renderer->context->UpdateSubresource(g_renderer->render_pipeline->vshader.cbuffer.Get(), 0, 0,
		&param,
		0, 0);                       
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_renderer->context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
	g_renderer->context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	g_renderer->context->Draw(6, 0);
}