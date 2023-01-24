#include "precomp.h" // include (only) this in every .cpp file
#include "convexhull.h"
#include <vector>
#include <iterator>

namespace Tmpl8 {

    Convexhull::Convexhull() {};

    void Convexhull::set_tanklist(vector<Tank*> ActiveTanks ) {
        BottomMergedTanks = bottom_tanksort(ActiveTanks);
    }

    vector<Tank*> Convexhull::bottom_tanksort(vector<Tank*> tanks)
    {
        //Merge sort lowest tank
        if (tanks.size() == 1) { return tanks; }

        size_t const indexmiddle = (tanks.size() / 2);

        vector<Tank*> split_l(tanks.begin(), (tanks.begin() + indexmiddle));
        vector<Tank*> split_r((tanks.begin() + indexmiddle), tanks.end());

        split_l = bottom_tanksort(split_l);
        split_r = bottom_tanksort(split_r);

        return bottom_tankmerge(split_l, split_r);
    }

    vector<Tank*> Convexhull::bottom_tankmerge(vector<Tank*> l_tanks, vector<Tank*> r_tanks)
    {
        vector<Tank*> Mergedtanks;
        while (l_tanks.size() > 0 && r_tanks.size() > 0) {
            if (l_tanks.at(0)->position.y < r_tanks.at(0)->position.y) {
                Mergedtanks.push_back(r_tanks.at(0));
                r_tanks.erase(r_tanks.begin());
            }
            else if (l_tanks.at(0)->position.y == r_tanks.at(0)->position.y)
            {
                if (l_tanks.at(0)->position.x > r_tanks.at(0)->position.x)
                {
                    Mergedtanks.push_back(r_tanks.at(0));
                    r_tanks.erase(r_tanks.begin());
                }
                else
                {
                    Mergedtanks.push_back(l_tanks.at(0));
                    l_tanks.erase(l_tanks.begin());
                }
            }
            else {
                Mergedtanks.push_back(l_tanks.at(0));
                l_tanks.erase(l_tanks.begin());
            }
        }
        while (l_tanks.size() > 0) {
            Mergedtanks.push_back(l_tanks.at(0));
            l_tanks.erase(l_tanks.begin());
        }
        while (r_tanks.size() > 0) {
            Mergedtanks.push_back(r_tanks.at(0));
            r_tanks.erase(r_tanks.begin());
        }

        return Mergedtanks;
    }

    vector<Tank*> Convexhull::angle_tanksort(vector<Tank*> tanks)
    {
        //Sorts the tanks on polar axis, using the lowest tank as reference.
        if (tanks.size() == 1) { return tanks; }

        size_t const indexmiddle = (tanks.size() / 2);

        vector<Tank*> split_l(tanks.begin(), (tanks.begin() + indexmiddle));
        vector<Tank*> split_r((tanks.begin() + indexmiddle), tanks.end());

        split_l = angle_tanksort(split_l);
        split_r = angle_tanksort(split_r);

        return angle_tankmerge(split_l, split_r);
    }

    vector<Tank*> Convexhull::angle_tankmerge(vector<Tank*> l_tanks, vector<Tank*> r_tanks)
    {

        int l_size = l_tanks.size();
        int r_size = r_tanks.size();
        vector<Tank*> Mergedtanks;
        while (l_size > 0 && r_size > 0) {

            if (anglecalculator(l_tanks.at(0)->position) == anglecalculator(r_tanks.at(0)->position)) {
                double a = distsqr(LowestPoint, l_tanks.at(0)->position);
                double b = distsqr(LowestPoint, r_tanks.at(0)->position);
                if (a > b) {
                    r_tanks.erase(r_tanks.begin());
                    r_size--;
                }
                else {
                    l_tanks.erase(l_tanks.begin());
                    l_size--;
                }
            }
            else if (anglecalculator(l_tanks.at(0)->position) > anglecalculator(r_tanks.at(0)->position)) {
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

    double Convexhull::anglecalculator(vec2 position) {
        return (atan2((LowestPoint.y - position.y), (position.x - LowestPoint.x)) * (180 / PI));
    }

    int Convexhull::rotation(vec2 pos1, vec2 pos2, vec2 pos3)
    {
        double IsLeftTurn = ((pos2.x - pos1.x) * (pos3.y - pos1.y)) - ((pos2.y - pos1.y) * (pos3.x - pos1.x));
        // If 1 = Left, if -1 = Right, if 0 = Colinair
        if (IsLeftTurn == 0) return 0;
        return (IsLeftTurn > 0) ? 1 : -1;
    }

    double Convexhull::distsqr(vec2 pos1, vec2 pos2)
    {
        return ((pos1.x - pos2.x) * (pos1.x - pos2.x) + (pos1.y - pos2.y) * (pos1.y - pos2.y));
    }

    vector<vec2> Convexhull::convexhullcreate()
    {
        LowestPoint = BottomMergedTanks.at(0)->position;
        Tank* LowestTank = BottomMergedTanks.at(0);
        BottomMergedTanks.erase(BottomMergedTanks.begin());
        vector<Tank*> AngledSortedTanks = angle_tanksort(BottomMergedTanks);
        AngledSortedTanks.push_back(LowestTank);

        forcefield_hull.push_back(AngledSortedTanks.at(AngledSortedTanks.size() - 2)->position);
        forcefield_hull.push_back(LowestPoint);

        for (int i = 0; i < AngledSortedTanks.size(); i++)
        {
            while (rotation(forcefield_hull.at(forcefield_hull.size() - 2), forcefield_hull.at(forcefield_hull.size() - 1), AngledSortedTanks.at(i)->position) > 0)
            {
                forcefield_hull.pop_back();
            }

            forcefield_hull.push_back(AngledSortedTanks.at(i)->position);
        }

        forcefield_hull.pop_back();

        return forcefield_hull;
    }
}