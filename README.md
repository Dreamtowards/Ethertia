# Ethertia

An "[OtherLife](https://en.wikipedia.org/wiki/OtherLife)" Game/Program, wanted:  
Minecraft<sup>Freedom</sup> + GTAV<sup>Detail</sup> + VRChat<sup>Interaction</sup>

> Site: https://elytra.dev/ethertia  
> Source: https://github.com/Dreamtowards/Ethertia `src/ethertia/client/Ethertia.h`    
> Devs: Eldrine Le Prismarine (https://elytra.dev/~pris), Dreamtowards


Demo videos:
[20221008](),  
Demo screenshots: (Early test, 20221005)
![snap1](src/assets/elytra/site/eth/res/demo-s1.png)


## Development



### Roadmap


### Todos

- [x] Blocky Terrain, Cubical Chunks, Unified Grids [@2022_Jul](https://www.youtube.com/watch?v=xDwgZkYrPm8&t=14s)
- [x] Blocky Terrain Basic Population. Simple Trees, Flowers, Vines
- [x] GUI System. GuiSlider, GuiCheckBox, GuiTextBox, GuiScrollBox [Bili @2022_Aug23](https://www.bilibili.com/video/BV1yU4y1k7EU)
- [x] Basic Rendering Optimization. Frustum Culling, Face Culling
- [x] MarchingCubes Smooth Terrain
- [x] BulletPhys integrate.
- [x] Simple Sphere Brush [Bili @2022_Aug30](https://www.bilibili.com/video/BV1JB4y1G7np)
- [x] Triplanar UV Mapping & Multi-Material.

<br>
  
- [ ] SurfaceNets Smooth Terrain
- [ ] Octrees (SVOs, Varying Grids)
- [ ] LoD (for SurfaceNets & SVOs)
- [ ] World Storage
- [ ] SDFs Brushes.
- [ ] Brush Masks (Limit, Exclude)
- [ ] Particles
- [ ] Deferred Rendering
- [ ] Grass Batch Rendering
- [ ] Simple Clouds (Flat noise)
- [ ] Blocky Clouds (Multi layers)
- [ ] Acid Rendering (Inception)
- [ ] Biomes
- [ ] Crafting Table [?]
- [ ] Cars. (wheels and body)
- [ ] Environment Reflection.
- [ ] Paradox Mapping.
- [ ] Shadow Mapping (necessary?)
- [ ] SSAO.
- [ ] Dual Contouring & SVOs & LoD
- [ ] Unified Theory of Smoothness & Sharpness features.
- [ ] VR Supports. MotionTrack. IKs.
- [ ] Skeletal Animation. Frame transfer.

### Credits

__Ethertia is developed by:__  
Game design, programming, graphics: [Eldrine Le Prismarine]()  
Music and sounds:  
Ingame artwork: 

__Ethertia uses following libraries&tools:__  
- Cplusplus, GCC, CLion IDE, CMake
- GLFW, GLM, glad, opengl
- stb_image, stb_vorbis by [Sean Barrett ('nothings')](http://nothings.org)
- BulletPhysics by Erwin Coumans



## Updates


### 22u41 @20221014

- [.42] Per-Fragment Material Texturing (single material out of greatest weight)

> ![note](saves/_figures/fig-221014-mtltex-maxweight.png)  
> Fugure: Per-Fragment Material Texturing, (one single material out for the vertex with the greatest weight on the triangle.
> use a geometry shader _chunk.gsh_ to generate _TriMtlWeights, TriMtlIds_)    
> the "boundary waves" may caused by non-geo-accurate weights (i.e. not associate with volume data). However I can't wait to try
> SurfaceNets now, that would fix this problem by the way.

### 22u40 @20201006

- [.62] Multiple Material Texturing (naive full-triangle material texturing).  
- [.13] Gamemode creative/spectator (no collision), Flying mode (no gravity).

> ![note](saves/_figures/fig-221006-mtltex-fulltriangle.png)  
> Multiple Material Naive Full-Triangle Texturing.  
> `/gamemode <1/2/3>   # survival/creative/spectator`  
> `/fly [on/off]`