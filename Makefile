all: mkdirectory 

FLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations 		\
		-Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion	\
		-Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wmissing-field-initializers		\
		-Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel  	\
		-Wtype-limits -Wwrite-strings -D_DEBUG -D_EJUDGE_CLIENT_SIDE

run: obj/main.o obj/log_errors.o obj/generals.o
	g++ obj/main.o obj/log_errors.o obj/generals.o -o run

proc:  obj/main_cpu.o obj/stack.o obj/generals.o obj/log_errors.o obj/assembler.o obj/process_text.o obj/proc.o obj/labels.o
	g++ obj/main_cpu.o obj/stack.o obj/generals.o obj/log_errors.o obj/assembler.o obj/process_text.o obj/proc.o obj/labels.o -o proc

asm: obj/main_asm.o obj/generals.o obj/log_errors.o obj/assembler.o obj/process_text.o obj/labels.o
	g++ obj/main_asm.o obj/generals.o obj/log_errors.o obj/assembler.o obj/process_text.o obj/labels.o -o assembler

obj/main.o: main.cpp
	g++ main.cpp -c -o obj/main.o $(FLAGS)

obj/main_cpu.o: main_cpu.cpp
	g++ main_cpu.cpp -c -o obj/main_cpu.o $(FLAGS)

obj/main_asm.o: main_asm.cpp
	g++ main_asm.cpp -c -o obj/main_asm.o $(FLAGS)

obj/assembler.o: assembler.cpp assembler.h architecture.h cmd.h 
	g++ assembler.cpp -c -o obj/assembler.o $(FLAGS)

obj/labels.o: labels.cpp labels.h 
	g++ labels.cpp -c -o obj/labels.o $(FLAGS)

obj/proc.o: proc.cpp proc.h cmd.h architecture.h cmd.h
	g++ proc.cpp -c -o obj/proc.o $(FLAGS)

obj/process_text.o: src\process_text\process_text.cpp src\process_text\process_text.h
	g++ src\process_text\process_text.cpp -c -o obj/process_text.o $(FLAGS)

obj/log_errors.o: src/log_info/log_errors.h src/log_info/log_errors.cpp
	g++ src\log_info\log_errors.cpp -c -o obj/log_errors.o $(FLAGS)

obj/stack.o: src\stack\stack.cpp src\stack\stack.h
	g++  src\stack\stack.cpp -c -o obj/stack.o $(FLAGS)

obj/generals.o: src\Generals_func\generals.cpp
	g++ src\Generals_func\generals.cpp -c -o obj/generals.o $(FLAGS)

.PHONY: cleanup mkdirectory

mkdirectory:
	mkdir -p obj

cleanup:
	rm *.o assembler