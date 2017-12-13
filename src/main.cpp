#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "stb_image.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

std::string readFile(const char* name) {
	std::ifstream t(name);
	std::stringstream buffer;
	buffer << t.rdbuf();
	return buffer.str();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

class GLShader {
public:
	virtual ~GLShader() {
		glDeleteShader(handle);
	}

	bool isSuccess() {
		int success;
		glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
		return success;
	}

	std::string getInfoLog() {
		char infoLog[512];
		glGetShaderInfoLog(handle, 512, NULL, infoLog);
		return infoLog;
	}

	GLuint handle;
};

class GLVertexShader : public GLShader {
public:
	GLVertexShader(const std::string& source) {
		auto str = source.c_str();
		handle = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(handle, 1, &str, NULL);
		glCompileShader(handle);
	}
};

class GLFragmentShader : public GLShader {
public:
	GLFragmentShader(const std::string& source) {
		auto str = source.c_str();
		handle = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(handle, 1, &str, NULL);
		glCompileShader(handle);
	}
};

class GLVertexBuffer {
public:
	GLVertexBuffer() {
		glGenBuffers(1, &handle);
	}

	~GLVertexBuffer() {
		glDeleteBuffers(1, &handle);
	}

	void bind() {
		glBindBuffer(GL_ARRAY_BUFFER, handle);
	}

	void unbind() {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void setData(GLsizeiptr size, const void* data, GLenum usage) {
		glBufferData(GL_ARRAY_BUFFER, size, data, usage);
	}

	GLuint handle;
};

class GLProgram {
public:
	GLProgram(const GLVertexShader& vs, const GLFragmentShader& fs) {
		handle = glCreateProgram();
		glAttachShader(handle, vs.handle);
		glAttachShader(handle, fs.handle);
		glLinkProgram(handle);
	}

	~GLProgram() {
		glDeleteProgram(handle);
	}

	bool isSuccess() {
		int success;
		glGetProgramiv(handle, GL_LINK_STATUS, &success);
		return success;
	}

	std::string getInfoLog() {
		char infoLog[512];
		glGetProgramInfoLog(handle, 512, NULL, infoLog);
		return infoLog;
	}

	int getUniformLocation(const char* name) {
		return glGetUniformLocation(handle, name);
	}

	void use() {
		glUseProgram(handle);
	}

	void setUniform4f(const char* name, GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
		int location = getUniformLocation(name);
		glUniform4f(location, x, y, z, w);
	}

	GLuint handle;
};

class GLVertexArray {
public:
	GLVertexArray(GLVertexBuffer* buffer) : buffer(buffer) {
		glGenVertexArrays(1, &handle);
	}

	~GLVertexArray() {
		glDeleteVertexArrays(1, &handle);
	}

	void bind() {
		glBindVertexArray(handle);
	}

	void unbind() {
		glBindVertexArray(0);
	}

	GLVertexBuffer* buffer;
	GLuint handle;

};

class GLElementBuffer {
public:
	GLElementBuffer() {
		glGenBuffers(1, &handle);
	}

	~GLElementBuffer() {
		glDeleteBuffers(1, &handle);
	}

	void bind() {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
	}

	void unbind() {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void setData(GLsizeiptr size, const void* data, GLenum usage) {
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
	}

	GLuint handle;
};

class GLMesh {
public:
	GLMesh(GLVertexArray* vertices, GLElementBuffer* indices) : vertices(vertices), indices(indices) {
	}

	void bind() {
		vertices->bind();
		indices->bind();
	}

	GLVertexArray* vertices;
	GLElementBuffer* indices;

};

class GLTexture {
public:
	unsigned int handle;
};

class GL {
public:
	int width;
	int height;

	void clearColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
		glClearColor(r, g, b, a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void viewport(GLint x, GLint y, GLsizei width, GLsizei height) {
		width = width;
		height = height;
		glViewport(0, 0, width, height);
	}

	void drawIndexed(GLenum mode, GLsizei count, GLenum type, const void* indices) {
		glDrawElements(mode, count, type, indices);
	}

	void use(GLProgram* program) {
		glUseProgram(program->handle);
	}

	GLTexture* loadTexture2D(const char* name) {
		unsigned int handle;
		glGenTextures(1, &handle);
		glBindTexture(GL_TEXTURE_2D, handle);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_set_flip_vertically_on_load(true);

		int width, height, nrChannels;
		unsigned char *data = stbi_load(name, &width, &height, &nrChannels, 0);
		if (!data) {
			glDeleteTextures(1, &handle);
			return nullptr;
		}

		unsigned int format;
		switch (nrChannels) {
		case 4:
			format = GL_RGBA;
			break;
		case 3:
		default:
			format = GL_RGB;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(data);

		auto texture = new GLTexture();
		texture->handle = handle;
		return texture;
	}

	void bind(GLTexture* texture, int slot) {
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, texture->handle);
	}

	void bind(GLMesh* mesh) {
		mesh->bind();
	}
};

GL gl;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

														 // glfw window creation
														 // --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	gl.width = SCR_WIDTH;
	gl.height = SCR_HEIGHT;

	glEnable(GL_DEPTH_TEST);

	// vertex shader
	auto vs = GLVertexShader(readFile("assets/vs.glsl"));
	if (!vs.isSuccess())
	{
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << vs.getInfoLog() << std::endl;
		return -1;
	}

	// fragment shader
	auto fs = GLFragmentShader(readFile("assets/fs.glsl"));
	if (!fs.isSuccess())
	{
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << fs.getInfoLog() << std::endl;
		return -1;
	}

	// link shaders
	auto program = GLProgram(vs, fs);
	if (!program.isSuccess()) {
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << program.getInfoLog() << std::endl;
		return -1;
	}

	auto VBO = GLVertexBuffer();
	auto VAO = GLVertexArray(&VBO);

	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	VAO.bind();
	VBO.bind();

	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	VBO.setData(sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	VBO.unbind();
	VAO.unbind();

	auto EBO = GLElementBuffer();
	EBO.bind();

	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 2,   // first triangle
		0, 2, 3,    // second triangle
	};

	EBO.setData(sizeof(indices), indices, GL_STATIC_DRAW);

	EBO.unbind();

	auto mesh = GLMesh(&VAO, &EBO);

	// uncomment this call to draw in wireframe polygons.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	program.use(); // don't forget to activate/use the shader before setting uniforms!

	auto texture1 = gl.loadTexture2D("assets/container.jpg");
	auto texture2 = gl.loadTexture2D("assets/awesomeface.png");

	glUniform1i(glGetUniformLocation(program.handle, "texture1"), 0);
	glUniform1i(glGetUniformLocation(program.handle, "texture2"), 1);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// input
		// -----
		processInput(window);

		// render
		// ------
		gl.clearColor(0.2f, 0.3f, 0.3f, 1.0f);

		gl.use(&program);
		gl.bind(texture1, 0);
		gl.bind(texture2, 1);

		glm::mat4 world(1.0f);
		glm::mat4 view(1.0f);
		glm::mat4 projection(1.0f);

		world = glm::rotate(world, (float)glfwGetTime(), glm::vec3(1.0f, 0.0f, 0.0f));
		world = glm::rotate(world, (float)glfwGetTime()*0.31f, glm::vec3(0.0f, 1.0f, 0.0f));
		world = glm::rotate(world, (float)glfwGetTime()*2.7f, glm::vec3(0.0f, 0.0f, 1.0f));

		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
		projection = glm::perspective(glm::radians(45.0f), (float)gl.width / (float)gl.height, 0.1f, 100.0f);

		unsigned int index = glGetUniformLocation(program.handle, "worldMatrix");
		glUniformMatrix4fv(index, 1, GL_FALSE, glm::value_ptr(world));

		index = glGetUniformLocation(program.handle, "viewMatrix");
		glUniformMatrix4fv(index, 1, GL_FALSE, glm::value_ptr(view));

		index = glGetUniformLocation(program.handle, "projectionMatrix");
		glUniformMatrix4fv(index, 1, GL_FALSE, glm::value_ptr(projection));

		gl.bind(&mesh);
		//gl.drawIndexed(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// glBindVertexArray(0); // no need to unbind it every time 

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	gl.viewport(0, 0, width, height);
}
