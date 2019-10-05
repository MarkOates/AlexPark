


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
	void virtual on_achieved();
};



Achievement *FACTORY_create_achievement(std::string achievement_name, Park &park, HUD &hud);


#define ACH_FIRST_VISITOR "First Visitor!"
#define ACH_MORE_THAN_15_VISITORS "More Than 15 Visitors"
#define ACH_INFO_CENTER "Info Center"
#define ACH_WATER_FOUNTAIN "Water Fountain"
#define ACH_MERRY_GO_ROUND "Merry Go Round"
#define ACH_PUBLIC_RESTROOMS "Public Restrooms"
#define ACH_MERCHANDISE_STORE "Merchandise Store"
#define ACH_CRAZY_LAND "Crazy Land"
#define ACH_HORROR_HOUSE "Horror House"
#define ACH_ROLLER_COASTER "Roller Coaster"
#define ACH_FERRIS_WHEEL "Ferris Wheel"
#define ACH_ALEX_STATUE "Alex the Alligator Statue"



