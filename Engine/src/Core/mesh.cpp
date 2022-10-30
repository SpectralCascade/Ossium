#include "resourcecontroller.h"
#include "mesh.h"
#include "file.h"

namespace Ossium
{

    REGISTER_RESOURCE(Mesh);
    
    bool Mesh::Load(std::string guid_path)
    {
        // Open the OBJ file
        File obj(guid_path);
        if (obj.HasError())
        {
            Log.Error(obj.GetError());
            return false;
        }

        // Read the very first element of the file
        std::string keyword = obj.ReadElement(' ');
        std::string element = "";

        Vector3 vert;
        Vector2 uv;
        Vector3 norm;
        while (!keyword.empty())
        {
            if (keyword == "v")
            {
                for (unsigned int i = 0; i < 3; i++)
                {
                    element = obj.ReadElement(' ');
                    DEBUG_ASSERT(Utilities::IsNumber(element), "OBJ must have exactly 3 elements per v; ReadElement() result = {0}", element);
                    vert(0, i) = Utilities::ToFloat(element);
                }
                vertices.push_back(vert);
            }
            else if (keyword == "vt")
            {
                for (unsigned int i = 0; i < 2; i++)
                {
                    element = obj.ReadElement(' ');
                    DEBUG_ASSERT(Utilities::IsNumber(element), "OBJ must have exactly 2 elements per vt; ReadElement() result = {0}", element);
                    uv(0, i) = Utilities::ToFloat(element);
                }
                texcoords.push_back(uv);
            }
            else if (keyword == "vn")
            {
                for (unsigned int i = 0; i < 3; i++)
                {
                    element = obj.ReadElement(' ');
                    DEBUG_ASSERT(Utilities::IsNumber(element), "OBJ must have exactly 3 elements per vn; ReadElement() result = {0}", element);
                    norm(0, i) = Utilities::ToFloat(element);
                }
                normals.push_back(norm);
            }
            else if (keyword == "f")
            {
                std::vector<MeshFaceElement> face;
                element = obj.ReadElement(' ');
                while (!element.empty() && Utilities::IsInt(element.substr(0, 1)))
                {
                    auto indices = Utilities::Split(element, '/');
                    Uint16 v = Utilities::ToInt(indices[0]);
                    Uint16 vt = Utilities::ToInt(indices[1]);
                    Uint16 vn = indices.size() > 2 ? Utilities::ToInt(indices[2]) : 0;
                    face.push_back(MeshFaceElement(v, vt, vn));
                    element = obj.ReadElement(' ');
                }
                faces.push_back(face);

                // Skip to next iteration as the next keyword has been read already
                keyword = element;
                continue;
            }
            else if (keyword == "usemtl")
            {
            }
            else if (keyword == "mtllib")
            {
                // Load material(s) used by the mesh
            }
            else
            {
                // Irrelevant data such as comments
            }
            // Get the next keyword
            keyword = obj.ReadElement(' ');
        }
        
        return true;
    }

    bool Mesh::Init(ResourceController* resources)
    {
        // TODO
        return true;
    }

    bool Mesh::LoadAndInit(std::string guid_path, ResourceController* resources)
    {
        return Load(guid_path) && Init(resources);
    }

    void Mesh::Render(RenderInput* pass, const Matrix<4, 4>& view, const Matrix<4, 4>& proj)
    {
        for (unsigned int face = 0, numfaces = faces.size(); face < numfaces; face++)
        {
            for (unsigned int i = 0, counti = faces[face].size(); i < counti; i++)
            {

            }
        }
    }

}
