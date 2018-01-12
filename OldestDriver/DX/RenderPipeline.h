#pragma once
#include <stdafx.h>
#include "DX.h"
#include "D3DTexture2D.h"

class D3DDevice;

class Shader : public Utility::ReferredObject {
protected:
public:
    //ComPtr<NativeShaderType> native_shader;
    ComPtr<ID3D10Blob> byte_code;
    std::vector<ComPtr<ID3D11Buffer>> native_constant_buffers;
    ComPtr<ID3D11SamplerState> native_sampler;

    virtual void CreateFromHLSLFile(D3DDevice *device, const cstring &filename) = 0;
    virtual void CreateFromBinaryFile(D3DDevice *device, const cstring &filename) = 0;
    virtual void CreateFromString(D3DDevice *device, const std::string &str) = 0;
    void UnInitialize() {
        byte_code.ReleaseAndGetAddressOf();
        native_constant_buffers.clear();
    }

    virtual void Release() {}
};

#define DEF_CREATOR(x) static Utility::ReferPtr<x> LoadHLSLFile(D3DDevice *device, const cstring &filename) {              \
        auto shader = Utility::ReferPtr<x>::New();                                                      \
        shader->CreateFromHLSLFile(device, filename);                                                   \
        return shader;                                                                                  \
    }                                                                                                   \
    static Utility::ReferPtr<x> LoadBinaryFile(D3DDevice *device, const cstring &filename) {            \
        auto shader = Utility::ReferPtr<x>::New();                                                      \
        shader->CreateFromBinaryFile(device, filename);                                                 \
        return shader;                                                                                  \
    }                                                                                                   \
    static Utility::ReferPtr<x> LoadCodeString(D3DDevice *device, const std::string &str) {             \
        auto shader = Utility::ReferPtr<x>::New();                                                      \
        shader->CreateFromString(device, str);                                                          \
        return shader;                                                                                  \
    }                                                                                                   \


class VertexShader : public Shader {
public:
    ComPtr<ID3D11VertexShader> native_shader;
    virtual void CreateFromHLSLFile(D3DDevice *device, const cstring &filename);
    virtual void CreateFromBinaryFile(D3DDevice *device, const cstring &filename);
    virtual void CreateFromString(D3DDevice *device, const std::string &str);
    void UnInitialize() {
        Shader::UnInitialize();
        native_shader.ReleaseAndGetAddressOf();
    }
    // DEF_CREATOR(VertexShader) //not good for debuging.
    static Utility::ReferPtr<VertexShader> LoadHLSLFile(D3DDevice *device, const cstring &filename) {
        auto shader = Utility::ReferPtr<VertexShader>::New();
        shader->CreateFromHLSLFile(device, filename);
        return shader;
    }
    static Utility::ReferPtr<VertexShader> LoadBinaryFile(D3DDevice *device, const cstring &filename) {
        auto shader = Utility::ReferPtr<VertexShader>::New();
        shader->CreateFromBinaryFile(device, filename);
        return shader;
    }
    static Utility::ReferPtr<VertexShader> LoadCodeString(D3DDevice *device, const std::string &str) {
        auto shader = Utility::ReferPtr<VertexShader>::New();
        shader->CreateFromString(device, str);
        return shader;
    }
};

class PixelShader : public Shader {
public:
    ComPtr<ID3D11PixelShader> native_shader;
    virtual void CreateFromHLSLFile(D3DDevice *device, const cstring &filename);
    virtual void CreateFromBinaryFile(D3DDevice *device, const cstring &filename);
    virtual void CreateFromString(D3DDevice *device, const std::string &str);
    void UnInitialize() {
        Shader::UnInitialize();
        native_shader.ReleaseAndGetAddressOf();
    }
   // DEF_CREATOR(PixelShader)  // not good for debuging.
    static Utility::ReferPtr<PixelShader> LoadHLSLFile(D3DDevice *device, const cstring &filename) {
        auto shader = Utility::ReferPtr<PixelShader>::New();
        shader->CreateFromHLSLFile(device, filename);
        return shader;
    }
    static Utility::ReferPtr<PixelShader> LoadBinaryFile(D3DDevice *device, const cstring &filename) {
        auto shader = Utility::ReferPtr<PixelShader>::New();
        shader->CreateFromBinaryFile(device, filename);
        return shader;
    }
    static Utility::ReferPtr<PixelShader> LoadCodeString(D3DDevice *device, const std::string &str) {
        auto shader = Utility::ReferPtr<PixelShader>::New();
        shader->CreateFromString(device, str);
        return shader;
    }
            
};

class D3DSampler : public Utility::ReferredObject {
public:
    ComPtr<ID3D11SamplerState> native_sampler;

    virtual void Release() {}
};

class D3DBlendState : public Utility::ReferredObject {
public:
    ComPtr<ID3D11BlendState> native_blend_state;

    virtual void Release() {}
};

class RenderPipeline : public Utility::ReferredObject {
public:
    ComPtr<ID3D11InputLayout> native_input_layout;
    Utility::ReferPtr<VertexShader> vshader;
    Utility::ReferPtr<PixelShader> pshader;

    void UnInitialize(){
        native_input_layout.ReleaseAndGetAddressOf();
        vshader.Release();
        pshader.Release();
    }

    virtual void Release() {
        UnInitialize();
    };

    void SetVertexShader(VertexShader *vs) {vshader = vs; }
    void SetPixelShader(PixelShader *ps) {pshader = ps; }
    VertexShader *GetVertexShader() {return vshader.Get(); }
    PixelShader *GetPixelShader() {return pshader.Get(); }
    void SetInputLayout(D3DDevice *device, const std::string *idents, const DXGI_FORMAT *formats, int count);
};

class ShaderCompileError : public std::runtime_error {
public:
    template<class ...Arg>
    ShaderCompileError(const Arg &...arg) :std::runtime_error(arg...) {}
};

#undef DEF_CREATOR

namespace Ext {namespace DX {
    namespace Shader {
        extern VALUE klass;
        extern VALUE klass_vshader;
        extern VALUE klass_pshader;

        void Init();
    }

    namespace RenderPipeline {
        extern VALUE klass;
        void Init();
    }
} }