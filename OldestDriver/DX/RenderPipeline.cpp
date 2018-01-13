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
        &sbuffer, &errmsg, nullptr); //cause a _com_error,,but why?, it returns S_OK...         

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

void D3DSampler::Initialize(D3DDevice *device) {
    D3D11_SAMPLER_DESC desc;
    desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // Ë«ÏßÐÔ
    desc.AddressU = desc.AddressV = desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
    desc.MipLODBias = 0.0f;
    desc.MaxAnisotropy = 0;
    desc.ComparisonFunc = static_cast<D3D11_COMPARISON_FUNC>(0);
    desc.BorderColor[0] = desc.BorderColor[1] = desc.BorderColor[2] = desc.BorderColor[3] = 0.0f;
    desc.MinLOD = 0.0f;
    desc.MaxLOD = D3D11_FLOAT32_MAX;
    ComPtr<ID3D11SamplerState> sampler;
    TCHECK(SUCCEEDED(device->native_device->CreateSamplerState(&desc, native_sampler.GetAddressOf())),
        "Fail to create sampler");
}

void RenderPipeline::SetInputLayout(D3DDevice *device, const std::string *idents, const DXGI_FORMAT *fmts, int count) {
    std::vector<D3D11_INPUT_ELEMENT_DESC> ied;

    for (int i = 0; i < count; i++) {
        ied.push_back(D3D11_INPUT_ELEMENT_DESC{ idents[i].c_str(), 0, fmts[i], 0,
            D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 });
    }
    HRESULT hr = S_FALSE;
    if (FAILED(hr = device->native_device->CreateInputLayout(ied.data(), count, vshader->byte_code->GetBufferPointer(),
        vshader->byte_code->GetBufferSize(), &native_input_layout))) {
        throw std::runtime_error("Failed to create input layout");
    }
}


namespace Ext {
    namespace DX {
        namespace Shader {
            VALUE klass;
            VALUE klass_vshader;
            VALUE klass_pshader;
            VALUE klass_eShaderCompileError;

            template<class T>
            static void DeleteShader(T *vs) {
                vs->SubRefer();
            }

            static VALUE initialize(VALUE self) {
                if(rb_block_given_p())rb_obj_instance_eval(0, nullptr, self);
                return self;
            }

            static VALUE create_from_hlsl(VALUE self, VALUE _device, VALUE _filename) {
                auto shader = GetNativeObject<::Shader>(self);
                if (!rb_obj_is_kind_of(_device, Ext::DX::D3DDevice::klass) ||
                    !rb_obj_is_kind_of(_filename, rb_cString)) {
                    rb_raise(rb_eArgError, "Shader::create_from_hlsl: This first param should be a DX::D3DDevice and the \
                        second one should be a String. (No Automatically Converting).");
                }
                auto device = GetNativeObject<::D3DDevice>(_device);
                cstring filename;
                U8ToU16(rb_string_value_cstr(&_filename), filename);
                try{
                    shader->CreateFromHLSLFile(device, filename);
                }
                catch (ShaderCompileError ce) {
                    rb_raise(klass_eShaderCompileError, ce.what());
                }
                catch (std::runtime_error re) {
                    rb_raise(rb_eRuntimeError, re.what());
                }
                return self;
            }

            static VALUE create_from_binfile(VALUE self, VALUE _device, VALUE _filename) {
                rb_raise(klass_eImplementStillNotSupported, "Shader::create_from_binfile implement is still not supported");
                return self;
            }

            static VALUE create_from_string(VALUE self, VALUE _device, VALUE str) {
                auto shader = GetNativeObject<::Shader>(self);
                if (!rb_obj_is_kind_of(_device, Ext::DX::D3DDevice::klass) ||
                    !rb_obj_is_kind_of(str, rb_cString)) {
                    rb_raise(rb_eArgError, "Shader::create_from_string: This first param should be a DX::D3DDevice and the \
                        second one should be a String. (No Automatically Converting).");
                }
                auto device = GetNativeObject<::D3DDevice>(_device);
                std::string code = rb_string_value_cstr(&str);
                try {
                    shader->CreateFromString(device, code);
                }
                catch (ShaderCompileError ce) {
                    rb_raise(klass_eShaderCompileError, ce.what());
                }
                catch (std::runtime_error re) {
                    rb_raise(rb_eRuntimeError, re.what());
                }
                return self;
            }

            static VALUE shader_initialize(int argc, VALUE *argv, VALUE self) {
                rb_raise(rb_eNotImpError, "Note: Shader class is only an abstract class, you should not call Shader.new");
                return Qnil;
            }

            void Init() {
                klass_eShaderCompileError = rb_define_class_under(module, "ShaderCompileError", rb_eException);
                klass = rb_define_class_under(module, "Shader", rb_cObject);
                rb_define_method(klass, "initialize", (rubyfunc)shader_initialize, -1);

                klass_vshader = rb_define_class_under(module, "VertexShader", klass);
                rb_define_alloc_func(klass_vshader, [](VALUE k)->VALUE{
                    auto s = new ::VertexShader;
                    s->AddRefer();
                    return Data_Wrap_Struct(k, nullptr, DeleteShader<::VertexShader>, s);
                });
                rb_define_method(klass_vshader, "initialize", (rubyfunc)initialize, 0);
                rb_define_method(klass_vshader, "create_from_hlsl", (rubyfunc)create_from_hlsl, 2);
                rb_define_method(klass_vshader, "create_from_binfile", (rubyfunc)create_from_binfile, 2);
                rb_define_method(klass_vshader, "create_from_string", (rubyfunc)create_from_string, 2);

                klass_pshader = rb_define_class_under(module, "PixelShader", klass);
                rb_define_alloc_func(klass_pshader, [](VALUE k)->VALUE {
                    auto s = new ::PixelShader;
                    s->AddRefer();
                    return Data_Wrap_Struct(k, nullptr, DeleteShader<::PixelShader>, s);
                });
                //copy-past-modify (*£þ¦á£þ)
                rb_define_method(klass_pshader, "initialize", (rubyfunc)initialize, 0);
                rb_define_method(klass_pshader, "create_from_hlsl", (rubyfunc)create_from_hlsl, 2);
                rb_define_method(klass_pshader, "create_from_binfile", (rubyfunc)create_from_binfile, 2);
                rb_define_method(klass_pshader, "create_from_string", (rubyfunc)create_from_string, 2);
            }
        }

        namespace RenderPipeline {
            VALUE klass;

            void Delete(::RenderPipeline *rp) {
                rp->SubRefer();
            }

            static VALUE New(VALUE k) {
                auto rp = new ::RenderPipeline;
                rp->AddRefer();
                return Data_Wrap_Struct(k, nullptr, Delete, rp);
            }

            static VALUE initialize(VALUE self) {
                if(rb_block_given_p())rb_obj_instance_eval(0, nullptr, self);
                return self;
            }

            static VALUE set_vshader(VALUE self, VALUE s) {
                auto rp = GetNativeObject<::RenderPipeline>(self);
                auto sd = GetNativeObject<::VertexShader>(s);
                rp->SetVertexShader(sd);
                rb_iv_set(self, "@vshader", s);
                return s;
            }
            static VALUE set_pshader(VALUE self, VALUE s) {
                auto rp = GetNativeObject<::RenderPipeline>(self);
                auto sd = GetNativeObject<::PixelShader>(s);
                rp->SetPixelShader(sd);
                rb_iv_set(self, "@pshader", s);
                return s;
            }
            
            static VALUE set_input_layout(VALUE self, VALUE _device, VALUE names, VALUE fmts) {
                if(!rb_obj_is_kind_of(_device, Ext::DX::D3DDevice::klass))
                    rb_raise(rb_eArgError, "RenderPipeline::set_input_layout: First param should be a DX::D3DDevice");
                if(!rb_obj_is_kind_of(names, rb_cArray) || !rb_obj_is_kind_of(fmts, rb_cArray))
                    rb_raise(rb_eArgError, "RenderPipeline::set_input_layout: 2nd and 3rd params should be Array");
                VALUE *pnames = (VALUE *)RARRAY_PTR(names);
                VALUE *pfmts = (VALUE *)RARRAY_PTR(fmts);
                int len1 = RARRAY_LEN(names);
                int len2 = RARRAY_LEN(fmts);
                if(len1 != len2)
                    rb_raise(rb_eArgError, "RenderPipeline::set_input_layout: Array lengths do not match");
                std::vector<std::string> ns;
                std::vector<DXGI_FORMAT> fs;
                for (int i = 0; i < len1; i++) {
                    ns.push_back(rb_string_value_cstr(pnames+i));
                    fs.push_back((DXGI_FORMAT)FIX2INT(pfmts[i]));
                }
                auto rp = GetNativeObject<::RenderPipeline>(self);
                auto device = GetNativeObject<::D3DDevice>(_device);
                try{
                    rp->SetInputLayout(device, ns.data(), fs.data(), len1);
                }
                catch (std::runtime_error re){
                    rb_raise(rb_eRuntimeError, re.what());
                }
                return Qnil;
            }

            void Init() {
                Ext::DX::Shader::Init();
                klass = rb_define_class_under(module, "RenderPipeline", rb_cObject);
                rb_define_alloc_func(klass, New);
                rb_define_method(klass, "initialize", (rubyfunc)initialize, 0);
                rb_attr_get(klass, rb_intern("vshader"));
                rb_attr_get(klass, rb_intern("pshader"));
                rb_define_method(klass, "set_vshader", (rubyfunc)set_vshader, 1);
                rb_define_method(klass, "set_pshader", (rubyfunc)set_pshader, 1);
                rb_alias(klass, rb_intern("vshader="), rb_intern("set_vshader"));
                rb_alias(klass, rb_intern("pshader="), rb_intern("set_pshader"));
                //inputlayout:
                rb_define_const(module, "R32G32B32A32_FLOAT", INT2FIX(DXGI_FORMAT_R32G32B32A32_FLOAT));
                rb_define_const(module, "R32G32B32_FLOAT", INT2FIX(DXGI_FORMAT_R32G32B32_FLOAT));
                rb_define_const(module, "R32G32_FLOAT", INT2FIX(DXGI_FORMAT_R32G32_FLOAT));
                rb_define_method(klass, "set_input_layout", (rubyfunc)set_input_layout, 3);
            }
        }
    }
}

