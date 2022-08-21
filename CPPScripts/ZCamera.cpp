#include "ZCamera.h"

namespace ZXEngine
{
	Camera::Camera(vec3 position, vec3 up, float yaw, float pitch) : Front(vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Fov(FOV)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		UpdateCameraVectors();
	}

	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	mat4 Camera::GetViewMatrix()
	{
		// return glm::lookAt(Position, Position + Front, Up);

		// ���³����Լ�ʵ��LookAt����
		// ����GLM��������Ϊ����ģ�ƽʱ�����ϵľ���������Ϊ����ģ��������￴��������ת�ù���һ������ʵ���viewMat������Ӧ���������ģ�
		//  Right.x,  Right.y,  Right.z, 0,
		//  Up.x,     Up.y,     Up.z,    0,
		// -Front.x, -Front.y, -Front.z, 0,
		//  0,        0,        0,       1
		// �����posMatͬ��
		mat4 viewMat = mat4(
			Right.x, Up.x, -Front.x, 0,
			Right.y, Up.y, -Front.y, 0,
			Right.z, Up.z, -Front.z, 0,
			0, 0, 0, 1);
		mat4 posMat = mat4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			-Position.x, -Position.y, -Position.z, 1);

		return viewMat * posMat;
	}

	mat4 Camera::GetProjectionMatrix()
	{
		return perspective(radians(Fov), (float)RenderEngine::scrWidth / (float)RenderEngine::scrHeight, 0.1f, 100.0f);
	}

	// ����ˮƽ����ֱƫ������������ӽ�
	void Camera::RotateAngleOfView(float horizontalOffset, float verticalOffset, bool constrainPitch = true)
	{
		horizontalOffset *= MouseSensitivity;
		verticalOffset *= MouseSensitivity;

		Yaw += horizontalOffset;
		Pitch += verticalOffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		// Update Front, Right and Up Vectors using the updated Euler angles
		UpdateCameraVectors();
	}

	void Camera::Render(list<GameObject*> gameObjects)
	{
		for (auto go : gameObjects)
		{
			MeshRenderer* meshRenderer = go->GetComponent<MeshRenderer>("MeshRenderer");
		}

	}

	// Calculates the front vector from the Camera's (updated) Euler Angles
	void Camera::UpdateCameraVectors()
	{
		// Calculate the new Front vector
		vec3 front;
		front.x = cos(radians(Yaw)) * cos(radians(Pitch));
		front.y = sin(radians(Pitch));
		front.z = sin(radians(Yaw)) * cos(radians(Pitch));
		Front = normalize(front);
		// Also re-calculate the Right and Up vector
		// Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Right = normalize(cross(Front, WorldUp));
		Up = normalize(cross(Right, Front));
	}
}