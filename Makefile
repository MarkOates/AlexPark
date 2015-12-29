

ALLEGRO_DIR=/Users/markoates/Repos/allegro/build
ALLEGRO_LIBS=-lallegro_color -lallegro_font -lallegro_ttf -lallegro_dialog -lallegro_audio -lallegro_acodec -lallegro_primitives -lallegro_image -lallegro_main -lallegro


debug: motion.o
	g++ -std=gnu++11 motion.o ./src/main.cpp -o main.exe -I./src -I$(ALLEGRO_DIR)/include -L$(ALLEGRO_DIR)/lib $(ALLEGRO_LIBS)

release: motion.o
	g++ -std=gnu++11 motion.o ./src/main.cpp -o main.exe -I./src -I$(ALLEGRO_DIR) -L$(ALLEGRO_DIR) $(ALLEGRO_LIBS)

motion.o:
	g++ -std=gnu++11 -c ./src/motion.cpp -I./src -I$(ALLEGRO_DIR)/include -L$(ALLEGRO_DIR)/lib


