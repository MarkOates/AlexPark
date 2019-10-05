





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



class MoreThan15Visitors : public Achievement
{
public:
	MoreThan15Visitors(Park &park, HUD &hud)
		: Achievement(park, hud)
	{
		name = ACH_MORE_THAN_15_VISITORS;
		achievement_message = "Wow, you're park is really growing fast!  You already have 15 visitors in your park.  You have UNLOCKED the Park Bench and Jungle Gym!";
	}
	bool test_condition() override
	{
		if (park.visitors.size() >= 15) return true;
		return false;
	}
	void on_achieved() override
	{
		hud.all_assets.push_back(FACTORY_create_asset(PA_PARK_BENCH));
		hud.all_assets.push_back(FACTORY_create_asset(PA_JUNGLE_GYM));
	}
};




class WaterFountainAchievement : public Achievement
{
public:
	WaterFountainAchievement(Park &park, HUD &hud)
		: Achievement(park, hud)
	{
		name = ACH_WATER_FOUNTAIN;
		achievement_message = "Not everybody is here to buy things.  Some just want to enjoy the scerenity.  How about building a nice WATER FOUNTAIN?";
	}
	bool test_condition() override
	{
		if (park.visitors.size() >= 20) return true;
		return false;
	}
	void on_achieved() override
	{
		hud.all_assets.push_back(FACTORY_create_asset(PA_WATER_FOUNTAIN));
	}
};





class MerryGoRoundAchievement : public Achievement
{
public:
	MerryGoRoundAchievement(Park &park, HUD &hud)
		: Achievement(park, hud)
	{
		name = ACH_MERRY_GO_ROUND;
		achievement_message = "People are really starting to like this place.  They're asking for more, something fun!  How about we build a new MERRY-GO-ROUND?";
	}
	bool test_condition() override
	{
		if (park.visitors.size() >= 30) return true;
		return false;
	}
	void on_achieved() override
	{
		hud.all_assets.push_back(FACTORY_create_asset(PA_MERRY_GO_ROUND));
	}
};



class InformationCenterAchievement : public Achievement
{
public:
	InformationCenterAchievement(Park &park, HUD &hud)
		: Achievement(park, hud)
	{
		name = ACH_INFO_CENTER;
		achievement_message = "Look at this place, you're a natural!  You know what, I think it's time we get this place buzzing and tell people what we're all about. Let's build an INFORMATION CENTER.";
	}
	bool test_condition() override
	{
		if (park.visitors.size() >= 50) return true;
		return false;
	}
	void on_achieved() override
	{
		hud.all_assets.push_back(FACTORY_create_asset(PA_INFORMATION_CENTER));
	}
};




class PublicRestroomsAchievement : public Achievement
{
public:
	PublicRestroomsAchievement(Park &park, HUD &hud)
		: Achievement(park, hud)
	{
		name = ACH_PUBLIC_RESTROOMS;
		achievement_message = "Yikes!  Everyone seems to enjoying this place, but they aren't staying for very long.  All this food and drink, and nowhere to 'go'.  You unlocked PUBLIC RESTROOMS.";
	}
	bool test_condition() override
	{
		if (park.visitors.size() >= 60) return true;
		return false;
	}
	void on_achieved() override
	{
		hud.all_assets.push_back(FACTORY_create_asset(PA_PUBLIC_RESTROOMS));
	}
};





class MerchandiseStoreAchievement : public Achievement
{
public:
	MerchandiseStoreAchievement(Park &park, HUD &hud)
		: Achievement(park, hud)
	{
		name = ACH_MERCHANDISE_STORE;
		achievement_message = "Yup.  We're starting to make a name for ourselves.  Visitors are enjoying this place so much that they're wanting to take a piece of it home.  You unlocked MERCHANDISE STORE.";
	}
	bool test_condition() override
	{
		if (park.visitors.size() >= 100) return true;
		return false;
	}
	void on_achieved() override
	{
		hud.all_assets.push_back(FACTORY_create_asset(PA_MERCHANDISE_STORE));
	}
};




class CrazyLandAchievement : public Achievement
{
public:
	CrazyLandAchievement(Park &park, HUD &hud)
		: Achievement(park, hud)
	{
		name = ACH_CRAZY_LAND;
		achievement_message = "This place is getting crazy.  So crazy in fact that customers are asking for some serious theme park excitement.  You unlocked CRAZY LAND.";
	}
	bool test_condition() override
	{
		if (park.visitors.size() >= 200) return true;
		return false;
	}
	void on_achieved() override
	{
		hud.all_assets.push_back(FACTORY_create_asset(PA_CRAZY_LAND));
	}
};




class HorrorHouseAchievement : public Achievement
{
public:
	HorrorHouseAchievement(Park &park, HUD &hud)
		: Achievement(park, hud)
	{
		name = ACH_HORROR_HOUSE;
		achievement_message = "It's scary how fast this place has grown.  You unlocked HORROR HOUSE.";
	}
	bool test_condition() override
	{
		if (park.visitors.size() >= 300) return true;
		return false;
	}
	void on_achieved() override
	{
		hud.all_assets.push_back(FACTORY_create_asset(PA_HORROR_HOUSE));
	}
};




class RollerCoasterAchievement : public Achievement
{
public:
	RollerCoasterAchievement(Park &park, HUD &hud)
		: Achievement(park, hud)
	{
		name = ACH_ROLLER_COASTER;
		achievement_message = "Making this park has been one wild ride.  You unlocked ROLLER COASTER.";
	}
	bool test_condition() override
	{
		if (park.visitors.size() >= 500) return true;
		return false;
	}
	void on_achieved() override
	{
		hud.all_assets.push_back(FACTORY_create_asset(PA_ROLLER_COASTER));
	}
};




class FerrisWheelAchievement : public Achievement
{
public:
	FerrisWheelAchievement(Park &park, HUD &hud)
		: Achievement(park, hud)
	{
		name = ACH_FERRIS_WHEEL;
		achievement_message = "You know, from the way I see things up here, the sky's the limit.  You unlocked FERRIS WHEEL.";
	}
	bool test_condition() override
	{
		if (park.visitors.size() >= 800) return true;
		return false;
	}
	void on_achieved() override
	{
		hud.all_assets.push_back(FACTORY_create_asset(PA_FERRIS_WHEEL));
	}
};



class AlexStatueAchievement : public Achievement
{
public:
	AlexStatueAchievement(Park &park, HUD &hud)
		: Achievement(park, hud)
	{
		name = ACH_ALEX_STATUE;
		achievement_message = "I don't think things could be much better. You've made one of the best park's I've ever seen. Congratulations, you WON THE GAME and unlocked the final asset.";
	}
	bool test_condition() override
	{
		if (park.visitors.size() >= 1000) return true;
		return false;
	}
	void on_achieved() override
	{
		hud.all_assets.push_back(FACTORY_create_asset(PA_ALEX_STATUE));
	}
};






Achievement *FACTORY_create_achievement(std::string achievement_name, Park &park, HUD &hud)
{
	if (achievement_name == ACH_FIRST_VISITOR) return new FirstVisitor(park, hud);
	if (achievement_name == ACH_MORE_THAN_15_VISITORS) return new MoreThan15Visitors(park, hud);
	if (achievement_name == ACH_WATER_FOUNTAIN) return new WaterFountainAchievement(park, hud);
	if (achievement_name == ACH_INFO_CENTER) return new InformationCenterAchievement(park, hud);
	if (achievement_name == ACH_MERRY_GO_ROUND) return new MerryGoRoundAchievement(park, hud);
	if (achievement_name == ACH_PUBLIC_RESTROOMS) return new PublicRestroomsAchievement(park, hud);
	if (achievement_name == ACH_MERCHANDISE_STORE) return new MerchandiseStoreAchievement(park, hud);
	if (achievement_name == ACH_CRAZY_LAND) return new CrazyLandAchievement(park, hud);
	if (achievement_name == ACH_HORROR_HOUSE) return new HorrorHouseAchievement(park, hud);
	if (achievement_name == ACH_ROLLER_COASTER) return new RollerCoasterAchievement(park, hud);
	if (achievement_name == ACH_FERRIS_WHEEL) return new FerrisWheelAchievement(park, hud);
	if (achievement_name == ACH_ALEX_STATUE) return new AlexStatueAchievement(park, hud);
	
	std::cerr << "Could not create an achievement for \"" << achievement_name << "\"" << std::endl;
	return NULL;
}



