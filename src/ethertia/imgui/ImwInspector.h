
#pragma once

#include <unordered_map>

#include <ethertia/world/Entity.h>



// EntityList and EntityInspector of a World

class ImwInspector
{
public:

	inline static Entity SelectedEntity{};


	static void ShowHierarchy(bool* _open);


	static void ShowInspector(bool* _open);


    inline static std::unordered_map<uint32_t, std::function<void(void*)>> ComponentInspectors;

    template<typename ComponentType>
    static void AddComponentInspector(const std::function<void(ComponentType&)>& fn)
    {
        ComponentInspectors[entt::type_hash<ComponentType>().value()] = [fn](void* cp_data)
            {
                fn(*static_cast<ComponentType*>(cp_data));
            };
    }

    static void InitComponentInspectors();
};