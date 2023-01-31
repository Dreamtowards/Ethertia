//
// Created by Dreamtowards on 2022/12/29.
//

#ifndef ETHERTIA_ENTITYCOMPONENT_H
#define ETHERTIA_ENTITYCOMPONENT_H

class EntityComponent
{
public:


};

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

class EntityComponentLight : public EntityComponent
{
public:

//    std::vector<Light*> getLights() {}

};




#endif //ETHERTIA_ENTITYCOMPONENT_H
