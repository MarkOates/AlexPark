

#include <allegro5/allegro.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <algorithm>
#include <deque>

#define TAU (ALLEGRO_PI * 2)


#include "motion.h"
#include "color_encode_id.h"
#include "assets.h"
#include "vec3d.h"
#include "park_asset.h"
#include "heat_map.h"
#include "achievement.h"
#include "park.h"
#include "hud.h"
#include "ground.h"
#include "camera.h"
#include "achievements.h"


Achievement::Achievement(Park &park, HUD &hud)
   : name("[achievement_name]")
   , achieved(false)
   , achievement_message("[achievement_message]")
   , park(park)
   , hud(hud)
{}

bool Achievement::check_update()
{
   if (achieved) return false;
   achieved = test_condition();
   if (achieved)
   {
      on_achieved();
      hud.spawn_dialogue("Achievement Unlocked!", achievement_message.c_str(), 0xf091, al_color_name("gold"));
      return true;
   }
   return false;
}

bool Achievement::test_condition() 
{
   return false;
}

void Achievement::on_achieved()
{
   // spawn a dialogue
   // unlock some thing that is currently locked
}



class Args
{
private:
   std::vector<std::string> args;

   bool find(std::string string)
   {
      return std::find(args.begin(), args.end(), string) != args.end();
   }

public:
   Args()
      : args{}
   {}

   void set(int argc, char **argv)
   {
      for (int i=0; i<argc; i++) args.push_back(argv[i]);
   }

   bool skip_dialogs()
   {
      return find("skip_tutorial_dialogs");
   }
};



class Project
{
public:
   ALLEGRO_DISPLAY *display;
   ALLEGRO_BITMAP *pointer_target_buffer;
   Args args;
   Motion motion;
   Camera camera;
   Park park;
   HUD hud;
   bool abort_game;

   ALLEGRO_BITMAP *texture;
   Ground ground;

   int mouse_x, mouse_y;
   int ground_x, ground_y;

   ALLEGRO_BITMAP *asset_plot_shadow;
   ALLEGRO_BITMAP *grass_texture;
   ALLEGRO_BITMAP *stone_walkway;

   Project(ALLEGRO_DISPLAY *display, Args args)
      : display(display)
      , pointer_target_buffer(al_create_bitmap(al_get_display_width(display), al_get_display_height(display)))
      , args(args)
      , motion()
      , camera(0, 0, 0)
      , park()
      , hud(display, pointer_target_buffer, &park, motion)
      , abort_game(false)
      , texture(al_load_bitmap("data/bitmaps/stone.png"))
      , ground(32, 32)
      , mouse_x(0)
      , mouse_y(0)
      , ground_x(0)
      , ground_y(0)
      , asset_plot_shadow(al_load_bitmap("data/bitmaps/plot_shadow.png"))
      , grass_texture(generate_noise_bitmap(1024, 1024, al_color_html("89ae58"), al_color_html("6e943c")))
      , stone_walkway(al_load_bitmap("data/bitmaps/stone_walkway.png"))
   {
      // set the camera position to nice and wide
      camera.zoom_pos -= 1.0;
      camera.position.x += 20;
      camera.position.y -= 10;
      for (unsigned i=0; i<1; i++)
      {
         //park.purchase_asset(hud.get_current_selected_asset(), 16, 16);
      }

      if (!args.skip_dialogs())
      {
         hud.spawn_dialogue("Welcome to Your Park!", "What an exciting time to be a business person.  You've been given this plot of land and have been granted a permit to create a park.", 0xf15b);
         hud.spawn_dialogue("Welcome to Your Park!", "But right now, there's nobody here!  It's just an empty plot.  So let's start by building something to see if we can get people interested in coming.", 0xf0a1);
         hud.spawn_dialogue("Welcome to Your Park!", "To start, click the icon on the bottom right.  This will open up your ASSETS window.  There's not a lot there to build with, yet, but it's a start.", 0xf0a1);
      }
   }

   void refresh_ground_render_surface()
   {
      ALLEGRO_STATE previous_state;
      al_store_state(&previous_state, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_BLENDER);
      al_set_target_bitmap(ground.render_surface);      
      al_set_render_state(ALLEGRO_DEPTH_TEST, 0);

      al_clear_to_color(al_color_html("689f41"));
      float x_scale = al_get_bitmap_width(ground.render_surface) / (float)ground.w;
      float y_scale = al_get_bitmap_height(ground.render_surface) / (float)ground.h;

      al_draw_bitmap(grass_texture, 0, 0, 0);

      // draw the nice eyecandy shadows and stone_walkways
      for (unsigned i=0; i<park.assets.size(); i++)
      {
         int x = park.assets[i]->position.x * x_scale;
         int y = park.assets[i]->position.z * y_scale;
         if (park.assets[i]->type == PA_WALKWAY)
            al_draw_bitmap(stone_walkway, x, y, 0);
         else
            al_draw_bitmap(asset_plot_shadow, x-16, y-16, 0);
      }

      // draw the cursor
      if (!hud.dialogue_is_open())
      {
         if (ground_x >= 0)
         {
            al_draw_circle(ground_x * x_scale + x_scale/2, ground_y * y_scale + x_scale/2,
               16, al_map_rgba_f(0.1, 0.1, 0.1, 0.1), 3.0);
            al_draw_rectangle(ground_x * x_scale, ground_y * y_scale,
               ground_x * x_scale + x_scale, ground_y * y_scale + y_scale,
               al_color_name("orange"), 3);
            //al_draw_filled_rectangle(ground_x * x_scale, ground_y * y_scale,
            //   ground_x * x_scale + x_scale, ground_y * y_scale + y_scale,
            //   al_color_name("yellow"));
         }
      }

      al_restore_state(&previous_state);
      al_set_render_state(ALLEGRO_DEPTH_TEST, 1);
   }

   void on_timer(ALLEGRO_EVENT &ev)
   {
      // update the motion manager
      motion.update(ev.timer.timestamp);

      //
      // update the park
      //

      if (!hud.dialogue_is_open())
      {
         park.update();
         hud.test_achievements();
      }


      //
      // this draws the ids as colors into a buffer
      //

      camera.setup_camera_perspective(pointer_target_buffer);
      al_clear_to_color(al_color_name("black"));
      al_clear_depth_buffer(1000);

      ground.draw(true);

      park.draw(true);

      hud.draw(true);


      //
      // parse the asset data and retrieve the useful information
      //

      // set the current hovered_asset_id
      ALLEGRO_COLOR pointed_pixel_value = al_get_pixel(pointer_target_buffer, mouse_x, mouse_y);
      park.hovered_asset_id = decode_id(pointed_pixel_value);

      ground_x = -1;
      ground_y = -1;
      hud.hovered_ui_id = -1;
      if (park.hovered_asset_id >= 40000) // gui
      {
         hud.hovered_ui_id = park.hovered_asset_id - 40000;
      }
      else if (park.hovered_asset_id >= 1000)
      {
         ground.unmap_texture_coordinates(park.hovered_asset_id, 1000, &ground_x, &ground_y);
         //al_set_target_bitmap(ground.texture);
         //al_put_pixel(x, y, al_color_name("dodgerblue"));
      }

      //render_ground();

      // set the park asset as "hovered"
      for (unsigned i=0; i<park.assets.size(); i++)
         park.assets[i]->hovered = (park.assets[i]->id == park.hovered_asset_id);


      //
      // this draws the actual scene
      //

      refresh_ground_render_surface();

      camera.setup_camera_perspective(al_get_backbuffer(display));
      al_clear_to_color(al_color_name("dodgerblue"));
      al_clear_depth_buffer(1000);

      ground.draw();

      park.draw();   

      hud.draw();
   }
   void on_key_char(ALLEGRO_EVENT &ev)
   {
      if (hud.dialogue_is_open())
      {
         hud.close_dialogue();
         return;
      }

      switch(ev.keyboard.keycode)
      {
         case ALLEGRO_KEY_RIGHT:
            camera.position.x ++;
            break;
         case ALLEGRO_KEY_LEFT:
            camera.position.x --;
            break;
         case ALLEGRO_KEY_UP:
            camera.position.z--;
            break;
         case ALLEGRO_KEY_DOWN:
            camera.position.z++;
            break;
         case ALLEGRO_KEY_OPENBRACE:
            camera.zoom_pos += 0.1;
            break;
         case ALLEGRO_KEY_CLOSEBRACE:
            camera.zoom_pos -= 0.1;
            break;
         case ALLEGRO_KEY_EQUALS:
            park.money += 1000;
            break;
         case ALLEGRO_KEY_F1:
            // this will save a bitmap of the pointer_target_buffer,
            // which is the bitmap used to ditermine what object
            // the mouse is pointing at
            al_save_bitmap("pointer_buffer.bmp", pointer_target_buffer);
            break;
         case ALLEGRO_KEY_M:
            hud.toggle_asset_window();
            break;
      }
   }
   void on_key_up() {}
   void on_key_down() {}
   void on_mouse_axes(ALLEGRO_EVENT &ev)
   {
      mouse_x = ev.mouse.x;
      mouse_y = ev.mouse.y;
   }
   void on_mouse_up() {}
   void on_mouse_down()
   {
      if (hud.dialogue_is_open())
      {
         hud.close_dialogue();
         return;
      }

      if (park.hovered_asset_id == 60000)
      {
         hud.toggle_asset_window();
         return;
      }

      hud.activate_ui(hud.hovered_ui_id);
      
      if (ground_x < 0 || ground_y < 0) return;

      park.purchase_asset(hud.get_current_selected_asset(), ground_x, ground_y);
   }
};



int main(int argc, char* argv[])
{
   al_init();

   al_init_primitives_addon();
   al_init_image_addon();
   al_install_keyboard();
   al_install_mouse();
   al_init_font_addon();
   al_init_ttf_addon();

   ALLEGRO_PATH *resource_path = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
   al_change_directory(al_path_cstr(resource_path, ALLEGRO_NATIVE_PATH_SEP));
   al_destroy_path(resource_path);

   ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
   ALLEGRO_TIMER *timer = al_create_timer(1.0/60.0);

   // create the display
   al_set_new_display_option(ALLEGRO_DEPTH_SIZE, 32, ALLEGRO_SUGGEST);
   al_set_new_display_option(ALLEGRO_SUPPORT_NPOT_BITMAP, 0, ALLEGRO_REQUIRE);
   ALLEGRO_DISPLAY *display = al_create_display(1920, 1080);

   al_register_event_source(event_queue, al_get_timer_event_source(timer));
   al_register_event_source(event_queue, al_get_mouse_event_source());   
   al_register_event_source(event_queue, al_get_keyboard_event_source());   
   al_register_event_source(event_queue, al_get_display_event_source(display));   

   Args args;
   args.set(argc, argv);

   Project project(display, args);
   al_start_timer(timer);

   while (!project.abort_game)
   {
      ALLEGRO_EVENT current_event, next_event;
      al_wait_for_event(event_queue, &current_event);

      switch(current_event.type)
      {
      case ALLEGRO_EVENT_TIMER:
         project.on_timer(current_event);
         al_flip_display();
         while (al_peek_next_event(event_queue, &next_event)
            && next_event.type == ALLEGRO_EVENT_TIMER
            && next_event.timer.source == current_event.timer.source)
               al_drop_next_event(event_queue);
         break;
      case ALLEGRO_EVENT_KEY_CHAR:
         project.on_key_char(current_event);
         break;
      case ALLEGRO_EVENT_KEY_DOWN:
         project.on_key_down();
         break;
      case ALLEGRO_EVENT_KEY_UP:
         project.on_key_up();
         break;
      case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
         project.on_mouse_down();
         break;
      case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
         project.on_mouse_up();
         break;
      case ALLEGRO_EVENT_MOUSE_AXES:
         project.on_mouse_axes(current_event);
         break;
      case ALLEGRO_EVENT_DISPLAY_CLOSE:
         project.abort_game = true;
         break;
      default:
         break;
      }
   }


   return 0;
}


