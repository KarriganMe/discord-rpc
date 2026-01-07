cmake_minimum_required(VERSION 3.16)
project(DiscordRPC)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_PREFIX_PATH "C:/Qt/6.10.1/mingw_64")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)
find_package(Python3 REQUIRED COMPONENTS Development)

# Set executable name and type
if(WIN32)
    set(GUI_TYPE WIN32) # This removes console window
endif()

add_executable(DiscordRPC ${GUI_TYPE}
        main.cpp
        worker.cpp
        worker.h
        gui.cpp
        gui.h
)

target_link_libraries(DiscordRPC PRIVATE Qt6::Widgets Python3::Python)
target_include_directories(DiscordRPC PRIVATE ${Python3_INCLUDE_DIRS})

# Copy Python DLL
if(WIN32)
    add_custom_command(TARGET DiscordRPC POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${Python3_RUNTIME_LIBRARY_DIRS}/python314.dll"
            $<TARGET_FILE_DIR:DiscordRPC>
    )
    
    # Deploy Qt dependencies with correct path
    add_custom_command(TARGET DiscordRPC POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Deploying Qt dependencies..."
        COMMAND "C:/Qt/6.10.1/mingw_64/bin/windeployqt.exe"
                --release 
                --no-translations
                $<TARGET_FILE:DiscordRPC>
    )
endif()
