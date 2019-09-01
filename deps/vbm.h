#pragma once

#include <glad/glad.h>
#include <vmath.h>

#define VBM_FLAG_HAS_VERTICES
#define VBM_FLAG_HAD_INDICES
#define VBM_FLAG_HAS_FRAMES
#define VBM_FLAG_HAS_MATERIALS

typedef struct VBM_HEADER_T {
    unsigned int magic;
    unsigned int size;
    char name[64];
    unsigned int num_attribs;
    unsigned int num_frames;
    // unsigned int num_chunks;
    unsigned int num_vertices;
    unsigned int num_indices;
    unsigned int index_type;
    unsigned int num_materials;
    unsigned int flags;
} VBM_HEADER;

typedef struct VBM_HEADER_OLD_T {
    unsigned int magic;
    unsigned int size;
    char name[64];
    unsigned int num_attribs;
    unsigned int num_frames;
    unsigned int num_chunks;
    unsigned int num_vertices;
    unsigned int num_indices;
    unsigned int index_type;
    unsigned int num_materials;
    unsigned int flags;
} VBM_HEADER_OLD;

typedef struct VBM_ATTRIB_HEADER_T {
   char name[64];
    unsigned int type;
    unsigned int components;
    unsigned int flags;
} VBM_ATTRIB_HEADER;

typedef struct VBM_FRAME_HEADER_T {
    unsigned int first;
    unsigned int count;
    unsigned int flags;
} VBM_FRAME_HEADER;

typedef struct VBM_RENDER_CHUNK_T {
    unsigned int material_index;
    unsigned int first;
    unsigned int count;
} VBM_RENDER_CHUNK;

typedef struct VBM_VEC4F_T{
    float x;
    float y;
    float z;
    float w;
} VBM_VEC4F;

typedef struct VBM_VEC3F_T{
    float x;
    float y;
    float z;
} VBM_VEC3F;

typedef struct VBM_VEC2F_T{
    float x;
    float y;
} VBM_VEC2F;

typedef struct VBM_MATERIAL_T{
    char name[32];              /// Name of material
    VBM_VEC3F ambient;          /// Ambient color
    VBM_VEC3F diffuse;          /// Diffuse color
    VBM_VEC3F specular;         /// Specular color
    VBM_VEC3F specular_exp;     /// Specular exponent
    float shininess;            /// Shininess
    float alpha;                /// Alpha (transparency)
    VBM_VEC3F transmission;     /// Transmissivity
    float ior;                  /// Index of refraction (optical density)
    char ambient_map[64];       /// Ambient map (texture)
    char diffuse_map[64];       /// Diffuse map (texture)
    char specular_map[64];      /// Specular map (texture)
    char normal_map[64];        /// Normal map (texture)
} VBM_MATERIAL;

class VBObject {
public:
    VBObject(void);
    virtual ~VBObject(void);

    bool loadFromVBM(const char *filename, int vertexIndex, int normalIndex, int texCoord0Index);
    void render(unsigned int frameIndex, unsigned int instances = 0);
    bool free();

    unsigned int getVertexCount(unsigned int frame = 0) {
        return frame < m_header.num_frames ? m_frame[frame].count : 0;
    }

    unsigned int getAttributeCount(void) const
    {
        return m_header.num_attribs;
    }

    const char * getAttributeName(unsigned int index) const
    {
        return index < m_header.num_attribs ? m_attrib[index].name : 0;
    }

    unsigned int getFrameCount(void) const
    {
        return m_header.num_frames;
    }

    unsigned int getMaterialCount(void) const
    {
        return m_header.num_materials;
    }
    const char * getMaterialName(unsigned int material_index) const
    {
        return m_material[material_index].name;
    }

    const vmath::vec3 getMaterialAmbient(unsigned int material_index) const
    {
        return vmath::vec3(m_material[material_index].ambient.x, m_material[material_index].ambient.y, m_material[material_index].ambient.z);
    }

    const vmath::vec3 getMaterialDiffuse(unsigned int material_index) const
    {
        return vmath::vec3(m_material[material_index].diffuse.x, m_material[material_index].diffuse.y, m_material[material_index].diffuse.z);
    }

    const char * getMaterialDiffuseMapName(unsigned int material_index) const
    {
        return m_material[material_index].diffuse_map;
    }

    const char * getMaterialSpecularMapName(unsigned int material_index) const
    {
        return m_material[material_index].specular_map;
    }

    const char * getMaterialNormalMapName(unsigned int material_index) const
    {
        return m_material[material_index].normal_map;
    }

    void setMaterialDiffuseTexture(unsigned int material_index, GLuint texname)
    {
        m_material_textures[material_index].diffuse = texname;
    }

    void setMaterialSpecularTexture(unsigned int material_index, GLuint texname)
    {
        m_material_textures[material_index].specular = texname;
    }

    void setMaterialNormalTexture(unsigned int material_index, GLuint texname)
    {
        m_material_textures[material_index].normal = texname;
    }

    void bindVertexArray() {
        glBindVertexArray(m_vao);
    }

protected:
    GLuint m_vao;
    GLuint m_attribute_buffer;
    GLuint m_index_buffer;

    VBM_HEADER m_header;
    VBM_ATTRIB_HEADER *m_attrib;
    VBM_FRAME_HEADER *m_frame;
    VBM_MATERIAL *m_material;
    VBM_RENDER_CHUNK *m_chunks;

    struct material_texture {
        GLuint diffuse;
        GLuint specular;
        GLuint normal;
    };

    material_texture *m_material_textures;
};

