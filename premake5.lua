-- Defaults platform toolset to XP in visual studio
premake.override(premake.vstudio.vc2010, 'platformToolset', function (base, cfg)
	local map = { vs2012 = "v110_xp", vs2013 = "v120_xp" }
	local value = map[_ACTION]
	if value then
		-- should only be written if there is a C/C++ file in the config
		for i = 1, #cfg.files do
			if path.iscppfile(cfg.files[i]) then
				_p(2,'<PlatformToolset>%s</PlatformToolset>', value)
				return
			end
		end
	end
end)

os.mkdir("src")
os.mkdir("projects")

solution "SXPong"
    configurations { "Debug", "Release" }
    platforms { "x32" }
    language "C++"
    targetdir "$(SolutionDir)build/$(Configuration)/$(ProjectName)/"
    debugdir "$(SolutionDir)build/$(Configuration)/$(ProjectName)/"
    objdir "$(SolutionDir)temp"
    libdirs { "$(SolutionDir)/lib" }
	flags { "unicode" }

    configuration "Debug"
		defines { "_DEBUG" }
		flags {"StaticRuntime", "Symbols"}

    configuration "Release"
	    defines { "NDEBUG" }
		flags {"StaticRuntime", "Symbols", "Optimize" }

    project "SXPong"
	    location "projects/SXPong"
	    includedirs {
			"$(SolutionDir)src/SXPong",
			"$(SolutionDir)src/libpng1619",
			"$(SolutionDir)src/zlib-1.2.8"}
	    files {
			"src/SXPong/**",
			"src/zlib-1.2.8/**",
			"src/libpng1619/**"}
        flags { "WinMain" }
        kind "WindowedApp"
