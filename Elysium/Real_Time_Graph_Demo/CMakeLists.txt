cmake_minimum_required(VERSION 3.5)
project(RealTimeGraph)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt5 and its components
find_package(Qt5 REQUIRED COMPONENTS Widgets Charts)

# Enable automatic handling of moc files
set(CMAKE_AUTOMOC ON)

# Add the source file
set(SOURCES
    Sources/main.cpp
)

# Add the executable
add_executable(RealTimeGraph ${SOURCES})

# Link Qt5 libraries
target_link_libraries(RealTimeGraph Qt5::Widgets Qt5::Charts)