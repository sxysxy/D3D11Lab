#pragma once

#include "stdafx.h"
#include "global.h"

class Shader {

    ComPtr<ID3D10Blob> blob;
    ComPtr<ID3D11InputLayout> input_layout;

    template<typename T, const char *profile, const std::function<bool(ID3D10Blob*, T**)> &op>
    ComPtr<T> Initialize(const wstring &filename, D3D11_INPUT_ELEMENT_DESC *desc, int numdesc, bool checklayout);
};