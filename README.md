# gfx-templates
Windows setup code for different graphics APIs.
- win32_ogl4_template - OpenGL core profile graphics context using wglGetProcAddress to load OpenGL functions.
- win32_ogl4_glew_template - OpenGL core profile graphics context using GLEW to load OpenGL functions.


## Quick Start

Install [Visual Studio Community 2022](https://visualstudio.microsoft.com/).
Run init_vs.bat to setup the Visual Studio build tools.

Note: Change init_vs.bat if you have a different version of VS or if you installed it in a different location.

### Build

```console
$ build.bat
```
Or start the provided 4Coder or Visual Studio project.

## Dependencies
Windows and Visual Studio compiler. GLEW is used for the win32_ogl4_glew_template and is included in the repo.

## APIs
Currently supported graphics APIs are:
- OpenGL 4.6