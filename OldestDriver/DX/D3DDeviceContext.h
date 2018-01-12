#pragma once

#include <stdafx.h>
#include "D3DTexture2D.h"
#include "RenderPipeline.h"

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
    void BindVShaderResource(int slot_pos, D3DTexture2D *tex) {
        native_context->VSSetShaderResources(slot_pos, 1, tex->native_shader_resource_view.GetAddressOf());
    }
    void BindVShaderSampler(int slot_pos, D3DSampler *sampler) {
        native_context->VSSetSamplers(slot_pos, 1, sampler->native_sampler.GetAddressOf());
    }
    void BindVShaderSamper(int start_slot, int count, D3DSampler *const *ss) {
        ID3D11SamplerState *s[16];
        for(int i = 0; i < count; i++)s[i] = ss[i]->native_sampler.Get();
        native_context->VSSetSamplers(start_slot, count, s);
    }
    void BindPixelShader(PixelShader *ps);
    void BindPShaderResource(int start_slot, int count, D3DTexture2D * const *texes) {
        ID3D11ShaderResourceView *sr[16];
        for (int i = 0; i < count; i++)sr[i] = texes[i]->native_shader_resource_view.Get();
        native_context->PSSetShaderResources(start_slot, count, sr);
    }
    void BindPShaderResource(int slot_pos, D3DTexture2D *tex) {
        native_context->PSSetShaderResources(slot_pos, 1, tex->native_shader_resource_view.GetAddressOf());
    }
    void BindPShaderSampler(int slot_pos, D3DSampler *sampler) {
        native_context->PSSetSamplers(slot_pos, 1, sampler->native_sampler.GetAddressOf());
    }
    void BindPShaderSamper(int start_slot, int count, D3DSampler *const *ss) {
        ID3D11SamplerState *s[16];
        for (int i = 0; i < count; i++)s[i] = ss[i]->native_sampler.Get();
        native_context->PSSetSamplers(start_slot, count, s);
    }

    void SetTopology(D3D11_PRIMITIVE_TOPOLOGY topology) {
        native_context->IASetPrimitiveTopology(topology);
    }
    void Draw(int start_pos, int count) {
        native_context->Draw(count, start_pos);
    }
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