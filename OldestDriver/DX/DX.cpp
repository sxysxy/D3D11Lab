#include "DX.h"
#include "D3DDevice.h"
#include "D3DDeviceContext.h"
#include "SwapChain.h"
#include "D3DTexture2D.h"
#include "RenderPipeline.h"
#include "D3DBuffer.h"

namespace Ext{
    namespace DX{
        VALUE module;
        VALUE klass_eImplementStillNotSupported;

        void Ext::DX::Init() {
            module = rb_define_module("DX");
            klass_eImplementStillNotSupported = rb_define_class_under(module, "ImplementStillNotSupported", rb_eException);
            Ext::DX::D3DDevice::Init();
            Ext::DX::D3DDeviceContext::Init();
            Ext::DX::D3DTexture2D::Init();
            Ext::DX::SwapChain::Init();
            Ext::DX::RenderPipeline::Init();
            Ext::DX::D3DBuffer::Init();
        }

       
    }
}
