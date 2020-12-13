#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>


class Shader {
public:
    unsigned int identificator;

public:
    Shader();

    int LoadAndCompile(const std::string path, int type);
    void AttachToProgram(unsigned program_id);
    void Delete();

private:
    void CheckCompileErrors(unsigned int identificator, int type);
};

class ShaderProgram {
public:
    unsigned int identificator;
    Shader vertex_shader;
    Shader fragment_shader;

public:
    ShaderProgram();

    void Build(const std::string path_to_vertex_shader,
               const std::string path_to_fragment_shader);
    void Use();

    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;

    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec2(const std::string& name, float x, float y) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setVec4(const std::string& name, float x, float y, float z, float w) const;

    void setMat2(const std::string& name, const glm::mat2& mat) const;
    void setMat3(const std::string& name, const glm::mat3& mat) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;

private:
    void CheckLinkErrors(unsigned int shader);
};
#endif