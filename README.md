# Ethertia <sup><a href="https://elytra.dev/ethertia">[Site]</a><a href="https://github.com/Dreamtowards/Ethertia">[Src]</a><a href="https://elytra.dev/~pris">[Dev]</a></sup>

Voxels, Realistic, Interactive

[comment]: <> (Minecraft<sup>Freedom</sup> + GTAV<sup>Detail</sup> + VRChat<sup>Interaction</sup>)


Demo videos:
[20221008]()

Supported Platforms: Windows, MacOS, Linux

### Credits

__Ethertia is developed by:__  
Game design, programming, graphics: [Eldrine Le Prismarine]()  
Music and sounds:  
Ingame artwork:   
Used libs&tools: cpp, glfw, glm, stb, bulletphys



## Updates


### 22u49 @20221205 [.43]

![note](run/saves/_figures/fig-221203-atmo-test.png)
<img src="run/saves/_figures/fig-221203-p2.png" style="width: 20%">
<img src="run/saves/_figures/fig-221203-p3.png" style="width: 20%">

- [ ] Atmosphere test.
- [ ] Fix NormAvg at Chunk Boundary, fail. cost lot, have bugs yet.
- [x] GuiScrollBox, Gui::gPushScissor()
- [x] misc: OBJLoader::saveOBJ(), Deferred Render Composer FBO. Mth::worldRay()

### 22u48 @20221129 [.68]

![note](run/saves/_figures/fig-221129-p2.png)
<img src="run/saves/_figures/fig-221129-p3.png" style="width: 20%">
<img src="run/saves/_figures/fig-221129-sn-mtl.png" style="width: 20%">
<img src="run/saves/_figures/fig-221129-p4.png" style="width: 20%">
<img src="run/saves/_figures/fig-221129-p5.png" style="width: 20%">

- [x] [.79] SurfaceNets Terrain, Unified Grids
  - FeaturePoint Evaluation, Naive Method.
  - 1 of 8 Voxel Material Determine. Distance Method.
- [x] [.71] Terrain Material Texturing with Triangle Blend & Height Map
- [x] [.63] Triplanar Normal Mapping.
- [x] [.54] Material Texture Atlas spec.
- [x] [.42] Framebuffer, gbuffer init.

- [x] [.63] Deferred Rendering
- [x] [.34] Specular Lighting via Roughness map.

### 22u41 @20221014 [.29]

- [x] [.42] PerFragment Material Texturing (single material out of greatest weight in the triangle.)

![note](run/saves/_figures/fig-221014-mtltex-maxweight.png)  

### 22u40 @20201006

![note](run/saves/_figures/fig-221006-mtltex-fulltriangle.png)  
_ps. marching cubes with kernel program generated vertex UVs._

- [x] [.62] Multiple Material Texturing (naive full-triangle material texturing).  
- [x] [.13] Gamemode creative/spectator (collision disabled, `/gamemode <1/2/3>`), Fly mode (gravity disable).


---


## Roadmap


### Gameplay

Create, Explore, Survive.

- **Explore**
  - Biomes: Deserts, Plain, Cliff

- **Farming**
  - Vegetables: Wheat, Potatoes, Carrots

- **Vehicles**
  - Cars
  - Trains
  - Boats

### Todos

<br>



- [ ] SVOs and LoD for SurfaceNets
- [ ] SDF Brushes, Brush Masks (Limit, Exclude)
- [ ] Biomes

- [ ] Particle system
- [ ] Batch Grass Rendering
- [ ] Simple Clouds (Flat noise), Blocky Clouds (Multi layers), Volume Clouds
- [ ] Acid Rendering (Inception)
- [ ] Environment Reflection.
- [ ] Paradox Mapping.
- [ ] Shadow Mapping (necessary?)
- [ ] SSAO.

- [ ] World Storage
- [ ] Networking, Multiplayer

- [ ] Crafting Table [?]
- [ ] Cars. (wheels and body)
- [ ] Dual Contouring & SVOs & LoD
- [ ] Unified Theory of Smoothness & Sharpness features.
- [ ] VR Supports. MotionTrack. IKs.
- [ ] Skeletal Animation. Frame transfer.


<!--

### Ancient

- [x] Blocky Terrain, Cubical Chunks, Unified Grids, Basic Population. Simple Trees, Plants [@2022_Jul](https://www.youtube.com/watch?v=xDwgZkYrPm8&t=14s)
- [x] GUI System. GuiSlider, GuiCheckBox, GuiTextBox, GuiScrollBox.. [Bili @2022_Aug23](https://www.bilibili.com/video/BV1yU4y1k7EU)
- [x] MarchingCubes Terrain, Bulletphysics integrate. Simple Brush. Triplanar UV Mapping & Multi-Material. [Bili @2022_Aug30](https://www.bilibili.com/video/BV1JB4y1G7np)



### 


[Tutorial, Discussion, Questions; Multiplayer, Mods, Redstone]
Topic (Mods, Survival, Dev) / Type (Tutorial, Discussions, Questions)



Forum
- _Comprehensive Discussion_
  - Official News
  - Gameplay Discussions
  - Development Discussions
    - Software & Tools
  - Conferences
- Maps
- Mods
  - Resourcepacks
  - Shaderpacks
- Servers
- Avatars
- Modpacks


1. Normal Smooth at ChunkBoundary
1. Water
1. Cloud


**Building**

**Terrain Generation**

**Rendering**

**Multiplayer**

**GUI**

**Modding**

**Interacting**

-->