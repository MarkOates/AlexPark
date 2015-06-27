

class Camera
{
public:
	vec3d position;
	vec3d stepback;
	vec3d rotation;
	float tilt;

	float zoom_min;
	float zoom_max;
	float zoom_pos;

	Camera(float x, float y, float z)
		: position(x, y, z)
		, stepback(0, 0, 0)
		, rotation(0, 0, 0)
		, tilt(0)
		, zoom_min(10)
		, zoom_max(4)
		, zoom_pos(0.5)
	{
		set45_isometric();
	}

	void set45_isometric()
	{
		stepback = vec3d(0, 120, 120);
		rotation = vec3d(-TAU/8.0, TAU/16.0, 0);
	}

	void setup_camera_perspective(ALLEGRO_BITMAP *bmp)
	{
		ALLEGRO_TRANSFORM p;
		float aspect_ratio = (float)al_get_bitmap_height(bmp) / al_get_bitmap_width(bmp);
		al_set_target_bitmap(bmp);
		al_identity_transform(&p);

		{ // my camera translations go here
			// turn the camera right-left
			al_rotate_transform_3d(&p, 0, 1, 0, rotation.y);

			// position
			al_translate_transform_3d(&p,
				-position.x-stepback.x,
				-position.y-stepback.y,
				-position.z-stepback.z);

			// tilt the camera up-down
			al_rotate_transform_3d(&p, -1, 0, 0, rotation.x);
		}

		//al_perspective_transform(&p, -1, aspect_ratio, 1, 1, -aspect_ratio, 1000);
		float znear = 5;
		float zoom = (zoom_max - zoom_min) * zoom_pos + zoom_min; // 4 is closeup, 10 is wide
		stepback = vec3d(0, znear*zoom, znear*zoom);
		al_perspective_transform(&p,
			-1, aspect_ratio, znear,
			1, -aspect_ratio, 1000);
		al_use_projection_transform(&p);

		al_use_transform(al_get_current_transform());
		al_set_render_state(ALLEGRO_DEPTH_TEST, 1);
	}
};


