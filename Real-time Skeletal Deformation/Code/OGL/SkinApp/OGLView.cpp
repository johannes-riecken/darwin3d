#include <cstring>
///////////////////////////////////////////////////////////////////////////////
//
// OGLView.cpp : implementation file
//
// Purpose:	Implementation of OpenGL Window of Deformation System
//
// Created:
//		JL  2/18/98
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

#ifdef _MSC_VER
#include "stdafx.h"
#endif
#include "SkinApp.h"
#include "OGLView.h"
#include "SetRot.h"
#include "Matrix.h"
#include "Weight.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/// Application Definitions ///////////////////////////////////////////////////
// IF YOU WANT TO TRY A QUICKER TWO BONE WEIGHTING SYSTEM UNDEF THIS
#define DEFORM_GENERAL_SOLUTION		// FULL MULTI-BONE DEFORMATION
#define OGL_AXIS_DLIST		1		// OPENGL DISPLAY LIST ID
#define ROTATE_SPEED		1.0		// SPEED OF ROTATION
///////////////////////////////////////////////////////////////////////////////

/// Global Variables //////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/// Message Maps //////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(COGLView, CWnd)
	//{{AFX_MSG_MAP(COGLView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// COGLView

COGLView::COGLView()
{
/// Local Variables ///////////////////////////////////////////////////////////
    int loop;
///////////////////////////////////////////////////////////////////////////////

	// INITIALIZE THE MODE KEYS
	m_DrawGeometry = true;
	m_DrawBoneSystem = true;
	m_DrawOutline = true;
	m_Defmode_TwoBone = true;
	m_Skeleton = NULL;
	m_SelectedBone = NULL;
	m_AnimBlend = 0.0;
	LoadObjectFile("Mesh.dgf",&m_Mesh);

	// SET UP THE SELECTION ARRAY FOR PICKING VERTICES
	m_SelectFlags = (int *)malloc( sizeof(int) * m_Mesh.desc->pointCnt);
	for (loop = 0; loop <  m_Mesh.desc->pointCnt; loop++)
	{
		m_SelectFlags[loop] = 0;
	}
}

COGLView::~COGLView()
{
	free(m_SelectFlags);
	// FREE THE BASE MESH STORAGE
	free(m_DeformedMesh);
	free(m_Skeleton->children[0].CV_weight);
	free(m_Skeleton->children[1].CV_weight);
}

bool COGLView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, t_Bone *skeleton, CCreateContext* pContext)
{
/// Local Variables ///////////////////////////////////////////////////////////
    int loop;
///////////////////////////////////////////////////////////////////////////////
	m_Skeleton = skeleton;

	// SET UP THE WEIGHTS FOR THE BONES
	// UPPER ARM
	m_Skeleton->children[0].CV_weight = (float *)malloc(m_Mesh.desc->pointCnt * sizeof(float));
	// LOWER ARM
	m_Skeleton->children[1].CV_weight = (float *)malloc(m_Mesh.desc->pointCnt * sizeof(float));
	// HAND - JUST FOR COMPLETENESS THE INTERFACE DOESN'T SUPPORT
	// DEFORMATION OF THE HAND.  WEIGHTING DIALOG IS A PAIN IN THE
	// BUTT.  HOWEVER, IF YOU SET UP THE WEIGHTS IT WOULD WORK
	m_Skeleton->children[2].CV_weight = (float *)malloc(m_Mesh.desc->pointCnt * sizeof(float));

	// SET INITIAL WEIGHT VALUES
	for (loop = 0; loop < m_Mesh.desc->pointCnt; loop++)
	{
		m_Skeleton->children[0].CV_weight[loop] = 1.0f;
		m_Skeleton->children[1].CV_weight[loop] = 0.0f;
		// HAND WEIGHTING IS ALWAYS ZERO
		m_Skeleton->children[2].CV_weight[loop] = 0.0f;
	}

	// PLACE TO STORE BASE MESH POINTS
	m_DeformedMesh = (tColoredVertex *)malloc(sizeof(tColoredVertex) * m_Mesh.desc->pointCnt);
	memcpy(m_DeformedMesh, m_Mesh.desc->frame[m_Mesh.desc->cur_frame]->data,sizeof(tColoredVertex) * m_Mesh.desc->pointCnt);

	// GET A DEFAULT WEIGHT SYSTEM
	GetWeights(CString("arm.wgt"));

	UpdateStatus();	// DRAW INITIAL STATUS BAR
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void COGLView::UpdateStatus()
{
/// Local Variables ///////////////////////////////////////////////////////////
    char message[80];
///////////////////////////////////////////////////////////////////////////////

	if (m_SelectedBone != NULL)
		m_ptrStatusBar->SetPaneText(1,m_SelectedBone->name);
	else
		m_ptrStatusBar->SetPaneText(1,"*No Bone Selected*");


	sprintf(message,"UArm (%.1f,%.1f,%.1f)",
		m_Skeleton->children[0].rot.x,
		m_Skeleton->children[0].rot.y,
		m_Skeleton->children[0].rot.z);
	m_ptrStatusBar->SetPaneText(2,message);

	sprintf(message,"LArm (%.1f,%.1f,%.1f)",
		m_Skeleton->children[1].rot.x,
		m_Skeleton->children[1].rot.y,
		m_Skeleton->children[1].rot.z);
	m_ptrStatusBar->SetPaneText(3,message);
}

/////////////////////////////////////////////////////////////////////////////
// COGLView message handlers

bool COGLView::SetupPixelFormat(HDC hdc)
{
/// Local Variables ///////////////////////////////////////////////////////////
    PIXELFORMATDESCRIPTOR pfd, *ppfd;
    int pixelformat;
///////////////////////////////////////////////////////////////////////////////
    ppfd = &pfd;

    ppfd->nSize = sizeof(PIXELFORMATDESCRIPTOR);
    ppfd->nVersion = 1;
    ppfd->dwFlags = PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    ppfd->dwLayerMask = PFD_MAIN_PLANE;
    ppfd->iPixelType = PFD_TYPE_RGBA;
    ppfd->cColorBits = 16;
    ppfd->cDepthBits = 16;
    ppfd->cAccumBits = 0;
    ppfd->cStencilBits = 0;

    pixelformat = ChoosePixelFormat(hdc, ppfd);

    if ((pixelformat = ChoosePixelFormat(hdc, ppfd)) == 0) {
        MessageBox("ChoosePixelFormat failed", "Error", MB_OK);
        return false;
    }

    if (pfd.dwFlags & PFD_NEED_PALETTE) {
        MessageBox("Needs palette", "Error", MB_OK);
        return false;
    }

    if (SetPixelFormat(hdc, pixelformat, ppfd) == false) {
        MessageBox("SetPixelFormat failed", "Error", MB_OK);
        return false;
    }

    return true;
}


///////////////////////////////////////////////////////////////////////////////
// Function:	CreateBoneDLists
// Purpose:		Creates the Drawlists for the Bones in a Skeleton
// Arguments:	Pointer to a bone hierarchy
///////////////////////////////////////////////////////////////////////////////
void COGLView::CreateBoneDLists(t_Bone *bone)
{
	// ONLY MAKE A BONE IF THERE IS A CHILD
	if (bone->childCnt > 0)
	{
		// CREATE THE DISPLAY LIST FOR A BONE
		glNewList(bone->id,GL_COMPILE);
			glBegin(GL_LINE_STRIP);
				glVertex3f( 0.0f,  0.4f, 0.0f);		// 0
				glVertex3f(-0.4f,  0.0f,-0.4f);		// 1
				glVertex3f( 0.4f,  0.0f,-0.4f);		// 2
				glVertex3f( 0.0f,  bone->children->trans.y, 0.0f);		// Base
				glVertex3f(-0.4f,  0.0f,-0.4f);		// 1
				glVertex3f(-0.4f,  0.0f, 0.4f);		// 4
				glVertex3f( 0.0f,  0.4f, 0.0f);		// 0
				glVertex3f( 0.4f,  0.0f,-0.4f);		// 2
				glVertex3f( 0.4f,  0.0f, 0.4f);		// 3
				glVertex3f( 0.0f,  0.4f, 0.0f);		// 0
				glVertex3f(-0.4f,  0.0f, 0.4f);		// 4
				glVertex3f( 0.0f,  bone->children->trans.y, 0.0f);		// Base
				glVertex3f( 0.4f,  0.0f, 0.4f);		// 3
				glVertex3f(-0.4f,  0.0f, 0.4f);		// 4
			glEnd();
		glEndList();
		// CHECK IF THIS BONE HAS CHILDREN, IF SO RECURSIVE CALL
		if (bone->childCnt > 0)
			CreateBoneDLists(bone->children);
	}
}

int COGLView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
/// Local Variables ///////////////////////////////////////////////////////////
	RECT rect;
///////////////////////////////////////////////////////////////////////////////
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
    m_hDC = ::GetDC(m_hWnd);
    if (!SetupPixelFormat(m_hDC))
		PostQuitMessage (0);

    m_hRC = wglCreateContext(m_hDC);
    wglMakeCurrent(m_hDC, m_hRC);
    GetClientRect(&rect);
    initializeGL(rect.right, rect.bottom);

	// CREATE THE DISPLAY LIST FOR AN AXIS WITH ARROWS POINTING IN
	// THE POSITIVE DIRECTION Red = X, Green = Y, Blue = Z
	glNewList(OGL_AXIS_DLIST,GL_COMPILE);
		glPushMatrix();
		glScalef(4.0,4.0,4.0);
		glBegin(GL_LINES);
			glColor3f(1.0f, 0.0f, 0.0f);	// X AXIS STARTS - COLOR RED
			glVertex3f(-0.2f,  0.0f, 0.0f);
			glVertex3f( 0.2f,  0.0f, 0.0f);
			glVertex3f( 0.2f,  0.0f, 0.0f);	// TOP PIECE OF ARROWHEAD
			glVertex3f( 0.15f,  0.04f, 0.0f);
			glVertex3f( 0.2f,  0.0f, 0.0f);	// BOTTOM PIECE OF ARROWHEAD
			glVertex3f( 0.15f, -0.04f, 0.0f);
			glColor3f(0.0f, 1.0f, 0.0f);	// Y AXIS STARTS - COLOR GREEN
			glVertex3f( 0.0f,  0.2f, 0.0f);
			glVertex3f( 0.0f, -0.2f, 0.0f);
			glVertex3f( 0.0f,  0.2f, 0.0f);	// TOP PIECE OF ARROWHEAD
			glVertex3f( 0.04f,  0.15f, 0.0f);
			glVertex3f( 0.0f,  0.2f, 0.0f);	// BOTTOM PIECE OF ARROWHEAD
			glVertex3f( -0.04f,  0.15f, 0.0f);
			glColor3f(0.0f, 0.0f, 1.0f);	// Z AXIS STARTS - COLOR BLUE
			glVertex3f( 0.0f,  0.0f,  0.2f);
			glVertex3f( 0.0f,  0.0f, -0.2f);
			glVertex3f( 0.0f,  0.0f, 0.2f);	// TOP PIECE OF ARROWHEAD
			glVertex3f( 0.0f,  0.04f, 0.15f);
			glVertex3f( 0.0f, 0.0f, 0.2f);	// BOTTOM PIECE OF ARROWHEAD
			glVertex3f( 0.0f, -0.04f, 0.15f);
		glEnd();
		glPopMatrix();
	glEndList();

	if (m_Skeleton != NULL)
	{
		CreateBoneDLists(m_Skeleton->children);
	}
	drawScene(false);
	return 0;
}

/* OpenGL code */
GLvoid COGLView::resize( GLsizei width, GLsizei height )
{
// Local Variables ///////////////////////////////////////////////////////////
    GLfloat aspect;
///////////////////////////////////////////////////////////////////////////////

    glViewport(0, 0, width, height);

    aspect = (GLfloat)width/(GLfloat)height;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(35.0, aspect, 1.0, 2000.0);
    glMatrixMode(GL_MODELVIEW);
}

GLvoid COGLView::initializeGL(GLsizei width, GLsizei height)
{
/// Local Variables ///////////////////////////////////////////////////////////
    GLfloat aspect;
///////////////////////////////////////////////////////////////////////////////

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0);
    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    aspect = (GLfloat)width/(GLfloat)height;
	// Establish viewing volume
	gluPerspective(40.0, aspect,1, 2000);
    glMatrixMode(GL_MODELVIEW);

	// SET SOME OGL INITIAL STATES SO THEY ARE NOT DONE IN THE DRAW LOOP
	glPolygonMode(GL_FRONT,GL_LINE);
	glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
	glPointSize(4.0);		// NICE BEEFY POINTS FOR THE VERTEX SELECTION
}

// GET THE INFO ON THE VERSION OF OPENGL RUNNING
void COGLView::GetGLInfo(char *who,char *which, char *version)
{
    strcpy(who,(char *)::glGetString( GL_VENDOR ));

    strcpy(which,(char *)::glGetString( GL_RENDERER ));

    strcpy(version, (char *)::glGetString( GL_VERSION ));
}

///////////////////////////////////////////////////////////////////////////////
// Function:	drawModel
// Purpose:		Draw the final deformed mesh to the screen
// Arguments:	Base bone for mesh, deformed mesh data to draw
///////////////////////////////////////////////////////////////////////////////
GLvoid COGLView::drawModel(t_Bone *baseBone, tColoredVertex *deformedMesh)
{
/// Local Variables ///////////////////////////////////////////////////////////
    int loop;
///////////////////////////////////////////////////////////////////////////////

	if (deformedMesh != NULL)
	{
		// THIS MODEL HAS VERTEX COLORS
		glInterleavedArrays(GL_C3F_V3F,0,(GLvoid *)deformedMesh);
		glDrawArrays(GL_TRIANGLES,0,baseBone->desc->pointCnt);

		// NOW DRAW THE VERTEX MARKERS IF THEY ARE SELECTED
		glColor3f(1.0f, 0.0f, 0.0f);	// Selected Vertices are Red
		glBegin(GL_POINTS);
		for (loop = 0; loop < baseBone->desc->pointCnt; loop++)
		{
			// IF A POINT IS SELECTED DRAW IT
			if (m_SelectFlags[loop])
			{
				glVertex3f(deformedMesh[loop].x,deformedMesh[loop].y,deformedMesh[loop].z);
			}
		}
		glEnd();
	}
}

///////////////////////////////////////////////////////////////////////////////
// Function:	DeformBone
// Purpose:		Deforms a model by the skeleton
// Arguments:	Pointer to base bone and original and deformed data
// Notes:		This is the general solution version of the deformation.
//				It recurses through the skeletal hierarchy and adds the
//				influence of each bone.  This should work with as many
//				bones as you have in the system.  I have tried it on a
//				model with a 25 bones system and it worked great.
///////////////////////////////////////////////////////////////////////////////
GLvoid COGLView::DeformBone(t_Bone *rootBone, tColoredVertex *meshdata, tColoredVertex *defdata)
{
/// Local Variables ///////////////////////////////////////////////////////////
    int loop,boneloop;
	float weight;
	tVector pre,post;
	t_Bone *curBone;
///////////////////////////////////////////////////////////////////////////////
	// START ON THIS BONES CHILDREN
	curBone = rootBone->children;
	// GO THROUGH THEM ALL
	for (boneloop = 0; boneloop < rootBone->childCnt; boneloop++)
	{
		// FOR EVERY VERTEX IN MY DEFORMABLE MESH
		for (loop = 0; loop < m_Mesh.desc->pointCnt; loop++)
		{
			// GET THE WEIGHT
			weight = curBone->CV_weight[loop];
			if (weight > 0.0)		// ONLY CARE IF IT IS > 0
			{
				// OFFSET THE ROTATIONAL CENTER ABOUT THAT BONES CENTER
				pre.x = meshdata[loop].x - rootBone->children[boneloop].trans.x;
				pre.y = meshdata[loop].y - rootBone->children[boneloop].trans.y;
				pre.z = meshdata[loop].z - rootBone->children[boneloop].trans.z;
				// PUT IT THROUGH THE ROTATION
				MultVectorByMatrix(&rootBone->children[boneloop].matrix, &pre, &post);
				// ADD IN THE WEIGHTED DELTA OF THIS POSITION
				defdata[loop].x += ((post.x - meshdata[loop].x) * weight);
				defdata[loop].y += ((post.y - meshdata[loop].y) * weight);
				defdata[loop].z += ((post.z - meshdata[loop].z) * weight);
			}
		}
		// CHECK IF THIS BONE HAS CHILDREN, IF SO RECURSIVE CALL
		if (curBone->childCnt > 0)
			DeformBone(curBone, meshdata, defdata);

		curBone++;
	}
}
/////  DeformBone  ////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Function:	DeformModel
// Purpose:		Deforms a model by the skeleton
// Arguments:	Pointer to base bone
// Notes:		This is a special version for testing.  It assumes that
//				I only have two bones weighted between them instead of
//				a general solution.  Since it is not recursive and handles
//				multiple bones, it is faster.  It simply calculates the
//				two possible positions and interpolates between them.
//				For some characters this may be better as it is faster.
///////////////////////////////////////////////////////////////////////////////
GLvoid COGLView::DeformModel(t_Bone *rootBone)
{
/// Local Variables ///////////////////////////////////////////////////////////
    int loop;
	tVector pre,post1,post2;
	tColoredVertex *meshdata;	// IN THIS CASE THE DATA IS COLOR VERTICES
	tColoredVertex *defdata;	// IN THIS CASE THE DATA IS COLOR VERTICES
///////////////////////////////////////////////////////////////////////////////
	// GET A POINTER TO THE ACTUAL VERTEX DATA
	meshdata = (tColoredVertex *)m_Mesh.desc->frame[m_Mesh.desc->cur_frame]->data;
	defdata = m_DeformedMesh;
	for (loop = 0; loop < m_Mesh.desc->pointCnt; loop++)
	{
		pre.x = meshdata[loop].x - rootBone->children[0].trans.x;
		pre.y = meshdata[loop].y - rootBone->children[0].trans.y;
		pre.z = meshdata[loop].z - rootBone->children[0].trans.z;
		MultVectorByMatrix(&rootBone->children[0].matrix, &pre, &post1);
		pre.x = meshdata[loop].x - rootBone->children[1].trans.x;
		pre.y = meshdata[loop].y - rootBone->children[1].trans.y;
		pre.z = meshdata[loop].z - rootBone->children[1].trans.z;
		MultVectorByMatrix(&rootBone->children[1].matrix, &pre, &post2);
		defdata[loop].x = post1.x + ((post2.x - post1.x) * rootBone->children[1].CV_weight[loop]);
		defdata[loop].y = post1.y + ((post2.y - post1.y) * rootBone->children[1].CV_weight[loop]);
		defdata[loop].z = post1.z + ((post2.z - post1.z) * rootBone->children[1].CV_weight[loop]);
	}
}
//// DeformModel /////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Function:	drawSkeleton
// Purpose:		Actually draws the Skeleton it is recursive
// Arguments:	None
///////////////////////////////////////////////////////////////////////////////
GLvoid COGLView::drawSkeleton(t_Bone *rootBone)
{
/// Local Variables ///////////////////////////////////////////////////////////
    int loop;
	t_Bone *curBone;
///////////////////////////////////////////////////////////////////////////////
	curBone = rootBone->children;
	for (loop = 0; loop < rootBone->childCnt; loop++)
	{
		glPushMatrix();

		// Set base orientation and position
		glTranslatef(curBone->trans.x, curBone->trans.y, curBone->trans.z);

		glRotatef(curBone->rot.z, 0.0f, 0.0f, 1.0f);
		glRotatef(curBone->rot.y, 0.0f, 1.0f, 0.0f);
		glRotatef(curBone->rot.x, 1.0f, 0.0f, 0.0f);

		// THE SCALE IS LOCAL SO I PUSH AND POP
		glPushMatrix();
		glScalef(curBone->scale.x, curBone->scale.y, curBone->scale.z);

		// DO I WISH TO DISPLAY ANYTHING
		if (m_DrawBoneSystem)
		{
			// DRAW THE AXIS OGL OBJECT
			glCallList(OGL_AXIS_DLIST);
			// DRAW THE ACTUAL BONE STRUCTURE
			// ONLY MAKE A BONE IF THERE IS A CHILD
			if (curBone->childCnt > 0)
			{
				if (curBone == m_SelectedBone)
					glColor3f(1.0f, 1.0f, 0.0f);	// Selected bone is bright Yellow
				else
					glColor3f(0.4f, 0.4f, 0.0f);	// Selected bone is dull Yellow
				// DRAW THE BONE STRUCTURE
				glCallList(curBone->id);
			}
		}

		// GRAB THE MATRIX AT THIS POINT SO I CAN USE IT FOR THE DEFORMATION
		glGetFloatv(GL_MODELVIEW_MATRIX,curBone->matrix.m);

		glPopMatrix();	// THIS POP IS JUST FOR THE SCALE

		// CHECK IF THIS BONE HAS CHILDREN, IF SO RECURSIVE CALL
		if (curBone->childCnt > 0)
			drawSkeleton(curBone);

		glPopMatrix();	// THIS POPS THE WHOLE MATRIX

		curBone++;
	}
}
//// drawSkeleton /////////////////////////////////////////////////////////////

GLvoid COGLView::drawScene(bool drawSelectRect)
{
/// Local Variables ///////////////////////////////////////////////////////////
	float m[16];
///////////////////////////////////////////////////////////////////////////////

	if (m_Skeleton->rot.y  > 360.0f) m_Skeleton->rot.y  -= 360.0f;
    if (m_Skeleton->rot.x   > 360.0f) m_Skeleton->rot.x   -= 360.0f;
    if (m_Skeleton->rot.z > 360.0f) m_Skeleton->rot.z -= 360.0f;

    glDisable(GL_DEPTH_TEST);	// TURN OFF DEPTH TEST FOR CLEAR

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);	// ENABLE DEPTH TESTING

    glPushMatrix();

    // Set root skeleton's orientation and position
    glTranslatef(m_Skeleton->trans.x, m_Skeleton->trans.y, m_Skeleton->trans.z);

	glRotatef(m_Skeleton->rot.z, 0.0f, 0.0f, 1.0f);
    glRotatef(m_Skeleton->rot.y, 0.0f, 1.0f, 0.0f);
 	glRotatef(m_Skeleton->rot.x, 1.0f, 0.0f, 0.0f);

	drawSkeleton(m_Skeleton);

    glPopMatrix();

	// GRAB THE MATRIX AT THIS POINT SO I CAN USE IT FOR THE DEFORMATION
	glGetFloatv(GL_MODELVIEW_MATRIX,m);

	if (m_DrawGeometry)
	{
// THIS IS TO ALLOW TESTING ON A SIMPLE TWO BONE CASE
#ifdef DEFORM_GENERAL_SOLUTION
		// THE FULL REAL DEFORMATION HANDLES AS MANY BONES AS YOU WANT
		// COPY THE ORIGINAL MESH INTO THE DEFORMABLE DATA
		memcpy(m_DeformedMesh, m_Mesh.desc->frame[m_Mesh.desc->cur_frame]->data,sizeof(tColoredVertex) * m_Mesh.desc->pointCnt);
		DeformBone(m_Skeleton,
			(tColoredVertex *)m_Mesh.desc->frame[m_Mesh.desc->cur_frame]->data,
			m_DeformedMesh);
#else
		// TEST DEFORMATION FOR WEIGHTING BETWEEN TWO BONES ONLY
		DeformModel(m_Skeleton);
#endif
		drawModel(&m_Mesh,m_DeformedMesh);
	}

	// IF I AM DRAGGING A SELECTION BOX, DRAW IT
	if (drawSelectRect)
	{
		glMatrixMode(GL_PROJECTION);			// I WANT TO PLAY WITH THE PROJECTION
		glPushMatrix();							// SAVE THE OLD ONE
			glLoadIdentity();					// LOAD A NEW ONE
			gluOrtho2D(0,m_ScreenWidth,0,m_ScreenHeight);	// USE WINDOW SETTINGS
			glColor3f(1.0f, 1.0f, 1.0f);		// DRAW A WHITE BOX
			glBegin(GL_LINE_STRIP);
			glVertex2s((short)m_SelectRect.left,(short)m_SelectRect.top);
			glVertex2s((short)m_SelectRect.right,(short)m_SelectRect.top);
			glVertex2s((short)m_SelectRect.right,(short)m_SelectRect.bottom);
			glVertex2s((short)m_SelectRect.left,(short)m_SelectRect.bottom);
			glVertex2s((short)m_SelectRect.left,(short)m_SelectRect.top);
			glEnd();
		glPopMatrix();							// RESTORE THE OLD PROJECTION
	    glMatrixMode(GL_MODELVIEW);				// BACK TO MODEL MODE
	}

    glFinish();

    SwapBuffers(m_hDC);

	// DRAW THE STATS AT THE BOTTOM OF THE SCREEN
	UpdateStatus();
}


void COGLView::OnDestroy()
{
	CWnd::OnDestroy();
	if (m_hRC)
		wglDeleteContext(m_hRC);
    if (m_hDC)
		::ReleaseDC(m_hWnd,m_hDC);
    m_hRC = 0;
    m_hDC = 0;


}

void COGLView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	drawScene(false);

	// Do not call CWnd::OnPaint() for painting messages
}

void COGLView::OnSize(UINT nType, int cx, int cy)
{
	// RESIZE THE OPENGL WINDOW
	resize( cx,cy );
	m_ScreenWidth = cx;
	m_ScreenHeight = cy;
}

void COGLView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_mousepos = point;
	if (m_SelectedBone != NULL)
	{
		m_Grab_Rot_X = 	m_SelectedBone->rot.x;
		m_Grab_Rot_Y = 	m_SelectedBone->rot.y;
		m_Grab_Rot_Z = 	m_SelectedBone->rot.z;
		m_Grab_Trans_X = 	m_Skeleton->trans.x;
		m_Grab_Trans_Y = 	m_Skeleton->trans.y;
		m_Grab_Trans_Z = 	m_Skeleton->trans.z;
	}
	m_SelectRect.left = point.x;
	m_SelectRect.top = m_ScreenHeight - point.y;
	CWnd::OnLButtonDown(nFlags, point);
}

void COGLView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// IF I AM HOLDING THE 'SHIFT' BUTTON PICK VERTICES
	if ((nFlags & MK_SHIFT) > 0)
	{
		SelectVertices(true);
		drawScene(false);
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void COGLView::OnRButtonDown(UINT nFlags, CPoint point)
{
	m_mousepos = point;
	if (m_SelectedBone != NULL)
	{
		m_Grab_Rot_X = 	m_SelectedBone->rot.x;
		m_Grab_Rot_Y = 	m_SelectedBone->rot.y;
		m_Grab_Rot_Z = 	m_SelectedBone->rot.z;
		m_Grab_Trans_X = 	m_Skeleton->trans.x;
		m_Grab_Trans_Y = 	m_Skeleton->trans.y;
		m_Grab_Trans_Z = 	m_Skeleton->trans.z;
	}
	m_SelectRect.left = point.x;
	m_SelectRect.top = m_ScreenHeight - point.y;
	CWnd::OnRButtonDown(nFlags, point);
}

void COGLView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// IF I AM HOLDING THE 'SHIFT' BUTTON PICK VERTICES
	if ((nFlags & MK_SHIFT) > 0)
	{
		SelectVertices(false);
		drawScene(false);
	}


	CWnd::OnRButtonUp(nFlags, point);
}

void COGLView::HandleKeyDown(UINT nChar)
{
}

void COGLView::HandleKeyUp(UINT nChar)
{
	switch (nChar)
	{
	case 'D':
		m_Defmode_TwoBone = !m_Defmode_TwoBone;
		break;
	case 'O':
		glPolygonMode(GL_FRONT,GL_LINE);
		m_DrawOutline = false;
		break;
	case 'F':
		glPolygonMode(GL_FRONT,GL_FILL);
		m_DrawOutline = true;
		break;
	case '1':
		m_SelectedBone = &m_Skeleton->children[0];
		break;
	case '2':
		m_SelectedBone = &m_Skeleton->children[1];
		break;
	case '3':
		m_SelectedBone = &m_Skeleton->children[2];
		break;
	case ' ':		// CLEAR THE SELECTION FLAGS
		for (int loop = 0; loop <  m_Mesh.desc->pointCnt; loop++)
		{
			m_SelectFlags[loop] = 0;
		}
		break;
	}
	drawScene(false);
}

void COGLView::OnMouseMove(UINT nFlags, CPoint point)
{
	m_SelectRect.right = point.x;
	m_SelectRect.bottom = m_ScreenHeight - point.y;
	if (m_SelectedBone != NULL)
	{
		if (nFlags & MK_LBUTTON > 0)
		{
			// IF I AM HOLDING THE 'CTRL' BUTTON TRANSLATE
			if ((nFlags & MK_CONTROL) > 0)
			{
				if ((point.x - m_mousepos.x) != 0)
				{
					m_Skeleton->trans.x = m_Grab_Trans_X + (.1f * (point.x - m_mousepos.x));
					drawScene(false);
				}
				if ((point.y - m_mousepos.y) != 0)
				{
					m_Skeleton->trans.y = m_Grab_Trans_Y - (.1f * (point.y - m_mousepos.y));
					drawScene(false);
				}
			}
			// ELSE ROTATE THE ROOT
			else if ((nFlags & MK_SHIFT) == 0)
			{
				if ((point.x - m_mousepos.x) != 0)
				{
					m_SelectedBone->rot.y = m_Grab_Rot_Y + ((float)ROTATE_SPEED * (point.x - m_mousepos.x));
					drawScene(false);
				}
				if ((point.y - m_mousepos.y) != 0)
				{
					m_SelectedBone->rot.x = m_Grab_Rot_X + ((float)ROTATE_SPEED * (point.y - m_mousepos.y));
					drawScene(false);
				}
				// SET THE NEW START KEYFRAME POSITION
				m_SelectedBone->p_rot.x = m_SelectedBone->rot.x;
				m_SelectedBone->p_rot.y = m_SelectedBone->rot.y;
				m_SelectedBone->p_rot.z = m_SelectedBone->rot.z;
				m_AnimBlend = 0.0;
			}
		}
		else if ((nFlags & MK_RBUTTON) == MK_RBUTTON)
		{
			if ((nFlags & MK_CONTROL) > 0)
			{
				if ((point.x - m_mousepos.x) != 0)
				{
					m_Skeleton->trans.z = m_Grab_Trans_Z + (.1f * (point.x - m_mousepos.x));
					drawScene(false);
				}
			}
			else if ((nFlags & MK_SHIFT) == 0)
			{
				if ((point.x - m_mousepos.x) != 0)
				{
					m_SelectedBone->rot.z = m_Grab_Rot_Z + ((float)ROTATE_SPEED * (point.x - m_mousepos.x));
					drawScene(false);
				}
				// SET THE NEW START KEYFRAME POSITION
				m_SelectedBone->p_rot.x = m_SelectedBone->rot.x;
				m_SelectedBone->p_rot.y = m_SelectedBone->rot.y;
				m_SelectedBone->p_rot.z = m_SelectedBone->rot.z;
				m_AnimBlend = 0.0;
			}
		}
	}
	// IF I AM HOLDING THE 'SHIFT' BUTTON DRAW MY SELECTION BOX
	if ((nFlags & MK_SHIFT) > 0 &&
		(((nFlags & MK_LBUTTON) > 0) || ((nFlags & MK_RBUTTON) == MK_RBUTTON)))
	{
		drawScene(true);
	}
	CWnd::OnMouseMove(nFlags, point);
}

void COGLView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
/// Local Variables ///////////////////////////////////////////////////////////
	CSetRot dialog;
///////////////////////////////////////////////////////////////////////////////
	if (m_SelectedBone != NULL)
	{
		dialog.m_XAxis = m_SelectedBone->rot.x;
		dialog.m_YAxis = m_SelectedBone->rot.y;
		dialog.m_ZAxis = m_SelectedBone->rot.z;
		if (dialog.DoModal())
		{
			m_SelectedBone->rot.x = dialog.m_XAxis;
			m_SelectedBone->rot.y = dialog.m_YAxis;
			m_SelectedBone->rot.z = dialog.m_ZAxis;
		}
		drawScene(false);
	}
}

///////////////////////////////////////////////////////////////////////////////
// Function:	LoadObjectFile
// Purpose:		Load a 3D Model in my format
// Arguments:	Name of file and place to put it
///////////////////////////////////////////////////////////////////////////////
void COGLView::LoadObjectFile(char *filename,t_Bone *bonePtr)
{
/// Local Variables ///////////////////////////////////////////////////////////
	FILE *fp;
	tObjDesc	*desc;			/* Pointer to Desc */
	tObjFrame	*frame;			/* Pointer to Desc */
	short loop2;
	char tempstr[80];
///////////////////////////////////////////////////////////////////////////////

	ResetBone(bonePtr, m_Skeleton);		// SETUP INITIAL BONE SETTINGS

	bonePtr->trans.x = 0;
	bonePtr->trans.y = 0;
	bonePtr->trans.z = 0;
	bonePtr->rot.x = 0.0;
	bonePtr->rot.y = 0.0;
	bonePtr->rot.z = 0.0;


	fp = fopen(filename,"rb");
	if (fp != NULL)
	{
		fread(tempstr,1,4,fp); // FDAT
		if (strncmp(tempstr,"DARW",4)!= 0)
		{
			MessageBox("Not a Valid DGF File","Load File", MB_OK|MB_ICONEXCLAMATION);
			return;
		}
		fread(tempstr,1,4,fp); // FDAT

		fread(bonePtr,sizeof(t_Bone),1,fp);
		fread(tempstr,1,4,fp); // FDAT
		bonePtr->desc = (tObjDesc *)malloc(sizeof(tObjDesc));
		desc = bonePtr->desc;
		fread(desc,sizeof(tObjDesc),1,fp);
		for (loop2 = 0; loop2 < desc->frameCnt; loop2++)
		{
			fread(tempstr,1,4,fp); // FDAT
			desc->frame[loop2] = (tObjFrame	*)malloc(sizeof(tObjFrame));
			frame = desc->frame[loop2];
			fread(frame,sizeof(tObjFrame),1,fp);
			frame->data = (float *)malloc(sizeof(float) * desc->dataSize * desc->pointCnt);
			fread(frame->data,sizeof(float),desc->dataSize * desc->pointCnt,fp);
		}
		bonePtr->cur_desc = 0;
		fclose(fp);
	}
}

///////////////////////////////////////////////////////////////////////////////
// Function:	SelectVertices
// Purpose:		Use Feedback to get all the vertices in the view
// Arguments:	Should I select or de-select?
///////////////////////////////////////////////////////////////////////////////
void COGLView::SelectVertices(bool select)
{
/// Local Variables ///////////////////////////////////////////////////////////
	GLfloat *feedBuffer;
	GLint hitCount;
	tColoredVertex *meshdata;	// IN THIS CASE THE DATA IS COLOR VERTICES
	int loop;
///////////////////////////////////////////////////////////////////////////////
	// GET A TEMP POINTER TO THE ACTUAL VERTEX DATA
	meshdata = m_DeformedMesh;
	// INITIALIZE A PLACE TO PUT ALL THE FEEDBACK INFO (3 DATA, 1 TAG, 2 TOKENS)
	feedBuffer = (GLfloat *)malloc(sizeof(GLfloat) * m_Mesh.desc->pointCnt * 6);
	// TELL OPENGL ABOUT THE BUFFER
	glFeedbackBuffer(m_Mesh.desc->pointCnt * 6,GL_3D,feedBuffer);
	(void)glRenderMode(GL_FEEDBACK);	// SET IT IN FEEDBACK MODE

	for (loop = 0; loop < m_Mesh.desc->pointCnt; loop++)
	{
		// PASS THROUGH A MARKET LETTING ME KNOW WHAT VERTEX IT WAS
		glPassThrough((float)loop);
		// SEND THE VERTEX
		glBegin(GL_POINTS);
		glVertex3f(meshdata[loop].x,meshdata[loop].y,meshdata[loop].z);
		glEnd();
	}
	hitCount = glRenderMode(GL_RENDER); // HOW MANY HITS DID I GET
	CompareBuffer(hitCount,feedBuffer, select);		// CHECK THEM AGAINST MY SELECTION
	free(feedBuffer);		// GET RID OF THE MEMORY
}
////// SelectVertices /////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Function:	CompareBuffer
// Purpose:		Check the feedback buffer to see if anything is tagged
// Arguments:	Number of hits, pointer to buffer, Should I select or de-select
///////////////////////////////////////////////////////////////////////////////
void COGLView::CompareBuffer(GLint size, GLfloat *buffer,bool select)
{
/// Local Variables ///////////////////////////////////////////////////////////
	GLint count;
	GLfloat token,point[3];
	int loop,currentVertex;
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
			// CHECK IF THE POINT WAS IN MY SELECTION RECTANGLE
			// FLOATS 0 AND 1 ARE SCREEN X AND Y
			// NOTE: OPENGL SETS THE BOTTOM Y=0
			if (point[0] >= m_SelectRect.left &&
				point[0] <= m_SelectRect.right &&
				point[1] <= m_SelectRect.top &&
				point[1] >= m_SelectRect.bottom)
				// SET THIS VERTEX TO THE CURRENT SELECTION VALUE
				m_SelectFlags[currentVertex] = select;
		}
	}
}
////// CompareBuffer //////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Function:	ModifyWeights
// Purpose:		Edit the bone weights via a dialog
// Arguments:	None
///////////////////////////////////////////////////////////////////////////////
void COGLView::ModifyWeights()
{
/// Local Variables ///////////////////////////////////////////////////////////
	CWeight dialog;
	int value,loop;
///////////////////////////////////////////////////////////////////////////////
	if (dialog.DoModal())
	{
		value = dialog.m_pos;
		// SET NEW WEIGHT VALUES
		for (loop = 0; loop < m_Mesh.desc->pointCnt; loop++)
		{
			if (m_SelectFlags[loop])
			{
				m_Skeleton->children[0].CV_weight[loop] = 1.0f - ((float)value / 256.0f);
				m_Skeleton->children[1].CV_weight[loop] = (float)value / 256.0f;
			}
		}
		drawScene(false);
	}
}

///////////////////////////////////////////////////////////////////////////////
// Function:	ResetWeights
// Purpose:		Reset the bone weights
// Arguments:	None
///////////////////////////////////////////////////////////////////////////////
void COGLView::ResetWeights()
{
/// Local Variables ///////////////////////////////////////////////////////////
	int loop;
///////////////////////////////////////////////////////////////////////////////
	// RESET ALL THE WEIGHTS TO THE DEFAULT
	for (loop = 0; loop < m_Mesh.desc->pointCnt; loop++)
	{
		m_Skeleton->children[0].CV_weight[loop] = 1.0f;
		m_Skeleton->children[1].CV_weight[loop] = 0.0f;
	}
	// RESET THE ROTATIONS FOR THE TWO ARM BONES
	m_Skeleton->children[0].rot.x = 0.0;
	m_Skeleton->children[0].rot.y = 0.0;
	m_Skeleton->children[0].rot.z = 0.0;
	m_Skeleton->children[1].rot.x = 0.0;
	m_Skeleton->children[1].rot.y = 0.0;
	m_Skeleton->children[1].rot.z = 0.0;
	drawScene(false);
}

///////////////////////////////////////////////////////////////////////////////
// Function:	GetWeights
// Purpose:		Get the bone weights from a file
// Arguments:	Filename to get it from
// Returns:		Success
///////////////////////////////////////////////////////////////////////////////
bool COGLView::GetWeights(CString filename)
{
/// Local Variables ///////////////////////////////////////////////////////////
	int loop;
	FILE *fp;
///////////////////////////////////////////////////////////////////////////////
	fp = fopen(filename,"rb");
	if (fp != NULL)
	{
		for (loop = 0; loop < m_Mesh.desc->pointCnt; loop++)
		{
			fread(&m_Skeleton->children[0].CV_weight[loop],sizeof(float),1,fp);
			fread(&m_Skeleton->children[1].CV_weight[loop],sizeof(float),1,fp);
		}
		fclose(fp);
		drawScene(false);
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////
// Function:	SaveWeights
// Purpose:		Save the bone weights to a file
// Arguments:	Filename to put it in
// Returns:		Success
///////////////////////////////////////////////////////////////////////////////
bool COGLView::SaveWeights(CString filename)
{
/// Local Variables ///////////////////////////////////////////////////////////
	int loop;
	FILE *fp;
///////////////////////////////////////////////////////////////////////////////
	fp = fopen(filename,"wb");
	if (fp != NULL)
	{
		for (loop = 0; loop < m_Mesh.desc->pointCnt; loop++)
		{
			fwrite(&m_Skeleton->children[0].CV_weight[loop],sizeof(float),1,fp);
			fwrite(&m_Skeleton->children[1].CV_weight[loop],sizeof(float),1,fp);
		}
		fclose(fp);
		return true;
	}
	return false;
}
