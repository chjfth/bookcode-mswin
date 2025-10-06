#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <ddraw.h>
#include "sprites.h"
#include "brouhaha.h"

// Globals
LPNODE g_bottom_node;
LPNODE g_top_node;

void InitLinkedList( void )
{
    g_bottom_node = NULL;
    g_top_node = NULL;
}

void CloseLinkedList( void )
{
	LPNODE next_node = (LPNODE) NULL;
    LPNODE thenode;

	for ( thenode=g_bottom_node; thenode != (LPNODE)NULL; thenode=next_node )
    {
        next_node = thenode->next;
		free( thenode );
    }

	g_bottom_node = NULL;
	g_top_node = NULL;
}

void UpdateStates( void )
{
	// Go through each node in the list,
	// invoking its state function.

    LPNODE next_node = NULL;
    LPNODE thenode;

    for ( thenode=g_bottom_node; thenode!=(LPNODE)NULL; thenode=next_node )
    {
        next_node = thenode->next;
        
		if ( thenode->state ) 
			thenode->state( thenode );
    }

}

BOOL CheckHit( LPNODE ship, LPNODE shot )
{
	// Make a crude check to see if a ship is
	// occupying the same place as a shot

	if ( shot->posx < ship->posx ) 
		return FALSE;
	
	if ( shot->posy < ship->posy ) 
		return FALSE;
	
	if ( shot->posx > (ship->posx + 32.0) ) 
		return FALSE;
	
	if ( shot->posy > (ship->posy + 32.0) ) 
		return FALSE;

	return TRUE;
}

BOOL CheckForHits( LPNODE ship )
{
	// Go through all the nodes checking shot nodes for
	// collisions with the passed ship.

    LPNODE next_node = NULL;
    LPNODE thenode;

    for ( thenode=g_bottom_node; thenode!=(LPNODE)NULL; thenode=next_node )
    {
        next_node = thenode->next;
		if ( thenode->dwtype == SPRITE_SHOT )
		{
			if ( CheckHit( ship,  thenode ) )
			{
				// Disable the ship
				ship->offset = 0;
				ship->spriteset = &g_ghostsprite;
				ship->status = STATUS_HIT;
				ship->timedisabled = timeGetTime();
				// No need to check further
				return TRUE;		
			}
		}
    }
	return FALSE;
}

HRESULT DrawSprites( 
                 LPDIRECTDRAWSURFACE lpDDSSurface,  // destination surface
                 BOOL bDrawOrder                    // drawing order
                 )
{
	// Draw all the sprites in the list,
	// top down or bottom up.

    LPNODE	next_node = (LPNODE) NULL;
    LPNODE	thenode;
    HRESULT ddrval;

    if ( bDrawOrder )
    // start at the bottom of the list and work up -- items added most
    // recently will be drawn first
    {
        for ( thenode=g_top_node; thenode!=(LPNODE)NULL; thenode=next_node )
        {
            next_node = thenode->prev;
            ddrval = DrawSprite( thenode, lpDDSSurface );
            if FAILED( ddrval ) 
				return ddrval;
        }
    }
    else
    // start at the top of the list and work down -- items will be drawn in the
    // order they were added
    {
        for ( thenode=g_bottom_node; thenode!=(LPNODE)NULL; thenode=next_node )
        {
            next_node = thenode->next;
            ddrval = DrawSprite( thenode, lpDDSSurface );
            if FAILED( ddrval ) 
				return ddrval;
        }
    }

    return TRUE;
}

HRESULT DrawSprite( 
                LPNODE drawnode,                    // the sprite node
                LPDIRECTDRAWSURFACE lpDDSSurface    // destination surface
                )
{
    HRESULT ddrval;
    RECT    src;

    src.left = ( ( drawnode->frame+drawnode->offset ) %
                        drawnode->spriteset->stride ) *
                        drawnode->spriteset->width;
    src.top = ( ( drawnode->frame+drawnode->offset ) /
                        drawnode->spriteset->stride ) *
                        drawnode->spriteset->height;
    src.right = src.left + drawnode->spriteset->width;
    src.bottom = src.top + drawnode->spriteset->height;

    drawnode->timeupdate = timeGetTime();

    ddrval = lpDDSSurface->BltFast(
                        (DWORD)drawnode->posx,
                        (DWORD)drawnode->posy,
                        drawnode->spriteset->surface,
                        &src,
                        DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT );

    if FAILED( ddrval )
    {
        OutputDebugString( "DrawSprite: BltFast failed.\n" );
        return ddrval;
    } 

    return ddrval;
}

void AddNode ( 
              LPNODE newNode    // the node to be added
              )
{
    if (g_bottom_node == (LPNODE) NULL )
    {
        g_bottom_node = newNode;
        newNode->prev = (LPNODE) NULL ;
    }
    else
    {
        newNode->prev = g_top_node;
        newNode->prev->next = newNode;
    }
    g_top_node = newNode;
    newNode->next = (LPNODE) NULL;
}

void RemoveNode(
                LPNODE node     // the node to be removed
                )
{
    if (node == g_bottom_node)
    {
        g_bottom_node = node->next;
        if ( g_bottom_node != (LPNODE) NULL )
        {
            g_bottom_node->prev = (LPNODE) NULL;
        }
    }
    else if (node == g_top_node)
    {
        g_top_node = node->prev;
        g_top_node->next = (LPNODE) NULL;
    }
    else
    {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }
    free( node );
}

