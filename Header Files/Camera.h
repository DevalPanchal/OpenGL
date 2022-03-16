#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

class Camera {
	public:
		// position of the camera
		glm::vec3 Position;
		// the direction at which the camera is looking at, in this case the origin
		glm::vec3 Direction = glm::vec3(0.0f, 0.0f, -1.0f);
		// the positive y-axis is the up direction
		glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

		// screen height
		float width;
		float height;

		// camera speed
		float speed = 0.03f;
		// angle for viewing left and right (rotations)
		float viewingAngleX = -90.0f;
		float flyMode = 0.0f;

		// simple constructor
		Camera(float screenWidth, float screenHeight) {
			Camera::width = screenWidth;
			Camera::height = screenHeight;
			this->Position = glm::vec3(0.0f, 0.1f, 1.0f);
		}

		glm::mat4 Camera::getViewMatrix() {
			glm::mat4 view = glm::mat4(1.0f);

			view = glm::lookAt(Position, Position + Direction, Up);

			return view;
		}

		glm::mat4 Camera::getCameraPerspective() {
			glm::mat4 projection = glm::mat4(1.0f);

			projection = glm::perspective(glm::radians(45.0f), this->width / this->height, 0.1f, 300.0f);

			return projection;
		}

		void keyboardInputs(GLFWwindow* window) {
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
				// increase position in forward direction
				Position += this->speed * Direction;
			}
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
				// subtract viewing angle when looking left
				this->viewingAngleX -= 1.5f;
				float updateRotationX = cos(glm::radians(this->viewingAngleX));
				float updateRotationZ = sin(glm::radians(this->viewingAngleX));
				// update Direction vector
				Direction = glm::vec3(updateRotationX, 0.0f, updateRotationZ);
			}
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
				// decreasing position in backward direction
				Position -= this->speed * Direction;
			}
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
				// add to viewing angle when looking right
				this->viewingAngleX += 1.5f;
				float updateRotationX = cos(glm::radians(this->viewingAngleX));
				float updateRotationZ = sin(glm::radians(this->viewingAngleX));
				// increase position in direction
				Direction = glm::vec3(updateRotationX, 0.0f, updateRotationZ);
			}
			if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
				this->speed = 0.09f;
			}
			else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
				this->speed = 0.03;
			}

			if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
				this->flyMode += 0.01f;
				this->Position = glm::vec3(this->Position.x, 0.1f + flyMode, this->Position.z);
			}

			if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
				this->flyMode -= 0.01f;
				this->Position = glm::vec3(this->Position.x, 0.1f + flyMode, this->Position.z);
			}
			if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
				this->flyMode = 0.0f;
				this->Position = glm::vec3(0.0f, 0.1f + flyMode, 1.0f);
			}
		}
};


#endif