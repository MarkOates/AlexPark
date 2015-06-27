

#include <allegro5/allegro.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>

#include <string>
#include <vector>
#include <iostream>

#include "color_encode_id.h"
#include "park_asset.h"
#include "heat_map.h"
#include "park.h"
#include "vec3d.h"

#define TAU (ALLEGRO_PI * 2)




class Ground
{
public:
	int w;
	int h;
	ALLEGRO_BITMAP *texture;
	ALLEGRO_VERTEX vertexes[4];

	Ground(float w, float h, ALLEGRO_BITMAP *tex)
		: w(w)
		, h(h)
		, texture(tex)
		, vertexes({
				{-w/2, 0, -h/2, 0, 0, al_color_name("gray")},
				{-w/2, 0, h/2, 0, (float)al_get_bitmap_height(tex), al_color_name("white")},
				{w/2, 0, h/2, (float)al_get_bitmap_width(tex), (float)al_get_bitmap_height(tex), al_color_name("white")},
				{w/2, 0, -h/2, (float)al_get_bitmap_width(tex), 0, al_color_name("white")}
		})
	{
	}

	void draw()
	{
		al_draw_prim(&vertexes, NULL, texture, 0, 4, ALLEGRO_PRIM_TRIANGLE_FAN);
	}
};




class Model
{
public:
	std::vector<ALLEGRO_VERTEX> vertexes;
	std::vector<int> indexes;
	ALLEGRO_TRANSFORM transform;

	vec3d position;
	float rotation_y;
	ALLEGRO_BITMAP *texture;

	Model()
		: vertexes()
		, indexes()
		, transform()
		, position(0, 0, 0)
		, rotation_y(0)
		, texture(NULL)
	{
	}

	void set_texture(ALLEGRO_BITMAP *tx)
	{
		texture = tx;
	}

	void draw()
	{
		ALLEGRO_COLOR c = al_map_rgb_f(1, 1, 1);
		ALLEGRO_TRANSFORM t;
		ALLEGRO_VERTEX vtx[5] = {
		/*   x   y   z   u   v  c  */
			{ 0,  1,  0,  0, 64, c},
			{-0.5, 0, -0.5,  0,  0, c},
			{ 0.5, 0, -0.5, 64, 64, c},
			{ 0.5, 0,  0.5, 64,  0, c},
			{-0.5, 0,  0.5, 64, 64, c},
		};
		int indices[12] = {
			0, 1, 2,
			0, 2, 3,
			0, 3, 4,
			0, 4, 1
		};

		al_identity_transform(&t);
		al_rotate_transform_3d(&t, 0, 1, 0, rotation_y);
		al_translate_transform_3d(&t, position.x, position.y, position.z);
		al_use_transform(&t);
		al_draw_indexed_prim(vtx, NULL, texture, indices, 12, ALLEGRO_PRIM_TRIANGLE_LIST);
	}
};


static void set_perspective_transform(ALLEGRO_BITMAP* bmp)
{
   ALLEGRO_TRANSFORM p;
   float aspect_ratio = (float)al_get_bitmap_height(bmp) / al_get_bitmap_width(bmp);
   al_set_target_bitmap(bmp);
   al_identity_transform(&p);
   al_perspective_transform(&p, -1, aspect_ratio, 1, 1, -aspect_ratio, 1000);
   al_use_projection_transform(&p);
}


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
		al_clear_to_color(al_color_name("gray"));
		al_clear_depth_buffer(1000);
	}
};


class Project
{
public:
	ALLEGRO_DISPLAY *display;
	ALLEGRO_BITMAP *pointer_target_buffer;
	Camera camera;
	Park park;
	bool abort_game;

	ALLEGRO_BITMAP *texture;
	Model models[6];
	Ground ground;

	Project(ALLEGRO_DISPLAY *display)
		: display(display)
		, pointer_target_buffer(al_create_bitmap(al_get_display_width(display), al_get_display_height(display)))
		, camera(0, 0, 0)
		, park()
		, abort_game(false)
		, texture(al_load_bitmap("data/bitmaps/stone.png"))
		, ground(32, 32, al_load_bitmap("data/bitmaps/ground_texture.png"))
	{
		for (unsigned i=0; i<6; i++)
		{
			models[i].set_texture(texture);
			models[i].position.x = i*2;
		}
	}
	void on_timer()
	{
		//	set_perspective_transform(al_get_backbuffer(display));
		//camera.set_frustum_as_camera(display);
		camera.setup_camera_perspective(al_get_backbuffer(display));

		ground.draw();

		for (unsigned i=0; i<6; i++)
			models[i].draw();
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
		}
	}
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
	ALLEGRO_DISPLAY *display = al_create_display(800, 600);

	set_perspective_transform(al_get_backbuffer(display));
	
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


