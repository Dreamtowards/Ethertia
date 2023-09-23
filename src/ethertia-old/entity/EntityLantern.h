////
//// Created by Dreamtowards on 2023/1/31.
////
//
//#ifndef ETHERTIA_ENTITYLANTERN_H
//#define ETHERTIA_ENTITYLANTERN_H
//
//#include "Entity.h"
//
//class EntityLantern : public Entity
//{
//public:
//
//    EntityLantern() {
//
//        static VertexBuffer* M_Lantern = Loader::loadOBJ("material/lantern/mesh.obj");
//        static Model* M_Lan = Loader::loadModel(M_Lantern);
//        static Texture* Tex = Loader::loadTexture("material/lantern/diff.png");
//
//        m_Model = M_Lan;
//        m_DiffuseMap = Tex;
//        initRigidbody(0.5, createHullShape(M_Lan->vertexCount, M_Lantern->positions.data()));
//
//    }
//
//    Light* m_Light = new Light();
//
//    Light* getLights() {
//        m_Light->position = getPosition();
//        m_Light->color = {1, 1, 0.6};
//        m_Light->attenuation = {1, 0.14, 0.07};
//        return m_Light;
//    }
//};
//
//#endif //ETHERTIA_ENTITYLANTERN_H
