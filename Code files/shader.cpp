#include "../Header files/shader.h"


Shader::Shader() = default;

int Shader::LoadAndCompile(const std::string path, int type) {
    std::string code;
    std::ifstream file;

    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        file.open(path);
        std::stringstream stream;
        stream << file.rdbuf();
        file.close();
        code = stream.str();
    }
    catch (std::ifstream::failure& error) {
        std::cout << "Error in loading shader code:" << error.code() << std::endl;
        return -1;
    }

    const char* raw_code = code.c_str();

    identificator = glCreateShader(type);

    glShaderSource(identificator, 1, &raw_code, NULL);
    glCompileShader(identificator);
    CheckCompileErrors(identificator, type);

    return 0;
}

void Shader::AttachToProgram(unsigned program_id) {
    glAttachShader(program_id, identificator);
}

void Shader::Delete() {
    glDeleteShader(identificator);
}

void Shader::CheckCompileErrors(unsigned int identificator, int type) {
    int success_flag;
    char infomation_log[1024];

    glGetShaderiv(identificator, GL_COMPILE_STATUS, &success_flag);
    if (!success_flag) {
        glGetShaderInfoLog(identificator, 1024, NULL, infomation_log);
        if (type == GL_VERTEX_SHADER) {
            std::cout << "Error in compile (type: GL_VERTEX_SHADER)" << std::endl
                << infomation_log << std::endl;
        }
        else if (type == GL_FRAGMENT_SHADER) {
            std::cout << "Error in compile (type: GL_FRAGMENT_SHADER)" << std::endl
                << infomation_log << std::endl;
        }
    }
}



ShaderProgram::ShaderProgram() = default;

void ShaderProgram::Build(const std::string path_to_vertex_shader,
                          const std::string path_to_fragment_shader) {
    vertex_shader.LoadAndCompile(path_to_vertex_shader, GL_VERTEX_SHADER);
    fragment_shader.LoadAndCompile(path_to_fragment_shader, GL_FRAGMENT_SHADER);

    identificator = glCreateProgram();

    vertex_shader.AttachToProgram(identificator);
    fragment_shader.AttachToProgram(identificator);
    glLinkProgram(identificator);
    CheckLinkErrors(identificator);

    vertex_shader.Delete();
    fragment_shader.Delete();
}

void ShaderProgram::Use() {
    glUseProgram(identificator);
}

void ShaderProgram::setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(identificator, name.c_str()), (int)value);
}

void ShaderProgram::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(identificator, name.c_str()), value);
}

void ShaderProgram::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(identificator, name.c_str()), value);
}

void ShaderProgram::setVec2(const std::string& name, const glm::vec2& value) const {
    glUniform2fv(glGetUniformLocation(identificator, name.c_str()), 1, &value[0]);
}

void ShaderProgram::setVec2(const std::string& name, float x, float y) const {
    glUniform2f(glGetUniformLocation(identificator, name.c_str()), x, y);
}

void ShaderProgram::setVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(identificator, name.c_str()), 1, &value[0]);
}

void ShaderProgram::setVec3(const std::string& name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(identificator, name.c_str()), x, y, z);
}

void ShaderProgram::setVec4(const std::string& name, const glm::vec4& value) const {
    glUniform4fv(glGetUniformLocation(identificator, name.c_str()), 1, &value[0]);
}

void ShaderProgram::setVec4(const std::string& name, float x, float y, float z, float w) const {
    glUniform4f(glGetUniformLocation(identificator, name.c_str()), x, y, z, w);
}

void ShaderProgram::setMat2(const std::string& name, const glm::mat2& mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(identificator, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void ShaderProgram::setMat3(const std::string& name, const glm::mat3& mat) const {
    glUniformMatrix3fv(glGetUniformLocation(identificator, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void ShaderProgram::setMat4(const std::string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(identificator, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}


void ShaderProgram::CheckLinkErrors(unsigned int shader) {
    int success_flag;
    char information_log[1024];

    glGetProgramiv(shader, GL_LINK_STATUS, &success_flag);
    if (!success_flag) {
        glGetProgramInfoLog(shader, 1024, NULL, information_log);
        std::cout << "Error in linking: " << std::endl << information_log << std::endl;
    }

}
