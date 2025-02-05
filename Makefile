compile:
	gcc chunk.h common.h compiler.h memory.h vm.h debug.h \
		scanner.h value.h obj.h dictionary.h debug.c main.c memory.c vm.c value.c \
		chunk.c compiler.c scanner.c obj.c dictionary.c

