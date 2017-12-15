#include "stdafx.h"
#include "Shader.h"

template<typename T, const char *profile, const std::function<bool(ID3D10Blob*, T**)> &op>
ComPtr<T> Shader::Initialize(const wstring &filename, D3D11_INPUT_ELEMENT_DESC *desc, int numdesc, bool checklayout) {
    RCHECK(SUCCEEDED(D3DX11CompileFromFileW(filename.c_str(), 0, 0, "main", profile, 0, 0, 0, &blob, 0, 0)),
        L"±‡“Î◊≈…´∆˜ ß∞‹");

    /*
    D3D11_INPUT_ELEMENT_DESC ipdesc[2];
    //  POSITION
    ipdesc[0].SemanticName = "POSITION";
    ipdesc[0].SemanticIndex = 0U;
    ipdesc[0].Format = DXGI_FORMAT_R32G32_FLOAT;
    ipdesc[0].InputSlot = 0U;
    ipdesc[0].AlignedByteOffset = 0U;
    ipdesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    ipdesc[0].InstanceDataStepRate = 0U;
    //  TEXCOORD0
    ipdesc[1].SemanticName = "TEXCOORD";
    ipdesc[1].SemanticIndex = 0U;
    ipdesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    ipdesc[1].InputSlot = 0U;
    ipdesc[1].AlignedByteOffset = sizeof(DirectX::XMFLOAT2);
    ipdesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    ipdesc[1].InstanceDataStepRate = 0U;
    */

    if (checklayout) {
        RCHECK(SUCCEEDED(g_renderer->CreateInputLayout(desc, numdesc,
            blob->GetBufferPointer(), blob->GetBufferSize(), &input_layout)), L"¥¥Ω®InputLayout ß∞‹");
    }

    ComPtr<T> shader;
    op(blob.Get(), &shader);
    return shader;
}