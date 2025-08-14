#include "LJMUGeometryActor.h"



void LJMUGeometryActor::UseLitTextureMaterial(ResourcePtr texture)
{
    m_pLitTextureMaterial->Parameters.SetShaderResourceParameter(L"ColorTexture", texture);
    GetBody()->SetMaterial(m_pLitTextureMaterial);
}

void LJMUGeometryActor::UseSkyBoxMaterial(ResourcePtr texture)
{
    m_pSkyBoxMaterial->Parameters.SetShaderResourceParameter(L"ColorTexture", texture);
    GetBody()->SetMaterial(m_pSkyBoxMaterial);
}

MaterialPtr LJMUGeometryActor::createLitTexturedMaterial() {

    RendererDX11* pRenderer = RendererDX11::Get();
    MaterialPtr Material = MaterialPtr(new MaterialDX11);

    //create and fill the effect

    RenderEffectDX11* pEffect = new RenderEffectDX11();

    pEffect->SetVertexShader(pRenderer->LoadShader(VERTEX_SHADER,
        std::wstring(L"LJMULitTexture.hlsl"),
        std::wstring(L"VSMAIN"),
        std::wstring(L"vs_4_0")));


    pEffect->SetPixelShader(pRenderer->LoadShader(PIXEL_SHADER,
        std::wstring(L"LJMULitTexture.hlsl"),
        std::wstring(L"PSMAIN"),
        std::wstring(L"ps_4_0")));


    SamplerStateConfigDX11 SamplerConfig;
    SamplerConfig.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    SamplerConfig.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    SamplerConfig.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    SamplerConfig.MaxAnisotropy = 16;

    int TextureSampler = RendererDX11::Get()->CreateSamplerState(&SamplerConfig);
    Material->Parameters.SetSamplerParameter(L"TextureSampler", TextureSampler);

    //Enable the material to render the given view and set its effect
    Material->Params[VT_PERSPECTIVE].bRender = true;
    Material->Params[VT_PERSPECTIVE].pEffect = pEffect;

    return Material;
}
MaterialPtr LJMUGeometryActor::SkyBoxmaterial()
{
    RendererDX11* pRenderer = RendererDX11::Get();
    MaterialPtr Material = MaterialPtr(new MaterialDX11);

    //create and fill the effect

    RenderEffectDX11* pEffect = new RenderEffectDX11();

    pEffect->SetVertexShader(pRenderer->LoadShader(VERTEX_SHADER,
        std::wstring(L"LJMUTextureMap.hlsl"),
        std::wstring(L"VSMAIN"),
        std::wstring(L"vs_4_0")));


    pEffect->SetPixelShader(pRenderer->LoadShader(PIXEL_SHADER,
        std::wstring(L"LJMUTextureMap.hlsl"),
        std::wstring(L"PSMAIN"),
        std::wstring(L"ps_4_0")));


    SamplerStateConfigDX11 SamplerConfig;
    SamplerConfig.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    SamplerConfig.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    SamplerConfig.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    SamplerConfig.MaxAnisotropy = 16;

    int TextureSampler = RendererDX11::Get()->CreateSamplerState(&SamplerConfig);
    Material->Parameters.SetSamplerParameter(L"TextureSampler", TextureSampler);

    RasterizerStateConfigDX11 rsConfig;
    rsConfig.CullMode = D3D11_CULL_NONE;
    rsConfig.FillMode = D3D11_FILL_SOLID;

    int rasterizerState = pRenderer->CreateRasterizerState(&rsConfig);
    pEffect->m_iRasterizerState = rasterizerState;

    //Enable the material to render the given view and set its effect
    Material->Params[VT_PERSPECTIVE].bRender = true;
    Material->Params[VT_PERSPECTIVE].pEffect = pEffect;

    return Material;
}

LJMUGeometryActor::LJMUGeometryActor()
{
    m_pLitTextureMaterial = createLitTexturedMaterial();
    GetBody()->SetMaterial(m_pLitTextureMaterial);
    m_pSkyBoxMaterial = SkyBoxmaterial();
    GetBody()->SetMaterial(m_pSkyBoxMaterial);
}