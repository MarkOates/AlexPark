




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



