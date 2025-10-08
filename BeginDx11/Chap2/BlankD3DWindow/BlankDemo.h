/*
    Beginning DirectX 11 Game Programming
    By Allen Sherrod and Wendy Jones

    Blank Direct3D Demo Class - Demo that displays a blank Direct3D window.
*/


#ifndef _BLANK_DEMO_H_
#define _BLANK_DEMO_H_

#include"Dx11DemoBase.h"


class BlankDemo : public Dx11DemoBase
{
    public:
        BlankDemo( );
        virtual ~BlankDemo( );

        bool LoadContent( );
        void UnloadContent( );

        void Update( float dt );
        void Render( );
};

#endif