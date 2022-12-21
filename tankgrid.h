#pragma once
namespace Tmpl8
{

	class TankGrid
	{
	public:
		TankGrid();
		void add(Tank* tank);
		void move(Tank* tank, vec2 oldposition);

		//1420/720
        static const int CELL_SIZE = 72;
		static const int NUM_CELLSx = 20;
		static const int NUM_CELLSy = 10;
		static const int NUM_CELLStotal = NUM_CELLSx * NUM_CELLSy;
	private:
        Tank* cells_[NUM_CELLSx][NUM_CELLSy];
	};
};