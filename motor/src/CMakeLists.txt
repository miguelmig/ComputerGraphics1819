cmake_minimum_required(VERSION 2.8)

# Project Name
PROJECT(engine)

cmake_policy(SET CMP0015 NEW)
include_directories(./../thirdparty/include)
link_directories(./../thirdparty/lib)

if (WIN32)
	include_directories(./../thirdparty/include)
	link_directories(./../thirdparty/lib)
	#SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
	#SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
	SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/bin/Debug)
	SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR}/bin/Release)
	add_executable(${PROJECT_NAME} main.cpp Model.cpp utils.cpp Group.cpp Light.cpp)
	set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "../")
	target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} tinyxml2d glew32 DevIL)
	
	if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/bin/glut32.dll" 
		AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/bin/tinyxml2d.dll" 
		AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/bin/glew32.dll"
		AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/bin/DevIL.dll")
		file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/bin/glut32.dll
					DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG})
		
		file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/bin/glew32.dll
					DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG})
		
		file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/bin/tinyxml2d.dll
					DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG})
					
		file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/bin/glut32.dll
					DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE})
					
		file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/bin/glew32.dll
					DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE})
		
		file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/bin/tinyxml2d.dll
					DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE})
					
		file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/bin/devil.dll
					DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE})
		
	endif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/bin/glut32.dll" AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/bin/tinyxml2d.dll" AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/bin/glew32.dll" AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/bin/devil.dll")
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
	
else (WIN32) #Linux and Mac
	find_package(GLUT REQUIRED)
	include_directories(${GLUT_INCLUDE_DIR})
	link_directories(${GLUT_LIBRARY_DIRS})
	add_definitions(${GLUT_DEFINITIONS})

	if(NOT GLUT_FOUND)
		message(ERROR " GLUT not found!")
	endif(NOT GLUT_FOUND)

	find_package(OpenGL REQUIRED)
	include_directories(${OpenGL_INCLUDE_DIRS})
	link_directories(${OpenGL_LIBRARY_DIRS})
	add_definitions(${OpenGL_DEFINITIONS})
	if(NOT OPENGL_FOUND)
		message(ERROR " OPENGL not found!")
	endif(NOT OPENGL_FOUND)
	add_executable(${PROJECT_NAME} main.cpp Model.cpp utils.cpp Group.cpp Light.cpp)

	target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} tinyxml2d)
endif(WIN32)
