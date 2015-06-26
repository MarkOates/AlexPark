


class Park
{
public:
	std::vector<ParkAsset *> assets;
	Park()
		: assets()
	{
	}

	void draw()
	{
		for (unsigned i=0; i<assets.size(); i++)
			assets[i]->draw();
	}
};


