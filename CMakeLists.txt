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
        -DSTATIC_PATH="${CMAKE_CURRENT_LIST_DIR}/front/static/"
)


add_executable(${project_name}-exe 
                src/App.cpp
                src/AppComponent.hpp    
                src/controller/AuthController.hpp
                src/service/chat/Peer.hpp
                src/service/chat/Peer.cpp
                src/service/chat/Lobby.cpp
                src/service/chat/Lobby.hpp
                src/service/chat/Room.cpp
                src/service/chat/Room.hpp
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
                src/utils/utils.cpp
                src/service/interceptors/LoggerInterceptor.hpp
                src/service/db/DOs.hpp
                src/controller/StaticFilesManager.hpp
                src/controller/StaticFilesManager.cpp
                src/controller/StaticController.hpp
                src/service/dao/interfaces/IChatDao.hpp 
                src/service/dao/interfaces/ITokenDao.hpp 
                src/service/dao/interfaces/IUserDao.hpp
                src/service/dao/impl/postgres/ChatDao.cpp 
                src/service/dao/impl/postgres/ChatDao.hpp 
                src/service/dao/impl/postgres/TokenDao.cpp 
                src/service/dao/impl/postgres/TokenDao.hpp 
                src/service/dao/impl/postgres/UserDao.cpp 
                src/service/dao/impl/postgres/UserDao.hpp
                )       

target_include_directories(${project_name}-exe PUBLIC src/service/dao)


target_link_libraries(${project_name}-exe
        PUBLIC oatpp::oatpp 
        PUBLIC oatpp::oatpp-websocket
        PUBLIC oatpp::oatpp-postgresql
        PUBLIC oatpp::oatpp-test
        PUBLIC pantor::inja
)
