newoption
{
	trigger = "graphics",
	value = "OPENGL_VERSION",
	description = "version of OpenGL to build raylib against",
	allowed = {
		{ "opengl11", "OpenGL 1.1"},
		{ "opengl21", "OpenGL 2.1"},
		{ "opengl33", "OpenGL 3.3"},
		{ "opengl43", "OpenGL 4.3"},
		{ "openges2", "OpenGL ES2"},
		{ "openges3", "OpenGL ES3"}
	},
	default = "opengl33"
}

function download_progress(total, current)
    local ratio = current / total;
    ratio = math.min(math.max(ratio, 0), 1);
    local percent = math.floor(ratio * 100);
    print("Download progress (" .. percent .. "%/100%)")
end

function check_raylib()
    os.chdir("external")
    if(os.isdir("raylib-master") == false) then
        if(not os.isfile("raylib-master.zip")) then
            print("Raylib not found, downloading from github")
            local result_str, response_code = http.download("https://github.com/raysan5/raylib/archive/refs/heads/master.zip", "raylib-master.zip", {
                progress = download_progress,
                headers = { "From: Premake", "Referer: Premake" }
            })
        end
        print("Unzipping to " ..  os.getcwd())
        zip.extract("raylib-master.zip", os.getcwd())
        os.remove("raylib-master.zip")
    end
    os.chdir("../")
end

function check_raygui()
    os.chdir("external")
    if(not os.isfile("raygui.h")) then
        print("Raygui not found, downloading from github")
        local result_str, response_code = http.download("https://github.com/raysan5/raygui/raw/refs/heads/master/src/raygui.h", "raygui.h", {
            progress = download_progress,
            headers = { "From: Premake", "Referer: Premake" }
        })
    end
end

function check_gtest()
    os.chdir("external")
    if(os.isdir("googletest-main") == false) then
        if(not os.isfile("googletest-main.zip")) then
            print("googletest not found, downloading from github")
            local result_str, response_code = http.download("https://github.com/google/googletest/archive/refs/heads/main.zip", "googletest-main.zip", {
                progress = download_progress,
                headers = { "From: Premake", "Referer: Premake" }
            })
        end
        print("Unzipping to " ..  os.getcwd())
        zip.extract("googletest-main.zip", os.getcwd())
        os.remove("googletest-main.zip")
    end
    os.chdir("../")
end

function build_externals()
    print("calling externals")
    check_raylib()
    check_raygui()
    check_gtest()
end

function platform_defines()
    filter {"configurations:Debug or Release"}
        defines{"PLATFORM_DESKTOP"}

    filter {"configurations:Debug_RGFW or Release_RGFW"}
        defines{"PLATFORM_DESKTOP_RGFW"}

    filter {"options:graphics=opengl43"}
        defines{"GRAPHICS_API_OPENGL_43"}

    filter {"options:graphics=opengl33"}
        defines{"GRAPHICS_API_OPENGL_33"}

    filter {"options:graphics=opengl21"}
        defines{"GRAPHICS_API_OPENGL_21"}

    filter {"options:graphics=opengl11"}
        defines{"GRAPHICS_API_OPENGL_11"}

    filter {"options:graphics=openges3"}
        defines{"GRAPHICS_API_OPENGL_ES3"}

    filter {"options:graphics=openges2"}
        defines{"GRAPHICS_API_OPENGL_ES2"}

    filter {"system:macosx"}
        disablewarnings {"deprecated-declarations"}

    filter {"system:linux"}
        defines {"_GLFW_X11"}
        defines {"_GNU_SOURCE"}
-- This is necessary, otherwise compilation will fail since
-- there is no CLOCK_MONOTOMIC. raylib claims to have a workaround
-- to compile under c99 without -D_GNU_SOURCE, but it didn't seem
-- to work. raylib's Makefile also adds this flag, probably why it went
-- unnoticed for so long.
-- It compiles under c11 without -D_GNU_SOURCE, because c11 requires
-- to have CLOCK_MONOTOMIC
-- See: https://github.com/raysan5/raylib/issues/2729

    filter{}
end

raylib_dir = "external/raylib-master"
gtest_dir = "external/googletest-main"

baseName = path.getbasename(path.getdirectory(os.getcwd()));
workspaceName = baseName

if (os.isdir('build_files') == false) then
    os.mkdir('build_files')
end

if (os.isdir('external') == false) then
    os.mkdir('external')
end

workspace (workspaceName)
    location "../"
    configurations { "Debug", "Release", "Debug_RGFW", "Release_RGFW"}
    platforms { "x64", "x86", "ARM64"}

    defaultplatform ("x64")

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

    filter { "platforms:x64" }
        architecture "x86_64"

    filter { "platforms:Arm64" }
        architecture "ARM64"

    filter {}

    targetdir "bin/%{cfg.buildcfg}/"

    build_externals()

    startproject(workspaceName)

    project (workspaceName)
        kind "ConsoleApp"
        location "build_files/"
        targetdir "../bin/%{cfg.buildcfg}"

        filter {"system:windows", "configurations:Release", "action:gmake*"}
            kind "WindowedApp"
            buildoptions { "-Wl,--subsystem,windows" }

        filter {"system:windows", "configurations:Release", "action:vs*"}
            kind "WindowedApp"
            entrypoint "mainCRTStartup"

        filter "action:vs*"
            debugdir "$(SolutionDir)"

        filter{}

        vpaths
        {
            ["Header Files/*"] = { "../include/**.h",  "../include/**.hpp", "../src/**.h", "../src/**.hpp"},
            ["Source Files/*"] = {"../src/**.c", "src/**.cpp"},
        }
        files {"../src/**.c", "../src/**.cpp", "../src/**.h", "../src/**.hpp", "../include/**.h", "../include/**.hpp"}

        includedirs { "../src" }
        includedirs { "../include" }
        includedirs { "./external" }

        links {"raylib"}

        cdialect "C99"
        cppdialect "C++17"

        includedirs {raylib_dir .. "/src" }
        includedirs {raylib_dir .."/src/external" }
        includedirs { raylib_dir .."/src/external/glfw/include" }
        flags { "ShadowedVariables"}
        platform_defines()

        filter "action:vs*"
            defines{"_WINSOCK_DEPRECATED_NO_WARNINGS", "_CRT_SECURE_NO_WARNINGS"}
            dependson {"raylib"}
            links {"raylib.lib"}
            characterset ("Unicode")
            buildoptions { "/Zc:__cplusplus" }

        filter "system:windows"
            defines{"_WIN32"}
            links {"winmm", "gdi32", "opengl32"}
            libdirs {"../bin/%{cfg.buildcfg}"}

        filter "system:linux"
            links {"pthread", "m", "dl", "rt", "X11"}

        filter "system:macosx"
            links {"OpenGL.framework", "Cocoa.framework", "IOKit.framework", "CoreFoundation.framework", "CoreAudio.framework", "CoreVideo.framework", "AudioToolbox.framework"}

        filter{}


    project "raylib"
        kind "StaticLib"

        platform_defines()

        location "build_files/"

        language "C"
        targetdir "../bin/%{cfg.buildcfg}"

        filter "action:vs*"
            defines{"_WINSOCK_DEPRECATED_NO_WARNINGS", "_CRT_SECURE_NO_WARNINGS"}
            characterset ("Unicode")
            buildoptions { "/Zc:__cplusplus" }
        filter{}

        includedirs {raylib_dir .. "/src", raylib_dir .. "/src/external/glfw/include" }
        vpaths
        {
            ["Header Files"] = { raylib_dir .. "/src/**.h"},
            ["Source Files/*"] = { raylib_dir .. "/src/**.c"},
        }
        files {raylib_dir .. "/src/*.h", raylib_dir .. "/src/*.c"}

        removefiles {raylib_dir .. "/src/rcore_*.c"}

        filter { "system:macosx", "files:" .. raylib_dir .. "/src/rglfw.c" }
            compileas "Objective-C"

        filter{}

    project "googletest"
        kind "StaticLib"

        platform_defines()

        location "build_files/"

        cppdialect "C++17"
        targetdir "../bin/%{cfg.buildcfg}"

        filter "action:vs*"
            defines{"_WINSOCK_DEPRECATED_NO_WARNINGS", "_CRT_SECURE_NO_WARNINGS"}
            characterset ("Unicode")
            buildoptions { "/Zc:__cplusplus" }
        filter{}

        includedirs {gtest_dir .. "/googletest", gtest_dir .. "/googletest/include" }
        includedirs {gtest_dir .. "/googlemock", gtest_dir .. "/googlemock/include" }

        vpaths
        {
            ["Header Files"] = { gtest_dir .. "/googletest/include/**.h", gtest_dir .. "/googlemock/include/**.h"},
            ["Source Files/*"] = { gtest_dir .. "/googletest/src/**.cc",  gtest_dir .. "/googlemock/src/**.cc"},
        }

        files {gtest_dir .. "/src/*.h"}
        -- files {gtest_dir .. "/googletest/src/gtest_main.cc"}
        files {gtest_dir .. "/googletest/src/gtest-all.cc"}
        files {gtest_dir .. "/googlemock/src/gmock_main.cc"}
        files {gtest_dir .. "/googlemock/src/gmock-all.cc"}

        filter { "system:macosx", "files:" .. gtest_dir .. "/src/rglfw.c" }
            compileas "Objective-C"

        filter{}

    project "unit_test"
        kind "ConsoleApp"
        location "build_files/"
        targetdir "../bin/%{cfg.buildcfg}"

        filter {"system:windows", "configurations:Release", "action:gmake*"}
            kind "WindowedApp"
            buildoptions { "-Wl,--subsystem,windows" }

        filter {"system:windows", "configurations:Release", "action:vs*"}
            kind "WindowedApp"
            entrypoint "mainCRTStartup"

        filter "action:vs*"
            debugdir "$(SolutionDir)"

        filter{}

        vpaths
        {
            ["Header Files/*"] = { "../include/**.h", "../src/**.h"},
            ["Source Files/*"] = {"../test/**.c", "test/**.cc"},
        }
        files {"../test/**.cc", "../src/**.h",  "../include/**.h"}

        includedirs { "../src" }
        includedirs { "../include" }

        includedirs {raylib_dir .. "/src" }
        includedirs {raylib_dir .."/src/external" }
        includedirs { raylib_dir .."/src/external/glfw/include" }
        
        links {"googletest"}
        links {"raylib"}

        cdialect "C99"
        cppdialect "C++17"

        includedirs {gtest_dir .. "/googletest", gtest_dir .. "/googletest/include" }
        includedirs {gtest_dir .. "/googlemock", gtest_dir .. "/googlemock/include" }
        flags { "ShadowedVariables"}
        platform_defines()

        filter "action:vs*"
            defines{"_WINSOCK_DEPRECATED_NO_WARNINGS", "_CRT_SECURE_NO_WARNINGS"}
            dependson {"googletest"}
            links {"googletest.lib"}
            characterset ("Unicode")
            buildoptions { "/Zc:__cplusplus" }

        filter "system:windows"
            defines{"_WIN32"}
            links {"winmm", "gdi32", "opengl32"}
            libdirs {"../bin/%{cfg.buildcfg}"}

        filter "system:linux"
            links {"pthread", "m", "dl", "rt", "X11"}

        filter "system:macosx"
            links {"OpenGL.framework", "Cocoa.framework", "IOKit.framework", "CoreFoundation.framework", "CoreAudio.framework", "CoreVideo.framework", "AudioToolbox.framework"}

        filter{}
