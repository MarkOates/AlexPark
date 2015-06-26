


class HeatMap
{
public:
	int val[32][32];
	int tile_width;
	ALLEGRO_COLOR max_color;
	ALLEGRO_COLOR min_color;
	ALLEGRO_BITMAP *render;
	HeatMap()
		: max_color(al_color_name("orange"))
		, min_color(al_map_rgba_f(0.0, 0.0, 0.0, 0.0))
		, tile_width(32)
		, render(NULL)
	{
		for (unsigned y=0; y<32; y++)
			for (unsigned x=0; x<32; x++)
				val[x][y] = 0;
		render = al_create_bitmap(32, 32);
	}
	ALLEGRO_COLOR mix(const ALLEGRO_COLOR &col1, const ALLEGRO_COLOR &col2, float scale)
	{
		ALLEGRO_COLOR col;
		col.r = (col2.r - col1.r) * scale + col1.r;
		col.g = (col2.g - col1.g) * scale + col1.g;
		col.b = (col2.b - col1.b) * scale + col1.b;
		col.a = (col2.a - col1.a) * scale + col1.a;
		return col;
	}
	ALLEGRO_COLOR get_heat_color(float val)
	{
		return mix(max_color, min_color, val);
	}
	void update_render()
	{
		ALLEGRO_STATE previous_state;
		al_store_state(&previous_state, ALLEGRO_STATE_TARGET_BITMAP);
		al_set_target_bitmap(render);
		al_lock_bitmap(render, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_WRITEONLY);

		for (unsigned y=0; y<32; y++)
			for (unsigned x=0; x<32; x++)
			{
				al_put_pixel(x, y, get_heat_color(val[x][y] / 100.0));
			}

		al_unlock_bitmap(render);
	}
	void draw()
	{
		if (render) al_draw_bitmap(render, 0, 0, 0);
	}
};


