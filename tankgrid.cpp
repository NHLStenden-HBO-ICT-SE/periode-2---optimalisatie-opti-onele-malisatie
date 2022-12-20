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
            tank->prev_ = NULL;
            tank->next_ = cells_[cellX][cellY];
            cells_[cellX][cellY] = tank;
            if (tank->next_ != NULL)
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
            if (tank->prev_ != NULL)
            {
                tank->prev_->next_ = tank->next_;
            }
            if (tank->next_ != NULL)
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


}