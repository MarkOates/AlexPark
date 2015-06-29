





class YourFirstVisitor : public Achievement
{
public:
	YourFirstVisitor(Park &park, HUD &hud)
		: Achievement(park, hud)
	{
		name = "First Visitor";
		achievement_message = "Congratulations! Your first visitor has arrived.  What a momentus day!";
	}
	bool test_condition() override
	{
		if (park.visitors.size() > 0) return true;
		return false;
	}
	bool on_achieved() override
	{
	}
};



