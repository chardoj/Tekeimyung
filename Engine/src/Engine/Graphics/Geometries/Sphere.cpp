#include <iostream>
#include <Engine/Graphics/Geometries/Sphere.hpp>

Sphere::Sphere(Sphere::sInfo& info): Geometry(Geometry::eType::SPHERE)
{
    float radius = SIZE_UNIT;
    uint32_t rings = static_cast<uint32_t>(SIZE_UNIT * 3.0f);
    uint32_t sectors = static_cast<uint32_t>(SIZE_UNIT * 3.0f);

    // Sphere mesh
    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();



    float const R = 1.0f / (float)(rings - 1);
    float const S = 1.0f / (float)(sectors - 1);

    for(uint32_t r = 0; r < rings; ++r)
    {
        for(uint32_t s = 0; s < sectors; ++s)
        {
            float const y = sin(-(glm::pi<float>() / 2.0f) + glm::pi<float>() * r * R);
            float const x = cos(2.0f * glm::pi<float>() * s * S) * sin(glm::pi<float>() * r * R);
            float const z = sin(2.0f * glm::pi<float>() * s * S) * sin(glm::pi<float>() * r * R);

            Vertex vertex;

            vertex.uv = glm::vec2(s * S, r * R);
            vertex.pos = glm::vec3(x, y, z) * radius;
            vertex.normal = glm::vec3(x, y, z);
            vertex.color = glm::vec3(0.0f, 0.0f, 0.0f);

            mesh->vertexs.push_back(vertex);



            int curRow = r * sectors;
            int nextRow = (r + 1) * sectors;

            mesh->indices.push_back(curRow + s);
            mesh->indices.push_back(nextRow + s);
            mesh->indices.push_back(nextRow + (s + 1));

            mesh->indices.push_back(curRow + s);
            mesh->indices.push_back(nextRow + (s + 1));
            mesh->indices.push_back(curRow + (s + 1));
        }
    }


    // Sphere material
    Material material;
    material._constants.ambient = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    material._constants.diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    mesh->material = material;

    // Add plane to meshs list
    _meshs.push_back(mesh);

    initVertexData();
    initIndexData();
    _buffer.updateData(_vertexData, getVertexsSize(), _indexData, getIndicesSize());
    calculateSize();
}

Sphere::~Sphere() {}
