cmake_minimum_required(VERSION 3.5)
set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})

project(messanger_server VERSION ${PROJECT_VESRION})

find_package(Threads)
find_package(Boost COMPONENTS system)
find_library(PQXX_LIB pqxx)

add_executable(messanger_server src/main.cpp src/Database.cpp src/UserState.cpp)
target_include_directories(messanger_server
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include ${Boost_INCLUDE_DIR}
)

set_target_properties(messanger_server PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        )

target_link_libraries(messanger_server
        ${CMAKE_THREAD_LIBS_INIT}
        dl ${PQXX_LIB}
        ${Boost_LIBRARIES}
        )
install(TARGETS messanger_server RUNTIME DESTINATION bin)
set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT artem-glibkin@yandex.ru)

include(CPack)