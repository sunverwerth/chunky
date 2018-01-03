#ifndef LINA_H
#define LINA_H

#include <math.h>

struct Vector2 {
public:
	float x, y;

	Vector2() = default;
	Vector2(float x, float y) : x(x), y(y) {}
	Vector2(const Vector2& other) : x(other.x), y(other.y) {}

	float length() { return sqrtf(lengthSq()); }
	float lengthSq() { return x*x + y*y; }
	Vector2 normalized() { auto im = 1.0f / length(); return Vector2(x*im, y*im); }
	void normalize() { auto im = 1.0f / length(); x *= im; y *= im; }

	static Vector2 zero;
};

float operator*(const Vector2& a, const Vector2& b);
Vector2 operator+(const Vector2& a, const Vector2& b);
Vector2 operator-(const Vector2& a, const Vector2& b);
Vector2 operator*(const Vector2& a, float b);
Vector2 operator/(const Vector2& a, float b);
Vector2 operator-(const Vector2& a);


struct Vector3 {
public:
	union {
		struct { float x, y, z; };
		struct { float r, g, b; };
	};

	Vector3() = default;
	Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
	Vector3(const Vector3& other) : x(other.x), y(other.y), z(other.z) {}

	float length() { return sqrtf(lengthSq()); }
	float lengthSq() { return x*x + y*y + z*z; }
	Vector3 normalized() { auto im = 1.0f / length(); return Vector3(x*im, y*im, z*im); }
	void normalize() { auto im = 1.0f / length(); x *= im; y *= im; z *= im; }

	static Vector3 zero;
	static Vector3 up;
	static Vector3 down;
	static Vector3 forward;
	static Vector3 backward;
	static Vector3 left;
	static Vector3 right;
};

float operator*(const Vector3& a, const Vector3& b);
Vector3 operator+(const Vector3& a, const Vector3& b);
Vector3 operator-(const Vector3& a, const Vector3& b);
Vector3 operator*(const Vector3& a, float b);
Vector3 operator*(float b, const Vector3& a);
Vector3 operator/(const Vector3& a, float b);
Vector3 operator-(const Vector3& a);
Vector3 cross(const Vector3& a, const Vector3& b);

struct Vector4 {
public:
	union {
		struct { float x, y, z, w; };
		struct { float r, g, b, a; };
	};

	Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	Vector4(const Vector4& other) : x(other.x), y(other.y), z(other.z), w(other.w) {}

	float length() { return sqrtf(lengthSq()); }
	float lengthSq() { return x*x + y*y + z*z + w*w; }
	Vector4 normalized() { auto im = 1.0f / length(); return Vector4(x*im, y*im, z*im, w*im); }
	void normalize() { auto im = 1.0f / length(); x *= im; y *= im; z *= im; w *= im; }

	static Vector4 zero;
	static Vector4 white;
	static Vector4 red;
	static Vector4 green;
	static Vector4 blue;
	static Vector4 black;
};

Vector4 operator*(const Vector4& v, float f);
Vector4 operator*(const Vector4& a, const Vector4& b);
Vector4 operator+(const Vector4& a, const Vector4& b);

struct Quaternion;

struct Matrix3x3 {
public:
	float m[3][3];

	Matrix3x3() = default;
	Matrix3x3(const Matrix3x3&) = default;
	Matrix3x3(float _11, float _12, float _13, float _21, float _22, float _23, float _31, float _32, float _33) : m{
		_11, _12, _13,
		_21, _22, _23,
		_31, _32, _33,
	} {}
	Matrix3x3(const Quaternion& q);

	static Matrix3x3 identity;
};

Matrix3x3 operator*(const Matrix3x3& a, const Matrix3x3& b);

struct Matrix4x4 {
public:
	float m[4][4];

	Matrix4x4() = default;
	Matrix4x4(const Matrix4x4&) = default;
	Matrix4x4(float _11, float _12, float _13, float _14, float _21, float _22, float _23, float _24, float _31, float _32, float _33, float _34, float _41, float _42, float _43, float _44) : m{
		_11, _12, _13, _14,
		_21, _22, _23, _24,
		_31, _32, _33, _34,
		_41, _42, _43, _44,
	} {}
	Matrix4x4(const Quaternion& q);

	static Matrix4x4 identity;
};

Matrix4x4 operator*(const Matrix4x4& a, const Matrix4x4& b);

Matrix4x4 matrixPerspective(float fov, float aspect, float near, float far);
Matrix4x4 matrixOrthographic(float width, float height, float near, float far);
Matrix4x4 matrixTranslation(const Vector3& pos);

struct Quaternion {
public:
	float w, x, y, z;

	Quaternion() = default;
	Quaternion(float w, float x, float y, float z) : w(w), x(x), y(y), z(z) {}

	void normalize();
	float magnitude() const;

	static Quaternion axisAngle(const Vector3& axis, float angle);
	Quaternion inverse() const;
	Quaternion conjugate() const;
	void invert();

	static Quaternion identity;
};

Quaternion operator*(const Quaternion& a, const Quaternion& b);
Vector3 operator*(const Quaternion& q, const Vector3& v);

#endif
