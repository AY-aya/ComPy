# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\appPython_Compiler_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\appPython_Compiler_autogen.dir\\ParseCache.txt"
  "appPython_Compiler_autogen"
  )
endif()
