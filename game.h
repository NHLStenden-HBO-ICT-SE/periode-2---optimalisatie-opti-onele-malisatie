#pragma once

namespace Tmpl8
{
//forward declarations
class Tank;
class Rocket;
class Smoke;
class Particle_beam;

class Game
{
  public:
    void set_target(Surface* surface) { screen = surface; }
    void init();
    void shutdown();
    void update(float deltaTime);
    void draw();
    void tick(float deltaTime);
    void insertion_sort_tanks_health(const vector<Tank>& original, vector<const Tank*>& sorted_tanks, int begin, int end);
    void draw_health_bars(const vector<const Tank*>& sorted_tanks, const int team);
    void measure_performance();
    vector<Explosion*> getExplosions();
    vector<Smoke*> getSmokes();
    
    //vector<Tank> sort(vector<Tank> sortlist);

    Tank& find_closest_enemy(Tank& current_tank);

    void mouse_up(int button)
    { /* implement if you want to detect mouse button presses */
    }

    void mouse_down(int button)
    { /* implement if you want to detect mouse button presses */
    }

    void mouse_move(int x, int y)
    { /* implement if you want to detect mouse movement */
    }

    void key_up(int key)
    { /* implement if you want to handle keys */
    }

    void key_down(int key)
    { /* implement if you want to handle keys */
    }

  private:
    Surface* screen;
    
    vector<Tank> tanks;
    vector<Tank*> blue_tanks;
    vector<Tank*> red_tanks;

    vector<Tank*> active_tanks;
    vector<Rocket> rockets;
    vector<Smoke> smokes;
    vector<Explosion> explosions;
    vector<Particle_beam> particle_beams;

    Terrain background_terrain;
    vector<vec2> forcefield_hull;

    Font* frame_count_font;
    int frame_count = 0;

    bool lock_update = false;

    

    //Checks if a point lies on the left of an arbitrary angled line
    bool left_of_line(vec2 line_start, vec2 line_end, vec2 point);
};

}; // namespace Tmpl8