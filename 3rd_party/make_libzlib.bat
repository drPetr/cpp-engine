if not exist zlib-1.2.11-obj mkdir zlib-1.2.11-obj
make compile_flags="-Wall -Wextra -O3" include_libs="" --directory=zlib-1.2.11-obj --makefile=../makefile-zlib-1.2.11