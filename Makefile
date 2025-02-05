compile:
	gcc chunk.h common.h compiler.h memory.h vm.h debug.h \
		scanner.h value.h debug.c main.c memory.c vm.c value.c \
		chunk.c compiler.c scanner.c

