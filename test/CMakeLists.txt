cmake_minimum_required(VERSION 3.5)
project(LspTest VERSION 0.1 LANGUAGES CXX)

file(GLOB_RECURSE CODE_FILES 
	*.cpp 
	*.h 
	*.hpp 
)

add_executable( ${PROJECT_NAME}
	${CODE_FILES}
)

target_link_libraries(${PROJECT_NAME} PUBLIC LspCore)