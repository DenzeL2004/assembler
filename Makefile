all: mkdirectory build 

FLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations 		\
		-Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion	\
		-Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wmissing-field-initializers		\
		-Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel  	\
		-Wtype-limits -Wwrite-strings -D_DEBUG -D_EJUDGE_CLIENT_SIDE

build:  main.o stack.o generals.o log_errors.o assembler.o work_with_text.o
	g++ obj/main.o obj/stack.o  obj/generals.o obj/log_errors.o obj/assembler.o obj/work_with_text.o -o ass

main.o: main.cpp
	g++ main.cpp -c -o obj/main.o $(FLAGS)

assembler.o: assembler.cpp
	g++ assembler.cpp -c -o obj/assembler.o $(FLAGS)

stack.o: D:\project\My_stack\stack.cpp
	g++ D:\project\My_stack\stack.cpp -c -o obj/stack.o $(FLAGS)

work_with_text.o: D:\project\Hamlet-\work_with_text.cpp
	g++ D:\project\Hamlet-\work_with_text.cpp -c -o obj/work_with_text.o $(FLAGS)

log_errors.o: D:\project\Logs\log_errors.cpp
	g++ D:\project\Logs\log_errors.cpp -c -o obj/log_errors.o $(FLAGS)

generals.o: D:\project\Generals_func\generals.cpp
	g++ D:\project\Generals_func\generals.cpp -c -o obj/generals.o $(FLAGS)

.PHONY: cleanup mkdirectory

mkdirectory:
	mkdir -p obj

cleanup:
	rm *.o assembler