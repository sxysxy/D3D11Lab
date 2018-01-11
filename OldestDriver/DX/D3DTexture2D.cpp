#include <stdafx.h>
#include "D3DDevice.h"
#include "D3DDeviceContext.h"
#include "DX.h"
#include "D3DTexture2D.h"

D3DTexture2D::D3DTexture2D(D3DDevice * device, int w, int h, bool stenciled = false){
    Initialize(device, w, h, stenciled);
}

D3DTexture2D::D3DTexture2D(const cstring &filename, bool stenciled = false){
    Initialize(filename, stenciled);
}

void D3DTexture2D::Initialize(D3DDevice * device, int w, int h, bool stenciled = false){

}

void D3DTexture2D::Initialize(const cstring &filename, bool stenciled = false){

}


namespace Ext { namespace DX {
    namespace D3DTexture2D {  
        VALUE klass;

        void Init() {

        }
    }
} }

