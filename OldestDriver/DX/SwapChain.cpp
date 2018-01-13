#include "DX.h"
#include "SwapChain.h"
#include "D3DDevice.h"
#include <stdafx.h>
#include <extension.h>

void SwapChain::Initialize(D3DDevice * device, HFWindow * wnd, bool fullscreen = false, bool _stenciled = false) {
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof sd);
    sd.BufferCount = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.Windowed = !fullscreen;
    sd.OutputWindow = wnd->native_handle;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.BufferDesc.Width = wnd->width;
    sd.BufferDesc.Height = wnd->height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    native_device = device->native_device;
    stenciled = _stenciled;

    HRESULT hr = S_FALSE;
    if (FAILED(hr = device->native_dxgi_factory->CreateSwapChain(native_device.Get(),
        &sd, &native_swap_chain))) {throw std::runtime_error("Failed to create swapchain");}
                //Usually cause a _com_error because of DXGI_STATUS_OCCLUDED.
                //See https://msdn.microsoft.com/en-us/library/windows/desktop/cc308061(v=vs.85).aspx
    
    Resize(wnd->width, wnd->height);
    if(fullscreen)SetFullScreen(fullscreen);
}

void SwapChain::Resize(int w, int h) {
    backbuffer.UnInitialize(); //Must be placed before ResizeBuffers, otherwise ResizeBuffers will fail.
    
    HRESULT hr = native_swap_chain->ResizeBuffers(1, w, h,
        DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to resize swapchain's buffers.");
    }

    ComPtr<ID3D11Texture2D> native_backbuffer;
    native_swap_chain->GetBuffer(0,
        __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(native_backbuffer.GetAddressOf()));
    if (!native_backbuffer) {
       throw std::runtime_error("Failed to get swapchain's texture buffer");
    }

    backbuffer.Initialize(native_backbuffer.Get(), false);
    //backbuffer.Initialize(native_backbuffer.Get(), false);
}

namespace Ext { namespace DX {
    namespace SwapChain {
        VALUE klass;

        static void Delete(::SwapChain *s) {
            delete s;
        }

        static VALUE New(VALUE k) {
            auto *sc = new ::SwapChain;
            return Data_Wrap_Struct(k, nullptr, Delete, sc);
        }

        static VALUE present(int argc, VALUE *argv, VALUE self) {
            auto sc = GetNativeObject<::SwapChain>(self);
            if(argc > 1)rb_raise(rb_eArgError, "SwapChain::present: Wrong number of Arguments, expecting (0..1) but got %d", argc);
            sc->Present(argc? max(0, min(FIX2INT(argv[0]), 4)) : 0);
            return Qnil;
        }

        static VALUE set_fullscreen(int argc, VALUE *argv, VALUE self) {
            if(argc > 1)rb_raise(rb_eArgError, "SwapChain::set_fullscreen:Wrong number of Arguments, expecting (0..1), but given %d.", argc);
            auto sc = GetNativeObject<::SwapChain>(self);
            if(argc == 0)sc->SetFullScreen(true);
            else sc->SetFullScreen(argv[0] == Qtrue);
            return Qnil;
        }

        static VALUE initialize(int argc, VALUE *argv, VALUE self) {
            if (argc < 2 || argc > 4) {
                rb_raise(rb_eArgError, "SwapChain::initialize:Wrong number of Arguments, expecting (2..4) \
                     (device, window, [fullscreen], [with_stencil]), but given %d", argc);
            }
            
            if (!(rb_obj_is_kind_of(argv[0], Ext::DX::D3DDevice::klass)) || 
                !(rb_obj_is_kind_of(argv[1], Ext::HFWindow::klass))) {
                rb_raise(rb_eArgError, "SwapChain::initialize: Invalid Argument type");
            }

            auto sc = GetNativeObject<::SwapChain>(self);
            try{
            sc->Initialize(GetNativeObject<::D3DDevice>(argv[0]), GetNativeObject<Ext::HFWindow::RHFWindow>(argv[1]), 
                argc > 2 ? (argv[2] == Qtrue) : false, argc > 3 ? (argv[3] == Qtrue) : false);
            }
            catch (std::runtime_error re) {
                rb_raise(rb_eRuntimeError, re.what());
            }
            rb_iv_set(self, "@back_buffer", Data_Wrap_Struct(Ext::DX::D3DTexture2D::klass, nullptr, nullptr, &sc->backbuffer));
            return self;
        }

        static VALUE resize(VALUE self, VALUE w, VALUE h) {
            auto sc = GetNativeObject<::SwapChain>(self);
            sc->Resize(FIX2INT(w), FIX2INT(h));
            return Qnil;
        }

        static VALUE back_buffer(VALUE self) {
            return rb_iv_get(self, "@back_buffer");
        }

        void Init() {
            klass = rb_define_class_under(module, "SwapChain", rb_cObject);
            rb_define_alloc_func(klass, New);
            rb_define_method(klass, "initialize",(rubyfunc)initialize, -1);

            rb_define_const(klass, "VSYNC_NO", INT2FIX(0));
            rb_define_const(klass, "VSYNC_1_BLANK", INT2FIX(1));
            rb_define_const(klass, "VSYNC_2_BLANK", INT2FIX(2));
            rb_define_const(klass, "VSYNC_3_BLANK", INT2FIX(3));
            rb_define_const(klass, "VSYNC_4_BLANK", INT2FIX(4));
            rb_define_method(klass, "present", (rubyfunc)present, -1);

            rb_define_method(klass, "set_fullscreen", (rubyfunc)set_fullscreen, -1);
            rb_define_method(klass, "resize", (rubyfunc)resize, 2);
            rb_define_method(klass, "back_buffer", (rubyfunc)back_buffer, 0);
            rb_define_alias(klass, "backbuffer", "back_buffer");
        }
    }
}}


