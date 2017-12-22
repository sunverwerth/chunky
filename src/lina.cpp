#include "lina.h"

float operator*(const Vector2& a, const Vector2& b) {
	return a.x*b.x + a.y*b.y;
}

Vector2 operator+(const Vector2& a, const Vector2& b) {
	return Vector2(a.x + b.x, a.y + b.y);
}

Vector2 operator-(const Vector2& a, const Vector2& b) {
	return Vector2(a.x - b.x, a.y - b.y);
}

Vector2 operator*(const Vector2& a, float b) {
	return Vector2(a.x *b, a.y*b);
}

Vector2 operator/(const Vector2& a, float b) {
	return Vector2(a.x / b, a.y / b);
}

Vector2 operator-(const Vector2& a) {
	return Vector2(-a.x, -a.y);
}

Vector2 Vector2::zero(0.0f, 0.0f);




float operator*(const Vector3& a, const Vector3& b) {
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

Vector3 operator+(const Vector3& a, const Vector3& b) {
	return Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vector3 operator-(const Vector3& a, const Vector3& b) {
	return Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vector3 operator*(const Vector3& a, float b) {
	return Vector3(a.x * b, a.y * b, a.z * b);
}

Vector3 operator*(float b, const Vector3& a) {
	return Vector3(a.x * b, a.y * b, a.z * b);
}

Vector3 operator/(const Vector3& a, float b) {
	return Vector3(a.x / b, a.y / b, a.z / b);
}

Vector3 operator-(const Vector3& a) {
	return Vector3(-a.x, -a.y, -a.z);
}

Vector3 cross(const Vector3& a, const Vector3& b) {
	return Vector3(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	);
}

Vector3 Vector3::zero(0.0f, 0.0f, 0.0f);
Vector3 Vector3::up(0.0f, 1.0f, 0.0f);
Vector3 Vector3::down(0.0f, -1.0f, 0.0f);
Vector3 Vector3::left(-1.0f, 0.0f, 0.0f);
Vector3 Vector3::right(1.0f, 0.0f, 0.0f);
Vector3 Vector3::forward(0.0f, 0.0f, -1.0f);
Vector3 Vector3::backward(0.0f, 0.0f, 1.0f);

Vector4 Vector4::zero(0.0f, 0.0f, 0.0f, 0.0f);
Vector4 Vector4::white(1.0f, 1.0f, 1.0f, 1.0f);
Vector4 Vector4::red(1.0f, 0.0f, 0.0f, 1.0f);
Vector4 Vector4::green(0.0f, 1.0f, 0.0f, 1.0f);
Vector4 Vector4::blue(0.0f, 0.0f, 1.0f, 1.0f);
Vector4 Vector4::black(0.0f, 0.0f, 0.0f, 1.0f);

Vector4 operator*(const Vector4& v, float f) {
	return Vector4(v.x*f, v.y*f, v.z*f, v.w*f);
}

Vector4 operator+(const Vector4& a, const Vector4& b) {
	return Vector4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

Vector4 operator*(const Vector4& a, const Vector4& b) {
	return Vector4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}


Matrix3x3::Matrix3x3(const Quaternion& q) : m{
	1 - 2 * q.y*q.y - 2 * q.z*q.z,	2 * q.x*q.y - 2 * q.z*q.w,	2 * q.x*q.z + 2 * q.y*q.w,
	2 * q.x*q.y + 2 * q.z*q.w,	1 - 2 * q.x*q.x - 2 * q.z*q.z,	2 * q.y*q.z - 2 * q.x*q.w,
	2 * q.x*q.z - 2 * q.y*q.w,	2 * q.y*q.z + 2 * q.x*q.w,	1 - 2 * q.x*q.x - 2 * q.y*q.y,
} {}

Matrix3x3 operator*(const Matrix3x3& a, const Matrix3x3& b) {
	Matrix3x3 m;
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			float v = 0;
			for (int x = 0; x < 3; ++x) {
				v += a.m[i][x] * b.m[x][j];
			}
			m.m[i][j] = v;
		}
	}
	return m;
}

Matrix3x3 Matrix3x3::identity(
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f
);






/*Matrix4x4::Matrix4x4(const Quaternion& q) : m{
	1.0f - 2.0f * q.y*q.y - 2.0f * q.z*q.z,	2.0f * q.x*q.y - 2.0f * q.z*q.w,	    2.0f * q.x*q.z + 2.0f * q.y*q.w, 0.0f,
	2.0f * q.x*q.y + 2.0f * q.z*q.w,	    1.0f - 2.0f * q.x*q.x - 2.0f * q.z*q.z,	2.0f * q.y*q.z - 2.0f * q.x*q.w, 0.0f,
	2.0f * q.x*q.z - 2.0f * q.y*q.w,	    2.0f * q.y*q.z + 2.0f * q.x*q.w,        1.0f - 2.0f * q.x*q.x - 2.0f * q.y*q.y, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f,
} {}*/

Matrix4x4::Matrix4x4(const Quaternion& q) : m{
	1.0f - 2.0f * q.y*q.y - 2.0f * q.z*q.z,	2.0f * q.x*q.y + 2.0f * q.z*q.w,	    2.0f * q.x*q.z - 2.0f * q.y*q.w, 0.0f,
	2.0f * q.x*q.y - 2.0f * q.z*q.w,	    1.0f - 2.0f * q.x*q.x - 2.0f * q.z*q.z,	2.0f * q.y*q.z + 2.0f * q.x*q.w, 0.0f,
	2.0f * q.x*q.z + 2.0f * q.y*q.w,	    2.0f * q.y*q.z - 2.0f * q.x*q.w,        1.0f - 2.0f * q.x*q.x - 2.0f * q.y*q.y, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f,
} {}

Matrix4x4 operator*(const Matrix4x4& a, const Matrix4x4& b) {
	Matrix4x4 m;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			float v = 
				a.m[i][0] * b.m[0][j] + 
				a.m[i][1] * b.m[1][j] +
				a.m[i][2] * b.m[2][j] +
				a.m[i][3] * b.m[3][j];
			m.m[i][j] = v;
		}
	}
	return m;
}

Matrix4x4 matrixPerspective(float fov, float aspect, float near, float far) {
	float tanHalfFovy = std::tanf(fov / 2.0f);

	return Matrix4x4(
		1.0f / (aspect * tanHalfFovy), 0, 0, 0,
		0, 1.0f / tanHalfFovy, 0, 0,
		0, 0, -(far + near) / (far - near), -1,
		0, 0, -(2.0f*far*near) / (far - near), 0
	);
}

Matrix4x4 matrixOrthographic(float width, float height, float near, float far) {
	float d = far - near;
	return Matrix4x4(
		2.0f / width, 0, 0, 0,
		0, 2.0f / height, 0, 0,
		0, 0, -2.0f / d, -(near + far) / d,
		0, 0, 0, 1
	);
}

Matrix4x4 matrixTranslation(const Vector3& pos) {
	return Matrix4x4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		pos.x, pos.y, pos.z, 1.0f
	);
}

Matrix4x4 Matrix4x4::identity(
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
);

void Quaternion::normalize() {
	float im = 1.0f / magnitude();
	w *= im;
	x *= im;
	y *= im;
	z *= im;
}

float Quaternion::magnitude() const {
	return std::sqrtf(w*w + x*x + y*y + z*z);
}

Quaternion Quaternion::inverse() const {
	float iq = 1.0f / (w*w + x*x + y*y + z*z);
	return Quaternion(w * iq, -x * iq, -y * iq, -z * iq);
}

void Quaternion::invert() {
	float iq = 1.0f / (w*w + x*x + y*y + z*z);
	w = w * iq;
	x = -x * iq;
	y = -y * iq;
	z = -z * iq;
}

Quaternion Quaternion::conjugate() const {
	return Quaternion(w, -x, -y, -z);
}

Quaternion Quaternion::axisAngle(const Vector3& axis, float angle) {
	float s = std::sinf(angle / 2);
	return Quaternion{
		std::cosf(angle / 2),
		axis.x * s,
		axis.y * s,
		axis.z * s
	};
}

Quaternion operator*(const Quaternion& q, const Quaternion& r) {
	return Quaternion{
		r.w*q.w - r.x*q.x - r.y*q.y - r.z*q.z,
		r.w*q.x + r.x*q.w - r.y*q.z + r.z*q.y,
		r.w*q.y + r.x*q.z + r.y*q.w - r.z*q.x,
		r.w*q.z - r.x*q.y + r.y*q.x + r.z*q.w
	};
}

Vector3 operator*(const Quaternion& q, const Vector3& v) {
	auto qv = Vector3(q.x, q.y, q.z);
	auto t = cross(qv, v) * 2;
	return v + t*q.w + cross(qv, t);
}

Quaternion Quaternion::identity{ 1.0f, 0.0f, 0.0f, 0.0f };
