vs_1_1
; Constants:
;
; c0-c3 - View+Projection matrix
;
; c4.x - time
; c4.y - 0
; c4.z - 0.5
; c4.w - 1.0
;
; c7.x - pi
; c7.y - 1/2pi
; c7.z - 2pi
; c7.w - 0.05
;
; c10 - first four Taylor coefficients for sin(x)
; c11 - first four Taylor coefficients for cos(x)

dcl_position v0

; Decompress position
mov r0.x, v0.x
mov r0.y, c4.w ; 1
mov r0.z, v0.y
mov r0.w, c4.w ; 1

; Compute theta from distance and time
mov r4.xz, r0		; r4.x = r0.x, r4.z = r0.z
mov r4.y, c4.y		; r4.y = 0
dp3 r4.x, r4, r4	; r4.x = x² + z²
rsq r4.x, r4.x		; r4.x = 1/sqrt(r4.x)
rcp r4.x, r4.x		; final: d = sqrt(x² + z²)
					; d implies distance from ripple center

mul r4.x, r4, c4.x	; r4.x scale by time

; Clamp theta to -pi..pi
; ChatGPT: Old shader models cannot do modulus for floats cleanly, 
; so the book author use this trick:
add r4.x, r4.x, c7.x	; +π   → shift positive
mul r4.x, r4.x, c7.y	; *1/(2π)
frc r4.xy, r4.x			; fractional part (0..1)
mul r4.x, r4.x, c7.z	; *2π  → now 0..2π
add r4.x, r4.x,-c7.x	; -π → now -π..π

; Compute first four values in sin and cos series
mov r5.x, c4.w			; d^0
mov r4.x, r4.x			; d^1
mul r5.y, r4.x, r4.x	; d^2
mul r4.y, r4.x, r5.y	; d^3
mul r5.z, r5.y, r5.y	; d^4
mul r4.z, r4.x, r5.z	; d^5
mul r5.w, r5.y, r5.z	; d^6
mul r4.w, r4.x, r5.w	; d^7

; We now have:
; r4 = [θ¹, θ³, θ⁵, θ⁷]
; r5 = [θ⁰, θ², θ⁴, θ⁶]

mul r4, r4, c10			; do Taylor
dp4 r4.x, r4, c4.w		; sin(θ) = r4.x + r4.y + r4.z + r4.w , into r4.x
mul r5, r5, c11			; do Taylor
dp4 r5.x, r5, c4.w		; cos(θ) = r5.x + r5.y + r5.z + r5.w , into r5.x

; Set color
add r5.x, -r5.x, c4.w	; =1-cos(θ) , range is 0..2
mul oD0, r5.x, c4.z		; *0.5, make it 0..1

; Scale height
mul r0.y, r4.x, c7.w	; (final) y = sin(d * time) * 0.05

; Transform position
dp4 oPos.x, r0, c0
dp4 oPos.y, r0, c1
dp4 oPos.z, r0, c2
dp4 oPos.w, r0, c3

