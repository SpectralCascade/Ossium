#ifndef MESH_H
#define MESH_H

#include "schemamodel.h"
#include "coremaths.h"

namespace Ossium
{
    
    // Represents indices to the Mesh vertex, UV and normals for defining a face; same as the standard .OBJ format.
    // See https://en.wikipedia.org/wiki/Wavefront_.obj_file#Face_elements
    // Yes, this means that these indices count from 1, NOT from 0.
    struct MeshFaceElement
    {
        MeshFaceElement() = default;
        MeshFaceElement(Uint16 vert, Uint16 uv = 0, Uint16 norm = 0) : vert(vert), uv(uv), norm(norm) {}
        
        // Convert this face element to a .OBJ format string.
        std::string ToString();

        // Construct this face element from a .OBJ format face element string.
        void FromString(const std::string& data);

        // Index to the 3D position vertex.
        Uint16 vert;

        // Index to the texture coordinate.
        Uint16 uv;

        // Index to the normal vector.
        Uint16 norm;

    };

    struct MaterialElement
    {
        Uint16 face;
    };

    // A 3D mesh. Can be loaded from a .OBJ file. Does not support line elements, only faces.
    class Mesh : public Resource
    {
    public:
        DECLARE_RESOURCE(Mesh);

        // Load from a .OBJ file.
        bool Load(std::string guid_path, ResourceController* resources);

        // Init materials and prepare buffers for use on the GPU.
        bool Init();

        // Load Mesh resource from a .OBJ file, then load materials and prepare buffers for use on the GPU.
        bool LoadAndInit(std::string guid_path, ResourceController* resources);

        // Render the mesh.
        void Render(RenderInput* pass, const Matrix<4, 4>& view, const Matrix<4, 4>& proj);

        // Vertices of the mesh in 3D space.
        std::vector<Vector3> vertices;

        // UV texture coordinates in the normalised range 0 to 1.
        std::vector<Vector2> texcoords;

        // Vertex normals.
        std::vector<Vector3> normals;

        // Indices to the vertex elements, for building the faces of the 3D mesh on the GPU.
        // A face may have any number of elements, but will always be split into triangles behind the scenes for the GPU.
        std::vector<std::vector<MeshFaceElement>> faces;

    private:
        std::string NextElement(const std::string& obj, unsigned int& i);
        
    };

}


#endif // MESH_H
