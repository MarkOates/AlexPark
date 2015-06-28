

class Ground
{
public:
	int w;
	int h;
	ALLEGRO_BITMAP *render_surface, *remap_coordinates_texture;
	ALLEGRO_VERTEX vertexes[4];

	Ground(int w, int h)
		: w(w)
		, h(h)
		, render_surface(al_create_bitmap(1024, 1024))
		, remap_coordinates_texture(create_remapable_texture(w, h, 1000))
		, vertexes({
				{0, 0, 0, 0, 0, al_color_name("white")},
				{0, 0, (float)h, 0, (float)h, al_color_name("white")},
				{(float)w, 0, (float)h, (float)w, (float)h, al_color_name("white")},
				{(float)w, 0, 0, (float)w, 0, al_color_name("white")}
		})
	{
	}

	void fit_and_use_texture(ALLEGRO_BITMAP *bmp, int tile_n_times=1)
	{
		int w = al_get_bitmap_width(bmp);
		int h = al_get_bitmap_height(bmp);

		vertexes[0].u = 0;
		vertexes[0].v = 0;

		vertexes[1].u = 0;
		vertexes[1].v = h*tile_n_times;

		vertexes[2].u = w*tile_n_times;
		vertexes[2].v = h*tile_n_times;

		vertexes[3].u = w*tile_n_times;
		vertexes[3].v = 0; 
	}

	ALLEGRO_BITMAP *create_remapable_texture(int width, int height, int offset_number)
	{
		ALLEGRO_BITMAP *bmp = al_create_bitmap(width, height);
		ALLEGRO_STATE state;
		al_store_state(&state, ALLEGRO_STATE_TARGET_BITMAP);
		al_set_target_bitmap(bmp);
		al_clear_to_color(al_color_name("black"));
		al_lock_bitmap(bmp, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_WRITEONLY);
		for (unsigned y=0; y<height; y++)
			for (unsigned x=0; x<width; x++)
			{
				al_put_pixel(x, y, encode_id(offset_number + x + y*width));
			}
		al_unlock_bitmap(bmp);
		al_restore_state(&state);
		return bmp;
	}

	void unmap_texture_coordinates(int id, int offset_number, int *x, int *y)
	{
		id -= offset_number;
		*x = id % w;
		*y = id / w;
	}	

	void draw(bool use_id=false)
	{
		if (use_id) fit_and_use_texture(remap_coordinates_texture);
		else fit_and_use_texture(render_surface);
		al_draw_prim(&vertexes, NULL, use_id ? remap_coordinates_texture : render_surface, 0, 4, ALLEGRO_PRIM_TRIANGLE_FAN);
	}
};



