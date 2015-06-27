


class Park
{
public:
	int num_visitors;
	int money;
	int visitor_happiness;
	int employees;

	std::vector<ParkAsset *> assets;
	HeatMap traffic_map;

	Park()
		: num_visitors(0)
		, money(10000)
		, visitor_happiness(0)
		, employees(0)
		, assets()
		, traffic_map()
	{
	}

	void update()
	{
	}

	void draw()
	{
		for (unsigned i=0; i<assets.size(); i++)
			assets[i]->draw();

		traffic_map.draw();
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
		, font(al_load_font("data/fonts/Dense-Regular.otf", 40, 0))
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
		int i=0;
		ALLEGRO_COLOR color = al_color_name("white");
		al_draw_textf(font, color, 10, lineheight*i++, 0, "Visitors: %d", park.num_visitors);
		al_draw_textf(font, color, 10, lineheight*i++, 0, "$: %d", park.money);
		al_draw_textf(font, color, 10, lineheight*i++, 0, "visitor_happiness: %d", park.visitor_happiness);
		al_draw_textf(font, color, 10, lineheight*i++, 0, "employees: %d", park.employees);
		al_draw_textf(font, color, 10, lineheight*i++, 0, "num_assets: %d", park.assets.size());

		//al_set_target_bitmap(al_get_backbuffer(display));
	}
};



