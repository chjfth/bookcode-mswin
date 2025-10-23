// Chj: Answering the question:
// Build a rotation matrix that rotates 30бу along the axis (1, 1, 1)

#include <assert.h>
#include <windows.h> // for XMVerifyCPUSupport
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <iostream>
using namespace std;
using namespace DirectX;
using namespace DirectX::PackedVector;

// Overload the  "<<" operators so that we can use cout to 
// output XMVECTOR and XMMATRIX objects.
ostream& XM_CALLCONV operator << (ostream& os, FXMVECTOR v)
{
	XMFLOAT4 dest;
	XMStoreFloat4(&dest, v);

	os << "(" << dest.x << ", " << dest.y << ", " << dest.z << ", " << dest.w << ")";
	return os;
}

ostream& XM_CALLCONV operator << (ostream& os, FXMMATRIX m)
{
	for (int i = 0; i < 4; ++i)
	{
		os << XMVectorGetX(m.r[i]) << "\t";
		os << XMVectorGetY(m.r[i]) << "\t";
		os << XMVectorGetZ(m.r[i]) << "\t";
		os << XMVectorGetW(m.r[i]);
		os << endl;
	}
	return os;
}

inline bool myXMMatrixNearEqual(FXMMATRIX M1, FXMMATRIX M2, float epsilon)
{
	XMVECTOR Epsilon = XMVectorReplicate(epsilon);

	bool eq0 = XMVector4NearEqual(M1.r[0], M2.r[0], Epsilon);

	return XMVector4NearEqual(M1.r[0], M2.r[0], Epsilon)
		&& XMVector4NearEqual(M1.r[1], M2.r[1], Epsilon)
		&& XMVector4NearEqual(M1.r[2], M2.r[2], Epsilon)
		&& XMVector4NearEqual(M1.r[3], M2.r[3], Epsilon);
}


XMMATRIX myCalRotateMatrix(XMVECTOR axis, float rad)
{
	XMVECTOR modulen = XMVector3Length(axis);
	XMVECTOR axisunit = axis / modulen;

	float x = XMVectorGetX(axisunit);
	float y = XMVectorGetY(axisunit);
	float z = XMVectorGetZ(axisunit);

	float s = sin(rad);
	float c = cos(rad);

	XMMATRIX Rn(
		c+(1-c)*x*x,    (1-c)*x*y+s*z,  (1-c)*x*z-s*y,  0,
		(1-c)*x*y-s*z , c+(1-c)*y*y,    (1-c)*y*z+s*x,  0,
		(1-c)*x*z+s*y , (1-c)*y*z-s*x,  c+(1-c)*z*z,    0,
		0 ,0 ,0, 1); // final `1` in accordance with XMMatrixRotationAxis().

	// [2025-10-23] Pending Q: XMMatrixRotationAxis() internally does NOT use such direct 
	// matrix element calculation. How does he do it?

	return Rn; 
	
	// TODO: Can I apply C++11 Move Semantics?
}


void TestRotate(float degree)
{
	float rad = degree * XM_PI / 180;

	// We will rotate along (1,1,1), set this in axis111.
	XMVECTOR axis111 = XMVectorSet(1, 1, 1, 0);

	XMMATRIX answer = XMMatrixRotationAxis(axis111, rad);

	cout << "Chap03 Ex5, XMMatrixRotationAxis() "<< degree <<" degree, produces answer:" 
		<< endl << answer;

/*
	XMMATRIX Rx(
		1,  0,        0,        0,
		0,  cos(rad), sin(rad), 0,
		0 ,-sin(rad), cos(rad), 0, 
		0,  0,        0,        1);
	XMMATRIX Ry(
		cos(rad), 0, -sin(rad), 0,
		0,        1,         0, 0,
		sin(rad), 0,  cos(rad), 0,
		0,        0,         0, 1);
	XMMATRIX Rz(
		 cos(rad), sin(rad), 0, 0,
		-sin(rad), cos(rad), 0, 0,
		 0,        0,        1, 0,
		 0,        0,        0, 1);

	XMVECTOR unitx = XMVectorSet(1, 0, 0, 0);
	XMVECTOR unity = XMVectorSet(1, 1, 0, 0);
	XMVECTOR unitz = XMVectorSet(0, 0, 1, 0);
*/

	cout << endl;
	cout << "Now we verify it using p64 Rn matrix. Result:" << endl;

	XMMATRIX Rn = myCalRotateMatrix(axis111, rad);
	cout << Rn << endl;

	bool equal = myXMMatrixNearEqual(answer, Rn, 0.00001f);
	cout << "Near equal: " << equal << endl;
	
	assert(equal);
}

int main()
{
	// Check support for SSE2 (Pentium4, AMD K8, and above).
	if (!XMVerifyCPUSupport())
	{
		cout << "directx math not supported" << endl;
		return 0;
	}

	cout.setf(ios_base::boolalpha);

	TestRotate(30);

	return 0;
}