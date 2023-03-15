//
// Created by Dreamtowards on 2022/12/29.
//

#ifndef ETHERTIA_ENTITYCOMPONENT_H
#define ETHERTIA_ENTITYCOMPONENT_H

#include <ethertia/render/RenderEngine.h>

namespace Entt
{

class Component
{
public:

    virtual void _v() {};

};

class TransformComponent : public Entt::Component
{
public:
    glm::vec3 origin;
    glm::mat3 basis;
    glm::vec3 scale;
};

class ChunkProxyComponent
{

};

class PlayerComponent
{

};

class NoCullingFaceComponent
{

};

class MeshComponent : public Entt::Component
{
    VertexArrays* m_VAO = nullptr;
    Texture* m_DiffuseMap = nullptr;

public:
    MeshComponent(VertexArrays* mVao, Texture* mDiffuseMap) : m_VAO(mVao), m_DiffuseMap(mDiffuseMap) {}

    void onRender();

    void onLoad();

    void onUnload();


};



}


//class ComponentFaceCulling : public EntityComponent
//{
//public:
//    bool m_FaceCulling = true;
//};
//
//class ComponentRenderScale : public EntityComponent
//{
//public:
//
//};



class Light
{
public:
    using vec3 = glm::vec3;

    vec3 position;
    vec3 color;
    vec3 attenuation;

    vec3 direction;
    float coneAngle = 0;
    float coneRound = 0;

};



//#include <ethertia/render/Model.h>
//
//class EntityComponentRenderDefault : public EntityComponent
//{
//public:
//
//    Model*   m_Model   = nullptr;
//    Texture* m_Texture = nullptr;
//
//    void onRender()
//    {
//
//    }
//
//};




#endif //ETHERTIA_ENTITYCOMPONENT_H
