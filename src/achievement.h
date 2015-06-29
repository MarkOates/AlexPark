


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

	void check_update();

	bool virtual test_condition();
	bool virtual on_achieved();
};




