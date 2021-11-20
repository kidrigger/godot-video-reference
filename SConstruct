#!/usr/bin/env python
import os
import sys

# Try to detect the host platform automatically.
# This is used if no `platform` argument is passed
if sys.platform.startswith("linux"):
    host_platform = "linux"
elif sys.platform == "darwin":
    host_platform = "osx"
elif sys.platform == "win32" or sys.platform == "msys":
    host_platform = "windows"
else:
    raise ValueError("Could not detect platform automatically, please specify with " "platform=<platform>")

env = Environment(ENV=os.environ)

opts = Variables([], ARGUMENTS)

# Define our options
opts.Add(EnumVariable("target", "Compilation target", "debug", ["d", "debug", "r", "release"]))
opts.Add(EnumVariable("platform", "Compilation platform", host_platform, ["", "windows", "x11", "linux", "osx"]))
opts.Add(
    EnumVariable("p", "Compilation target, alias for 'platform'", host_platform, ["", "windows", "x11", "linux", "osx"])
)
opts.Add(EnumVariable("bits", "Target platform bits", "64", ("32", "64")))
opts.Add(BoolVariable("use_llvm", "Use the LLVM / Clang compiler", "no"))
opts.Add(BoolVariable("test", "Build to test dir", "yes"))
opts.Add(PathVariable("target_path", "The path where the lib is installed.", "bin/", PathVariable.PathAccept))
opts.Add(PathVariable("test_path", "The path where the test is installed.", "test/addons/godot_video_reference/bin/", PathVariable.PathAccept))
opts.Add(PathVariable("target_name", "The library name.", "libgdvideo", PathVariable.PathAccept))
opts.Add(BoolVariable("vsproj", "Generate a project for Visual Studio", "no"))

# Local dependency paths, adapt them to your setup
godot_headers_path = "godot-cpp/godot-headers/"
cpp_bindings_path = "godot-cpp/"
cpp_library = "libgodot-cpp"

# only support 64 at this time..
bits = 64

# Updates the environment with the option variables.
opts.Update(env)
# Generates help for the -h scons option.
Help(opts.GenerateHelpText(env))

# This makes sure to keep the session environment variables on Windows.
# This way, you can run SCons in a Visual Studio 2017 prompt and it will find
# all the required tools
if host_platform == "windows" and env["platform"] != "android":
    if env["bits"] == "64":
        env = Environment(TARGET_ARCH="amd64")
        env['msvc_arch'] = 'X64'
    elif env["bits"] == "32":
        env = Environment(TARGET_ARCH="x86")
        env['msvc_arch'] = 'X86'

    opts.Update(env)

# Process some arguments
if env["use_llvm"]:
    env["CC"] = "clang"
    env["CXX"] = "clang++"

if env["p"] != "":
    env["platform"] = env["p"]

if env["platform"] == "":
    print("No valid target platform selected.")
    quit()

# For the reference:
# - CCFLAGS are compilation flags shared between C and C++
# - CFLAGS are for C-specific compilation flags
# - CXXFLAGS are for C++-specific compilation flags
# - CPPFLAGS are for pre-processor flags
# - CPPDEFINES are for pre-processor defines
# - LINKFLAGS are for linking flags

if env["target"] == "debug":
    env.Append(CPPDEFINES=["DEBUG_ENABLED", "DEBUG_METHODS_ENABLED"])

# Check our platform specifics
if env["platform"] == "osx":
    env["target_path"] += "osx/"
    env["test_path"] += "osx/"
    cpp_library += ".osx"
    env.Append(CCFLAGS=["-arch", "x86_64"])
    env.Append(CXXFLAGS=["-std=c++17"])
    env.Append(LINKFLAGS=["-arch", "x86_64"])
    if env["target"] in ("debug", "d"):
        env.Append(CCFLAGS=["-g", "-O2"])
    else:
        env.Append(CCFLAGS=["-g", "-O3"])

elif env["platform"] in ("x11", "linux"):
    env["target_path"] += "x11/"
    env["test_path"] += "x11/"
    cpp_library += ".linux"
    env.Append(CCFLAGS=["-fPIC"])
    env.Append(CXXFLAGS=["-std=c++17"])
    if env["target"] in ("debug", "d"):
        env.Append(CCFLAGS=["-g3", "-Og"])
    else:
        env.Append(CCFLAGS=["-g", "-O3"])

elif env["platform"] == "windows":
    env["target_path"] += "win64/"
    env["test_path"] += "win64/"
    cpp_library += ".windows"
    # This makes sure to keep the session environment variables on windows,
    # that way you can run scons in a vs 2017 prompt and it will find all the required tools
    env.Append(ENV=os.environ)

    env.Append(CPPDEFINES=["WIN32", "_WIN32", "_WINDOWS", "_CRT_SECURE_NO_WARNINGS"])
    env.Append(CCFLAGS=["-W3", "-GR"])
    env.Append(CXXFLAGS=["-std:c++17"])
    if env["target"] in ("debug", "d"):
        env.Append(CPPDEFINES=["_DEBUG"])
        env.Append(CCFLAGS=["-EHsc", "-MDd", "-ZI", "-FS"])
        env.Append(LINKFLAGS=["-DEBUG", "/MACHINE:" + env['msvc_arch']])
    else:
        env.Append(CPPDEFINES=["NDEBUG"])
        env.Append(CCFLAGS=["-O2", "-EHsc", "-MD"])

    if not(env["use_llvm"]):
        env.Append(CPPDEFINES=["TYPED_METHOD_BIND"])

if env["target"] in ("debug", "d"):
    cpp_library += ".debug"
else:
    cpp_library += ".release"

cpp_library += "." + str(bits)

# make sure our binding library is properly includes
env.Append(CPPPATH=[".", godot_headers_path, cpp_bindings_path + "include/", cpp_bindings_path + "gen/include/"])
env.Append(LIBPATH=[cpp_bindings_path + "bin/"])
env.Append(LIBS=[cpp_library])

# tweak this if you want to use different folders, or more folders, to store your source code in.
env.Append(CPPPATH=["src/"])

def glob_filenames(pattern):
    from glob import glob
    return glob(pattern)

sources = glob_filenames("src/*.cpp")
includes = glob_filenames("src/*.h")

target_name = "";
if env["target"] in ("debug","d"):
    lib_target = env["target_path"]
    target_name = "Debug"
else:
    lib_target = env["target_path"]
    target_name = "Release"

if env["test"]:
    lib_target = env["test_path"]

library = env.SharedLibrary(target = lib_target + env["target_name"], source=sources)

if env["vsproj"]:
    vsproj = env.MSVSProject(target = 'godot_video_reference' + env['MSVSPROJECTSUFFIX'],
                    srcs = sources,
                    incs = includes,
                    localincs = [],
                    resources = [],
                    misc = ['LICENSE','README.md','.clang-format','.gitignore','.gitmodules'],
                    buildtarget = library,
                    variant = [target_name + '|'+env['msvc_arch']] * len(library)
                    )