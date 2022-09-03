#include <cstdio>
///////////////////////////////////////////////////////////////////////////////
//
// LoadOBJ.cpp : implementation file
//
// Purpose:	Implementation of OpenGL Window of OBJ Loader
//
// Created:
//		JL 9/23/98
//
// Notes: This version doesn't used shared vertices in a vertex array.  That
//		  would be a faster way of doing things.  This creates 3 vertices per
//        triangle.
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
#include <GL/gl.h>
#include <GL/glut.h>
#include "LoadOBJ.h"
#include <cstring>
#include <algorithm>
#include <vector>
#include <string>

///////////////////////////////////////////////////////////////////////////////
// Function:	ParseString
// Purpose:		Actually breaks a string of words into individual pieces
// Arguments:	Source string in, array to put the words and the count
///////////////////////////////////////////////////////////////////////////////
void ParseString(char *buffer, std::vector<std::string> *words, int *cnt) {
  std::string in = buffer, temp;

  in.erase(0, in.find_first_not_of(" \t"));
  in.erase(in.find_last_not_of(" \t") + 1);
  *cnt = 0;
  do {
    temp = in.substr(
        0, in.find_first_of(" \t"));
    words->push_back(temp);
    if (temp == in)
      break;
    in = in.substr(temp.length());
    in.erase(0, in.find_first_not_of(" \t"));
    *cnt = *cnt + 1;
  } while (1);
  *cnt = *cnt + 1;
}

//// ParseString //////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Procedure:	LoadMaterialLib
// Purpose:		Handles the Loading of a Material library
// Arguments:	Name of the Material Library
///////////////////////////////////////////////////////////////////////////////
void LoadMaterialLib(std::string name,t_Visual *visual)
{
/// Local Variables ///////////////////////////////////////////////////////////
	int cnt;
	char buffer[MAX_STRINGLENGTH];
    std::vector<std::string> words;
    std::string temp;
	FILE *fp;
///////////////////////////////////////////////////////////////////////////////
	strcpy(visual->map,"");
	fp = fopen(name.c_str(), "r");
	if (fp != NULL)
	{
		// FIRST PASS SETS UP THE NUMBER OF OBJECTS IN THE FILE
		while (!feof(fp))
		{
			fgets(buffer,MAX_STRINGLENGTH,fp);	// GET A STRING FROM THE FILE
			ParseString(buffer,&words,&cnt);	// BREAK THE STRING INTO cnt WORDS
			if (cnt > 0)						// MAKE SURE SOME WORDS ARE THERE
			{
				temp = words[0];			// CHECK THE FIRST WORK
				if (temp.length() > 0)
				{
					if (temp == "Ka")			// AMBIENT
					{
						visual->Ka.r = atof(words[1].c_str());
						visual->Ka.g = atof(words[2].c_str());
						visual->Ka.b = atof(words[3].c_str());
					}
					else if (temp == "Kd")		// DIFFUSE COLOR
					{
						visual->Kd.r = atof(words[1].c_str());
						visual->Kd.g = atof(words[2].c_str());
						visual->Kd.b = atof(words[3].c_str());
					}
					else if (temp == "Ks")		// SPECULAR COLOR
					{
						visual->Ks.r = atof(words[1].c_str());
						visual->Ks.g = atof(words[2].c_str());
						visual->Ks.b = atof(words[3].c_str());
					}
					else if (temp == "Ns")		// SPECULAR COEFFICIENT
					{
						visual->Ns = atof(words[1].c_str());
					}
					else if (temp == "map_Kd")	// TEXTURE MAP NAME
					{
						strcpy(visual->map,words[1].c_str());
					}
				}
			}
			words.clear();		// CLEAR WORD BUFFER
		}
		fclose(fp);
	}
}

///////////////////////////////////////////////////////////////////////////////
// Procedure:	HandleFace
// Purpose:		Handles the Face Line in an OBJ file.  Extracts index info to
//				a face Structure
// Arguments:	Array of words from the face line, place to put the data
// Notes:		Not an Official OBJ loader as it doesn't handle more then
//				3 vertex polygons.  This only handles Triangles
///////////////////////////////////////////////////////////////////////////////
void HandleFace(std::vector<std::string> *words,t_faceIndex *face)
{
/// Local Variables ///////////////////////////////////////////////////////////
	int loop;
    std::string temp;
	   std::string vStr,nStr,tStr;		// HOLD POINTERS TO ELEMENT POINTERS
	int nPos,tPos;
///////////////////////////////////////////////////////////////////////////////
	// LOOP THROUGH THE 3 WORDS OF THE FACELIST LINE, WORD 0 HAS 'f'
	for (loop = 1; loop < 4; loop++)
	{
		temp = (*words)[loop];			// GRAB THE NEXT WORD
		// FACE DATA IS IN THE FORMAT vertex/texture/normal
		tPos = temp.find('/');				// FIND THE '/' SEPARATING VERTEX AND TEXTURE
		vStr = temp.substr(0, tPos);				// GET THE VERTEX NUMBER
		temp[tPos] = ' ';				// CHANGE THE '/' TO A SPACE SO I CAN TRY AGAIN
		nPos = temp.find('/');				// FIND THE '/' SEPARATING TEXTURE AND NORMAL
		tStr = temp.substr(tPos + 1, nPos - tPos - 1);		// GET THE TEXTURE NUMBER
		nStr = temp.substr(nPos + 1);	// GET THE NORMAL NUMBER
		face->v[loop - 1] = atoi(vStr.c_str());		// STORE OFF THE INDEX FOR THE VERTEX
		face->t[loop - 1] = atoi(tStr.c_str());		// STORE OFF THE INDEX FOR THE TEXTURE
		face->n[loop - 1] = atoi(nStr.c_str());		// STORE OFF THE INDEX FOR THE NORMAL
	}
}
///// HandleFace //////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Procedure:	LoadOBJ
// Purpose:		Load an OBJ file into the current bone visual
// Arguments:	Name of 0BJ file and pointer to bone
// Notes:		Not an Official OBJ loader as it doesn't handle more then
//				3 vertex polygons or multiple objects per file.
///////////////////////////////////////////////////////////////////////////////
bool LoadOBJ(char *filename,t_Visual *visual)
{
/// Local Variables ///////////////////////////////////////////////////////////
	int loop,loop2,cnt;
	char buffer[MAX_STRINGLENGTH];
	std::vector<std::string> words;
	std::string temp;
	FILE *fp;
	long vCnt = 0, nCnt = 0, tCnt = 0, fCnt = 0;
	long vPos = 0, nPos = 0, tPos = 0, fPos = 0;
	tVector *vertex = NULL,*normal = NULL,*texture = NULL;
	t_faceIndex *face = NULL;
	float *data;
///////////////////////////////////////////////////////////////////////////////
	fp = fopen(filename,"r");
	if (fp != NULL)
	{
		// FIRST PASS SETS UP THE NUMBER OF OBJECTS IN THE FILE
		while (!feof(fp))
		{
			fgets(buffer,MAX_STRINGLENGTH,fp);	// GET A STRING FROM THE FILE
			ParseString(buffer,&words,&cnt);	// BREAK THE STRING INTO cnt WORDS
			if (cnt > 0)						// MAKE SURE SOME WORDS ARE THERE
			{
				temp = words[0];			// CHECK THE FIRST WORK
				if (temp.length() > 0)
				{
					if (temp[0] == 'v')			// ONLY LOOK AT WORDS THAT START WITH v
					{
						if (temp.length() > 1 && temp[1] == 'n')			// vn IS A NORMAL
							nCnt++;
						else if (temp.length() > 1 && temp[1] == 't')	// vt IS A TEXTURE
							tCnt++;
						else
							vCnt++;											// v IS A VERTEX
					}
					else if (temp[0] == 'f')
						fCnt++;												// f IS A FACE
				}
			}
			words.clear();		// CLEAR WORD BUFFER
		}

		// NOW THAT I KNOW HOW MANY, ALLOCATE ROOM FOR IT
		if (vCnt > 0)
		{
			vertex = (tVector *)malloc(vCnt * sizeof(tVector));
			if (nCnt > 0)
				normal = (tVector *)malloc(nCnt * sizeof(tVector));
			if (tCnt > 0)
				texture = (tVector *)malloc(tCnt * sizeof(tVector));
			if (fCnt > 0)
				face = (t_faceIndex *)malloc(fCnt * sizeof(t_faceIndex));

			fseek(fp,0,SEEK_SET);

			// NOW THAT IT IS ALL ALLOC'ED.  GRAB THE REAL DATA
			while (!feof(fp))
			{
				fgets(buffer,MAX_STRINGLENGTH,fp);
				ParseString(buffer,&words,&cnt);
				if (cnt > 0)
				{
					temp = words[0];
					if (temp.length() > 0)
					{
						if (temp[0] == 'v')		// WORDS STARTING WITH v
						{
							if (temp.length() > 1 && temp[1] == 'n')	// vn NORMALS
							{
								normal[nPos].x = atof(words[1].c_str());
								normal[nPos].y = atof(words[2].c_str());
								normal[nPos].z = atof(words[3].c_str());
								nPos++;
							}
							else if (temp.length() > 1 && temp[1] == 't')	// vt TEXTURES
							{
								texture[tPos].u = atof(words[1].c_str());
								texture[tPos].v = atof(words[2].c_str());
								tPos++;
							}
							else											// VERTICES
							{
								vertex[vPos].x = atof(words[1].c_str());
								vertex[vPos].y = atof(words[2].c_str());
								vertex[vPos].z = atof(words[3].c_str());
								vPos++;
							}
						}
						else if (temp[0] == 'f')			// f v/t/n v/t/n v/t/n	FACE LINE
						{
							if (words.size() > 4)
							{
								sprintf(buffer,"Face %ld has more then 3 vertices",fPos);
// 								MessageBox(NULL,buffer,"ERROR",MB_OK);
							}
							HandleFace(&words,&face[fPos]);
							fPos++;
						}
						else if (temp == "mtllib")  // HANDLE THE MATERIAL LIBRARY
						{
							LoadMaterialLib(words[1],visual);
						}
					}
				}
				words.clear();		// CLEAR WORD BUFFER
			}

			if (nCnt > 0)
			{
				if (tCnt > 0)
				{
					visual->dataFormat = GL_T2F_N3F_V3F;
					visual->vSize = 8;					// 2 texture, 3 normal, 3 vertex
					visual->vertexData = (float *)malloc(sizeof(float) * visual->vSize * fPos * 3);
					visual->triCnt = fPos;
				}
				else
				{
					visual->dataFormat = GL_N3F_V3F;
					visual->vSize = 6;					// 3 floats for normal, 3 for vertex
					visual->vertexData = (float *)malloc(sizeof(float) * visual->vSize * fPos * 3);
					visual->triCnt = fPos;
				}
			}
			else
			{
				visual->dataFormat = GL_V3F;
				visual->vSize = 3;					// 3 floats for vertex
				visual->vertexData = (float *)malloc(sizeof(float) * visual->vSize * fPos * 3);
				visual->triCnt = fPos;
			}

			data = visual->vertexData;
			for (loop = 0; loop < fPos; loop++)
			{
				for (loop2 = 0; loop2 < 3; loop2++)
				{
					// ALL FACE INDICES ARE 1 BASED INSTEAD OF 0
					if (tCnt > 0)	// IF TEXTURE COORDS WRITE OUT THOSE
					{
						*data++ = texture[face[loop].t[loop2] - 1].u;
						*data++ = texture[face[loop].t[loop2] - 1].v;
					}
					if (nCnt > 0)	// IF THERE ARE NORMALS WRITE THOSE OUT
					{
						*data++ = normal[face[loop].n[loop2] - 1].x;
						*data++ = normal[face[loop].n[loop2] - 1].y;
						*data++ = normal[face[loop].n[loop2] - 1].z;
					}
					*data++ = vertex[face[loop].v[loop2] - 1].x;	// SAVE OUT VERTICES
					*data++ = vertex[face[loop].v[loop2] - 1].y;
					*data++ = vertex[face[loop].v[loop2] - 1].z;
				}
			}

			if (vertex) free(vertex);
			if (normal) free(normal);
			if (texture) free(texture);
			if (face) free(face);
		}

		fclose(fp);
	}
	else
		return false;
	return true;
}
