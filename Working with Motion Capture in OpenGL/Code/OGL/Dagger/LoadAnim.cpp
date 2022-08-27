///////////////////////////////////////////////////////////////////////////////
//
// LoadAnim.cpp
//
// Purpose: implementation of the Motion Capture file Loader
//
// Created:
//		JL 9/5/97
// Versions:
//		1.0		12/2/97	Initial Merge of the BVA Code
//		1.02	1/10/97 Merge in of ASF Hierarchy read code
//
// Todo:
//		Still trying to catch up from the holidays
//		Need to merge AMC portion of Acclaim loader for animation
//		Merge of BVH reader code
//
//		I WILL PROBABLY PULL OUT THE CHANNELS INTO A NEW STRUCT
//		ADD SPEED OF PLAYBACK VARIABLE TO CHANNEL STRUCT
//
///////////////////////////////////////////////////////////////////////////////
//
//	Copyright 1997 Jeff Lander, All Rights Reserved.
//  For educational purposes only.
//  Please do not republish in electronic or print form without permission
//  Thanks - jeffl@darwin3d.com
//
///////////////////////////////////////////////////////////////////////////////
#ifdef _MSC_VER
#include "stdafx.h"
#endif
#include "LoadAnim.h"

///////////////////////////////////////////////////////////////////////////////
// Function:	ParseString
// Purpose:		Actually breaks a string of words into individual pieces
// Arguments:	Source string in, array to put the words and the count
///////////////////////////////////////////////////////////////////////////////
void ParseString(char *buffer,CStringArray *words,int *cnt)
{
/// Local Variables ///////////////////////////////////////////////////////////
	CString in = buffer, temp;
///////////////////////////////////////////////////////////////////////////////

	in.TrimLeft();
	in.TrimRight();
	*cnt = 0;
	do
	{
		temp = in.SpanExcluding(" \t");		// GET UP TO THE NEXT SPACE OR TAB
		words->Add(temp);
		if (temp == in) break;
		in = in.Right(in.GetLength() - temp.GetLength());
		in.TrimLeft();
		*cnt = *cnt + 1;
	} while (1);
	*cnt = *cnt + 1;
}
//// ParseString //////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Function:	FindBoneParent
// Purpose:		Find the pointer to a bone with a given name
// Arguments:	Source string in, array to put the words and the count
///////////////////////////////////////////////////////////////////////////////
t_Bone *FindBoneParent(t_Bone *bone, char *name)
{
/// Local Variables ///////////////////////////////////////////////////////////
	t_Bone *retbone;
	int loop;
///////////////////////////////////////////////////////////////////////////////
	// IF I HAVE THE RIGHT ONE, RETURN IT
	if (strcmp(bone->name,name) == 0)
		return bone;
	if (bone->childCnt > 0)
	{
		for (loop = 0; loop < bone->childCnt; loop++)
		{
			// IF ONE OF THE CHILDREN IS THE ONE, RETURN IT
			if (strcmp(bone->children[loop].name,name) == 0)
				return &bone->children[loop];
			if (bone->children[loop].childCnt > 0)
			{
				retbone = FindBoneParent(&bone->children[loop],name);
				if (retbone != NULL)
					return retbone;
			}
		}

	}
	// ALL HAS FAILED, RETURN NULL
	return NULL;
}
//// FindBoneParent ///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Function:	LoadBVA
// Purpose:		Actually load a BVA file into the system
// Arguments:	Name of the file to open and root skeleton to put it in
///////////////////////////////////////////////////////////////////////////////
BOOL LoadBVA(CString name,t_Bone *root)
{
/// Local Variables ///////////////////////////////////////////////////////////
	FILE *fp;		// I PREFER THIS STYLE OF FILE ACCESS
	int loop,loop2,cnt;
	char buffer[MAX_STRINGLENGTH];
	CStringArray words;
	CString temp;
	int frameCnt;
	float frameTime;
	t_Bone *tempBones,*curBone;
	float *tempChannel,*fptr;
///////////////////////////////////////////////////////////////////////////////
	// OPEN THE BVA FILE
	if (fp = fopen((LPCTSTR)name,"r")) {
		while (!feof(fp))
		{
			fgets(buffer,MAX_STRINGLENGTH,fp);
			ParseString(buffer,&words,&cnt);
			// SEE IF WE CAN FIND THE SEGMENT KEYWORD
			if (words.GetAt(0) == "Segment:")
			{
				// IF SO, WE FOUND A BONE SO ALLOC ROOM FOR IT
				tempBones = (t_Bone *)malloc((root->childCnt + 1) * sizeof(t_Bone));
				if (root->childCnt > 0 && root->children != NULL)
				{
					// COPY OVER THE ONES THAT WERE ALREADY THERE
					memcpy(tempBones,root->children,root->childCnt * sizeof(t_Bone));
					free(root->children);
				}
				curBone = &tempBones[root->childCnt++];
				root->children = tempBones;
				// COPY THE NAME INTO THE BONE
				strcpy(curBone->name,words.GetAt(1));
				// SET THE ID TO THE BONE NUMBER
				curBone->id = root->childCnt;
				ResetBone(curBone, root);		// SETUP INITIAL BONE SETTINGS
				words.RemoveAll();		// CLEAR WORD BUFFER
				// NEXT GET THE FRAMECOUNT
				fgets(buffer,MAX_STRINGLENGTH,fp);
				ParseString(buffer,&words,&cnt);
				// NEXT LINE SHOULD BE FRAMES THEN A COUNT
				if (words.GetAt(0) == "Frames:" && cnt == 2)
				{
					frameCnt = atoi(words.GetAt(1));
					words.RemoveAll();		// CLEAR WORD BUFFER
					// NEXT GET THE FRAMETIME
					fgets(buffer,MAX_STRINGLENGTH,fp);
					ParseString(buffer,&words,&cnt);
					if (words.GetAt(0) == "Frame" &&
						words.GetAt(1) == "Time:" &&
						cnt == 3)
					{
						frameTime = (float)atof(words.GetAt(2));
						words.RemoveAll();		// CLEAR WORD BUFFER
						// TWO JUNK LINES FOR THE UNITS.  I DON'T CARE
						fgets(buffer,MAX_STRINGLENGTH,fp);
						fgets(buffer,MAX_STRINGLENGTH,fp);
						// NOW READY TO LOAD ALL THE ANIMATION
						// ALLOC 9 FLOATS PER FRAME SINCE BVA HAS 9 ELEMENTS TO THE CHANNEL
						tempChannel = (float *)malloc(sizeof(float) * frameCnt *
							s_Channel_Type_Size[CHANNEL_TYPE_SRT]);
						fptr = tempChannel;
						// LOOP THROUGH THE FRAMES OF ANIMATION DATA
						for (loop = 0; loop < frameCnt; loop++)
						{
							fgets(buffer,MAX_STRINGLENGTH,fp);
							ParseString(buffer,&words,&cnt);
							if (cnt == s_Channel_Type_Size[CHANNEL_TYPE_SRT])
							{
								for (loop2 = 0; loop2 < s_Channel_Type_Size[CHANNEL_TYPE_SRT]; loop2++)
								{
									*fptr = (float)atof(words.GetAt(loop2));
									// CONVERT INCHES TO FEET
									if (loop2 < 3) *fptr = *fptr / 12;
									fptr++;
								}
							}
							else
							{
								sprintf(buffer,"Not Enough Entries in channel %s frame %d\nExpected %d Got %d",curBone->name,loop,s_Channel_Type_Size[CHANNEL_TYPE_SRT],cnt);
								::MessageBox(NULL,buffer,"BVA Load ERROR!!",MB_OK);
								free(tempChannel);
								return FALSE;
							}
							words.RemoveAll();		// CLEAR WORD BUFFER
						}

						// SET THE CHANNEL INFO INTO BONE
						curBone->primChannel = tempChannel;
						curBone->primChanType = CHANNEL_TYPE_SRT;
						curBone->primFrameCount = (float)frameCnt;
						curBone->primSpeed = (float)(30.0 * frameTime);	// CONVERT TO FRAME STEP SIZE AT 30FPS
						BoneSetFrame(curBone,0);
					}

				}
				else
				{
					sprintf(buffer,"Couldn't Find FrameCnt Area for Bone %s",curBone->name);
					::MessageBox(NULL,buffer,"BVA Load ERROR!!",MB_OK);
					free(tempBones);
					root->childCnt = 0;
					root->children = NULL;
					return FALSE;
				}
			}
		}
		fclose(fp);
		return TRUE;
	}
	return FALSE;
}
//// LoadBVA //////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Function:	LoadBVH
// Purpose:		Actually load a BVH file into the system
// Arguments:	Name of the file to open and root skeleton to put it in
///////////////////////////////////////////////////////////////////////////////
BOOL LoadBVH(CString name,t_Bone *root)
{
/// Local Variables ///////////////////////////////////////////////////////////
	FILE *fp;		// I PREFER THIS STYLE OF FILE ACCESS
	int cnt;
	char buffer[MAX_STRINGLENGTH];
	CStringArray words;
	CString temp;
	t_Bone *tempBones,*curBone,*parent;
	int boneNum = 0;
	float **dataptr,*animData = NULL;
	int channelCnt = 0,frameCnt = 0;
///////////////////////////////////////////////////////////////////////////////
	dataptr = (float **)malloc(sizeof(float *) * 255);
	curBone = root;
	// OPEN THE BVH FILE
	if (fp = fopen((LPCTSTR)name,"r")) {
		while (!feof(fp))
		{
			fgets(buffer,MAX_STRINGLENGTH,fp);
			ParseString(buffer,&words,&cnt);
			// SEE IF WE CAN FIND THE BONE SEGMENT KEYWORD
			if (words.GetAt(0) == "ROOT" || words.GetAt(0) == "JOINT" || words.GetAt(0) == "End")
			{
				// IF SO, WE FOUND A BONE SO ALLOC ROOM FOR IT
				tempBones = (t_Bone *)malloc((curBone->childCnt + 1) * sizeof(t_Bone));
				if (curBone->childCnt > 0 && curBone->children != NULL)
				{
					// COPY OVER THE ONES THAT WERE ALREADY THERE
					memcpy(tempBones,curBone->children,curBone->childCnt * sizeof(t_Bone));
					// fix up the data pointers for any copied ones
					for (int i = 0; i < curBone->childCnt; i++)
					{
						t_Bone *b1,*b2;
						b1 = &curBone->children[i];
						b2 = &tempBones[i];
						for (int j = 0; j < channelCnt; j++)
						{
							if (dataptr[j] == &b1->rot.x)  dataptr[j] = &b2->rot.x;
							if (dataptr[j] == &b1->rot.y)  dataptr[j] = &b2->rot.y;
							if (dataptr[j] == &b1->rot.z)  dataptr[j] = &b2->rot.z;
							if (dataptr[j] == &b1->trans.x)  dataptr[j] = &b2->trans.x;
							if (dataptr[j] == &b1->trans.y)  dataptr[j] = &b2->trans.y;
							if (dataptr[j] == &b1->trans.z)  dataptr[j] = &b2->trans.z;
						}
					}
					free(curBone->children);
				}
				parent = curBone;
				curBone->childCnt++;
				boneNum++;
				curBone->children = tempBones;
				curBone = &tempBones[curBone->childCnt - 1];
				// COPY THE NAME INTO THE BONE
				if (words.GetSize() > 2)
					sprintf(curBone->name,"%s %s",words.GetAt(1),words.GetAt(2));
				else
					sprintf(curBone->name,"%s",words.GetAt(1));
				// SET THE ID TO THE BONE NUMBER
				curBone->id = boneNum;
				ResetBone(curBone, parent);		// SETUP INITIAL BONE SETTINGS
//				curBone->curMatrix = (tMatrix *)malloc(sizeof(tMatrix));

				words.RemoveAll();		// CLEAR WORD BUFFER
				// NEXT GET THE FRAMECOUNT
				fgets(buffer,MAX_STRINGLENGTH,fp);
				ParseString(buffer,&words,&cnt);
				if (words.GetAt(0) != "{")
				{
					::MessageBox(NULL,"Error parsing BVH File: No opening Braces","ERROR",MB_OK);
				}
			}
			else if (words.GetAt(0) == "CHANNELS")	// Channel Pointers
			{
				for (int loop = 0; loop < atoi(words.GetAt(1)); loop++)
				{
					if (words.GetAt(2 + loop) == "Xposition")
					{
						dataptr[channelCnt++] = &curBone->trans.x;
					}
					else if (words.GetAt(2 + loop) == "Yposition")
					{
						dataptr[channelCnt++] = &curBone->trans.y;
					}
					else if (words.GetAt(2 + loop) == "Zposition")
					{
						dataptr[channelCnt++] = &curBone->trans.z;
					}
					else if (words.GetAt(2 + loop) == "Xrotation")
					{
						dataptr[channelCnt++] = &curBone->rot.x;
					}
					else if (words.GetAt(2 + loop) == "Yrotation")
					{
						dataptr[channelCnt++] = &curBone->rot.y;
					}
					else if (words.GetAt(2 + loop) == "Zrotation")
					{
						dataptr[channelCnt++] = &curBone->rot.z;
					}
				}
			}
			else if (words.GetAt(0) == "OFFSET")
			{
				curBone->trans.x = curBone->b_trans.x = atof(words.GetAt(1)) / 10.0f;
				curBone->trans.y = curBone->b_trans.y = atof(words.GetAt(2)) / 10.0f;
				curBone->trans.z = curBone->b_trans.z = atof(words.GetAt(3)) / 10.0f;
			}
			else if (words.GetAt(0) == "}")
			{
				curBone = curBone->parent;
			}
			else if (words.GetAt(0) == "MOTION")
			{
				words.RemoveAll();		// CLEAR WORD BUFFER
				break;
			}
			words.RemoveAll();		// CLEAR WORD BUFFER
		}
		// Now we should be in motion area
		while (!feof(fp))
		{
			fgets(buffer,MAX_STRINGLENGTH,fp);
			ParseString(buffer,&words,&cnt);
			if (words.GetAt(0) == "Frames:")
			{
				frameCnt = atoi(words.GetAt(1));
				animData = (float *)malloc(sizeof(float) * channelCnt * frameCnt);
				if (animData != NULL)
				{
					words.RemoveAll();		// CLEAR WORD BUFFER
					// NEXT GET THE FRAME TIME
					fgets(buffer,MAX_STRINGLENGTH,fp);
					ParseString(buffer,&words,&cnt);
					words.RemoveAll();		// CLEAR WORD BUFFER
					for (int loop = 0; loop < frameCnt; loop++)
					{
						fgets(buffer,MAX_STRINGLENGTH,fp);
						ParseString(buffer,&words,&cnt);
						for (int loop2 = 0; loop2 < channelCnt; loop2++)
						{
							animData[loop * channelCnt + loop2] = atof(words.GetAt(loop2));
							if (loop2 < 3)
								animData[loop * channelCnt + loop2] = animData[loop * channelCnt + loop2] / 10.0f;
						}

						words.RemoveAll();		// CLEAR WORD BUFFER
					}

				}
			}

			words.RemoveAll();		// CLEAR WORD BUFFER
		}
		// Set the First Frame
		for (int loop = 0; loop < channelCnt; loop++)
			*dataptr[loop] = animData[loop];


		// SET THE CHANNEL INFO INTO BONE
		root->primChannel = animData;
		root->primChanType = CHANNEL_TYPE_BVH;
		root->primFrameCount = (float)frameCnt;
		root->primCurFrame = 0;
		root->primSpeed = (float)(1);	// CONVERT TO FRAME STEP SIZE AT 30FPS
		root->secChannel = (float *)dataptr;
		root->secFrameCount = (float)channelCnt;

		fclose(fp);
		return TRUE;
	}
	return FALSE;
}
//// LoadBVH //////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Function:	HandleAcclaimBone
// Purpose:		Parse the Bone block in an Acclaim ASF file
// Arguments:	Pointer to file handle and bone root
///////////////////////////////////////////////////////////////////////////////
void HandleAcclaimBone(FILE *fp,t_Bone *root)
{
/// Local Variables ///////////////////////////////////////////////////////////
	int cnt;
	char buffer[MAX_STRINGLENGTH];
	char mess[MAX_STRINGLENGTH];		// FOR ERROR MESSAGE
	CStringArray words;
	t_Bone *tempBones,*curBone;
	float dirx = 0.0,diry = 0.0,dirz = 0.0;		// TEMP STORAGE FOR BONE DIRECTION
	float length = 0.0;							// BONE LENGTH STORAGE
///////////////////////////////////////////////////////////////////////////////
	// IF SO, WE FOUND A BONE SO ALLOC ROOM FOR IT
	tempBones = (t_Bone *)malloc((root->childCnt + 1) * sizeof(t_Bone));
	if (root->childCnt > 0 && root->children != NULL)
	{
		// COPY OVER THE ONES THAT WERE ALREADY THERE
		memcpy(tempBones,root->children,root->childCnt * sizeof(t_Bone));
		free(root->children);
	}
	// GRAB A HANDLE TO THE CURRENT BONE
	curBone = &tempBones[root->childCnt++];
	ResetBone(curBone,NULL);
	// SINCE THE ACCLAIM SCALE IS SO MUCH LARGE, I AM SCALING UP THE BONES
	// SO YOU CAN ACTUALLY SEE THEM
	curBone->scale.x =
	curBone->scale.y =
	curBone->scale.z = 10.0;

	// I AM ASSUMING A ROTATION ONLY CHANNEL SETTING
	// SINCE NOT ALL BLOCKS HAVE DOF IN MY DATA
	curBone->flags = CHANNEL_TYPE_RXYZ;	// ROTATION (RX RY RZ) ORDER
	root->children = tempBones;
	do
	{
		words.RemoveAll();		// CLEAR WORD BUFFER
		fgets(buffer,MAX_STRINGLENGTH,fp);
		ParseString(buffer,&words,&cnt);
		// HANDLE THE ID KEYWORD
		if (words.GetAt(0) == "id")
		{
			curBone->id = atoi(words.GetAt(1));
		}
		// HANDLE THE NAME KEYWORD
		else if (words.GetAt(0) == "name")
		{
			strcpy(curBone->name, words.GetAt(1));
		}
		// HANDLE THE DIRECTION KEYWORD
		else if (words.GetAt(0) == "direction")
		{
			// THIS CONTROLS THE VECTOR AT WHICH THE BONE IS DEFINED
			// SO I WANT TO STORE IT
			dirx = atof(words.GetAt(1));
			diry = atof(words.GetAt(2));
			dirz = atof(words.GetAt(3));
		}
		// HANDLE THE LENGTH KEYWORD
		else if (words.GetAt(0) == "length")
		{
			length = atof(words.GetAt(1));
			// STORE OFF THE LENGTH VECTOR TO CALC THE TRANSFORMS LATER
			curBone->length.x = length * dirx;
			curBone->length.y = length * diry;
			curBone->length.z = length * dirz;
		}
		// HANDLE THE BODYMASS KEYWORD
		else if (words.GetAt(0) == "bodymass")
		{
			curBone->mass = atof(words.GetAt(1));
		}
		// HANDLE THE COFMASS KEYWORD
		else if (words.GetAt(0) == "cofmass")
		{
			// CENTER IS A PERCENTAGE ALONG THE BONE SO I MULT BY THE LENGTH VECTOR
			curBone->center.x = atof(words.GetAt(1)) * curBone->b_trans.x;
			curBone->center.y = atof(words.GetAt(1)) * curBone->b_trans.y;
			curBone->center.z = atof(words.GetAt(1)) * curBone->b_trans.z;
		}
		// HANDLE THE DOF KEYWORD
		else if (words.GetAt(0) == "dof")
		{
			// HERE WE NEED TO DETERMINE THE NUMBER OF CHANNELS AND ORDER
			if (words.GetAt(1) == "rx" && words.GetAt(2) == "ry" &&
				words.GetAt(3) == "rz")
			{
				curBone->flags = CHANNEL_TYPE_RXYZ;	// ROTATION (RX RY RZ) ORDER
			}
			// I AM ONLY SUPPORTING THAT DOF SO REPORT OTHERS
			// NONE OF MY DATA IS IN ANOTHER FORMAT
			else
			{
				sprintf(mess,"Unsupported DOF for Bone '%s': %s",curBone->name,buffer);
				::MessageBox(NULL,buffer,"ASSF Load ERROR!!",MB_OK);
			}
		}
	}
	// I AM NOT HANDLING THE "LIMITS" OR "AXIS" BLOCKS
	// KEEP PARSING UNTIL WE HIT AN END BLOCK
	while ((words.GetAt(0) != "end") && !feof(fp));
}
//// HandleAcclaimBone ////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Function:	HandleAcclaimHierarchy
// Purpose:		Parse the Hierarchy block in an Acclaim ASF file
// Arguments:	Pointer to file handle and bone root
// Discussion:	Tricky point is that since the position of the bone is
//				Based on the length of the parent's bone, I need to fix
//				this up at this point once I know the parent.
///////////////////////////////////////////////////////////////////////////////
void HandleAcclaimHierarchy(FILE *fp,t_Bone *root)
{
/// Local Variables ///////////////////////////////////////////////////////////
	int bonecnt,loop,loop2,cnt;
	char buffer[MAX_STRINGLENGTH];
	char	name[80];					// BONE NAME
	CStringArray words;
	t_Bone *tempBone,*bonelist;
///////////////////////////////////////////////////////////////////////////////
	// ALLOC ROOM FOR THE BONE LIST
	bonelist = (t_Bone *)malloc((root->childCnt) * sizeof(t_Bone));
	if (root->childCnt > 0 && root->children != NULL)
	{
		// COPY OVER THE ONES THAT WERE ALREADY THERE
		memcpy(bonelist,root->children,root->childCnt * sizeof(t_Bone));
		bonecnt = root->childCnt;
		free(root->children);
	}
	do
	{
		words.RemoveAll();		// CLEAR WORD BUFFER
		fgets(buffer,MAX_STRINGLENGTH,fp);
		ParseString(buffer,&words,&cnt);
		// HANDLE ANY VALID HIERARCHY NODES
		if (words.GetAt(0) != "end" && words.GetAt(0).GetAt(0) != '#')
		{
			// HANDLE THE ROOT BIT
			if (words.GetAt(0) == "root")
			{
				root->childCnt = words.GetSize() - 1;
				root->children = (t_Bone *)malloc((root->childCnt) * sizeof(t_Bone));
				// GO THROUGH THE LIST OF CHILDREN
				for (loop = 0; loop < root->childCnt; loop++)
				{
					// FIND A MATCH IN MY BONELIST
					for (loop2 = 0; loop2 < bonecnt; loop2++)
					{
						if (words.GetAt(loop + 1) == bonelist[loop2].name)
						{
							// FOUND THE MATCH SO COPY IT OVER
							memcpy(&root->children[loop],&bonelist[loop2],sizeof(t_Bone));
						}
					}
				}
			}
			// ANYTHING BUT THE "ROOT" ONE
			else
			{
				strcpy(name,words.GetAt(0));	// NAME OF PARENT
				tempBone = FindBoneParent(root,name);	// GET THE POINTER TO THE PARENT
				if (tempBone != NULL)
				{
					tempBone->childCnt = words.GetSize() - 1;
					tempBone->children = (t_Bone *)malloc((tempBone->childCnt) * sizeof(t_Bone));
					// GO THROUGH THE LIST OF CHILDREN
					for (loop = 0; loop < tempBone->childCnt; loop++)
					{
						// FIND A MATCH IN MY BONELIST
						for (loop2 = 0; loop2 < bonecnt; loop2++)
						{
							if (words.GetAt(loop + 1) == bonelist[loop2].name)
							{
								// FOUND THE MATCH SO COPY IT OVER
								memcpy(&tempBone->children[loop],&bonelist[loop2],sizeof(t_Bone));
								// SET THE TRANSLATION FOR THIS BONE NOW BASED ON THE PARENT LENGTH
								tempBone->children[loop].b_trans.x = tempBone->length.x;
								tempBone->children[loop].b_trans.y = tempBone->length.y;
								tempBone->children[loop].b_trans.z = tempBone->length.z;
								memcpy(&tempBone->children[loop].trans,&tempBone->children[loop].b_trans,sizeof(tVector));
							}
						}
					}
				}
			}
		}
	}
	// I AM NOT HANDLING THE "LIMITS" OR "AXIS" BLOCKS
	// KEEP PARSING UNTIL WE HIT AN END BLOCK
	while ((words.GetAt(0) != "end") && !feof(fp));
	// FREE THE BONELIST
	free(bonelist);
}
//// HandleAcclaimHierarchy ///////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Function:	LoadAcclaim
// Purpose:		Actually load a Acclaim file into the system
// Arguments:	Name of the file to open and root skeleton to put it in
///////////////////////////////////////////////////////////////////////////////
BOOL LoadAcclaim(CString name,t_Bone *root)
{
/// Local Variables ///////////////////////////////////////////////////////////
	int cnt;
	FILE *fp;		// I PREFER THIS STYLE OF FILE ACCESS
	char buffer[MAX_STRINGLENGTH];
	CStringArray words;
	BOOL grabNew = TRUE;		// DO I NEED A NEW LINE?
///////////////////////////////////////////////////////////////////////////////
	// OPEN THE ACCLAIM ASF PORTION OF THE FILE
	// THIS PART HOLDS THE DEFINITIONS FOR ALL THE BONES AS WELL AS THE
	// HIERARCHY INFO
	if (fp = fopen((LPCTSTR)name,"r")) {
		while (!feof(fp))
		{
			if (grabNew)
			{
				words.RemoveAll();		// CLEAR WORD BUFFER
				fgets(buffer,MAX_STRINGLENGTH,fp);
				ParseString(buffer,&words,&cnt);
			}
			grabNew = TRUE;	// MAKE SURE I DO THIS TIME
			// SEE IF WE CAN FIND THE NAME KEYWORD
			if (words.GetAt(0) == ":name")
			{
				// STORE OFF THE NAME
				strcpy(root->name,words.GetAt(1));
			}
			// I AM IGNORING UNITS KEYWORD
			else if (words.GetAt(0) == ":units")
			{
				// GO THROUGH THE UNITS ITEMS
				do
				{
					words.RemoveAll();		// CLEAR WORD BUFFER
					fgets(buffer,MAX_STRINGLENGTH,fp);
					ParseString(buffer,&words,&cnt);
				}
				while ((words.GetAt(0).GetAt(0) != ':') && !feof(fp));
				grabNew = FALSE;	// DON'T REGRAB A MESSAGE
			}
			// I AM IGNORING DOCUMENTATION BLOCK
			else if (words.GetAt(0) == ":documentation")
			{
				// GO THROUGH THE DOCUMENTATION ITEMS
				do
				{
					words.RemoveAll();		// CLEAR WORD BUFFER
					fgets(buffer,MAX_STRINGLENGTH,fp);
					ParseString(buffer,&words,&cnt);
				}
				while ((words.GetAt(0).GetAt(0) != ':') && !feof(fp));
				grabNew = FALSE;	// DON'T REGRAB A MESSAGE
			}
			// HANDLE THE ROOT BLOCK
			// SINCE I WANT IT IN THE MIDDLE OF THE DISPLAY AND
			// INITIALIZED LIKE I HAVE IT, I WILL IGNORE
			else if (words.GetAt(0) == ":root")
			{
				// SHOULD JUST BE TWO LINES BUT...
				do
				{
					words.RemoveAll();		// CLEAR WORD BUFFER
					fgets(buffer,MAX_STRINGLENGTH,fp);
					ParseString(buffer,&words,&cnt);
				}
				while ((words.GetAt(0).GetAt(0) != ':') && !feof(fp));
				grabNew = FALSE;	// DON'T REGRAB A MESSAGE
			}
			// HANDLE THE BONEDATA BLOCK
			// THIS IS THE MEAT OF THE SKELETAL LOAD
			else if (words.GetAt(0) == ":bonedata")
			{
				do
				{
					words.RemoveAll();		// CLEAR WORD BUFFER
					fgets(buffer,MAX_STRINGLENGTH,fp);
					ParseString(buffer,&words,&cnt);
					if (words.GetAt(0) == "begin")
					{
						// FOUND A BONE BLOCK, GO OFF AND HANDLE IT
						HandleAcclaimBone(fp,root);
					}
				}
				while ((words.GetAt(0).GetAt(0) != ':') && !feof(fp));
				grabNew = FALSE;	// DON'T REGRAB A MESSAGE
			}
			// HANDLE THE HIERARCHY BLOCK
			// CONTROLS THE PARENT STRUCTURE OF THE SKELETON
			else if (words.GetAt(0) == ":hierarchy")
			{
				do
				{
					words.RemoveAll();		// CLEAR WORD BUFFER
					fgets(buffer,MAX_STRINGLENGTH,fp);
					ParseString(buffer,&words,&cnt);
					if (words.GetAt(0) == "begin" && !feof(fp))
					{
						// FOUND A BONE BLOCK, GO OFF AND HANDLE IT
						HandleAcclaimHierarchy(fp,root);
					}
				}
				while ((words.GetAt(0).GetAt(0) != ':') && !feof(fp));
				grabNew = FALSE;	// DON'T REGRAB A MESSAGE
			}
		}
		fclose(fp);
		return TRUE;
	}
	return FALSE;
}
//// LoadAcclaim //////////////////////////////////////////////////////////////
