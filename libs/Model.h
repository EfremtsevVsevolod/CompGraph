#pragma once
#include <cstddef>
#include <iostream>
#include <vector>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "./Texture.h"
#include "./Shader.h"


//C compatible POD structure
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texture_position;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};


class Mesh {
public:
    Mesh(const std::vector<Vertex>& vertexes,
         const std::vector<Texture>& textures,
         bool volume = true)
        : vertexes(vertexes.begin(), vertexes.end()),
        textures(textures.begin(), textures.end()),
        volume(volume) {}

    void InitializeMesh() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertexes.size() * sizeof(Vertex), std::data(vertexes), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(0));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, texture_position)));

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, tangent)));

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, bitangent)));

        glBindVertexArray(0);
    }

    void DrawMesh(const ShaderProgram& shader_program) {
        int cnt_diffuse = 0;
        int cnt_specular = 0;
        int cnt_normal = 0;
        int cnt_depth = 0;

        for (int i = 0; i < textures.size(); ++i) {
            std::stringstream name_texture;

            if (textures[i].type == DIFFUSE) {
                name_texture << "diffuse_map[" << cnt_diffuse++ << "]";
            }
            else if (textures[i].type == SPECULAR) {
                name_texture << "specular_map[" << cnt_specular++ << "]";
            }
            else if (textures[i].type == NORMAL) {
                name_texture << "normal_map[" << cnt_normal++ << "]";
            }
            else {
                name_texture << "shadow_map[" << cnt_depth++ << "]";
            }
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, textures[i].texture);
            name_texture << ".texture_data";
            shader_program.SetInt(name_texture.str(), i);
        }

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertexes.size());
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);
    };

    bool IsVolume() const {
        return volume;
    };

public:
    std::vector<Vertex> vertexes;
    std::vector<Texture> textures;
    bool volume;

    GLuint VAO = 0, VBO = 0;
};


class FigurePosition {
public:
    static constexpr std::string_view FIGURE_POSITION = "figure_position";
    static constexpr std::string_view MODEL = "model";
    static constexpr std::string_view VIEW = "view";
    static constexpr std::string_view PROJECTION = "projection";

public:
    FigurePosition(glm::mat4 model, glm::mat4 view, glm::mat4 projection)
        : model(model), view(view), projection(projection) {}

    void UseFigurePosition(const ShaderProgram& shader_program) const {
        std::stringstream name;
        name << FIGURE_POSITION << ".";

        shader_program.SetMat4(name.str() + std::string(MODEL), model);
        shader_program.SetMat4(name.str() + std::string(VIEW), view);
        shader_program.SetMat4(name.str() + std::string(PROJECTION), projection);
    }

private:
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
};


struct Transform {
public:
    Transform(glm::vec3 translate = glm::vec3(0.0f), glm::vec3 scalev = glm::vec3(1.0f), GLfloat turn = 0.0f)
        : translate(translate), scale(scale), turn(turn) {}
public:
    glm::vec3 translate;
    glm::vec3 scale;
    GLfloat turn;
};
