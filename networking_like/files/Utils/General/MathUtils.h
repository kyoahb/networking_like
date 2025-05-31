#pragma once


#include "Utils/Imports/raylib_wrapper.h"

class MathUtils {
public:
	/**
	* Returns the angle in radians
	* @param degrees Angle in degrees
	*
	* @return float Angle in radians
	*/
	static float radians(float degrees) {
		return degrees * PI / 180.0f;
	};

	/**
	* Clamps a value between a minimum and maximum
	* @param a Value to clamp
	* @param min Minimum value
	* @param max Maximum value
	*
	* @return float The clamped value
	*/
	static float clamp(float a, float min, float max) {
		if (a < min) return min;
		if (a > max) return max;
		return a;
	};

	/**
	* Clamps a vector between a minimum and maximum
	* @param a Vector to clamp
	* @param min Minimum value
	* @param max Maximum value
	*
	* @return Vector3 The clamped vector
	*/
	static raylib::Vector3 clamp_vec3(raylib::Vector3 a, float min, float max) {
		return raylib::Vector3{ clamp(a.x, min, max), clamp(a.y, min, max), clamp(a.z, min, max) };
	};

	/**
	* Overfills (loops around) a value about a clamp (eg 540 overfilled between (0, 360) becomes 180)
	* @param a Value to overfill
	* @param min Minimum value
	* @param max Maximum value
	*
	* @return float The overfilled value
	*/
	static float overfill(float a, float min, float max) {
		float range = max - min;
		return fmodf((fmodf(a - min, range) + range), range) + min;
	};

	/**
	* Overfills a vector about a clamp (eg 540 overfilled between (0, 360) becomes 180)
	* @param a Vector to overfill
	* @param min Minimum value
	* @param max Maximum value
	*
	* @return Vector3 The overfilled vector
	*/
	static raylib::Vector3 overfill_vec3(raylib::Vector3 a, float min, float max) {
		return raylib::Vector3{ overfill(a.x, min, max), overfill(a.y, min, max), overfill(a.z, min, max) };
	};

	/**
	* Finds forward vector from rotation
	* @param rotation Rotation vector
	*
	* @return Vector3 The forward vector
	*/
	static raylib::Vector3 forward(Vector3 rotation) {
		float yawRad = radians(rotation.y);
		float pitchRad = radians(rotation.x);

		raylib::Vector3 forward = {
			sin(yawRad) * cos(pitchRad),
			-sin(pitchRad),
			-cos(yawRad) * cos(pitchRad)
		};

		return forward.Normalize();
	}

	/**
	* Calculates a right vector from rotation angles
	* @param rotation Rotation in degrees (x = pitch, y = yaw, z = roll)
	* @return Vector3 Right vector (normalized)
	*/
	static raylib::Vector3 right(const raylib::Vector3& rotation) {
		float yawRad = radians(rotation.y);

		// Right vector is perpendicular to forward in the horizontal plane
		raylib::Vector3 right = {
			sin(yawRad + PI / 2),
			0.0f,
			-cos(yawRad + PI / 2)
		};

		return right.Normalize();
	}

	/**
	* Calculates an up vector from rotation angles
	* @param rotation Rotation in degrees (x = pitch, y = yaw, z = roll)
	* @return Vector3 Up vector (normalized)
	*/
	static raylib::Vector3 up(const raylib::Vector3& rotation) {
		// Get forward and right vectors
		raylib::Vector3 f = forward(rotation);
		raylib::Vector3 r = right(rotation);

		// Cross product of right and forward gives up vector
		raylib::Vector3 up = r.CrossProduct(f);

		return up.Normalize();
	}

	/**
	* Calculate a target position from an origin and rotation
	* @param origin Origin position
	* @param rotation Rotation angles in degrees
	* @param distance Distance to target
	* @return Vector3 Target position
	*/
	static raylib::Vector3 calculate_target(const raylib::Vector3& origin, const raylib::Vector3& rotation, float distance = 1.0f) {
		raylib::Vector3 forwardVector = forward(rotation);

		// Scale the forward vector by the distance
		forwardVector *= distance;

		// Add to origin to get target position
		return origin + forwardVector;
	}


	/**
	* Convert Euler angles (0-360) to Axis-Angle representation
	* @param euler Euler angles in degrees (x = pitch, y = yaw, z = roll)
	* @return raylib::Vector4 Axis-Angle representation (x, y, z = axis, w = angle in radians)
	*/
	static raylib::Vector4 euler_to_axis_angle(const raylib::Vector3& euler) {
		float pitchRad = radians(euler.x);
		float yawRad = radians(euler.y);
		float rollRad = radians(euler.z);
		// Calculate the axis of rotation (normalized)
		raylib::Vector3 axis = {
			sin(yawRad) * cos(pitchRad),
			-sin(pitchRad),
			-cos(yawRad) * cos(pitchRad)
		};
		axis = axis.Normalize();
		// Calculate the angle of rotation
		float angle = sqrtf(pitchRad * pitchRad + yawRad * yawRad + rollRad * rollRad);
		return raylib::Vector4{ axis.x, axis.y, axis.z, angle };
	};



};