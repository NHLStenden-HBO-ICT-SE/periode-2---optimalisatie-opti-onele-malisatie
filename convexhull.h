#pragma once

namespace Tmpl8
{
	//forward declarations
	class Tank;
	

	class Convexhull
	{
	public:
		Convexhull();

		void set_tanklist(vector<Tank*> ActiveTanks);

		vector<vec2> convexhullcreate();

	private:
		
		vector<Tank*> bottom_tanksort(vector<Tank*> tanks);
		vector<Tank*> bottom_tankmerge(vector<Tank*> l_tanks, vector<Tank*> r_tanks);
		vector<Tank*> angle_tanksort(vector<Tank*> tanks);
		vector<Tank*> angle_tankmerge(vector<Tank*> l_tanks, vector<Tank*> r_tanks);
		int rotation(vec2 LastHull, vec2 pos2, vec2 pos3);
		double distsqr(vec2 pos1, vec2 pos2);
		
		double anglecalculator(vec2 position);

		vector<Tank*> BottomMergedTanks;
		
		vector<vec2> forcefield_hull;

		vec2 LowestPoint;
	};

}; // namespace Tmpl8