///////////////////////////////////////////////////////////////////////////////
//
// Quaternion.h : Quaternion System structure definition file
//
// Purpose:	Quaternion Conversion and Evaluation Functions
//
// I DIDN'T PUT THESE IN A C++ CLASS FOR CROSS PLATFORM COMPATIBILITY
// SINCE THE ENGINE MAY BE IMPLEMENTED ON CONSOLES AND OTHER SYSTEMS
//
// Created:
//		JL 9/1/97		
// Revisions:
//		Integrated into Skinning Demo		2/18/98
//
///////////////////////////////////////////////////////////////////////////////
//
//	Copyright 1998 Jeff Lander, All Rights Reserved.
//  For educational purposes only.
//  Please do not republish in electronic or print form without permission
//  Thanks - jeffl@darwin3d.com
//
///////////////////////////////////////////////////////////////////////////////

#if !defined(QUATERN_H__INCLUDED_)
#define QUATERN_H__INCLUDED_

#include "Math.h"

///////////////////////////////////////////////////////////////////////////////
// Function:	EulerToQuaternion
// Purpose:		Convert a set of Euler angles to a Quaternion
// Arguments:	A rotation set of 3 angles, a quaternion to set
///////////////////////////////////////////////////////////////////////////////
void EulerToQuaternion(const tVector *rot, tQuaternion *quat);

///////////////////////////////////////////////////////////////////////////////
// Function:	EulerToQuaternion2
// Purpose:		Convert a set of Euler angles to a Quaternion
// Arguments:	A rotation set of 3 angles, a quaternion to set
///////////////////////////////////////////////////////////////////////////////
void EulerToQuaternion2(const tVector *rot, tQuaternion *quat);

///////////////////////////////////////////////////////////////////////////////
// Function:	QuatToAxisAngle
// Purpose:		Convert a Quaternion to Axis Angle representation
// Arguments:	A quaternion to convert, a axisAngle to set
// Discussion:  As the order of rotations is important I am
//				using the Quantum Mechanics convention of (X,Y,Z)
//				a Yaw-Pitch-Roll (Y,X,Z) system would have to be
//				adjusted
///////////////////////////////////////////////////////////////////////////////
void QuatToAxisAngle(const tQuaternion *quat,tQuaternion *axisAngle);

///////////////////////////////////////////////////////////////////////////////
// Function:	SlerpQuat
// Purpose:		Spherical Linear Interpolation Between two Quaternions
// Arguments:	Two Quaternions, blend factor, result quaternion
///////////////////////////////////////////////////////////////////////////////
void SlerpQuat(const tQuaternion *quat1,const tQuaternion *quat2,const float slerp, tQuaternion *result);

#endif // !defined(QUATERN_H__INCLUDED_)
