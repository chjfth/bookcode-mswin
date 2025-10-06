#ifndef SPRITES_H
#define SPRITES_H

typedef struct _spriteset
{
	LPDIRECTDRAWSURFACE		surface;
	int						stride;
	int						height;
	int						width;
}SPRITESET;
typedef SPRITESET* LPSPRITESET;

typedef void (*LPSTATE) (struct _NODE* node);

typedef struct _NODE
{
	struct _NODE*	next;                         
 	struct _NODE*	prev;   
    DWORD           dwtype;
	BYTE			status;
	double 			posx;                         
	double 			posy;                            
	double 			velx;                        
	double 			vely;            
	int 			frame;     
	BYTE			offset;
	DWORD			timedisabled;
	DWORD		 	timeborn;
    DWORD           timeupdate;
	DWORD			timeinput;
	BYTE			byinput;
	LPSTATE 		state;               
	SPRITESET* 		spriteset;
}NODE;
typedef NODE* LPNODE;

#define SPRITE_SHIP 1
#define SPRITE_SHOT 2

#define STATUS_OK	1
#define STATUS_HIT	2

// Prototypes

void	InitLinkedList( void );
void	CloseLinkedList( void );
void	UpdateStates( void );
BOOL	CheckHit( LPNODE, LPNODE );
BOOL	CheckForHits( LPNODE );
HRESULT DrawSprites( LPDIRECTDRAWSURFACE, BOOL );
HRESULT DrawSprite( LPNODE, LPDIRECTDRAWSURFACE );
void	AddNode ( LPNODE  );
void	RemoveNode ( LPNODE );

#endif // SPRITES_H
