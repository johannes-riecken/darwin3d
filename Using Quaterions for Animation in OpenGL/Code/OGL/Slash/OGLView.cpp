///////////////////////////////////////////////////////////////////////////////
//
// OGLView.cpp : implementation file
//
// Purpose:	Implementation of OpenGL Window of Quaternion Animation System
//
// Created:
//		JL 11/1/97
//
///////////////////////////////////////////////////////////////////////////////
//
//	Copyright 1997 Jeff Lander, All Rights Reserved.
//  For educational purposes only.
//  Please do not republish in electronic or print form without permission
//  Thanks - jeffl@darwin3d.com
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Slash.h"
#include "OGLView.h"
#include "Model.h"		// SOFTIMAGE MODEL DATA
#include "SetRot.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/// Application Definitions ///////////////////////////////////////////////////
#define OGL_AXIS_DLIST		1		// OPENGL DISPLAY LIST ID
#define ROTATE_SPEED		1.0		// SPEED OF ROTATION
///////////////////////////////////////////////////////////////////////////////

/// Global Variables //////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// COGLView

COGLView::COGLView()
{
	// INITIALIZE THE MODE KEYS
	m_DrawGeometry = TRUE;
	m_UseQuat = TRUE;
	m_Skeleton = NULL;
}

COGLView::~COGLView()
{
}

///////////////////////////////////////////////////////////////////////////////
// Procedure:	UpdateStatus
// Purpose:		Update the status bar with orientation info
///////////////////////////////////////////////////////////////////////////////
void COGLView::UpdateStatus()
{
/// Local Variables ///////////////////////////////////////////////////////////
    char message[80];
///////////////////////////////////////////////////////////////////////////////

	// PRINT IF I AM IN EULER OR QUATERNION MODE
	if (m_UseQuat)
		m_ptrStatusBar->SetPaneText(1,"Quaternion Mode");
	else
		m_ptrStatusBar->SetPaneText(1,"Euler Mode");

	// PRINT THE CURRENT EULER ANGLES
	sprintf(message,"Rot (%.1f,%.1f,%.1f)",
		m_Skeleton->children->rot.x,
		m_Skeleton->children->rot.y,
		m_Skeleton->children->rot.z);
	m_ptrStatusBar->SetPaneText(2,message);

	// PRINT THE CURRENT QUATERNION
	sprintf(message,"Q (%.2f,%.2f,%.2f) %.2f",
		m_Skeleton->children->quat.x,
		m_Skeleton->children->quat.y,
		m_Skeleton->children->quat.z,
		m_Skeleton->children->quat.w);
	m_ptrStatusBar->SetPaneText(3,message);
}

BOOL COGLView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, t_Bone *skeleton, CCreateContext* pContext)
{
	m_Skeleton = skeleton;
	UpdateStatus();	// DRAW INITIAL STATUS BAR
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

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
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// COGLView message handlers

BOOL COGLView::SetupPixelFormat(HDC hdc)
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
        return FALSE;
    }

    if (pfd.dwFlags & PFD_NEED_PALETTE) {
        MessageBox("Needs palette", "Error", MB_OK);
        return FALSE;
    }

    if (SetPixelFormat(hdc, pixelformat, ppfd) == FALSE) {
        MessageBox("SetPixelFormat failed", "Error", MB_OK);
        return FALSE;
    }

    return TRUE;
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


	drawScene();
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
    gluPerspective(10.0, aspect, 1.0, 2000.0);
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
	gluPerspective(10.0, aspect,1, 2000);
    glMatrixMode(GL_MODELVIEW);

	// SET SOME OGL INITIAL STATES SO THEY ARE NOT DONE IN THE DRAW LOOP
	glPolygonMode(GL_FRONT,GL_FILL);
	glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
}

// GET THE INFO ON THE VERSION OF OPENGL RUNNING
void COGLView::GetGLInfo()
{
//// Local Variables ////////////////////////////////////////////////////////////////
	char *who, *which, *ver, *ext, *message;
	int len;
/////////////////////////////////////////////////////////////////////////////////////
	who = (char *)::glGetString( GL_VENDOR );
	which = (char *)::glGetString( GL_RENDERER );
	ver = (char *)::glGetString( GL_VERSION );
	ext = (char *)::glGetString( GL_EXTENSIONS );

	len = 200 + strlen(who) + strlen(which) + strlen(ver) + strlen(ext);

	message = (char *)malloc(len);
	sprintf(message,"Who:\t%s\nWhich:\t%s\nVersion:\t%s\nExtensions:\t%s",
		who, which, ver, ext);

	::MessageBox(NULL,message,"GL Info",MB_OK);

	free(message);
}

///////////////////////////////////////////////////////////////////////////////
// Procedure:	drawModel
// Purpose:		Draws the model associated with a bone
// Notes:		Currently uses a global model not associated with the bone
//              The data uses Quads with shared vertices and vertex coloring
//				so I chose to use indexed vertex arrays
///////////////////////////////////////////////////////////////////////////////
GLvoid COGLView::drawModel(t_Bone *curBone)
{
/// Local Variables ///////////////////////////////////////////////////////////
    int loop,pointloop;
///////////////////////////////////////////////////////////////////////////////
	// Declare the Array of Data
	glInterleavedArrays(SwordFORMAT,0,(GLvoid *)&gSwordModel);
	// Draw all the Quads at once
	glDrawElements(GL_QUADS,SwordPOLYCNT * 4,GL_UNSIGNED_SHORT,gSword);

	// THIS CODE WAS THE EQUIVALENT OF THE FOLLOWING, BUT FASTER
/*	glBegin(GL_QUADS);
	for (loop = 0; loop < SwordPOLYCNT; loop++)
	{
		for (pointloop = 0; pointloop < 4; pointloop++)
		{
			glArrayElement(gSword[loop][pointloop]);
		}
	}
	glEnd();*/
}
// drawModel

///////////////////////////////////////////////////////////////////////////////
// Procedure:	drawScene
// Purpose:		Draws the current OpenGL scene
///////////////////////////////////////////////////////////////////////////////
GLvoid COGLView::drawScene(GLvoid)
{
/// Local Variables ///////////////////////////////////////////////////////////
    int loop;
	t_Bone *curBone;
	tQuaternion axisAngle,temp;
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

	// ROTATE THE ROOT
	glRotatef(m_Skeleton->rot.z, 1.0f, 0.0f, 0.0f);
    glRotatef(m_Skeleton->rot.y, 0.0f, 1.0f, 0.0f);
 	glRotatef(m_Skeleton->rot.x, 0.0f, 0.0f, 1.0f);

	curBone = m_Skeleton->children;
	for (loop = 0; loop < m_Skeleton->childCnt; loop++)
	{
		glPushMatrix();

		// TRANSLATE OUT THE LOCAL BONE
		glTranslatef(curBone->trans.x, curBone->trans.y, curBone->trans.z);

		// CONVERT THE EULER TO QUATERNION. STORE IT FOR DISPLAY AND USE
		EulerToQuaternion(&curBone->rot,&curBone->quat);
	// TEST OF DIFFERENT METHOD OF CONVERSION
	//	EulerToQuaternion2(&curBone->rot,&curBone->quat);
		if (m_UseQuat)
		{
			// QUATERNION HAS TO BE CONVERTED TO AN AXIS/ANGLE REPRESENTATION
			QuatToAxisAngle(&curBone->quat,&axisAngle);
			AxisAngleToQuat((tVector *)&axisAngle, axisAngle.w, &temp);
			// DO THE ROTATION
			glRotatef(axisAngle.w, axisAngle.x, axisAngle.y, axisAngle.z);
		}
		else
		{
			// Rotate out the model via Euler angles
			glRotatef(curBone->rot.z, 0.0f, 0.0f, 1.0f);
			glRotatef(curBone->rot.y, 0.0f, 1.0f, 0.0f);
			glRotatef(curBone->rot.x, 1.0f, 0.0f, 0.0f);

		}

		// IF I WANT TO, DRAW THE GEOMETRY
		if (m_DrawGeometry)
			drawModel(curBone);

		// DRAW THE AXIS
		glCallList(OGL_AXIS_DLIST);

		glPopMatrix();

		curBone++;
	}

    glPopMatrix();
    glFinish();

    SwapBuffers(m_hDC);

	// DRAW THE STATS AT THE BOTTOM OF THE SCREEN
	UpdateStatus();
}
// 	drawScene

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
	drawScene();

	// Do not call CWnd::OnPaint() for painting messages
}

void COGLView::OnSize(UINT nType, int cx, int cy)
{
	// RESIZE THE OPENGL WINDOW
//	resize( cx,cy );
}

///////////////////////////////////////////////////////////////////////////////
// Procedure:	OnLButtonDown
// Purpose:		Left button down grabs the current point pos so I can use it
///////////////////////////////////////////////////////////////////////////////
void COGLView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_mousepos = point;
	m_Grab_Rot_X = 	m_Skeleton->children->rot.x;
	m_Grab_Rot_Y = 	m_Skeleton->children->rot.y;
	m_Grab_Rot_Z = 	m_Skeleton->children->rot.z;
	m_Grab_Trans_X = 	m_Skeleton->trans.x;
	m_Grab_Trans_Y = 	m_Skeleton->trans.y;
	m_Grab_Trans_Z = 	m_Skeleton->trans.z;
	CWnd::OnLButtonDown(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
// Procedure:	OnRButtonDown
// Purpose:		Right button down grabs the current point pos so I can use it
///////////////////////////////////////////////////////////////////////////////
void COGLView::OnRButtonDown(UINT nFlags, CPoint point)
{
	m_mousepos = point;
	m_Grab_Rot_X = 	m_Skeleton->children->rot.x;
	m_Grab_Rot_Y = 	m_Skeleton->children->rot.y;
	m_Grab_Rot_Z = 	m_Skeleton->children->rot.z;
	m_Grab_Trans_X = 	m_Skeleton->trans.x;
	m_Grab_Trans_Y = 	m_Skeleton->trans.y;
	m_Grab_Trans_Z = 	m_Skeleton->trans.z;
	CWnd::OnLButtonDown(nFlags, point);
}

void COGLView::HandleKeyDown(UINT nChar)
{
}

void COGLView::HandleKeyUp(UINT nChar)
{
	tVector	euler;
	switch (nChar)
	{
	case 'G':
		m_DrawGeometry = !m_DrawGeometry;
		break;
	case 'E':
		QuatToEuler(&m_Skeleton->children->quat, &euler);
	}
}

///////////////////////////////////////////////////////////////////////////////
// Procedure:	OnMouseMove
// Purpose:		Handle mouse moves while pressed
///////////////////////////////////////////////////////////////////////////////
void COGLView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (nFlags & MK_LBUTTON > 0)
	{
		// IF I AM HOLDING THE 'CONTROL' BUTTON TRANSLATE
		if ((nFlags & MK_CONTROL) > 0)
		{
			if ((point.x - m_mousepos.x) != 0)
			{
				m_Skeleton->trans.x = m_Grab_Trans_X + (.1f * (point.x - m_mousepos.x));
				drawScene();
			}
			if ((point.y - m_mousepos.y) != 0)
			{
				m_Skeleton->trans.y = m_Grab_Trans_Y - (.1f * (point.y - m_mousepos.y));
				drawScene();
			}
		}
		// ELSE ROTATE THE BONE
		else
		{
			if ((point.x - m_mousepos.x) != 0)
			{
				m_Skeleton->children->rot.y = m_Grab_Rot_Y + ((float)ROTATE_SPEED * (point.x - m_mousepos.x));
				drawScene();
			}
			if ((point.y - m_mousepos.y) != 0)
			{
				m_Skeleton->children->rot.x = m_Grab_Rot_X + ((float)ROTATE_SPEED * (point.y - m_mousepos.y));
				drawScene();
			}
		}
	}
	else if ((nFlags & MK_RBUTTON) == MK_RBUTTON)
	{
		if ((nFlags & MK_CONTROL) > 0)
		{
			if ((point.x - m_mousepos.x) != 0)
			{
				m_Skeleton->trans.z = m_Grab_Trans_Z + (.1f * (point.x - m_mousepos.x));
				drawScene();
			}
		}
		else
		{
			if ((point.x - m_mousepos.x) != 0)
			{
				m_Skeleton->children->rot.z = m_Grab_Rot_Z + ((float)ROTATE_SPEED * (point.x - m_mousepos.x));
				drawScene();
			}
		}
	}
	CWnd::OnMouseMove(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
// Procedure:	OnLButtonDblClk
// Purpose:		Left Double click, get dialog for Orientation
///////////////////////////////////////////////////////////////////////////////
void COGLView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CSetRot dialog;
	dialog.m_XAxis = m_Skeleton->children->rot.x;
	dialog.m_YAxis = m_Skeleton->children->rot.y;
	dialog.m_ZAxis = m_Skeleton->children->rot.z;
	if (dialog.DoModal())
	{
		m_Skeleton->children->rot.x = dialog.m_XAxis;
		m_Skeleton->children->rot.y = dialog.m_YAxis;
		m_Skeleton->children->rot.z = dialog.m_ZAxis;
	}
	drawScene();
}
