





class FirstVisitor : public Achievement
{
public:
	FirstVisitor(Park &park, HUD &hud)
		: Achievement(park, hud)
	{
		name = ACH_FIRST_VISITOR;
		achievement_message = "Congratulations! Your first visitor has arrived.  What a momentus day!";
	}
	bool test_condition() override
	{
		if (park.visitors.size() > 0) return true;
		return false;
	}
};




Achievement *FACTORY_create_achievement(std::string achievement_name, Park &park, HUD &hud)
{
	if (achievement_name == ACH_FIRST_VISITOR)
		return new FirstVisitor(park, hud);
	
	std::cerr << "Could not create an achievement for \"" << achievement_name << "\"" << std::endl;
	return NULL;
}



