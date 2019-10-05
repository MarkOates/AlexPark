


void draw_ustr_chr(int32_t ustr_char, float x, float y, float align_x, float align_y, ALLEGRO_COLOR color, ALLEGRO_FONT *font, bool shaded = true)
{
   if (!font) return;
   ALLEGRO_USTR *ustr = al_ustr_new("");
   al_ustr_set_chr(ustr, 0, ustr_char);
   if (shaded)
   al_draw_ustr(font, al_map_rgba_f(0, 0, 0, 0.3),
      x - al_get_ustr_width(font, ustr) * align_x + 2,
      y - al_get_font_line_height(font) * align_y + 3,
      0,
      ustr); 
   al_draw_ustr(font, color,
      x - al_get_ustr_width(font, ustr) * align_x,
      y - al_get_font_line_height(font) * align_y,
      0,
      ustr); 
   al_ustr_free(ustr);
}


void draw_dialogue_frame(int x, int y, int w, int h, bool yellow_border=false)
{
	al_draw_filled_rectangle(x, y, x+w, y+h, al_map_rgba_f(0.2, 0.2, 0.2, 0.8));
	if (yellow_border)
		al_draw_rectangle(x, y, x+w, y+h, al_color_name("yellow"), 6);
}


int draw_asset_stat(float x, float y, int32_t icon_char, int value, ALLEGRO_COLOR color, ALLEGRO_FONT *text_font, ALLEGRO_FONT *icon_font)
{
	int w_of_stat = 60;
	int h_of_stat = 33;

	float x_cursor = x;
	al_draw_filled_rectangle(x_cursor, y, x_cursor+w_of_stat, y+h_of_stat, color);
		
	draw_ustr_chr(icon_char, x_cursor + 24, y+h_of_stat/2, 0.5, 0.5, al_color_name("white"), icon_font, false);
	al_draw_textf(text_font, al_color_name("white"), x_cursor + 45, y+5, ALLEGRO_ALIGN_CENTER, "%d", value);
	return w_of_stat;
}


void draw_asset_stats_bar(ParkAsset *asset, float x, float y, ALLEGRO_FONT *text_font, ALLEGRO_FONT *icon_font)
{
	int x_cursor = x;
	x_cursor += draw_asset_stat(x_cursor, y, 0xf155, asset->expense_per_turn, al_color_name("firebrick"), text_font, icon_font);
	x_cursor += draw_asset_stat(x_cursor, y, 0xf0d6, asset->profit, al_color_name("olivedrab"), text_font, icon_font);
	x_cursor += draw_asset_stat(x_cursor, y, 0xf0c0, asset->max_num_customers_served, al_color_name("darkblue"), text_font, icon_font);
	x_cursor += draw_asset_stat(x_cursor, y, 0xf234, asset->num_customers_brought_to_park, al_color_name("rebeccapurple"), text_font, icon_font);
	x_cursor += draw_asset_stat(x_cursor, y, 0xf1ae, asset->customer_happiness_created, al_color_name("webpurple"), text_font, icon_font);
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
	ALLEGRO_FONT *font, *icon_font;
	int32_t icon_char;
	ALLEGRO_COLOR icon_color;
	float x, y, w, h;
	float time_created; // this is to give it a 2 second delay before the user can click it away
	float lifespan_min;
	Dialogue(std::string title, std::string message, ALLEGRO_FONT *font, ALLEGRO_FONT *icon_font=NULL, int32_t icon_char=0, ALLEGRO_COLOR icon_color=al_color_name("orange"))
		: message(message)
		, title(title)
		, font(font)
		, icon_font(icon_font)
		, icon_char(icon_char)
		, icon_color(icon_color)
		, x(200)
		, y(200)
		, w(500)
		, h(250)
		, time_created(al_get_time())
		, lifespan_min(2)
	{}
	float current_lifespan()
	{
		return al_get_time() - time_created;
	}
	bool can_close()
	{
		return current_lifespan() >= lifespan_min;
	}
	void draw()
	{
		int padding_x = 30;
		int padding_y = 20;
		draw_dialogue_frame(x, y, w, h, true);

		if (icon_char && icon_font) draw_ustr_chr(icon_char, x, y, 0.5, 0.5, icon_color, icon_font, true);

		al_draw_text(font, al_color_name("yellow"), x+padding_x, y+padding_y, 0, title.c_str());

		al_draw_multiline_text(font, al_color_name("white"), x+padding_x, y+padding_y + 40, w-padding_x*2, al_get_font_line_height(font)-padding_x*2, 0, message.c_str());
		if (can_close())
			al_draw_text(font, al_color_name("aquamarine"), x+w, y+h, ALLEGRO_ALIGN_RIGHT, "PRESS ANY KEY");
	}
};



class HUD
{
public:
	ALLEGRO_DISPLAY *display;
	ALLEGRO_BITMAP *hud_overlay;
	ALLEGRO_BITMAP *hud_overlay_pointer_target_buffer;
	ALLEGRO_FONT *font_small;
	ALLEGRO_FONT *font;
	ALLEGRO_FONT *header_font;
	ALLEGRO_FONT *icons, *icons_big;
	Park &park;
	Motion &motion;
   bool skip_tutorial_dialogs;

	int park_asset_index_selected_on_menu;
	std::vector<ParkAsset *> all_assets;

	std::map<std::string, ALLEGRO_BITMAP *> asset_icons;

	int hovered_ui_id;

	bool asset_window_visible;

	bool opened_window_first_time;



	std::vector<Achievement *> all_achievements;



	std::deque<Dialogue> dialogues;

	void spawn_dialogue(std::string title, std::string message, int32_t ustr_char=0, ALLEGRO_COLOR color=al_color_name("aqua"))
	{
		dialogues.push_back(Dialogue(title, message, font, icons_big, ustr_char, color));
		Dialogue *dialog = &dialogues.back();

		dialog->y = dialog->y+80;
		motion.canimate(&dialog->y, dialog->y, dialog->y-80, al_get_time(), al_get_time()+0.6, interpolator::doubleFastIn, NULL, NULL);
	}

	bool dialogue_is_open()
	{
		return !dialogues.empty();
	}

	bool close_dialogue()
	{
		if (!dialogues.empty())
		{
			Dialogue *current_dialogue = &dialogues.front();
			if (current_dialogue->can_close())
			{
				motion.clear_animations_on(&current_dialogue->y);
				dialogues.pop_front();
				return true;
			}
		}
		return false;
	}




	HUD(ALLEGRO_DISPLAY *display, ALLEGRO_BITMAP *pointer_target_buffer, Park *park, Motion &motion, bool skip_tutorial_dialogs=false)
		: display(display)
		, hud_overlay(al_create_sub_bitmap(al_get_backbuffer(display), 0, 0, al_get_display_width(display), al_get_display_height(display)))
		, hud_overlay_pointer_target_buffer(al_create_sub_bitmap(pointer_target_buffer, 0, 0, al_get_bitmap_width(pointer_target_buffer), al_get_bitmap_height(pointer_target_buffer)))
		, font_small(al_load_font("data/fonts/Dense-Regular.otf", 20, 0))
		, font(al_load_font("data/fonts/Dense-Regular.otf", 36, 0))
		, header_font(al_load_font("data/fonts/Dense-Regular.otf", 42, 0))
		, icons(al_load_font("data/fonts/FontAwesome.otf", 20, 0))
		, icons_big(al_load_font("data/fonts/FontAwesome.otf", 50, 0))
		, all_assets()
		, park_asset_index_selected_on_menu(-1)
		, park(*park)
		, motion(motion)
		, asset_icons()
		, hovered_ui_id(-1)
		, asset_window_visible(false)
		, all_achievements()
		, dialogues()
		, opened_window_first_time(false)
		, skip_tutorial_dialogs(skip_tutorial_dialogs)
	{
		if (!font || !icons) std::cerr << "AAHGFHGHGHGHG (no fonts found)" << std::endl;

		// build our list of all the available assets
		all_assets.push_back(FACTORY_create_asset(PA_WALKWAY));
		all_assets.push_back(FACTORY_create_asset(PA_TREE));
		all_assets.push_back(FACTORY_create_asset(PA_CONCESSION_STAND));
		//all_assets.push_back(FACTORY_create_asset(PA_PARK_BENCH));
		//all_assets.push_back(FACTORY_create_asset(PA_JUNGLE_GYM));
		//all_assets.push_back(FACTORY_create_asset(PA_WATER_FOUNTAIN));
		//all_assets.push_back(FACTORY_create_asset(PA_MERRY_GO_ROUND));
		//all_assets.push_back(FACTORY_create_asset(PA_INFORMATION_CENTER));
		//all_assets.push_back(FACTORY_create_asset(PA_PUBLIC_RESTROOMS));
		//all_assets.push_back(FACTORY_create_asset(PA_MERCHANDISE_STORE));
		//all_assets.push_back(FACTORY_create_asset(PA_CRAZY_LAND));
		//all_assets.push_back(FACTORY_create_asset(PA_HORROR_HOUSE));
		//all_assets.push_back(FACTORY_create_asset(PA_ROLLER_COASTER));
		//all_assets.push_back(FACTORY_create_asset(PA_FERRIS_WHEEL));
		//all_assets.push_back(FACTORY_create_asset(PA_ALEX_STATUE));

		park_asset_index_selected_on_menu = 0;

		// generate our asset icons
		for (unsigned i=0; i<all_assets.size(); i++)
			asset_icons[all_assets[i]->type] = generate_asset_bitmap(all_assets[i], icons_big);

		// build our list of achievements
		all_achievements.push_back(FACTORY_create_achievement(ACH_FIRST_VISITOR, *park, *this));
		all_achievements.push_back(FACTORY_create_achievement(ACH_MORE_THAN_15_VISITORS, *park, *this));
		all_achievements.push_back(FACTORY_create_achievement(ACH_INFO_CENTER, *park, *this));
		//all_achievements.push_back(FACTORY_create_achievement(ACH_WATER_FOUNTAIN, *park, *this));
		all_achievements.push_back(FACTORY_create_achievement(ACH_MERRY_GO_ROUND, *park, *this));
		all_achievements.push_back(FACTORY_create_achievement(ACH_PUBLIC_RESTROOMS, *park, *this));
		all_achievements.push_back(FACTORY_create_achievement(ACH_MERCHANDISE_STORE, *park, *this));
		all_achievements.push_back(FACTORY_create_achievement(ACH_CRAZY_LAND, *park, *this));
		all_achievements.push_back(FACTORY_create_achievement(ACH_HORROR_HOUSE, *park, *this));
		all_achievements.push_back(FACTORY_create_achievement(ACH_ROLLER_COASTER, *park, *this));
		all_achievements.push_back(FACTORY_create_achievement(ACH_FERRIS_WHEEL, *park, *this));
		all_achievements.push_back(FACTORY_create_achievement(ACH_ALEX_STATUE, *park, *this));
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

	ALLEGRO_BITMAP *get_asset_icon(ParkAsset *asset, std::string asset_type)
	{
		if (!asset) return NULL;
		if (asset_icons.find(asset_type) == asset_icons.end())
		{
			asset_icons[asset->type] = generate_asset_bitmap(asset, icons_big); /// wowoofff, that's a hack
			//return NULL;
		}
		return asset_icons[asset_type];
	}

	void toggle_asset_window()
	{
		if (!skip_tutorial_dialogs && !opened_window_first_time)
		{
			opened_window_first_time = true;
			spawn_dialogue("These Are Assets You Can Build.", "Right now only have a few things you can build, so there's not a lot to choose from. But, as your park grows you'll be able to build more things.", 0xf0a1);
			spawn_dialogue("These Are Assets You Can Build.", "Each item costs an amount of money to build.  And each item has certain advantages and disadvantages.", 0xf0a1);
			spawn_dialogue("These Are Assets You Can Build.", "Keep in mind, though. When all assets are permanent.  You won't be able to remove it once it's built.", 0xf0a1);
			spawn_dialogue("These Are Assets You Can Build.", "Let's start simple.  Your goal is to earn money and get people into your park.  BUILD A CONCESSION STAND so we can start generating some income.", 0xf0a1);
		}
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
			al_draw_textf(font, color, 40, 20, 0, "visitors: %d", (int)park.visitors.size());
			al_draw_textf(font, color, 200, 20, 0, "$ %d", park.money);
			
			al_draw_filled_rounded_rectangle(al_get_display_width(display)-350, 10, al_get_display_width(display)-10, 60, 16, 12, al_map_rgba_f(0, 0, 0, 0.2));
			al_draw_textf(font, color, al_get_display_width(display)-200-100, 20, ALLEGRO_ALIGN_LEFT, "happiness: %d", (int)(park.visitor_happiness*10));
			al_draw_textf(font, color, al_get_display_width(display)-100-50, 20, ALLEGRO_ALIGN_LEFT, "boredom: %d", (int)(park.visitor_boredom*10));

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
				ALLEGRO_BITMAP *asset_icon = get_asset_icon(current_selected_asset, current_selected_asset->type);
				if (asset_icon) al_draw_bitmap(asset_icon, al_get_display_width(display)-128-20, al_get_display_height(display)-128-40, 0);
				al_draw_textf(font, color, al_get_display_width(display)-20, al_get_display_height(display)-40, ALLEGRO_ALIGN_RIGHT,
					"%s - $%d", current_selected_asset->type.c_str(), current_selected_asset->initial_cost);
			}
		}

		// dirty dirty, putting this line here is a bit of a hack
		if (draw_id) al_draw_filled_circle(al_get_display_width(display)-128/2-20, al_get_display_height(display)-128/2-40, 128/2, encode_id(60000));


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
			al_clear_to_color(encode_id(60000));
		}

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
					ALLEGRO_BITMAP *asset_icon = get_asset_icon(asset, asset->type);
					if (asset_icon) al_draw_bitmap(asset_icon, box_x, box_y, 0);

					// draw the name
					al_draw_text(font_small, al_color_name("white"), circle_x, circle_y+32+20, ALLEGRO_ALIGN_CENTER, asset->type.c_str()); 
					al_draw_textf(font_small, al_color_name("white"), circle_x, circle_y+32+20+20, ALLEGRO_ALIGN_CENTER, "$%d", asset->initial_cost); 

					if (hovered_ui_id == (5*row+col))
					{
						float hover_line_width = 8;
						al_draw_circle(
							circle_x,
							circle_y,
							inner_circle_w/2+hover_line_width/2, al_color_name("yellow"), hover_line_width);

						draw_asset_stats_bar(asset, x+w-330, y+29, font_small, icons); 
					}
				}
			}
		}

	}
};



