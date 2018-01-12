#include <stdafx.h>
#include <extension.h>
#include "D3DDevice.h"
#include "DX.h"
#include "D3DDeviceContext.h"

D3DDeviceContext::D3DDeviceContext(D3DDevice *device) {
    Initialize(device);
}
void D3DDeviceContext::Initialize(D3DDevice *device) {
    device->native_device->CreateDeferredContext(0, &native_context);
    native_device = device->native_device;
}
void D3DDeviceContext::FinishiCommandList() {
    native_context->FinishCommandList(true, &native_command_list);
}

D3DDeviceImmdiateContext::D3DDeviceImmdiateContext(D3DDevice *device) {
    native_device = device->native_device;
    native_device->GetImmediateContext(&native_context);
}

void D3DDeviceImmdiateContext::ExecuteCommandList(ComPtr<ID3D11CommandList> list) {
    native_context->ExecuteCommandList(list.Get(), true);
}

namespace Ext {
    namespace DX {
        namespace D3DDeviceContext {
            VALUE klass;
            VALUE klass_command_list;
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
            static VALUE get_command_list(VALUE self) {
                auto context = GetNativeObject<::D3DDeviceContext>(self);
                context->FinishiCommandList();
                return Data_Wrap_Struct(klass_command_list, nullptr, nullptr, context->native_command_list.Get());
            }
            static VALUE clear_command_list(VALUE self) {
                auto context = GetNativeObject<::D3DDeviceContext>(self);
                context->native_context->FinishCommandList(false, nullptr);
                return Qnil;
            }

            VALUE exec_command_list(VALUE self, VALUE list) {
                if (!rb_obj_is_kind_of(list, klass_command_list)) {
                    rb_raise(rb_eArgError, "D3DDeviceImmdiateContext::exec_command_list: Given argument is not a kind D3DCommandList.");
                }
                ComPtr<ID3D11CommandList> clist;        //Use ComPtr
                Data_Get_Struct(list, ID3D11CommandList, clist);
                auto context = GetNativeObject<::D3DDeviceImmdiateContext>(self);
                context->ExecuteCommandList(clist);
                return Qnil;
            }

            void Init() {
                klass = rb_define_class_under(module, "D3DDeviceContext", rb_cObject);
                rb_define_alloc_func(klass, New);
                rb_define_method(klass, "initialize", (rubyfunc)initialize, 1);
                rb_define_method(klass, "get_command_list", (rubyfunc)get_command_list, 0);
                rb_define_method(klass, "clear_command_list", (rubyfunc)clear_command_list, 0);
                klass_immcontext = rb_define_class_under(module, "D3DDeviceImmdiateContext", klass);

                rb_undef_alloc_func(klass_immcontext);
                rb_undef_method(CLASS_OF(klass_immcontext), "new");
                rb_define_method(klass_immcontext, "exec_command_list", (rubyfunc)exec_command_list, 1);

                klass_command_list = rb_define_class_under(module, "D3DCommandList", rb_cObject);
            }
        }
} }