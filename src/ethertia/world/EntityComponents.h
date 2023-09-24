

#include <glm/glm.hpp>
#include <string>



// name as TagComponent instead of NameComponent, because 'Tag' could means some 'attached information' like IsEnabled
struct TagComponent
{
	bool IsEnabled;

	std::string Name;

	TagComponent(const std::string& name, bool enabled = true) : Name(name), IsEnabled(enabled) {}
};



struct TransformComponent
{
	glm::mat4 Transform;
};

