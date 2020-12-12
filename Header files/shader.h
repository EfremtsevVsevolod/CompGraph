#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

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

public:
    ShaderProgram();

    void Build(const std::string path_to_vertex_shader,
               const std::string path_to_fragment_shader);
    void Use();

    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;

private:
    void CheckLinkErrors(unsigned int shader);
};
#endif