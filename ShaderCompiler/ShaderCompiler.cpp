// ShaderCompiler.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

char psf[] = "ps_4_0";
char pvf[] = "vs_4_0";
wchar_t buffer[1024];
int _tmain(int argc, LPCWSTR argv[]){
    if (argc != 3) {
        printf("Usage: sc filename ps/vs. \n  Output: filename.bin");
    }
    else {
        ComPtr<ID3D10Blob> sd, err;
        char *pf;
        if(argv[2][0] == L'p')pf = psf;
        else if(argv[2][0] == L'v')pf = pvf;
        else {
            printf("Profile not supported. (vs or ps only)");
            return 0;
        }
        
        HRESULT hr = D3DX11CompileFromFileW(argv[1], 0, 0, "main", pf, 0, 0, 0, &sd, &err, nullptr);
        if (FAILED(hr)) {
            if(err->GetBufferPointer())
                printf("Complie Error: %s", (LPCSTR)err->GetBufferPointer());
            else {
                printf("Complie Error, unknown reason.");
            }
        }
        else {
            lstrcpyW(buffer, argv[1]);
            int len = lstrlenW(buffer);
            int p = len;
            for (int i = len - 1; ~i; i--)if (buffer[i] == L'.') {
                p = i;
                break;
            }
            buffer[p] = L'.';
            buffer[p+1] = L'b';
            buffer[p+2] = L'i';
            buffer[p+3] = L'n';
            buffer[p+4] = 0;
            FILE *fout = _wfopen(buffer, L"wb");
            int size = sd->GetBufferSize();
            fwrite(&size, 4, 1, fout);
            fwrite(sd->GetBufferPointer(), sd->GetBufferSize(), 1, fout);
            fclose(fout);
            wprintf(L"OK, Output %s\n", buffer);
            
        }
    }

    return 0;
}

