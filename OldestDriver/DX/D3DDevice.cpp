#include "D3DDevice.h"
#include <extension.h>
#include "DX.h"
#include <stdafx.h>

void D3DDevice::Initialize(D3D_DRIVER_TYPE tp = D3D_DRIVER_TYPE_HARDWARE) {
    if(tp > 5)throw std::invalid_argument("Invalid D3DDevice type");
    TCHECK(SUCCEEDED(D3D11CreateDevice(0, tp, 0, 0, 0, 0,
        D3D11_SDK_VERSION, 
        &native_device, 0, 0)), "Failed to create D3D11 device");
    ID3D11Device *d = native_device.Get();
    d->QueryInterface(__uuidof(IDXGIDevice), &native_dxgi_device);
    native_dxgi_device->GetParent(__uuidof(IDXGIAdapter), &native_dxgi_adapter);
    native_dxgi_adapter->GetParent(__uuidof(IDXGIFactory), &native_dxgi_factory);
    immcontext = Utility::ReferPtr<D3DDeviceImmdiateContext>::New(this);
}
void D3DDevice::QueryAdapterInfo(DXGI_ADAPTER_DESC *d) {
    native_dxgi_adapter->GetDesc(d);
}



namespace Ext { namespace DX {
    namespace D3DDevice {
        VALUE klass;

        void Delete(::D3DDevice *d) {
            delete d;
        }

        VALUE New(VALUE klass){
            auto d = new ::D3DDevice;
            return Data_Wrap_Struct(klass, nullptr, Delete, d);
        }

        static VALUE initialize(int argc, VALUE *argv, VALUE self) {
            if (argc > 1) {
                rb_raise(rb_eArgError, "D3DDevice#intialize:Wrong number of arguments, expecting (0..1), but given %d", argc);
            }
            D3D_DRIVER_TYPE t;
            if(argc == 0)t = D3D_DRIVER_TYPE_HARDWARE;
            if(argc == 1)t = (D3D_DRIVER_TYPE)FIX2INT(argv[0]);
            auto d = GetNativeObject<::D3DDevice>(self);
            try{
                d->Initialize(t);
            }
            catch (std::runtime_error err) {
                rb_raise(rb_eRuntimeError, err.what());
            }
            catch (std::invalid_argument err) {
                rb_raise(rb_eArgError, err.what());
            }
            return self;
        }

        static VALUE query_adapter_info(VALUE self) {
            auto sc = GetNativeObject<::D3DDevice>(self);
            DXGI_ADAPTER_DESC d;
            sc->QueryAdapterInfo(&d);
            VALUE info = rb_hash_new();

            std::string s;
            U16ToU8(d.Description, s, CP_UTF8);
            rb_hash_aset(info, ID2SYM(rb_intern("description")),rb_str_new(s.c_str(), s.length()));
            rb_hash_aset(info, ID2SYM(rb_intern("dedicated_vide_memory")), INT2NUM(d.DedicatedVideoMemory / 1024 / 1024));
            rb_hash_aset(info, ID2SYM(rb_intern("delicated_system_memory")), INT2NUM(d.DedicatedSystemMemory / 1024 / 1024));
            rb_hash_aset(info, ID2SYM(rb_intern("shared_system_memory")), INT2NUM(d.SharedSystemMemory / 1024 / 1024));

            return info;
        }

        static VALUE immcontext(VALUE self) {
            auto d = GetNativeObject<::D3DDevice>(self);
            return Data_Wrap_Struct(Ext::DX::D3DDeviceContext::klass_immcontext, nullptr, nullptr, d->immcontext.Get());
        }

        void Init() {
            klass = rb_define_class_under(module, "D3DDevice", rb_cObject);
            rb_define_alloc_func(klass, New);
            rb_define_const(klass, "HARDWARE_DEVICE", INT2FIX(D3D_DRIVER_TYPE_HARDWARE));
            rb_define_const(klass, "SIMULATED_DEVICE", INT2FIX(D3D_DRIVER_TYPE_WARP));
            rb_define_method(klass, "initialize", (rubyfunc)initialize, -1);
            rb_define_method(klass, "query_adapter_info", (rubyfunc)query_adapter_info, 0);
            rb_define_method(klass, "immcontext", (rubyfunc)immcontext, 0);
        }
    }
} }

