main: main.cpp 
	g++ main.cpp include/glad.c -o prog.exe -lglfw3 -lopengl32 -lgdi32
