

class Ground
{
public:
	int w;
	int h;
	ALLEGRO_BITMAP *texture, *remap_coordinates_texture;
	ALLEGRO_VERTEX vertexes[4];

	Ground(int w, int h, ALLEGRO_BITMAP *tex)
		: w(w)
		, h(h)
		, texture(tex)
		, remap_coordinates_texture(create_remapable_texture(w, h, 1000))
		, vertexes({
				{-w/2.0f, 0, -h/2.0f, 0, 0, al_color_name("white")},
				{-w/2.0f, 0, h/2.0f, 0, (float)h, al_color_name("white")},
				{w/2.0f, 0, h/2.0f, (float)w, (float)h, al_color_name("white")},
				{w/2.0f, 0, -h/2.0f, (float)w, 0, al_color_name("white")}
		})
	{
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
		al_draw_prim(&vertexes, NULL, use_id ? remap_coordinates_texture : texture, 0, 4, ALLEGRO_PRIM_TRIANGLE_FAN);
	}

};



