-- premake5.lua

-- some const

project_dir = "./Projects"

-- Actions
newaction {
    trigger = "clean",
    description = "clean up project files",
    execute = function()
        os.rmdir(project_dir)
    end
}

-- Options
newoption {
    trigger = "with-opengl",
    description = "Using OpenGL as rendering Graphics API."
}

newoption {
    trigger = "with-physX",
    description = "Using OpenGL as rendering Graphics API."
}

workspace "ZooidEngine"
    location (project_dir .. "/" .. _ACTION)
    configurations { "Debug", "Release" }

project "ZooidEngine"
    location (project_dir .. "/".. _ACTION .."/ZooidEngine")
    kind "StaticLib"
    language "C++"
    
    staticruntime "on"

    targetdir "Binaries/%{cfg.buildcfg}"

    includedirs { 
        "Dependencies/freetype-2.9.1/include",
        "ZooidEngine",
        "./"
    }

    files { "ZooidEngine/**.h", 
            "ZooidEngine/**.cpp",
            "ZooidEngine/**.c"
        }
    
    -- exclude this file since it will be added on Platform specific
    removefiles { 
        "ZooidEngine/Renderer/GL/**" 
    }

    defines { "WIN32", "_CONSOLE", "_UNICODE", "UNICODE" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
    
    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
    
    filter "action:vs2015"
        postbuildcommands {
            "{COPY} \"" .. path.getabsolute("./Dependencies/freetype-2.9.1/win32/lib-vs2015/debug/*.dll") .. "\" %{cfg.targetdir}",
            "{COPY} \"" .. path.getabsolute("./Dependencies/freetype-2.9.1/win32/lib-vs2015/debug/*.pdb") .. "\" %{cfg.targetdir}"
        }

    filter "action:vs2017"
        postbuildcommands {
            "{COPY} \"" .. path.getabsolute("./Dependencies/freetype-2.9.1/win32/lib-vs2017/debug/*.dll") .. "\" %{cfg.targetdir}",
            "{COPY} \"" .. path.getabsolute("./Dependencies/freetype-2.9.1/win32/lib-vs2017/debug/*.pdb") .. "\" %{cfg.targetdir}"
        }
    
    -- basic configuration block for OpenGL
    configuration "with-opengl"
        
        includedirs {
            "Dependencies/glew-2.0.0/include",
            "Dependencies/glfw/include",
        }

        files { "ZooidEngine/Renderer/GL/**" }

        defines { "Z_RENDER_OPENGL" }

        postbuildcommands {
            "{COPY} \"" .. path.getabsolute("./Dependencies/glew-2.0.0/bin/Release/Win32/*.DLL") .. "\" %{cfg.targetdir}",
            "{COPY} \"" .. path.getabsolute("./Dependencies/glew-2.0.0/bin/Release/Win32/*.DLL") .. "\" %{cfg.targetdir}",
        }

    -- basic configuration block for PhysX
    configuration "with-physX"
    
        includedirs {
            "$(Physx)/../PxShared/include",
            "$(Physx)/Include"
        }

        defines { "Z_PHYSICS_PHYSX" }
    
    filter { "options:with-physX","action:vs2015" }
        postbuildcommands {
            "{COPY} \"$(PhysX)/Bin/vc14win32/*DEBUG_x86.DLL\" %{cfg.targetdir}",
            "{COPY} \"$(PhysX)/Bin/vc14win32/nvToolsExt32_1.DLL\" %{cfg.targetdir}",
            "{COPY} \"$(PhysX)/Bin/vc14win32/PhysXDevice32.DLL\" %{cfg.targetdir}",
            "{COPY} \"$(PhysX)/../PxShared/bin/vc14win32/*DEBUG_x86.DLL\" %{cfg.targetdir}",
            "{COPY} \"$(PhysX)/../PxShared/bin/vc14win32/*DEBUG_x86.pdb\" %{cfg.targetdir}",
        }
    
    filter { "options:with-physX","action:vs2017" }
        postbuildcommands {
            "{COPY} \"$(PhysX)/Bin/vc15win32/*DEBUG_x86.DLL\" %{cfg.targetdir}",
            "{COPY} \"$(PhysX)/Bin/vc15win32/nvToolsExt32_1.DLL\" %{cfg.targetdir}",
            "{COPY} \"$(PhysX)/Bin/vc15win32/PhysXDevice32.DLL\" %{cfg.targetdir}",
            "{COPY} \"$(PhysX)/../PxShared/bin/vc15win32/*DEBUG_x86.DLL\" %{cfg.targetdir}",
            "{COPY} \"$(PhysX)/../PxShared/bin/vc15win32/*DEBUG_x86.pdb\" %{cfg.targetdir}",
        }

group "Demo"

-- BEGIN: Default value for all project under this group
function setup_game_project()
    defines { "WIN32" }

    targetdir "Binaries/%{cfg.buildcfg}"
        
    includedirs {
        "ZooidEngine",
        "./"
    }

    staticruntime "on"

    filter "configurations:Debug"
            defines { "DEBUG" }
            symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

    dependson { "ZooidEngine" }

    configuration "*" 
        links { "ZooidEngine", "freetype" }

    filter "action:vs2015"
        libdirs {
            "./Dependencies/freetype-2.9.1/win32/lib-vs2015/debug"
        }
    
    filter "action:vs2017"
        libdirs {
            "./Dependencies/freetype-2.9.1/win32/lib-vs2017/debug"
        }

    -- basic configuration block for OpenGL
    configuration "with-opengl"
        libdirs {
            "./Dependencies/glew-2.0.0/lib/Release/Win32"
        }

        links {
            "glew32",
            "glfw3",
            "opengl32"
        }
    
    filter { "options:with-opengl","action:vs2015" }
        libdirs {
            "./Dependencies/glfw/Debug/lib-vc2015"
        }

    filter { "options:with-opengl","action:vs2017" }
        libdirs {
            "./Dependencies/glfw/Debug/lib-vc2017"
        }
    
    -- basic configuration block for PhysX
    configuration "with-physX"
        links {
            "LowLevelAABBDEBUG",
            "LowLevelClothDEBUG",
            "LowLevelDEBUG",
            "LowLevelDynamicsDEBUG",
            "LowLevelParticlesDEBUG",
            "PhysX3CharacterKinematicDEBUG_x86",
            "PhysX3CommonDEBUG_x86",
            "PhysX3CookingDEBUG_x86",
            "PhysX3DEBUG_x86",
            "PhysX3ExtensionsDEBUG",
            "PhysX3VehicleDEBUG",
            "SceneQueryDEBUG",
            "SimulationControllerDEBUG",
            "PsFastXmlDEBUG_x86",
            "PxFoundationDEBUG_x86",
            "PxPvdSDKDEBUG_x86",
            "PxTaskDEBUG_x86"
        }

    filter { "options:with-physX","action:vs2015" }
        libdirs {
            "$(Physx)/Lib/vc14win32",
            "$(Physx)/../PxShared/lib/vc14win32"
        }

    filter { "options:with-physX","action:vs2017" }
        libdirs {
            "$(Physx)/Lib/vc15win32",
            "$(Physx)/../PxShared/lib/vc15win32"
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

-- Tools Projects
group "Tools"

project "FontAssetGen"
    location (project_dir .. "/".. _ACTION .."/Tools")
    kind "ConsoleApp"
    language "C++"

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