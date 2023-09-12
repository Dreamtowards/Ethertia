

#include "Mth.h"



namespace glm
{
    std::ostream& operator<<(std::ostream& s, const glm::vec3& v) {
        s << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return s;
    }
}
