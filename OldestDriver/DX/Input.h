#pragma once
#include <stdafx.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

namespace Input {
    
    extern ComPtr<IDirectInput8> native_dinput_object;
    void Initialize();

    class Device : public Utility::ReferredObject {
    public:
        Utility::ReferPtr<Utility::HFBuffer<char>> data_buffer[2]; //double buffer 
        ComPtr<IDirectInputDevice8> native_dinput_device;
        void Acquire() {
            native_dinput_device->Acquire();
        }

        void UnAcqure() {
            native_dinput_device->Unacquire();
        }

        void UnInitialize() {
            UnAcqure();
            native_dinput_device.ReleaseAndGetAddressOf();
            data_buffer[0].Release();
            data_buffer[1].Release();
        }

        virtual void Release() {
            UnInitialize();
        }

    protected:
        int buffer_index;
    public:
        bool ReadDeviceData() {
            HRESULT hr;
            buffer_index ^= 1;   //Ë«»º³å¼¼Êõ
            while (true) {
                native_dinput_device->Poll();
                Acquire();
                hr = native_dinput_device->GetDeviceState(data_buffer[buffer_index&1]->size, data_buffer[buffer_index&1]->ptr);
                if(SUCCEEDED(hr))break;
                if(hr != DIERR_INPUTLOST || hr != DIERR_NOTACQUIRED)return false;
                if(FAILED(native_dinput_device->Acquire()))return false;
            }
            return true;
        } 
       
    };

    class Keyboard : public Input::Device{
    public:
        Keyboard(){}
        Keyboard(HWND www) {Initialize(www);}
        void Initialize(HWND hWnd);
        bool IsKeyPressedNow(int keycode);
        bool IsKeyPressedBefore(int keycode);
    };
};

namespace Ext {
    namespace DX {
        namespace Input {
            extern VALUE module_Input;
            extern VALUE klass_Keyboard;
            void Init();
        }
    }
}