


class ParkAsset
{
private:
	static int last_id;

public:
	// labels
	std::string name;
	int type;
	int id;

	// dimentions
	int x;
	int y;
	int width;
	int height;

	// cost
	float initial_cost;

	// expenses
	float expense;
	int num_employees;
	float initial_hype;
	float hype_depreciation;
	float radius_of_influence;
	float time_per_customer;

	ParkAsset(int type, float x, float y)
		: name("Unnamed")
		, type(0)
		, id(++last_id)
		, x(x)
		, y(y)
		, width(2)
		, height(2)
		, initial_cost(0)
		, expense(0)
		, num_employees(0)
		, initial_hype(0)
		, hype_depreciation(0)
		, radius_of_influence(0)
		, time_per_customer(0)
	{}

	virtual void draw() {}
};
int ParkAsset::last_id = 0;




