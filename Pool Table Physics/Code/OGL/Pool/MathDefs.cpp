///////////////////////////////////////////////////////////////////////////////
//
// MathDefs.c : implementation file
//
// Purpose:	Implementation of Math Routines
//
// Created:
//		JL  2/18/98
// Revisions:
//
///////////////////////////////////////////////////////////////////////////////
//
//	Copyright 1998 Jeff Lander, All Rights Reserved.
//  For educational purposes only.
//  Please do not republish in electronic or print form without permission
//  Thanks - jeffl@darwin3d.com
//
///////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include "mathdefs.h"

///////////////////////////////////////////////////////////////////////////////
// Function:	IdentityMatrix
// Purpose:		Creates and Identity 4x4 Matrix in OpenGL Format
// Arguments:	Matrix
// Notes:		This routing is tweaked to handle OpenGLs column-major format
//				This is one obvious place for optimization perhaps asm code
///////////////////////////////////////////////////////////////////////////////
void IdentityMatrix(tMatrix *mat) 
{
///// Local Variables /////////////////////////////////////////////////////////
	int loop;
///////////////////////////////////////////////////////////////////////////////
	for (loop = 0; loop < 16; loop++)
		mat->m[loop] = 0.0f;
	mat->m[0] = 
	mat->m[5] = 
	mat->m[10] = 
	mat->m[15] = 
		1.0f;
}
//// IdentityMatrix ///////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// tVector Class
//////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Function:	equals operator
// Purpose:		Sets the current vector to equal list of floats
// Arguments:	float * 
///////////////////////////////////////////////////////////////////////////////
tVector tVector::operator=( float *arg )
{
	x = arg[0];
	y = arg[1];
	z = arg[2];
	w = arg[3];
	
	return *this;
}


///////////////////////////////////////////////////////////////////////////////
// Function:	addition operator
// Purpose:		Subtracts a tVector from the current vector
// Arguments:	tVector 
///////////////////////////////////////////////////////////////////////////////
tVector tVector::operator+=( tVector &v )
{
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;

	return *this;
}

///////////////////////////////////////////////////////////////////////////////
// Function:	subtraction operator
// Purpose:		Subtracts a tVector from the current vector
// Arguments:	tVector 
///////////////////////////////////////////////////////////////////////////////
tVector tVector::operator-=( tVector &v )
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;

	return *this;
}

///////////////////////////////////////////////////////////////////////////////
// Function:	multiplication operator
// Purpose:		Multiply a tVector by float
// Arguments:	float
///////////////////////////////////////////////////////////////////////////////
tVector tVector::operator*( float arg )
{
	tVector v;

	v.x = x * arg;
	v.y = y * arg;
	v.z = z * arg;
	v.w = w * arg;

	return v;
}

///////////////////////////////////////////////////////////////////////////////
// Function:	division operator
// Purpose:		Divides a tVector by float
// Arguments:	float
///////////////////////////////////////////////////////////////////////////////
tVector tVector::operator/( float arg )
{
	tVector v;

	v.x = x / arg;
	v.y = y / arg;
	v.z = z / arg;
	v.w = w / arg;

	return v;
}

///////////////////////////////////////////////////////////////////////////////
// Function:	multiplication operator
// Purpose:		Multiply a tVector by float
// Arguments:	float
///////////////////////////////////////////////////////////////////////////////
tVector tVector::operator*=( float arg )
{
	tVector v;

	x *= arg;
	y *= arg;
	z *= arg;
	w *= arg;

	return *this;
}

///////////////////////////////////////////////////////////////////////////////
// Function:	division operator
// Purpose:		Divides a tVector by float
// Arguments:	float
///////////////////////////////////////////////////////////////////////////////
tVector tVector::operator/=( float arg )
{
	tVector v;

	x /= arg;
	y /= arg;
	z /= arg;
	w /= arg;

	return *this;
}

///////////////////////////////////////////////////////////////////////////////
// Function:	SquaredLength
// Purpose:		Returns the squared length of the vector
// Arguments:	none
///////////////////////////////////////////////////////////////////////////////
float tVector::Length2() 
{
	return((x * x) + (y * y) + (z * z) + (w * w));
}

///////////////////////////////////////////////////////////////////////////////
// Function:	Length
// Purpose:		Returns the length of the vector
// Arguments:	none
///////////////////////////////////////////////////////////////////////////////
float tVector::Length() 
{
	return((float)sqrt(Length2()));
}

///////////////////////////////////////////////////////////////////////////////
// Function:	NormalizeVector
// Purpose:		Normalizes the class vector
// Arguments:	none
///////////////////////////////////////////////////////////////////////////////
void tVector::NormalizeVector() 
{
	float len = Length();
    if (len != 0.0) 
	{ 
		x /= len;  
		y /= len; 
		z /= len; 
	}
}

///////////////////////////////////////////////////////////////////////////////
// Function:	Lerp
// Purpose:		Interpolates between vector and another by a factor
// Arguments:	Vector to lerp to and factor
///////////////////////////////////////////////////////////////////////////////
void tVector::Lerp(tVector *v1, float factor) 
{
	x = x * (1.0f - factor) + v1->x * factor; 
	y = y * (1.0f - factor) + v1->y * factor; 
	z = z * (1.0f - factor) + v1->z * factor; 
	w = w * (1.0f - factor) + v1->w * factor; 
}

///////////////////////////////////////////////////////////////////////////////
// Function:	CrossProduct
// Purpose:		Sets the vector to equal the cross product of two vectors
// Arguments:	tVector *
///////////////////////////////////////////////////////////////////////////////
void tVector::CrossProduct(tVector &v1, tVector &v2)
{
	x = (v1.y * v2.z) - (v1.z * v2.y);
	y = (v1.z * v2.x) - (v1.x * v2.z);
	z = (v1.x * v2.y) - (v1.y * v2.x);
}

///////////////////////////////////////////////////////////////////////////////
// Function:	Dot
// Purpose:		Return the dot product of two vectors
// Arguments:	tVector *
///////////////////////////////////////////////////////////////////////////////
float tVector::Dot(tVector *v)
{
	return ((x * v->x) + (y * v->y) + (z * v->z) + (w * v->w));
}

///////////////////////////////////////////////////////////////////////////////
// Function:	Dot
// Purpose:		Return the dot product of two vectors
// Arguments:	tVector *
///////////////////////////////////////////////////////////////////////////////
float tVector::Dot(tVector &v)
{
	return ((x * v.x) + (y * v.y) + (z * v.z) + (w * v.w));
}

///////////////////////////////////////////////////////////////////////////////
// Function:	Dot
// Purpose:		Return the dot product of two vectors
// Arguments:	vector elements
///////////////////////////////////////////////////////////////////////////////
float tVector::Dot(float vx, float vy, float vz, float vw)
{
	return ((x * vx) + (y * vy) + (z * vz) + (w * vw));
}

///////////////////////////////////////////////////////////////////////////////
// Function:	MultVectorByMatrix
// Purpose:		Multiplies a vector by a 4x4 Matrix in OpenGL Format
// Arguments:	Matrix
// Notes:		This routing is tweaked to handle OpenGLs column-major format
//				This is one obvious place for optimization perhaps asm code
///////////////////////////////////////////////////////////////////////////////
void tVector::MultVectorByMatrix(float *mat)
{
	tVector result;
	result.x = (mat[0] * x) +
			   (mat[4] * y) +	
			   (mat[8] * z) +
			   (mat[12] * w);
	result.y = (mat[1] * x) +
			   (mat[5] * y) +	
			   (mat[9] * z) +
			   (mat[13] * w);
	result.z = (mat[2] * x) +
			   (mat[6] * y) +	
			   (mat[10] * z) +
			   (mat[14] * w);
	result.w = (mat[3] * x) +
			   (mat[7] * y) +	
			   (mat[11] * z) +
			   (mat[15] * w);

	x = result.x;
	y = result.y;
	z = result.z;
	w = result.w;
}
//// MultVectorByMatrix //////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Function:	MultVectorByRotMatrix
// Purpose:		Multiplies a vector by a rotation only 4x4 Matrix in OpenGL Format
// Arguments:	Matrix
// Notes:		This routing is tweaked to handle OpenGLs column-major format
//				This is one obvious place for optimization perhaps asm code
///////////////////////////////////////////////////////////////////////////////
void tVector::MultVectorByRotMatrix(float *mat)
{
	tVector result;
	result.x = (mat[0] * x) +
			   (mat[4] * y) +	
			   (mat[8] * z);
	result.y = (mat[1] * x) +
			   (mat[5] * y) +	
			   (mat[9] * z);
	result.z = (mat[2] * x) +
			   (mat[6] * y) +	
			   (mat[10] * z);

	x = result.x;
	y = result.y;
	z = result.z;
}
//// MultVectorByRotMatrix //////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// End of tVector Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// tVector3 Class
//////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Function:	equals operator
// Purpose:		Sets the current vector to equal list of floats
// Arguments:	float * 
///////////////////////////////////////////////////////////////////////////////
tVector3 tVector3::operator=( float *arg )
{
	x = arg[0];
	y = arg[1];
	z = arg[2];
	
	return *this;
}


///////////////////////////////////////////////////////////////////////////////
// Function:	addition operator
// Purpose:		Subtracts a tVector3 from the current vector
// Arguments:	tVector3 
///////////////////////////////////////////////////////////////////////////////
tVector3 tVector3::operator+=( tVector3 &v )
{
	x += v.x;
	y += v.y;
	z += v.z;

	return *this;
}

///////////////////////////////////////////////////////////////////////////////
// Function:	subtraction operator
// Purpose:		Subtracts a tVector3 from the current vector
// Arguments:	tVector3 
///////////////////////////////////////////////////////////////////////////////
tVector3 tVector3::operator-=( tVector3 &v )
{
	x -= v.x;
	y -= v.y;
	z -= v.z;

	return *this;
}

///////////////////////////////////////////////////////////////////////////////
// Function:	multiplication operator
// Purpose:		Multiply a tVector3 by float
// Arguments:	float
///////////////////////////////////////////////////////////////////////////////
tVector3 tVector3::operator*( float arg )
{
	tVector3 v;

	v.x = x * arg;
	v.y = y * arg;
	v.z = z * arg;

	return v;
}

///////////////////////////////////////////////////////////////////////////////
// Function:	division operator
// Purpose:		Divides a tVector3 by float
// Arguments:	float
///////////////////////////////////////////////////////////////////////////////
tVector3 tVector3::operator/( float arg )
{
	tVector3 v;

	v.x = x / arg;
	v.y = y / arg;
	v.z = z / arg;

	return v;
}

///////////////////////////////////////////////////////////////////////////////
// Function:	multiplication operator
// Purpose:		Multiply a tVector3 by float
// Arguments:	float
///////////////////////////////////////////////////////////////////////////////
tVector3 tVector3::operator*=( float arg )
{
	tVector3 v;

	x *= arg;
	y *= arg;
	z *= arg;

	return *this;
}

///////////////////////////////////////////////////////////////////////////////
// Function:	division operator
// Purpose:		Divides a tVector3 by float
// Arguments:	float
///////////////////////////////////////////////////////////////////////////////
tVector3 tVector3::operator/=( float arg )
{
	tVector3 v;

	x /= arg;
	y /= arg;
	z /= arg;

	return *this;
}
///////////////////////////////////////////////////////////////////////////////
// Function:	Length2
// Purpose:		Returns the squared length of the vector
// Arguments:	none
///////////////////////////////////////////////////////////////////////////////
float tVector3::Length2() 
{
	return((x * x) + (y * y) + (z * z));
}

///////////////////////////////////////////////////////////////////////////////
// Function:	Length
// Purpose:		Returns the length of the vector
// Arguments:	none
///////////////////////////////////////////////////////////////////////////////
float tVector3::Length() 
{
	return((float)sqrt(Length2()));
}

///////////////////////////////////////////////////////////////////////////////
// Function:	NormalizeVector
// Purpose:		Normalizes the class vector
// Arguments:	none
///////////////////////////////////////////////////////////////////////////////
void tVector3::NormalizeVector() 
{
	float len = Length();
    if (len != 0.0) 
	{ 
		x /= len;  
		y /= len; 
		z /= len; 
	}
}

///////////////////////////////////////////////////////////////////////////////
// Function:	Lerp
// Purpose:		Interpolates between vector and another by a factor
// Arguments:	Vector to lerp to and factor
///////////////////////////////////////////////////////////////////////////////
void tVector3::Lerp(tVector3 *v1, float factor) 
{
	x = x * (1.0f - factor) + v1->x * factor; 
	y = y * (1.0f - factor) + v1->y * factor; 
	z = z * (1.0f - factor) + v1->z * factor; 
}

///////////////////////////////////////////////////////////////////////////////
// Function:	CrossProduct
// Purpose:		Sets the vector to equal the cross product of two vectors
// Arguments:	tVector3 *
///////////////////////////////////////////////////////////////////////////////
void tVector3::CrossProduct(tVector3 &v1, tVector3 &v2)
{
	x = (v1.y * v2.z) - (v1.z * v2.y);
	y = (v1.z * v2.x) - (v1.x * v2.z);
	z = (v1.x * v2.y) - (v1.y * v2.x);
}

///////////////////////////////////////////////////////////////////////////////
// Function:	Dot
// Purpose:		Return the dot product of two vectors
// Arguments:	tVector3 *
///////////////////////////////////////////////////////////////////////////////
float tVector3::Dot(tVector3 *v)
{
	return ((x * v->x) + (y * v->y) + (z * v->z));
}

///////////////////////////////////////////////////////////////////////////////
// Function:	Dot
// Purpose:		Return the dot product of two vectors
// Arguments:	tVector3 *
///////////////////////////////////////////////////////////////////////////////
float tVector3::Dot(tVector3 &v)
{
	return ((x * v.x) + (y * v.y) + (z * v.z));
}

///////////////////////////////////////////////////////////////////////////////
// Function:	Dot
// Purpose:		Return the dot product of two vectors
// Arguments:	vector elements
///////////////////////////////////////////////////////////////////////////////
float tVector3::Dot(float vx, float vy, float vz)
{
	return ((x * vx) + (y * vy) + (z * vz));
}

///////////////////////////////////////////////////////////////////////////////
// Function:	MultVectorByMatrix
// Purpose:		Multiplies a vector by a 4x4 Matrix in OpenGL Format
// Arguments:	Matrix
// Notes:		This routing is tweaked to handle OpenGLs column-major format
//				This is one obvious place for optimization perhaps asm code
///////////////////////////////////////////////////////////////////////////////
void tVector3::MultVectorByMatrix(float *mat)
{
	tVector3 result;
	result.x = (mat[0] * x) +
			   (mat[4] * y) +	
			   (mat[8] * z) +
			   mat[12];
	result.y = (mat[1] * x) +
			   (mat[5] * y) +	
			   (mat[9] * z) +
			   mat[13];
	result.z = (mat[2] * x) +
			   (mat[6] * y) +	
			   (mat[10] * z) +
			   mat[14];

	x = result.x;
	y = result.y;
	z = result.z;
}
//// MultVectorByMatrix //////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Function:	MultVectorByRotMatrix
// Purpose:		Multiplies a vector by a rotation only 4x4 Matrix in OpenGL Format
// Arguments:	Matrix
// Notes:		This routing is tweaked to handle OpenGLs column-major format
//				This is one obvious place for optimization perhaps asm code
///////////////////////////////////////////////////////////////////////////////
void tVector3::MultVectorByRotMatrix(float *mat)
{
	tVector3 result;
	result.x = (mat[0] * x) +
			   (mat[4] * y) +	
			   (mat[8] * z);
	result.y = (mat[1] * x) +
			   (mat[5] * y) +	
			   (mat[9] * z);
	result.z = (mat[2] * x) +
			   (mat[6] * y) +	
			   (mat[10] * z);

	x = result.x;
	y = result.y;
	z = result.z;
}
//// MultVectorByRotMatrix //////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// End of tVector3 Class
//////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Function:	CrossProduct
// Purpose:		Sets the vector to equal the cross product of two vectors
// Arguments:	tVector *
///////////////////////////////////////////////////////////////////////////////
tVector3 CrossProduct(tVector3 &v1, tVector3 &v2)
{
	tVector3 vRet;
	vRet.x = (v1.y * v2.z) - (v1.z * v2.y);
	vRet.y = (v1.z * v2.x) - (v1.x * v2.z);
	vRet.z = (v1.x * v2.y) - (v1.y * v2.x);
	return vRet;
}
