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
#include "Graph.h"

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

auto fogColor = Vector3(0.8, 0.8, 1);
float fogStart = 50;
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
	return Vector3(floorf(pos.x), floorf(pos.y), floorf(pos.z));
}

Chunk* getChunk(const Vector3& pos) {
	auto fl = floor(pos);
	auto cp = getChunkPos(pos.x, pos.y, pos.z);
	return getChunk(cp.x, cp.y, cp.z);
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

double currentTime, dt;
Vector3 position(0.5, 10, 0.5);
Vector3 velocity(0, 0, 0);
Vector2 move(0, 0);
bool forward, backward, left, right, jump, click, rclick, grounded, debugInfo = false, fullScreen = false;
bool gravity = true;
bool initPlayer = true;

void updateChunk(Chunk* chunk) {
	for (int i = 0; i < 10; ++i) {
		if (!chunk->liveBlocks.empty()) {
			int i = rand() % chunk->liveBlocks.size();
			auto block = chunk->liveBlocks[i];
			chunk->liveBlocks[i] = chunk->liveBlocks.back();
			chunk->liveBlocks.pop_back();

			Vector3 wp(block.x + chunk->gridx*chunkSize, block.y + chunk->gridy*chunkSize, block.z + chunk->gridz*chunkSize);
			auto type = getBlockAt(wp.x, wp.y, wp.z);
			if (type == BlockType::WOOD) {
				if (block.power > 0 && getBlockAt(wp.x, wp.y + 1, wp.z) == BlockType::AIR) {
					setBlockAt(wp.x, wp.y + 1, wp.z, BlockType::WOOD);
					chunk->liveBlocks.push_back(DynamicBlock{ block.x, block.y + 1, block.z, block.power - 1 });
				}
				if (block.power > 0 && block.power < 5) {
					for (int dx = -1; dx < 2; ++dx) {
						for (int dz = -1; dz < 2; ++dz) {
							if (getBlockAt(wp.x + dx, wp.y, wp.z + dz) == BlockType::AIR) {
								setBlockAt(wp.x + dx, wp.y, wp.z + dz, BlockType::LEAVES);
								chunk->liveBlocks.push_back(DynamicBlock{ block.x + dx, block.y, block.z + dz, 1 + rand() % 2 });
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
								chunk->liveBlocks.push_back(DynamicBlock{ block.x + dx, block.y, block.z + dz, block.power - 1 });
							}
						}
					}
				}
			}
			else if (type == BlockType::WATER) {
				if (getBlockAt(wp.x, wp.y - 1, wp.z) == BlockType::AIR) {
					setBlockAt(wp.x, wp.y - 1, wp.z, BlockType::WATER);
					setBlockAt(wp.x, wp.y, wp.z, BlockType::AIR);
					chunk->liveBlocks.push_back(DynamicBlock{ block.x, block.y - 1, block.z, block.power });
				}
				else {
					for (int dx = -1; dx < 2; ++dx) {
						for (int dz = -1; dz < 2; ++dz) {
							if (getBlockAt(wp.x + dx, wp.y, wp.z + dz) == BlockType::AIR) {
								setBlockAt(wp.x + dx, wp.y, wp.z + dz, BlockType::WATER);
								chunk->liveBlocks.push_back(DynamicBlock{ block.x + dx, block.y, block.z + dz, block.power });
							}
						}
					}
				}
			}
		}
	}
}

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

	gl.width = 320;
	gl.height = 200;

	GLFWwindow* window = glfwCreateWindow(gl.width, gl.height, "LearnOpenGL", NULL, NULL);
	glfwSetWindowPos(window, 50, 50);
	glfwGetFramebufferSize(window, &gl.width, &gl.height);

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

	glViewport(0, 0, gl.width, gl.height);

	camera = new Camera();
	camera->width = gl.width;
	camera->height = gl.height;
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
	gui->root->add(label);

	gui->root->add(new GUI::Label(Vector2(0, 4), "."));


	float fps = 0;

	auto meter = new Graph(Vector2::zero, Vector2(200, 50));
	meter->watch = &fps;
	meter->scale = 0.5f;
	meter->addAxisHorizontal(60, "60", Vector4::blue);
	meter->addAxisHorizontal(30, "30", Vector4::red);
	meter->add(new GUI::Label(Vector2(0, 50), "FPS"));
	gui->root->add(meter);

	gui->root->add(new GUI::Panel(Vector2(0, -225), Vector2(32, 32)));

	auto chMat = new Material();
	chMat->alpha = false;
	chMat->program = gl.createProgram("assets/vs.glsl", "assets/fs.glsl");
	chMat->textures.push_back(gl.loadTexture("assets/mc.png"));
	Chunk::chunkMaterial = chMat;

	auto wMat = new Material();
	wMat->alpha = true;
	wMat->program = chMat->program;
	wMat->textures.push_back(chMat->textures[0]);
	Chunk::waterMaterial = wMat;

	ChunkGenerator gen(glfwGetTime()*10000);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	double lastTick = glfwGetTime();

	GLDebug::init();
	models.push_back(GLDebug::lineModel);

	//glfwSwapInterval(0);
	int numTris = 0;

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		currentTime = glfwGetTime();
		dt = currentTime - lastTick;
		lastTick = currentTime;

		fps = 1.0f / dt;

		if (dt > 0.1) {
			dt = 0.1;
		}

		processInput(window);

		gl.clearAll(Vector4(fogColor.r, fogColor.g, fogColor.b, 1.0f), 1.0);

		// raycast block
		auto p = camera->position;
		bool hasBlock = false;
		Vector3 blockPos(0, 0, 0);
		Vector3 prevBlockPos;
		for (int i = 0; i < 100; ++i) {
			p = p + camera->front() * 0.1f;
			prevBlockPos = blockPos;
			blockPos = p;
			auto fl = floor(p);
			auto ch = getChunkPos(fl.x, fl.y, fl.z);
			auto chunk = getChunk(ch.x, ch.y, ch.z);
			if (!chunk) continue;
			auto block = chunk->getBlockAt(fl.x - ch.x*chunkSize, fl.y - ch.y*chunkSize, fl.z - ch.z*chunkSize);
			if (block != BlockType::AIR && block != BlockType::WATER) {
				hasBlock = true;
				//GLDebug::aabb(fl-Vector3(0.001,0.001,0.001), fl + Vector3(1.002, 1.002, 1.002), Vector4(1,1,1,0.5f));
				break;
			}
		}

		// input
		velocity.z = velocity.x = 0;
		if (!gravity) {
			velocity.y = 0;
		}
		float speed = gravity ? 4 : 100;
		auto r = camera->right();
		auto f = camera->front();
		f.y = 0;
		f.normalize();
		if (forward) velocity = velocity + f * speed;
		if (backward) velocity = velocity - f * speed;
		if (left) velocity = velocity - r * speed;
		if (right) velocity = velocity + r * speed;
		if ((grounded || !gravity) && jump) velocity.y = 6;
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
		if (rclick && hasBlock) {
			auto fl = floor(prevBlockPos);
			auto ch = getChunkPos(fl.x, fl.y, fl.z);
			auto chunk = getChunk(ch.x, ch.y, ch.z);
			chunk->setBlockAt(fl.x - ch.x*chunkSize, fl.y - ch.y*chunkSize, fl.z - ch.z*chunkSize, BlockType::DIRT);
		}

		auto qp = floor(position);
		auto cp = getChunkPos(qp.x, qp.y, qp.z);

		// generate missing chunks
		std::vector<Chunk*> newChunks;
		for (int y = -5; y < 6; ++y) {
			for (int x = -5; x < 6; ++x) {
				for (int z = -5; z < 6; ++z) {
					int ix = cp.x + x;
					int iy = cp.y + y;
					int iz = cp.z + z;
					if (getChunk(ix, iy, iz)) continue;
					auto chunk = new Chunk(ix, iy, iz);
					chunk->generateBlocks(&gen);
					chunks.push_back(chunk);
					break;
				}
			}
		}

		// remove chunks that are too far away
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

		// generate chunk meshes
		auto myChunk = getChunk(position);
		if (myChunk && myChunk->isDirty) {
			myChunk->generateModel();
			if (myChunk->isNew) {
				models.push_back(myChunk->model);
				models.push_back(myChunk->waterModel);
				myChunk->isNew = false;
			}
		}

		std::sort(chunks.begin(), chunks.end(), [&](Chunk* a, Chunk* b) {
			auto d1 = (Vector3(a->gridx*chunkSize, a->gridy*chunkSize, a->gridz*chunkSize) - camera->position).lengthSq();
			auto d2 = (Vector3(b->gridx*chunkSize, b->gridy*chunkSize, b->gridz*chunkSize) - camera->position).lengthSq();
			if (a->isNew) d1 *= 0.01f;
			if (b->isNew) d2 *= 0.01f;
			return d1 < d2;
		});
		for (auto& chunk : chunks) {
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

		for (auto& cur : chunks) {
			updateChunk(cur);

			if (debugInfo) {
				Vector3 min = Vector3(cur->gridx*chunkSize, cur->gridy*chunkSize, cur->gridz*chunkSize);
				Vector3 max = min + Vector3(chunkSize, chunkSize, chunkSize);
				Vector4 col(1, 1, 1, 0.5);
				if (cur->isNew) col = Vector4(0, 0, 1, 0.5);
				else if (!cur->liveBlocks.empty()) col = Vector4(1, 0, 0, 0.5);
				GLDebug::aabb(min, max, col);
			}
		}

		grounded = false;
		auto floorBlock = getBlockAt(qp.x, qp.y, qp.z);
		if (gravity) {
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
		}

		auto headBlock = getBlockAt(camera->position);
		if (headBlock == BlockType::WATER) {
			fogColor = Vector3(0.35, 0.35, 0.55);
			fogStart = 1;
		}
		else {
			fogColor = Vector3(0.9, 0.9, 1);
			fogStart = 25;
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
		sstr << "Num Tris: " << numTris << "\n";

		label->text = sstr.str();
		label->position = Vector2(-gui->camera->width / 2, gui->camera->height / 2);


		meter->position = Vector2(-gui->root->size.x / 2, -gui->root->size.y / 2);

		// camera
		camera->position = position + Vector3::up * 1.5f;
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

		numTris = 0;
		// models
		for (auto& model : models) {
			model->fade -= dt;
			if (model->fade < 0) model->fade = 0;
			if (model->mesh->numVertices == 0) continue;

			model->material->use();

			model->material->program->setUniform("view", view);
			model->material->program->setUniform("projection", projection);
			model->material->program->setUniform("eyePos", camera->position);
			model->material->program->setUniform("lightPos", lightPos);
			model->material->program->setUniform("lightColor", lightColor);
			model->material->program->setUniform("fogColor", fogColor);
			model->material->program->setUniform("fogStart", fogStart);
			model->material->program->setUniform("fade", model->fade);
			model->material->program->setUniform("time", (float)glfwGetTime());

			auto world = Matrix4x4(model->rotation);
			world = world * matrixTranslation(model->position);

			model->material->program->setUniform("world", world);
			model->material->program->setUniform("worldView", view * world);
			model->material->program->setUniform("worldViewProjection", projection * view * world);

			model->mesh->draw();
			numTris += model->mesh->numIndices / 3;
		}

		GLDebug::reset();


		gl.clearDepth(1.0);

		gui->camera->width = gl.width;
		gui->camera->height = gl.height;
		gui->root->size.x = gui->camera->width;
		gui->root->size.y = gui->camera->height;

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
	static bool oldlmousedown;
	static bool oldrmousedown;
	forward = backward = left = right = click = rclick = jump = false;

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
		position = Vector3(0.5f + rand() % 65536, 100, 0.5f + rand() % 65536);
	}
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
		debugInfo = !debugInfo;
	}
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
		gravity = !gravity;
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
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) && !oldlmousedown) {
		click = true;
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) && !oldrmousedown) {
		rclick = true;
	}
	oldlmousedown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1);
	oldrmousedown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	gl.width = width;
	gl.height = height;
	gl.viewport(0, 0, width, height);
	camera->width = width;
	camera->height = height;
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
