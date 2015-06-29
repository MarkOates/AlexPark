

debug: motion.o
	g++ -std=gnu++11 motion.o ./src/main.cpp -o main.exe -I./src -IE:/allegro-5.1.11-mingw-edgar/include -LE:/allegro-5.1.11-mingw-edgar/lib -lallegro_monolith-debug.dll

release: motion.o
	g++ -std=gnu++11 motion.o ./src/main.cpp -o main.exe -I./src -IE:/allegro-5.1.11-mingw-edgar/include -LE:/allegro-5.1.11-mingw-edgar/lib -lallegro_monolith.dll

motion.o:
	g++ -std=gnu++11 -c ./src/motion.cpp -I./src -IE:/allegro-5.1.11-mingw-edgar/include -LE:/allegro5.1.11-mingw-edgar/lib


