#include "../libs/Texture.h"


void Texture2D::LoadTexture(const std::string& texture_file_path, const std::string& type_texture, bool alpha) {
    GLuint tmp_texture_id;
    glGenTextures(1, &tmp_texture_id);

    texture_id = tmp_texture_id;
    type = type_texture;

    glBindTexture(GL_TEXTURE_2D, *texture_id); 

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLint width, height, nr_channels;
    GLboolean *texture = stbi_load(texture_file_path.c_str(), &width, &height, &nr_channels, 0);

    if (texture) {
        if (alpha) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
    } else {
        std::cerr << "ERROR::TEXTURE::TEXTURE_LOADING_FAILED" << std::endl;
    }

    stbi_image_free(texture);
}

void Texture2D::GenShadowTexture(GLuint width, GLuint height) {
    GLuint tmp_texture_id;
    glGenTextures(1, &tmp_texture_id);

    texture_id = tmp_texture_id;
    type = SHADOW_MAP;

    glBindTexture(GL_TEXTURE_2D, *texture_id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void Texture2D::UseTexture(const ShaderProgram& shader_program, const std::string name, GLuint idx) const  {
    glActiveTexture(GL_TEXTURE0 + idx);
    glBindTexture(GL_TEXTURE_2D, *texture_id);
    if (texture_params) {
        shader_program.SetFloat(name + "." + std::string(TextureParams::FLARE), texture_params->flare);
        shader_program.SetFloat(name + "." + std::string(TextureParams::DIFF_COEF), texture_params->diff_coef);
        shader_program.SetFloat(name + "." + std::string(TextureParams::HEIGHT_COEF), texture_params->height_coef);
    }
}


void CreateTexture2DUnion(const std::vector<Texture2D> &texture_array, 
                          const std::vector <ShaderProgram>& shader_programs,
                          const std::vector <std::string> names) {
    for (size_t idx = 0; idx < texture_array.size(); ++idx) {
        texture_array[idx].UseTexture(shader_programs[idx], names[idx], idx);
    }
}

void BindShadowTexture(const Texture2D& shadow_texture, GLuint FBO_id) {
    if (shadow_texture.type != Texture2D::SHADOW_MAP) {
        std::cerr << "ERROR::TEXTURE::TEXTURE_IS_NOT_SHADOW" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, FBO_id);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, *shadow_texture.texture_id, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void TextureCube::LoadTexture(const std::string&, const std::string&, bool) {};

void TextureCube::UseTextureForShadowRendering() const {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, *texture_id);
}

void TextureCube::UseTexture(const ShaderProgram& shader_program, const std::string name, GLuint idx) const {
    glActiveTexture(GL_TEXTURE0 + idx);
    glBindTexture(GL_TEXTURE_CUBE_MAP, *texture_id);
    std::stringstream name_texture;
    name_texture << name;
    shader_program.SetInt(name_texture.str(), idx);
}

void TextureCube::GenShadowTexture(GLuint width, GLuint height) {
    GLuint tmp_texture_id;
    glGenTextures(1, &tmp_texture_id);

    texture_id = tmp_texture_id;
    type = SHADOW_CUBE_MAP;

    glBindTexture(GL_TEXTURE_CUBE_MAP, *texture_id);

    for (size_t idx = 0; idx < 6; ++idx) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + idx, 0, GL_DEPTH_COMPONENT, width, 
                     height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void BindShadowCubeTexture(const TextureCube& shadow_texture, GLuint FBO_id) {
    if (shadow_texture.type != TextureCube::SHADOW_CUBE_MAP) {
        std::cerr << "ERROR::TEXTURE::TEXTURE_IS_NOT_SHADOW" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, FBO_id);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, *shadow_texture.texture_id, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}