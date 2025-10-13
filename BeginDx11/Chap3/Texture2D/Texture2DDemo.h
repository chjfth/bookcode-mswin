/*
    Beginning DirectX 11 Game Programming
    By Allen Sherrod and Wendy Jones

    TextureDemo - Demonstrates texture mapping in Direct3D 11.
*/


#ifndef _TEXTURE_2D_DEMO_H_
#define _TEXTURE_2D_DEMO_H_

#include"Dx11DemoBase.h"


class TextureDemo : public Dx11DemoBase
{
    public:
        TextureDemo( );
        virtual ~TextureDemo( );

        bool LoadContent( );
        void UnloadContent( );

        void Update( float dt );
        void Render( );

    private:
        ID3D11VertexShader* solidColorVS_;
        ID3D11PixelShader* solidColorPS_;

        ID3D11InputLayout* inputLayout_;
        ID3D11Buffer* vertexBuffer_;

        ID3D11ShaderResourceView* colorMap_;
        ID3D11SamplerState* colorMapSampler_;
};

#endif