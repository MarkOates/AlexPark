


class HUD;
class Park;


class Achievement
{
public:
	std::string name;
	std::string achievement_message;
	bool achieved;

	Park &park;
	HUD &hud;

	Achievement(Park &park, HUD &hud);

	bool check_update(); // returns true if the achievement was met during this check

	bool virtual test_condition();
	bool virtual on_achieved();
};



Achievement *FACTORY_create_achievement(std::string achievement_name, Park &park, HUD &hud);


#define ACH_FIRST_VISITOR "First Visitor!"



