#include <cstdio>
///////////////////////////////////////////////////////////////////////////////
//
// LoadSkel.cpp
//
// Purpose: implementation of the Custom Skeleton file Loader
//
// Created:
//		JL 9/12/99
//
///////////////////////////////////////////////////////////////////////////////
//
//	Copyright 1999 Jeff Lander, All Rights Reserved.
//  For educational purposes only.
//  Please do not republish in electronic or print form without permission
//  Thanks - jeffl@darwin3d.com
//
///////////////////////////////////////////////////////////////////////////////
#ifdef _MSC_VER
#include "stdafx.h"
#endif
#include "LoadSkel.h"
#include <cstdlib>
#include <cstring>


#define MAX_CHILDREN 8
typedef struct
{
	void	*parent;
	void	*child[MAX_CHILDREN];
	int		childCnt;
	float	tx,ty,tz;
	float	rx,ry,rz;
	char	name[80];
} tSkeletonNode;


///////////////////////////////////////////////////////////////////////////////
// Fixes up the skeletal structure once it is loaded
///////////////////////////////////////////////////////////////////////////////
void AddDarwinBranch(t_Bone *root, tSkeletonNode *curNode,tSkeletonNode *baseNode)
{
/// Local Variables ///////////////////////////////////////////////////////////
	int i;
	t_Bone *child;
///////////////////////////////////////////////////////////////////////////////
	root->children = (t_Bone *)malloc((root->childCnt) * sizeof(t_Bone));
	for (i = 0; i < root->childCnt; i++)
	{
		child = &root->children[i];
		ResetBone(child,root);
		strcpy(child->name,baseNode[(intptr_t)curNode->child[i]].name);
		child->trans.x = baseNode[(intptr_t)curNode->child[i]].tx;
		child->trans.y = baseNode[(intptr_t)curNode->child[i]].ty;
		child->trans.z = baseNode[(intptr_t)curNode->child[i]].tz;
		child->rot.x = baseNode[(intptr_t)curNode->child[i]].rx;
		child->rot.y = baseNode[(intptr_t)curNode->child[i]].ry;
		child->rot.z = baseNode[(intptr_t)curNode->child[i]].rz;
		child->b_trans.x = baseNode[(intptr_t)curNode->child[i]].tx;
		child->b_trans.y = baseNode[(intptr_t)curNode->child[i]].ty;
		child->b_trans.z = baseNode[(intptr_t)curNode->child[i]].tz;
		child->b_rot.x = baseNode[(intptr_t)curNode->child[i]].rx;
		child->b_rot.y = baseNode[(intptr_t)curNode->child[i]].ry;
		child->b_rot.z = baseNode[(intptr_t)curNode->child[i]].rz;
		child->curMatrix = (tMatrix *)malloc(sizeof(tMatrix));
		child->childCnt = baseNode[(intptr_t)curNode->child[i]].childCnt;
		if (child->childCnt > 0)
		{
			AddDarwinBranch(child,&baseNode[(intptr_t)curNode->child[i]],baseNode);
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
// Function:	LoadSkeleton
// Purpose:		Load a Darwin Format Skeleton
// Arguments:	Name of the file to open and root skeleton to put it in
///////////////////////////////////////////////////////////////////////////////
bool LoadSkeleton(const char* name,t_Bone *root)
{
/// Local Variables ///////////////////////////////////////////////////////////
	int i,j,boneCnt,parent;
	FILE *fp;		// I PREFER THIS STYLE OF FILE ACCESS
	tSkeletonNode *tempBone,*bonelist;
	t_Bone *child;
	char	temp[5];
///////////////////////////////////////////////////////////////////////////////
	if ((fp = fopen((const char*)name,"rb"))) {
		fread(temp,sizeof(char),4,fp);
		fread(&boneCnt,sizeof(intptr_t),1,fp);
		bonelist = (tSkeletonNode *)malloc((boneCnt) * sizeof(tSkeletonNode));
		for (i = 0; i < boneCnt; i++)
		{
			tempBone = &bonelist[i];
			fread(tempBone->name,sizeof(char),80,fp);
			fread(&tempBone->tx,sizeof(float),1,fp);
			fread(&tempBone->ty,sizeof(float),1,fp);
			fread(&tempBone->tz,sizeof(float),1,fp);
			fread(&tempBone->rx,sizeof(float),1,fp);
			fread(&tempBone->ry,sizeof(float),1,fp);
			fread(&tempBone->rz,sizeof(float),1,fp);
			// FIND THE PARENT
			fread(&parent,sizeof(intptr_t),1,fp);
			if (parent == -1)
				tempBone->parent = NULL;
			else
				tempBone->parent = (t_Bone *)(long)parent;

			fread(&tempBone->childCnt,sizeof(intptr_t),1,fp);
			for (j = 0; j < tempBone->childCnt; j++)
			{
				fread(&tempBone->child[j],sizeof(intptr_t),1,fp);
			}
		}
		fclose(fp);

		// SET UP ROOT
//		strcpy(root->name,bonelist[0].name);
		root->b_trans.x = bonelist[0].tx;
		root->b_trans.y = bonelist[0].ty;
		root->b_trans.z = bonelist[0].tz;
		root->b_rot.x = bonelist[0].rx;
		root->b_rot.y = bonelist[0].ry;
		root->b_rot.z = bonelist[0].rz;
		root->trans.x = bonelist[0].tx;
		root->trans.y = bonelist[0].ty;
		root->trans.z = bonelist[0].tz;
		root->rot.x = bonelist[0].rx;
		root->rot.y = bonelist[0].ry;
		root->rot.z = bonelist[0].rz;
		root->childCnt = bonelist[0].childCnt;
		root->children = (t_Bone *)malloc((root->childCnt) * sizeof(t_Bone));
		root->curMatrix = (tMatrix *)malloc(sizeof(tMatrix));
		for (i = 0; i < root->childCnt; i++)
		{
			child = &root->children[i];
			ResetBone(child,root);
			strcpy(child->name,bonelist[(intptr_t)bonelist[0].child[i]].name);
			child->trans.x = bonelist[(intptr_t)bonelist[0].child[i]].tx;
			child->trans.y = bonelist[(intptr_t)bonelist[0].child[i]].ty;
			child->trans.z = bonelist[(intptr_t)bonelist[0].child[i]].tz;
			child->rot.x = bonelist[(intptr_t)bonelist[0].child[i]].rx;
			child->rot.y = bonelist[(intptr_t)bonelist[0].child[i]].ry;
			child->rot.z = bonelist[(intptr_t)bonelist[0].child[i]].rz;
			child->b_trans.x = bonelist[(intptr_t)bonelist[0].child[i]].tx;
			child->b_trans.y = bonelist[(intptr_t)bonelist[0].child[i]].ty;
			child->b_trans.z = bonelist[(intptr_t)bonelist[0].child[i]].tz;
			child->b_rot.x = bonelist[(intptr_t)bonelist[0].child[i]].rx;
			child->b_rot.y = bonelist[(intptr_t)bonelist[0].child[i]].ry;
			child->b_rot.z = bonelist[(intptr_t)bonelist[0].child[i]].rz;
			child->childCnt = bonelist[(intptr_t)bonelist[0].child[i]].childCnt;
			child->curMatrix = (tMatrix *)malloc(sizeof(tMatrix));
			if (child->childCnt > 0)
			{
				AddDarwinBranch(child,&bonelist[(intptr_t)bonelist[0].child[i]],bonelist);
			}
		}

		free(bonelist);

		// Move the initial position so it can be seen
		root->b_trans.x = 0.0f;
		root->b_trans.y = -7.0f;
		root->b_trans.z = -40.0f;
		root->trans.x = 0.0f;
		root->trans.y = -7.0f;
		root->trans.z = -40.0f;

	}
	return true;
}

