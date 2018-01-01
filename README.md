# Zooid Engine
Version: 0.1.0 (2017)

A C++ game engine that implemented and used for practice and experiment. Currently developed using VS 2015.

I've been developing the engine during my free time between works, school and another project, so the progress is pretty much slow.

## Things Done
These are some features that I made for this version of the engine.

* **Math Library**
  * Some basic math data structures, such as Vector3, Vector2, Matrix3x3, Matrix4x4, and Quaternion; 
  * Math operations and functions, such as Matrix-Quaternion conversion, Matrix rotate around an axis, Orthographis and Perspective matrix calculation.
* **Game Object Model**
  * Basic game object model; Add and remove children from/to one object. 
  * Some basic RTTI (or DTI for now)
* **Event System** - Handling event in an object and broadcast to children if and only if there are children that handle such event.
* **Configurable Memory Management** - Implemented using a collection of a number of memory pool; Implemented Handle struct to allocating and deallocating block from Memory Management.
* Basic Resource Manager - Including reading and writing to File.
* **OpenGL Renderer** - Currently, developed for OpenGL 3.3 or later.
  * Buffer Manager - Handling generate/reading data buffer manager from CPU memory to GPU
  * Texture Manager - Reading texture to the engine
  * Shader Manager - Reading and compile shader to GPU
* **Custom Data Structures** to work with Memory Management: 
  * Resizable Array, 
  * Resizable HashMap (also using HashString), 
  * Custom String.
* **Input Manager** that handle keyboard and mouse input

## Libraries and External Codes
This list consists all libraries and code that help me implementing the engine.

* **GLEW 2.00** - http://glew.sourceforge.net/
* **GLFW** - http://www.glfw.org/
* **stb_image** - https://github.com/nothings/stb to load image png, jpg, etc for texture.
* **lookup3.c, by Bob Jenkins** - http://burtleburtle.net/bob/c/lookup3.c used in HashMap that using String as a key.

## References
* Gregory, Jason. *Game Engine Architecture. Second Edition*. CRC Press, 2014
* Deloura, Mark. *Game Programming Gems*. Charles River Media, 2000
* Deloura, Mark. *Game Programming Gems 2*. Charles River Media, 2001
* Watt, Alan. *3D Computer Graphics. Third Edition*. Addison-Wesley, 2000
* *Learn OpenGL* https://learnopengl.com/
