#pragma once

using ComponentMask = uint64_t; // assume up to 64 component types


enum ComponentID
{
    Transform_ID,
    Camera_ID,
    Mesh_ID,

    TotalComponentsCount
};


struct Component {
    virtual ~Component() = default;
    ComponentMask mask;
    uint32_t typeID;
};


struct TransformComponent : public Component {
    static constexpr uint32_t StaticTypeID = Transform_ID;

    XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
    XMFLOAT4 rotation = { 0.0f, 0.0f, 0.0f, 1.0f }; // quaternion
    XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f };

    Entity    parent = { UINT32_MAX };
    bool      dirty = true;

    TransformComponent() {
        mask = 1ULL << StaticTypeID;
        typeID = StaticTypeID;
    }
};

// Contient tout ce dont le CameraSystem a besoin pour calculer la view/proj.
struct CameraComponent : public Component {
    static constexpr uint32_t StaticTypeID = Camera_ID; // Choisissez un ID libre (> 3 dans votre exemple)

    // Position et vecteurs de base
    XMFLOAT3 position = { 0.0f, 0.0f, -1.0f }; // camera placee 1 unites en -Z
    XMFLOAT3 forward = { 0.0f, 0.0f,  1.0f };
    XMFLOAT3 up = { 0.0f, 1.0f,  0.0f };
    XMFLOAT3 right = { 1.0f, 0.0f,  0.0f };

    // Matrices stockees en float4x4 (transpose si HLSL attend column‐major)
    XMFLOAT4X4 viewMatrix = {};
    XMFLOAT4X4 projectionMatrix = {};

    // Parametres de projection
    float fovDegrees = 90.0f;
    float aspectRatio = 16.0f / 9.0f;
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;

    // Flag “dirty” si position/orientation/projection a change
    bool viewDirty = true;
    bool projectionDirty = true;

    // Vitesse et sensibilite souris
    float cameraSpeed = 0.2f;
    float mouseSensitivity = 0.005f;
    bool  rotating = false;

    // Pour stocker la position precedente de la souris
    POINT prevMousePos = { 0, 0 };

    CameraComponent() {
        mask = (1ULL << StaticTypeID);
        typeID = StaticTypeID;
    }
};


struct MeshComponent : public Component
{
    static constexpr uint32_t StaticTypeID = Mesh_ID;
    uint32_t meshID;
    uint32_t materialID;

    MeshComponent(uint32_t meshID_, uint32_t materialID_)
    {
        mask = 1ULL << StaticTypeID;
        typeID = StaticTypeID;
        meshID = meshID_;
        materialID = materialID_;
    }
};