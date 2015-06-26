

debug:
	g++ main.cpp -o main.exe -IE:/allegro-5.1.11-mingw-edgar/include -LE:/allegro-5.1.11-mingw-edgar/lib -lallegro_monolith-debug.dll

release:
	g++ main.cpp -o main.exe -IE:/allegro-5.1.11-mingw-edgar/include -LE:/allegro-5.1.11-mingw-edgar/lib -lallegro_monolith.dll


