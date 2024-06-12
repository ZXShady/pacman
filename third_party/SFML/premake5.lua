local SFML = {}

SFML.add_libdir = function(compiler)
    filter("architecture:x86")
    libdirs("extlibs/libs-" .. compiler .. "/x86")
    filter("architecture:x86_64")
    libdirs("extlibs/libs-" .. compiler .. "/x64")
    filter({})
end

project("SFML")
  language("C++")
  cppdialect("C++17")
  kind("StaticLib")
  local cwd = os.getcwd()
  local OutputDir = "%{cfg.buildcfg}"
  local ObjectDir = "bin-int/" .. OutputDir
  local TargetDir = "bin/" .. OutputDir
  local SrcDirs = {
      "Graphics","Audio","Network","System","Window"
  }
  local DisableDeprecationsMacros = {
      "_WINSOCK_DEPRECATED_NO_WARNINGS",
      "_CRT_SECURE_NO_WARNINGS"
  }
  local ExtLibsIncludeDirs = {
      "AL","freetype2",
      "glad/include","mingw",
      "miniaudio","minimp3",
      "ogg","stb_image",
      "vorbis","vulkan"
  }

  targetdir(TargetDir)
  objdir(ObjectDir)

  -- remove all files
  filter({ "files:examples/**.*" })
    flags({"ExcludeFromBuild"})
  filter({})


  includedirs("extlibs/headers")
  for _,value in pairs(ExtLibsIncludeDirs) do
    includedirs("extlibs/headers/" .. value)
  end

  filter("architecture:x86")
    libdirs("extlibs/bin/x86")
  filter("architecture:x86_64")
    libdirs("extlibs/bin/x64")
  filter({})

  includedirs({"include"})
  includedirs({"src"})


  for _,value in pairs(DisableDeprecationsMacros) do
    defines(value)
  end

  defines("SFML_STATIC")
  defines("FLAC__NO_DLL")
  defines("OV_EXCLUDE_STATIC_CALLBACKS")

  
  files({"include/**.inl","include/**.hpp"})

  
  for _,value in pairs(SrcDirs) do
    files("src/SFML/" .. value .."/*.inl")
    files("src/SFML/" .. value .."/*.cpp")
    files("src/SFML/" .. value .."/*.hpp")
  end

  filter("system:windows")
    defines({"UNICODE" ,"_UNICODE"})
    files("src/SFML/Main/MainWin32.cpp")
      for _,value in pairs(SrcDirs) do
      files("src/SFML/" .. value .."/Win32/*.inl")
      files("src/SFML/" .. value .."/Win32/*.cpp")
      files("src/SFML/" .. value .."/Win32/*.hpp")
    end
  filter({})

  filter("toolset:msc*")
    SFML.add_libdir("msvc")
  filter("toolset:gcc* or clang*")
    SFML.add_libdir("mingw")
  filter({})

  links({"openal32","opengl32","freetype","flac"
  ,"vorbisenc","vorbisfile","vorbis","ogg"})

  filter("system:linux or freebsd or macosx")
    links("pthread")
  filter("system:linux")
    links("rt")
  filter("system:windows")
    links("winmm")
    links("legacy_stdio_definitions")
  filter("system:android")
    links({"android","log"})
  filter({})

  warnings("off")
  disablewarnings(4068) -- Unknown Pragma
  disablewarnings(4996) -- C4996 can occur if you use MFC or ATL functions that were deprecated for security reasons
  filter("toolset:msc*")
    defines(_CRT_SECURE_NO_WARNINGS)
  filter({})



  function add_SFML()
    defines("SFML_STATIC")
    libdirs(cwd .. "/" .. TargetDir)
    links("SFML")
    includedirs(cwd .. "/include")
  end
