#pragma once

#include "MeshSync/MeshSync.h"
#include "MeshSync/MeshSyncUtils.h"

using namespace mu;

struct msmqSettings
{
    ms::ClientSettings client_settings;

    float scale_factor = 200.0f;
    std::string host_camera_path = "/Main Camera";
    bool auto_sync = false;
    bool sync_normals = true;
    bool sync_vertex_color = true;
    bool make_double_sided = false;
    bool sync_camera = true;
    bool sync_bones = true;
    bool sync_poses = true;
    bool sync_textures = true;

    bool bake_skin = false;
    bool bake_cloth = false;
};

class msmqContext
{
public:
    msmqContext(MQBasePlugin *plugin);
    ~msmqContext();

    msmqSettings& getSettings();

    void clear();
    void flushPendingRequests(MQDocument doc);
    void sendMeshes(MQDocument doc, bool force = false);
    void sendCamera(MQDocument doc, bool force = false);
    bool importMeshes(MQDocument doc);

private:
    struct ObjectRecord : public mu::noncopyable
    {
        MQObject obj = nullptr;
        ms::MeshPtr dst;
    };

    struct BoneRecord : public mu::noncopyable
    {
        UINT bone_id = -1;
        UINT parent_id = -1;
        std::string name;
        float3 world_pos = float3::zero();
        quatf world_rot = quatf::identity();
        float4x4 bindpose = float4x4::identity();
        ms::TransformPtr dst = ms::Transform::create();
    };

    using HostMeshes = std::map<int, ms::MeshPtr>;

    int exportTexture(const std::string& path, ms::TextureType type);
    MQObject findMesh(MQDocument doc, const char *name);
    MQObject createMesh(MQDocument doc, const ms::Mesh& data, const char *name);
    void extractMeshData(MQDocument doc, MQObject src, ms::Mesh& dst);
    void extractCameraData(MQDocument doc, MQScene src, ms::Camera& dst); // true if anything changed
    void buildBonePath(std::string& dst, BoneRecord& bd);


    MQBasePlugin *m_plugin = nullptr;

    msmqSettings m_settings;

    HostMeshes m_host_meshes;

    std::vector<ObjectRecord> m_obj_records;
    std::map<UINT, BoneRecord> m_bone_records;

    ms::IDGenerator<MQCMaterial*> m_material_ids;
    std::vector<int> m_material_index_to_id;

    ms::CameraPtr m_camera;
    ms::TextureManager m_texture_manager;
    ms::MaterialManager m_material_manager;
    ms::EntityManager m_entity_manager;

    ms::AsyncSceneSender m_send_meshes;
    ms::AsyncSceneSender m_send_camera;
    bool m_pending_send_meshes = false;
};
