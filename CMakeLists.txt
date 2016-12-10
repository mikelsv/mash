cmake_minimum_required(VERSION 3.1)

# Set Project Name
set(PROJECTNAME mash)


project(${PROJECTNAME})

set(SOURCE ${PROJECTNAME}.cpp msvcore2/VString.cpp msvcore2/MString.cpp)

set (CMAKE_CXX_STANDARD 11)

add_executable(${PROJECTNAME} ${SOURCE})

TARGET_LINK_LIBRARIES(${PROJECTNAME}
pthread
pcre2-8
)



