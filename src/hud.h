










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

	int hovered_ui_id;

	HUD(ALLEGRO_DISPLAY *display, ALLEGRO_BITMAP *pointer_target_buffer, Park *park)
		: display(display)
		, hud_overlay(al_create_sub_bitmap(al_get_backbuffer(display), 0, 0, al_get_display_width(display), al_get_display_height(display)))
		, hud_overlay_pointer_target_buffer(al_create_sub_bitmap(pointer_target_buffer, 0, 0, al_get_bitmap_width(pointer_target_buffer), al_get_bitmap_height(pointer_target_buffer)))
		, font(al_load_font("data/fonts/Dense-Regular.otf", 36, 0))
		, header_font(al_load_font("data/fonts/Dense-Regular.otf", 42, 0))
		, icons(al_load_font("data/fonts/FontAwesome.otf", 30, 0))
		, icons_big(al_load_font("data/fonts/FontAwesome.otf", 60, 0))
		, all_assets()
		, park_asset_index_selected_on_menu(-1)
		, park(*park)
		, hovered_ui_id(-1)
	{
		if (!font || !icons) std::cerr << "AAHGFHGHGHGHG (no fonts found)" << std::endl;

		// build our list of all the available assets
		all_assets.push_back(FACTORY_create_asset(PA_CONCESSION_STAND));
		all_assets.push_back(FACTORY_create_asset(PA_PARK_ENTRANCE));
		all_assets.push_back(FACTORY_create_asset(PA_MERRY_GO_ROUND));
		all_assets.push_back(FACTORY_create_asset(PA_ROLLER_COASTER));
		all_assets.push_back(FACTORY_create_asset(PA_BUSH));
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

			if (get_current_selected_asset())
				al_draw_text(font, color, al_get_display_width(display)-20, al_get_display_height(display)-40, ALLEGRO_ALIGN_RIGHT, get_current_selected_asset()->type.c_str());

			draw_ustr_chr(0xf15a, 100, 100, 0.5, 0.5, al_color_name("white"), icons);
		}


		draw_asset_selection_window(draw_id);
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
		int x=100;
		int y=100;
		int w=800;
		int h=600;

		ALLEGRO_COLOR fill_color = draw_id ? al_color_name("black") : al_map_rgba_f(0.2, 0.2, 0.2, 0.2);
		al_draw_filled_rectangle(x, y, x+w, y+h, fill_color);

		int inset_x = 70;
		int inset_y = 100;
		int circle_w = 120;
		int inner_circle_w = 100;
		int circle_padding_x = 20;
		int circle_padding_y = 20;
		for (int row=0; row<3; row++)
			for (int col=0; col<5; col++)
		{
			int circle_x = circle_w/2 + inset_x + x + col*circle_w + col*circle_padding_x;
			int circle_y = circle_w/2 + inset_y + y + row*circle_w + row*circle_padding_y;

			if (!draw_id)
				al_draw_filled_circle(
					circle_x,
					circle_y,
					circle_w/2, al_map_rgba_f(0.2, 0.2, 0.2, 0.2));
		
			ParkAsset *asset = get_asset_by_index(5 * row + col);	
			if (asset)
			{
				if (draw_id)
					al_draw_filled_circle(
						circle_x,
						circle_y,
						inner_circle_w/2, encode_id(40000 + 5*row + col));

				if (!draw_id)
				{
					al_draw_filled_circle(
						circle_x,
						circle_y,
						inner_circle_w/2, asset->color);

					if (hovered_ui_id == (5*row+col))
					{
						al_draw_circle(
							circle_x,
							circle_y,
							inner_circle_w/2, al_color_name("yellow"), 5);
						
						draw_ustr_chr(asset->icon_char, circle_x, circle_y, 0.5, 0.5, al_color_name("white"), icons_big);
					}
				}
			}
		}

		if (draw_id)
		{
			int text_off_x = 10;
			int text_off_y = 10;
			int shadow_depth_x = 2;
			int shadow_depth_y = 2;
			al_draw_text(header_font, al_color_name("black"), x+text_off_x+shadow_depth_x, y+text_off_y+shadow_depth_y, 0, "ASSETS");
			al_draw_text(header_font, al_color_name("white"), x+text_off_x, y+text_off_y, 0, "ASSETS");
		}
	}
};



