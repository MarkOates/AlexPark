

#include <allegro5/allegro.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>

#include <string>
#include <vector>
#include <iostream>

#define TAU (ALLEGRO_PI * 2)

#include "color_encode_id.h"
#include "vec3d.h"
#include "park_asset.h"
#include "heat_map.h"
#include "park.h"
#include "ground.h"
#include "camera.h"




class Project
{
public:
	ALLEGRO_DISPLAY *display;
	ALLEGRO_BITMAP *pointer_target_buffer;
	Camera camera;
	Park park;
	bool abort_game;

	ALLEGRO_BITMAP *texture;
	ParkAsset park_assets[6];
	Ground ground;

	int mouse_x, mouse_y;

	Project(ALLEGRO_DISPLAY *display)
		: display(display)
		, pointer_target_buffer(al_create_bitmap(al_get_display_width(display), al_get_display_height(display)))
		, camera(0, 0, 0)
		, park()
		, abort_game(false)
		, texture(al_load_bitmap("data/bitmaps/stone.png"))
		, ground(32, 32, al_load_bitmap("data/bitmaps/ground_texture.png"))
		, mouse_x(0)
		, mouse_y(0)
	{
		for (unsigned i=0; i<6; i++)
		{
			park_assets[i].set_texture(texture);
			park_assets[i].position.x = i*2;
		}
	}
	ParkAsset *get_asset_by_id(int id)
	{
		for (unsigned i=0; i<6; i++)
			if (park_assets[i].id == id) return &park_assets[i];
		return NULL;
	}
	void on_timer()
	{
		//
		// this draws the ids as colors into a buffer
		//

		camera.setup_camera_perspective(pointer_target_buffer);
		al_clear_to_color(al_color_name("black"));
		al_clear_depth_buffer(1000);

		for (unsigned i=0; i<6; i++)
			park_assets[i].draw(true);

		ground.draw(true);

		// set the current hovered_asset_id
		ALLEGRO_COLOR pointed_pixel_value = al_get_pixel(pointer_target_buffer, mouse_x, mouse_y);
		int hovered_asset_id = decode_id(pointed_pixel_value);

		// for now, lets just the square blue that's being pointed to
		if (hovered_asset_id >= 1000)
		{
			int x, y;
			ground.unmap_texture_coordinates(hovered_asset_id, 1000, &x, &y);
			al_set_target_bitmap(ground.texture);
			al_put_pixel(x, y, al_color_name("dodgerblue"));
		}

		//
		// this draws the actual scene
		//

		camera.setup_camera_perspective(al_get_backbuffer(display));
		al_clear_to_color(al_color_name("gray"));
		al_clear_depth_buffer(1000);

		ground.draw();

		for (unsigned i=0; i<6; i++)
		{
			park_assets[i].hovered = (park_assets[i].id == hovered_asset_id);
			park_assets[i].draw();
		}
	}
	void on_key_char(ALLEGRO_EVENT &ev)
	{
		switch(ev.keyboard.keycode)
		{
			case ALLEGRO_KEY_RIGHT:
				camera.position.x ++;
				break;
			case ALLEGRO_KEY_LEFT:
				camera.position.x --;
				break;
			case ALLEGRO_KEY_UP:
				camera.position.z--;
				break;
			case ALLEGRO_KEY_DOWN:
				camera.position.z++;
				break;
			case ALLEGRO_KEY_PAD_PLUS:
				camera.zoom_pos += 0.1;
				break;
			case ALLEGRO_KEY_PAD_MINUS:
				camera.zoom_pos -= 0.1;
				break;
			case ALLEGRO_KEY_ENTER:
				al_save_bitmap("pointer_buffer.bmp", pointer_target_buffer);
				break;
		}
	}
	void on_key_up() {}
	void on_key_down() {}
	void on_mouse_axes(ALLEGRO_EVENT &ev)
	{
		mouse_x = ev.mouse.x;
		mouse_y = ev.mouse.y;
	}
	void on_mouse_up() {}
	void on_mouse_down() {}
};



int main(int argc, char* argv[])
{
	al_init();

	al_init_primitives_addon();
	al_init_image_addon();
	al_install_keyboard();
	al_install_mouse();
	al_init_font_addon();
	al_init_ttf_addon();

	ALLEGRO_PATH *resource_path = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
	al_change_directory(al_path_cstr(resource_path, ALLEGRO_NATIVE_PATH_SEP));
	al_destroy_path(resource_path);

	ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
	ALLEGRO_TIMER *timer = al_create_timer(1.0/60.0);

	// create the display
	al_set_new_display_option(ALLEGRO_DEPTH_SIZE, 32, ALLEGRO_SUGGEST);
	al_set_new_display_option(ALLEGRO_SUPPORT_NPOT_BITMAP, 0, ALLEGRO_REQUIRE);
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
			project.on_key_char(current_event);
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
			project.on_mouse_axes(current_event);
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


