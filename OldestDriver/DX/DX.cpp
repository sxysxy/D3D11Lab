#include "DX.h"
#include "D3DDevice.h"
#include "D3DDeviceContext.h"
#include "SwapChain.h"
#include "D3DTexture2D.h"
#include "RenderPipeline.h"

namespace Ext{
    namespace DX{
        VALUE module;

        void Ext::DX::Init() {
            module = rb_define_module("DX");
            Ext::DX::D3DDevice::Init();
            Ext::DX::D3DDeviceContext::Init();
            Ext::DX::D3DTexture2D::Init();
            Ext::DX::SwapChain::Init();
            Ext::DX::RenderPipeline::Init();
        }

       
    }
}
