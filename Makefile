

debug:
	g++ -std=gnu++11 ./src/main.cpp -o main.exe -I./src -IE:/allegro-5.1.11-mingw-edgar/include -LE:/allegro-5.1.11-mingw-edgar/lib -lallegro_monolith-debug.dll

release:
	g++ -std=gnu++11 ./src/main.cpp -o main.exe -I./src -IE:/allegro-5.1.11-mingw-edgar/include -LE:/allegro-5.1.11-mingw-edgar/lib -lallegro_monolith.dll


