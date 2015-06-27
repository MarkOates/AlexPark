

ALLEGRO_COLOR encode_id(int id)
{
	ALLEGRO_COLOR color;
	//if (id >= (256*4)) return al_map_rgba(0, 0, 0, 0);
	unsigned char r = id / 256;
	unsigned char g = id % 256;
	return al_map_rgba(r, g, 0, 255);
}

int decode_id(ALLEGRO_COLOR color)
{
	unsigned char r, g, b, a;
	al_unmap_rgba(color, &r, &g, &b, &a);
	return r * 256 + g;
}

void color_encode_test()
{
	srand(time(0));
	for (unsigned i=0; i<20000; i++)
	{
		std::cout << i << " " << ((decode_id(encode_id(i)) == i) ? "PASS" : "FAIL" )<< std::endl;
	}
}



