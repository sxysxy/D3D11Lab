#pragma once
#include <stdafx.h>
#include "DX.h"
#include "D3DDevice.h"
#include "D3DDeviceContext.h"

class RenderPipeline : public Utility::ReferredObject {

};

namespace Ext {namespace DX {
    namespace RenderPipeline {
        extern VALUE klass;
        void Init();
    }
} }