
# Voxel Raycast Implementation. (Unified Grids)

http://www.cse.yorku.ca/~amana/research/grid.pdf

https://www.gamedev.net/blogs/entry/2265248-voxel-traversal-algorithm-ray-casting/

体素的Raycast，在最简但全面的路径下遍历，途中检测体素是否碰撞

该遍历方法，经过每个和射线相交的体素。每次迭代 只有几个浮点值比较和加法操作。

```cpp
// void raycast(vec3 rpos, vec3 rdir)

// xyz: -1 or 1. 
// if one axis is 0.0f, then it's _t_ would be NaN (div by zero), i.e. wouldn't be use in stepping.
vec3 step = glm::signal(rdir);

// initial _t_ that reach to self grid boundary.
vec3 tMax = glm::abs(glm::fract(rpos) - glm::max(step, 0.0f)) / glm::abs(rdir);

vec3 tDelta = 1.0f / glm::abs(rdir);

glm::vec3 p = glm::floor(rpos);

int _itr = 0;
while (++_itr < 100) {
    int face;  // 0: -X, 1: +X, 2: -Y, 3: +Y, 4: -Z, 5: +Z
    if (tMax.x < tMax.y && tMax.x < tMax.z) {
        p.x += step.x;
        tMax.x += tDelta.x;
        face = step.x > 0 ? 0 : 1;
    } else if (tMax.y < tMax.z) {
        p.y += step.y;
        tMax.y += tDelta.y;
        face = step.y > 0 ? 2 : 3;
    } else {
        p.z += step.z;
        tMax.z += tDelta.z;
        face = step.z > 0 ? 4 : 5;
    }

    // traversing voxel along the ray with point p.
    u8 b = getMaterial(p);
    if (b) {  // checks the collision.
        if (isPlacing())
            setMaterial(p + Mth::QFACES[face], Blocks::STONE);
        else
            setMaterial(p, 0);
        return;
    }
}

//  Faces: 
//  vec3(-1, 0, 0), vec3( 1, 0, 0),
//  vec3( 0,-1, 0), vec3( 0, 1, 0),
//  vec3( 0, 0,-1), vec3( 0, 0, 1)
```