HPL1 Engine Source Code
=======================

This is my attempt to port HPL1 engine to modern platforms and also make it
blob-free.

This is a roadmap:

- [x] Remove CG library from dependencies (you can still see stuff because it's
  legacy OpenGL, hence shaders are optional).
- [x] Bundle Newton, AngelScript and OALWrapper.
- [x] Port to AngelScript and Newton from
  [here](https://github.com/shamazmazum/AmnesiaTheDarkDescent) (this was some
  redundant work, I mislooked 'Newton' branch here).
- [x] Port to SDL2.
- [ ] Write OpenGL shader and program classes (also we need abstract stuff like
  `iGpuShader` and `iGpuProgram` from HPL2).
- [ ] Rewrite materials to understand new shaders.
- [ ] Rewrite CG shaders to GLSL.
- [ ] Fix bugs which may arise in physics engine (see `SceneTest` demo, the
  collision detection behaves strangely there).

License Information
-------------------
All code is under the GPL Version 3 license except for the "test" which are
included under the ZLIB license. All of the assets are licensed under the
Creative Commons Attribution Share-Alike 3.0 license except for the CG shaders
which are under the ZLIB license.  Please read the COPYING and LICENSE-* files
for more information on terms of use.
