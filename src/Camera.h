#ifndef Camera_h
#define Camera_h

#include "lina.h"

class Camera {
public:
	Vector3 position;
	Quaternion rotation;
	float fov;
	float zNear;
	float zFar;
	bool isOrthographic;
	float width;
	float height;
	float yaw = 0;
	float pitch = 0;

	Vector3 front() {
		return rotation * Vector3::forward;
	}

	Vector3 right() {
		return rotation * Vector3::right;
	}

	Matrix4x4 getProjectionMatrix() {
		if (fov > 3.14f - 0.1f) fov = 3.14f - 0.1f;
		else if (fov < 0.1f) fov = 0.1f;

		if (isOrthographic) {
			return matrixOrthographic(width, height, zNear, zFar);
		}
		else {
			return matrixPerspective(fov, width / height, zNear, zFar);
		}
	}

	Matrix4x4 getViewMatrix() {
		if (pitch > 3.14f / 2) pitch = 3.14f / 2;
		else if (pitch < -3.14f / 2) pitch = -3.14f / 2;
		rotation = Quaternion::axisAngle(Vector3::up, yaw) * Quaternion::axisAngle(Vector3::right, pitch);
		return matrixTranslation(-position) * rotation.inverse();
	}
};

#endif
