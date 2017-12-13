#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

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
const unsigned int SCR_HEIGHT = 600;

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

class GLVertexArray {
public:
	GLVertexArray(GLVertexBuffer* buffer): buffer(buffer) {
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
	GLMesh(GLVertexArray* vertices, GLElementBuffer* indices): vertices(vertices), indices(indices) {
	}

	void bind() {
		vertices->bind();
		indices->bind();
	}

	GLVertexArray* vertices;
	GLElementBuffer* indices;
};

class GL {
public:
	void clearColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
		glClearColor(r, g, b, a);
		glClear(GL_COLOR_BUFFER_BIT);		
	}

	void viewport(GLint x, GLint y, GLsizei width, GLsizei height) {
		glViewport(0, 0, width, height);
	}

	void drawIndexed(GLenum mode, GLsizei count, GLenum type, const void* indices) {
		glDrawElements(mode, count, type, indices);
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
		0.5f,  0.5f, 0.0f,	1.0f, 0.0f, 0.0f,  // top right
		0.5f, -0.5f, 0.0f,	0.0f, 1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,	0.0f, 0.0f, 1.0f, // bottom left
		-0.5f,  0.5f, 0.0f,	1.0f, 0.0f, 1.0f, // top left 
	};

	VBO.setData(sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	VBO.unbind();
	VAO.unbind();

	auto EBO = GLElementBuffer();
	EBO.bind();

	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	EBO.setData(sizeof(indices), indices, GL_STATIC_DRAW);

	EBO.unbind();

	auto mesh = GLMesh(&VAO, &EBO);

	// uncomment this call to draw in wireframe polygons.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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

		// draw our first triangle
		program.use();

		float timeValue = glfwGetTime();
		float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
		program.setUniform4f("ourColor", 0.0f, greenValue, 0.0f, 1.0f);

		mesh.bind();
		gl.drawIndexed(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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
