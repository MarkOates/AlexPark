




void draw_dialogue_frame(int x, int y, int w, int h)
{
	al_draw_filled_rectangle(x, y, x+w, y+h, al_map_rgba_f(0.2, 0.2, 0.2, 0.8));
}





ALLEGRO_BITMAP *generate_asset_bitmap(ParkAsset *asset, ALLEGRO_FONT *icons_big)
{
	int size = 128;
	ALLEGRO_COLOR col = asset->color;
	int padding = 8;

	// set everything up for rendering
	int w = size;
	int h = size;
	ALLEGRO_BITMAP *surface = al_create_bitmap(w, h);
	ALLEGRO_STATE previous_state;
	al_store_state(&previous_state, ALLEGRO_STATE_TARGET_BITMAP);


	int circle_x = w/2;
	int circle_y = h/2;
	int inner_circle_w = 100;
	// start drawing on the bitmap
	al_set_target_bitmap(surface);
	al_clear_to_color(al_map_rgba_f(0, 0, 0, 0));
	al_draw_filled_circle(circle_x, circle_y, w/2, al_map_rgba_f(0.2, 0.2, 0.2, 0.2));
	al_draw_filled_circle(circle_x, circle_y, w/2 - padding*2, col);


	al_draw_filled_circle(circle_x, circle_y, inner_circle_w/2, asset->color);

	// a nice outline
	al_draw_circle(circle_x, circle_y, inner_circle_w/2, al_map_rgba_f(0, 0, 0, 0.2), 5);

	draw_ustr_chr(asset->icon_char, circle_x, circle_y, 0.5, 0.5, asset->icon_color, icons_big);


	// restore everything back to where it was
	al_restore_state(&previous_state);

	// return the generated image
	return surface; 
}



class Dialogue
{
public:
	std::string message;
	std::string title;
	ALLEGRO_FONT *font;
	int x, y, w, h;
	Dialogue(std::string title, std::string message, ALLEGRO_FONT *font)
		: message(message)
		, title(title)
		, font(font)
		, x(200)
		, y(200)
		, w(500)
		, h(250)
	{}
	void draw()
	{
		int padding_x = 30;
		int padding_y = 20;
		draw_dialogue_frame(x, y, w, h);

		al_draw_text(font, al_color_name("yellow"), x+padding_x, y+padding_y, 0, title.c_str());

		al_draw_multiline_text(font, al_color_name("white"), x+padding_x, y+padding_y + 40, w-padding_x*2, al_get_font_line_height(font)-padding_x*2, 0, message.c_str());
		al_draw_text(font, al_color_name("yellowgreen"), x+w, y+h, ALLEGRO_ALIGN_RIGHT, "PRESS ANY KEY");
	}
};



class HUD
{
public:
	ALLEGRO_DISPLAY *display;
	ALLEGRO_BITMAP *hud_overlay;
	ALLEGRO_BITMAP *hud_overlay_pointer_target_buffer;
	ALLEGRO_FONT *font;
	ALLEGRO_FONT *header_font;
	ALLEGRO_FONT *icons, *icons_big;
	Park &park;

	int park_asset_index_selected_on_menu;
	std::vector<ParkAsset *> all_assets;

	std::map<std::string, ALLEGRO_BITMAP *> asset_icons;

	int hovered_ui_id;

	bool asset_window_visible;



	std::vector<Achievement *> all_achievements;



	std::deque<Dialogue> dialogues;

	void spawn_dialogue(std::string title, std::string message)
	{
		dialogues.push_back(Dialogue(title, message, font));
	}

	bool dialogue_is_open()
	{
		return !dialogues.empty();
	}

	bool close_dialogue()
	{
		if (!dialogues.empty())
		{
			dialogues.pop_front();
			return true;
		}
		return false;
	}




	HUD(ALLEGRO_DISPLAY *display, ALLEGRO_BITMAP *pointer_target_buffer, Park *park)
		: display(display)
		, hud_overlay(al_create_sub_bitmap(al_get_backbuffer(display), 0, 0, al_get_display_width(display), al_get_display_height(display)))
		, hud_overlay_pointer_target_buffer(al_create_sub_bitmap(pointer_target_buffer, 0, 0, al_get_bitmap_width(pointer_target_buffer), al_get_bitmap_height(pointer_target_buffer)))
		, font(al_load_font("data/fonts/Dense-Regular.otf", 36, 0))
		, header_font(al_load_font("data/fonts/Dense-Regular.otf", 42, 0))
		, icons(al_load_font("data/fonts/FontAwesome.otf", 30, 0))
		, icons_big(al_load_font("data/fonts/FontAwesome.otf", 50, 0))
		, all_assets()
		, park_asset_index_selected_on_menu(-1)
		, park(*park)
		, asset_icons()
		, hovered_ui_id(-1)
		, asset_window_visible(false)
		, all_achievements()
		, dialogues()
	{
		if (!font || !icons) std::cerr << "AAHGFHGHGHGHG (no fonts found)" << std::endl;

		// build our list of all the available assets
		all_assets.push_back(FACTORY_create_asset(PA_CONCESSION_STAND));
		all_assets.push_back(FACTORY_create_asset(PA_WALKWAY));
		all_assets.push_back(FACTORY_create_asset(PA_MERRY_GO_ROUND));
		all_assets.push_back(FACTORY_create_asset(PA_ROLLER_COASTER));
		all_assets.push_back(FACTORY_create_asset(PA_TREE));
		all_assets.push_back(FACTORY_create_asset(PA_HORROR_HOUSE));
		all_assets.push_back(FACTORY_create_asset(PA_CRAZY_LAND));
		all_assets.push_back(FACTORY_create_asset(PA_INFORMATION_CENTER));
		all_assets.push_back(FACTORY_create_asset(PA_PUBLIC_RESTROOMS));
		all_assets.push_back(FACTORY_create_asset(PA_WATER_FOUNTAIN));
		all_assets.push_back(FACTORY_create_asset(PA_MERCHANDISE_STORE));
		all_assets.push_back(FACTORY_create_asset(PA_JUNGLE_GYM));
		all_assets.push_back(FACTORY_create_asset(PA_PARK_BENCH));
		all_assets.push_back(FACTORY_create_asset(PA_FERRIS_WHEEL));
		all_assets.push_back(FACTORY_create_asset(PA_ALEX_STATUE));

		park_asset_index_selected_on_menu = 0;

		// generate our asset icons
		for (unsigned i=0; i<all_assets.size(); i++)
			asset_icons[all_assets[i]->type] = generate_asset_bitmap(all_assets[i], icons_big);

		// build our list of achievements
		all_achievements.push_back(FACTORY_create_achievement(ACH_FIRST_VISITOR, *park, *this));
	}

	void test_achievements()
	{
		for (unsigned i=0; i<all_achievements.size(); i++)
			if (all_achievements[i]->check_update()) return;
	}

	bool set_selected_asset_by_type(std::string asset_type)
	{
		park_asset_index_selected_on_menu = -1;
		for (unsigned i=0; i<all_assets.size(); i++)
		{
			if (all_assets[i]->type == asset_type)
			{
				park_asset_index_selected_on_menu = i;
				return true;
			}
		}
		return false;
	}

	void activate_ui(int ui_id)
	{
		ParkAsset *asset = get_asset_by_index(ui_id);
		if (asset)
		{
			set_selected_asset_by_type(asset->type);
			toggle_asset_window();
			return;
		}
	}

	ALLEGRO_BITMAP *get_asset_icon(std::string asset_type)
	{
		if (asset_icons.find(asset_type) == asset_icons.end()) return NULL;
		return asset_icons[asset_type];
	}

	void toggle_asset_window()
	{
		asset_window_visible = !asset_window_visible;
	}

	void draw(bool draw_id=false)
	{
		al_set_target_bitmap(draw_id ? hud_overlay_pointer_target_buffer : hud_overlay);
		al_set_render_state(ALLEGRO_DEPTH_TEST, 0);
	
		if (!draw_id)
		{
			int lineheight = al_get_font_ascent(font);
			int y_cursor = lineheight;
			ALLEGRO_COLOR color = al_color_name("white");

			al_draw_filled_rounded_rectangle(10, 10, 350, 60, 16, 12, al_map_rgba_f(0, 0, 0, 0.2));
			al_draw_textf(font, color, 20, 20, 0, "visitors: %d", park.visitors.size());	
			al_draw_textf(font, color, 200, 20, 0, "$ %d", park.money);
			
			al_draw_filled_rounded_rectangle(al_get_display_width(display)-350, 10, al_get_display_width(display)-10, 60, 16, 12, al_map_rgba_f(0, 0, 0, 0.2));
			al_draw_textf(font, color, al_get_display_width(display)-200, 20, ALLEGRO_ALIGN_RIGHT, "happiness: %d", park.visitor_happiness);

			ParkAsset *asset = park.get_asset_by_id(park.hovered_asset_id);
			if (asset)
			{
				// this shows info about the current item that is selected
				al_draw_textf(font, color, al_get_display_width(display)/2, al_get_display_height(display)-40, ALLEGRO_ALIGN_CENTER,
					"%s %d", asset->type.c_str(), asset->id);
			}

			ParkAsset *current_selected_asset = get_current_selected_asset();
			if (current_selected_asset)
			{
				ALLEGRO_BITMAP *asset_icon = get_asset_icon(current_selected_asset->type);
				if (asset_icon) al_draw_bitmap(asset_icon, al_get_display_width(display)-100-20, al_get_display_height(display)-100-40, 0);
				al_draw_textf(font, color, al_get_display_width(display)-20, al_get_display_height(display)-40, ALLEGRO_ALIGN_RIGHT,
					"%s - $%d", current_selected_asset->type.c_str(), current_selected_asset->initial_cost);
			}
		}


		if (asset_window_visible) draw_asset_selection_window(draw_id);


		if (dialogue_is_open()) dialogues.front().draw();
		// draw the assets_menu
	}
	ParkAsset *get_current_selected_asset()
	{
		return get_asset_by_index(park_asset_index_selected_on_menu);
	}
	ParkAsset *get_asset_by_index(int index)
	{
		if (index < 0) return NULL;
		if (index >= all_assets.size()) return NULL;
		return all_assets[index];
	}
	void draw_asset_selection_window(bool draw_id=false)
	{
		int w=850;
		int h=580;
		int x=al_get_display_width(display)-100-w;
		int y=al_get_display_height(display)-80-h;

		if (draw_id)
		{
			al_draw_filled_rectangle(x, y, x+w, y+h, al_color_name("black"));
		}
		else
		{
			draw_dialogue_frame(x,y, w, h);	
	
			int text_off_x = 10;
			int text_off_y = 10;
			int shadow_depth_x = 2;
			int shadow_depth_y = 2;
			al_draw_text(header_font, al_color_name("black"), x+text_off_x+shadow_depth_x, y+text_off_y+shadow_depth_y, 0, "ASSETS");
			al_draw_text(header_font, al_color_name("white"), x+text_off_x, y+text_off_y, 0, "ASSETS");
		}

		int inset_x = 70;
		int inset_y = 100;
		int circle_w = 128;
		int inner_circle_w = 100;
		int circle_padding_x = 20;
		int circle_padding_y = 20;
		for (int row=0; row<3; row++)
			for (int col=0; col<5; col++)
		{
			int box_x = inset_x + x + col*circle_w + col*circle_padding_x;
			int box_y = inset_y + y + row*circle_w + row*circle_padding_y;
			int circle_x = box_x + circle_w/2;
			int circle_y = box_y + circle_w/2;

			// get the asset here		
			ParkAsset *asset = get_asset_by_index(5 * row + col);	

			if (asset)
				// this asset has been unlocked; draw it
			{
				if (draw_id)
					al_draw_filled_circle(circle_x, circle_y, inner_circle_w/2, encode_id(40000 + 5*row + col));

				if (!draw_id)
				{
					ALLEGRO_BITMAP *asset_icon = get_asset_icon(asset->type);
					if (asset_icon) al_draw_bitmap(asset_icon, box_x, box_y, 0);
					//ALLEGRO_BITMAP *asset_icon = generate_asset_bitmap(asset, icons_big);
					//al_draw_bitmap(asset_icon, circle_x, circle_y, 0);
					//al_destroy_bitmap(asset_icon);
/*
					al_draw_filled_circle(
						circle_x,
						circle_y,
						inner_circle_w/2, asset->color);

					al_draw_circle(circle_x, circle_y, inner_circle_w/2, al_map_rgba_f(0, 0, 0, 0.2), 5);

					draw_ustr_chr(asset->icon_char, circle_x, circle_y, 0.5, 0.5, al_color_name("white"), icons_big);
*/

					if (hovered_ui_id == (5*row+col))
					{
						al_draw_circle(
							circle_x,
							circle_y,
							inner_circle_w/2, al_color_name("yellow"), 5);
					}
				}
			}
		}

	}
};



