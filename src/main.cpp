

#include <allegro5/allegro.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>

#include <string>
#include <vector>
#include <iostream>

#include "color_encode_id.h"
#include "vec3d.h"
#include "park_asset.h"
#include "heat_map.h"
#include "park.h"

#define TAU (ALLEGRO_PI * 2)




class Ground
{
public:
	int w;
	int h;
	ALLEGRO_BITMAP *texture, *remap_coordinates_texture;
	ALLEGRO_VERTEX vertexes[4];

	Ground(int w, int h, ALLEGRO_BITMAP *tex)
		: w(w)
		, h(h)
		, texture(tex)
		, remap_coordinates_texture(create_remapable_texture(w, h, 1000))
		, vertexes({
				{-w/2.0f, 0, -h/2.0f, 0, 0, al_color_name("white")},
				{-w/2.0f, 0, h/2.0f, 0, (float)h, al_color_name("white")},
				{w/2.0f, 0, h/2.0f, (float)w, (float)h, al_color_name("white")},
				{w/2.0f, 0, -h/2.0f, (float)w, 0, al_color_name("white")}
		})
	{
	}

	ALLEGRO_BITMAP *create_remapable_texture(int width, int height, int offset_number)
	{
		ALLEGRO_BITMAP *bmp = al_create_bitmap(width, height);
		ALLEGRO_STATE state;
		al_store_state(&state, ALLEGRO_STATE_TARGET_BITMAP);
		al_set_target_bitmap(bmp);
		al_clear_to_color(al_color_name("black"));
		al_lock_bitmap(bmp, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_WRITEONLY);
		for (unsigned y=0; y<height; y++)
			for (unsigned x=0; x<width; x++)
			{
				al_put_pixel(x, y, encode_id(offset_number + x + y*width));
			}
		al_unlock_bitmap(bmp);
		al_restore_state(&state);
		return bmp;
	}

	std::pair<int, int> unmap_texture_coordinates(int id, int offset_number)
	{
		id -= offset_number;
		int x = id % w;
		int y = id / w;
		return std::pair<int, int>(x, y);
	}	


	void draw(bool use_id=false)
	{
		al_draw_prim(&vertexes, NULL, use_id ? remap_coordinates_texture : texture, 0, 4, ALLEGRO_PRIM_TRIANGLE_FAN);
	}

};




class Camera
{
public:
	vec3d position;
	vec3d stepback;
	vec3d rotation;
	float tilt;

	float zoom_min;
	float zoom_max;
	float zoom_pos;

	Camera(float x, float y, float z)
		: position(x, y, z)
		, stepback(0, 0, 0)
		, rotation(0, 0, 0)
		, tilt(0)
		, zoom_min(10)
		, zoom_max(4)
		, zoom_pos(0.5)
	{
		set45_isometric();
	}

	void set45_isometric()
	{
		stepback = vec3d(0, 120, 120);
		rotation = vec3d(-TAU/8.0, TAU/16.0, 0);
	}

	void setup_camera_perspective(ALLEGRO_BITMAP *bmp)
	{
		ALLEGRO_TRANSFORM p;
		float aspect_ratio = (float)al_get_bitmap_height(bmp) / al_get_bitmap_width(bmp);
		al_set_target_bitmap(bmp);
		al_identity_transform(&p);

		{ // my camera translations go here
			// turn the camera right-left
			al_rotate_transform_3d(&p, 0, 1, 0, rotation.y);

			// position
			al_translate_transform_3d(&p,
				-position.x-stepback.x,
				-position.y-stepback.y,
				-position.z-stepback.z);

			// tilt the camera up-down
			al_rotate_transform_3d(&p, -1, 0, 0, rotation.x);
		}

		//al_perspective_transform(&p, -1, aspect_ratio, 1, 1, -aspect_ratio, 1000);
		float znear = 5;
		float zoom = (zoom_max - zoom_min) * zoom_pos + zoom_min; // 4 is closeup, 10 is wide
		stepback = vec3d(0, znear*zoom, znear*zoom);
		al_perspective_transform(&p,
			-1, aspect_ratio, znear,
			1, -aspect_ratio, 1000);
		al_use_projection_transform(&p);

		al_use_transform(al_get_current_transform());
		al_set_render_state(ALLEGRO_DEPTH_TEST, 1);
	}
};

#include <utility>
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
			std::pair<int, int> coords = ground.unmap_texture_coordinates(hovered_asset_id, 1000);
			int x = std::get<0>(coords);
			int y = std::get<1>(coords);
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


