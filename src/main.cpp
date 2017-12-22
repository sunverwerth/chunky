#include <glad/glad.h>
#include <GLFW/glfw3.h>

#ifdef _WIN32
#include <Windows.h>
#endif

#include "GLContext.h"
#include "GLProgram.h"
#include "GLMesh.h"
#include "lina.h"
#include "Model.h"
#include "Material.h"
#include "GUI.h"
#include "Camera.h"
#include "Chunk.h"
#include "ChunkGenerator.h"
#include "GLDebug.h"
#include "stb_image.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

std::string readFile(const std::string& name) {
	std::ifstream t(name);
	std::stringstream buffer;
	buffer << t.rdbuf();
	return buffer.str();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double dx, double dy);

void error(const std::string& msg) {
#ifdef _WIN32
	MessageBoxA(0, msg.c_str(), "Error", MB_OK);
#else
	std::cerr << msg << "\n";
#endif
}

GLContext gl;
Camera* camera = nullptr;
std::vector<Chunk*> chunks;
int hits = 0;
int misses = 0;
Chunk* lastChunk = nullptr;
Chunk* getChunk(int gridx, int gridy, int gridz) {
	if (lastChunk && lastChunk->gridx == gridx && lastChunk->gridy == gridy && lastChunk->gridz == gridz) {
		hits++;
		return lastChunk;
	}
	misses++;
	for (auto& chunk : chunks) {
		if (chunk->gridx == gridx && chunk->gridy == gridy && chunk->gridz == gridz) {
			lastChunk = chunk;
			return chunk;
		}
	}
	return nullptr;
}

BlockType getBlockAt(int x, int y, int z);
Vector3 getChunkPos(int x, int y, int z);

Vector3 getChunkPos(int x, int y, int z) {
	Vector3 result(x / chunkSize, y / chunkSize, z / chunkSize);
	if (x < 0 && (x % chunkSize) != 0) result.x -= 1;
	if (y < 0 && (y % chunkSize) != 0) result.y -= 1;
	if (z < 0 && (z % chunkSize) != 0) result.z -= 1;
	return result;
}

Vector3 floor(const Vector3& pos) {
	return Vector3(std::floorf(pos.x), std::floorf(pos.y), std::floorf(pos.z));
}

BlockType getBlockAt(const Vector3& pos) {
	auto qp = floor(pos);
	return getBlockAt(qp.x, qp.y, qp.z);
}

BlockType getBlockAt(int x, int y, int z) {
	Vector3 cp = getChunkPos(x, y, z);
	auto chunk = getChunk(cp.x, cp.y, cp.z);
	if (!chunk) return BlockType::AIR;

	return chunk->getBlockAt(x - cp.x*chunkSize, y - cp.y*chunkSize, z - cp.z*chunkSize);
}

void setBlockAt(int x, int y, int z, BlockType type) {
	Vector3 cp = getChunkPos(x, y, z);
	auto chunk = getChunk(cp.x, cp.y, cp.z);
	if (!chunk) return;

	chunk->setBlockAt(x - cp.x*chunkSize, y - cp.y*chunkSize, z - cp.z*chunkSize, type);
}

double time, dt;
Vector3 position(0.5, 10, 0.5);
Vector3 velocity(0, 0, 0);
Vector2 move(0, 0);
bool forward, backward, left, right, jump, click, grounded, debugInfo = false, fullScreen = false;
bool initPlayer = true;

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int nShowCmd) {
#else
int main() {
#endif
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	gl.width = 1024;
	gl.height = 768;

	GLFWwindow* window = glfwCreateWindow(gl.width, gl.height, "LearnOpenGL", NULL, NULL);
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
		error("Failed to initialize GLAD");
		return -1;
	}

	gl.enable(GL_DEPTH_TEST);
	gl.enable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	std::vector<Model*> models;

	camera = new Camera();
	camera->position = position;
	camera->rotation = Quaternion::identity;
	camera->fov = 3.14f / 2.0f;
	camera->zNear = 0.1f;
	camera->zFar = 1000.0f;
	camera->isOrthographic = false;

	auto gui = new GUI();
	gui->camera = new Camera();
	gui->camera->isOrthographic = true;
	gui->camera->zNear = 0.1f;
	gui->camera->zFar = 1000.0f;
	gui->material = new Material();
	gui->material->alpha = true;
	gui->material->depthTest = false;
	gui->material->depthWrite = false;
	gui->material->program = gl.createProgram("assets/gui_vs.glsl", "assets/gui_fs.glsl");
	gui->material->textures.push_back(gl.loadTexture("assets/gui.png"));

	GUI::Label* label = new GUI::Label(Vector2(0, 0), "");
	label->color = Vector4::white;
	gui->root->children.push_back(label);

	gui->root->children.push_back(new GUI::Label(Vector2::zero, "."));

	class Meter : public GUI::Widget {
	public:
		struct Axis {
			bool horiz;
			int pos;
			Vector4 col;
		};

		Meter(int width, int height) : Widget(Vector2::zero), width(width), height(height), samples(new float[width]) {
			std::fill(samples, samples + width, 0.0f);
		}

		void addSample(float v) {
			samples[pos] = v;
			pos = (pos + 1) % width;
		}

		void addAxisHorizontal(int y, const Vector4& color) {
			axes.push_back({ true, y, color });
		}

		void addAxisVertical(int x, const Vector4& color) {
			axes.push_back({ false, x, color });
		}

		void generateOwnVertices(const Vector2& offset, std::vector<GUI::GUIVertex>& vertices) override {
			for (int i = 0; i < width; ++i) {
				auto p = offset + position + Vector2(i, 0);
				float samp = samples[(pos + i) % width];
				quad(vertices, p, Vector2(1, samp), Vector2(0, 0), Vector2(1.0f / 256, 1.0f / 256), Vector4(1, 1, 1, 0.75));
			}
			for (auto& axis : axes) {
				if (axis.horiz) {
					quad(vertices, position + offset + Vector2(0, axis.pos), Vector2(width, 1), Vector2(0, 0), Vector2(1.0f / 256, 1.0f / 256), axis.col);
				}
				else {
					quad(vertices, position + offset + Vector2(axis.pos, 0), Vector2(1, height), Vector2(0, 0), Vector2(1.0f / 256, 1.0f / 256), axis.col);
				}
			}
		}

		int width;
		int height;
		std::vector<Axis> axes;
		float* samples;
		int pos = 0;
	};

	auto meter = new Meter(200, 100);
	meter->addAxisHorizontal(60, Vector4::blue);
	meter->addAxisHorizontal(30, Vector4::red);
	meter->children.push_back(new GUI::Label(Vector2(0, 34), "30"));
	meter->children.push_back(new GUI::Label(Vector2(0, 64), "60"));
	gui->root->children.push_back(meter);

	auto chMat = new Material();
	chMat->alpha = false;
	chMat->program = gl.createProgram("assets/vs.glsl", "assets/fs.glsl");
	chMat->textures.push_back(gl.loadTexture("assets/atlas.png"));
	Chunk::chunkMaterial = chMat;

	auto wMat = new Material();
	wMat->alpha = true;
	wMat->program = chMat->program;
	wMat->textures.push_back(chMat->textures[0]);
	Chunk::waterMaterial = wMat;

	ChunkGenerator gen(glfwGetTimerValue());

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	double lastTick = glfwGetTime();

	GLDebug::init();
	models.push_back(GLDebug::lineModel);

	// render loop
	// -----------
	auto fogColor = Vector3(0.8, 0.8, 1);
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		gl.clearAll(Vector4(fogColor.r, fogColor.g, fogColor.b, 1.0f), 1.0);

		time = glfwGetTime();
		dt = time - lastTick;
		double realDt = dt;
		lastTick = time;

		if (dt > 0.1) {
			dt = 0.1;
		}

		auto p = camera->position;
		bool hasBlock = false;
		Vector3 blockPos;
		for (int i = 0; i < 100; ++i) {
			p = p + camera->front() * 0.1f;
			auto fl = floor(p);
			auto ch = getChunkPos(fl.x, fl.y, fl.z);
			auto chunk = getChunk(ch.x, ch.y, ch.z);
			if (chunk && chunk->getBlockAt(fl.x - ch.x*chunkSize, fl.y - ch.y*chunkSize, fl.z - ch.z*chunkSize) != BlockType::AIR) {
				hasBlock = true;
				blockPos = p;
				//GLDebug::aabb(fl-Vector3(0.001,0.001,0.001), fl + Vector3(1.002, 1.002, 1.002), Vector4(1,1,1,0.5f));
				break;
			}
		}

		velocity.z = velocity.x = 0;
		auto r = camera->right();
		auto f = camera->front();
		f.y = 0;
		f.normalize();
		if (forward) velocity = velocity + f * 4;
		if (backward) velocity = velocity - f * 4;
		if (left) velocity = velocity - r * 4;
		if (right) velocity = velocity + r * 4;
		if (grounded && jump) velocity.y = 6;
		if (click && hasBlock) {
			auto fl = floor(blockPos);
			auto ch = getChunkPos(fl.x, fl.y, fl.z);
			auto chunk = getChunk(ch.x, ch.y, ch.z);
			chunk->setBlockAt(fl.x - ch.x*chunkSize, fl.y - ch.y*chunkSize, fl.z - ch.z*chunkSize, BlockType::AIR);
			for (int x = -1; x < 2; ++x) {
				for (int y = -1; y < 2; ++y) {
					for (int z = -1; z < 2; ++z) {
						chunk->liveBlocks.push_back(DynamicBlock{ (int)fl.x - (int)ch.x*chunkSize + x, (int)fl.y - (int)ch.y*chunkSize + y, (int)fl.z - (int)ch.z*chunkSize + z,1 });
					}
				}
			}
		}

		auto qp = floor(position);
		auto cp = getChunkPos(qp.x, qp.y, qp.z);

		std::vector<Chunk*> newChunks;
		for (int y = -1; y < 2; ++y) {
			for (int x = -5; x < 6; ++x) {
				for (int z = -5; z < 6; ++z) {
					int ix = cp.x + x;
					int iy = cp.y + y;
					int iz = cp.z + z;
					if (getChunk(ix, iy, iz)) continue;
					auto chunk = new Chunk(ix, iy, iz);
					chunk->generateBlocks(&gen);
					chunks.push_back(chunk);
				}
			}
		}

		std::vector<Chunk*> remaining;
		for (auto& chunk : chunks) {
			Vector3 pos = Vector3(chunk->gridx, chunk->gridy, chunk->gridz)*chunkSize;
			if ((pos - position).length() > camera->zFar) {
				if (chunk == lastChunk) lastChunk = nullptr;
				if (chunk->model) models.erase(std::remove(models.begin(), models.end(), chunk->model));
				if (chunk->waterModel) models.erase(std::remove(models.begin(), models.end(), chunk->waterModel));
				delete chunk;
			}
			else {
				remaining.push_back(chunk);
			}
		}
		chunks = remaining;

		std::sort(chunks.begin(), chunks.end(), [&](Chunk* a, Chunk* b) {
			auto d1 = (Vector3(a->gridx*chunkSize, a->gridy*chunkSize, a->gridz*chunkSize) - camera->position).lengthSq();
			auto d2 = (Vector3(b->gridx*chunkSize, b->gridy*chunkSize, b->gridz*chunkSize) - camera->position).lengthSq();
			return d1 < d2;
		});
		for (auto& chunk : chunks) {
			Vector3 min = Vector3(chunk->gridx*chunkSize, chunk->gridy*chunkSize, chunk->gridz*chunkSize);
			Vector3 max = min + Vector3(chunkSize, chunkSize, chunkSize);
			Vector4 col(Vector4::white);
			if (chunk->isNew) col = Vector4::blue;
			else if (!chunk->liveBlocks.empty()) col = Vector4::red;
			if (debugInfo) GLDebug::aabb(min, max, col);
			if (chunk->isDirty) {
				chunk->generateModel();
				if (chunk->isNew) {
					models.push_back(chunk->model);
					models.push_back(chunk->waterModel);
					chunk->isNew = false;
				}
				if (chunk->model->mesh->numVertices > 0) break;
			}
		}

		if (initPlayer) {
			initPlayer = false;
			for (int y = 300; y > -300; --y) {
				if (getBlockAt(Vector3(position.x, y, position.z)) != BlockType::AIR) {
					position.y = y + 1;
					break;
				}
			}
		}

		for (auto& updateChunk : chunks) {

			if (!updateChunk->liveBlocks.empty()) {
				int i = rand() % updateChunk->liveBlocks.size();
				auto block = updateChunk->liveBlocks[i];
				updateChunk->liveBlocks[i] = updateChunk->liveBlocks.back();
				updateChunk->liveBlocks.pop_back();

				Vector3 wp(block.x + updateChunk->gridx*chunkSize, block.y + updateChunk->gridy*chunkSize, block.z + updateChunk->gridz*chunkSize);
				auto type = getBlockAt(wp.x, wp.y, wp.z);
				if (type == BlockType::WOOD) {
					if (block.power > 0 && getBlockAt(wp.x, wp.y + 1, wp.z) == BlockType::AIR) {
						setBlockAt(wp.x, wp.y + 1, wp.z, BlockType::WOOD);
						updateChunk->liveBlocks.push_back(DynamicBlock{ block.x, block.y + 1, block.z, block.power - 1 });
					}
					if (block.power > 0 && block.power < 8) {
						for (int dx = -1; dx < 2; ++dx) {
							for (int dz = -1; dz < 2; ++dz) {
								if (getBlockAt(wp.x + dx, wp.y, wp.z + dz) == BlockType::AIR) {
									setBlockAt(wp.x + dx, wp.y, wp.z + dz, BlockType::LEAVES);
									updateChunk->liveBlocks.push_back(DynamicBlock{ block.x + dx, block.y, block.z + dz, block.power / 4 });
								}
							}
						}
					}
				}
				else if (type == BlockType::LEAVES) {
					if (block.power > 0) {
						for (int dx = -1; dx < 2; ++dx) {
							for (int dz = -1; dz < 2; ++dz) {
								if (getBlockAt(wp.x + dx, wp.y, wp.z + dz) == BlockType::AIR) {
									setBlockAt(wp.x + dx, wp.y, wp.z + dz, BlockType::LEAVES);
									updateChunk->liveBlocks.push_back(DynamicBlock{ block.x + dx, block.y, block.z + dz, block.power - 1 });
								}
							}
						}
					}
				}
				else if (type == BlockType::WATER) {
					if (getBlockAt(wp.x, wp.y - 1, wp.z) == BlockType::AIR) {
						setBlockAt(wp.x, wp.y - 1, wp.z, BlockType::WATER);
						setBlockAt(wp.x, wp.y, wp.z, BlockType::AIR);
						updateChunk->liveBlocks.push_back(DynamicBlock{ block.x, block.y - 1, block.z, block.power });
					}
					else {
						for (int dx = -1; dx < 2; ++dx) {
							for (int dz = -1; dz < 2; ++dz) {
								if (getBlockAt(wp.x + dx, wp.y, wp.z + dz) == BlockType::AIR) {
									setBlockAt(wp.x + dx, wp.y, wp.z + dz, BlockType::WATER);
									updateChunk->liveBlocks.push_back(DynamicBlock{ block.x + dx, block.y, block.z + dz, block.power });
								}
							}
						}
					}
				}
			}
		}

		grounded = false;
		auto floorBlock = getBlockAt(qp.x, qp.y, qp.z);
		if (floorBlock == BlockType::AIR) {
			velocity.y -= 15.81f*dt;
		}
		else if (floorBlock == BlockType::WATER) {
			grounded = true;
			velocity.y -= 1.5f*dt;
			if (velocity.y > 2) {
				velocity.y = 2;
			}
			if (velocity.y < -2) {
				velocity.y = -2;
			}
		}

		position = position + velocity * dt;
		qp = floor(position);

		if (velocity.y < 0) {
			qp = floor(position);
			floorBlock = getBlockAt(qp.x, qp.y, qp.z);
			if (floorBlock != BlockType::AIR && floorBlock != BlockType::WATER) {
				velocity.y = 0;
				position.y = qp.y + 1 + 0.001f;
				grounded = true;
			}
		}

		if (velocity.x > 0) {
			qp = floor(position + Vector3::right*0.25f);
			floorBlock = getBlockAt(qp.x, qp.y, qp.z);
			auto qp2 = floor(position);
			auto floorBlock2 = getBlockAt(qp2.x, qp2.y, qp2.z);
			if (floorBlock != BlockType::AIR && floorBlock != BlockType::WATER) {
				velocity.x = 0;
				position.x = qp.x - 1 + 0.5f + 0.25f - 0.001f;
			}
		}
		else if (velocity.x < 0) {
			qp = floor(position + Vector3::left*0.25f);
			floorBlock = getBlockAt(qp.x, qp.y, qp.z);
			if (floorBlock != BlockType::AIR && floorBlock != BlockType::WATER) {
				velocity.x = 0;
				position.x = qp.x + 1 + 0.5f - 0.25f + 0.001f;
			}
		}

		if (velocity.z > 0) {
			qp = floor(position + Vector3::backward*0.25f);
			floorBlock = getBlockAt(qp.x, qp.y, qp.z);
			if (floorBlock != BlockType::AIR && floorBlock != BlockType::WATER) {
				velocity.z = 0;
				position.z = qp.z - 1 + 0.5f + 0.25f - 0.001f;
			}
		}
		else if (velocity.z < 0) {
			qp = floor(position + Vector3::forward*0.25f);
			floorBlock = getBlockAt(qp.x, qp.y, qp.z);
			if (floorBlock != BlockType::AIR && floorBlock != BlockType::WATER) {
				velocity.z = 0;
				position.z = qp.z + 1 + 0.5f - 0.25f + 0.001f;
			}
		}

		int numActive = 0;
		for (auto& chunk : chunks) {
			numActive += chunk->liveBlocks.size();
		}

		std::stringstream sstr;
		sstr << "pos   - X: " << position.x << " Y: " << position.y << " Z: " << position.z << "\n";
		sstr << "block - X: " << qp.x << " Y: " << qp.y << " Z: " << qp.z << "\n";
		sstr << "chunk - X: " << cp.x << " Y: " << cp.y << " Z: " << cp.z << "\n";
		sstr << "Active blocks: " << numActive << "\n";
		sstr << "Chunk hit ratio: " << ((long long)hits * 100 / (hits + misses)) << "%\n";
		label->text = sstr.str();

		meter->position = label->position - Vector2(0, 150);
		meter->addSample(1.0f / realDt);

		// camera
		camera->position = position + Vector3::up * 1.5f;
		camera->width = gl.width;
		camera->height = gl.height;
		auto view = camera->getViewMatrix();
		auto projection = camera->getProjectionMatrix();

		// light
		auto lightPos = Vector3(0, 1, -4);
		auto lightColor = Vector3(1.0f, 1.0f, 1.0f);

		GLDebug::buildMesh();

		std::sort(models.begin(), models.end(), [&](const Model* a, const Model* b) {
			int ia = 1;
			if (a->material->alpha) ia *= 100000;
			if (!a->material->depthTest) ia *= 100000;
			int ib = 1;
			if (b->material->alpha) ib *= 100000;
			if (!b->material->depthTest) ib *= 100000;
			return ia < ib;
		});

		// models
		for (auto& model : models) {
			model->fade -= dt;
			if (model->fade < 0) model->fade = 0;
			model->material->use();

			model->material->program->setUniform("view", view);
			model->material->program->setUniform("projection", projection);
			model->material->program->setUniform("eyePos", camera->position);
			model->material->program->setUniform("lightPos", lightPos);
			model->material->program->setUniform("lightColor", lightColor);
			model->material->program->setUniform("fogColor", fogColor);
			model->material->program->setUniform("fade", model->fade);
			model->material->program->setUniform("time", (float)glfwGetTime());

			auto world = Matrix4x4(model->rotation);
			world = world * matrixTranslation(model->position);

			model->material->program->setUniform("world", world);
			model->material->program->setUniform("worldView", view * world);
			model->material->program->setUniform("worldViewProjection", projection * view * world);

			model->mesh->draw();
		}

		GLDebug::reset();


		gl.clearDepth(1.0);

		gui->camera->width = gl.width;
		gui->camera->height = gl.height;

		label->position.x = -gl.width / 2;
		label->position.y = gl.height / 2;

		gui->material->use();
		gui->material->program->setUniform("projection", gui->camera->getProjectionMatrix());

		gui->updateMesh();
		gui->mesh->draw();

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
	static bool oldmousedown;
	forward = backward = left = right = click = jump = false;

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		forward = true;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		left = true;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		backward = true;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		right = true;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		jump = true;
	}
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
		initPlayer = true;
		position = Vector3(0.5f + rand(), 50, 0.5f + rand());
	}
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
		debugInfo = !debugInfo;
	}
	if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS) {
		fullScreen = !fullScreen;
		if (fullScreen) {
			auto monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode * mode = glfwGetVideoMode(monitor);
			glfwSetWindowSize(window, mode->width, mode->height);
			glfwSetWindowPos(window, 0, 0);
		}
		else {
			// Use start-up values for "windowed" mode.
			glfwSetWindowSize(window, 1024, 768);
			glfwSetWindowPos(window, 0, 0);
		}
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) && !oldmousedown) {
		click = true;
	}
	oldmousedown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	gl.width = width;
	gl.height = height;
	gl.viewport(0, 0, width, height);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void mouse_callback(GLFWwindow* window, double x, double y)
{
	static double oldx = x;
	static double oldy = y;

	auto dx = x - oldx;
	auto dy = y - oldy;

	oldx = x;
	oldy = y;

	if (camera) {
		camera->yaw -= dx*0.005f*camera->fov;
		camera->pitch -= dy*0.005f*camera->fov;
	}
}


void scroll_callback(GLFWwindow* window, double dx, double dy)
{
	if (camera) {
		camera->fov -= dy*0.1f;
	}
}
