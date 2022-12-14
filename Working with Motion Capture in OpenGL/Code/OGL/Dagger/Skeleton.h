///////////////////////////////////////////////////////////////////////////////
//
// Skeleton.h : Animation System structure definition file
//
// Purpose:	Structure Definition of Hierarchical Animation System
//
// I DIDN'T PUT THESE IN A C++ CLASS FOR CROSS PLATFORM COMPATIBILITY
// SINCE THE ENGINE MAY BE IMPLEMENTED ON CONSOLES AND OTHER SYSTEMS
//
// Created:
//		JL 9/1/97
//
///////////////////////////////////////////////////////////////////////////////
//
//	Copyright 1997 Jeff Lander, All Rights Reserved.
//  For educational purposes only.
//  Please do not republish in electronic or print form without permission
//  Thanks - jeffl@darwin3d.com
//
///////////////////////////////////////////////////////////////////////////////

#if !defined(SKELETON_H__INCLUDED_)
#define SKELETON_H__INCLUDED_

/// Bone Definitions /////////////////////////////////////////////////////////
#define BONE_ID_ROOT				1		// ROOT BONE
///////////////////////////////////////////////////////////////////////////////

/// Channel Definitions ///////////////////////////////////////////////////////
#define CHANNEL_TYPE_NONE			0		// NO CHANNEL APPLIED
#define CHANNEL_TYPE_SRT			1		// SCALE ROTATION AND TRANSLATION
#define CHANNEL_TYPE_TRANS			2		// CHANNEL HAS TRANSLATION (X Y Z) ORDER
#define CHANNEL_TYPE_RXYZ			4		// ROTATION (RX RY RZ) ORDER
#define CHANNEL_TYPE_RZXY			8		// ROTATION (RZ RX RY) ORDER
#define CHANNEL_TYPE_RYZX			16		// ROTATION (RY RZ RX) ORDER
#define CHANNEL_TYPE_RZYX			32		// ROTATION (RZ RY RX) ORDER
#define CHANNEL_TYPE_RXZY			64		// ROTATION (RX RZ RY) ORDER
#define CHANNEL_TYPE_RYXZ			128		// ROTATION (RY RX RZ) ORDER
#define CHANNEL_TYPE_S				256		// SCALE ONLY
#define CHANNEL_TYPE_T				512		// TRANSLATION ONLY (X Y Z) ORDER
#define CHANNEL_TYPE_INTERLEAVED	1024	// THIS DATA STREAM HAS MULTIPLE CHANNELS
#define CHANNEL_TYPE_BVH			2048	// SPECIAL FORMAT FOR BVH

///////////////////////////////////////////////////////////////////////////////

// COUNT OF NUMBER OF FLOATS FOR EACH CHANNEL TYPE
static int s_Channel_Type_Size[] =
{
	0,
	9,
	6,
	3,
	3,
	3,
	3,
	3,
	3,
	3,
	3
};

typedef struct
{
	float x,y,z;
} tVector;

typedef struct
{
	float x,y,z,a;
} tQuaternion;

/// Structure Definitions ///////////////////////////////////////////////////////

// THIS STRUCTURE DEFINES A BONE IN THE ANIMATION SYSTEM
// A BONE IS ACTUALLY AN ABSTRACT REPRESENTATION OF A OBJECT
// IN THE 3D WORLD.  A CHARACTER COULD BE MADE OF ONE BONE
// WITH MULTIPLE VISUALS OF ANIMATION ATTACHED.  THIS WOULD
// BE SIMILAR TO A QUAKE CHARACTER.  BY MAKING IT HAVE LEVELS
// OF HIERARCHY AND CHANNELS OF ANIMATION IT IS JUST MORE FLEXIBLE
struct t_Bone
{
	long	id;							// BONE ID
	char	name[80];					// BONE NAME
	long	flags;						// BONE FLAGS
	// HIERARCHY INFO
	t_Bone	*parent;					// POINTER TO PARENT BONE
	int 	childCnt;					// COUNT OF CHILD BONES
	t_Bone	*children;					// POINTER TO CHILDREN
	// TRANSFORMATION INFO
	tVector	b_scale;					// BASE SCALE FACTORS
	tVector	b_rot;						// BASE ROTATION FACTORS
	tVector	b_trans;					// BASE TRANSLATION FACTORS
	tVector	scale;						// CURRENT SCALE FACTORS
	tVector	rot;						// CURRENT ROTATION FACTORS
	tVector	trans;						// CURRENT TRANSLATION FACTORS
	tQuaternion quat;					// QUATERNION USEFUL FOR ANIMATION

	// ANIMATION INFO
	unsigned int	primChanType;				// WHAT TYPE OF PREIMARY CHAN IS ATTACHED
	float	*primChannel;				// POINTER TO PRIMARY CHANNEL OF ANIMATION
	float 	primFrameCount;				// FRAMES IN PRIMARY CHANNEL
	float	primSpeed;					// CURRENT PLAYBACK SPEED
	float	primCurFrame;				// CURRENT FRAME NUMBER IN CHANNEL
	unsigned int	secChanType;				// WHAT TYPE OF SECONDARY CHAN IS ATTACHED
	float	*secChannel;				// POINTER TO SECONDARY CHANNEL OF ANIMATION
	float	secFrameCount;				// FRAMES IN SECONDARY CHANNEL
	float	secCurFrame;				// CURRENT FRAME NUMBER IN CHANNEL
	float	secSpeed;					// CURRENT PLAYBACK SPEED
	float	animBlend;					// BLENDING FACTOR (ANIM WEIGHTING)
	// DOF CONSTRAINTS
	int		min_rx, max_rx;				// ROTATION X LIMITS
	int		min_ry, max_ry;				// ROTATION Y LIMITS
	int		min_rz, max_rz;				// ROTATION Z LIMITS
	float	damp_width, damp_strength;	// DAMPENING SETTINGS
	// VISUAL ELEMENTS
	int		visualCnt;					// COUNT OF ATTACHED VISUAL ELEMENTS
	long	*visuals;					// POINTER TO VISUALS
	int		*CV_ptr;					// POINTER TO CONTROL VERTICES
	float	*CV_weight;					// POINTER TO ARRAY OF WEIGHTING VALUES
	// COLLISION ELEMENTS
	float	bbox[6];					// BOUNDING BOX (UL XYZ, LR XYZ)
	tVector	center;						// CENTER OF OBJECT (MASS)
	float	bsphere;					// BOUNDING SPHERE (RADIUS)
	// PHYSICS
	tVector	length;						// BONE LENGTH VECTOR
	float	mass;						// MASS
	float	friction;					// STATIC FRICTION
	float	kfriction;					// KINETIC FRICTION
	float	elast;						// ELASTICITY
};

///////////////////////////////////////////////////////////////////////////////

/// Support Function Definitions //////////////////////////////////////////////

void DestroySkeleton(t_Bone *root);
void ResetBone(t_Bone *bone,t_Bone *parent);
void BoneSetFrame(t_Bone *bone,int frame);
void BoneAdvanceFrame(t_Bone *bone,int direction,bool doChildren);

///////////////////////////////////////////////////////////////////////////////

#endif // !defined(SKELETON_H__INCLUDED_)
