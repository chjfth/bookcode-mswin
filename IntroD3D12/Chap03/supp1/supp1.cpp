// [2025-10-24] Chj: 
//This program shows that, the following operations are NOT equal:
// 1. Apply 3  rotations on inputv, along X-axis, then Y-axis, then Z-axis,
// 2. Apply one rotation on inputv, along axis(1,1,1).

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


void TestRotateXYZ(FXMVECTOR inputv, float degree)
{
	float rad = degree * XM_PI / 180;

	// Experiment one: Rotate along X-axis, then Y-axis, then Z-axis

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

	float epsilon = 0.00001f;
	assert( myXMMatrixNearEqual(Rx, XMMatrixRotationX(rad), epsilon) );
	assert( myXMMatrixNearEqual(Ry, XMMatrixRotationY(rad), epsilon) );
	assert( myXMMatrixNearEqual(Rz, XMMatrixRotationZ(rad), epsilon) );

	XMVECTOR xdone = XMVector3TransformNormal(inputv , Rx);
	XMVECTOR ydone = XMVector3TransformNormal(xdone, Ry);
	XMVECTOR zdone = XMVector3TransformNormal(ydone, Rz);

	cout << "Rotate "<< degree <<" degree along X-axis, Y-axis, then Z-axis." << endl;
	cout << "Input  vector: " << inputv << endl;
	cout << "Output vector: " << zdone << endl;

	cout << endl;

	// Experiment two: Rotate along axis(1,1,1).

	XMVECTOR axis111 = XMVectorSet(1, 1, 1, 0);
	XMMATRIX rmatrix = XMMatrixRotationAxis(axis111, rad);

	XMVECTOR rdone = XMVector3TransformNormal(inputv, rmatrix);

	cout << "Rotate " << degree << " degree along axis(1,1,1)." << endl;
	cout << "Input  vector: " << inputv << endl;
	cout << "Output vector: " << rdone << endl;

	XMVECTOR Epsilon = XMVectorReplicate(epsilon);
	assert(!XMVector3NearEqual(zdone, rdone, Epsilon));

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

	XMVECTOR inputv = XMVectorSet(3, 2, 1, 0);
//	inputv = inputv * 2;
	TestRotateXYZ(inputv, 30);

	return 0;
}