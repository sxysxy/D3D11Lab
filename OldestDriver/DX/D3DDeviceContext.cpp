#include <stdafx.h>
#include <extension.h>
#include "D3DDevice.h"
#include "DX.h"
#include "D3DDeviceContext.h"

D3DDeviceContext::D3DDeviceContext(D3DDevice *device) {
    Initialize(device);
}
void D3DDeviceContext::Initialize(D3DDevice *device) {
    HRCHECK(device->native_device->CreateDeferredContext(0, &native_context), "Failed to create deferred context");
    native_device = device->native_device;
}
void D3DDeviceContext::FinishiCommandList() {
    native_context->FinishCommandList(false, &native_command_list);
}

void D3DDeviceContext::BindPipeline(RenderPipeline *pipeline) {
    if(pipeline->vshader)BindVertexShader(pipeline->vshader.Get());
    if(pipeline->pshader)BindPixelShader(pipeline->pshader.Get());
    if(pipeline->native_input_layout)native_context->IASetInputLayout(pipeline->native_input_layout.Get());
}

void D3DDeviceContext::BindVertexShader(VertexShader *vshader) {
    native_context->VSSetShader(vshader->native_shader.Get(), 0, 0);

    //for (int i = 0; i < render_pipeline->vshader->native_constant_buffers.size(); i++) {
    //    native_context->VSSetConstantBuffers(i, 1, render_pipeline->vshader->native_constant_buffers[i].GetAddressOf());
    //}
    const auto &buffers = vshader->native_constant_buffers;
    native_context->VSSetConstantBuffers(0, buffers.size(), reinterpret_cast<ID3D11Buffer *const *>(buffers.data()));
}
void D3DDeviceContext::BindPixelShader(PixelShader *pshader) {
    native_context->PSSetShader(pshader->native_shader.Get(), 0, 0);

    //for (int i = 0; i < render_pipeline->pshader->native_constant_buffers.size(); i++) {
    //    native_context->VSSetConstantBuffers(i, 1, render_pipeline->pshader->native_constant_buffers[i].GetAddressOf());
    //}
    const auto &buffers = pshader->native_constant_buffers;
    native_context->VSSetConstantBuffers(0, buffers.size(), reinterpret_cast<ID3D11Buffer *const *>(buffers.data()));
}

void D3DDeviceContext::ClearState() {
    native_context->ClearState();
}

D3DDeviceImmdiateContext::D3DDeviceImmdiateContext(D3DDevice *device) {
    native_device = device->native_device;
    native_device->GetImmediateContext(&native_context);
}

void D3DDeviceImmdiateContext::ExecuteCommandList(D3DDeviceContext *ocontext) {
    native_context->ExecuteCommandList(ocontext->native_command_list.Get(), false);
}

namespace Ext {
    namespace DX {
        namespace D3DDeviceContext {
            VALUE klass;
            VALUE klass_immcontext;

            void Delete(::D3DDeviceContext *d) {
                delete d;
            }

            VALUE New(VALUE k){
                return Data_Wrap_Struct(k, nullptr, Delete, new ::D3DDeviceContext);
            }

            static VALUE initialize(VALUE self, VALUE _device) {
                if (!rb_obj_is_kind_of(_device, Ext::DX::D3DDevice::klass)) {
                    rb_raise(rb_eArgError, "D3DDeviceContext::initialize:Param device should be a D3DDeviceContext.");
                }
                auto context = GetNativeObject<::D3DDeviceContext>(self);
                auto device = GetNativeObject<::D3DDevice>(_device);
                context->Initialize(device);
                
                return self;
            }

            VALUE exec_command_list(VALUE self, VALUE _deferred_context) {
                if (!rb_obj_is_kind_of(_deferred_context, klass) || rb_obj_is_kind_of(_deferred_context, klass_immcontext)) {
                    rb_raise(rb_eArgError, "D3DDeviceImmdiateContext::exec_command_list: Given argument should be a DX::DeviceContext");
                }

                auto context = GetNativeObject<::D3DDeviceImmdiateContext>(self);
                auto ocontext = GetNativeObject<::D3DDeviceContext>(self);
                context->ExecuteCommandList(ocontext);
                return Qnil;
            }

            static VALUE bind_pipeline(VALUE self, VALUE rp) {
                if (!rb_obj_is_kind_of(rp, Ext::DX::RenderPipeline::klass)) {
                    rb_raise(rb_eArgError, "D3DDeviceContext::bind_pipeline: The param should be a DX::RenderPipeline");
                }
                rb_iv_set(self, "@pipeline", rp);
                auto context = GetNativeObject<::D3DDeviceContext>(self);
                auto rpp = GetNativeObject<::RenderPipeline>(rp);
                context->BindPipeline(rpp);
                return rp;
            }
            static VALUE clear_state(VALUE self) {
                auto context = GetNativeObject<::D3DDeviceContext>(self);
                context->ClearState();
                return Qnil;
            }
            static VALUE finishi_command_list(VALUE self) {
                return rb_iv_get(self, "@command_list");
            }

            void Init() {
                klass = rb_define_class_under(module, "D3DDeviceContext", rb_cObject);
                rb_define_alloc_func(klass, New);
                rb_define_method(klass, "initialize", (rubyfunc)initialize, 1);
                rb_define_method(klass, "finishi_command_list", (rubyfunc)finishi_command_list, 0);
                rb_define_method(klass, "bind_pipeline", (rubyfunc)bind_pipeline, 1);
                rb_define_method(klass, "clear_state", (rubyfunc)clear_state, 0);
                rb_attr_get(klass, rb_intern("pipeline"));

                klass_immcontext = rb_define_class_under(module, "D3DDeviceImmdiateContext", klass);
                rb_undef_alloc_func(klass_immcontext);
                rb_undef_method(CLASS_OF(klass_immcontext), "new");
                rb_define_method(klass_immcontext, "exec_command_list", (rubyfunc)exec_command_list, 1);
                rb_undef_method(klass_immcontext, "get_command_list");
                rb_undef_method(klass_immcontext, "command_list");
                rb_undef_method(klass_immcontext, "clear_command_list");

            }
        }
} }