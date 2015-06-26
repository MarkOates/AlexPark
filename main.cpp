

#include <allegro5/allegro.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>


class Project
{
public:
	ALLEGRO_DISPLAY *display;
	bool abort_game;
	Project(ALLEGRO_DISPLAY *display)
		: display(display)
		, abort_game(false)
	{}
	void on_timer()
	{
		al_clear_to_color(al_color_name("gray"));
		al_draw_filled_circle(al_get_display_width(display)/2, al_get_display_height(display)/2, 10, al_color_name("white"));
	}
	void on_key_char() {}
	void on_key_up() {}
	void on_key_down() {}
	void on_mouse_axes() {}
	void on_mouse_up() {}
	void on_mouse_down() {}
};


int main(int argc, char* argv[])
{
	al_init();

	al_init_primitives_addon();
	al_install_keyboard();
	al_install_mouse();
	al_init_font_addon();
	al_init_ttf_addon();

	ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
	ALLEGRO_TIMER *timer = al_create_timer(1.0/60.0);
	ALLEGRO_DISPLAY *display = al_create_display(800, 600);
	
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_mouse_event_source());	
	al_register_event_source(event_queue, al_get_keyboard_event_source());	
	al_register_event_source(event_queue, al_get_display_event_source(display));	

	Project project(display);
	al_start_timer(timer);

	while (!project.abort_game)
	{
		ALLEGRO_EVENT current_event;
		al_wait_for_event(event_queue, &current_event);

		switch(current_event.type)
		{
		case ALLEGRO_EVENT_TIMER:
			project.on_timer();
			al_flip_display();
			break;
		case ALLEGRO_EVENT_KEY_CHAR:
			project.on_key_char();
			break;
		case ALLEGRO_EVENT_KEY_DOWN:
			project.on_key_down();
			break;
		case ALLEGRO_EVENT_KEY_UP:
			project.on_key_up();
			break;
		case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
			project.on_mouse_down();
			break;
		case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
			project.on_mouse_up();
			break;
		case ALLEGRO_EVENT_MOUSE_AXES:
			project.on_mouse_axes();
			break;
		case ALLEGRO_EVENT_DISPLAY_CLOSE:
			project.abort_game = true;
			break;
		default:
			break;
		}
	}


	return 0;
}


