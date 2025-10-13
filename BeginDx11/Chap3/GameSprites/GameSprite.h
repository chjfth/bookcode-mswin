/*
    Beginning DirectX 11 Game Programming
    By Allen Sherrod and Wendy Jones

    GameSprite - Used to represent a 2D sprite's transformation.
*/


#ifndef _GAME_SPRITE_H_
#define _GAME_SPRITE_H_

#include<xnamath.h>


class GameSprite
{
    public:
        GameSprite( );
        virtual ~GameSprite( );

        XMMATRIX GetWorldMatrix( );

        void SetPosition( XMFLOAT2& position );
        void SetRotation( float rotation );
        void SetScale( XMFLOAT2& scale );


    private:
        XMFLOAT2 position_;
        float rotation_;
        XMFLOAT2 scale_;
};

#endif