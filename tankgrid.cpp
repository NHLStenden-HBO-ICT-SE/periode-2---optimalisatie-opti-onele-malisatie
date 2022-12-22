#include "precomp.h" // include (only) this in every .cpp file
#include "tankgrid.h"
#include <vector>
#include <iterator>
#include <stdlib.h>

namespace Tmpl8 {

    TankGrid::TankGrid()
    {
        // Clear the grid.
        for (int x = 0; x < NUM_CELLSx; x++)
        {
            for (int y = 0; y < NUM_CELLSy; y++)
            {
                cells_[x][y] = nullptr;
            }
        }
    };

        void TankGrid::add(Tank* tank)
        {
            
            // Determine which grid cell it's in.
            int cellX = (int)(tank->position.x / TankGrid::CELL_SIZE);
            int cellY = (int)(tank->position.y / TankGrid::CELL_SIZE);
            // Add to the front of list for the cell it's in.
            tank->prev_ = nullptr;
            tank->next_ = cells_[cellX][cellY];
            cells_[cellX][cellY] = tank;
            if (tank->next_ != nullptr)
            {
                tank->next_->prev_ = tank;
            }
        };

        void TankGrid::move(Tank* tank, vec2 oldposition)
        {
            // See which cell it was in.
            int oldCellX = (int)(oldposition.x / TankGrid::CELL_SIZE);
            int oldCellY = (int)(oldposition.y / TankGrid::CELL_SIZE);
            // See which cell it's moving to.
            int cellX = (int)(tank->position.x / TankGrid::CELL_SIZE);
            int cellY = (int)(tank->position.y / TankGrid::CELL_SIZE);
            // If it didn't change cells, we're done.
            if (oldCellX == cellX && oldCellY == cellY) return;
            // Unlink it from the list of its old cell.
            if (tank->prev_ != nullptr)
            {
                tank->prev_->next_ = tank->next_;
            }
            if (tank->next_ != nullptr)
            {
                tank->next_->prev_ = tank->prev_;
            }
            // If it's the head of a list, remove it.
            if (cells_[oldCellX][oldCellY] == tank)
            {
                cells_[oldCellX][oldCellY] = tank->next_;
            }
            // Add it back to the grid at its new cell.
            add(tank);
        }

        void TankGrid::CheckCollision(Tank* tank) {
            int cellX = (int)(tank->position.x / TankGrid::CELL_SIZE);
            int cellY = (int)(tank->position.y / TankGrid::CELL_SIZE);
			
			
				// Handle other units in this cell.

            if (tank->next_ != nullptr)
            {
                Collision(tank, tank->next_);
            };
				// Also try the neighboring cells.  
				
				if (cellX > 0 && cellY > 0){Collision(tank, cells_[cellX - 1][cellY + 1]);}                     // < ^
				if (cellX > 0) Collision(tank, cells_[cellX - 1][cellY]);                                       // <
				if (cellX > 0 && cellY > 0) Collision(tank, cells_[cellX - 1][cellY - 1]);                      // < V
				if (cellY < NUM_CELLSy) Collision(tank, cells_[cellX][cellY + 1]);                              // ^
				if (cellY > 0) Collision(tank, cells_[cellX][cellY - 1]);                                       // V
                if (cellX < NUM_CELLSx && cellY > 0) { Collision(tank, cells_[cellX + 1][cellY + 1]); }         // > ^
				if (cellX < NUM_CELLSx) Collision(tank, cells_[cellX + 1][cellY]);                              // >
                if (cellX < NUM_CELLSx && cellY > NUM_CELLSy) Collision(tank, cells_[cellX + 1][cellY - 1]);    // > V  
				
        }

        void TankGrid::Collision(Tank* tank, Tank* other_tank) {

            Tank* other2_tank = other_tank;

        while(other2_tank != nullptr){
            if (other2_tank->active) {
                vec2 dir = tank->get_position() - other2_tank->get_position();
                if (dir != 0) {
                    float dir_squared_len = dir.sqr_length();

                    float col_squared_len = (tank->get_collision_radius() + other2_tank->get_collision_radius());
                    col_squared_len *= col_squared_len;

                    if (dir_squared_len < col_squared_len)
                    {
                        tank->push(dir.normalized(), 1.f);
                    }
                }
            }
			other2_tank = other2_tank->next_;
        }
        }

}