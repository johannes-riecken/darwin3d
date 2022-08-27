///////////////////////////////////////////////////////////////////////////////
// Function:	QuatToEuler
// Purpose:		Convert a Quaternion back to Euler Angles
// Arguments:	Quaternions and target Euler vector
// Notes:		The method is to convert Quaternion to a 3x3 matrix and
//				decompose the matrix.  This is subject to the
//				ambiguities of square roots and problems with inverse trig.
//				Matrix to Euler conversion is really very ill-defined but works
//				for my purposes.
///////////////////////////////////////////////////////////////////////////////
void QuatToEuler(const tQuaternion *quat, tVector *euler)
{
/// Local Variables ///////////////////////////////////////////////////////////
	float matrix[3][3];
	float cx,sx,x;
	float cy,sy,y,yr;
	float cz,sz,z;
///////////////////////////////////////////////////////////////////////////////
	// CONVERT QUATERNION TO MATRIX - I DON'T REALLY NEED ALL OF IT
	matrix[0][0] = 1.0f - (2.0f * quat->y * quat->y) - (2.0f * quat->z * quat->z);
//	matrix[0][1] = (2.0f * quat->x * quat->y) - (2.0f * quat->w * quat->z);
//	matrix[0][2] = (2.0f * quat->x * quat->z) + (2.0f * quat->w * quat->y);
	matrix[1][0] = (2.0f * quat->x * quat->y) + (2.0f * quat->w * quat->z);
//	matrix[1][1] = 1.0f - (2.0f * quat->x * quat->x) - (2.0f * quat->z * quat->z);
//	matrix[1][2] = (2.0f * quat->y * quat->z) - (2.0f * quat->w * quat->x);
	matrix[2][0] = (2.0f * quat->x * quat->z) - (2.0f * quat->w * quat->y);
	matrix[2][1] = (2.0f * quat->y * quat->z) + (2.0f * quat->w * quat->x);
	matrix[2][2] = 1.0f - (2.0f * quat->x * quat->x) - (2.0f * quat->y * quat->y);

	sy = -matrix[2][0];
	cy = sqrt(1 - (sy * sy));
	yr = (float)atan2(sy,cy);
	euler->y = (yr * 180.0f) / (float)M_PI;

	// AVOID DIVIDE BY ZERO ERROR ONLY WHERE Y= +-90 or +-270
	// NOT CHECKING cy BECAUSE OF PRECISION ERRORS
	if (sy != 1.0f && sy != -1.0f)
	{
		cx = matrix[2][2] / cy;
		sx = matrix[2][1] / cy;
		euler->x = ((float)atan2(sx,cx) * 180.0f) / (float)M_PI;	// RAD TO DEG

		cz = matrix[0][0] / cy;
		sz = matrix[1][0] / cy;
		euler->z = ((float)atan2(sz,cz) * 180.0f) / (float)M_PI;	// RAD TO DEG
	}
	else
	{
		// SINCE Cos(Y) IS 0, I AM SCREWED.  ADOPT THE STANDARD Z = 0
		// I THINK THERE IS A WAY TO FIX THIS BUT I AM NOT SURE.  EULERS SUCK
		// NEED SOME MORE OF THE MATRIX TERMS NOW
		matrix[1][1] = 1.0f - (2.0f * quat->x * quat->x) - (2.0f * quat->z * quat->z);
		matrix[1][2] = (2.0f * quat->y * quat->z) - (2.0f * quat->w * quat->x);
		cx = matrix[1][1];
		sx = -matrix[1][2];
		euler->x = ((float)atan2(sx,cx) * 180.0f) / (float)M_PI;	// RAD TO DEG

		cz = 1.0f;
		sz = 0.0f;
		euler->z = ((float)atan2(sz,cz) * 180.0f) / (float)M_PI;	// RAD TO DEG
	}
}
// QuatToEuler  ///////////////////////////////////////////////////////////////
