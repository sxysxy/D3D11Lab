#include "Input.h"
#include <HFWindow.h>
#pragma comment(lib, "dinput8.lib")

namespace Input {
    ComPtr<IDirectInput8> native_dinput_object;
    bool initialized;
    void Initialize() {
        if(!initialized){
            HRESULT hr = DirectInput8Create(GetModuleHandle(0), 0x0800,
                IID_IDirectInput8, &native_dinput_object, nullptr);
            if(FAILED(hr))throw std::runtime_error("Failed to create direct input 8 object");
            initialized = true;
        }
    }

    void Keyboard::Initialize(HWND hWnd) {
        if (!native_dinput_object)throw std::runtime_error("Call Input::Initialize first");
        using namespace Utility;
        data_buffer[0] = ReferPtr<HFBuffer<char>>::New(256);
        data_buffer[1] = ReferPtr<HFBuffer<char>>::New(256);
        buffer_index = 0;
        HRESULT hr = native_dinput_object->CreateDevice(GUID_SysKeyboard, &native_dinput_device, nullptr);
        if (FAILED(hr))throw std::runtime_error("Failed to create keyboard device object");
        hr = native_dinput_device->SetDataFormat(&c_dfDIKeyboard);
        if (FAILED(hr))throw std::runtime_error("Failed to set keyboard data format");
        hr = native_dinput_device->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
        DIPROPDWORD property;
        property.diph.dwSize = sizeof DIPROPDWORD;
        property.diph.dwHeaderSize = sizeof DIPROPHEADER;
        property.diph.dwObj = 0;
        property.diph.dwHow = DIPH_DEVICE;
        property.dwData = 16;
        hr = native_dinput_device->SetProperty(DIPROP_BUFFERSIZE, &property.diph);
        if (FAILED(hr)) throw std::runtime_error("Failed to set keyboard property");
    }
    bool Keyboard::IsKeyPressedNow(int keycode) {
        return data_buffer[buffer_index]->ptr[keycode] & 0x80;
    }
    bool Keyboard::IsKeyPressedBefore(int keycode) {
        return data_buffer[buffer_index^1]->ptr[keycode] & 0x80;
    }
}

namespace Ext {
    namespace DX {
        namespace Input {
            VALUE module_Input;
            VALUE klass_Keyboard;
            VALUE InputInitialize(VALUE self) {
                try{
                    ::Input::Initialize();
                }
                catch (std::runtime_error re) {
                    rb_raise(rb_eRuntimeError, re.what());
                    return Qfalse;
                }
                return Qtrue;
            }

            void Keyboard_delete(::Input::Keyboard *k) {
                k -> SubRefer();
            }

            VALUE Keyboard_new(VALUE k) {
                auto kbd = new ::Input::Keyboard;
                kbd->AddRefer();
                return Data_Wrap_Struct(k, nullptr, Keyboard_delete, kbd);
            }

            VALUE Keyboard_initialize(VALUE self, VALUE wnd) {
                if(!rb_obj_is_kind_of(wnd, Ext::HFWindow::klass))rb_raise(rb_eArgError, 
                    "Input::Keyboard::initialize: The only param should be a HFWindow");
                auto keyboard = GetNativeObject<::Input::Keyboard>(self);
                auto window = GetNativeObject<Ext::HFWindow::RHFWindow>(wnd);
                try{
                    keyboard->Initialize(window->native_handle);
                }
                catch (std::runtime_error re) {
                    rb_raise(rb_eRuntimeError, re.what());
                }
                return self;
            }

            VALUE Keyboard_read_device_data(VALUE self) {
                auto keyboard = GetNativeObject<::Input::Keyboard>(self);
                return keyboard->ReadDeviceData() ? Qtrue : Qfalse;
            }

            VALUE Keyboard_is_pressed_now(VALUE self, VALUE key) {
                auto keyboard = GetNativeObject<::Input::Keyboard>(self);
                return keyboard->IsKeyPressedNow(FIX2INT(key)) ? Qtrue : Qfalse;
            }

            VALUE keyboard_is_pressed_before(VALUE self, VALUE key) {
                auto keyboard = GetNativeObject<::Input::Keyboard>(self);
                return keyboard->IsKeyPressedBefore(FIX2INT(key))? Qtrue : Qfalse;
            }

            void Init() {
                module_Input = rb_define_module_under(module, "Input");
                klass_Keyboard = rb_define_class_under(module_Input, "Keyboard", rb_cObject);
                rb_define_alloc_func(klass_Keyboard, Keyboard_new);
                rb_define_method(klass_Keyboard, "initialize", (rubyfunc)Keyboard_initialize, 1);
                rb_define_method(klass_Keyboard, "is_pressed_now", (rubyfunc)Keyboard_is_pressed_now, 1);
                rb_define_method(klass_Keyboard, "is_pressed_before", (rubyfunc)keyboard_is_pressed_before, 1);
            }
        }
    }
}