#include "Input.h"
#pragma comment(lib, "dinput8.lib")

namespace Input {
    ComPtr<IDirectInput8> native_dinput_object;
    void Initialize() {
        HRESULT hr = DirectInput8Create(GetModuleHandle(0), 0x0800,
             IID_IDirectInput8, &native_dinput_object, nullptr);
        if(FAILED(hr))throw std::runtime_error("Failed to create direct input 8 object");
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