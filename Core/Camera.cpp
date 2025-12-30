#include "Camera.h"

Camera::Camera(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up, ProjectionInfo& projectionInfo)
{
	m_cameraPosition = position;
	m_up = up;

	windowHeight = projectionInfo.height;
	windowWidth = projectionInfo.width;

	float aspectRatio = (float)projectionInfo.width / (float)projectionInfo.height;

	m_cameraOrientation = glm::lookAt(position, target, up);
	m_projection = glm::perspective(projectionInfo.fov, aspectRatio, projectionInfo.zNear, projectionInfo.zFar);
}

void Camera::Update(float dt)
{
	if (m_mouseMovement.buttonClicked)
	{
		CalculateMousePos();
	}

	m_oldMousePos = m_mouseMovement.pos;

	CalculateVelocity(dt);

	m_cameraPosition += m_velocity * dt;
}

bool Camera::KeyPressedHandler(CameraMovement& movement, int key, int scanCode, int action, int mods)
{
	bool pressed = action != GLFW_RELEASE;

	bool handled = true;

	switch (key)
	{
	case GLFW_KEY_W:
		movement.forward = pressed;
		break;
	case GLFW_KEY_S:
		movement.backward = pressed;
		break;
	case GLFW_KEY_A:
		movement.left = pressed;
		break;
	case GLFW_KEY_D:
		movement.right = pressed;
		break;
	default:
		handled = false;
		break;
	}

	return handled;
}

void Camera::MouseMovedHandler(int xPos, int yPos)
{
	m_mouseMovement.pos.x = (float)xPos / (float)windowWidth;
	m_mouseMovement.pos.y = (float)yPos / (float)windowHeight;
}

void Camera::MouseButtonHandler(int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		bool pressed = action == GLFW_PRESS;
		m_mouseMovement.buttonClicked = pressed;
	}
}

glm::mat4 Camera::GetViewMatrix() const
{
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), -m_cameraPosition);
	glm::mat4 rotate = glm::mat4_cast(m_cameraOrientation);

	glm::mat4 res = rotate * translate;
	return res;
}

glm::mat4 Camera::GetVPMatrix() const
{
	glm::mat4 view = GetViewMatrix();

	glm::mat4 res = m_projection * view;
	return res;
}

void Camera::CalculateVelocity(float dt)
{
	glm::vec3 acceleration = CalculateAcceleration();

	if (acceleration == glm::vec3(0.0f))
	{
		m_velocity -= m_velocity * std::min(m_damping * dt, 1.0f);
	}
	else
	{
		m_velocity += acceleration * m_acceleration * dt;
		float maxSpeed = m_cameraMovement.slowCam ? m_slowSpeed : m_maxSpeed;

		if (glm::length(m_velocity) > maxSpeed)
		{
			m_velocity = glm::normalize(m_velocity) * maxSpeed;
		}
	}
}

glm::vec3 Camera::CalculateAcceleration()
{
	glm::mat4 v = glm::mat4_cast(m_cameraOrientation);

	glm::vec3 Forward, Up, Right;

	Right = glm::vec3(v[0][0], v[1][0], v[2][0]);
	Forward = -glm::vec3(v[0][2], v[1][2], v[2][2]);
	Up = glm::cross(Forward, Right);

	glm::vec3 acceleration = glm::vec3(0.0f);

	if (m_cameraMovement.forward)
	{
		acceleration += Forward;
	}
	if (m_cameraMovement.backward)
	{
		acceleration -= Forward;
	}
	if (m_cameraMovement.right)
	{
		acceleration += Right;
	}
	if (m_cameraMovement.left)
	{
		acceleration -= Right;
	}
	if (m_cameraMovement.up)
	{
		acceleration += Up;
	}
	if (m_cameraMovement.down)
	{
		acceleration -= Up;
	}

	return acceleration;
}

void Camera::CalculateMousePos()
{
	glm::vec2 delta = m_mouseMovement.pos - m_oldMousePos;

	glm::quat deltaQuat = glm::quat(glm::vec3(m_mouseSpeed * delta.y, m_mouseSpeed * delta.x, 0.0f));

	m_cameraOrientation = glm::normalize(deltaQuat * m_cameraOrientation);

	SetUpOrientation();
}

void Camera::SetUpOrientation()
{
	glm::mat4 view = GetViewMatrix();

	glm::vec3 forward = glm::vec3(view[0][2], view[1][2], view[2][2]);

	m_cameraOrientation = glm::lookAt(m_cameraPosition, m_cameraPosition - forward, m_up);
}
