# LCOLONQ Microgam jam 2026

Here's my entry for the game jam!

Link: [api.colonq.computer/jam/2026](api.colonq.computer/jam/2026)

## Build from Source
Run `make` from project root directory.  
Must add a web build copy of raylib and a copy of the raylib source into a `vendor/` directory.  
      raylib/
      libraylib.web.a


Makefile contains three variable to fill in for the location of raylib dependencies.  
```
RAYLIBWEB=<web compiled raylib archive>
RAYLIBSRC=<src files for raylib>
SHELLFILE=<shell file for web compiled raylib using emscripten>
```
This project also requires emscripten to build. 


