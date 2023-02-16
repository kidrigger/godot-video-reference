# Godot VideoStream Reference plugin

This is a reference plugin to use as a base to implement Video codecs for Godot 4 and later.

This is a Work In Progress.
Currently tested with [godot#15a97a2](https://github.com/godotengine/godot/commit/15a97a2e8462ff76fe2eb44094f61320065b7dc8)
On linux (debian) and Windows.

## Building this plugin

The instructions build for `target=editor` and output binaries into the test folder.

Compile [`godot-cpp`](https://github.com/godotengine/godot-cpp):  
```bash
cd godot-cpp
scons target=editor generate_bindings=yes
cd ..
```

After this compile the plugin:
```
scons target=debug test=true
```
Test flag puts the build binaries into the install location in the `test/` project
Remove this flag to export to `bin/`

## Visual Studio
Adding `vsproj=yes` will generate a Visual Studio solution `.sln` for the file.

## In case of errors

If godot headers are outdated you may need to update the extension api before compiling `godot-cpp`
```
cd ./godot-cpp/godot-headers/
godot --dump-extension-api --dump-gdextension-interface
cd ../..
```

