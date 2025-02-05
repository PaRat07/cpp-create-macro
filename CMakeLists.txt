cmake_minimum_required(VERSION 3.31)
project(create_macro)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

# Download Boost.Preprocessor from GitHub
FetchContent_Declare(
        boost_pp
        GIT_REPOSITORY https://github.com/boostorg/preprocessor.git
        GIT_TAG        boost-1.87.0  # Specific Boost version tag
        GIT_SHALLOW    TRUE          # Download only latest commit (faster)
)

# Make the downloaded content available
FetchContent_MakeAvailable(boost_pp)

add_executable(create_macro main.cpp)
add_compile_options(-fmacro-backtrace-limit=0)
# Link Boost.PP headers to your target
target_include_directories(${PROJECT_NAME} PRIVATE
        ${boost_pp_SOURCE_DIR}/include  # Path to Boost headers
)
