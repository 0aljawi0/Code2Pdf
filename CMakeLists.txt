cmake_minimum_required (VERSION 3.16)

set(X_VCPKG_APPLOCAL_DEPS_INSTALL ON CACHE BOOL "Install vcpkg dependencies locally")

project (Code2Pdf CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (WIN32)
    enable_language(RC)
endif()

find_package(glfw3 REQUIRED)

set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/third_party/imgui)
set(IMGUI_SOURCE
	${IMGUI_DIR}/imgui.cpp
	${IMGUI_DIR}/imgui_draw.cpp
	${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

add_executable (Code2Pdf WIN32
	src/main.cpp
	src/core/PathUtils.cpp
    src/core/Recommends.cpp
    src/core/Scanner.cpp
    src/core/TreeBuilder.cpp
    src/core/PdfWriter.cpp
    src/ui/Theme.cpp
    src/ui/Widgets.cpp
    src/ui/SettingsDialog.cpp
    src/ui/SuccessDialog.cpp
    src/ui/AppWindow.cpp
	${IMGUI_SOURCE}
)

if (WIN32)
    set_target_properties(Code2Pdf PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
endif()

if (WIN32)
	target_sources(Code2Pdf PRIVATE 
        "resources.rc"
        src/platform/Win32DialogService.cpp
    )
    target_include_directories(Code2Pdf PRIVATE ${CMAKE_SOURCE_DIR})
    target_link_libraries(Code2Pdf PRIVATE glfw opengl32 gdi32 user32 shell32 ole32 uuid comdlg32 dwmapi)
else()
	target_link_libraries(Code2Pdf PRIVATE glfw GL dl pthread)
endif()

target_include_directories(Code2Pdf PRIVATE
    src
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)