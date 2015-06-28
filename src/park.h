




class Visitor
{
public:
	float time_in;
	float boredom;
	float happiness;
	bool served_this_turn;
	Visitor()
		: time_in(0)
		, boredom(0)
		, happiness(5)
		, served_this_turn(false)
	{}
	bool ready_to_leave()
	{
		if (happiness <= 0) return true;
		if (boredom >= 5) return true;
		if (time_in >= 7) return true;
		return false;
	}
	void update()
	{
		time_in++;
		happiness--;
		boredom++;
	}
};




class Park // this is basically the Player
{
public:
	int money;
	float visitor_happiness;
	float customer_attraction;

	std::vector<ParkAsset *> assets;
	std::vector<Visitor *> visitors;
	int hovered_asset_id;
	HeatMap traffic_map;
	int plots_of_land[32][32];

	float update_speed;
	float update_counter;

	Park()
		: money(1000)
		, visitor_happiness(0)
		, assets()
		, visitors()
		, hovered_asset_id(0)
		, traffic_map()
		, update_counter(1)
		, update_speed(0.005)
	{
		for (int y=0; y<32; y++)
			for (int x=0; x<32; x++)
				plots_of_land[x][y] = 0;
	}

	bool purchase_asset(ParkAsset *menu_asset_selected, int x, int y)
	{
		// is the provided asset example valid?
		if (!menu_asset_selected) return false;

		// is the space available?
		if (x < 0 || x >= 32) return false; // outside of bounds!!
		if (y < 0 || y >= 32) return false; // outside of bounds!!
		if (plots_of_land[x][y] != 0) return false; // this plot is occupied (or not available -1)!

		// can the park afford it?
		if (money < menu_asset_selected->initial_cost) return false; // you don't have enough money

		// make it
		ParkAsset *new_asset = FACTORY_create_asset(menu_asset_selected->type);
		assets.push_back(new_asset);
		new_asset->position.x = x;
		new_asset->position.z = y;

		// add the id to the lease
		plots_of_land[x][y] = new_asset->id;

		// deduct the money from the park
		money -= new_asset->initial_cost;

		return true; // woo-hoo!
	}

	void update()
	{
		update_counter -= update_speed;
		if (update_counter <= 0) update_counter = 1.0;
		else return;

		std::cout << ".";

		//
		// ADD/REMOVE VISITORS
		//

		// remove visitors who are ready to leave the park
		for (unsigned i=0; i<visitors.size(); i++)
		{
			if (visitors[i]->ready_to_leave())
			{
				delete visitors[i];
				visitors.erase(visitors.begin() + i);
				i--;
			}
		}

		// calculate the number of new customers coming in
		int new_customers = 0;
		for (unsigned i=0; i<assets.size(); i++)
		{
			new_customers += assets[i]->num_customers_brought_to_park;
		}

		// create the new visitors
		for (unsigned i=0; i<new_customers; i++)
			visitors.push_back(new Visitor());


		//
		// SERVE THE VISITORS
		//

		// start by marking the visitors as unserved
		for (unsigned i=0; i<visitors.size(); i++)
			visitors[i]->served_this_turn = false;

		// ... and by marking all the assets as not having served anybody yet
		for (unsigned i=0; i<assets.size(); i++)
			assets[i]->num_customers_served_this_turn = 0;

		// shuffle them (to have them randomly served by random assets)
		std::vector<Visitor *> shuffled_visitors = visitors;
		std::random_shuffle(shuffled_visitors.begin(), shuffled_visitors.end());
		std::vector<ParkAsset *> shuffled_assets = assets;
		std::random_shuffle(shuffled_assets.begin(), shuffled_assets.end());

		// update the interactions between the visitors and the assets
		for (unsigned i=0; i<shuffled_visitors.size(); i++)
		{
			// pick an asset
			ParkAsset *asset = get_random_untapped_asset(&shuffled_assets);
			if (!asset) break;

			// serve a customer with that asset
			shuffled_visitors[i]->boredom--;
			shuffled_visitors[i]->happiness += asset->customer_happiness_created;
			shuffled_visitors[i]->served_this_turn = true;

			// show that the asset has served a customer
			asset->num_customers_served_this_turn++;
		}	

		// update all the visitors
		for (unsigned i=0; i<visitors.size(); i++)
		{
			visitors[i]->update();
		}


		//
		// COUNT THE ASSET EXPENSES / ASSET PROFITS FOR THE PARK
		//

		for (unsigned i=0; i<assets.size(); i++)
		{
			money -= assets[i]->expense_per_turn;
			money += assets[i]->profit * assets[i]->num_customers_served_this_turn;
		}
	}

	ParkAsset *get_random_untapped_asset(std::vector<ParkAsset *> *shuffled_assets)
	{
		std::random_shuffle(shuffled_assets->begin(), shuffled_assets->end());
		for (unsigned i=0; i<shuffled_assets->size(); i++)
			if (!shuffled_assets->at(i)->tapped()) return shuffled_assets->at(i);
		return NULL;
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
			assets[i]->draw(use_id);

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

	ParkAsset *park_asset_selected_on_menu;
	std::vector<ParkAsset *> all_assets;

	HUD(ALLEGRO_DISPLAY *display, Park *park)
		: display(display)
		, hud_overlay(al_create_sub_bitmap(al_get_backbuffer(display), 0, 0, al_get_display_width(display), al_get_display_height(display)))
		, font(al_load_font("data/fonts/Dense-Regular.otf", 36, 0))
		, icons(al_load_font("data/fonts/FontAwesome.otf", 30, 0))
		, all_assets()
		, park_asset_selected_on_menu(NULL)
		, park(*park)
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

		// select the first one on the list
		park_asset_selected_on_menu = all_assets[0];
	}
	void draw()
	{
		al_set_target_bitmap(hud_overlay);
		al_set_render_state(ALLEGRO_DEPTH_TEST, 0);

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

		if (park_asset_selected_on_menu)
			al_draw_text(font, color, al_get_display_width(display)-20, al_get_display_height(display)-40, ALLEGRO_ALIGN_RIGHT, park_asset_selected_on_menu->type.c_str());

		draw_ustr_chr(0xf15a, 100, 100, 0.5, 0.5, al_color_name("white"), icons);
	}
};






