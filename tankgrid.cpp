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
                cells_[x][y] = NULL;
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
    //Scans through the cells around the tank in a 3 by 3 grid.
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                if (cells_[cellX - 1 + i][cellY - 1 + j] != cells_[cellX][cellY])
                {
                    Collision(tank, cells_[cellX - 1 + i][cellY - 1 + j]);
                }
                else {
                    Collision(tank, tank->next_);
                }
            }
        }
    }


    void TankGrid::Collision(Tank* tank, Tank* other_tank) {

        //Tank* other_tank = other_tank;

        while (other_tank != nullptr) {
            if (other_tank->active) {
                vec2 dir = tank->get_position() - other_tank->get_position();
                if (dir != 0) {
                    float dir_squared_len = dir.sqr_length();

                    float col_squared_len = (tank->get_collision_radius() + other_tank->get_collision_radius());
                    col_squared_len *= col_squared_len;

                    if (dir_squared_len < col_squared_len)
                    {
                        tank->push(dir.normalized(), 1.f);
                    }
                }
            }
        }
        other_tank = other_tank->next_;
    }

            vector<Tank*> TankGrid::RocketCheckCollision(Rocket * rocket) {
                int cellRocketX = (int)(rocket->position.x / TankGrid::CELL_SIZE);
                int cellRocketY = (int)(rocket->position.y / TankGrid::CELL_SIZE);
                vector<Tank*> TanksInArea;
                // Handle other units in this cell.
            //Scans through the cells around the tank in a 3 by 3 grid.
                for (int i = 0; i < 3; i++)
                {
                    for (int j = 0; j < 3; j++)
                    {
                        
                        Tank* tank = cells_[cellRocketX - 1 + i][cellRocketY - 1 + j];
                            while(tank != nullptr){
                                if (tank->active) {
                                    TanksInArea.push_back(tank);
                                }
                            tank = tank->next_;
                            }
                    }
                }
                return TanksInArea;
            }
            
}