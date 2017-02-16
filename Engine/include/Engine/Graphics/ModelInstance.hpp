/**
* @Author   Guillaume Labey
*/

#pragma once

# include <vector>
# include <Engine/Graphics/Model.hpp>
# include <Engine/Graphics/MeshInstance.hpp>

class ModelInstance
{
public:
    ModelInstance(Model* model);
    ModelInstance(const ModelInstance& modelInstance);
    ~ModelInstance();

    ModelInstance& operator=(const ModelInstance& modelInstance);

    void                        draw(const ShaderProgram& shaderProgram, const glm::vec4& color, const glm::mat4 transform);
    const Model*                getModel() const;
    Model*                      getModel();
    std::vector<std::unique_ptr<MeshInstance> >&  getMeshsInstances();

private:
    Model*                      _model;
    std::vector<std::unique_ptr<MeshInstance> >   _meshsInstances;
};
