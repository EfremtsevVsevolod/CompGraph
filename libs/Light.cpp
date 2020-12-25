#include "../libs/Light.h"


LightPoint::LightPoint(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
    GLfloat attenuation_const, GLfloat attenuation_lin, GLfloat attenuation_quad)
    : position(position), ambient(ambient), diffuse(diffuse), specular(specular),
      attenuation_const(attenuation_const), attenuation_lin(attenuation_lin), attenuation_quad(attenuation_quad) {}

void LightPoint::UseLight(const ShaderProgram& shader_program, int idx) const {
    std::stringstream name;
    name << LIGHT << "[" << idx << "]" << ".";
    shader_program.SetVec3(name.str() + std::string(POSITION), position);

    shader_program.SetVec3(name.str() + std::string(AMBIDENT), ambient);
    shader_program.SetVec3(name.str() + std::string(DIFFUSE), diffuse);
    shader_program.SetVec3(name.str() + std::string(SPECULAR), specular);

    shader_program.SetFloat(name.str() + std::string(ATTENUATION_CONST), attenuation_const);
    shader_program.SetFloat(name.str() + std::string(ATTENUATION_LIN), attenuation_lin);
    shader_program.SetFloat(name.str() + std::string(ATTENUATION_QUAD), attenuation_quad);
}

glm::vec3 LightPoint::GetPosition() const {
    return position;
}


LightDirected::LightDirected(glm::vec3 position, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
    : position(position), direction(direction), ambient(ambient), diffuse(diffuse), specular(specular) {}

void LightDirected::UseLight(const ShaderProgram& shader_program) const {
    std::stringstream name;
    name << LIGHT << ".";
    shader_program.SetVec3(name.str() + std::string(POSITION), position);

    shader_program.SetVec3(name.str() + std::string(DIRECTION), direction);

    shader_program.SetVec3(name.str() + std::string(AMBIDENT), ambient);
    shader_program.SetVec3(name.str() + std::string(DIFFUSE), diffuse);
    shader_program.SetVec3(name.str() + std::string(SPECULAR), specular);
}

glm::vec3 LightDirected::GetPosition() const {
    return position;
}

glm::vec3 LightDirected::GetDirection() const {
    return direction;
}