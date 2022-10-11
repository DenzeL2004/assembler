all: mkdirectory build 

FLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations 		\
		-Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion	\
		-Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wmissing-field-initializers		\
		-Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel  	\
		-Wtype-limits -Wwrite-strings -D_DEBUG -D_EJUDGE_CLIENT_SIDE

build:  obj/main.o obj/stack.o obj/generals.o obj/log_errors.o obj/assembler.o obj/process_text.o obj/proc.o obj/labels.o
	g++ obj/main.o obj/stack.o  obj/generals.o obj/log_errors.o obj/assembler.o obj/process_text.o obj/proc.o obj/labels.o -o assembler

obj/main.o: main.cpp
	g++ main.cpp -c -o obj/main.o $(FLAGS)

obj/assembler.o: assembler.cpp assembler.h assembler_config.h
	g++ assembler.cpp -c -o obj/assembler.o $(FLAGS)

obj/labels.o: labels.cpp labels.h
	g++ labels.cpp -c -o obj/labels.o $(FLAGS)

obj/proc.o: proc.cpp proc.h
	g++ proc.cpp -c -o obj/proc.o $(FLAGS)

obj/process_text.o: process_text.cpp process_text.h
	g++ process_text.cpp -c -o obj/process_text.o $(FLAGS)

obj/log_errors.o: D:\project\Logs\log_errors.cpp
	g++ D:\project\Logs\log_errors.cpp -c -o obj/log_errors.o $(FLAGS)

obj/stack.o: D:\project\My_stack\stack.cpp D:\project\My_stack\stack.h
	g++  D:\project\My_stack\stack.cpp -c -o obj/stack.o $(FLAGS)

obj/generals.o: D:\project\Generals_func\generals.cpp
	g++ D:\project\Generals_func\generals.cpp -c -o obj/generals.o $(FLAGS)

.PHONY: cleanup mkdirectory

mkdirectory:
	mkdir -p obj

cleanup:
	rm *.o assembler