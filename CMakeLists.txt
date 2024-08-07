# -------------------------------------------------------------------------------------------------
# CMake build script
# -------------------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.10)
enable_testing()
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include("cmake/colormsg.cmake")
include(FetchContent)
include(ExternalProject)

# -------------------------------------------------------------------------------------------------
# Datoviz project
# -------------------------------------------------------------------------------------------------
set(DATOVIZ_VERSION 0.2.0)
project(datoviz VERSION ${DATOVIZ_VERSION} DESCRIPTION "datoviz")

# -------------------------------------------------------------------------------------------------
# Datoviz options
# -------------------------------------------------------------------------------------------------
option(DATOVIZ_WITH_CLI "Build Datoviz command-line interface with tests and demos" ON)
option(DATOVIZ_WITH_GLFW "Build Datoviz with glfw" ON)
option(DATOVIZ_WITH_MSDF "Build Datoviz with msdfgen" ON)

# option(DATOVIZ_WITH_SWIFTSHADER "Build Datoviz with Swiftshader support" OFF)

# -------------------------------------------------------------------------------------------------
# Build type
# -------------------------------------------------------------------------------------------------
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_STANDARD)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 17) # Adjust the version to your project's requirements

# NOTE: -one-off script to remove unnecessary #includes
# need to install llvm, llvm-dev, clang from the same version of iwyu
# fix the source files with:
#
# 1. In the build/ subfolder of datoviz:
# cmake ..
# make C=include-what-you-use CFLAGS="-Xiwyu --error_always" 2>iwyu.out
#
# 2. In the iwyu git cloned directory:
# python fix_includes.py -n --nosafe_headers
# --ignore_re="_deps|_shaders.c|_textures.c|_fonts.c"
# < ~/git/datoviz-distributed/build/iwyu.out
#
# find_program(iwyu_path NAMES include-what-you-use iwyu REQUIRED)
# set(CMAKE_C_INCLUDE_WHAT_YOU_USE iwyu -Xiwyu;any;-Xiwyu;iwyu;-Xiwyu;args)

# -------------------------------------------------------------------------------------------------
# Detect the OS
# -------------------------------------------------------------------------------------------------
set(OS_LINUX 0)
set(OS_MACOS 0)
set(OS_WIN32 0)

if(UNIX AND NOT APPLE)
    set(OS_LINUX 1)
elseif(APPLE)
    set(OS_MACOS 1)
elseif(WIN32)
    set(OS_WIN32 1)
endif()

# -------------------------------------------------------------------------------------------------
# Detect the compiler
# -------------------------------------------------------------------------------------------------
set(CC_GCC 0)
set(CC_CLANG 0)
set(CC_MSVC 0)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    # message("compiler is GCC")
    set(CC_GCC 1)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # message("compiler is clang")
    set(CC_CLANG 1)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    # message("compiler is MSVC")
    set(CC_MSVC 1)
endif()

# -------------------------------------------------------------------------------------------------
# Directories
# -------------------------------------------------------------------------------------------------
set(DATA_DIR "${CMAKE_SOURCE_DIR}/data")
set(SPIRV_DIR ${CMAKE_BINARY_DIR}/spirv)
set(ARTIFACTS_DIR "${CMAKE_BINARY_DIR}/artifacts")

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR})
file(MAKE_DIRECTORY ${SPIRV_DIR})
file(MAKE_DIRECTORY "${ARTIFACTS_DIR}")

# -------------------------------------------------------------------------------------------------
# Include directories
# -------------------------------------------------------------------------------------------------
set(INCL_DIRS
    ${CMAKE_SOURCE_DIR}/include/datoviz
    ${CMAKE_SOURCE_DIR}/external
    ${CMAKE_SOURCE_DIR}/external/imgui
)

# ---- OS-specific directories --------------------------------------------------------------------
if(${OS_MACOS})
    set(INCL_DIRS ${INCL_DIRS} /usr/local/include)
elseif(${OS_WIN32})
    set(INCL_DIRS ${INCL_DIRS}) # $ENV{VULKAN_SDK}/include $ENV{CGLM_INCLUDE})
    find_path(MINGW_DIR NAMES "gcc.exe")
endif()

# -------------------------------------------------------------------------------------------------
# Linking libraries
# -------------------------------------------------------------------------------------------------
set(LINK_LIBS cglm)

# Link with system libraries.
if(NOT ${WIN32})
    set(LINK_LIBS m dl pthread ${LINK_LIBS})
endif()

# ---- OS-specific linking libraries --------------------------------------------------------------
if(${OS_MACOS})
    link_directories(/usr/local/lib)
elseif(${OS_WIN32})
    # link_directories($ENV{VULKAN_SDK}\\Lib $ENV{VULKAN_SDK}\\Bin) # $ENV{CGLM_LIB} ${MINGW_DIR})
    set(LINK_LIBS ${LINK_LIBS})
endif()

# -------------------------------------------------------------------------------------------------
# Build dependencies
# -------------------------------------------------------------------------------------------------

# NOTE: if ON, will avoid fetching git repositories, thereby making builds faster.
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

# ---- cglm ---------------------------------------------------------------------------------------
FetchContent_Declare(
    cglm
    GIT_REPOSITORY https://github.com/recp/cglm/
    GIT_TAG v0.9.4
)
FetchContent_MakeAvailable(cglm)

# ---- glfw ---------------------------------------------------------------------------------------
set(HAS_GLFW 0)

if(DATOVIZ_WITH_GLFW)
    set(HAS_GLFW 1)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw/
        GIT_TAG 3.4
    )
    FetchContent_MakeAvailable(glfw)

    set(INCL_DIRS ${INCL_DIRS} "${glfw_SOURCE_DIR}/include")
    set(LINK_LIBS ${LINK_LIBS} glfw)
endif()

# ---- Vulkan -------------------------------------------------------------------------------------
set(HAS_GLSLANG 0)

find_library(Vulkan_LIBRARY NAMES vulkan HINTS "$ENV{VULKAN_SDK}/lib" "${CMAKE_SOURCE_DIR}/libs/vulkan" REQUIRED)

if(${OS_MACOS})
    find_program(GLSLC "glslc.osx-arm64" HINTS "$ENV{VULKAN_SDK}/bin" "${CMAKE_SOURCE_DIR}/bin/vulkan" REQUIRED)
    set(LINK_LIBS ${LINK_LIBS} "${CMAKE_SOURCE_DIR}/libs/vulkan/libvulkan.dylib")
else()
    find_program(GLSLC "glslc.linux64" HINTS "$ENV{VULKAN_SDK}/bin" "${CMAKE_SOURCE_DIR}/bin/vulkan" REQUIRED)
    set(LINK_LIBS ${LINK_LIBS} "${CMAKE_SOURCE_DIR}/libs/vulkan/libvulkan.so")
endif()

# ---- msdf ------------------------------------------------------------------------------------
set(HAS_MSDF 0)

if(DATOVIZ_WITH_MSDF)
    set(HAS_MSDF 1)

    set(MSDFGEN_CORE_ONLY OFF)
    set(MSDFGEN_BUILD_STANDALONE OFF)
    set(MSDFGEN_USE_SKIA OFF)
    set(MSDFGEN_DISABLE_SVG OFF)

    set(MSDF_ATLAS_BUILD_STANDALONE OFF)
    set(MSDF_ATLAS_USE_VCPKG OFF)
    set(MSDF_ATLAS_USE_SKIA OFF)
    set(MSDF_ATLAS_DYNAMIC_RUNTIME ON)
    set(MSDF_ATLAS_MSDFGEN_EXTERNAL OFF)

    set(BUILD_SHARED_LIBS ON)

    # Freetype is required for msdf-atlas-gen
    # sudo apt install libtinyxml2-dev libfreetype-dev
    find_package(Freetype REQUIRED)

    # tinyxml2
    FetchContent_Declare(
        tinyxml2
        GIT_REPOSITORY https://github.com/leethomason/tinyxml2.git
        GIT_TAG 10.0.0
    )
    FetchContent_MakeAvailable(tinyxml2)

    # MSDF atlas
    FetchContent_Declare(
        msdfgen-atlas
        GIT_REPOSITORY https://github.com/Chlumsky/msdf-atlas-gen.git
        GIT_TAG master
    )
    FetchContent_MakeAvailable(msdfgen-atlas)

    set(INCL_DIRS ${INCL_DIRS} ${msdfgen-atlas_SOURCE_DIR}/msdfgen ${FREETYPE_INCLUDE_DIR_ft2build})
    set(LINK_LIBS ${LINK_LIBS} msdfgen::msdfgen-ext msdf-atlas-gen::msdf-atlas-gen ${FREETYPE_LIBRARIES})
endif()

# -------------------------------------------------------------------------------------------------
# Compile definitions
# -------------------------------------------------------------------------------------------------
set(COMPILE_DEFINITIONS ${COMPILE_DEFINITIONS}
    LOG_USE_COLOR
    ENABLE_VALIDATION_LAYERS=1

    # Directories.
    ROOT_DIR=\"${CMAKE_SOURCE_DIR}\"
    DATA_DIR=\"${DATA_DIR}\"
    SPIRV_DIR=\"${SPIRV_DIR}\"
    ARTIFACTS_DIR=\"${ARTIFACTS_DIR}\"

    # OS.
    OS_LINUX=${OS_LINUX}
    OS_MACOS=${OS_MACOS}
    OS_WIN32=${OS_WIN32}

    # Compiler.
    CC_MSVC=${CC_MSVC}
    CC_GCC=${CC_GCC}
    CC_CLANG=${CC_CLANG}

    # Dependencies.
    HAS_GLFW=${HAS_GLFW}
    HAS_GLSLANG=${HAS_GLSLANG}

    # HAS_PNG=${HAS_PNG}
)

# -------------------------------------------------------------------------------------------------
# Compiler options
# -------------------------------------------------------------------------------------------------

# ---- common compiler options --------------------------------------------------------------------
# NOTE: need to remove -pg (gprof profiling) in RELEASE mode?
set(COMMON_FLAGS "-g -m64 -pedantic \
    -Wall \
    -Wextra \
    -Werror=vla \
    -Wcast-align \
    -Wcast-qual \
    -Wredundant-decls \
    -Wswitch-default \
    -Wdisabled-optimization \
    -Wmissing-declarations \
    -Wmissing-include-dirs \
    -Wshadow \
    -Wsign-conversion \
    -Wstrict-overflow=5 \
    -Wundef \
    -Wformat=2 -Winit-self \
    -Wno-unused-function \
    -Wno-unused-parameter \
    -Wno-format-nonliteral \
    ")

# -Wno-missing-field-initializers
# -Wno-unused-result \
# -Wno-variadic-macros")

# ---- compiler-specific options ------------------------------------------------------------------
if(CC_GCC)
    # gcc

    # # for fpng
    # set(GCC_COMPILE_FLAGS "\
    # -fvisibility=hidden
    # -fPIC \
    # -fno-strict-aliasing \
    # -fno-omit-frame-pointer \
    # -D_LARGEFILE64_SOURCE=1 \
    # -D_FILE_OFFSET_BITS=64 \
    # -Wall \
    # -Wextra \
    # -Isrc")
    set(COMMON_FLAGS "${COMMON_FLAGS} \
        -Wlogical-op \
        -Wno-stringop-overread \
        -fdiagnostics-color=always \
        -DFPNG_NO_SSE=0 -msse4.1 -mpclmul") # for fpng

    # NOTES: static analysis for later?
    # -fanalyzer \
    # -Wno-analyzer-possible-null-dereference \
    # -Wno-analyzer-possible-null-argument \
    # -Wno-analyzer-malloc-leak \
    # -Wno-analyzer-use-after-free \
    # -Wno-analyzer-double-free \

    # -Wdouble-promotion \
    # fsanitize=address fsanitize-trap=undefined

    # NOTE: to comment when using valgrind
    if(NOT OS_WIN32)
        set(COMMON_FLAGS "${COMMON_FLAGS} -pg")
    endif()

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAGS} \
        -std=gnu11 -Wmissing-prototypes -Wimplicit-function-declaration -Wstrict-prototypes")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_FLAGS} -std=c++17")

elseif(CC_CLANG)
    # clang
    # on Ubuntu, must do: sudo apt-get install libc++abi-dev libc++-dev
    set(COMMON_FLAGS "${COMMON_FLAGS} \
        -Wno-unused-parameter \
        -fcolor-diagnostics \
        -Wno-missing-braces \
        -Wmissing-prototypes \
        ")

    # -fcatch-undefined-behavior \  # runtime checks

    # for fpng
    set(COMMON_FLAGS "${COMMON_FLAGS} -fdiagnostics-color=always")

    if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
        set(COMMON_FLAGS "${COMMON_FLAGS} -DFPNG_NO_SSE=0 -msse4.1 -mpclmul")
    else()
    endif()

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAGS} -std=gnu11")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_FLAGS} -stdlib=libc++ -std=c++17")
    set(CMAKE_OBJC_FLAGS "${CMAKE_OBJC_FLAGS} ${COMMON_FLAGS}
        -Wno-partial-availability -Wno-unguarded-availability-new")

elseif(CC_MSVC)
    # MSVC
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /std:c11")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif()

# -------------------------------------------------------------------------------------------------
# RPATH
# see https://gitlab.kitware.com/cmake/community/-/wikis/doc/cmake/RPATH-handling
# -------------------------------------------------------------------------------------------------

# use, i.e. don't skip the full RPATH for the build tree
set(CMAKE_SKIP_BUILD_RPATH FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# the RPATH to be used when installing, but only if it's not a system directory
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)

if("${isSystemDir}" STREQUAL "-1")
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
endif("${isSystemDir}" STREQUAL "-1")

# -------------------------------------------------------------------------------------------------
# Shaders compilation
# -------------------------------------------------------------------------------------------------
file(GLOB shader_sources
    "*/glsl/*.vert"
    "*/glsl/*.geom"
    "*/glsl/*.comp"
    "*/glsl/*.frag"
    "*/*/glsl/*.vert"
    "*/*/glsl/*.geom"
    "*/*/glsl/*.comp"
    "*/*/glsl/*.frag"
)

foreach(shader_source ${shader_sources})
    get_filename_component(shader_name ${shader_source} NAME)

    # NOTE: there was a bug because the shader output was a relative path, not a full path
    # and cmake believed the output was not properly created by the custom command glslc.
    set(shader_output "${SPIRV_DIR}/${shader_name}.spv")
    add_custom_command(
        OUTPUT ${shader_output}
        COMMAND ${GLSLC}
        -o "${shader_output}" ${shader_source}
        -I "${CMAKE_SOURCE_DIR}/include/datoviz/scene/glsl"
        DEPENDS ${shader_source}
        IMPLICIT_DEPENDS ${shader_source}
    )
    list(APPEND shader_outputs ${shader_output})
endforeach()

add_custom_target(shaders_spirv DEPENDS ${shader_outputs})

# NOTE: Only include graphics shaders in the embed resources files.
# file(GLOB embed_spirv "${SPIRV_DIR}/graphics_*.spv")
# message(${embed_spirv})
set(path_shadersc "${CMAKE_BINARY_DIR}/_shaders.c")
add_custom_command(
    OUTPUT ${path_shadersc}
    COMMAND ${CMAKE_COMMAND}
    -D DIR="${SPIRV_DIR}/*"
    -D PREFIX="shader"
    -D OUTPUT=${path_shadersc}
    -P "${CMAKE_SOURCE_DIR}/cmake/embed_resources.cmake"
    DEPENDS shaders_spirv ${shader_sources}
    IMPLICIT_DEPENDS shaders_spirv ${shader_sources}
)
add_custom_target(shaders DEPENDS ${path_shadersc})

# -------------------------------------------------------------------------------------------------
# Other binary resources
# -------------------------------------------------------------------------------------------------

# Textures.
set(path_tex "${CMAKE_BINARY_DIR}/_textures.c")
set(tex_files
    "${CMAKE_SOURCE_DIR}/data/textures/cmap_atlas.img"
)
add_custom_command(
    OUTPUT ${path_tex}
    COMMAND ${CMAKE_COMMAND}
    -D DIR="${tex_files}"
    -D PREFIX="texture"
    -D OUTPUT=${path_tex}
    -P "${CMAKE_SOURCE_DIR}/cmake/embed_resources.cmake"
)

# Fonts.
set(path_fonts "${CMAKE_BINARY_DIR}/_fonts.c")
set(font_files

    "${CMAKE_SOURCE_DIR}/data/fonts/Roboto-Medium.ttf"
    "${CMAKE_SOURCE_DIR}/data/fonts/Roboto_Medium_atlas.bin" # serialized font atlas

    "${CMAKE_SOURCE_DIR}/data/fonts/NotoSansMono-Light.ttf"
    "${CMAKE_SOURCE_DIR}/data/fonts/NotoEmoji-Regular.ttf"
    "${CMAKE_SOURCE_DIR}/data/fonts/fontawesome-webfont.ttf")
add_custom_command(
    OUTPUT ${path_fonts}
    COMMAND ${CMAKE_COMMAND}
    -D DIR="${font_files}"
    -D PREFIX="font"
    -D OUTPUT=${path_fonts}
    -P "${CMAKE_SOURCE_DIR}/cmake/embed_resources.cmake"
)

# Test data.
set(path_testdata "${CMAKE_BINARY_DIR}/_testdata.c")
set(testdata_files
    "${CMAKE_SOURCE_DIR}/data/textures/crate.rgba"
)
add_custom_command(
    OUTPUT ${path_testdata}
    COMMAND ${CMAKE_COMMAND}
    -D DIR="${testdata_files}"
    -D PREFIX="testdata"
    -D OUTPUT=${path_testdata}
    -P "${CMAKE_SOURCE_DIR}/cmake/embed_resources.cmake"
)

# -------------------------------------------------------------------------------------------------
# Source files
# -------------------------------------------------------------------------------------------------

# These files will be compiled with no warnings.
set(external_sources

    # "external/tinycthread.c"
    "external/vk_mem_alloc.cpp"
    "external/fpng.cpp"
    "external/memorymeasure.c"

    "external/b64/buffer.c"
    "external/b64/encode.c"
    "external/b64/decode.c"

    "external/imgui/imgui.cpp"
    "external/imgui/imgui_demo.cpp"
    "external/imgui/imgui_draw.cpp"
    "external/imgui/imgui_tables.cpp"
    "external/imgui/imgui_widgets.cpp"
    "external/imgui/backends/imgui_impl_glfw.cpp"
    "external/imgui/backends/imgui_impl_vulkan.cpp"
)

set(sources

    # Utils
    "src/_atomic.cpp"
    "src/_list.c"
    "src/_map.cpp"
    "src/_mutex.c"
    "src/_prng.cpp"
    "src/_thread.c"
    "src/client_input.c"
    "src/fifo.c"
    "src/fileio.cpp"
    "src/input.c"
    "src/keyboard.c"
    "src/log.c"
    "src/mouse.c"
    "src/timer.c"

    # Renderer
    "src/alloc.c"
    "src/board.c"
    "src/canvas.c"
    "src/context.c"
    "src/datalloc.c"
    "src/host.c"
    "src/loop.c"
    "src/pipe.c"
    "src/pipelib.c"
    "src/recorder.c"
    "src/renderer.cpp"
    "src/resources.c"
    "src/spirv.c"
    "src/surface.c"
    "src/transfers.c"
    "src/vklite.c"
    "src/workspace.c"

    # Client
    "src/client.c"
    "src/presenter.c"
    "src/request.c"
    "src/window.c"

    # GUI
    "src/gui.cpp"
    "src/fps.cpp"

    # Scene
    "src/scene/animation.c"
    "src/scene/app.c"
    "src/scene/arcball.c"
    "src/scene/array.c"
    "src/scene/atlas.cpp"
    "src/scene/axis.c"
    "src/scene/axes.c"
    "src/scene/baker.c"
    "src/scene/camera.c"
    "src/scene/dual.c"
    "src/scene/font.c"
    "src/scene/graphics.c"
    "src/scene/labels.c"
    "src/scene/meshobj.cpp"
    "src/scene/mvp.c"
    "src/scene/panzoom.c"
    "src/scene/params.c"
    "src/scene/scene.c"
    "src/scene/sdf.cpp"
    "src/scene/shape.c"
    "src/scene/ticks.c"
    "src/scene/transform.c"
    "src/scene/viewport.c"
    "src/scene/viewset.c"
    "src/scene/visual.c"

    # Visuals
    "src/scene/visuals/basic.c"
    "src/scene/visuals/fake_sphere.c"
    "src/scene/visuals/glyph.c"
    "src/scene/visuals/image.c"
    "src/scene/visuals/slice.c"
    "src/scene/visuals/mesh.c"
    "src/scene/visuals/path.c"
    "src/scene/visuals/pixel.c"
    "src/scene/visuals/point.c"
    "src/scene/visuals/marker.c"
    "src/scene/visuals/segment.c"
    "src/scene/visuals/volume.c"

    ${external_sources}
)

# -------------------------------------------------------------------------------------------------
# Datoviz shared library
# -------------------------------------------------------------------------------------------------
add_library(libdatoviz SHARED ${sources} ${path_shadersc} ${path_tex} ${path_fonts})
set_source_files_properties(${external_sources} PROPERTIES COMPILE_FLAGS -w)
set_target_properties(libdatoviz PROPERTIES
    VERSION ${DATOVIZ_VERSION}
    SOVERSION 2

    # PUBLIC_HEADER include/datoviz/datoviz.h
    OUTPUT_NAME "datoviz"
)

if(${OS_WIN32})
    set_target_properties(
        libdatoviz PROPERTIES
        OUTPUT_NAME "libdatoviz"
    )
endif()

add_dependencies(libdatoviz shaders)
target_compile_definitions(libdatoviz PUBLIC ${COMPILE_DEFINITIONS})
target_include_directories(libdatoviz PUBLIC ${INCL_DIRS})
target_link_libraries(libdatoviz ${LINK_LIBS})

# -------------------------------------------------------------------------------------------------
# CLI tool
# -------------------------------------------------------------------------------------------------
if(DATOVIZ_WITH_CLI)
    set(cli
        "cli/main.c"

        # Utils
        "tests/test_alloc.c"
        "tests/test_client_input.c"
        "tests/test_fifo.c"
        "tests/test_fileio.c"
        "tests/test_input.c"
        "tests/test_keyboard.c"
        "tests/test_list.c"
        "tests/test_map.c"
        "tests/test_mouse.c"
        "tests/test_obj.c"
        "tests/test_prng.c"
        "tests/test_thread.c"
        "tests/test_timer.c"

        # Renderer
        "tests/test_board.c"
        "tests/test_canvas.c"
        "tests/test_datalloc.c"
        "tests/test_gui.c"
        "tests/test_loop.c"
        "tests/test_pipe.c"
        "tests/test_pipelib.c"
        "tests/test_renderer.c"
        "tests/test_resources.c"
        "tests/test_transfers.c"
        "tests/test_vklite.c"
        "tests/test_workspace.c"

        # Client
        "tests/test_client.c"
        "tests/test_presenter.c"
        "tests/test_request.c"
        "tests/test_window.c"

        # Scene
        "tests/scene/test_animation.c"
        "tests/scene/test_app.c"
        "tests/scene/test_arcball.c"
        "tests/scene/test_array.c"
        "tests/scene/test_atlas.c"
        "tests/scene/test_axis.c"
        "tests/scene/test_axes.c"
        "tests/scene/test_baker.c"
        "tests/scene/test_camera.c"
        "tests/scene/test_colormaps.c"
        "tests/scene/test_dual.c"
        "tests/scene/test_font.c"
        "tests/scene/test_graphics.c"
        "tests/scene/test_labels.c"
        "tests/scene/test_mvp.c"
        "tests/scene/test_panzoom.c"
        "tests/scene/test_params.c"
        "tests/scene/test_sdf.c"
        "tests/scene/test_shape.c"
        "tests/scene/test_ticks.c"
        "tests/scene/test_viewset.c"
        "tests/scene/test_visual.c"

        # Visuals
        "tests/scene/visuals/test_basic.c"
        "tests/scene/visuals/test_fake_sphere.c"
        "tests/scene/visuals/test_glyph.c"
        "tests/scene/visuals/test_image.c"
        "tests/scene/visuals/test_slice.c"
        "tests/scene/visuals/test_mesh.c"
        "tests/scene/visuals/test_path.c"
        "tests/scene/visuals/test_pixel.c"
        "tests/scene/visuals/test_point.c"
        "tests/scene/visuals/test_marker.c"
        "tests/scene/visuals/test_segment.c"
        "tests/scene/visuals/test_volume.c"

        # Scene
        "tests/scene/test_scene.c"

        "tests/test.c"
    )

    add_executable(datovizcli ${cli} ${path_testdata})
    set_target_properties(datovizcli PROPERTIES OUTPUT_NAME "datoviz")
    target_compile_definitions(datovizcli PUBLIC ${COMPILE_DEFINITIONS})
    target_include_directories(datovizcli PUBLIC ${INCL_DIRS} ${CMAKE_SOURCE_DIR}/tests)
    target_link_libraries(datovizcli libdatoviz)
endif()
