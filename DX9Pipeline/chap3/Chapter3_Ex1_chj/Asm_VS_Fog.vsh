vs_1_1              // version instruction
#define fogStart c9.x
#define fogEnd   c9.z

def c9,  2, -1, 2.66, -1  // fog start & end values
def c10, 0, -1, 1,    -1  

dcl_position v0
dcl_texcoord v7

m4x4 r0, v0, c0    // transform vertices by world-view-projection matrix
mov oPos, r0
mov oT0, v7

m4x4 r1, v0, c4          // transform vertices by world-view matrix

mov r2.x, fogStart       //                         r2.x = 2
sub r2.y, fogEnd, r2.x   // (fog end - fog start)   r2.y = 0.66
rcp r2.z, r2.y           // 1/(fog end - fog start) r2.z = 1.515

// Assume that r1.z = 2.10 (a little inside fog)

sub r3.z, fogEnd, r1.z   // (fog end - distance)      r3.z = 0.56
mul r3.x, r3.z, r2.z     // (fog end - distance)/(fog end - fog start)
                         //                           r3.x = 0.8484 (ratio)

max r3.x, c10.x, r3.x    // clamp above 0
min r3.x, c10.z, r3.x    // clamp below 1
mov oFog, r3.x           // output per-vertex fog factor
