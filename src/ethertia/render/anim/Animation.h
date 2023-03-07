//
// Created by Dreamtowards on 2023/3/7.
//

#ifndef ETHERTIA_ANIMATION_H
#define ETHERTIA_ANIMATION_H

#include <vector>
#include <map>

// Skeleton Animation.



class Animation
{
public:

    struct Joint
    {
        int parent_idx;    // root: -1
        // Joint* parent = nullptr;
        std::string name;

        glm::mat4 curr_trans;

        glm::mat4 inv_bind_pose;
    };

    struct KeyFrame
    {
        // Timestamp
        float time;

        // Transformation
        glm::vec3 pos;
        glm::quat rot;


        static glm::mat4 lerp_transf(float t, KeyFrame begin, KeyFrame end)
        {

        }
    };

    // KeyFrames for Joints
    std::map<std::string, std::vector<Animation::KeyFrame>> m_KeyFrames;
    // std::vector<std::pair<Joint*, std::vector<Animation::KeyFrame>>> m_KeyFrames;

//    static float getDuration() {}




    // calc Joint Transformations for an Animation at a given Time.
    static void calcAnimJoints(float time, const Animation& anim, const std::vector<Joint>& joints)
    {
        for (auto& it : anim.m_KeyFrames)
        {
            const std::vector<KeyFrame>& keyframes = it.second;

            const KeyFrame* curr = nullptr;
            const KeyFrame* next = nullptr;
            for (int i = 0; i < keyframes.size(); ++i) {
                if (time < keyframes[i].time) {
                    assert(i > 0);
                    curr = &keyframes[i-1];
                    next = &keyframes[i];
                    break;
                }
            }
            assert(curr != next);

            float t = Mth::rlerp(time, curr->time, next->time);
            assert(t >= 0.0f && t < 1.0f);

            glm::mat4 rel_trans = KeyFrame::lerp_transf(t, *curr, *next);


//            Joint& joint = it.first;
//            if (joint.parent_idx != -1)
//            {
//                // Joint Order Required: order of parent joint must be former (be valid curr_trans).
//                joint.curr_trans = joint.parent_joint.curr_trans * rel_trans;
//            }
        }
    }


    // static std::vector<glm::mat4> getJointTransforms();
    // trans[i] = j.curr_trans * j.inv_bind_trnas;




    static Animation loadAnim()
    {

    }

    static std::vector<Animation::Joint> loadSkeleton()
    {

    }

};

#endif //ETHERTIA_ANIMATION_H
