#pragma once
#include "DX.h"
#include "D3DDevice.h"
class D3DDeviceContext;

class D3DBuffer {
public:
    ComPtr<ID3D11Buffer> native_buffer;
};

namespace Ext {
    namespace DX {
        namespace D3DBuffer {
            extern VALUE klass;

            void Init();
        }
    }
}