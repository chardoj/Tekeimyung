/**
* @Author   Guillaume Labey
*/

#pragma once

# include <vector>
# include <assimp/scene.h>

# include <Engine/Graphics/Buffer.hpp>
# include <Engine/Graphics/Material.hpp>
# include <Engine/Graphics/Skeleton.hpp>

class Model;

class Mesh {
public:
    Mesh(Model* model);
    virtual ~Mesh();

    virtual bool                    loadFromAssimp(Skeleton& skeleton, aiMesh *mesh);
    Material*                       getMaterial() const;
    void                            setMaterial(Material* material);

    Model*                          getModel() const;


public:
    std::vector<Vertex>             vertexs;
    std::vector<GLuint>             indices;

    uint32_t                        offset;
    uint32_t                        idxOffset;

private:
    // Material
    Material*                       _material;
    Model*                          _model;

private:
    void                            loadBones(Skeleton& skeleton, aiMesh *mesh);
    void                            addVertexBonesInfos(uint32_t vertexId, float weight, uint32_t boneId);
};
