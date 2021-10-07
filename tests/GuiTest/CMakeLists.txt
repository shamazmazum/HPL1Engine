cmake_minimum_required (VERSION 3.0)

add_executable(GuiTest
  GuiTest.cpp
  ../Common/SimpleCamera.cpp)

target_include_directories (GuiTest
  PRIVATE ${CMAKE_SOURCE_DIR}/include
  PRIVATE ${CMAKE_SOURCE_DIR}/dependencies/OALWrapper/include
  PRIVATE ${CMAKE_SOURCE_DIR}/dependencies/AngelScript/include
  PRIVATE ${CMAKE_SOURCE_DIR}/dependencies/Newton/include
  PRIVATE ${GLEW_INCLUDE_DIRS})

target_link_libraries (GuiTest HPL)
