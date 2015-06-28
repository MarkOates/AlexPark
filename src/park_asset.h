


class ParkAsset
{
private:
	static int last_id;

public:
	// labels
	std::string type;
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
	float initial_cost;

	// expenses
	float expense;
	int num_employees;
	float initial_hype;
	float hype_depreciation;
	float radius_of_influence;
	float time_per_customer;

	ParkAsset()
		: type("")
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
		, expense(0)
		, num_employees(0)
		, initial_hype(0)
		, hype_depreciation(0)
		, radius_of_influence(0)
		, time_per_customer(0)
	{
	}

	virtual void build_model() {}

	void set_texture(ALLEGRO_BITMAP *tx)
	{
		texture = tx;
	}

	void set_color(ALLEGRO_COLOR color)
	{
		for (unsigned i=0; i<vertexes.size(); i++)
			vertexes[i].color = color;
	}

	void draw(bool use_id=false)
	{
		if (vertexes.empty() || indexes.empty()) return;

		return;
		if (use_id) set_color(encode_id(id));
		else if (hovered) set_color(al_color_name("yellow"));
		else set_color(al_color_name("white"));


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
};
int ParkAsset::last_id = 0;



class ConcessionStand : public ParkAsset
{
public:
	ConcessionStand()
		: ParkAsset()
	{
		type = "Concession Stand";
		position = vec3d(0, 0, 0);
		rotation_y = 0;
		width = 2;
		height = 2;
		texture = NULL;
		initial_cost = 0;
		expense = 0;
		num_employees = 0;
		initial_hype = 0;
		hype_depreciation = 0;
		radius_of_influence = 0;
		time_per_customer = 0;

		build_model();
	}

	void build_model() override
	{
		vertexes.clear();
		indexes.clear();
		//ALLEGRO_COLOR c = (use_id) ? encode_id(id) : (hovered ? al_color_name("yellow") : al_map_rgb_f(1, 1, 1));

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
};






