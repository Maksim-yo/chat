cmake_minimum_required(VERSION 3.16)

set(project_name chat) 

project(${project_name} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src)


find_package(oatpp              1.3.0 REQUIRED)
find_package(oatpp-websocket    1.3.0 REQUIRED)

find_package(inja REQUIRED)

add_definitions(
        -DFRONT_PATH="${CMAKE_CURRENT_LIST_DIR}/front/"
)


add_executable(${project_name}-exe 
                src/App.cpp
                src/AppComponent.hpp    
                src/controller/AuthController.hpp
                src/service/Peer.hpp
                src/service/Peer.cpp
                src/service/ConnectionHandler.cpp
                src/service/ConnectionHandler.hpp
                src/service/Room.cpp
                src/service/Room.hpp
                src/dto/DTOs.hpp
                )

target_link_libraries(${project_name}-exe
        PUBLIC oatpp::oatpp 
        PUBLIC pantor::inja
        PUBLIC oatpp::oatpp-websocket

)

