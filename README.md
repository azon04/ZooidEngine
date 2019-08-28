# Zooid Engine
Version: 0.1.1

A C++ game engine that implemented and used for practice and experiment. Currently developed using VS 2017.

I've been developing the engine during my free time between works, school and another projects, so the progress is pretty much slow.

## Things Done
These are some features that I made for this version of the engine.

* **Math Library**
  * Some basic math data structures, such as Vector3, Vector2, Matrix3x3, Matrix4x4, and Quaternion; 
  * Math operations and functions, such as Matrix-Quaternion conversion, Matrix rotate around an axis, Orthographis and Perspective matrix calculation.
  * Implement SIMD for vector4-vector4 multiplication and matrix4x4-matrix4x4 multiplication
* **Game Object Model**
  * Basic game object model; Add and remove children from/to one object. 
  * Some basic RTTI (or DTI for now)
* **Event System** - Handling event in an object and broadcast to children if and only if there are children that handle such event.
* **Configurable Memory Management** - Implemented using a collection of a number of memory pool; Implemented Handle struct to allocating and deallocating block from Memory Management; Also handling allocating more memory if needed.
* **Basic Resource Manager** - Including reading and writing to File, and these also handles pass the resource to GPU if needed.
  * Material Resource Manager
  * Mesh Resource Manager
  * Buffer Resource Manager
  * Shader Resource Manager
* **Rendering Interface Module** - Implemented basic rendering interface functions.
* **OpenGL Renderer** - Currently, developed for OpenGL 4.3 or later. This is implemented using Rendering Interface module so it will be relatively easy to replace with another Graphics API.
* **Supported Rendering Techniques**:
  * Deferred Rendering
  * Forward Rendering
* **Render Graph**
* **Rendering Stuff**
  * Screen-Space Ambient Occlusion (SSAO)
  * Shadow Map
  * Cascaded Shadow Map (CSM) for directional light
  * Cubemap Skybox
* **Custom Data Structures** to work with Memory Management: 
  * Resizable Array, 
  * Resizable HashMap (also using HashString), 
  * Custom String.
* **Input Manager** that handle keyboard and mouse input
* **Simple Drawlist** - Made drawlist to be collected/accumulated before drawing to renderer.
* **Components**
  * SceneComponent - which has position/transform in the game world.
  * RenderComponent - has Mesh to be drawn by renderer and also has transform.
* **Basic Module** - make module system so it's easy to implement module in the engine.
* **Multithreading** - game and rendering thread is in different threads
* **ModelParser Tool** - import external 3D model format to internal model/mesh format.
* **Basic Log Manager** - support grouping log, enabling or disabling certain group, and put log into file (currently too intrusive)
* **Basic Physics** - Currently using PhysX 3.4 (put "Physx" with PhysX path in Environment Variable)
* **UI** - UI using ZooidUI, the Immediate-Mode GUI that I also manage.
* **Skeletal Animation** - Basic Skeletal animation, including full-body animation, partial-body animation, animation blend and additive animation blend. The setting/animation blend can be setting using tree/hierarchical animation blending (setup using text file).
* **Font Rendering** - The engine is now able to render font from font file, texture, generated SDF texture, and Multi-channel SDF texture.
* **Font Texture Generation Tools** - generating font glyphs texture and descriptor file for the texture to read in the engine. The tools can generate SDF texture and Multi-channel SDF texture. The tools is used https://github.com/Chlumsky/msdfgen to help in SDF and MSDF generation. 
* **Demo: Scene Viewer** - Basic viewer to load and view a scene file (.scz file).
* **Demo: Animation Demo** - basic demo how animation set up and played. See "Resources/TestAnimation" for the assets used in the demo.
* **Misc**
  * High resolution timer with C++11 or basic timer if not using C++11.

## Libraries and External Codes
This list consists all libraries and code that help me implementing the engine.

* **GLEW 2.00** - http://glew.sourceforge.net/
* **GLFW** - http://www.glfw.org/
* **stb_image** - https://github.com/nothings/stb to load image png, jpg, etc for texture.
* **lookup3.c, by Bob Jenkins** - http://burtleburtle.net/bob/c/lookup3.c used in HashMap that using String as a key.
* **assimp** - https://github.com/assimp/assimp to export from outer mesh format to internal mesh format
* **NVidia PhysX 3.4** - https://www.geforce.com/hardware/technology/physx
* **MSDFGen** - https://github.com/Chlumsky/msdfgen
* **ZooidUI** - I also manage this repository. https://github.com/azon04/ZooidUI

## Assets
* **Mixamo** Animation assets used for the demo and testing are grabbed from https://www.mixamo.com/

## References
* Gregory, Jason. *Game Engine Architecture. Second Edition*. CRC Press, 2014
* Deloura, Mark. *Game Programming Gems*. Charles River Media, 2000
* Deloura, Mark. *Game Programming Gems 2*. Charles River Media, 2001
* Watt, Alan. *3D Computer Graphics. Third Edition*. Addison-Wesley, 2000
* *Learn OpenGL* https://learnopengl.com/
