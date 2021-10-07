cmake_minimum_required (VERSION 3.0)

add_executable(SceneTest
  SceneTest.cpp
  SceneCamera.cpp
  )

target_include_directories (SceneTest
  PRIVATE ${CMAKE_SOURCE_DIR}/include
  PRIVATE ${CMAKE_SOURCE_DIR}/dependencies/OALWrapper/include
  PRIVATE ${CMAKE_SOURCE_DIR}/dependencies/AngelScript/include
  PRIVATE ${CMAKE_SOURCE_DIR}/dependencies/Newton/include
  PRIVATE ${GLEW_INCLUDE_DIRS})

target_link_libraries (SceneTest HPL)
