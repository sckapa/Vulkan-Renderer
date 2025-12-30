#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <GLFW/glfw3.h>

struct ProjectionInfo
{
	float fov;
	float width;
	float height;
	float zNear;
	float zFar;
};

struct CameraMovement
{
	bool forward = false;
	bool backward = false;
	bool right = false;
	bool left = false;
	bool up = false;
	bool down = false;
	bool slowCam = false;
};

struct MouseMovement
{
	glm::vec2 pos = glm::vec2(0.0f);
	bool buttonClicked = false;
};

class Camera
{
public:
	Camera(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up, ProjectionInfo& projectionInfo);
	
	CameraMovement m_cameraMovement;
	MouseMovement m_mouseMovement;
	float m_acceleration = 100.0f;
	float m_damping = 5.0f;
	float m_maxSpeed = 10.0f;
	float m_slowSpeed = 10.0f;
	float m_mouseSpeed = 4.0f;

	void Update(float dt);

	bool KeyPressedHandler(CameraMovement& movement, int key, int scanCode, int action, int mods);
	void MouseMovedHandler(int xPos, int yPos);
	void MouseButtonHandler(int button, int action, int mods);

	const glm::mat4& GetProjectionMatrix() const { return m_projection; }
	glm::vec3 GetPosition() const { return m_cameraPosition; }
	glm::mat4 GetViewMatrix() const;
	glm::mat4 GetVPMatrix() const;

private:
	void CalculateVelocity(float dt);
	glm::vec3 CalculateAcceleration();

	void CalculateMousePos();
	void SetUpOrientation();

	glm::mat4 m_projection = glm::mat4(0.0f);
	glm::vec3 m_cameraPosition = glm::vec3(0.0f);
	glm::quat m_cameraOrientation = glm::quat(glm::vec3(0.0f));
	glm::vec3 m_velocity = glm::vec3(0.0f);
	glm::vec2 m_oldMousePos = glm::vec2(0.0f);
	glm::vec3 m_up = glm::vec3(0.0f);

	int windowWidth = 0;
	int windowHeight = 0;
};
