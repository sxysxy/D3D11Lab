#include "Sprite.h"
#include "global.h"

Sprite::Sprite(Texture2D *tex) {
    texture = tex;

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
            L"�������㻺����ʧ��");
}

void Sprite::Render() {
    g_renderer.context->PSSetShaderResources(0, 1, texture->shader_resource_view.GetAddressOf());
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    g_renderer.context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
    g_renderer.context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    g_renderer.context->Draw(6, 0);
}