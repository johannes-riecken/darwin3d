///////////////////////////////////////////////////////////////////////////////
// Procedure:	CheckDOFRestrictions
// Purpose:		Make sure link is within valid DOF
// Arguments:	Link
///////////////////////////////////////////////////////////////////////////////
void COGLView::CheckDOFRestrictions(t_Bone *link)
{
/// Local Variables ///////////////////////////////////////////////////////////
	tVector		euler;		// PLACE TO STORE EULER ANGLES
///////////////////////////////////////////////////////////////////////////////

	// FIRST STEP IS TO CONVERT LINK QUATERNION BACK TO EULER ANGLES
	QuatToEuler(&link->quat, &euler);

	// CHECK THE DOF SETTINGS
	if (euler.x > (float)link->max_rx)
		euler.x = (float)link->max_rx;
	if (euler.x < (float)link->min_rx)
		euler.x = (float)link->min_rx;
	if (euler.y > (float)link->max_ry)
		euler.y = (float)link->max_ry;
	if (euler.y < (float)link->min_ry)
		euler.y = (float)link->min_ry;
	if (euler.z > (float)link->max_rz)
		euler.z = (float)link->max_rz;
	if (euler.z < (float)link->min_rz)
		euler.z = (float)link->min_rz;

	// BACK TO QUATERNION
	EulerToQuaternion(&euler, &link->quat);
}

///////////////////////////////////////////////////////////////////////////////
// Procedure:	ComputeCCDLink
// Purpose:		Compute an IK Solution to an end effector position in 3D
// Arguments:	End Target (x,y,z)
// Returns:		TRUE if a solution exists, FALSE if the position isn't in reach
///////////////////////////////////////////////////////////////////////////////
BOOL COGLView::ComputeCCDLink(tVector endPos)
{
/// Local Variables ///////////////////////////////////////////////////////////
	tVector		rootPos,curEnd,desiredEnd,targetVector,curVector,crossResult;
	double		cosAngle,turnAngle,turnDeg;
	int			link,tries;
	tQuaternion aquat;
///////////////////////////////////////////////////////////////////////////////
	// START AT THE LAST LINK IN THE CHAIN
	link = EFFECTOR_POS - 1;
	tries = 0;						// LOOP COUNTER SO I KNOW WHEN TO QUIT
	do
	{
		// THE COORDS OF THE X,Y,Z POSITION OF THE ROOT OF THIS BONE IS IN THE MATRIX
		// TRANSLATION PART WHICH IS IN THE 12,13,14 POSITION OF THE MATRIX
		rootPos.x = m_Link[link].matrix.m[12];
		rootPos.y = m_Link[link].matrix.m[13];
		rootPos.z = m_Link[link].matrix.m[14];

		// POSITION OF THE END EFFECTOR
		curEnd.x = m_Link[EFFECTOR_POS].matrix.m[12];
		curEnd.y = m_Link[EFFECTOR_POS].matrix.m[13];
		curEnd.z = m_Link[EFFECTOR_POS].matrix.m[14];

		// DESIRED END EFFECTOR POSITION
		desiredEnd.x = (float)endPos.x;
		desiredEnd.y = (float)endPos.y;
		desiredEnd.z = (float)endPos.z;

		// SEE IF I AM ALREADY CLOSE ENOUGH
		if (VectorSquaredDistance(&curEnd, &desiredEnd) > IK_POS_THRESH)
		{
			// CREATE THE VECTOR TO THE CURRENT EFFECTOR POS
			curVector.x = curEnd.x - rootPos.x;
			curVector.y = curEnd.y - rootPos.y;
			curVector.z = curEnd.z - rootPos.z;
			// CREATE THE DESIRED EFFECTOR POSITION VECTOR
			targetVector.x = endPos.x - rootPos.x;
			targetVector.y = endPos.y - rootPos.y;
			targetVector.z = endPos.z - rootPos.z;

			// NORMALIZE THE VECTORS (EXPENSIVE, REQUIRES A SQRT)
			NormalizeVector(&curVector);
			NormalizeVector(&targetVector);

			// THE DOT PRODUCT GIVES ME THE COSINE OF THE DESIRED ANGLE
			cosAngle = DotProduct(&targetVector,&curVector);

			// IF THE DOT PRODUCT RETURNS 1.0, I DON'T NEED TO ROTATE AS IT IS 0 DEGREES
			if (cosAngle < 0.99999)
			{
				// USE THE CROSS PRODUCT TO CHECK WHICH WAY TO ROTATE
				CrossProduct(&curVector, &targetVector, &crossResult);
				NormalizeVector(&crossResult);
				turnAngle = acos((float)cosAngle);	// GET THE ANGLE
				turnDeg = RADTODEG(turnAngle);		// COVERT TO DEGREES
				// DAMPING
				if (m_Damping && turnDeg > m_Link[link].damp_width)
					turnDeg = m_Link[link].damp_width;
				AxisAngleToQuat(&crossResult,turnDeg,&aquat);
				MultQuaternions(&m_Link[link].quat, &aquat,&m_Link[link].quat);
				// HANDLE THE DOF RESTRICTIONS IF I WANT THEM
				if (m_DOF_Restrict)
					CheckDOFRestrictions(&m_Link[link]);
				// RECALC ALL THE MATRICES WITHOUT DRAWING ANYTHING
				drawScene(FALSE);		// CHANGE THIS TO TRUE IF YOU WANT TO SEE THE ITERATION
			}
			if (--link < 0) link = EFFECTOR_POS - 1;	// START OF THE CHAIN, RESTART
		}
	// QUIT IF I AM CLOSE ENOUGH OR BEEN RUNNING LONG ENOUGH
	} while (tries++ < MAX_IK_TRIES &&
				VectorSquaredDistance(&curEnd, &desiredEnd) > IK_POS_THRESH);
	if (tries == MAX_IK_TRIES)
		return FALSE;
	else
		return TRUE;
}

