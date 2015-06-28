




class Park
{
public:
	int num_visitors;
	int money;
	int visitor_happiness;
	int employees;

	std::vector<ParkAsset *> assets;
	int hovered_asset_id;
	HeatMap traffic_map;

	Park()
		: num_visitors(0)
		, money(10000)
		, visitor_happiness(0)
		, employees(0)
		, assets()
		, hovered_asset_id(0)
		, traffic_map()
	{
	}

	void update()
	{
		
	}

	ParkAsset *get_asset_by_id(int id)
	{
		for (unsigned i=0; i<assets.size(); i++)
			if (assets[i]->id == id) return assets[i];
		return NULL;
	}

	void draw(bool use_id=false)
	{
		for (unsigned i=0; i<assets.size(); i++)
			assets[i]->draw_cube(use_id);

		//traffic_map.draw();
	}
};




class HUD
{
public:
	ALLEGRO_DISPLAY *display;
	ALLEGRO_BITMAP *hud_overlay;
	ALLEGRO_FONT *font;
	ALLEGRO_FONT *icons;
	Park &park;

	HUD(ALLEGRO_DISPLAY *display, Park *park)
		: display(display)
		, hud_overlay(al_create_sub_bitmap(al_get_backbuffer(display), 0, 0, al_get_display_width(display), al_get_display_height(display)))
		, font(al_load_font("data/fonts/Dense-Regular.otf", 36, 0))
		, icons(al_load_font("data/fonts/FontAwesome.otf", 30, 0))
		, park(*park)
	{
		if (!font || !icons) std::cerr << "AAHGFHGHGHGHG (no fonts found)" << std::endl;
	}
	void draw()
	{
		al_set_target_bitmap(hud_overlay);
		al_set_render_state(ALLEGRO_DEPTH_TEST, 0);

		int lineheight = al_get_font_ascent(font);
		int y_cursor = lineheight;
		ALLEGRO_COLOR color = al_color_name("white");

		al_draw_filled_rounded_rectangle(10, 10, 350, 60, 16, 12, al_map_rgba_f(0, 0, 0, 0.2));
		al_draw_textf(font, color, 20, 20, 0, "visitors: %d", park.num_visitors);	
		al_draw_textf(font, color, 200, 20, 0, "$$$: %d", park.money);
		
		al_draw_filled_rounded_rectangle(al_get_display_width(display)-350, 10, al_get_display_width(display)-10, 60, 16, 12, al_map_rgba_f(0, 0, 0, 0.2));
		al_draw_textf(font, color, al_get_display_width(display)-200, 20, ALLEGRO_ALIGN_RIGHT, "happiness: %d", park.visitor_happiness);
		al_draw_textf(font, color, al_get_display_width(display)-20, 20, ALLEGRO_ALIGN_RIGHT, "employees: %d", park.employees);

		ParkAsset *asset = park.get_asset_by_id(park.hovered_asset_id);
		if (asset)
		{
			// this shows info about the current item that is selected
			al_draw_textf(font, color, al_get_display_width(display)/2, al_get_display_height(display)-40, ALLEGRO_ALIGN_CENTER,
				"%s %d", asset->type.c_str(), asset->id);
		}
	}
};



