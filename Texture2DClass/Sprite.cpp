#include "Sprite.h"
#include "global.h"

Sprite::Sprite(Texture2D *tex) {
    texture = tex;
    zoomx = zoomy = 1.0f;
    x = y = ox = oy = 0;
    angle = 0;
    static Vertex inital[] = {
        { { 1.0f, 1.0f},{ 1.0f, 1.0f } },
        { { 0.0f, 0.0f},{ 0.0f, 0.0f } },
        { { 1.0f, 0.0f},{ 1.0f, 0.0f } },
        { { 0.0f, 0.0f},{ 0.0f, 0.0f } },
        { { 1.0f, 1.0f},{ 1.0f, 1.0f } },
        { { 0.0f, 1.0f},{ 0.0f, 1.0f } },
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
    RCHECK(SUCCEEDED(g_renderer.device->CreateBuffer(&desc, &data, &vertex_buffer)), 
            L"创建顶点缓冲区失败");
}

void Sprite::Render() {
    g_renderer.context->PSSetShaderResources(0, 1, texture->shader_resource_view.GetAddressOf());
    ShaderParam param = {   zoomx * texture->width / g_renderer.width,   //width
                            zoomy * texture->height / g_renderer.height,  //height
        2.0f * x / g_renderer.width, 2.0f * y / g_renderer.height,       //x, y
        sin(angle), cos(angle) };  //rotation
    g_renderer.context->UpdateSubresource(g_renderer.vertex_shader_cbuffer.Get(), 0, 0, 
        &param,
        0, 0);
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    g_renderer.context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
    g_renderer.context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    g_renderer.context->Draw(6, 0);
}