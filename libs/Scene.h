#pragma once
#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Model.h"


/* Производит примитивы Vertex для объекта из переданных данных */
class ObjectCreater {
public:
    ObjectCreater(const std::vector<glm::vec3>&, const std::vector<glm::vec3>&, const std::vector<glm::vec2>&, 
                  const std::vector<GLuint>&, const std::vector<GLuint>&, const std::vector<GLuint>&, size_t);
    std::vector<Vertex> CreateObject() const;

private:
    static std::pair<glm::vec3, glm::vec3> GetBiTangent(const std::vector<glm::vec3>&, const glm::vec3, const std::vector<glm::vec2>&);

private:
    std::vector<glm::vec3> vertexes_coordinates;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texture_coordinates;

    std::vector<GLuint> includes_vertexes_coords;
    std::vector<GLuint> includes_normals;
    std::vector<GLuint> includes_texture_coords;

    size_t cycle;
};

ObjectCreater::ObjectCreater(const std::vector<glm::vec3>& vertexes_coordinates, const std::vector<glm::vec3>& normals,
    const std::vector<glm::vec2>& texture_coordinates, const std::vector<GLuint>& includes_vertexes_coords,
    const std::vector<GLuint>& includes_normals, const std::vector<GLuint>& includes_texture_coords, size_t cycle)
    : vertexes_coordinates(vertexes_coordinates.begin(), vertexes_coordinates.end()), normals(normals.begin(), normals.end()),
    texture_coordinates(texture_coordinates.begin(), texture_coordinates.end()),
    includes_vertexes_coords(includes_vertexes_coords.begin(), includes_vertexes_coords.end()),
    includes_normals(includes_normals.begin(), includes_normals.end()),
    includes_texture_coords(includes_texture_coords.begin(), includes_texture_coords.end()),
    cycle(cycle) {}

std::vector<Vertex> ObjectCreater::CreateObject() const {
    if (includes_normals.size() != includes_vertexes_coords.size() &&
        includes_normals.size() != includes_texture_coords.size() &&
        includes_normals.size() % 3 != 0 && vertexes_coordinates.size() % 3 != 0 &&
        normals.size() % 3 != 0 && texture_coordinates.size() % 3 != 0) {
        std::cerr << "ERROR::OBJECT_CREATER::UNKNOWN_INDEXING_DATA" << std::endl;
        return std::vector<Vertex>();
    }

    std::vector<Vertex> vertexes_object;

    size_t size_batch_vertex_coords = vertexes_coordinates.size() / cycle;
    size_t size_batch_norms = normals.size() / cycle;
    size_t size_batch_texture_coords = texture_coordinates.size() / cycle;

    for (size_t idx = 0; idx < cycle; ++idx) {
        for (size_t jdx = 0; jdx < includes_normals.size() / 3; ++jdx) {
            std::vector<glm::vec3> cur_vertex_coords{
                vertexes_coordinates[size_batch_vertex_coords * idx + includes_vertexes_coords[3 * jdx]],
                vertexes_coordinates[size_batch_vertex_coords * idx + includes_vertexes_coords[3 * jdx + 1]],
                vertexes_coordinates[size_batch_vertex_coords * idx + includes_vertexes_coords[3 * jdx + 2]]
            };
            glm::vec3 normal = normals[size_batch_norms * idx + includes_normals[jdx]];
            std::vector<glm::vec2> cur_texture_coords{
                texture_coordinates[size_batch_texture_coords * idx + includes_texture_coords[3 * jdx]],
                texture_coordinates[size_batch_texture_coords * idx + includes_texture_coords[3 * jdx + 1]],
                texture_coordinates[size_batch_texture_coords * idx + includes_texture_coords[3 * jdx + 2]]
            };

            auto [tangent, bitangent] = GetBiTangent(cur_vertex_coords, normal, cur_texture_coords);

            for (size_t kdx = 0; kdx < 3; ++kdx) {
                Vertex vertex;
                vertex.position = cur_vertex_coords[kdx];
                vertex.normal = normal;
                vertex.texture_position = cur_texture_coords[kdx];
                vertex.tangent = tangent;
                vertex.bitangent = bitangent;

                vertexes_object.push_back(vertex);
            }
        }
    }

    return vertexes_object;
}

std::pair<glm::vec3, glm::vec3> ObjectCreater::GetBiTangent(const std::vector<glm::vec3>& vertex_coords,
    const glm::vec3 normal,
    const std::vector<glm::vec2>& texture_coords) {
    glm::vec3 side_first = vertex_coords[1] - vertex_coords[0];
    glm::vec3 side_second = vertex_coords[2] - vertex_coords[0];

    glm::vec2 tex_side_first = texture_coords[1] - texture_coords[0];
    glm::vec2 tex_side_second = texture_coords[2] - texture_coords[0];

    GLfloat norm = 1.0f / (tex_side_first.x * tex_side_second.y - tex_side_first.y * tex_side_second.x);

    glm::vec3 tangent;
    tangent.x = norm * (tex_side_second.y * side_first.x - tex_side_first.y * side_second.x);
    tangent.y = norm * (tex_side_second.y * side_first.y - tex_side_first.y * side_second.y);
    tangent.z = norm * (tex_side_second.y * side_first.z - tex_side_first.y * side_second.z);

    glm::vec3 bitangent;
    bitangent.x = norm * (tex_side_first.x * side_second.x - tex_side_second.x * side_first.x);
    bitangent.y = norm * (tex_side_first.x * side_second.y - tex_side_second.x * side_first.y);
    bitangent.z = norm * (tex_side_first.x * side_second.z - tex_side_second.x * side_first.z);

    return { tangent, bitangent };
}


