#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "libs/Texture.h"
#include "libs/Shader.h"
#include "libs/Model.h"
#include "libs/Light.h"
#include "libs/Scene.h"
#include "libs/Camera.h"
#include "libs/Shader.h"



void InitGLFW() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
}


void GLADLoader() {
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "ERROR::GLAD::FAILED_TO_INITIALIZE_GLAD" << std::endl;
		exit(-1);
	}
}


struct WindowContext {
public:
	WindowContext() = delete;

	~WindowContext() = delete;

	WindowContext(const WindowContext&) = delete;

	WindowContext& operator=(const WindowContext&) = delete;

	static void FramebufferSizeCallback(GLFWwindow *window, GLint width, GLint height) {
		glViewport(0, 0, width, height);
	}

	static void CursorPosCallback(GLFWwindow *window, GLdouble x_pos, GLdouble y_pos) {
		if (!last_x || !last_y) {
			last_x = x_pos;
			last_y = y_pos;
		}
		GLfloat x_offset = static_cast<GLfloat>(x_pos) - *last_x;
		GLfloat y_offset = *last_y - static_cast<GLfloat>(y_pos);

		last_x = x_pos;
		last_y = y_pos;

		camera->ProcessMouseMovement(x_offset, y_offset);
	}

	static void ScrollCallback(GLFWwindow *window, GLdouble x_offset, GLdouble y_offset) {
		camera->ProcessMouseScroll(static_cast<GLfloat>(y_offset));
	}

	static void InitializeWindowContext(Camera& camera) {
		WindowContext::camera = &camera;
	}

public:
	static Camera* camera;
	static std::optional<GLfloat> last_x;
	static std::optional<GLfloat> last_y;
};

Camera* WindowContext::camera = nullptr;
std::optional<GLfloat> WindowContext::last_x = std::nullopt;
std::optional<GLfloat> WindowContext::last_y = std::nullopt;


enum class SwitchRender {
	SCENE,
	SHADOW_MAP,
	SHADOW_CUBE
};


void Render(const Camera& camera, GLfloat scr_wight, GLfloat scr_height,
	std::vector<Mesh>& objects, Texture& shadow_texture,
	std::vector<ShaderProgram>& shader_programs,
	std::vector<LightPoint>& lights_point, LightDirected& light_directed,
	std::vector<Transform>& transforms, SwitchRender switch_render = SwitchRender::SCENE) {

	static constexpr GLuint SHADOW_MAP_POSITION = 5;
	static constexpr GLuint SHADOW_CUBE_MAP_POSITION = 6;

	static glm::mat4 light_space;
	static std::vector<std::vector<glm::mat4>> shadow_transforms(lights_point.size());

	static glm::mat4 model;
	static glm::mat4 view;
	static glm::mat4 projection;

	switch (switch_render) {
	case SwitchRender::SHADOW_MAP: {
		GLfloat near_plane = 1.0f, far_plane = 7.5f;
		glm::mat4 light_projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		glm::mat4 light_view = glm::lookAt(light_directed.GetPosition(), light_directed.GetDirection(),
			                               glm::vec3(0.0f, 1.0f, 0.0f));
		light_space = light_projection * light_view;

		shader_programs[0].UseShaderProgram();
		shader_programs[0].SetMat4("light_space", light_space);
		glActiveTexture(GL_TEXTURE0);

		for (size_t idx = 0; idx < objects.size(); ++idx) {
			if (objects[idx].IsVolume()) {
				glCullFace(GL_FRONT);
			}
			model = glm::mat4(1.0f);
			model = glm::translate(model, transforms[idx].translate);
			model = glm::rotate(model, glm::radians(transforms[idx].turn), glm::vec3(1.0f));
			model = glm::scale(model, transforms[idx].scale);

			FigurePosition figure_position{ model, view, projection };
			figure_position.UseFigurePosition(shader_programs[0]);

			objects[idx].DrawMesh(shader_programs[0]);
			glCullFace(GL_BACK);
		}
		break;
	}
	case SwitchRender::SHADOW_CUBE: {
		GLfloat coef_resolution = static_cast<GLfloat>(scr_wight) / scr_height;
		GLfloat near_plane = 1.0f, far_plane = 25.0f;
		glm::mat4 shadow_projection = glm::perspective(glm::radians(90.0f), coef_resolution, near_plane, far_plane);
		for (size_t idx = 0; idx < lights_point.size(); ++idx) {
			shadow_transforms[idx].clear();
			shadow_transforms[idx].emplace_back(shadow_projection * glm::lookAt(lights_point[idx].GetPosition(),
				lights_point[idx].GetPosition() + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
			shadow_transforms[idx].emplace_back(shadow_projection * glm::lookAt(lights_point[idx].GetPosition(),
				lights_point[idx].GetPosition() + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
			shadow_transforms[idx].emplace_back(shadow_projection * glm::lookAt(lights_point[idx].GetPosition(),
				lights_point[idx].GetPosition() + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
			shadow_transforms[idx].emplace_back(shadow_projection * glm::lookAt(lights_point[idx].GetPosition(),
				lights_point[idx].GetPosition() + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
			shadow_transforms[idx].emplace_back(shadow_projection * glm::lookAt(lights_point[idx].GetPosition(),
				lights_point[idx].GetPosition() + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
			shadow_transforms[idx].emplace_back(shadow_projection * glm::lookAt(lights_point[idx].GetPosition(),
				lights_point[idx].GetPosition() + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
			shader_programs[0].UseShaderProgram();
			for (size_t jdx = 0; jdx < shadow_transforms[idx].size(); ++jdx) {
				std::stringstream name;
				name << "shadow_view[" << jdx << "]";
				shader_programs[0].SetMat4(name.str(), shadow_transforms[idx][jdx]);
			}
			shader_programs[0].SetFloat("far_plane", far_plane);
			shader_programs[0].SetVec3("light_position", lights_point[idx].GetPosition());
			//shadow_cube[idx].UseTextureForShadowRendering();
			for (size_t jdx = 0; jdx < objects.size(); ++jdx) {
				model = glm::mat4(1.0f);
				model = glm::translate(model, transforms[jdx].translate);
				model = glm::rotate(model, glm::radians(transforms[jdx].turn), glm::vec3(1.0f));
				model = glm::scale(model, transforms[jdx].scale);

				FigurePosition figure_position{ model, view, projection };
				figure_position.UseFigurePosition(shader_programs[0]);

				objects[jdx].DrawMesh(shader_programs[0]);
			}
		}
		break;
	}
	case SwitchRender::SCENE: {
		projection = glm::perspective(glm::radians(camera.GetZoom()), scr_wight / scr_height, 0.1f, 100.0f);
		view = camera.GetViewMatrix();

		for (size_t idx = 0; idx < objects.size(); ++idx) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, transforms[idx].translate);
			model = glm::rotate(model, glm::radians(transforms[idx].turn), glm::vec3(1.0f));
			model = glm::scale(model, transforms[idx].scale);

			shader_programs[idx].UseShaderProgram();
			shader_programs[idx].SetMat4("light_space", light_space);

			glActiveTexture(GL_TEXTURE0 + SHADOW_MAP_POSITION);
			glBindTexture(GL_TEXTURE_2D, shadow_texture.texture);
			shader_programs[idx].SetInt("shadow_map", SHADOW_MAP_POSITION);

			FigurePosition figure_position{ model, view, projection };
			figure_position.UseFigurePosition(shader_programs[idx]);
			shader_programs[idx].SetVec3("view_position", camera.GetPosition());

			for (size_t jdx = 0; jdx < lights_point.size(); ++jdx) {
				lights_point[jdx].UseLight(shader_programs[idx], jdx);
				shader_programs[idx].SetFloat("far_plane", 25.0f);
			}

			light_directed.UseLight(shader_programs[idx]);

			objects[idx].DrawMesh(shader_programs[idx]);
		}
		break;
	}
	default:
		std::cerr << "ERROR::RENDER::UNKNOWN_RENDER_TYPE" << std::endl;
		break;
	}
}


class Window {
private:
	static constexpr GLuint SCR_WIDTH = 1200;
	static constexpr GLuint SCR_HEIGHT = 800;
	static constexpr GLuint SHADOW_MAP_WIDTH = 4096;
	static constexpr GLuint SHADOW_MAP_HEIGHT = 4069;
	static constexpr GLuint SHADOW_CUBE_MAP_WIDTH = 2048;
	static constexpr GLuint SHADOW_CUBE_MAP_HEIGHT = 2048;

private:
	GLFWwindow *window = nullptr;
	Camera camera;

	GLfloat delta_time = 0.0f;
	GLfloat last_frame = 0.0f;

public:
	Window() = default;

	void Initialize(const std::string& title) {
		window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, title.c_str(), nullptr, nullptr);
		if (window == nullptr) {
			std::cerr << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			exit(-1);
		}
		WindowContext::InitializeWindowContext(camera);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwMakeContextCurrent(window);
		glfwSetFramebufferSizeCallback(window, WindowContext::FramebufferSizeCallback);
		glfwSetCursorPosCallback(window, WindowContext::CursorPosCallback);
		glfwSetScrollCallback(window, WindowContext::ScrollCallback);
	}


	void Rendering() {
		/*std::vector<ShaderLoadInfo> shareds_cube_info =
			{ {"./scr/Shaders/ModelVertexShader.hlsl", GL_VERTEX_SHADER},
			  {"./scr/Shaders/ModelFragmentShader.hlsl", GL_FRAGMENT_SHADER} };

		std::vector<ShaderLoadInfo> shareds_light_info =
			{ {"./scr/Shaders/LightVertexShader.hlsl", GL_VERTEX_SHADER},
			  {"./scr/Shaders/LightFragmentShader.hlsl", GL_FRAGMENT_SHADER} };

		std::vector<ShaderLoadInfo> shareds_floor_info =
			{ {"./scr/Shaders/FloorVertexShader.hlsl", GL_VERTEX_SHADER},
			  {"./scr/Shaders/FloorFragmentShader.hlsl", GL_FRAGMENT_SHADER} };*/

		ShaderProgram shader_cube_program;
		shader_cube_program.Initialize({ "./scr/Shaders/ModelVertexShader.hlsl",
									     "./scr/Shaders/ModelFragmentShader.hlsl" },
									   { GL_VERTEX_SHADER , GL_FRAGMENT_SHADER });


		ShaderProgram shader_light_program;
		shader_light_program.Initialize({ "./scr/Shaders/LightVertexShader.hlsl",
										 "./scr/Shaders/LightFragmentShader.hlsl" },
						     		    { GL_VERTEX_SHADER , GL_FRAGMENT_SHADER });

		ShaderProgram shareds_floor_program;
		shareds_floor_program.Initialize({ "./scr/Shaders/FloorVertexShader.hlsl",
										 "./scr/Shaders/FloorFragmentShader.hlsl" },
			                             { GL_VERTEX_SHADER , GL_FRAGMENT_SHADER });


		std::vector<ShaderProgram> shaders_scene;
		//shaders_scene.push_back(shader_cube_program);
		//shaders_scene.push_back(shader_cube_program);
		//shaders_scene.push_back(shader_cube_program);
		//shaders_scene.push_back(shader_cube_program);
		//shaders_scene.push_back(shader_cube_program);
		//shaders_scene.push_back(shader_cube_program);
		//shaders_scene.push_back(shareds_floor_program);
		shaders_scene.push_back(shader_light_program);

		std::vector<glm::vec3> vertex_coords_cube {
			glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, -0.5f, -0.5f),
			glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(-0.5f,  0.5f, -0.5f),
			glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.5f, -0.5f,  0.5f),
			glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(-0.5f,  0.5f,  0.5f), 
			glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-0.5f,  0.5f, -0.5f),
			glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-0.5f, -0.5f,  0.5f),
			glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(0.5f,  0.5f, -0.5f),
			glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.5f, -0.5f,  0.5f),
			glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, -0.5f, -0.5f),
			glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(-0.5f, -0.5f,  0.5f),
			glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.5f,  0.5f, -0.5f),
			glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(-0.5f,  0.5f,  0.5f)
		};

		std::vector<glm::vec3> normal_cube {
			glm::vec3(0.0f, 0.0f, -1.0f),
			glm::vec3(0.0f,  0.0f, 1.0f),
			glm::vec3(-1.0f,  0.0f,  0.0f), 
			glm::vec3(1.0f,  0.0f,  0.0f),
			glm::vec3(0.0f, -1.0f,  0.0f),
			glm::vec3(0.0f,  1.0f,  0.0f)
		};

		std::vector<glm::vec2> tex_coords_cube {
			glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f),
			glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f),
			glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f),
			glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f),
			glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f),
			glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f),
			glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f),
			glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f),
			glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f),
			glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f)
		};

		std::vector<glm::vec2> tex_coords_column {
			glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 3.0f), glm::vec2(0.0f, 3.0f),
			glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 3.0f), glm::vec2(0.0f, 3.0f),
			glm::vec2(0.0f, 3.0f), glm::vec2(1.0f, 3.0f),
			glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f),
			glm::vec2(0.0f, 3.0f), glm::vec2(1.0f, 3.0f),
			glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f),
			glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f),
			glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f),
			glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f),
			glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f)
		};

		std::vector<GLuint> indexes_vertex_coords_cube = {
			0, 1, 2,
			2, 3, 0
		};

		std::vector<GLuint> indexes_norm_cube = {
			0, 0, 0,
			0, 0, 0
		};

		std::vector<GLuint> indexes_tex_coords_cube = {
			0, 1, 2,
			2, 3, 0
		};

		size_t cycle_cube = 6;


		ObjectCreater creater_cube { vertex_coords_cube, normal_cube, tex_coords_cube, 
			indexes_vertex_coords_cube, indexes_norm_cube, indexes_tex_coords_cube, cycle_cube };

		ObjectCreater creater_column { vertex_coords_cube, normal_cube, tex_coords_column,
			indexes_vertex_coords_cube, indexes_norm_cube, indexes_tex_coords_cube, cycle_cube };

		std::vector<Vertex> vertexes_cube = creater_cube.CreateObject();
		std::vector<Vertex> vertexes_column = creater_column.CreateObject();
		
		std::vector<glm::vec3> vertex_coords_floor {
			glm::vec3(7.0f, 0.0f, 7.0f), glm::vec3(-7.0f, 0.0f, 7.0f),
			glm::vec3(-7.0f, 0.0f, -7.0f), glm::vec3(7.0f, 0.0f, -7.0f)
		};


		std::vector<glm::vec3> normal_floor {
			glm::vec3(0.0f, 1.0f, 0.0f)
		};

		std::vector<glm::vec2> tex_coords_floor {
			glm::vec2(28.0f, 0.0f), glm::vec2(0.0f, 0.0f),
			glm::vec2(0.0f, 28.0f), glm::vec2(28.0f, 28.0f)
		};

		std::vector<GLuint> indexes_vertex_coords_floor {
			0, 1, 2,
			0, 2, 3
		};

		std::vector<GLuint> indexes_norm_floor {
			0, 0, 0,
			0, 0, 0
		};

		std::vector<GLuint> indexes_tex_coords_floor {
			0, 1, 2,
			0, 2, 3
		};

		size_t cycle_floor = 1;

		ObjectCreater creater_floor { vertex_coords_floor, normal_floor, tex_coords_floor,
			indexes_vertex_coords_floor, indexes_norm_floor, indexes_tex_coords_floor, cycle_floor };

		std::vector<Vertex> vertexes_floor = creater_floor.CreateObject();


		std::vector<Texture> textures_cube;
		textures_cube.push_back(LoadTexture("./textures/bricks2.jpg", DIFFUSE));
		textures_cube.push_back(LoadTexture("./textures/bricks2.jpg", SPECULAR));
		textures_cube.push_back(LoadTexture("./textures/bricks2_normal.jpg", NORMAL));
		textures_cube.push_back(LoadTexture("./textures/bricks2_disp.jpg", DEPTH));


		std::vector<Texture> textures_floor;
		textures_floor.push_back(LoadTexture("./textures/Stone_Wall_007_COLOR.jpg", DIFFUSE));
		textures_floor.push_back(LoadTexture("./textures/Stone_Wall_007_NORM.jpg", NORMAL));
		textures_floor.push_back(LoadTexture("./textures/Stone_Wall_007_DEPTH.png", DEPTH));


		//Mesh cube { vertexes_cube, textures_cube };
		//cube.InitializeMesh();

		//Mesh column { vertexes_column, textures_cube };
		//column.InitializeMesh();

		//Mesh floor { vertexes_floor, textures_floor, false };
		//floor.InitializeMesh();

		Mesh light { vertexes_cube, std::vector<Texture>{} };
		light.InitializeMesh();
	
		std::vector<Mesh> meshs_scene;
		//meshs_scene.push_back(cube);
		//meshs_scene.push_back(cube);
		//meshs_scene.push_back(column);
		//meshs_scene.push_back(column);
		//meshs_scene.push_back(column);
		//meshs_scene.push_back(column);
		//meshs_scene.push_back(floor);
		meshs_scene.push_back(light);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		glm::vec3 light_position(0.0f, 0.0f, 0.0f);

		LightPoint light_point_param{
			light_position,
			glm::vec3(0.05f, 0.05f, 0.05f),
			glm::vec3(0.4f, 0.4f, 0.4f),
			glm::vec3(0.5f, 0.5f, 0.5f),
			0.1, 0.09, 0.032 };

		LightDirected light_directed {
			glm::vec3(1.0f, 3.0f, 1.5f),
			glm::vec3(-0.2f, -1.0f, -0.3f),
			glm::vec3(0.05f, 0.05f, 0.05f),
			glm::vec3(0.9f, 0.9f, 0.9f),
			glm::vec3(0.5f, 0.5f, 0.5f)};

		std::vector<LightPoint> lights_point;
		lights_point.push_back(light_point_param);


		std::vector<Transform> transforms;
		//transforms.emplace_back(glm::vec3(0.1f, 0.0f, -1.0f));
		//transforms.emplace_back(glm::vec3(1.0f, -1.2f, -1.5f), glm::vec3(1.0f), 15);
		//transforms.emplace_back(glm::vec3(-1.5f, -0.498f, -1.5f), glm::vec3(1.0f, 3.0f, 1.0f));
		//transforms.emplace_back(glm::vec3(1.5f, -0.498f, -1.5f), glm::vec3(1.0f, 3.0f, 1.0f));
		//transforms.emplace_back(glm::vec3(-1.5f, -0.498f, 1.5f), glm::vec3(1.0f, 3.0f, 1.0f));
		//transforms.emplace_back(glm::vec3(1.5f, -0.498f, 1.5f), glm::vec3(1.0f, 3.0f, 1.0f));
		//transforms.emplace_back(glm::vec3(0.0f, -2.0f, 0.0f));
		transforms.emplace_back(light_position, glm::vec3(0.2f));


		//glm::vec3 floor_position(0.0f, -1.0f, 0.0f);
		//glm::vec3 cube1_position(2.0f, -0.995f, 5.0f);

		GLfloat flare_cube = 32;
		GLfloat flare_floor = 8;

		GLuint depth_FBO;
		glGenFramebuffers(1, &depth_FBO);

		ShaderProgram shader_shadow_program;

		shader_shadow_program.Initialize({ "./scr/Shaders/ShadowVertexShader.hlsl",
										   "./scr/Shaders/ShadowFragmentShader.hlsl" },
										{ GL_VERTEX_SHADER , GL_FRAGMENT_SHADER });

		std::vector<ShaderProgram> shaders_shadow{ shader_shadow_program };

		ShaderProgram shader_shadow_cube_program;

		shader_shadow_cube_program.Initialize({ "./scr/Shaders/ShadowVertexShader.hlsl",
												"./scr/Shaders/ShadowCubeGeometryShader.hlsl",
												"./scr/Shaders/ShadowCubeFragmentShader.hlsl" },
											  { GL_VERTEX_SHADER, GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER });

		std::vector<ShaderProgram> shaders_shadow_cube{ shader_shadow_cube_program };


		Texture depth_map = GenerateDepthTexture(SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);

		while (!glfwWindowShouldClose(window)) {
			GLfloat current_frame = glfwGetTime();
			delta_time = current_frame - last_frame;
			last_frame = current_frame;
			KeyboardInput();

			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


			//glViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);
			//glBindFramebuffer(GL_FRAMEBUFFER, depth_FBO);
			//glClear(GL_DEPTH_BUFFER_BIT);

			//Render(camera, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, meshs_scene, depth_map,
			//	   shaders_shadow, lights_point, light_directed,
			//	   transforms, SwitchRender::SHADOW_MAP);


			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			Render(camera, SCR_WIDTH, SCR_HEIGHT, meshs_scene, depth_map,
				   shaders_scene, lights_point, light_directed, transforms);

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
		glfwTerminate();
	}

	void KeyboardInput() {
		if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			camera.ProcessKeyboard(FORWARD, delta_time);
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			camera.ProcessKeyboard(BACKWARD, delta_time);
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			camera.ProcessKeyboard(LEFT, delta_time);
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			camera.ProcessKeyboard(RIGHT, delta_time);
		}
	}
};


int main(int argc, char **argv) {
	InitGLFW();
	Window window;
	window.Initialize("test");
	GLADLoader();
	window.Rendering();
	return 0;
}
