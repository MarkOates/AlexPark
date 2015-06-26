


class Park
{
public:
	std::vector<ParkAsset *> assets;
	HeatMap traffic;
	Park()
		: assets()
		, traffic()
	{
	}

	void draw()
	{
		for (unsigned i=0; i<assets.size(); i++)
			assets[i]->draw();

		traffic.draw();
	}
};


