# Godot VideoStream Reference plugin

This is a reference plugin to use as a base to implement Video codecs for Godot 4 and later.

This is a Work In Progress.

## Building this plugin
Make sure that when you clone this repository you initialise the submodules recursively.

Generate `extension_api.json` against the [development branch for this interface from my fork](https://github.com/kidrigger/godot/tree/gdext_videodecoder)
and compile [`godot-cpp`](https://github.com/godotengine/godot-cpp) as per usual:

(Make sure to replace `godot` with the relevant path to the executable)

```
cd .\godot-cpp\godot-headers\
godot --dump-extension-api extension_api.json
cd ..
scons target=debug generate_bindings=yes
cd ..
```

After this compile the plugin:
```
scons target=debug test=true
```
Test flag puts the build binaries into the install location in the `test/` project
Remove this flag to export to `bin/`

> It is important to build debug builds as the editor requires debug builds to run.

## Visual Studio
Adding `vsproj=yes` will generate a Visual Studio solution `.sln` for the file.


## About The Author

This repository is maintained by Anish "KidRigger" Bhobe
