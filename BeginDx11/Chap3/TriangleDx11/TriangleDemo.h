/*
    Beginning DirectX 11 Game Programming
    By Allen Sherrod and Wendy Jones

    Triangle Demo - Demo used to draw a 2D triangle to the screen.
*/


#ifndef _TRIANGLE_DEMO_H_
#define _TRIANGLE_DEMO_H_

#include"Dx11DemoBase.h"


class TriangleDemo : public Dx11DemoBase
{
    public:
        TriangleDemo( );
        virtual ~TriangleDemo( );

        bool LoadContent( );
        void UnloadContent( );

        void Update( float dt );
        void Render( );

    private:
        ID3D11VertexShader* solidColorVS_;
        ID3D11PixelShader* solidColorPS_;

        ID3D11InputLayout* inputLayout_;
        ID3D11Buffer* vertexBuffer_;
};

#endif