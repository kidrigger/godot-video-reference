#!/usr/bin/env python
import os
import platform
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
opts.Add(EnumVariable("target", "Compilation target", "debug", ["e", "editor", "d", "debug", "r", "release"]))
opts.Add(EnumVariable("platform", "Compilation platform", host_platform, ["", "windows", "x11", "linux", "osx"]))
opts.Add(
    EnumVariable("p", "Compilation target, alias for 'platform'", host_platform, ["", "windows", "x11", "linux", "osx"])
)
opts.Add(EnumVariable("bits", "Target platform bits", "64", ("32", "64")))
opts.Add(BoolVariable("use_llvm", "Use the LLVM / Clang compiler", "no"))
opts.Add(BoolVariable("test", "Build to test dir", "yes"))
opts.Add(BoolVariable("dev_build", "Debug symbols", "yes"))
opts.Add(PathVariable("target_path", "The path where the lib is installed.", "bin/", PathVariable.PathAccept))
opts.Add(PathVariable("test_path", "The path where the test is installed.", "test/addons/godot_video_reference/bin/", PathVariable.PathAccept))
opts.Add(PathVariable("target_name", "The library name.", "libgdvideo", PathVariable.PathAccept))
opts.Add(BoolVariable("vsproj", "Generate a project for Visual Studio", "no"))
opts.Add(BoolVariable("builtin_opus", "Use Built-in Opus", "yes"))
opts.Add(BoolVariable("builtin_libogg", "Use Built-in LibOgg", "yes"))

# Local dependency paths, adapt them to your setup
godot_headers_path = "godot-cpp/gdextension/"
cpp_bindings_path = "godot-cpp/"
cpp_library = "libgodot-cpp"

# only support 64 at this time..
bits = 64

# Updates the environment with the option variables.
opts.Update(env)
# Generates help for the -h scons option.
Help(opts.GenerateHelpText(env))

architecture_array = ["", "universal", "x86_32", "x86_64", "arm32", "arm64", "rv64", "ppc32", "ppc64", "wasm32"]
architecture_aliases = {
    "x64": "x86_64",
    "amd64": "x86_64",
    "armv7": "arm32",
    "armv8": "arm64",
    "arm64v8": "arm64",
    "aarch64": "arm64",
    "rv": "rv64",
    "riscv": "rv64",
    "riscv64": "rv64",
    "ppcle": "ppc32",
    "ppc": "ppc32",
    "ppc64le": "ppc64",
}
opts.Add(EnumVariable("arch", "CPU architecture", "", architecture_array, architecture_aliases))

opts.Update(env)
Help(opts.GenerateHelpText(env))

if env['arch'] == "":
    # No architecture specified. Default to arm64 if building for Android,
    # universal if building for macOS or iOS, wasm32 if building for web,
    # otherwise default to the host architecture.
    if env["platform"] in ["osx", "ios"]:
        env["arch"] = "universal"
    elif env["platform"] == "android":
        env["arch"] = "arm64"
    elif env["platform"] == "javascript":
        env["arch"] = "wasm32"
    else:
        host_machine = platform.machine().lower()
        if host_machine in architecture_array:
            env["arch"] = host_machine
        elif host_machine in architecture_aliases.keys():
            env["arch"] = architecture_aliases[host_machine]
        elif "86" in host_machine:
            # Catches x86, i386, i486, i586, i686, etc.
            env["arch"] = "x86_32"
        else:
            print("Unsupported CPU architecture: " + host_machine)
            Exit()

# We use this to re-set env["arch"] anytime we call opts.Update(env).
env_arch = env["arch"]

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

if env["target"] in ("debug", "editor"):
    env.Append(CPPDEFINES=["DEBUG_ENABLED", "DEBUG_METHODS_ENABLED"])

# Check our platform specifics
if env["platform"] == "osx":
    env["target_path"] += "osx/"
    env["test_path"] += "osx/"
    cpp_library += ".osx"
    env.Append(CCFLAGS=["-arch", "x86_64"])
    env.Append(CXXFLAGS=["-std=c++17"])
    env.Append(LINKFLAGS=["-arch", "x86_64", "-ldl"])
    if env["target"] in ("debug", "d", "editor", "e"):
        env.Append(CCFLAGS=["-g", "-O2"])
    else:
        env.Append(CCFLAGS=["-g", "-O3"])

elif env["platform"] in ("x11", "linux"):
    env["target_path"] += "x11/"
    env["test_path"] += "x11/"
    cpp_library += ".linux"
    env.Append(CCFLAGS=["-fPIC"])
    env.Append(CXXFLAGS=["-std=c++17"])
    if env["target"] in ("debug", "d", "editor", "e"):
        env.Append(CCFLAGS=["-g", "-O2"])
    else:
        env.Append(CCFLAGS=["-g", "-O3"])

elif env["platform"] == "windows":
    env["target_path"] += "win64/"
    env["test_path"] += "win64/"
    cpp_library += ".windows"
    # This makes sure to keep the session environment variables on windows,
    # that way you can run scons in a vs 2017 prompt and it will find all the required tools
    env.Append(ENV=os.environ)

    env.Append(CPPDEFINES=["WIN32", "_WIN32", "_WINDOWS", "_CRT_SECURE_NO_WARNINGS", "_ITERATOR_DEBUG_LEVEL=2"])
    env.Append(CCFLAGS=["-W3", "-GR"])
    env.Append(CXXFLAGS=["-std:c++17"])
    if env["target"] in ("debug", "d", "editor", "e"):
        env.Append(CPPDEFINES=["_DEBUG"])
        env.Append(CCFLAGS=["-EHsc", "-MDd", "-ZI", "-FS"])
        env.Append(LINKFLAGS=["-DEBUG", "/MACHINE:" + env['msvc_arch']])
    else:
        env.Append(CPPDEFINES=["NDEBUG"])
        env.Append(CCFLAGS=["-O2", "-EHsc", "-MD"])

    if not(env["use_llvm"]):
        env.Append(CPPDEFINES=["TYPED_METHOD_BIND"])

if env["target"] in ("debug", "d"):
    cpp_library += ".template_debug"
elif env["target"] in ("editor", "e"):
    cpp_library += ".editor"
else:
    cpp_library += ".template_release"

if env["dev_build"]:
    cpp_library += ".dev"

cpp_library += "." + str(env_arch)

# make sure our binding library is properly includes
env.Append(CPPPATH=[".", godot_headers_path, cpp_bindings_path + "include/", cpp_bindings_path + "gen/include/"])
env.Append(LIBPATH=[cpp_bindings_path + "bin/"])
env.Append(LIBS=[cpp_library])

# tweak this if you want to use different folders, or more folders, to store your source code in.
env.Append(CPPPATH=["src/"])

Export('env')

def glob_filenames(pattern):
    from glob import glob
    return glob(pattern)

env.__class__.sources = []
env.__class__.modules_sources = []
env.__class__.includes = []

sources = glob_filenames("src/*.cpp")
includes = glob_filenames("src/*.h")

env.sources += sources
env.includes = includes

def add_source_files(self, arr, regex):
    if type(regex) == list:
        arr += regex
        # print(arr)
    elif type(regex) == str:
        arr += glob_filenames(regex)
        # print(arr)

env.__class__.add_source_files = add_source_files 

SConscript("thirdparty/SCsub")

env.Prepend(CPPPATH=["thirdparty/"])
env.Prepend(CPPPATH=["thirdparty/libsimplewebm"])
env.Prepend(CPPPATH=["thirdparty/libsimplewebm/libwebm"])
env.Prepend(CPPPATH=["thirdparty/libvpx"])

target_name = ""
if env["target"] in ("debug","d", "editor", "e"):
    lib_target = env["target_path"]
    target_name = "Debug"
else:
    lib_target = env["target_path"]
    target_name = "Release"

if env["test"]:
    lib_target = env["test_path"]

library = env.SharedLibrary(target = lib_target + env["target_name"], source=env.sources+env.modules_sources)

# print (env.modules_sources)

if env["vsproj"]:
    vsproj = env.MSVSProject(target = 'godot_video_reference' + env['MSVSPROJECTSUFFIX'],
                    srcs = env.sources + env.modules_sources,
                    incs = env.includes,
                    localincs = [],
                    resources = [],
                    misc = ['LICENSE','README.md','.clang-format','.gitignore','.gitmodules'],
                    buildtarget = library,
                    variant = [target_name + '|'+env['msvc_arch']] * len(library)
                    )