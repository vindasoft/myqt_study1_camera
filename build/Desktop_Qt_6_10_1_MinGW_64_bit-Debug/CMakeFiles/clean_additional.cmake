# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\VideoMonitorTool_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\VideoMonitorTool_autogen.dir\\ParseCache.txt"
  "VideoMonitorTool_autogen"
  )
endif()
