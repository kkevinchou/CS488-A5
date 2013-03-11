#include "a2.hpp"

// By Chi Ming Chou
// Username: cm2chou
// Userid: 20309042

// Return a matrix to represent a counterclockwise rotation of "angle"
// degrees around the axis "axis", where "axis" is one of the
// characters 'x', 'y', or 'z'.
Matrix4x4 rotation(double angle, char axis)
{
	Vector4D row1;
	Vector4D row2;
	Vector4D row3;
	Vector4D row4;

  angle = angle / 180.0 * M_PI;

  switch (axis) {
  	case 'x':
  		row1 = Vector4D(1, 0, 0, 0);
  		row2 = Vector4D(0, cos(angle), -sin(angle), 0);
  		row3 = Vector4D(0, sin(angle), cos(angle), 0);
  		row4 = Vector4D(0, 0, 0, 1);
  	break;
  	case 'y':
  		row1 = Vector4D(cos(angle), 0, sin(angle), 0);
  		row2 = Vector4D(0, 1, 0, 0);
  		row3 = Vector4D(-sin(angle), 0, cos(angle), 0);
  		row4 = Vector4D(0, 0, 0, 1);
  	break;
  	case 'z':
  		row1 = Vector4D(cos(angle), -sin(angle), 0, 0);
  		row2 = Vector4D(sin(angle), cos(angle), 0, 0);
  		row3 = Vector4D(0, 0, 1, 0);
  		row4 = Vector4D(0, 0, 0, 1);
  	break;
  	default:
  		cerr << "Error: Invalid axis passed into rotation(angle, axis)" << endl;
  		exit(1);
  }

  Matrix4x4 r(row1, row2, row3, row4);

  return r;
}

Matrix4x4 axisRotation(const Vector3D& axis) {
    double radians, fInvLength, fNewVecX, fNewVecY, fNewVecZ;
    Matrix4x4 result;

    /* Find the length of the vector which is the angle of rotation
     * (in radians)
     */
    radians = sqrt(axis[0] * axis[0] + axis[1] * axis[1] + axis[2] * axis[2]);

    /* If the vector has zero length - return the identity matrix */
    if (radians > -0.000001 && radians < 0.000001) {
        return result;
    }

    /* Normalize the rotation vector now in preparation for making
     * rotation matrix.
     */
    fInvLength = 1 / radians;
    fNewVecX   = axis[0] * fInvLength;
    fNewVecY   = axis[1] * fInvLength;
    fNewVecZ   = axis[2] * fInvLength;

    /* Create the arbitrary axis rotation matrix */
    double dSinAlpha = sin(radians);
    double dCosAlpha = cos(radians);
    double dT = 1 - dCosAlpha;

    result[0][0] = dCosAlpha + fNewVecX*fNewVecX*dT;
    result[0][1] = fNewVecX*fNewVecY*dT - dSinAlpha*fNewVecZ;
    result[0][2] = fNewVecZ*fNewVecX*dT + dSinAlpha*fNewVecY;
    result[0][3] = 0;

    result[1][0] = fNewVecX*fNewVecY*dT + fNewVecZ*dSinAlpha;
    result[1][1] = dCosAlpha + fNewVecY*fNewVecY*dT;
    result[1][2] = fNewVecZ*fNewVecY*dT - dSinAlpha*fNewVecX;
    result[1][3] = 0;

    result[2][0] = fNewVecX*fNewVecZ*dT - fNewVecY*dSinAlpha;
    result[2][1] = fNewVecY*fNewVecZ*dT + dSinAlpha*fNewVecX;
    result[2][2] = dCosAlpha + fNewVecZ*fNewVecZ*dT;
    result[2][3] = 0;

    result[3][0] = 0;
    result[3][1] = 0;
    result[3][2] = 0;
    result[3][3] = 1;

    return result;
}

// Return a matrix to represent a displacement of the given vector.
Matrix4x4 translation(const Vector3D& displacement)
{

  Vector4D row1(1, 0, 0, displacement[0]);
  Vector4D row2(0, 1, 0, displacement[1]);
  Vector4D row3(0, 0, 1, displacement[2]);
  Vector4D row4(0, 0, 0, 1);

  Matrix4x4 t(row1, row2, row3, row4);

  return t;
}

// Return a matrix to represent a nonuniform scale with the given factors.
Matrix4x4 scaling(const Vector3D& scale)
{
	Vector4D row1(scale[0], 0, 0, 0);
  Vector4D row2(0, scale[1], 0, 0);
  Vector4D row3(0, 0, scale[2], 0);
  Vector4D row4(0, 0, 0, 1);

  Matrix4x4 s(row1, row2, row3, row4);

  return s;
}

Matrix4x4 columnOrderedMatrix(const Matrix4x4& matrix) {
    double vals[16];

    int index = 0;
    for (int i = 0; i < 4; i++) {
        Vector4D col = matrix.getColumn(i);
        for (int j = 0; j < 4; j++) {
            vals[index++] = col[j];
        }
    }

    return Matrix4x4(vals);
}