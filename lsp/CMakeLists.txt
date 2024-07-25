cmake_minimum_required(VERSION 3.5)

project(LspCore VERSION 0.1 LANGUAGES CXX)

file(GLOB_RECURSE CODE_FILES 
	*.cpp 
	*.h 
	*.hpp 
)

message( ${CODE_FILES})

add_library( ${PROJECT_NAME}
	${CODE_FILES}
)

target_include_directories(${PROJECT_NAME} PUBLIC include)

set_target_properties(${PROJECT_NAME} PROPERTIES
	ENABLE_EXPORTS TRUE
)


