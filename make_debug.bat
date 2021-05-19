if not exist "debug" mkdir "debug"
make compile_flags="-Wall -Wextra -Wpedantic -Wno-cast-function-type -std=c++1z -DDEBUG" include_libs="" --directory="debug" --makefile=../makefile