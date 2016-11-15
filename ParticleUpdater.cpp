#include "ShaderCode.h"

const int n_particles               = 5000;
const int space_cells_per_axis      = 16;
const int space_cells_in_3d_space   = space_cells_per_axis*space_cells_per_axis*space_cells_per_axis;

GLfloat position[3*n_particles];
vector<int> cells[space_cells_in_3d_space];
GLint ordererd_index[n_particles];
GLint space_distribution_index[space_cells_in_3d_space];

void ReorderPerticlesInCells(){
    
    for(int i = 0; i != space_cells_in_3d_space; i++){
        cells[i].clear();
    }

    for(int i = 0; i != n_particles; i++){
        float x = position[3*i];
        float y = position[3*i];
        float z = position[3*i];
        int index = 0;
        index += int(z*float(space_cells_per_axis));
        index *= space_cells_per_axis;
        index += int(y*float(space_cells_per_axis));
        index *= space_cells_per_axis;
        index += int(x*float(space_cells_per_axis));
        cells[index].push_back(i);
    }
    int start_index = 0;
    for(int i = 0; i != space_cells_in_3d_space; i++){
        space_distribution_index[i] = start_index;
        for(int j=0;j!=cells[i].size();j++){
            ordererd_index[start_index] = cells[i][j];
            start_index++;
        }
    }
}