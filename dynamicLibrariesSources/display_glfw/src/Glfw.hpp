#pragma once

#include <memory>
#include <string>
#include <map>
#include <list>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstdint>
#include "KeyStateManager.hpp"

#define DISPLAY_GLFW_WIN_WIDTH 1024
#define DISPLAY_GLFW_WIN_HEIGHT 720

class Glfw : public KeyStateManager {
public:
	class ConstructorException : public std::exception {
	public:
		ConstructorException(void) noexcept;
		ConstructorException(std::string) noexcept;
		virtual const char *what() const noexcept;
		~ConstructorException(void) noexcept;
		ConstructorException(ConstructorException const &src) noexcept;
	private:
		ConstructorException &operator=(ConstructorException const &rhs) noexcept;
		std::string _error;
	};

	Glfw() = delete;
	Glfw(std::string const &name, uint16_t width, uint16_t height);
	virtual ~Glfw();
	Glfw(Glfw const &shader) = delete;
	Glfw &operator=(Glfw const &shader) = delete;


	virtual void update();
	void render();
	bool exit() const;

protected:
	bool cursor_;

	static float offsetX_;
	static float offsetY_;
	static bool mouseCallbackCalled_;

private:
	GLFWwindow *window_;

	void clean_();


	static std::map<GLFWwindow *, Glfw &> glfwByWindow_;
	static float lastX_;
	static float lastY_;
	static bool firstMouse_;

	static void mouseCallback_(GLFWwindow *, double xpos, double ypos);
	static void callbackKey_(GLFWwindow *window, int key, int, int action, int);
	static void callbackError_(int, const char *errorMessage);
};