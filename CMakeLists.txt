project(ProxyServer)
cmake_minimum_required( VERSION 3.0.0 )


set( Boost_USE_STATIC_LIBS        OFF )
set( Boost_USE_MULTITHREADED      ON )
set( Boost_USE_STATIC_RUNTIME     OFF )
set( BOOST_ALL_DYN_LINK           ON )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

find_package( Boost REQUIRED )


include_directories( ${Boost_INCLUDE_DIR} )
link_directories( ${Boost_LIBRARY_DIRS} )

add_executable( ${PROJECT_NAME} main.cpp ./src/tcpproxy_server.cpp)
target_link_libraries( ${PROJECT_NAME} ${Boost_LIBRARIES} )