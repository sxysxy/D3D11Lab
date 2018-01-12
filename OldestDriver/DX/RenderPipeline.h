#pragma once
#include <stdafx.h>
#include "DX.h"
#include "D3DDevice.h"
#include "D3DDeviceContext.h"

template<class NativeShaderType>
class Shader : public Utility::ReferredObject {
protected:
public:
    ComPtr<NativeShaderType> native_shader;
    ComPtr<ID3D10Blob> byte_code;
    virtual void CreateFromHLSLFile(D3DDevice *device, const cstring &filename) = 0;
    virtual void CreateFromBinaryFile(D3DDevice *device, const cstring &filename) = 0;
    virtual void CreateFromString(D3DDevice *device, const std::string &str) = 0;
    void UnInitialize() {
        native_shader.ReleaseAndGetAddressOf();
        byte_code.ReleaseAndGetAddressOf();
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


class VertexShader : public Shader<ID3D11VertexShader> {
public:
    virtual void CreateFromHLSLFile(D3DDevice *device, const cstring &filename);
    virtual void CreateFromBinaryFile(D3DDevice *device, const cstring &filename);
    virtual void CreateFromString(D3DDevice *device, const std::string &str);

    DEF_CREATOR(VertexShader)
};

class PixelShader : public Shader<ID3D11PixelShader> {
public:
    virtual void CreateFromHLSLFile(D3DDevice *device, const cstring &filename);
    virtual void CreateFromBinaryFile(D3DDevice *device, const cstring &filename);
    virtual void CreateFromString(D3DDevice *device, const std::string &str);

    DEF_CREATOR(PixelShader)
};

class RenderPipeline : public Utility::ReferredObject {
public:
    ComPtr<ID3D11InputLayout> native_input_layout;
    Utility::ReferPtr<VertexShader> vshader;
    Utility::ReferPtr<PixelShader> pshader;

    virtual void Release() {
        vshader.Release();
        pshader.Release();
    }
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