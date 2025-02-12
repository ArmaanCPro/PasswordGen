# PasswordGen

This is a password generator and hasher using c++. The main part of it is the `generator` project, which is a static library. It contains a class `PasswordGenerator` that can be used to generate passwords. 
The `tests` project uses Google Test to unit test the password generation. It might not be the best written unit tests, but they get the job done. The `generator` uses `libsodium` to hash its passwords as well as maintain memory safety (plaintext passwords generally shouldn't be in memory for too long). 
At the project's current stage, it's basically a glorified `libsodium` wrapper, but I intend to add more features to it.

## Usage
At this state, the project isn't really ready for usage. I plan to add another project in the solution that will be a gui that uses the `generator` to generate passwords. Nonetheless, the `generator` project can still statically link to any CMake project w/ `git submodules`.
Expect the API to change drastically in the future. The `PasswordGenerator` class itself mainly exists to generate passwords and hash passwords. It also maintains a password policy, which can be set by the user. 
It contains fields like encryption strength, password length, use numbers, etc. `Generator.h` is the only file that needs to be included for now.
As for the `cli` project, it does work but is quite basic.

## Building
The project uses CMake to build. It uses both CMake's `FetchContent` as well as `vcpkg` to download dependencies. 
CMake looks for vcpkg based on a `VCPKG_ROOT` environment variable. You can change that in the outermost `CMakeLists.txt` file if needed. Otherwise, it uses cmake's `FetchContent` to download vcpkg.
The only reason I'm using FetchContent is for Google Test. It was a pain to use vcpkg with it, so I just used `FetchContent`. 
But, libsodium is installed using vcpkg. Aside from all that, you should be able to just build the project normally using CMake without any arguments.

## Dependencies
- `CMake`
- `vcpkg`
- `libsodium`
- `Google Test`