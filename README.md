# PasswordGen

This is a password generator and hasher using c++. The main part of it is the `generator` project, which is a static library. In the future, I might change it to being a DLL. It contains a class `PasswordGenerator` that can be used to generate passwords. The `tests` project uses Google Test to unit test the password generation. It might not be the best written unit tests, but they get the job done. The `generator` uses `libsodium` to hash its passwords as well as maintain memory safety (plaintext passwords generally shouldn't be in memory for too long). At the project's current stage, it's basically a glorified `libsodium` wrapper, but I intent to add more features to it.

## Usage
At this state, the project isn't really ready for usage. I plan to add another project in the solution that will be either a gui using imgui or at least a console application that uses the `generator` to generate passwords. Nonetheless, the `generator` project can still statically link to any CMake project w/ `git submodules`. However, expect the API to change drastically in the future. The `PasswordGenerator` class itself mainly exists to generate passwords and hash passwords. It also maintains a password policy, which can be set by the user. It contains fields like encryption strength, password length, use numbers, etc. `Generator.h` is the only file that needs to be included for now.

## Building
The project uses CMake to build. It uses both CMake's `FetchContent` as well as `vcpkg` to download dependencies. Right now, CMake looks for vcpkg based on a `VCPKG_ROOT` environment variable. You can change that in the outermost `CMakeLists.txt` file if needed. The only reason I'm using FetchContent is for Google Test. It was a pain to use vcpkg with it, so I just used FetchContent. But, libsodium is installed using vcpkg. Aside from all that, you should be able to just build the project normally using CMake without any arguments, but I've only tested it so far using CLion, and at an earlier stage in the project VS Code.

## Dependencies
- `CMake`
- `vcpkg`
- `libsodium`
- `Google Test`