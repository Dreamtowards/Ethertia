////
//// Created by Dreamtowards on 2023/1/31.
////
//
//#ifndef ETHERTIA_ENTITYTORCH_H
//#define ETHERTIA_ENTITYTORCH_H
//
//#include <ethertia/entity/Entity.h>
//#include <ethertia/render/particle/Particle.h>
//
//class EntityTorch : public Entity
//{
//public:
//
//    EntityTorch()
//    {
//        // 很快就会取消这种 Entity,
//
//        static VertexBuffer* M_Lantern = Loader::loadOBJ("material/torch/mesh.obj");
//        static Model* M_Lan = Loader::loadModel(M_Lantern);
//        static Texture* Tex = Loader::loadTexture("material/torch/diff.png");
//
//        m_Model = M_Lan;
//        m_DiffuseMap = Tex;
//        initRigidbody(0, createHullShape(M_Lan->vertexCount, M_Lantern->positions.data()));
//
//    }
//
//    Light* m_Light = new Light();
//
//    Light* getLights() {
//        m_Light->position = getPosition();
//        m_Light->color = {1, 0.3, 0.6};
//        m_Light->attenuation = {1, 0.14, 0.07};
//        return m_Light;
//    }
//
//    Particle* genParticles() {
//
//        Particle* p = new Particle();
//        p->position = getPosition() + glm::vec3(0, 0.4, 0);
//        p->max_lifetime = 1;
//        p->size_grow = 1;
//
//        static Texture* TEX_FIRE = Loader::loadTexture("misc/particles/fire.png");
//        static Texture* TEX_SMOKE = Loader::loadTexture("misc/particles/smoke.png");
//        p->texture = Timer::timestampMillis() % 2 == 1 ? TEX_FIRE : TEX_SMOKE;
//        p->tex_grids = 8;
//
//                int i = Timer::timestampMillis() * 7293423;
//                glm::vec3 rand{Mth::hash(i), Mth::hash(i*34243), Mth::hash(i*279128)};
//        p->velocity += glm::vec3(0,1,0) * 2.3f + rand * 1.0f;
//
//        return p;
//    }
//
//};
//
//#endif //ETHERTIA_ENTITYTORCH_H
