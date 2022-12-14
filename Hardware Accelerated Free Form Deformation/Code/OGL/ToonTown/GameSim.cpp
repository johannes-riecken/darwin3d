///////////////////////////////////////////////////////////////////////////////
//
// Game Simulation Implementation File			GameSim.c
//
// Purpose:	Implementation of Toon Physics
//			This is the meat of the physical simulation for the ToonTown
// Created:
//		JL 3/5/00	Created for the new sample app
//
// Modified:
//
// Notes:	Some base of this code is from the OpenGL Superbible.  Great Book
///////////////////////////////////////////////////////////////////////////////
//
//	Copyright 1998-2000 Jeff Lander, All Rights Reserved.
//  For educational purposes only.
//  Please do not republish in electronic or print form without permission
//  Thanks - jeffl@darwin3d.com
//
///////////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>
#include "externs.h"	// Data shared between files
#include <cstring>

#define		FFD_NODE_COUNT	64
#define		FFD_WIDTH	4
#define		FFD_HEIGHT	4
#define		FFD_DEPTH	4

#define		SYSTEM_COUNT	3
#define		MAX_CONTACTS	64

void			SetupWorld();

t_Particle		g_Particle[FFD_NODE_COUNT];

float			g_Kd;
float			g_Kr;	// Particle to Wall Coefficient of Restitution
float			g_Ksh;
float			g_Ksd;
float			g_Csf;
float			g_Ckf;
float			g_Kd;
int				g_IntegratorType;
int				g_CollisionRootFinding = false;		// ONLY SET WHEN FINDING A COLLISION
int				g_UseDamping = true;				// Use a Damping force
int				g_UseFriction = true;				// Use Friction
int				g_UseGravity = true;				// Use Gravity
int				g_DrawSprings = true;
int				g_MouseForceActive = false;
int				g_DrawCVs = true;
int				g_DrawMesh = true;
int				g_Pick[2];

tVector			g_CueForce;
tVector			g_Gravity;
tVector			g_MouseDragPos[2];

t_Contact		g_Contact[MAX_CONTACTS];			// LIST OF POSSIBLE COLLISIONS
int				g_ContactCnt;						// COLLISION COUNT
t_CollisionPlane	g_CollisionPlane[4];		// LIST OF COLLISION PLANES
int					g_CollisionPlaneCnt;
t_Particle			g_GameSys[SYSTEM_COUNT][FFD_NODE_COUNT];			// LIST OF PHYSICAL PARTICLES
t_Particle			*g_CurrentSys,*g_TargetSys;
int				g_ParticleCount;
t_Spring		*g_Spring;				// VALID SPRINGS IN SYSTEM
int				g_SpringCnt;
float			g_WorldSizeX,g_WorldSizeY,g_WorldSizeZ;
tVector			g_FFDmin,g_FFDmax;
///////////////////////////////////////////////////////////////////////////////
// Initialize the world and the objects in it.
bool InitGame(void)
{
	g_Pick[0] = -1;
	g_Pick[1] = -1;

	g_Kd	= 0.5f;	// DAMPING FACTOR
	g_Kr	= 0.6f;		// 1.0 = SUPERBALL BOUNCE 0.0 = DEAD WEIGHT
	g_Ksh	= 80.0f;		// HOOK'S SPRING CONSTANT
	g_Ksd	= 0.9f;		// SPRING DAMPING CONSTANT

	g_Csf	= 0.2f;		// Default Static Friction
	g_Ckf	= 0.05f;	// Default Kinetic Friction

	MAKEVECTOR(g_Gravity, 0.0f, -16.0f, 0.0f)	// Feet per second

	// Pick an Integrator.  Either seem to work fine for this app
	// Euler is faster so....
//	g_IntegratorType = MIDPOINT_INTEGRATOR;
	g_IntegratorType = EULER_INTEGRATOR;

	g_CollisionRootFinding = false;		// ONLY SET WHEN FINDING A COLLISION

	g_ContactCnt = 0;
	g_SpringCnt = 0;


	g_ParticleCount = FFD_NODE_COUNT;
	SetupWorld();

	g_CurrentSys = g_GameSys[0];
	g_TargetSys = g_GameSys[1];

	g_CollisionPlaneCnt = 5;

	// CREATE THE SIZE FOR THE SIMULATION WORLD
	g_WorldSizeX = 15.0f;
	g_WorldSizeY = 15.0f;
	g_WorldSizeZ = 15.0f;

	// MAKE THE BOTTOM PLANE (FLOOR)
	MAKEVECTOR(g_CollisionPlane[0].normal,0.0f, 1.0f, 0.0f)
	g_CollisionPlane[0].d = g_WorldSizeY / 2.0f;

	// MAKE THE LEFT PLANE
	MAKEVECTOR(g_CollisionPlane[1].normal,-1.0f, 0.0f, 0.0f)
	g_CollisionPlane[1].d = g_WorldSizeX / 2.0f;

	// MAKE THE RIGHT PLANE
	MAKEVECTOR(g_CollisionPlane[2].normal,1.0f, 0.0f, 0.0f)
	g_CollisionPlane[2].d = g_WorldSizeX / 2.0f;

	// MAKE THE FRONT PLANE
	MAKEVECTOR(g_CollisionPlane[3].normal,0.0f, 0.0f, -1.0f)
	g_CollisionPlane[3].d = g_WorldSizeZ / 2.0f;

	// MAKE THE BACK PLANE
	MAKEVECTOR(g_CollisionPlane[4].normal,0.0f, 0.0f, 1.0f)
	g_CollisionPlane[4].d = g_WorldSizeZ / 2.0f;


	return true;
}

void AddSpring(int p1, int p2,t_Particle *sys)
{
	t_Spring		*tempSprings;

	tempSprings = (t_Spring *)malloc(sizeof(t_Spring) * (g_SpringCnt + 1));
	if (g_SpringCnt > 0)
	{
		memcpy(tempSprings,g_Spring,sizeof(t_Spring) * g_SpringCnt);
		free(g_Spring);
	}
	g_Spring = tempSprings;
	tempSprings = &g_Spring[g_SpringCnt];
	g_SpringCnt = g_SpringCnt + 1;
	tempSprings->Ks = g_Ksh;
	tempSprings->Kd = g_Ksd;
	tempSprings->p1 = p1;
	tempSprings->p2 = p2;
	tempSprings->restLen =
		sqrt(VectorSquaredDistance(&sys[p1].pos,
								   &sys[p2].pos));
}

void SetupWorld()
{
/// Local Variables ///////////////////////////////////////////////////////////
	int loop, loop2,px,py,pz;
///////////////////////////////////////////////////////////////////////////////
	MAKEVECTOR(g_FFDmin, 9999.0f, 9999.0f, 9999.0f)
	MAKEVECTOR(g_FFDmax, -9999.0f, -9999.0f, -9999.0f)
	for (loop = 0; loop < SYSTEM_COUNT; loop++)
	{
		for (loop2 = 0; loop2 < g_ParticleCount; loop2++)
		{
			MAKEVECTOR(g_GameSys[loop][loop2].pos,3.0f - ((loop2 % 4) * 2.0f), FFD_STARTY - ((loop2 / 16) * 1.4f), 6.0f - (((loop2 % 16) / 4) * 4.0f))
			memcpy(&g_GameSys[loop][loop2].rest_pos,&g_GameSys[loop][loop2].pos,sizeof(tVector));
			MAKEVECTOR(g_GameSys[loop][loop2].v,0.0f, 0.0f, 0.0f)
			MAKEVECTOR(g_GameSys[loop][loop2].f,0.0f, 0.0f, 0.0f)
			g_GameSys[loop][loop2].oneOverM = 1.0f / 0.34f;
			MAKEVECTOR(g_GameSys[loop][loop2].angMom,0.0f, 0.0f, 0.0f)
			MAKEVECTOR(g_GameSys[loop][loop2].torque,0.0f, 0.0f, 0.0f)

			// Clear the Quaternion
			MAKEVECTOR(g_GameSys[loop][loop2].orientation,0.0f, 0.0f, 0.0f)
			g_GameSys[loop][loop2].orientation.w = 1.0f;	// Handle the W part

			g_GameSys[loop][loop2].flags = 0;
			if (g_GameSys[loop][loop2].pos.x < g_FFDmin.x)
				g_FFDmin.x = g_GameSys[loop][loop2].pos.x;
			if (g_GameSys[loop][loop2].pos.y < g_FFDmin.y)
				g_FFDmin.y = g_GameSys[loop][loop2].pos.y;
			if (g_GameSys[loop][loop2].pos.z < g_FFDmin.z)
				g_FFDmin.z = g_GameSys[loop][loop2].pos.z;

			if (g_GameSys[loop][loop2].pos.x > g_FFDmax.x)
				g_FFDmax.x = g_GameSys[loop][loop2].pos.x;
			if (g_GameSys[loop][loop2].pos.y > g_FFDmax.y)
				g_FFDmax.y = g_GameSys[loop][loop2].pos.y;
			if (g_GameSys[loop][loop2].pos.z > g_FFDmax.z)
				g_FFDmax.z = g_GameSys[loop][loop2].pos.z;
		}
	}

	g_SpringCnt = 0;
	// Setup the springs
	for (loop = 0; loop < g_ParticleCount; loop++)
	{
		px = loop % FFD_WIDTH;
		pz = (loop % (FFD_WIDTH * FFD_HEIGHT)) / FFD_WIDTH;
		py = loop / (FFD_WIDTH * FFD_HEIGHT);

		if (px < 3)
		{
			AddSpring(loop, loop + 1,&g_GameSys[0][0]);
		}
		if (pz < 3)
		{
			AddSpring(loop, loop + 4,&g_GameSys[0][0]);
		}
		if (py < 3)
		{
			AddSpring(loop, loop + 16,&g_GameSys[0][0]);
		}
		if (px < 3 && py < 3)
		{
			AddSpring(loop, loop + 17,&g_GameSys[0][0]);
		}
		if (px > 0 && py < 3)
		{
			AddSpring(loop, loop + 15,&g_GameSys[0][0]);
		}
		if (py < 3 && pz < 3)
		{
			AddSpring(loop, loop + 20,&g_GameSys[0][0]);
		}
		if (py < 3 && pz > 0)
		{
			AddSpring(loop, loop + 12,&g_GameSys[0][0]);
		}
		if (px < 3 && pz < 3)
		{
			AddSpring(loop, loop + 5,&g_GameSys[0][0]);
		}
		if (px > 0 && pz < 3)
		{
			AddSpring(loop, loop + 3,&g_GameSys[0][0]);
		}
		if (py < 3 && pz > 0)
		{
			AddSpring(loop, loop + 12,&g_GameSys[0][0]);
		}
		if (px < 3 && py < 3 && pz < 3)
		{
			AddSpring(loop, loop + 21,&g_GameSys[0][0]);
		}
		if (px > 0 && py < 3 && pz < 3)
		{
			AddSpring(loop, loop + 19,&g_GameSys[0][0]);
		}
		if (px < 3 && py > 0 && pz < 3)
		{
			AddSpring(loop, loop - 11,&g_GameSys[0][0]);
		}
		if (px > 0 && py > 0 && pz < 3)
		{
			AddSpring(loop, loop - 13,&g_GameSys[0][0]);
		}
	}
}

void DrawSimWorld()
{
	t_Particle		*tempParticle;
	t_Spring		*tempSpring;
	int				loop;

	// FIRST DRAW THE WORLD CONTAINER
	glColor3f(1.0f,1.0f,1.0f);
    // do a big linestrip to get most of edges
    glBegin(GL_LINE_STRIP);
        glVertex3f(-g_WorldSizeX/2.0f, g_WorldSizeY/2.0f,-g_WorldSizeZ/2.0f);
        glVertex3f( g_WorldSizeX/2.0f, g_WorldSizeY/2.0f,-g_WorldSizeZ/2.0f);
        glVertex3f( g_WorldSizeX/2.0f, g_WorldSizeY/2.0f, g_WorldSizeZ/2.0f);
        glVertex3f(-g_WorldSizeX/2.0f, g_WorldSizeY/2.0f, g_WorldSizeZ/2.0f);
        glVertex3f(-g_WorldSizeX/2.0f, g_WorldSizeY/2.0f,-g_WorldSizeZ/2.0f);
        glVertex3f(-g_WorldSizeX/2.0f,-g_WorldSizeY/2.0f,-g_WorldSizeZ/2.0f);
    glEnd();
    // fill in the stragglers
    glBegin(GL_LINES);
        glVertex3f( g_WorldSizeX/2.0f, g_WorldSizeY/2.0f,-g_WorldSizeZ/2.0f);
        glVertex3f( g_WorldSizeX/2.0f,-g_WorldSizeY/2.0f,-g_WorldSizeZ/2.0f);

        glVertex3f( g_WorldSizeX/2.0f, g_WorldSizeY/2.0f, g_WorldSizeZ/2.0f);
        glVertex3f( g_WorldSizeX/2.0f,-g_WorldSizeY/2.0f, g_WorldSizeZ/2.0f);

        glVertex3f(-g_WorldSizeX/2.0f, g_WorldSizeY/2.0f, g_WorldSizeZ/2.0f);
        glVertex3f(-g_WorldSizeX/2.0f,-g_WorldSizeY/2.0f, g_WorldSizeZ/2.0f);
    glEnd();

    // draw floor
    glDisable(GL_CULL_FACE);
    glBegin(GL_QUADS);
        glColor3f(0.0f,0.0f,0.5f);
        glVertex3f(-g_WorldSizeX/2.0f,-g_WorldSizeY/2.0f,-g_WorldSizeZ/2.0f);
        glVertex3f( g_WorldSizeX/2.0f,-g_WorldSizeY/2.0f,-g_WorldSizeZ/2.0f);
        glVertex3f( g_WorldSizeX/2.0f,-g_WorldSizeY/2.0f, g_WorldSizeZ/2.0f);
        glVertex3f(-g_WorldSizeX/2.0f,-g_WorldSizeY/2.0f, g_WorldSizeZ/2.0f);
    glEnd();
    glEnable(GL_CULL_FACE);


	if (g_CurrentSys)
	{
		if (g_Spring && g_DrawSprings)
		{
			glBegin(GL_LINES);
			glColor3f(0.0f,0.8f,0.8f);
			tempSpring = g_Spring;
			for (loop = 0; loop < g_SpringCnt; loop++)
			{
				glVertex3fv((float *)&g_CurrentSys[tempSpring->p1].pos);
				glVertex3fv((float *)&g_CurrentSys[tempSpring->p2].pos);
				tempSpring++;
			}
			if (g_MouseForceActive)	// DRAW MOUSESPRING FORCE
			{
				if (g_Pick[0] > -1)
				{
					glColor3f(0.8f,0.0f,0.8f);
					glVertex3fv((float *)&g_CurrentSys[g_Pick[0]].pos);
					glVertex3fv((float *)&g_MouseDragPos[0]);
				}
				if (g_Pick[1] > -1)
				{
					glColor3f(0.8f,0.0f,0.8f);
					glVertex3fv((float *)&g_CurrentSys[g_Pick[1]].pos);
					glVertex3fv((float *)&g_MouseDragPos[1]);
				}
			}
			glEnd();
		}
		if (g_DrawCVs)
		{
			glBegin(GL_POINTS);
			tempParticle = g_CurrentSys;
			for (loop = 0; loop < g_ParticleCount; loop++)
			{
				// Picked particles are green then red for 1 and 2
				if (loop == g_Pick[0])
					glColor3f(0.0f,0.8f,0.0f);
				else if (loop == g_Pick[1])
					glColor3f(0.8f,0.0f,0.0f);
				// If particles are in contact, Draw them in Orange
//				else if (tempParticle->contacting && g_UseFriction)
//					glColor3f(1.0f,0.5f,0.0f);
				// Normally Yellow
				else
					glColor3f(0.8f,0.0f,0.8f);

				glVertex3fv((float *)&tempParticle->pos);
				tempParticle++;
			}
			glEnd();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// Function:	CompareBuffer
// Purpose:		Check the feedback buffer to see if anything is hit
// Arguments:	Number of hits, pointer to buffer, point to test
///////////////////////////////////////////////////////////////////////////////
void CompareBuffer(int size, float *buffer,float x, float y)
{
/// Local Variables ///////////////////////////////////////////////////////////
	GLint count;
	GLfloat token,point[3];
	int loop,currentVertex,result = -1;
	long nearest = -1, dist;
///////////////////////////////////////////////////////////////////////////////
	count = size;
	while (count)
	{
		token = buffer[size - count];	// CHECK THE TOKEN
		count--;
		if (token == GL_PASS_THROUGH_TOKEN)	// VERTEX MARKER
		{
			currentVertex = (int)buffer[size - count]; // WHAT VERTEX
			count--;
		}
		else if (token == GL_POINT_TOKEN)
		{
			// THERE ARE THREE ELEMENTS TO A POINT TOKEN
			for (loop = 0; loop < 3; loop++)
			{
				point[loop] = buffer[size - count];
				count--;
			}
			dist = ((x - point[0]) * (x - point[0])) + ((y - point[1]) * (y - point[1]));
			if (result == -1 || dist < nearest)
			{
				nearest = dist;
				result = currentVertex;
			}
		}
	}

	if (nearest < 50.0f)
	{
		if (g_Pick[0] == -1)
			g_Pick[0] = result;
		else if (g_Pick[1] == -1)
			g_Pick[1] = result;
		else
		{
			g_Pick[0] = result;
			g_Pick[1] = -1;
		}
	}
}
////// CompareBuffer //////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Function:	GetNearestPoint
// Purpose:		Use OpenGL Feedback to find the closest point to a mouseclick
// Arguments:	Screen coordinates of the hit
///////////////////////////////////////////////////////////////////////////////
void GetNearestPoint(int x, int y)
{
/// Local Variables ///////////////////////////////////////////////////////////
	float *feedBuffer;
	int hitCount;
	t_Particle *tempParticle;
	int loop;
///////////////////////////////////////////////////////////////////////////////
	// INITIALIZE A PLACE TO PUT ALL THE FEEDBACK INFO (3 DATA, 1 TAG, 2 TOKENS)
	feedBuffer = (float *)malloc(sizeof(GLfloat) * g_ParticleCount * 6);
	// TELL OPENGL ABOUT THE BUFFER
	glFeedbackBuffer(g_ParticleCount * 6,GL_3D,feedBuffer);
	(void)glRenderMode(GL_FEEDBACK);	// SET IT IN FEEDBACK MODE

	tempParticle = g_CurrentSys;
	for (loop = 0; loop < g_ParticleCount; loop++)
	{
		// PASS THROUGH A MARKET LETTING ME KNOW WHAT VERTEX IT WAS
		glPassThrough((float)loop);
		// SEND THE VERTEX
		glBegin(GL_POINTS);
		glVertex3fv((float *)&tempParticle->pos);
		glEnd();
		tempParticle++;
	}
	hitCount = glRenderMode(GL_RENDER); // HOW MANY HITS DID I GET
	CompareBuffer(hitCount,feedBuffer,(float)x,(float)y);		// CHECK THE HIT
	free(feedBuffer);		// GET RID OF THE MEMORY
}
////// GetNearestPoint ////////////////////////////////////////////////////////


// Velocity Threshold that decides between Static and Kinetic Friction
#define STATIC_THRESHOLD	0.03f

void ComputeForces( t_Particle	*system)
{
/// Local Variables ///////////////////////////////////////////////////////////
	int loop;
	t_Particle		*curPart,*p1,*p2;
	tVector		contactN;
	float		FdotN,VdotN,Vmag;
	tVector		Vn,Vt;				// CONTACT RESOLUTION IMPULSE
	t_Spring		*spring;
	float		dist, Hterm, Dterm;
	tVector		springForce,deltaV,deltaP;
///////////////////////////////////////////////////////////////////////////////

	curPart = system;
	for (loop = 0; loop < FFD_NODE_COUNT; loop++)
	{
		MAKEVECTOR(curPart->f,0.0f,0.0f,0.0f)		// Clear Force Vector
		MAKEVECTOR(curPart->torque,0.0f, 0.0f, 0.0f)	// Clear Torque Vector

		if (g_UseGravity && curPart->oneOverM != 0) // && curParticle->type != CONTACTING)
		{
			curPart->f.x += (g_Gravity.x / curPart->oneOverM);
			curPart->f.y += (g_Gravity.y / curPart->oneOverM);
			curPart->f.z += (g_Gravity.z / curPart->oneOverM);
		}

		if (g_UseDamping)
		{
			curPart->f.x += (-g_Kd * curPart->v.x);
			curPart->f.y += (-g_Kd * curPart->v.y);
			curPart->f.z += (-g_Kd * curPart->v.z);
		}
		else
		{
			curPart->f.x += (-DEFAULT_DAMPING * curPart->v.x);
			curPart->f.y += (-DEFAULT_DAMPING * curPart->v.y);
			curPart->f.z += (-DEFAULT_DAMPING * curPart->v.z);
		}


		// Handle Friction forces for  points in Contact with the a surface
		if (g_UseFriction)
		{
			// Calculate Fn
			FdotN = g_Gravity.y / curPart->oneOverM;		// Gravity
			// Calculate Vt Velocity Tangent to Normal Plane
			MAKEVECTOR(contactN,0.0f, 1.0f, 0.0f)
			VdotN = DotProduct(&contactN,&curPart->v);
			ScaleVector(&contactN, VdotN, &Vn);
			VectorDifference(&curPart->v, &Vn, &Vt);
			Vmag = VectorSquaredLength(&Vt);
			// Check if Velocity is faster then threshold
			if (Vmag > STATIC_THRESHOLD)		// Use Kinetic Friction model
			{
				NormalizeVector(&Vt);
				ScaleVector(&Vt, (FdotN * g_Ckf), &Vt);
				VectorSum(&curPart->f,&Vt,&curPart->f);
			}
			else	// Use Static Friction Model
			{
				Vmag = Vmag / STATIC_THRESHOLD;
				NormalizeVector(&Vt);
				ScaleVector(&Vt, (FdotN * g_Csf * Vmag), &Vt);  // Scale Friction by Velocity
				VectorSum(&curPart->f,&Vt,&curPart->f);
			}
		}

		curPart++;
	}

	// NOW DO ALL THE SPRINGS
	spring = g_Spring;
	for (loop = 0; loop < g_SpringCnt; loop++)
	{
		p1 = &system[spring->p1];
		p2 = &system[spring->p2];
		VectorDifference(&p1->pos,&p2->pos,&deltaP);	// Vector distance
		dist = VectorLength(&deltaP);					// Magnitude of deltaP

		Hterm = (dist - spring->restLen) * spring->Ks;	// Ks * (dist - rest)

		VectorDifference(&p1->v,&p2->v,&deltaV);		// Delta Velocity Vector
		Dterm = (DotProduct(&deltaV,&deltaP) * spring->Kd) / dist; // Damping Term

		ScaleVector(&deltaP,1.0f / dist, &springForce);	// Normalize Distance Vector
		ScaleVector(&springForce,-(Hterm + Dterm),&springForce);	// Calc Force
		VectorSum(&p1->f,&springForce,&p1->f);			// Apply to Particle 1
		VectorDifference(&p2->f,&springForce,&p2->f);	// - Force on Particle 2
		spring++;					// DO THE NEXT SPRING
	}

	// APPLY THE MOUSE DRAG FORCES IF THEY ARE ACTIVE
	if (g_MouseForceActive)
	{
		// APPLY TO EACH PICKED PARTICLE
		if (g_Pick[0] > -1)
		{
			p1 = &system[g_Pick[0]];
			VectorDifference(&p1->pos,&g_MouseDragPos[0],&deltaP);	// Vector distance
			dist = VectorLength(&deltaP);					// Magnitude of deltaP

			if (dist != 0.0f)
			{
				Hterm = (dist) * 8.8f;					// Ks * dist

				ScaleVector(&deltaP,1.0f / dist, &springForce);	// Normalize Distance Vector
				ScaleVector(&springForce,-(Hterm),&springForce);	// Calc Force
				VectorSum(&p1->f,&springForce,&p1->f);			// Apply to Particle 1
			}
		}
		if (g_Pick[1] > -1)
		{
			p1 = &system[g_Pick[1]];
			VectorDifference(&p1->pos,&g_MouseDragPos[1],&deltaP);	// Vector distance
			dist = VectorLength(&deltaP);					// Magnitude of deltaP

			if (dist != 0.0f)
			{
				Hterm = (dist) * 8.8f;					// Ks * dist

				ScaleVector(&deltaP,1.0f / dist, &springForce);	// Normalize Distance Vector
				ScaleVector(&springForce,-(Hterm),&springForce);	// Calc Force
				VectorSum(&p1->f,&springForce,&p1->f);			// Apply to Particle 1
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
// Function:	IntegrateSysOverTime
// Purpose:		Does the Integration for all the points in a system
// Arguments:	Initial Position, Source and Target Particle Systems and Time
// Notes:		Computes a single integration step
///////////////////////////////////////////////////////////////////////////////
void IntegrateSysOverTime(t_Particle *initial,t_Particle *source, t_Particle *target, float deltaTime)
{
/// Local Variables ///////////////////////////////////////////////////////////
	int loop;
	float deltaTimeMass;
///////////////////////////////////////////////////////////////////////////////
	for (loop = 0; loop < FFD_NODE_COUNT; loop++)
	{
		deltaTimeMass = deltaTime * initial->oneOverM;
		// DETERMINE THE NEW VELOCITY FOR THE PARTICLE
		target->v.x = initial->v.x + (source->f.x * deltaTimeMass);
		target->v.y = initial->v.y + (source->f.y * deltaTimeMass);
		target->v.z = initial->v.z + (source->f.z * deltaTimeMass);

		target->oneOverM = initial->oneOverM;

		// SET THE NEW POSITION
		target->pos.x = initial->pos.x + (deltaTime * source->v.x);
		target->pos.y = initial->pos.y + (deltaTime * source->v.y);
		target->pos.z = initial->pos.z + (deltaTime * source->v.z);

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

int CheckForCollisions( t_Particle	*system )
{
/// Local Variables ///////////////////////////////////////////////////////////
	int collisionState = NOT_COLLIDING;
    float const depthEpsilon = 0.001f;
    float const partDepthEpsilon = 0.0001f;
	int loop,planeIndex;
	t_Particle *curPart;
	t_CollisionPlane *plane;
	float axbyczd,relativeVelocity;
///////////////////////////////////////////////////////////////////////////////

	g_ContactCnt = 0;		// THERE ARE CURRENTLY NO CONTACTS

	curPart = system;
	for (loop = 0; (loop < FFD_NODE_COUNT) && (collisionState != PENETRATING);
			loop++,curPart++)
	{
        for(planeIndex = 0;(planeIndex < g_CollisionPlaneCnt) &&
            (collisionState != PENETRATING);planeIndex++)
        {
			plane = &g_CollisionPlane[planeIndex];

            axbyczd = DotProduct(&curPart->pos,&plane->normal) + plane->d;

            if(axbyczd < -depthEpsilon)
            {
				// ONCE ANY PARTICLE PENETRATES A WALL, QUIT THE LOOP
				collisionState = PENETRATING;
            }
            else
            if(axbyczd < depthEpsilon)
            {
                relativeVelocity = DotProduct(&plane->normal,&curPart->v);
				if(relativeVelocity < 0.0f)
                {
                    collisionState = COLLIDING_WITH_WALL;
					g_Contact[g_ContactCnt].type = COLLIDING_WITH_WALL;
					g_Contact[g_ContactCnt].particle = loop;
					g_Contact[g_ContactCnt].Kr = g_Kr;		// Particle to wall
					memcpy(&g_Contact[g_ContactCnt].normal,&plane->normal,sizeof(tVector));
					g_ContactCnt++;
                }
            }
        }
	}

    return collisionState;
}

// Handle the contact resolution
void ResolveCollisions( t_Particle	*system )
{
	t_Contact	*contact;
	t_Particle		*part;		// THE PARTICLE COLLIDING
	float		VdotN;
	tVector		Vn,Vt;				// CONTACT RESOLUTION IMPULSE
	int			loop;

	contact = g_Contact;
	for (loop = 0; loop < g_ContactCnt; loop++,contact++)
	{
		part = &system[contact->particle];
		// CALCULATE Vn
		VdotN = DotProduct(&contact->normal,&part->v);
		ScaleVector(&contact->normal, VdotN, &Vn);
		// CALCULATE Vt
		VectorDifference(&part->v, &Vn, &Vt);

		// Check if it was a collision with a wall or particle
		if (contact->type == COLLIDING_WITH_WALL)
		{
			// SCALE Vn BY COEFFICIENT OF RESTITUTION
			ScaleVector(&Vn, contact->Kr, &Vn);
			// SET THE VELOCITY TO BE THE NEW IMPULSE
			VectorDifference(&Vt, &Vn, &part->v);
		}

	}
}

void Simulate(float DeltaTime, bool running)
{
    float		CurrentTime = 0.0f;
    float		TargetTime = DeltaTime;
	t_Particle		*tempSys;
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
			g_CollisionRootFinding = true;
            // we simulated too far, so subdivide time and try again
            TargetTime = (CurrentTime + TargetTime) / 2.0f;

            // blow up if we aren't moving forward each step, which is
            // probably caused by interpenetration at the frame start
            assert(fabs(TargetTime - CurrentTime) > EPSILON);
        }
        else
        {
            // either colliding or clear
            if(collisionState == COLLIDING_WITH_WALL)
            {
                int Counter = 0;
                do
                {
                    ResolveCollisions(g_TargetSys);
                    Counter++;
                } while((CheckForCollisions(g_TargetSys) >=
                            COLLIDING_WITH_WALL) && (Counter < 500));

                assert(Counter < 500);
				g_CollisionRootFinding = false;	// FOUND THE COLLISION POINT BACK TO NORMAL
            }

            // we made a successful step, so swap configurations
            // to "save" the data for the next step

			CurrentTime = TargetTime;
			TargetTime = DeltaTime;

			MAKEVECTOR(g_CueForce,0.0f,0.0f,0.0f);	// Clear Cue Force

			// SWAP MY TWO SYSTEM BUFFERS SO I CAN DO IT AGAIN
			tempSys = g_CurrentSys;
			g_CurrentSys = g_TargetSys;
			g_TargetSys = tempSys;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Function:	SetMouseForce
// Purpose:		Allows the user to interact with selected points by dragging
// Arguments:	Delta distance from clicked point, local x and y axes
///////////////////////////////////////////////////////////////////////////////
void SetMouseForce(int deltaX,int deltaY, tVector *localX, tVector *localY)
{
/// Local Variables ///////////////////////////////////////////////////////////
	tVector tempX,tempY;
///////////////////////////////////////////////////////////////////////////////
	ScaleVector(localX,  (float)deltaX * 0.03f, &tempX);
	ScaleVector(localY,  -(float)deltaY * 0.03f, &tempY);
	if (g_Pick[0] > -1)
	{
		VectorSum(&g_CurrentSys[g_Pick[0]].pos,&tempX,&g_MouseDragPos[0]);
		VectorSum(&g_MouseDragPos[0],&tempY,&g_MouseDragPos[0]);
	}
	if (g_Pick[1] > -1)
	{
		VectorSum(&g_CurrentSys[g_Pick[1]].pos,&tempX,&g_MouseDragPos[1]);
		VectorSum(&g_MouseDragPos[1],&tempY,&g_MouseDragPos[1]);
	}
}
/// SetMouseForce /////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Function:	SetFFDWeights
// Purpose:		Approximate an FFD by setting up control weights
// Arguments:	Pointer to base mesh visual structure
///////////////////////////////////////////////////////////////////////////////
void SetFFDWeights(t_ToonVisual *visual)
{
/// Local Variables ///////////////////////////////////////////////////////////
	tVector *vertex;
	int loop,cvLoop;
	float	XBasis[4], YBasis[4], ZBasis[4];
	float	u, v, w;
	float	*vertexWeight;
	int		px,py,pz;
///////////////////////////////////////////////////////////////////////////////
	// Allocate the space for all the weights
	visual->weightData = (float *)malloc(visual->vertexCnt * FFD_NODE_COUNT * sizeof(float));
	vertex = visual->vertex;
	// Go through all the vertices
	for (loop = 0; loop < visual->vertexCnt; loop++, vertex++)
	{
		// Find where each vertex is within the FFD grid
		// Effectively scales each vertex to 0-1
		u = (vertex->x - g_FFDmin.x)/(g_FFDmax.x - g_FFDmin.x);
		v = (vertex->y - g_FFDmin.y)/(g_FFDmax.y - g_FFDmin.y);
		w = (vertex->z - g_FFDmin.z)/(g_FFDmax.z - g_FFDmin.z);

		// X Bezier Basis Functions
		XBasis[0] = (1.0f - u) * (1.0f - u) * (1.0f - u);
		XBasis[1] = 3.0f * u * (1.0f - u) * (1.0f - u);
		XBasis[2] = 3.0f * u * u * (1.0f - u);;
		XBasis[3] = u * u * u;

		// Y Bezier Basis Functions
		YBasis[0] = (1.0f - v) * (1.0f - v) * (1.0f - v);
		YBasis[1] = 3.0f * v * (1.0f - v) * (1.0f - v);
		YBasis[2] = 3.0f * v * v * (1.0f - v);;
		YBasis[3] = v * v * v;

		// Z Bezier Basis Functions
		ZBasis[0] = (1.0f - w) * (1.0f - w) * (1.0f - w);
		ZBasis[1] = 3.0f * w * (1.0f - w) * (1.0f - w);
		ZBasis[2] = 3.0f * w * w * (1.0f - w);;
		ZBasis[3] = w * w * w;

		// Pointer to Place to store weight data
		vertexWeight = &visual->weightData[loop * 64];
		// Go through the control vertices
		for (cvLoop = 0; cvLoop < FFD_NODE_COUNT; cvLoop++,vertexWeight++)
		{
			// Some quick math to find the component indices
			px = FFD_WIDTH - (cvLoop % FFD_WIDTH) - 1;
			py = FFD_HEIGHT - (cvLoop / (FFD_WIDTH * FFD_HEIGHT)) - 1;
			pz = FFD_DEPTH - ((cvLoop % (FFD_WIDTH * FFD_HEIGHT)) / FFD_WIDTH) - 1;

			// set the vertex weight for this CV
			*vertexWeight = (XBasis[px] * YBasis[py] * ZBasis[pz]);
		}
	}
}
/// SetFFDWeights /////////////////////////////////////////////////////////////
