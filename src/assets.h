

float random_float(float min, float max)
{
    return ((float) rand()/RAND_MAX)*(max-min) + min;
}


ALLEGRO_COLOR mix_color(const ALLEGRO_COLOR &col1, const ALLEGRO_COLOR &col2, float scale)
{
	ALLEGRO_COLOR col;
	col.r = (col2.r - col1.r) * scale + col1.r;
	col.g = (col2.g - col1.g) * scale + col1.g;
	col.b = (col2.b - col1.b) * scale + col1.b;
	col.a = (col2.a - col1.a) * scale + col1.a;
	return col;
}



ALLEGRO_BITMAP *generate_noise_bitmap(float w, float h, ALLEGRO_COLOR color1, ALLEGRO_COLOR color2)
{
	// set everything up
	ALLEGRO_BITMAP *surface = al_create_bitmap(w, h);
	ALLEGRO_STATE state;
	al_store_state(&state, ALLEGRO_STATE_TARGET_BITMAP);

	// set the drawing surface
	al_set_target_bitmap(surface);

	// write the (randomly colored) pixels
	al_lock_bitmap(surface, ALLEGRO_PIXEL_FORMAT_ARGB_8888, ALLEGRO_LOCK_WRITEONLY);
	for (int x=0; x<w; x++)
	{
		for (int y=0; y<h; y++)
		{
			//float r = random_float(color1.r, color2.r);
			//float g = random_float(color1.g, color2.g);
			//float b = random_float(color1.b, color2.b);
			//al_put_pixel(x, y, al_map_rgba_f(r, g, b, 1.0));

			float ran = random_float(0, 1);
			al_put_pixel(x, y, mix_color(color1, color2, ran));
		}
	}
	al_unlock_bitmap(surface);

	// return everything back to the way it was
	al_restore_state(&state);

	// return the generated image
	return surface;
}


