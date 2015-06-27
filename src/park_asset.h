


class ParkAsset
{
private:
	static int last_id;

public:
	// labels
	int type;
	int id;

	// dimentions
	vec3d position;
	float rotation_y;
	int width;
	int height;

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
		: type(0)
		, id(++last_id)
		, position(0, 0, 0)
		, rotation_y(0)
		, width(2)
		, height(2)
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
	{}

	void set_texture(ALLEGRO_BITMAP *tx)
	{
		texture = tx;
	}

	virtual void draw()
	{
		ALLEGRO_COLOR c = al_map_rgb_f(1, 1, 1);
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

		al_identity_transform(&t);
		al_rotate_transform_3d(&t, 0, 1, 0, rotation_y);
		al_translate_transform_3d(&t, position.x, position.y, position.z);
		al_use_transform(&t);
		al_draw_indexed_prim(vtx, NULL, texture, indices, 12, ALLEGRO_PRIM_TRIANGLE_LIST);

	}
};
int ParkAsset::last_id = 0;




