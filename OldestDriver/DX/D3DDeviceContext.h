#pragma once

#include <stdafx.h>
#include "D3DTexture2D.h"
#include "RenderPipeline.h"

class D3DBuffer;
class D3DConstantBuffer;
class D3DVertexBuffer;

class D3DDeviceContext : public Utility::ReferredObject{
protected:
    ComPtr<ID3D11Device> native_device;
public:
    ComPtr<ID3D11DeviceContext> native_context;
    ComPtr<ID3D11CommandList> native_command_list;
    
    D3DDeviceContext() {};
    D3DDeviceContext(D3DDevice *device);
    void Initialize(D3DDevice *device);
    void UnInitialize() {
        native_context.ReleaseAndGetAddressOf();
        native_command_list.ReleaseAndGetAddressOf();
    }
    virtual void Release() {
        UnInitialize();
    };

    void FinishiCommandList();
    void BindPipeline(RenderPipeline *pipeline);
    void BindVertexShader(VertexShader *vs);
    void BindVShaderResource(int start_slot, int count, D3DTexture2D * const *texes) {
        ID3D11ShaderResourceView *sr[16];
        for(int i = 0; i < count; i++)sr[i] = texes[i]->native_shader_resource_view.Get();
        native_context->VSSetShaderResources(start_slot, count, sr);
    }
    void BindVShaderResource(int slot_pos, const D3DTexture2D *tex) {
        native_context->VSSetShaderResources(slot_pos, 1, tex->native_shader_resource_view.GetAddressOf());
    }
    void BindVShaderSampler(int slot_pos, const D3DSampler *sampler) {
        native_context->VSSetSamplers(slot_pos, 1, sampler->native_sampler.GetAddressOf());
    }
    void BindVShaderSamper(int start_slot, int count, const D3DSampler *const *ss) {
        ID3D11SamplerState *s[16];
        for(int i = 0; i < count; i++)s[i] = ss[i]->native_sampler.Get();
        native_context->VSSetSamplers(start_slot, count, s);
    }
    void BindVShaderConstantBuffer(int start_slot, int count, D3DConstantBuffer *const *cs);
    void BindVShaderConstantBuffer(int slot_pos, const D3DConstantBuffer *bf);
    void BindVertexBuffer(int start_slot, int count, D3DVertexBuffer *const *vbs, UINT stride);
    void BindVertexBuffer(int slot_pos, const D3DVertexBuffer *vb, UINT stride);

    //PS
    void BindPixelShader(PixelShader *ps);
    void BindPShaderResource(int start_slot, int count, D3DTexture2D * const *texes) {
        ID3D11ShaderResourceView *sr[16];
        for (int i = 0; i < count; i++)sr[i] = texes[i]->native_shader_resource_view.Get();
        native_context->PSSetShaderResources(start_slot, count, sr);
    }
    void BindPShaderResource(int slot_pos, const D3DTexture2D *tex) {
        native_context->PSSetShaderResources(slot_pos, 1, tex->native_shader_resource_view.GetAddressOf());
    }
    void BindPShaderSampler(int slot_pos, const D3DSampler *sampler) {
        native_context->PSSetSamplers(slot_pos, 1, sampler->native_sampler.GetAddressOf());
    }
    void BindPShaderSampler(int start_slot, int count, D3DSampler *const *ss) {
        ID3D11SamplerState *s[16];
        for (int i = 0; i < count; i++)s[i] = ss[i]->native_sampler.Get();
        native_context->PSSetSamplers(start_slot, count, s);
    }
    void BindPShaderConstantBuffer(int start_slot, int count, D3DConstantBuffer *const *cs);
    void BindPShaderConstantBuffer(int slot_pos, const D3DConstantBuffer *bf);
    void SetViewport(const Utility::Rect &rect, float min_deep = 0.0f, float max_deep = 1.0f) {
        D3D11_VIEWPORT vp{(float)rect.x, (float)rect.y, (float)rect.w, (float)rect.h, min_deep, max_deep};
        native_context->RSSetViewports(1, &vp);
    }
    void SetTopology(D3D11_PRIMITIVE_TOPOLOGY topology) {
        native_context->IASetPrimitiveTopology(topology);
    }
    void Draw(int start_pos, int count) {
        native_context->Draw(count, start_pos);
    }
    void SetRenderTarget(int count, D3DTexture2D *const *ts) {
        ID3D11RenderTargetView *rtvs[16];
        for(int i = 0; i < count; i++)rtvs[i] = ts[i]->native_render_target_view.Get();
        native_context->OMSetRenderTargets(count, rtvs, ts[0]->native_depth_sencil_view.Get());
    }
    void SetRenderTarget(D3DTexture2D *tex) {
        native_context->OMSetRenderTargets(1, tex->native_render_target_view.GetAddressOf(), 
            tex->native_depth_sencil_view.Get());
    }
    void ClearRenderTarget(D3DTexture2D *tex, const FLOAT *color) {
        native_context->ClearRenderTargetView(tex->native_render_target_view.Get(), color);
        if(tex->native_depth_sencil_view)
            native_context->ClearDepthStencilView(tex->native_depth_sencil_view.Get(),
            D3D11_CLEAR_STENCIL | D3D11_CLEAR_DEPTH, 1.0f, 0);
    }
    void UpdateSubResource(D3DBuffer *buf, void *data);
    void ClearState();
};

class D3DDeviceImmdiateContext : public D3DDeviceContext {
public:
    D3DDeviceImmdiateContext(D3DDevice *device);
    void ExecuteCommandList(D3DDeviceContext *ocontext);
};

namespace Ext {
    namespace DX {
        namespace D3DDeviceContext {
            extern VALUE klass;
            extern VALUE klass_immcontext;

            void Init();
        }
    }
}