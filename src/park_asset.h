







class ParkAsset
{
private:
	static int last_id;

public:
	// labels
	std::string type;
	ALLEGRO_COLOR color;
	int32_t icon_char; 
	ALLEGRO_COLOR icon_color;
	int id;

	// dimentions
	vec3d position;
	float rotation_y;
	int width;
	int height;

	// state
	bool hovered;

	// model
	std::vector<ALLEGRO_VERTEX> vertexes;
	std::vector<int> indexes;
	ALLEGRO_BITMAP *texture;

	// cost
	int initial_cost;

	// expenses
	float expense_per_turn;
	int max_num_customers_served;
	float customer_happiness_created;
	float profit;
	float num_customers_brought_to_park;

	// state(2)
	int num_customers_served_this_turn;

	bool tapped()
	{
		if (num_customers_served_this_turn >= max_num_customers_served) return true;
		return false;
	}

	ParkAsset()
		: type("")
		, color(al_color_name("gray"))
		, icon_char(0xf1b2)
		, icon_color(al_color_name("white"))
		, id(++last_id)
		, position(0, 0, 0)
		, rotation_y(0)
		, width(2)
		, height(2)
		, hovered(false)
		, vertexes()
		, indexes()
		, texture(NULL)
		, initial_cost(0)
		, expense_per_turn(0)
		, max_num_customers_served(0)
		, customer_happiness_created(0)
		, profit(0)
		, num_customers_brought_to_park(0)
		, num_customers_served_this_turn(0)
	{
		build_pyramid();
	}

	void build_pyramid()
	{
		vertexes.clear();
		indexes.clear();

		ALLEGRO_COLOR c = al_color_name("white");
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

		for (unsigned i=0; i<5; i++)
		{
			vtx[i].x += 0.5;
			vtx[i].z += 0.5;
		}	

		// transfer the vtxs to vertexes
		for (unsigned i=0; i<5; i++)
			vertexes.push_back(vtx[i]);

		for (unsigned i=0; i<12; i++)
			indexes.push_back(indices[i]);
	}

	void build_tall_cube()
	{
		build_cube();
		for (unsigned i=0; i<vertexes.size(); i++)
			vertexes[i].y *= 2;
	}

	void build_small_cube()
	{
		build_cube();
		for (unsigned i=0; i<vertexes.size(); i++)
		{
			vertexes[i].x *= 0.5;
			vertexes[i].y *= 0.5;
			vertexes[i].z *= 0.5;
			vertexes[i].x += 0.25;
			vertexes[i].z += 0.25;
		}
	}

	void build_cube()
	{
		vertexes.clear();
		indexes.clear();

		ALLEGRO_COLOR c = al_color_name("white");
		ALLEGRO_TRANSFORM t;
		float txw = 32;
		float txh = 32;
		ALLEGRO_VERTEX vtx[8] = {
			{ 0.500000, 0.000000, -0.500000, txw, 0, c },
			{ 0.500000, 0.000000, 0.500000, txw, txh, c },
			{ -0.500000, 0.000000, 0.500000, 0, txh, c },
			{ -0.500000, 0.000000, -0.500000, 0, 0, c },
			{ 0.500000, 1.000000, -0.500000, 0, 0, c },
			{ 0.500000, 1.000000, 0.500000, 0, 0, c },
			{ -0.500000, 1.000000, 0.500000, 0, 0, c },
			{ -0.500000, 1.000000, -0.500000, 0, 0, c }
		};
		int indices[36] = {
			1-1, 2-1, 3-1,
			1-1, 3-1, 4-1,
			5-1, 8-1, 7-1,
			5-1, 7-1, 6-1,
			1-1, 5-1, 6-1,
			1-1, 6-1, 2-1,
			2-1, 6-1, 7-1,
			2-1, 7-1, 3-1,
			3-1, 7-1, 8-1,
			3-1, 8-1, 4-1,
			5-1, 1-1, 4-1,
			5-1, 4-1, 8-1
		};

		for (unsigned i=0; i<8; i++)
		{
			vtx[i].x += 0.5;
			vtx[i].z += 0.5;
		}

		// transfer the vtxs to vertexes
		for (unsigned i=0; i<8; i++)
			vertexes.push_back(vtx[i]);

		for (unsigned i=0; i<36; i++)
			indexes.push_back(indices[i]);
	}

	void set_texture(ALLEGRO_BITMAP *tx)
	{
		texture = tx;
	}

	void set_color(ALLEGRO_COLOR col)
	{
		for (unsigned i=0; i<vertexes.size(); i++)
			vertexes[i].color = col;
	}

	void draw(bool use_id=false)
	{
		if (vertexes.empty() || indexes.empty()) return;

		if (use_id) set_color(encode_id(id));
		else if (hovered) set_color(al_color_name("yellow"));
		else set_color(color);


		ALLEGRO_STATE prev_state;
		al_store_state(&prev_state, ALLEGRO_STATE_TRANSFORM);
		ALLEGRO_TRANSFORM t;

		al_identity_transform(&t);
		al_rotate_transform_3d(&t, 0, 1, 0, rotation_y);
		al_translate_transform_3d(&t, position.x, position.y, position.z);
		al_use_transform(&t);
		al_draw_indexed_prim(&vertexes[0], NULL, use_id ? NULL : texture, &indexes[0], indexes.size(), ALLEGRO_PRIM_TRIANGLE_LIST);

		al_restore_state(&prev_state);
	}
/*
	virtual void draw_cube(bool use_id=false)
	{
		ALLEGRO_COLOR c = (use_id) ? encode_id(id) : (hovered ? al_color_name("yellow") : al_map_rgb_f(1, 1, 1));
		ALLEGRO_TRANSFORM t;
		float txw = 32;
		float txh = 32;
		ALLEGRO_VERTEX vtx[8] = {
			{ 0.500000, 0.000000, -0.500000, txw, 0, c },
			{ 0.500000, 0.000000, 0.500000, txw, txh, c },
			{ -0.500000, 0.000000, 0.500000, 0, txh, c },
			{ -0.500000, 0.000000, -0.500000, 0, 0, c },
			{ 0.500000, 1.000000, -0.500000, 0, 0, c },
			{ 0.500000, 1.000000, 0.500000, 0, 0, c },
			{ -0.500000, 1.000000, 0.500000, 0, 0, c },
			{ -0.500000, 1.000000, -0.500000, 0, 0, c }
		};
		int indices[36] = {
			1-1, 2-1, 3-1,
			1-1, 3-1, 4-1,
			5-1, 8-1, 7-1,
			5-1, 7-1, 6-1,
			1-1, 5-1, 6-1,
			1-1, 6-1, 2-1,
			2-1, 6-1, 7-1,
			2-1, 7-1, 3-1,
			3-1, 7-1, 8-1,
			3-1, 8-1, 4-1,
			5-1, 1-1, 4-1,
			5-1, 4-1, 8-1
		};

		for (unsigned i=0; i<8; i++)
		{
			vtx[i].x += 0.5;
			vtx[i].z += 0.5;
		}	

		ALLEGRO_STATE prev_state;
		al_store_state(&prev_state, ALLEGRO_STATE_TRANSFORM);

		al_identity_transform(&t);
		al_rotate_transform_3d(&t, 0, 1, 0, rotation_y);
		al_translate_transform_3d(&t, position.x, position.y, position.z);
		al_use_transform(&t);
		al_draw_indexed_prim(vtx, NULL, use_id ? NULL : texture, indices, 36, ALLEGRO_PRIM_TRIANGLE_LIST);

		al_restore_state(&prev_state);
	}
	virtual void draw_pyramid(bool use_id=false)
	{
		ALLEGRO_COLOR c = (use_id) ? encode_id(id) : (hovered ? al_color_name("yellow") : al_map_rgb_f(1, 1, 1));
		ALLEGRO_TRANSFORM t;
		ALLEGRO_VERTEX vtx[5] = {
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

		for (unsigned i=0; i<5; i++)
		{
			vtx[i].x += 0.5;
			vtx[i].z += 0.5;
		}	

		ALLEGRO_STATE prev_state;
		al_store_state(&prev_state, ALLEGRO_STATE_TRANSFORM);

		al_identity_transform(&t);
		al_rotate_transform_3d(&t, 0, 1, 0, rotation_y);
		al_translate_transform_3d(&t, position.x, position.y, position.z);
		al_use_transform(&t);
		al_draw_indexed_prim(vtx, NULL, use_id ? NULL : texture, indices, 12, ALLEGRO_PRIM_TRIANGLE_LIST);

		al_restore_state(&prev_state);
	}
*/
};
int ParkAsset::last_id = 0;





class ParkAssetType
{
public:
	ALLEGRO_BITMAP *thumbnail;
	ParkAsset &asset_ex;
};



#define PA_CONCESSION_STAND "Concession Stand"
#define PA_WALKWAY "Walkway"
#define PA_MERRY_GO_ROUND "Merry-Go-Round"
#define PA_ROLLER_COASTER "Roller Coaster"
#define PA_TREE "Tree"
#define PA_HORROR_HOUSE "Horror House"
#define PA_CRAZY_LAND "Crazy Land"
#define PA_INFORMATION_CENTER "Information Center"
#define PA_PUBLIC_RESTROOMS "Public Restrooms"
#define PA_WATER_FOUNTAIN "Water Fountain"
#define PA_MERCHANDISE_STORE "Merchandise Store"
#define PA_JUNGLE_GYM "Jungle Gym"
#define PA_PARK_BENCH "Park Bench"
#define PA_FERRIS_WHEEL "Ferris Wheel"
#define PA_ALEX_STATUE "Alex Statue"



class ConcessionStand : public ParkAsset
{
public:
	ConcessionStand()
		: ParkAsset()
	{
		type = PA_CONCESSION_STAND;
		texture = NULL;
		color = al_color_name("antiquewhite");
		icon_char = 0xf0f5;
		icon_color = al_color_name("brown");

		initial_cost = 500;

		expense_per_turn = 1;
		max_num_customers_served = 3;
		customer_happiness_created = 0;
		profit = 3;
		num_customers_brought_to_park = 1;

		build_cube();
	}
};



class Walkway : public ParkAsset
{
public:
	Walkway()
		: ParkAsset()
	{
		type = PA_WALKWAY;
		texture = NULL;
		color = al_color_name("darkolivegreen");
		icon_char = 0xf0c8;
		icon_color = al_color_name("darkgray");

		initial_cost = 0;

		expense_per_turn = 0;
		max_num_customers_served = 0;
		customer_happiness_created = 0;
		profit = 0;
		num_customers_brought_to_park = 0;

		vertexes.clear();
		indexes.clear();
	}
};



class MerryGoRound : public ParkAsset
{
public:
	MerryGoRound()
		: ParkAsset()
	{
		type = PA_MERRY_GO_ROUND;
		texture = NULL;
		color = al_color_name("orange");
		icon_char = 0xf052;
		icon_color = al_color_name("beige");

		initial_cost = 500;

		expense_per_turn = 1;
		max_num_customers_served = 2;
		customer_happiness_created = 3;
		profit = 0;
		num_customers_brought_to_park = 3;
	}
};



class RollerCoaster : public ParkAsset
{
public:
	RollerCoaster()
		: ParkAsset()
	{
		type = PA_ROLLER_COASTER;
		texture = NULL;
		color = al_color_name("red");
		icon_char = 0xf135;
		icon_color = al_color_name("yellow");

		initial_cost = 8000;

		expense_per_turn = 2;
		max_num_customers_served = 3;
		customer_happiness_created = 5;
		profit = 0;
		num_customers_brought_to_park = 5;
	}
};



class Tree : public ParkAsset
{
public:
	Tree()
		: ParkAsset()
	{
		type = PA_TREE;
		texture = NULL;
		color = al_color_html("46752d");
		icon_char = 0xf1bb;
		icon_color = al_color_html("6ba342");

		initial_cost = 50;

		expense_per_turn = 0;
		max_num_customers_served = 1;
		customer_happiness_created = 1;
		profit = 0;
		num_customers_brought_to_park = 0;
	}
};



class HorrorHouse : public ParkAsset
{
public:
	HorrorHouse()
		: ParkAsset()
	{
		type = PA_HORROR_HOUSE;
		texture = NULL;
		color = al_color_name("darkslategray");
		icon_char = 0xf1ad;
		icon_color = al_color_name("darkcyan");

		initial_cost = 5000;

		expense_per_turn = 2;
		max_num_customers_served = 3;
		customer_happiness_created = 4;
		profit = 0;
		num_customers_brought_to_park = 5;
	}
};



class CrazyLand : public ParkAsset
{
public:
	CrazyLand()
		: ParkAsset()
	{
		type = PA_CRAZY_LAND;
		texture = NULL;
		color = al_color_name("deeppink");
		icon_char = 0xf140;
		icon_color = al_color_name("yellow");

		initial_cost = 5000;

		expense_per_turn = 2;
		max_num_customers_served = 3;
		customer_happiness_created = 4;
		profit = 0;
		num_customers_brought_to_park = 5;
	}
};



class InformationCenter : public ParkAsset
{
public:
	InformationCenter()
		: ParkAsset()
	{
		type = PA_INFORMATION_CENTER;
		texture = NULL;
		color = al_color_name("dodgerblue");
		icon_char = 0xf05a;
		icon_color = al_color_name("aliceblue");

		initial_cost = 2000;

		expense_per_turn = 2;
		max_num_customers_served = 3;
		customer_happiness_created = 3;
		profit = 0;
		num_customers_brought_to_park = 3;
	}
};



class PublicRestrooms : public ParkAsset
{
public:
	PublicRestrooms()
		: ParkAsset()
	{
		type = PA_PUBLIC_RESTROOMS;
		texture = NULL;
		color = al_color_name("beige");
		icon_char = 0xf183;
		icon_color = al_color_name("darkblue");

		initial_cost = 1500;

		expense_per_turn = 2;
		max_num_customers_served = 5;
		customer_happiness_created = 5;
		profit = 0;
		num_customers_brought_to_park = 0;
	}
};



class WaterFountain : public ParkAsset
{
public:
	WaterFountain()
		: ParkAsset()
	{
		type = PA_WATER_FOUNTAIN;
		texture = NULL;
		color = al_color_name("lightslategray");
		icon_char = 0xf043;
		icon_color = al_color_name("lightskyblue");
		

		initial_cost = 500;

		expense_per_turn = 0;
		max_num_customers_served = 1;
		customer_happiness_created = 2;
		profit = 0;
		num_customers_brought_to_park = 0;

		build_small_cube();
	}
};


class MerchandiseStore : public ParkAsset
{
public:
	MerchandiseStore()
		: ParkAsset()
	{
		type = PA_MERCHANDISE_STORE;
		texture = NULL;
		color = al_color_name("blue");

		initial_cost = 3000;

		expense_per_turn = 0;
		max_num_customers_served = 5;
		customer_happiness_created = 1;
		profit = 7;
		num_customers_brought_to_park = 5;
	}
};


class JungleGym : public ParkAsset
{
public:
	JungleGym()
		: ParkAsset()
	{
		type = PA_JUNGLE_GYM;
		texture = NULL;
		color = al_color_name("silver");

		initial_cost = 300;

		expense_per_turn = 0;
		max_num_customers_served = 2;
		customer_happiness_created = 1;
		profit = 0;
		num_customers_brought_to_park = 0;
	}
};


class ParkBench : public ParkAsset
{
public:
	ParkBench()
		: ParkAsset()
	{
		type = PA_PARK_BENCH;
		texture = NULL;
		color = al_color_name("peru");

		initial_cost = 200;

		expense_per_turn = 0;
		max_num_customers_served = 1;
		customer_happiness_created = 1;
		profit = 0;
		num_customers_brought_to_park = 0;
	}
};


class FerrisWheel : public ParkAsset
{
public:
	FerrisWheel()
		: ParkAsset()
	{
		type = PA_FERRIS_WHEEL;
		texture = NULL;
		color = al_color_name("powderblue");
		icon_char = 0xf185;

		initial_cost = 10000;

		expense_per_turn = 3;
		max_num_customers_served = 3;
		customer_happiness_created = 5;
		profit = 0;
		num_customers_brought_to_park = 5;
	}
};



class AlexStatue : public ParkAsset
{
public:
	AlexStatue()
		: ParkAsset()
	{
		type = PA_ALEX_STATUE;
		texture = NULL;
		color = al_color_name("springgreen");

		initial_cost = 100000;

		expense_per_turn = 0;
		max_num_customers_served = 3;
		customer_happiness_created = 3;
		profit = 0;
		num_customers_brought_to_park = 5;
	}
};






ParkAsset *FACTORY_create_asset(std::string type)
{
	if (type == PA_CONCESSION_STAND) return new ConcessionStand();
	if (type == PA_WALKWAY) return new Walkway();
	if (type == PA_MERRY_GO_ROUND) return new MerryGoRound();
	if (type == PA_ROLLER_COASTER) return new RollerCoaster();
	if (type == PA_TREE) return new Tree();
	if (type == PA_HORROR_HOUSE) return new HorrorHouse();
	if (type == PA_CRAZY_LAND) return new CrazyLand();
	if (type == PA_INFORMATION_CENTER) return new InformationCenter();
	if (type == PA_PUBLIC_RESTROOMS) return new PublicRestrooms();
	if (type == PA_WATER_FOUNTAIN) return new WaterFountain();
	if (type == PA_MERCHANDISE_STORE) return new MerchandiseStore();
	if (type == PA_JUNGLE_GYM) return new JungleGym();
	if (type == PA_PARK_BENCH) return new ParkBench();
	if (type == PA_FERRIS_WHEEL) return new FerrisWheel();
	if (type == PA_ALEX_STATUE) return new AlexStatue();

	std::cerr << "Cannot create new ParkAsset of type \"" << type << "\"" << std::endl;
}
