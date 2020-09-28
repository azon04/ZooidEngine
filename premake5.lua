-- premake5.lua

-- some const

project_dir = "./Projects"

-- Actions
newaction {
    trigger = "clean",
    description = "clean up project files",
    execute = function()
        os.rmdir(project_dir)
        os.rmdir("./ZooidEngine.sln")
    end
}

-- Options
newoption {
    trigger = "with-opengl",
    description = "Using OpenGL as rendering Graphics API."
}

newoption {
    trigger = "with-dx11",
    description = "Using DirectX11 as rendering Graphics API."
}

newoption {
    trigger = "with-physX",
    description = "Using PhysX as main physics engine."
}

workspace "ZooidEngine"
    location ("./")
    configurations { "Debug", "Release" }
    platforms { "Win32", "Win64" }

    staticruntime "off"

    filter { "platforms:Win32" }
        system "Windows"
        architecture "x86"
    
    filter { "platforms:Win64" }
        system "Windows"
        architecture "x86_64"

project "ZooidEngine"
    location (project_dir .. "/".. _ACTION .."/ZooidEngine")
    kind "StaticLib"
    language "C++"

    targetdir "Binaries/%{cfg.platform}/%{cfg.buildcfg}"

    includedirs { 
        "Dependencies/freetype-2.9.1/include",
        "ZooidEngine",
        "./"
    }

    files { "ZooidEngine/**.h", 
            "ZooidEngine/**.cpp",
            "ZooidEngine/**.c",
            "ZooidEngine/**.natvis",
            "ZooidEngine/Shaders/**.vs",
            "ZooidEngine/Shaders/**.frag"
        }
    
    -- exclude this file since it will be added on Platform specific
    removefiles { 
        "ZooidEngine/Renderer/GL/**",
        "ZooidEngine/Renderer/DX11/**",
    }

    defines { "_CONSOLE", "_UNICODE", "UNICODE", "_CRT_SECURE_NO_WARNINGS" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
    
    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
    
    filter { "platforms:Win32", "action:vs2015" }
        postbuildcommands {
            "{COPY} \"" .. path.getabsolute("./Dependencies/freetype-2.9.1/win32/lib-vs2015/debug/*.dll") .. "\" %{cfg.targetdir}",
            "{COPY} \"" .. path.getabsolute("./Dependencies/freetype-2.9.1/win32/lib-vs2015/debug/*.pdb") .. "\" %{cfg.targetdir}"
        }

    filter { "platforms:Win32", "action:vs2017" }
        postbuildcommands {
            "{COPY} \"" .. path.getabsolute("./Dependencies/freetype-2.9.1/win32/lib-vs2017/debug/*.dll") .. "\" %{cfg.targetdir}",
            "{COPY} \"" .. path.getabsolute("./Dependencies/freetype-2.9.1/win32/lib-vs2017/debug/*.pdb") .. "\" %{cfg.targetdir}"
        }

    filter { "platforms:Win64", "action:vs2015" }
        postbuildcommands {
            "{COPY} \"" .. path.getabsolute("./Dependencies/freetype-2.9.1/x64/lib-vs2015/debug/*.dll") .. "\" %{cfg.targetdir}",
            "{COPY} \"" .. path.getabsolute("./Dependencies/freetype-2.9.1/x64/lib-vs2015/debug/*.pdb") .. "\" %{cfg.targetdir}"
        }

    filter { "platforms:Win64", "action:vs2017" }
        postbuildcommands {
            "{COPY} \"" .. path.getabsolute("./Dependencies/freetype-2.9.1/x64/lib-vs2017/debug/*.dll") .. "\" %{cfg.targetdir}",
            "{COPY} \"" .. path.getabsolute("./Dependencies/freetype-2.9.1/x64/lib-vs2017/debug/*.pdb") .. "\" %{cfg.targetdir}"
        }
    
    filter { "platforms:Win32" }
        defines{ "WIN32" }
    
    filter { "platforms:Win64" }
        defines{ "WIN64" }
    
    -- basic configuration block for OpenGL
    configuration "with-opengl"
        
        includedirs {
            "Dependencies/glew-2.0.0/include",
            "Dependencies/glfw/include",
        }

        files { "ZooidEngine/Renderer/GL/**" }

        defines { "ZE_RENDER_OPENGL=1" }

        filter { "platforms:Win32" }
            postbuildcommands {
                "{COPY} \"" .. path.getabsolute("./Dependencies/glew-2.0.0/bin/Release/Win32/*.DLL") .. "\" %{cfg.targetdir}",
                "{COPY} \"" .. path.getabsolute("./Dependencies/glew-2.0.0/bin/Release/Win32/*.DLL") .. "\" %{cfg.targetdir}",
            }
        
        filter { "platforms:Win64" }
            postbuildcommands {
                "{COPY} \"" .. path.getabsolute("./Dependencies/glew-2.0.0/bin/Release/x64/*.DLL") .. "\" %{cfg.targetdir}",
                "{COPY} \"" .. path.getabsolute("./Dependencies/glew-2.0.0/bin/Release/x64/*.DLL") .. "\" %{cfg.targetdir}",
            }
    
    -- basic configuration block for DirectX11
    configuration "with-dx11"
        
        files { "ZooidEngine/Renderer/DX11/**" }

        directSDKPath = os.getenv( "DXSDK_DIR" )
        sysincludedirs { directSDKPath.."Include" }
        syslibdirs { directSDKPath.."Lib/x86" }

    -- basic configuration block for PhysX
    configuration "with-physX"
    
        includedirs {
            "$(Physx)/../PxShared/include",
            "$(Physx)/Include"
        }
    
    filter { "options:with-physX", "action:vs2015", "platforms:Win32" }
        postbuildcommands {
            "{COPY} \"$(PhysX)/Bin/vc14win32/*DEBUG_x86.DLL\" %{cfg.targetdir}",
            "{COPY} \"$(PhysX)/Bin/vc14win32/nvToolsExt32_1.DLL\" %{cfg.targetdir}",
            "{COPY} \"$(PhysX)/Bin/vc14win32/PhysXDevice32.DLL\" %{cfg.targetdir}",
            "{COPY} \"$(PhysX)/../PxShared/bin/vc14win32/*DEBUG_x86.DLL\" %{cfg.targetdir}",
            "{COPY} \"$(PhysX)/../PxShared/bin/vc14win32/*DEBUG_x86.pdb\" %{cfg.targetdir}",
        }
    
    filter { "options:with-physX","action:vs2017", "platforms:Win32" }
        postbuildcommands {
            "{COPY} \"$(PhysX)/Bin/vc15win32/*DEBUG_x86.DLL\" %{cfg.targetdir}",
            "{COPY} \"$(PhysX)/Bin/vc15win32/nvToolsExt32_1.DLL\" %{cfg.targetdir}",
            "{COPY} \"$(PhysX)/Bin/vc15win32/PhysXDevice32.DLL\" %{cfg.targetdir}",
            "{COPY} \"$(PhysX)/../PxShared/bin/vc15win32/*DEBUG_x86.DLL\" %{cfg.targetdir}",
            "{COPY} \"$(PhysX)/../PxShared/bin/vc15win32/*DEBUG_x86.pdb\" %{cfg.targetdir}",
        }

    filter { "options:with-physX", "action:vs2015", "platforms:Win64" }
        postbuildcommands {
            "{COPY} \"$(PhysX)/Bin/vc14win64/*DEBUG_x64.DLL\" %{cfg.targetdir}",
            "{COPY} \"$(PhysX)/Bin/vc14win64/nvToolsExt64_1.DLL\" %{cfg.targetdir}",
            "{COPY} \"$(PhysX)/Bin/vc14win64/PhysXDevice64.DLL\" %{cfg.targetdir}",
            "{COPY} \"$(PhysX)/../PxShared/bin/vc14win64/*DEBUG_x64.DLL\" %{cfg.targetdir}",
            "{COPY} \"$(PhysX)/../PxShared/bin/vc14win64/*DEBUG_x64.pdb\" %{cfg.targetdir}",
        }
    
    filter { "options:with-physX","action:vs2017", "platforms:Win64" }
        postbuildcommands {
            "{COPY} \"$(PhysX)/Bin/vc15win64/*DEBUG_x64.DLL\" %{cfg.targetdir}",
            "{COPY} \"$(PhysX)/Bin/vc15win64/nvToolsExt64_1.DLL\" %{cfg.targetdir}",
            "{COPY} \"$(PhysX)/Bin/vc15win64/PhysXDevice64.DLL\" %{cfg.targetdir}",
            "{COPY} \"$(PhysX)/../PxShared/bin/vc15win64/*DEBUG_x64.DLL\" %{cfg.targetdir}",
            "{COPY} \"$(PhysX)/../PxShared/bin/vc15win64/*DEBUG_x64.pdb\" %{cfg.targetdir}",
        }

group "Demo"

-- BEGIN: Default value for all project under this group
function setup_game_project()

    includedirs {
        "ZooidEngine",
        "./"
    }
    
    targetdir "Binaries/%{cfg.platform}/%{cfg.buildcfg}"

    dependson { "ZooidEngine" }

    filter { "platforms:Win32" }
        defines { "WIN32" }

    filter { "platforms:Win64" }
        defines { "WIN64" }
        
    filter "configurations:Debug"
            defines { "DEBUG" }
            symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

    configuration "*" 
        links { "ZooidEngine", "freetype" }

    filter { "action:vs2015", "platforms:Win32" }
        libdirs {
            "./Dependencies/freetype-2.9.1/win32/lib-vs2015/debug"
        }
    
    filter { "action:vs2017", "platforms:Win32" }
        libdirs {
            "./Dependencies/freetype-2.9.1/win32/lib-vs2017/debug"
        }
        
    filter { "action:vs2015", "platforms:Win64" }
        libdirs {
            "./Dependencies/freetype-2.9.1/x64/lib-vs2015/debug"
        }

    filter { "action:vs2017", "platforms:Win64" }
        libdirs {
            "./Dependencies/freetype-2.9.1/x64/lib-vs2017/debug"
        }

    -- basic configuration block for OpenGL
    configuration "with-opengl"
        links {
            "glew32",
            "glfw3",
            "opengl32"
        }
    
    filter { "options:with-opengl", "platforms:Win32" }    
        libdirs {
            "./Dependencies/glew-2.0.0/lib/Release/Win32"
        }
    
    filter { "options:with-opengl", "platforms:Win64" }    
        libdirs {
            "./Dependencies/glew-2.0.0/lib/Release/x64"
        }

    filter { "options:with-opengl", "action:vs2015", "platforms:Win32" }
        libdirs {
            "./Dependencies/glfw/Debug/win32/lib-vc2015"
        }

    filter { "options:with-opengl", "action:vs2017", "platforms:Win32"  }
        libdirs {
            "./Dependencies/glfw/Debug/win32/lib-vc2017"
        }

    filter { "options:with-opengl", "action:vs2015", "platforms:Win64" }
        libdirs {
            "./Dependencies/glfw/Debug/x64/lib-vc2015"
        }

    filter { "options:with-opengl", "action:vs2017", "platforms:Win64"  }
        libdirs {
            "./Dependencies/glfw/Debug/x64/lib-vc2017"
        }
    
    -- basic configuration block for PhysX
    configuration "with-physX"
        links {
            "LowLevelAABBDEBUG",
            "LowLevelClothDEBUG",
            "LowLevelDEBUG",
            "LowLevelDynamicsDEBUG",
            "LowLevelParticlesDEBUG",            
            "PhysX3ExtensionsDEBUG",
            "PhysX3VehicleDEBUG",
            "SceneQueryDEBUG",
            "SimulationControllerDEBUG"
        }

    filter { "options:with-physX", "platforms:Win32" }
        links {
            "PhysX3CharacterKinematicDEBUG_x86",
            "PhysX3CommonDEBUG_x86",
            "PhysX3CookingDEBUG_x86",
            "PhysX3DEBUG_x86",
            "PsFastXmlDEBUG_x86",
            "PxFoundationDEBUG_x86",
            "PxPvdSDKDEBUG_x86",
            "PxTaskDEBUG_x86"
        }

    filter { "options:with-physX", "platforms:Win64" }
        links {
            "PhysX3CharacterKinematicDEBUG_x64",
            "PhysX3CommonDEBUG_x64",
            "PhysX3CookingDEBUG_x64",
            "PhysX3DEBUG_x64",
            "PsFastXmlDEBUG_x64",
            "PxFoundationDEBUG_x64",
            "PxPvdSDKDEBUG_x64",
            "PxTaskDEBUG_x64"
        }

    filter { "options:with-physX", "action:vs2015", "platforms:Win32" }
        libdirs {
            "$(Physx)/Lib/vc14win32",
            "$(Physx)/../PxShared/lib/vc14win32"
        }

    filter { "options:with-physX", "action:vs2017", "platforms:Win32" }
        libdirs {
            "$(Physx)/Lib/vc15win32",
            "$(Physx)/../PxShared/lib/vc15win32"
        }

    filter { "options:with-physX", "action:vs2015", "platforms:Win64" }
        libdirs {
            "$(Physx)/Lib/vc14win64",
            "$(Physx)/../PxShared/lib/vc14win64"
        }

    filter { "options:with-physX", "action:vs2017", "platforms:Win64" }
        libdirs {
            "$(Physx)/Lib/vc15win64",
            "$(Physx)/../PxShared/lib/vc15win64"
        }

end
-- END

project "SceneViewer"
    location (project_dir .. "/".. _ACTION .."/Demo")
    kind "WindowedApp"
    language "C++"

    files
    {
        "Demo/SceneViewer/**"
    }

    debugdir "./"
    debugargs { "Basic/Scene/LightTest.scz --log" }

    setup_game_project()

project "AnimationDemo"
    location (project_dir .. "/".. _ACTION .."/Demo")
    kind "WindowedApp"
    language "C++"

    files
    {
        "Demo/AnimationDemo/**"
    }

    debugdir "./"
    debugargs { "--log" }

    setup_game_project()

project "PhysicsSandBox"
    location (project_dir .. "/".. _ACTION .."/Demo")
    kind "WindowedApp"
    language "C++"

    files
    {
        "Demo/PhysicsSandBox/**"
    }

    debugdir "./"
    debugargs { "--log" }

    setup_game_project()

project "SpaceShooter"
    location (project_dir .. "/".. _ACTION .."/Demo")
    kind "WindowedApp"
    language "C++"

    files
    {
        "Demo/SpaceShooter/**"
    }

    debugdir "./"
    debugargs { "--log" }

    setup_game_project()

project "SceneEditor"
    location (project_dir .. "/".. _ACTION .."/Demo")
    kind "WindowedApp"
    language "C++"

    files
    {
        "Demo/SceneEditor/**"
    }

    debugdir "./"
    debugargs { "Basic/Scene/LightTest.scz --log" }

    setup_game_project()

-- Tools Projects
group "Tools"

project "FontAssetGen"
    location (project_dir .. "/".. _ACTION .."/Tools")
    kind "ConsoleApp"
    language "C++"

    removeplatforms{ "Win64" }

    targetdir "Binaries/Tools"

    staticruntime "on"
    
    debugdir "./"
    debugargs { "Resources/Basic/Font/open-sans/OpenSans-Light.ttf -o Resources -p Basic -pxh 54 -rm ALL" }

    includedirs {
        "Dependencies/msdfgen",
        "Dependencies/msdfgen/include"
    }

    files {
        "Tools/Common/**",
        "Tools/FontAssetGen/**"
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
    
    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

    configuration "*"
        libdirs {
            "Dependencies/Builds/msdfgen/win32_vs14",
            "Dependencies/msdfgen/lib"
        }

        links {
            "msdfgen",
            "freetype"
        }

project "ModelParser"
    location (project_dir .. "/".. _ACTION .."/Tools")
    kind "ConsoleApp"
    language "C++"
    
    removeplatforms{ "Win64" }

    targetdir "Binaries/Tools"

    staticruntime "on"

    debugdir "./"
    debugargs { "Resources/Raw/RifleAimingAdditiveRight_Baked.fbx --out Resources --package TestAnimation --omitAnimChannel S --noMesh --skeletonPath TestAnimation/Skeleton/Walking_baked_skel.skelz --reference Resources/Raw/RifleAimingIdle_Baked.fbx" }

    includedirs {
        "Dependencies/Builds/assimp/include"
    }

    files {
        "Tools/Common/**",
        "Tools/ModelParser/**"
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

    filter "action:vs2015"
        libdirs {
            "Dependencies/Builds/assimp/Debug/x86-vs140"
        }

        links {
            "assimp-vc140-mt"
        }

        postbuildcommands {
            "{COPY} \"" .. path.getabsolute("./Dependencies/Builds/assimp/Debug/x86-vs140/*.dll") .. "\" %{cfg.targetdir}",
            "{COPY} \"" .. path.getabsolute("./Dependencies/Builds/assimp/Debug/x86-vs140/*.pdb") .. "\" %{cfg.targetdir}"
        }
        
    
    filter "action:vs2017"
        libdirs {
            "Dependencies/Builds/assimp/Debug/x86-vs141"
        }

        links {
            "assimp-vc141-mt"
        }

        postbuildcommands {
            "{COPY} \"" .. path.getabsolute("./Dependencies/Builds/assimp/Debug/x86-vs141/*.dll") .. "\" %{cfg.targetdir}",
            "{COPY} \"" .. path.getabsolute("./Dependencies/Builds/assimp/Debug/x86-vs141/*.pdb") .. "\" %{cfg.targetdir}"
        }