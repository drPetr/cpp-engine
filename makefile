program_name := ../app 
source_dirs  := . core core/math renderer renderer/opengl

source_dirs      := $(addprefix ../,$(source_dirs))
search_wildcards := $(addsuffix /*.cpp,$(source_dirs))

CC:= g++
INCLUDE:= -I../

$(program_name): $(notdir $(patsubst %.cpp,%.o, $(wildcard $(search_wildcards) ) ) )
	${CC} $^ -o $@ $(include_libs)

VPATH := $(source_dirs) 

%.o: %.cpp
	${CC} -c -MD $(compile_flags) $(INCLUDE) $<

include $(wildcard *.d) 