cmake_minimum_required (VERSION 3.0)
project (HPL)

set(HPL_VERSION_MAJOR 1)
set(HPL_VERSION_MINOR 0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-switch -Wno-undefined-var-template")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-ignored-attributes -Wno-extern-c-compat")
set(CMAKE_CXX_FLAGS_RELEASE "-fno-strict-aliasing")

option(USE_SDL2 "Use SDL2 instead of SDL1.2" OFF)

if(USE_SDL2)
  find_package(SDL2_ttf REQUIRED)
  find_package(SDL2_image REQUIRED)
  #set(SDL_TTF_INCLUDE ${SDL_TTF_INCLUDE_DIRS})
  #set(SDL_TTF_LIBRARY ${SDL_TTF_LIBRARIES})
else(USE_SDL2)
  find_package(SDL_ttf REQUIRED)
  find_package(SDL_image REQUIRED)
  set(SDL_TTF_INCLUDE ${SDL_TTF_INCLUDE_DIRS})
  set(SDL_TTF_LIBRARY ${SDL_TTF_LIBRARIES})
  set(SDL_IMAGE_INCLUDE ${SDL_IMAGE_INCLUDE_DIRS})
  set(SDL_IMAGE_LIBRARY ${SDL_IMAGE_LIBRARIES})
endif(USE_SDL2)

add_subdirectory(dependencies/OALWrapper)
add_subdirectory(dependencies/AngelScript)
add_subdirectory(dependencies/Newton)

set(OpenGL_GL_PREFERENCE LEGACY)
find_package(OpenGL REQUIRED)
#find_package(Theora REQUIRED)
#find_package(Devil REQUIRED)
find_package(GLEW REQUIRED)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/dependencies/OALWrapper/include
        ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/AngelScript/include
        ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/Newton/include
        ${SDL_TTF_INCLUDE}
        ${SDL_IMAGE_INCLUDE}
        ${GLEW_INCLUDE_DIRS}
        ${OPENGL_INCLUDE_DIR}
)
file(GLOB_RECURSE all_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/sources/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sources/*.c
)
# Removed Sources
file(GLOB_RECURSE to_remove_all
    win32.cpp
    Video*Theora.cpp
    )
list(REMOVE_ITEM all_sources
    ${to_remove_all})
list(REMOVE_DUPLICATES all_sources)

add_library(HPL STATIC ${all_sources})
target_link_libraries(HPL
  OALWrapper
  Newton
  AngelScript
  ${SDL_TTF_LIBRARY}
  ${SDL_IMAGE_LIBRARY}
  ${GLEW_LIBRARIES}
  ${OPENGL_gl_LIBRARY}
  ${OPENGL_glu_LIBRARY})
