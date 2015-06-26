

#include <allegro5/allegro.h>
#include <allegro5/allegro_color.h>

int main(int argc, char* argv[])
{
	al_init();
	ALLEGRO_DISPLAY *display = al_create_display(800, 600);
	al_clear_to_color(al_color_name("gray"));
	al_flip_display();
	al_rest(4);
	return 0;
}


