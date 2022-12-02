#include "precomp.h" // include (only) this in every .cpp file
#include <vector>
#include <iterator>

using namespace std;
//2048
constexpr auto num_tanks_blue = 2048;
constexpr auto num_tanks_red = 2048;

constexpr auto tank_max_health = 1000;
constexpr auto rocket_hit_value = 60;
constexpr auto particle_beam_hit_value = 50;

constexpr auto tank_max_speed = 1.0;

constexpr auto health_bar_width = 70;

constexpr auto max_frames = 2000;

//Global performance timer
//jasper time: 84736.2
//Quickest jasper time: 77339.3
constexpr auto REF_PERFORMANCE = 85699.5; //UPDATE THIS WITH YOUR REFERENCE PERFORMANCE (see console after 2k frames)
static timer perf_timer;
static float duration;

//Load sprite files and initialize sprites
static Surface* tank_red_img = new Surface("assets/Tank_Proj2.png");
static Surface* tank_blue_img = new Surface("assets/Tank_Blue_Proj2.png");
static Surface* rocket_red_img = new Surface("assets/Rocket_Proj2.png");
static Surface* rocket_blue_img = new Surface("assets/Rocket_Blue_Proj2.png");
static Surface* particle_beam_img = new Surface("assets/Particle_Beam.png");
static Surface* smoke_img = new Surface("assets/Smoke.png");
static Surface* explosion_img = new Surface("assets/Explosion.png");

static Sprite tank_red(tank_red_img, 12);
static Sprite tank_blue(tank_blue_img, 12);
static Sprite rocket_red(rocket_red_img, 12);
static Sprite rocket_blue(rocket_blue_img, 12);
static Sprite smoke(smoke_img, 4);
static Sprite explosion(explosion_img, 9);
static Sprite particle_beam_sprite(particle_beam_img, 3);

const static vec2 tank_size(7, 9);
const static vec2 rocket_size(6, 6);

const static float tank_radius = 3.f;
const static float rocket_radius = 5.f;

// -----------------------------------------------------------
// Initialize the simulation state
// This function does not count for the performance multiplier
// (Feel free to optimize anyway though ;) )
// -----------------------------------------------------------
void Game::init()
{
    frame_count_font = new Font("assets/digital_small.png", "ABCDEFGHIJKLMNOPQRSTUVWXYZ:?!=-0123456789.");

    tanks.reserve(num_tanks_blue + num_tanks_red);

    uint max_rows = 24;

    float start_blue_x = tank_size.x + 40.0f;
    float start_blue_y = tank_size.y + 30.0f;

    float start_red_x = 1088.0f;
    float start_red_y = tank_size.y + 30.0f;

    float spacing = 7.5f;

    //Spawn blue tanks
    for (int i = 0; i < num_tanks_blue; i++)
    {
        vec2 position{ start_blue_x + ((i % max_rows) * spacing), start_blue_y + ((i / max_rows) * spacing) };
        tanks.push_back(Tank(position.x, position.y, BLUE, &tank_blue, &smoke, 1100.f, position.y + 16, tank_radius, tank_max_health, tank_max_speed));
    }
    //Spawn red tanks
    for (int i = 0; i < num_tanks_red; i++)
    {
        vec2 position{ start_red_x + ((i % max_rows) * spacing), start_red_y + ((i / max_rows) * spacing) };
        tanks.push_back(Tank(position.x, position.y, RED, &tank_red, &smoke, 100.f, position.y + 16, tank_radius, tank_max_health, tank_max_speed));
    }
    
    for (int i = 0; i < num_tanks_blue + num_tanks_red; i++)
    {
        active_tanks.push_back(&tanks.at(i));
    }

    particle_beams.push_back(Particle_beam(vec2(590, 327), vec2(100, 50), &particle_beam_sprite, particle_beam_hit_value));
    particle_beams.push_back(Particle_beam(vec2(64, 64), vec2(100, 50), &particle_beam_sprite, particle_beam_hit_value));
    particle_beams.push_back(Particle_beam(vec2(1200, 600), vec2(100, 50), &particle_beam_sprite, particle_beam_hit_value));
}

// -----------------------------------------------------------
// Close down application
// -----------------------------------------------------------
void Game::shutdown()
{
}

// -----------------------------------------------------------
// Iterates through all tanks and returns the closest enemy tank for the given tank
// -----------------------------------------------------------
Tank& Game::find_closest_enemy(Tank& current_tank)
{
    float closest_distance = numeric_limits<float>::infinity();
    int closest_index = 0;

    for (int i = 0; i < tanks.size(); i++)
    {
        if (tanks.at(i).allignment != current_tank.allignment && tanks.at(i).active)
        {
            float sqr_dist = fabsf((tanks.at(i).get_position() - current_tank.get_position()).sqr_length());
            if (sqr_dist < closest_distance)
            {
                closest_distance = sqr_dist;
                closest_index = i;
            }
        }
    }

    return tanks.at(closest_index);
}

//Checks if a point lies on the left of an arbitrary angled line
bool Tmpl8::Game::left_of_line(vec2 line_start, vec2 line_end, vec2 point)
{
    return ((line_end.x - line_start.x) * (point.y - line_start.y) - (line_end.y - line_start.y) * (point.x - line_start.x)) < 0;
}

// -----------------------------------------------------------
// Update the game state:
// Move all objects
// Update sprite frames
// Collision detection
// Targeting etc..
// -----------------------------------------------------------
void Game::update(float deltaTime)
{
    //Calculate the route to the destination for each tank using BFS
    //Initializing routes here so it gets counted for performance..
    if (frame_count == 0)
    {
        for (Tank& t : tanks)
        {
            t.set_route(background_terrain.get_route(t, t.target));
        }
    }

    //Check tank collision and nudge tanks away from each other
    for (Tank* tank : active_tanks)
    {
            for (Tank& other_tank : tanks)
            {
                if (tank == &other_tank || !other_tank.active) continue;

                vec2 dir = tank->get_position() - other_tank.get_position();
                float dir_squared_len = dir.sqr_length();

                float col_squared_len = (tank->get_collision_radius() + other_tank.get_collision_radius());
                col_squared_len *= col_squared_len;

                if (dir_squared_len < col_squared_len)
                {
                    tank->push(dir.normalized(), 1.f);
                }
            }
    }

    //Update tanks
    for (Tank* tank : active_tanks)
    {
            //Move tanks according to speed and nudges (see above) also reload
            tank->tick(background_terrain);

            //Shoot at closest target if reloaded
            if (tank->rocket_reloaded())
            {
                Tank& target = find_closest_enemy(*tank);

                rockets.push_back(Rocket(tank->position, (target.get_position() - tank->position).normalized() * 3, rocket_radius, tank->allignment, ((tank->allignment == RED) ? &rocket_red : &rocket_blue)));

                tank->reload_rocket();
            }
    }

    //Update smoke plumes
    for (Smoke& smoke : smokes)
    {
        smoke.tick();
    }

    //Calculate "forcefield" around active tanks
    forcefield_hull.clear();

    
    //Find bottom most tank position
    vector<Tank*> BottomMergedTanks = BottomTankSort(active_tanks);
    vec2 point_on_hull = BottomMergedTanks.at(0)->position;
 

    //Calculate convex hull for 'rocket barrier'

    forcefield_hull.push_back(point_on_hull);
    ConvexHull(BottomMergedTanks);

    /*
    for (Tank* tank : LeftMergedTanks)
    {
            forcefield_hull.push_back(point_on_hull);
            vec2 endpoint = LeftMergedTanks.at(LeftMergedTanks.size() - 1)->position;

            for (Tank* tank : LeftMergedTanks)
            {
                if ((endpoint == point_on_hull) || left_of_line(point_on_hull, endpoint, tank->position))
                {
                    endpoint = tank->position;
                }
            }
            point_on_hull = endpoint;

            if (endpoint == forcefield_hull.at(0))
            {
                break;
            }
    }
    */
    //Update rockets
    for (Rocket& rocket : rockets)
    {
        rocket.tick();

        //Check if rocket collides with enemy tank, spawn explosion, and if tank is destroyed spawn a smoke plume
        for (Tank* tank : active_tanks)
        {
            if ((tank->allignment != rocket.allignment) && rocket.intersects(tank->position, tank->collision_radius))
            {
                explosions.push_back(Explosion(&explosion, tank->position));

                if (tank->hit(rocket_hit_value))
                {
                    smokes.push_back(Smoke(smoke, tank->position - vec2(7, 24)));
                }

                rocket.active = false;
                break;
            }
        }
    }

    //Disable rockets if they collide with the "forcefield"
    //Hint: A point to convex hull intersection test might be better here? :) (Disable if outside)
    for (Rocket& rocket : rockets)
    {
        if (rocket.active)
        {
            for (size_t i = 0; i < forcefield_hull.size(); i++)
            {
                if (circle_segment_intersect(forcefield_hull.at(i), forcefield_hull.at((i + 1) % forcefield_hull.size()), rocket.position, rocket.collision_radius))
                {
                    explosions.push_back(Explosion(&explosion, rocket.position));
                    rocket.active = false;
                }
            }
        }
    }



    //Remove exploded rockets with remove erase idiom
    rockets.erase(std::remove_if(rockets.begin(), rockets.end(), [](const Rocket& rocket) { return !rocket.active; }), rockets.end());

    //Update particle beams
    for (Particle_beam& particle_beam : particle_beams)
    {
        particle_beam.tick(tanks);

        //Damage all tanks within the damage window of the beam (the window is an axis-aligned bounding box)
        for (Tank* tank : active_tanks)
        {
            if (particle_beam.rectangle.intersects_circle(tank->get_position(), tank->get_collision_radius()))
            {
                if (tank->hit(particle_beam.damage))
                {
                    smokes.push_back(Smoke(smoke, tank->position - vec2(0, 48)));
                }
            }
        }
    }

    //Update explosion sprites and remove when done with remove erase idiom
    for (Explosion& explosion : explosions)
    {
        explosion.tick();
    }
    explosions.erase(std::remove_if(explosions.begin(), explosions.end(), [](const Explosion& explosion) { return explosion.done(); }), explosions.end());

    for (int i = active_tanks.size() - 1; i >= 0; i--)
    {
        if(!active_tanks.at(i)->active)
        {
            active_tanks.erase(active_tanks.begin() + i);
        }
    }

}


// -----------------------------------------------------------
// Draw all sprites to the screen
// (It is not recommended to multi-thread this function)
// -----------------------------------------------------------
void Game::draw()
{
    // clear the graphics window
    screen->clear(0);

    //Draw background
    background_terrain.draw(screen);

    //Draw sprites
    for (int i = 0; i < num_tanks_blue + num_tanks_red; i++)
    {
        tanks.at(i).draw(screen);

        vec2 tank_pos = tanks.at(i).get_position();
    }

    for (Rocket& rocket : rockets)
    {
        rocket.draw(screen);
    }

    for (Smoke& smoke : smokes)
    {
        smoke.draw(screen);
    }

    for (Particle_beam& particle_beam : particle_beams)
    {
        particle_beam.draw(screen);
    }

    for (Explosion& explosion : explosions)
    {
        explosion.draw(screen);
    }

    //Draw forcefield (mostly for debugging, its kinda ugly..)
    for (size_t i = 0; i < forcefield_hull.size(); i++)
    {
        vec2 line_start = forcefield_hull.at(i);
        vec2 line_end = forcefield_hull.at((i + 1) % forcefield_hull.size());
        line_start.x += HEALTHBAR_OFFSET;
        line_end.x += HEALTHBAR_OFFSET;
        screen->line(line_start, line_end, 0x0000ff);
    }

    //Draw sorted health bars
    for (int t = 0; t < 2; t++)
    {
        const int NUM_TANKS = ((t < 1) ? num_tanks_blue : num_tanks_red);

        const int begin = ((t < 1) ? 0 : num_tanks_blue);
        vector<const Tank*> sorted_tanks;
        insertion_sort_tanks_health(tanks, sorted_tanks, begin, begin + NUM_TANKS);
        sorted_tanks.erase(std::remove_if(sorted_tanks.begin(), sorted_tanks.end(), [](const Tank* tank) { return !tank->active; }), sorted_tanks.end());

        draw_health_bars(sorted_tanks, t);
    }
}

// -----------------------------------------------------------
// Sort tanks by health value using insertion sort
// -----------------------------------------------------------
void Tmpl8::Game::insertion_sort_tanks_health(const vector<Tank>& original, vector<const Tank*>& sorted_tanks, int begin, int end)
{
    const int NUM_TANKS = end - begin;
    sorted_tanks.reserve(NUM_TANKS);
    sorted_tanks.emplace_back(&original.at(begin));

    for (int i = begin + 1; i < (begin + NUM_TANKS); i++)
    {
        const Tank& current_tank = original.at(i);

        for (int s = (int)sorted_tanks.size() - 1; s >= 0; s--)
        {
            const Tank* current_checking_tank = sorted_tanks.at(s);

            if ((current_checking_tank->compare_health(current_tank) <= 0))
            {
                sorted_tanks.insert(1 + sorted_tanks.begin() + s, &current_tank);
                break;
            }

            if (s == 0)
            {
                sorted_tanks.insert(sorted_tanks.begin(), &current_tank);
                break;
            }
        }
    }
}

// -----------------------------------------------------------
// Draw the health bars based on the given tanks health values
// -----------------------------------------------------------
void Tmpl8::Game::draw_health_bars(const vector<const Tank*>& sorted_tanks, const int team)
{
    int health_bar_start_x = (team < 1) ? 0 : (SCRWIDTH - HEALTHBAR_OFFSET) - 1;
    int health_bar_end_x = (team < 1) ? health_bar_width : health_bar_start_x + health_bar_width - 1;

    for (int i = 0; i < SCRHEIGHT - 1; i++)
    {
        //Health bars are 1 pixel each
        int health_bar_start_y = i * 1;
        int health_bar_end_y = health_bar_start_y + 1;

        screen->bar(health_bar_start_x, health_bar_start_y, health_bar_end_x, health_bar_end_y, REDMASK);
    }

    //Draw the <SCRHEIGHT> least healthy tank health bars
    int draw_count = std::min(SCRHEIGHT, (int)sorted_tanks.size());
    for (int i = 0; i < draw_count - 1; i++)
    {
        //Health bars are 1 pixel each
        int health_bar_start_y = i * 1;
        int health_bar_end_y = health_bar_start_y + 1;

        float health_fraction = (1 - (sorted_tanks.at(i)->health /tank_max_health));

        if (team == 0) { screen->bar(health_bar_start_x + (health_bar_width * health_fraction), health_bar_start_y, health_bar_end_x, health_bar_end_y, GREENMASK); }
        else { screen->bar(health_bar_start_x, health_bar_start_y, health_bar_end_x - (health_bar_width * health_fraction), health_bar_end_y, GREENMASK); }
    }
}

// -----------------------------------------------------------
// When we reach max_frames print the duration and speedup multiplier
// Updating REF_PERFORMANCE at the top of this file with the value
// on your machine gives you an idea of the speedup your optimizations give
// -----------------------------------------------------------
void Tmpl8::Game::measure_performance()
{
    char buffer[128];
    if (frame_count >= max_frames)
    {
        if (!lock_update)
        {
            duration = perf_timer.elapsed();
            cout << "Duration was: " << duration << " (Replace REF_PERFORMANCE with this value)" << endl;
            lock_update = true;
        }

        frame_count--;
    }

    if (lock_update)
    {
        screen->bar(420 + HEALTHBAR_OFFSET, 170, 870 + HEALTHBAR_OFFSET, 430, 0x030000);
        int ms = (int)duration % 1000, sec = ((int)duration / 1000) % 60, min = ((int)duration / 60000);
        sprintf(buffer, "%02i:%02i:%03i", min, sec, ms);
        frame_count_font->centre(screen, buffer, 200);
        sprintf(buffer, "SPEEDUP: %4.1f", REF_PERFORMANCE / duration);
        frame_count_font->centre(screen, buffer, 340);
    }
}

// -----------------------------------------------------------
// Main application tick function
// -----------------------------------------------------------
void Game::tick(float deltaTime)
{
    if (!lock_update)
    {
        update(deltaTime);
    }
    draw();

    measure_performance();

    // print something in the graphics window
    //screen->Print("hello world", 2, 2, 0xffffff);

    // print something to the text window
    //cout << "This goes to the console window." << std::endl;

    //Print frame count
    frame_count++;
    string frame_count_string = "FRAME: " + std::to_string(frame_count);
    frame_count_font->print(screen, frame_count_string.c_str(), 350, 580);
}

vector<Tank*> Game::BottomTankSort(vector<Tank*> tanks) 
{
    if (tanks.size() == 1) { return tanks; }

    size_t const indexmiddle = (tanks.size() / 2);
    //MERGE SORT HIER

	vector<Tank*> split_l(tanks.begin(), (tanks.begin() + indexmiddle));
	vector<Tank*> split_r((tanks.begin() + indexmiddle), tanks.end());

    split_l = BottomTankSort(split_l);
    split_r = BottomTankSort(split_r);

    return BottomTankMerge(split_l, split_r);
}

vector<Tank*> Game::BottomTankMerge(vector<Tank*> l_tanks, vector<Tank*> r_tanks)
{
    vector<Tank*> Mergedtanks;
    while (l_tanks.size() > 0 && r_tanks.size() > 0) {
        if (l_tanks.at(0)->position.y < r_tanks.at(0)->position.y) {
            Mergedtanks.push_back(r_tanks.at(0));
            r_tanks.erase(r_tanks.begin());
        }
        else{
			Mergedtanks.push_back(l_tanks.at(0));
			l_tanks.erase(l_tanks.begin());
        }
    }
    while(l_tanks.size() > 0){
		Mergedtanks.push_back(l_tanks.at(0));
		l_tanks.erase(l_tanks.begin());
    }
    while (r_tanks.size() > 0) {
        Mergedtanks.push_back(r_tanks.at(0));
        r_tanks.erase(r_tanks.begin());
    }

    return Mergedtanks;
}

vector<Tank*> Game::AngleTankSort(vector<Tank*> tanks)
{
    if (tanks.size() == 1) { return tanks; }

    size_t const indexmiddle = (tanks.size() / 2);
    //MERGE SORT HIER

    vector<Tank*> split_l(tanks.begin(), (tanks.begin() + indexmiddle));
    vector<Tank*> split_r((tanks.begin() + indexmiddle), tanks.end());

    split_l = AngleTankSort(split_l);
    split_r = AngleTankSort(split_r);

    return AngleTankMerge(split_l, split_r);
}

vector<Tank*> Game::AngleTankMerge(vector<Tank*> l_tanks, vector<Tank*> r_tanks)
{

    int l_size = l_tanks.size();
    int r_size = r_tanks.size();
    vector<Tank*> Mergedtanks;
    while (l_size > 0 && r_size > 0) {

        if (AngleCalculator(l_tanks.at(0)->position) == AngleCalculator(r_tanks.at(0)->position)) {
            double a = distSqr(forcefield_hull.at(0), l_tanks.at(0)->position);
            double b = distSqr(forcefield_hull.at(0), r_tanks.at(0)->position);
            if (a > b) {
                r_tanks.erase(r_tanks.begin());
                r_size--;
            }
            else {
                l_tanks.erase(l_tanks.begin());
                l_size--;
            }
        }
        else if (AngleCalculator(l_tanks.at(0)->position) < AngleCalculator(r_tanks.at(0)->position)) {
            Mergedtanks.push_back(r_tanks.at(0));
            r_tanks.erase(r_tanks.begin());
            r_size--;
        }
        else {
            Mergedtanks.push_back(l_tanks.at(0));
            l_tanks.erase(l_tanks.begin());
            l_size--;
        }
    }
    while (l_size > 0) {
        Mergedtanks.push_back(l_tanks.at(0));
        l_tanks.erase(l_tanks.begin());
        l_size--;
    }
    while (r_tanks.size() > 0) {
        Mergedtanks.push_back(r_tanks.at(0));
        r_tanks.erase(r_tanks.begin());
        r_size--;
    }

    return Mergedtanks;
}

double Game::AngleCalculator(vec2 position){
    /*
    double dot = (forcefield_hull.at(0).x * position.x) + (forcefield_hull.at(0).y * position.y);
    double det = (forcefield_hull.at(0).x * position.x) - (forcefield_hull.at(0).y * position.y);
    return atan2(det, dot);
    */
   return atan2((position.y - forcefield_hull.at(0).y), (position.x - forcefield_hull.at(0).x)) * (180/PI);
}

int Game::Rotation(vec2 LastHull, vec2 pos2, vec2 pos3) 
{
    int m1 = (pos2.y - LastHull.y) / (pos2.x - LastHull.x);
    int m2 = (pos3.y - pos2.y) / (pos3.x - pos2.x);

    //counterclockwise
    if (m1 < m2) 
    {return 1;}
    //Clockwise
    else if (m1 > m2)
    {return -1;}
    //Straight
    else
    {return 0;}
}

double Game::distSqr(vec2 pos1, vec2 pos2)
{
    return ((pos1.x - pos2.x) * (pos1.x - pos2.x) + (pos1.y - pos2.y) * (pos1.y - pos2.y));
}


void Game::ConvexHull(vector<Tank*> LeftMergedTanks)
{
    int rotation;
    vector<Tank*> AngledSortedTanks = AngleTankSort(LeftMergedTanks);
    forcefield_hull.push_back(AngledSortedTanks.at(1)->position);

    //lower half
    for (int i = 2; i < AngledSortedTanks.size(); i++)
    {
        vec2 next = AngledSortedTanks.at(i)->position;
        vec2 p = forcefield_hull.back();
        forcefield_hull.pop_back();

        while (forcefield_hull.size() != 1 && Rotation(forcefield_hull.back(), p, AngledSortedTanks.at(i)->position) <= 0)
        {
            p = forcefield_hull.back();
            forcefield_hull.pop_back();
        }   
        forcefield_hull.push_back(p);
        forcefield_hull.push_back(AngledSortedTanks.at(i)->position);
    }

    vec2 p = forcefield_hull.back();
    forcefield_hull.pop_back();
    if (Rotation(forcefield_hull.back(), p, forcefield_hull.at(0)) > 0)
    {
        forcefield_hull.push_back(p);
    }

}