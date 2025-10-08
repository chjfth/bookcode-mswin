/*
    Beginning DirectX 11 Game Programming
    By Allen Sherrod and Wendy Jones

    Blank Direct3D Demo Class - Demo that displays a blank Direct3D window.
*/


#include"BlankDemo.h"
#include<xnamath.h>


BlankDemo::BlankDemo( )
{

}


BlankDemo::~BlankDemo( )
{

}


bool BlankDemo::LoadContent( )
{
    return true;
}


void BlankDemo::UnloadContent( )
{

}


void BlankDemo::Update( float dt )
{

}


void BlankDemo::Render( )
{
    if( d3dContext_ == 0 )
        return;

    float clearColor[4] = { 0.0f, 0.0f, 0.25f, 1.0f };
    d3dContext_->ClearRenderTargetView( backBufferTarget_, clearColor );

    swapChain_->Present( 0, 0 );
}