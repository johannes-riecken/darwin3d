///////////////////////////////////////////////////////////////////////////////
//
// Game Simulation Implementation File			GameSim.c
//
// Purpose:	Implementation of Billiards Physics
//			This is the meat of the physical simulation for the pool table
// Created:
//		JL 9/5/99	Created for the new sample app
//
// Modified:
//
// Notes:	Some base of this code is from the OpenGL Superbible.  Great Book
///////////////////////////////////////////////////////////////////////////////
//
//	Copyright 1998-1999 Jeff Lander, All Rights Reserved.
//  For educational purposes only.
//  Please do not republish in electronic or print form without permission
//  Thanks - jeffl@darwin3d.com
//
///////////////////////////////////////////////////////////////////////////////

#include <windows.h>	// Normal Windows stuff
#include <assert.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <math.h>
#include "externs.h"	// Data shared between files

#define		BALL_COUNT		2
#define		SYSTEM_COUNT	3
#define		MAX_CONTACTS	10

void			SetupBalls();

t_CueStick		g_CueStick;
t_Ball			g_Ball[2];		// Put two Balls in the world

float			g_Kd;
float			g_Kr_Bumper;	// Ball to Bumper Coefficient of Restitution
float			g_Kr_Ball;		// Ball to Ball Coefficient of Restitution
float			g_Ksh;
float			g_Ksd;
float			g_Csf;
float			g_Ckf;
int				g_IntegratorType;
int				g_CollisionRootFinding = FALSE;		// ONLY SET WHEN FINDING A COLLISION
int				g_UseDamping = TRUE;				// Use a Damping force
int				g_UseFriction = TRUE;				// Use Friction
int				g_CueHitBall = FALSE;				// Set when a Cue hits the ball
int				g_BallInPlay = FALSE;				// Ball has been hit
tVector3		g_CueForce;							
tVector3		g_Gravity;

t_Contact		g_Contact[MAX_CONTACTS];			// LIST OF POSSIBLE COLLISIONS
int				g_ContactCnt;						// COLLISION COUNT
t_CollisionPlane	g_CollisionPlane[4];		// LIST OF COLLISION PLANES
int					g_CollisionPlaneCnt;			
t_Ball			g_GameSys[SYSTEM_COUNT][BALL_COUNT];			// LIST OF PHYSICAL PARTICLES
t_Ball			*g_CurrentSys,*g_TargetSys;


///////////////////////////////////////////////////////////////////////////////
// Initialize the world and the objects in it.
BOOL InitGame(void)
{
	g_CueStick.draw = g_CueStick.old_draw = 0.2f;
	g_CueStick.yaw = 0.0f;
	g_CueStick.pos.x = -4.0f;
	g_CueStick.pos.y = TABLE_POSITION;
	g_CueStick.pos.z = 0.0f;


	g_Kd	= 0.02f;	// DAMPING FACTOR
	g_Kr_Bumper	= 0.8f;		// 1.0 = SUPERBALL BOUNCE 0.0 = DEAD WEIGHT
	g_Kr_Ball	= 0.2f;		// 1.0 = SUPERBALL BOUNCE 0.0 = DEAD WEIGHT
	g_Ksh	= 5.0f;		// HOOK'S SPRING CONSTANT
	g_Ksd	= 0.1f;		// SPRING DAMPING CONSTANT

	g_Csf	= 0.2f;		// Default Static Friction
	g_Ckf	= 0.1f;	// Default Kinetic Friction

	g_Gravity = tVector3(0.0f, -32.0f, 0.0f);

	// Pick an Integrator.  Either seem to work fine for this app
	// Euler is faster so....
//	g_IntegratorType = MIDPOINT_INTEGRATOR;
	g_IntegratorType = EULER_INTEGRATOR;

	g_CollisionRootFinding = FALSE;		// ONLY SET WHEN FINDING A COLLISION

	g_ContactCnt = 0;

	SetupBalls();

	g_CurrentSys = g_GameSys[0];
	g_TargetSys = g_GameSys[1];
	
	g_CollisionPlaneCnt = 4;

	// Left Bumper
	g_CollisionPlane[0].normal = tVector3(1.0f, 0.0f, 0.0f);
	g_CollisionPlane[0].d = LEFT_BUMPER;

	// Right Bumper
	g_CollisionPlane[1].normal = tVector3(-1.0f, 0.0f, 0.0f);
	g_CollisionPlane[1].d = RIGHT_BUMPER;

	// Top Bumper
	g_CollisionPlane[2].normal = tVector3( 0.0f, 0.0f, -1.0f);
	g_CollisionPlane[2].d = TOP_BUMPER;

	// Right Bumper
	g_CollisionPlane[3].normal = tVector3( 0.0f, 0.0f, 1.0f);
	g_CollisionPlane[3].d = BOTTOM_BUMPER;
	
	return TRUE;
}


void SetupBalls()
{
/// Local Variables ///////////////////////////////////////////////////////////
	int loop, loop2;
///////////////////////////////////////////////////////////////////////////////
	for (loop = 0; loop < SYSTEM_COUNT; loop++)
	{
		for (loop2 = 0; loop2 < BALL_COUNT; loop2++)
		{	
			switch (loop2)
			{
			// Ball 1 is the Cueball
			case 0:
				g_GameSys[loop][loop2].pos = tVector3( -4.0f, TABLE_POSITION, 0.0f);
				break;
			case 1:
				g_GameSys[loop][loop2].pos = tVector3( 4.0f, TABLE_POSITION, 0.0f);
				break;
			}
			g_GameSys[loop][loop2].v = tVector3( 0.0f, 0.0f, 0.0f);
			g_GameSys[loop][loop2].f = tVector3( 0.0f, 0.0f, 0.0f);
			g_GameSys[loop][loop2].oneOverM = 1.0f / 0.34f;
			g_GameSys[loop][loop2].angMom = tVector3( 0.0f, 0.0f, 0.0f);
			g_GameSys[loop][loop2].torque = tVector3( 0.0f, 0.0f, 0.0f);

			// Clear the Quaternion
			g_GameSys[loop][loop2].orientation = tQuaternion();

			g_GameSys[loop][loop2].flags = 0;
			g_GameSys[loop][loop2].flags = 0;
		}
	}
}


// Velocity Threshold that decides between Static and Kinetic Friction
#define STATIC_THRESHOLD	0.03f				

void ComputeForces( t_Ball	*system)
{
/// Local Variables ///////////////////////////////////////////////////////////
	int loop;
	t_Ball		*curBall;
	tVector3		contactN;
	float		FdotN,VdotN,Vmag;		
	tVector3		Vn,Vt;				// CONTACT RESOLUTION IMPULSE
///////////////////////////////////////////////////////////////////////////////

	curBall = system;
	for (loop = 0; loop < BALL_COUNT; loop++)
	{
		curBall->f = tVector3( 0.0f,0.0f,0.0f);			// Clear Force Vector
		curBall->torque = tVector3( 0.0f, 0.0f, 0.0f);	// Clear Torque Vector

		// TODO: Since I am only really doing 2D, I am not applying gravity here

		if (g_UseDamping)
		{
			curBall->f += (curBall->v * -g_Kd);
		}
		else
		{
			curBall->f += (curBall->v * -DEFAULT_DAMPING);
		}


		// Handle Friction forces for Balls in Contact with the Table
		// In my simple Pool table, the ball is always in contact
		// Only friction with the table top is handled
		if (g_UseFriction)
		{
			// Calculate Fn 
			FdotN = g_Gravity.y / curBall->oneOverM;		// Gravity
			// Calculate Vt Velocity Tangent to Normal Plane
			contactN = tVector3( 0.0f, 1.0f, 0.0f);
			VdotN = contactN.Dot(&curBall->v);
			Vn = contactN * VdotN;
			Vt = curBall->v - Vn;
			Vmag = Vt.Length2();
			// Check if Velocity is faster then threshold
			if (Vmag > STATIC_THRESHOLD)		// Use Kinetic Friction model
			{
				Vt.NormalizeVector();
				Vt *= (FdotN * g_Ckf);
				curBall->f += Vt;
			}
			else	// Use Static Friction Model
			{
				Vmag = Vmag / STATIC_THRESHOLD;
				Vt.NormalizeVector();
				Vt *= (FdotN * g_Csf * Vmag);
				curBall->f += Vt;

				// Once the Cue Ball Stops reset the stick
				if (loop == 0)								
				{
					g_BallInPlay = FALSE;
				}
			}
		}

		curBall++;
	}

	// CHECK IF THERE IS A USER FORCE BEING APPLIED
	if (g_CueHitBall)
	{
		system[0].f += g_CueForce;
	}
}   


///////////////////////////////////////////////////////////////////////////////
// Function:	IntegrateSysOverTime 
// Purpose:		Does the Integration for all the points in a system
// Arguments:	Initial Position, Source and Target Particle Systems and Time
// Notes:		Computes a single integration step
///////////////////////////////////////////////////////////////////////////////
void IntegrateSysOverTime(t_Ball *initial,t_Ball *source, t_Ball *target, float deltaTime)
{
/// Local Variables ///////////////////////////////////////////////////////////
	int loop;
	float deltaTimeMass;
///////////////////////////////////////////////////////////////////////////////
	for (loop = 0; loop < BALL_COUNT; loop++)
	{
		deltaTimeMass = deltaTime * initial->oneOverM;
		// DETERMINE THE NEW VELOCITY FOR THE PARTICLE
		target->v = initial->v + (source->f * deltaTimeMass);

		target->oneOverM = initial->oneOverM;

		// SET THE NEW POSITION
		target->pos = initial->pos + (source->v * deltaTime);

		initial++;
		source++;
		target++;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Function:	EulerIntegrate 
// Purpose:		Calculate new Positions and Velocities given a deltatime
// Arguments:	DeltaTime that has passed since last iteration
// Notes:		This integrator uses Euler's method
///////////////////////////////////////////////////////////////////////////////
void EulerIntegrate( float DeltaTime)
{
	// JUST TAKE A SINGLE STEP
	IntegrateSysOverTime(g_CurrentSys,g_CurrentSys, g_TargetSys,DeltaTime);
}

///////////////////////////////////////////////////////////////////////////////
// Function:	MidPointIntegrate 
// Purpose:		Calculate new Positions and Velocities given a deltatime
// Arguments:	DeltaTime that has passed since last iteration
// Notes:		This integrator uses the Midpoint method
///////////////////////////////////////////////////////////////////////////////
void MidPointIntegrate( float DeltaTime)
{
/// Local Variables ///////////////////////////////////////////////////////////
	float		halfDeltaT;
///////////////////////////////////////////////////////////////////////////////
	halfDeltaT = DeltaTime / 2.0f;

	// TAKE A HALF STEP AND UPDATE VELOCITY AND POSITION
	IntegrateSysOverTime(g_CurrentSys,g_CurrentSys,&g_GameSys[2][0],halfDeltaT);

	// COMPUTE FORCES USING THESE NEW POSITIONS AND VELOCITIES
	ComputeForces(&g_GameSys[2][0]);

	// TAKE THE FULL STEP WITH THIS NEW INFORMATION
	IntegrateSysOverTime(g_CurrentSys,&g_GameSys[2][0],g_TargetSys,DeltaTime);
}

int CheckForCollisions( t_Ball	*system )
{
/// Local Variables ///////////////////////////////////////////////////////////
	int collisionState = NOT_COLLIDING;
    float const depthEpsilon = 0.001f;
    float const ballDepthEpsilon = 0.0001f;
	int loop,loop2,planeIndex;
	t_Ball *curBall,*ball2;
	t_CollisionPlane *plane;
	float axbyczd,dist,relativeVelocity;
	tVector3	distVect;
///////////////////////////////////////////////////////////////////////////////

	g_ContactCnt = 0;		// THERE ARE CURRENTLY NO CONTACTS

	curBall = system;
	for (loop = 0; (loop < BALL_COUNT) && (collisionState != PENETRATING); 
			loop++,curBall++)
	{
        for(planeIndex = 0;(planeIndex < g_CollisionPlaneCnt) &&
            (collisionState != PENETRATING);planeIndex++)
        {
			plane = &g_CollisionPlane[planeIndex];

			axbyczd = curBall->pos.Dot(&plane->normal) + plane->d;

            if(axbyczd < -depthEpsilon)
            {
				// ONCE ANY BALL PENETRATES A BUMPER, QUIT THE LOOP
				collisionState = PENETRATING;
            }
            else
            if(axbyczd < depthEpsilon)
            {
                relativeVelocity = plane->normal.Dot(&curBall->v);
				if(relativeVelocity < 0.0f)
                {
					// TODO: Add a Collision with bumper sound here
                    collisionState = COLLIDING_WITH_WALL;
					g_Contact[g_ContactCnt].type = COLLIDING_WITH_WALL;
					g_Contact[g_ContactCnt].ball = loop; 
					g_Contact[g_ContactCnt].Kr = g_Kr_Bumper;		// Ball to bumper Kr
					memcpy(&g_Contact[g_ContactCnt].normal,&plane->normal,sizeof(tVector3));
					g_ContactCnt++;
                }
            }
        }

		// Now check ball to ball collisions.  
		ball2 = system;
		for (loop2 = 0; (loop2 < BALL_COUNT) && (collisionState != PENETRATING); 
				loop2++,ball2++)
		{
			if (loop2 == loop) continue;		// don't test against self
			if (curBall->flags) continue;		// Skip already handled balls

			distVect = curBall->pos - ball2->pos;

			dist = distVect.Length2();
			// SINCE IT IS TESTING THE SQUARED DISTANCE, SQUARE THE RADIUS ALSO
			dist = dist - (BALL_DIAMETER * BALL_DIAMETER);	// Width of a ball 

			if(dist < -ballDepthEpsilon)
			{
				// ONCE ANY BALL PENETRATES, QUIT THE LOOP
				collisionState = PENETRATING;
			}
			else
			if(dist < ballDepthEpsilon)
			{
				// NORMALIZE THE VECTOR
				distVect.NormalizeVector();

				relativeVelocity = distVect.Dot(curBall->v);

				if(relativeVelocity < 0.0f)
				{
					collisionState = COLLIDING_WITH_BALL;
					g_Contact[g_ContactCnt].type = COLLIDING_WITH_BALL;
					g_Contact[g_ContactCnt].ball = loop; 
					g_Contact[g_ContactCnt].ball2 = loop2; 
					g_Contact[g_ContactCnt].Kr = g_Kr_Ball;		// Ball to Ball Kr
					memcpy(&g_Contact[g_ContactCnt].normal,&distVect,sizeof(tVector3));
					ball2->flags = 1;
					g_ContactCnt++;
				}
			}
		}

	}

    return collisionState;
}

void ResolveCollisions( t_Ball	*system )
{
	t_Contact	*contact;
	t_Ball		*ball,*ball2;		// THE PARTICLE COLLIDING
	float		VdotN;		
	tVector3	Vn,Vt,Vn1;				// CONTACT RESOLUTION IMPULSE
	int			loop;

	contact = g_Contact;
	for (loop = 0; loop < g_ContactCnt; loop++,contact++)
	{
		ball = &system[contact->ball];
		// CALCULATE Vn
		VdotN = contact->normal.Dot(&ball->v);
		Vn = contact->normal * VdotN;
		// CALCULATE Vt
		Vt = ball->v - Vn;

		// Check if it was a collision with a wall or ball
		if (contact->type == COLLIDING_WITH_WALL)
		{
			// SCALE Vn BY COEFFICIENT OF RESTITUTION
			Vn *= contact->Kr;
			// SET THE VELOCITY TO BE THE NEW IMPULSE
			ball->v = Vt - Vn;
		}
		else	// Ball to ball collision
		{
			// Scale Vn By coefficient of restitution for ball 1
			Vn1 = Vn * contact->Kr;
			// Set the velocity to be the new impulse
			ball->v = Vt - Vn1;

			ball2 = &system[contact->ball2];
			// Scale Vn By 1 - coefficient of restitution for ball 2
			Vn1 = Vn * (1.0f - contact->Kr);
			// Add it to the balls direction
			ball2->v += Vn1;

			ball->flags = 0;
			ball2->flags = 0;
		}

		// TODO: Bumper Collision induced Spin
	}
}

void Simulate(float DeltaTime, BOOL running)
{
    float		CurrentTime = 0.0f;
    float		TargetTime = DeltaTime;
	t_Ball		*tempSys;
	int			collisionState;

    while(CurrentTime < DeltaTime)
    {
		if (running)
		{
			ComputeForces(g_CurrentSys);

			// IN ORDER TO MAKE THINGS RUN FASTER, I HAVE THIS LITTLE TRICK
			// IF THE SYSTEM IS DOING A BINARY SEARCH FOR THE COLLISION POINT,
			// I FORCE EULER'S METHOD ON IT. OTHERWISE, LET THE USER CHOOSE.
			// THIS DOESN'T SEEM TO EFFECT STABILITY EITHER WAY
			if (g_CollisionRootFinding)
			{
				EulerIntegrate(TargetTime-CurrentTime);
			}
			else
			{
				switch (g_IntegratorType)
				{
				case EULER_INTEGRATOR:
					EulerIntegrate(TargetTime-CurrentTime);
					break;
				case MIDPOINT_INTEGRATOR:
					MidPointIntegrate(TargetTime-CurrentTime);
					break;
				}
			}
		}

		collisionState = CheckForCollisions(g_TargetSys);

        if(collisionState == PENETRATING)
        {
			// TELL THE SYSTEM I AM LOOKING FOR A COLLISION SO IT WILL USE EULER
			g_CollisionRootFinding = TRUE;
            // we simulated too far, so subdivide time and try again
            TargetTime = (CurrentTime + TargetTime) / 2.0f;

            // blow up if we aren't moving forward each step, which is
            // probably caused by interpenetration at the frame start
            assert(fabs(TargetTime - CurrentTime) > EPSILON);
        }
        else
        {
            // either colliding or clear
            if(collisionState == COLLIDING_WITH_WALL || 
			   collisionState == COLLIDING_WITH_BALL)
            {
                int Counter = 0;
                do
                {
                    ResolveCollisions(g_TargetSys);
                    Counter++;
                } while((CheckForCollisions(g_TargetSys) >=
                            COLLIDING_WITH_WALL) && (Counter < 500));

                assert(Counter < 500);
				g_CollisionRootFinding = FALSE;	// FOUND THE COLLISION POINT BACK TO NORMAL
            }

            // we made a successful step, so swap configurations
            // to "save" the data for the next step
            
			CurrentTime = TargetTime;
			TargetTime = DeltaTime;

			g_CueForce = tVector3( 0.0f,0.0f,0.0f);	// Clear Cue Force

			// SWAP MY TWO SYSTEM BUFFERS SO I CAN DO IT AGAIN
			tempSys = g_CurrentSys;
			g_CurrentSys = g_TargetSys;
			g_TargetSys = tempSys;
        }
    }
}
