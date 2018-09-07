#ifndef OSS_VECTOR_H
#define OSS_VECTOR_H

struct OSS_Vector
{
    float x;
    float y;
};

/// Basic vector arithmetic
OSS_Vector operator+(OSS_Vector vec_a, OSS_Vector vec_b);

void operator+=(OSS_Vector &vec_a, OSS_Vector vec_b);

OSS_Vector operator-(OSS_Vector vec_a, OSS_Vector vec_b);

void operator-=(OSS_Vector &vec_a, OSS_Vector vec_b);

OSS_Vector operator*(OSS_Vector vec_a, OSS_Vector vec_b);

/// Scalar multiplication
OSS_Vector operator*(OSS_Vector vec, float scalar);
OSS_Vector operator*(float scalar, OSS_Vector vec);

/// Dot product
float OSS_CalcDotProduct(OSS_Vector vec_a, OSS_Vector vec_b);

/// Magnitude of vector squared
float OSS_CalcMagnitudeSquared(OSS_Vector vec);

/// Magnitude of vector
float OSS_CalcMagnitude(OSS_Vector vec);

/// Normalised (unit) vector
OSS_Vector OSS_CalcUnitVector(OSS_Vector vec);

/// Vector projection of vec_a onto vec_b
OSS_Vector OSS_CalcProjectVector(OSS_Vector vec_a, OSS_Vector vec_b);

/// Calculate distance squared between two point vectors
float OSS_CalcDistanceSquared(OSS_Vector vec_a, OSS_Vector vec_b);

/// Calculate distance between two point vectors
float OSS_CalcDistance(OSS_Vector vec_a, OSS_Vector vec_b);

/// Calculate reflection of vector
OSS_Vector OSS_CalcReflectionVector(OSS_Vector vec, OSS_Vector normal);

/// Rotate a vector 90 degrees clockwise (anti-clockwise in SDL coordinate system)...
OSS_Vector OSS_CalcRotation90Clockwise(OSS_Vector vec);

void OSS_CalcRotation90Clockwise(OSS_Vector &vec);

#endif // OSS_VECTOR_H
