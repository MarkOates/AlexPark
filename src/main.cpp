

#include <allegro5/allegro.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

#define TAU (ALLEGRO_PI * 2)


void draw_ustr_chr(int32_t ustr_char, float x, float y, float align_x, float align_y, ALLEGRO_COLOR color, ALLEGRO_FONT *font)
{
	if (!font) return;
	ALLEGRO_USTR *ustr = al_ustr_new("");
	al_ustr_set_chr(ustr, 0, ustr_char);
	al_draw_ustr(font, color,
		x - al_get_ustr_width(font, ustr) * align_x,
		y - al_get_font_line_height(font) * align_y,
		0,
		ustr); 
	al_ustr_free(ustr);
}


#include "color_encode_id.h"
#include "assets.h"
#include "vec3d.h"
#include "park_asset.h"
#include "heat_map.h"
#include "park.h"
#include "hud.h"
#include "ground.h"
#include "camera.h"






class Project
{
public:
	ALLEGRO_DISPLAY *display;
	ALLEGRO_BITMAP *pointer_target_buffer;
	Camera camera;
	Park park;
	HUD hud;
	bool abort_game;

	ALLEGRO_BITMAP *texture;
	Ground ground;

	int mouse_x, mouse_y;
	int ground_x, ground_y;

	ALLEGRO_BITMAP *asset_plot_shadow;
	ALLEGRO_BITMAP *grass_texture;

	Project(ALLEGRO_DISPLAY *display)
		: display(display)
		, pointer_target_buffer(al_create_bitmap(al_get_display_width(display), al_get_display_height(display)))
		, camera(0, 0, 0)
		, park()
		, hud(display, pointer_target_buffer, &park)
		, abort_game(false)
		, texture(al_load_bitmap("data/bitmaps/stone.png"))
		, ground(32, 32)
		, mouse_x(0)
		, mouse_y(0)
		, ground_x(0)
		, ground_y(0)
		, asset_plot_shadow(al_load_bitmap("data/bitmaps/plot_shadow.png"))
		, grass_texture(generate_noise_bitmap(1024, 1024, al_color_html("89ae58"), al_color_html("6e943c")))
	{
		// set the camera position to nice and wide
		camera.zoom_pos -= 1.0;
		camera.position.x += 20;
		camera.position.y -= 10;
		for (unsigned i=0; i<1; i++)
		{
			park.purchase_asset(hud.get_current_selected_asset(), 16, 16);
		}
	}

	void refresh_ground_render_surface()
	{
		ALLEGRO_STATE previous_state;
		al_store_state(&previous_state, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_BLENDER);
		al_set_target_bitmap(ground.render_surface);		
		al_set_render_state(ALLEGRO_DEPTH_TEST, 0);

		al_clear_to_color(al_color_html("689f41"));
		float x_scale = al_get_bitmap_width(ground.render_surface) / (float)ground.w;
		float y_scale = al_get_bitmap_height(ground.render_surface) / (float)ground.h;

		al_draw_bitmap(grass_texture, 0, 0, 0);

//		for (unsigned y=0; y<ground.h; y++)
//			for (unsigned x=0; x<ground.w; x++)
//				al_draw_filled_circle(x * x_scale, y * y_scale, 1, al_color_name("black"));

		// draw the cursor
		if (ground_x >= 0)
		{
			
			al_draw_circle(ground_x * x_scale + x_scale/2, ground_y * y_scale + x_scale/2,
				16, al_map_rgba_f(0.1, 0.1, 0.1, 0.1), 3.0);
			al_draw_rectangle(ground_x * x_scale, ground_y * y_scale,
				ground_x * x_scale + x_scale, ground_y * y_scale + y_scale,
				al_color_name("orange"), 3);
			//al_draw_filled_rectangle(ground_x * x_scale, ground_y * y_scale,
			//	ground_x * x_scale + x_scale, ground_y * y_scale + y_scale,
			//	al_color_name("yellow"));
		}

		// draw the nice eyecandy shadows
		for (unsigned i=0; i<park.assets.size(); i++)
		{
			int x = park.assets[i]->position.x * x_scale;
			int y = park.assets[i]->position.z * y_scale;
			al_draw_bitmap(asset_plot_shadow, x-16, y-16, 0);
		}

		al_restore_state(&previous_state);
		al_set_render_state(ALLEGRO_DEPTH_TEST, 1);
	}

	void on_timer()
	{
		//
		// update the park
		//

		park.update();


		//
		// this draws the ids as colors into a buffer
		//

		camera.setup_camera_perspective(pointer_target_buffer);
		al_clear_to_color(al_color_name("black"));
		al_clear_depth_buffer(1000);

		ground.draw(true);

		park.draw(true);

		hud.draw(true);


		//
		// parse the asset data and retrieve the useful information
		//

		// set the current hovered_asset_id
		ALLEGRO_COLOR pointed_pixel_value = al_get_pixel(pointer_target_buffer, mouse_x, mouse_y);
		park.hovered_asset_id = decode_id(pointed_pixel_value);

		ground_x = -1;
		ground_y = -1;
		hud.hovered_ui_id = -1;
		if (park.hovered_asset_id > 40000) // gui
		{
			hud.hovered_ui_id = park.hovered_asset_id - 40000;
		}
		else if (park.hovered_asset_id >= 1000)
		{
			ground.unmap_texture_coordinates(park.hovered_asset_id, 1000, &ground_x, &ground_y);
			//al_set_target_bitmap(ground.texture);
			//al_put_pixel(x, y, al_color_name("dodgerblue"));
		}

		//render_ground();

		// set the park asset as "hovered"
		for (unsigned i=0; i<park.assets.size(); i++)
			park.assets[i]->hovered = (park.assets[i]->id == park.hovered_asset_id);


		//
		// this draws the actual scene
		//

		refresh_ground_render_surface();

		camera.setup_camera_perspective(al_get_backbuffer(display));
		al_clear_to_color(al_color_name("dodgerblue"));
		al_clear_depth_buffer(1000);

		ground.draw();

		park.draw();	

		hud.draw();
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
			case ALLEGRO_KEY_EQUALS:
				park.money += 1000;
				break;
			case ALLEGRO_KEY_ENTER:
				// this will save a bitmap of the pointer_target_buffer,
				// which is the bitmap used to ditermine what object
				// the mouse is pointing at
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
	void on_mouse_down()
	{
		if (ground_x < 0 || ground_y < 0) return;

		park.purchase_asset(hud.get_current_selected_asset(), ground_x, ground_y);
	}
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
	ALLEGRO_DISPLAY *display = al_create_display(1200, 800);

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


