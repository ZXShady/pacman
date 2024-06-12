workspace("Pacman Game")
  configurations({"Debug","Release","Distribution"})
  platforms({"x64","x86"})

  startproject("Pacman")
  filter("platforms:x64")
    architecture("x86_64")
  filter("platforms:x86")
    architecture("x86")

  filter("configurations:Debug")
    optimize("debug")
    symbols("on")
    defines("PACMAN_DEBUG")
  filter("configurations:Release")
    optimize("on")
    symbols("off")
    defines("PACMAN_RELEASE")
  filter("configurations:Distribution")
    optimize("speed")
    symbols("off")
    defines("PACMAN_DISTRIBUTION")
  filter({})
    include("third_party/SFML/premake5.lua")

project("Pacman")
  kind("WindowedApp")
  local OutputDir = "%{cfg.buildcfg}"
  local ObjectDir = "bin-int/" .. OutputDir
  local TargetDir = "bin/" .. OutputDir
  targetdir(TargetDir)
  objdir(ObjectDir)

	add_SFML()

  files({"src/**.cpp","src/**.hpp"})