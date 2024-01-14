cmake_minimum_required(VERSION 3.16)

set(project_name chat) 

project(${project_name} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src)


find_package(oatpp              1.3.0 REQUIRED)
find_package(oatpp-websocket    1.3.0 REQUIRED)
find_package(oatpp-postgresql   1.3.0 REQUIRED)
find_package(inja REQUIRED)

add_definitions(
        -DFRONT_PATH="${CMAKE_CURRENT_LIST_DIR}/front/"
        -DDATABASE_MIGRATIONS="${CMAKE_CURRENT_LIST_DIR}/src/migrations"
        -DCONFIG_PATH="${CMAKE_CURRENT_LIST_DIR}/config/config.json"
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
                src/dto/ConfigDto.hpp
                src/service/db/Database.hpp
                src/DatabaseComponent.hpp
                src/service/auth/AuthServiceBase.hpp
                src/service/auth/AuthServiceBase.cpp
                src/service/auth/AuthTokenService.hpp
                src/service/auth/AuthTokenService.cpp
                src/service/auth/UserObject.hpp
                src/utils/utils.hpp
                )

target_link_libraries(${project_name}-exe
        PUBLIC oatpp::oatpp 
        PUBLIC oatpp::oatpp-websocket
        PUBLIC oatpp::oatpp-postgresql
        PUBLIC oatpp::oatpp-test
        PUBLIC pantor::inja
)
