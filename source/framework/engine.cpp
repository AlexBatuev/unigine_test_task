#include "engine.h"

Engine *Engine::get()
{
	static Engine engine;
	return &engine;
}

bool Engine::init(int width, int height, const char *title)
{
	window_width = static_cast<float>(width);
	window_height = static_cast<float>(height);

	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	// configure global opengl state
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// build and compile shaders
	shader.load("shader.vert", "shader.frag");
	shader.use();

	// camera
	camera.Position = glm::vec3(0.0f, 0.0f, 3.0f);
	lastX = window_width / 2.0f;
	lastY = window_height / 2.0f;
	firstMouse = true;
	cam_speed = SPEED;

	// timing
	deltaTime = 0.0f;
	lastFrame = 0.0f;

	// lighting
	lightDir = glm::normalize(glm::vec3(-0.5f, -2.0f, 0.5f));
	lightColor = glm::vec3(1.0f, 0.92f, 0.71f); // yellow
	lightAmbient = glm::vec3(0.23f, 0.245f, 0.3f); // blue

	envColor = glm::vec3(0.61f, 0.66f, 0.68f); // blue

	return true;
}

bool Engine::isDone()
{
	return glfwWindowShouldClose(window) != 0;
}

void Engine::update()
{
	// per-frame time logic
	float currentFrame = static_cast<float>(glfwGetTime());
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	// input
	processInput(window);
}

void Engine::render()
{
	// background
	glClearColor(envColor.x, envColor.y, envColor.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// camera
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), window_width / window_height, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();
	shader.use();
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);
	
	// light
	shader.setVec3("viewPos", camera.Position);
	shader.setVec3("lightDir", -lightDir);
	shader.setVec3("lightColor", lightColor);
	shader.setVec3("lightAmbient", lightAmbient);

	// render objects
	for (int i = 0; i < objects.size(); i++)
		objects[i]->draw();

	// restore to default
	shader.setMat4("model", glm::mat4(1.0f));
	shader.setVec3("albedo", glm::vec3(1.0f));
}

void Engine::swap()
{
	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void Engine::shutdown()
{
	// destroy objects
	for (int i = 0; i < objects.size(); i++)
		delete objects[i];
	objects.clear();

	glfwTerminate();
}

Object *Engine::createObject()
{
	Object *obj = new Object(&shader);
	objects.push_back(obj);
	return obj;
}

Object *Engine::createObject(Mesh *mesh)
{
	Object *obj = new Object(mesh, &shader);
	objects.push_back(obj);
	return obj;
}

void Engine::deleteObject(int index)
{
	delete objects[index];
	objects.erase(objects.begin() + index);
}

void Engine::deleteObject(Object *obj)
{
	auto it = std::find(objects.begin(), objects.end(), obj);
	if (it == objects.end())
		return;

	objects.erase(it);
	delete obj;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void Engine::processInput(GLFWwindow *window)
{
	Engine *engine = Engine::get();
	Camera &camera = engine->camera;
	float deltaTime = engine->deltaTime;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.MovementSpeed = engine->cam_speed * 2.0f;
	else
		camera.MovementSpeed = engine->cam_speed;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void Engine::framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void Engine::mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
	Engine *engine = Engine::get();
	bool &firstMouse = engine->firstMouse;
	float &lastX = engine->lastX;
	float &lastY = engine->lastY;

	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	engine->camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void Engine::scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
	Engine::get()->camera.ProcessMouseScroll(static_cast<float>(yoffset));
}