if not exist release mkdir release
make compile_flags="-Wall -Wextra -Wpedantic -Wno-cast-function-type -std=c++1z" include_libs="" --directory=release --makefile=../makefile