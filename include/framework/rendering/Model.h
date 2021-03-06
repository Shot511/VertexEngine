#pragma once

#include <memory>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "Mesh.h"
#include "Material.h"
#include <entityx/Entity.h>
#include "Shader.h"

namespace Vertex
{
    class Model
    {
    public:
        Model();
        virtual ~Model();

        /* Primitives */
        void genCone    (float height = 3.0f, float r = 1.5f, unsigned int slices = 10, unsigned int stacks = 10);
        void genCube    (float radius = 1.0f);
        void genCylinder(float height = 3.0f, float radius = 1.5f, unsigned int slices = 10);
        void genPlane   (float width  = 1.0f, float height = 1.0f, unsigned int slices = 5, unsigned int stacks = 5);
        void genSphere  (float radius = 1.5f, unsigned int slices = 12);
        void genTorus   (float innerRadius = 1.0f, float outerRadius = 2.0f, unsigned int slices = 10, unsigned int stacks = 10);
        void genQuad    (float width = 1.0f, float height = 1.0f);

        void load(const std::string & filename);
        void render(Shader & shader);

        void setDrawMode(GLenum draw_mode);
        GLenum getDrawMode() { return getMesh(0).getDrawMode(); }

        Mesh & getMesh(unsigned int index = 0)
        {
            if (index > m_meshes.size())
            {
                index = m_meshes.size() - 1;
            }

            return m_meshes[index];
        }

        unsigned meshesCount() const { return m_meshes.size(); }

    private:
        void calcTangentSpace(VertexBuffers & buffers) const;
        void genPrimitive(VertexBuffers & buffers);

        void processNode(aiNode * node, const aiScene * scene, aiString & directory);
        Mesh processMesh(aiMesh * mesh, const aiScene * scene, aiString & directory) const;

        void loadMaterialTextures(Mesh & mesh, aiMaterial * mat, aiTextureType type, Material::TextureType texture_type, aiString & directory) const;

        std::vector<Mesh> m_meshes;
    };
}
