#pragma once

namespace Tmpl8
{
	//forward declarations
	class Tank;
	

	class Convexhull
	{
	public:
		Convexhull();

		void SetTankList(vector<Tank*> ActiveTanks);

		vector<vec2> ConvexHullcreate();

	private:
		
		vector<Tank*> BottomTankSort(vector<Tank*> tanks);
		vector<Tank*> BottomTankMerge(vector<Tank*> l_tanks, vector<Tank*> r_tanks);
		vector<Tank*> AngleTankSort(vector<Tank*> tanks);
		vector<Tank*> AngleTankMerge(vector<Tank*> l_tanks, vector<Tank*> r_tanks);
		int Rotation(vec2 LastHull, vec2 pos2, vec2 pos3);
		double distSqr(vec2 pos1, vec2 pos2);
		
		double AngleCalculator(vec2 position);

		vector<Tank*> BottomMergedTanks;
		
		vector<vec2> forcefield_hull;

		vec2 LowestPoint;
	};

}; // namespace Tmpl8