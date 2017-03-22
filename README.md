# Vertex Engine
This is open source 3D graphics engine mostly for proof of concept graphics application purposes. In the future it may evolve into some kind of game engine.

The newest features (may be **unstable**) will be pushed to **develop** branch and **stable** version of the engine will be pushed to **master** branch. 

## Short-term TODOs
- [ ] Refactor engine's architecture
- [x] Remove watch_ptr class
- [ ] Add base abstract class for lights
- [ ] Improve scene graph
- [ ] Easy to use toolkit to create demo/game scenes
- [x] Is CoreAssetManager necessary? Ans: Yes, it, is
- [ ] Better resource management 
- [ ] Easy to use material system
- [ ] Refactored & enhanced shader class
- [ ] Add feature to add #include directive in shaders (generic default shaders)
- [x] Update & move dlls to bin32/bin64 subfolders
- [ ] Add Forward rendering technique
- [ ] Add Deferred rendering technique
- [ ] Add easy to use interface to switch between different rendering techniques
- [ ] Easy to use interface for postprocess effects
- [ ] Shadow mapping (simple shadow mapping, variance shadow mapping)
- [ ] Models' animation (linear blend skinning, dual quaternion skinning)
- [ ] Directional ligths support
- [ ] Spot ligths support
- [ ] Point ligths support
- [ ] Light Shafts
- [ ] Support for geometry shaders
- [ ] Support for compute shaders
- [ ] Support for tesselation shaders

## Long-term TODOs
- [ ] Convert project to use CMake
- [ ] Graphical editor to creation of demo/game scenes easier
- [ ] Support for Lua scripting language
- [ ] Constructive solid geometry
- [ ] GPU supported ray tracing, photon mapping
- [ ] Support for Vulkan renderer
- [ ] Efficient and realistic atmospheric scattering

### Dependencies' versions
- SDL  2.0.4 (stable)
- GLEW 2.0.0
- FreeImage 3.17.0
- Assimp 3.2
