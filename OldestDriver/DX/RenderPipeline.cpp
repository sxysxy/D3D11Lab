#include "RenderPipeline.h"
#include "D3DTexture2D.h"
#include "D3DDeviceContext.h"
#include "D3DDevice.h"
#include "DX.h"
#include <extension.h>

void VertexShader::CreateFromHLSLFile(D3DDevice *device, const cstring & filename) {
    ComPtr<ID3D10Blob> sbuffer, errmsg;

    HRESULT hr = D3DX11CompileFromFileW(filename.c_str(), nullptr, nullptr, "main", 
        "vs_4_0", 0, 0, 0, 
        &sbuffer, &errmsg, nullptr);
    if (FAILED(hr)) {
        std::string msg;
        Ext::U16ToU8(filename.c_str(), msg);
        if (errmsg) {
            msg.append(":Compiler Message:\n");
            msg.append((LPCSTR)errmsg->GetBufferPointer());
            throw ShaderCompileError(msg);
        }
        else {
            msg.append(":File not found.");
            throw FileNotFoundException(msg);
        }
    }
    byte_code = sbuffer;
    device->native_device->CreateVertexShader(byte_code->GetBufferPointer(), byte_code->GetBufferSize(), 0, &native_shader);
}

void VertexShader::CreateFromBinaryFile(D3DDevice *device, const cstring & filename) {
    throw ImplementStillNotSupported("VertexShader::CreateFromBinaryFile:The implement is still not supported.");
}

void VertexShader::CreateFromString(D3DDevice *device, const std::string & str) {
    ComPtr<ID3D10Blob> sbuffer, errmsg;
    HRESULT hr = D3DX11CompileFromMemory(str.c_str(), str.length(), 0, 0, 0, "main", "vs_4_0", 0, 0, 0, &sbuffer, &errmsg, 0);
    if (FAILED(hr)) {
        if (errmsg) {
            std::string msg;
            msg.append(":Compiler Message:\n");
            msg.append((LPCSTR)errmsg->GetBufferPointer());
            throw ShaderCompileError(msg);
        }
        else {
            throw std::runtime_error("Unknown error occurred when Compiling shader");
        }
    }
    byte_code = sbuffer;
    device->native_device->CreateVertexShader(byte_code->GetBufferPointer(), byte_code->GetBufferSize(), 0, &native_shader);
}

void PixelShader::CreateFromHLSLFile(D3DDevice *device, const cstring & filename) {
    ComPtr<ID3D10Blob> sbuffer, errmsg;

    HRESULT hr = D3DX11CompileFromFileW(filename.c_str(), nullptr, nullptr, "main",
        "ps_4_0", 0, 0, 0,
        &sbuffer, &errmsg, nullptr);
    if (FAILED(hr)) {
        std::string msg;
        Ext::U16ToU8(filename.c_str(), msg);
        if (errmsg) {
            msg.append(":Compiler Message:\n");
            msg.append((LPCSTR)errmsg->GetBufferPointer());
            throw ShaderCompileError(msg);
        }
        else {
            msg.append(":File not found.");
            throw FileNotFoundException(msg);
        }
    }
    byte_code = sbuffer;
    device->native_device->CreatePixelShader(byte_code->GetBufferPointer(), byte_code->GetBufferSize(), 0, &native_shader);
}

void PixelShader::CreateFromBinaryFile(D3DDevice *device, const cstring & filename) {
    throw ImplementStillNotSupported("PixelShader::CreateFromBinaryFile:The implement is still not supported.");
}

void PixelShader::CreateFromString(D3DDevice *device, const std::string & str) {
    ComPtr<ID3D10Blob> sbuffer, errmsg;
    HRESULT hr = D3DX11CompileFromMemory(str.c_str(), str.length(), 0, 0, 0, "main", "ps_4_0", 0, 0, 0, &sbuffer, &errmsg, 0);
    if (FAILED(hr)) {
        if (errmsg) {
            std::string msg;
            msg.append(":Compiler Message:\n");
            msg.append((LPCSTR)errmsg->GetBufferPointer());
            throw ShaderCompileError(msg);
        }
        else {
            throw std::runtime_error("Unknown error occurred when Compiling shader");
        }
    }
    byte_code = sbuffer;
    device->native_device->CreatePixelShader(byte_code->GetBufferPointer(), byte_code->GetBufferSize(), 0, &native_shader);
}



namespace Ext {
    namespace DX {
        namespace Shader {
            VALUE klass;
            VALUE klass_vshader;
            VALUE klass_pshader;
            void Init() {
                klass = rb_define_class_under(module, "Shader", rb_cObject);

                klass_vshader = rb_define_class_under(module, "VertexShader", klass);

                klass_pshader = rb_define_class_under(module, "PixelShader", klass);
            }
        }

        namespace RenderPipeline {
            VALUE klass;
            void Init() {
                Ext::DX::Shader::Init();
                klass = rb_define_class_under(module, "RenderPipeline", rb_cObject);

            }
        }
    }
}

