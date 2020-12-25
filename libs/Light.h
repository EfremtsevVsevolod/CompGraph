#pragma once
#include <sstream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Shader.h"


class LightPoint {
public:
    static constexpr std::string_view LIGHT = "light_point";

public:
    LightPoint(glm::vec3, glm::vec3, glm::vec3, glm::vec3, GLfloat, GLfloat, GLfloat);
    void UseLight(const ShaderProgram&, int) const;
    glm::vec3 GetPosition() const;

private:
    static constexpr std::string_view POSITION = "position";
    static constexpr std::string_view AMBIDENT = "ambient";
    static constexpr std::string_view DIFFUSE = "diffuse";
    static constexpr std::string_view SPECULAR = "specular";
    static constexpr std::string_view ATTENUATION_CONST = "attenuation_const";
    static constexpr std::string_view ATTENUATION_LIN = "attenuation_lin";
    static constexpr std::string_view ATTENUATION_QUAD = "attenuation_quad";

private:
    glm::vec3 position;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    GLfloat attenuation_const;
    GLfloat attenuation_lin;
    GLfloat attenuation_quad;
};


class LightDirected {
public:
    static constexpr std::string_view LIGHT = "light_directed";

public:
    LightDirected(glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3);
    void UseLight(const ShaderProgram&) const;
    glm::vec3 GetPosition() const;
    glm::vec3 GetDirection() const;

private:
    static constexpr std::string_view POSITION = "position";
    static constexpr std::string_view DIRECTION = "direction";
    static constexpr std::string_view AMBIDENT = "ambient";
    static constexpr std::string_view DIFFUSE = "diffuse";
    static constexpr std::string_view SPECULAR = "specular";

private:
    glm::vec3 position;
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};