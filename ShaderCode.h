#ifndef SHADERCODE_H
#define SHADERCODE_H

#include <GL/glew.h>
#include "Timer.h"
#include "Matrix.h"
#include "Shader.h"
#include "TriangulationTable.h"


#define GLSL(src) "#version 330 core\n" #src

using namespace std;
using namespace MaliSDK;

extern GLfloat model_time;
extern const GLuint tesselation_level;
extern GLfloat isosurface_level;
extern unsigned int window_width;
extern unsigned int window_height;

extern const GLuint samples_per_axis;
extern const GLuint samples_in_3d_space;
extern const GLuint cells_per_axis;
extern const GLuint cells_in_3d_space;

extern Timer timer;

extern const int n_spheres;
extern const int n_sphere_position_components;
extern Matrix    mvp;

/* 1. Calculate sphere positions stage variable data. */

extern const GLchar* spheres_updater_vert_shader;
extern const GLchar* spheres_updater_frag_shader;
extern GLuint        spheres_updater_program_id;
extern GLuint        spheres_updater_vert_shader_id;
extern GLuint        spheres_updater_frag_shader_id;
extern GLuint        spheres_updater_sphere_positions_buffer_object_id;
extern GLuint        spheres_updater_transform_feedback_object_id;
extern const GLchar* spheres_updater_uniform_time_name;
extern GLuint        spheres_updater_uniform_time_id;
extern const GLchar* sphere_position_varying_name;
void SetUpSphereUpdater();
void DrawSphereUpdater();
void CleanUpSphereUpdater();


/* 2. Scalar field generation stage variable data. */

extern const GLchar* scalar_field_vert_shader;
extern const GLchar* scalar_field_frag_shader;
extern GLuint        scalar_field_program_id;
extern GLuint        scalar_field_vert_shader_id;
extern GLuint        scalar_field_frag_shader_id;
extern GLuint        scalar_field_buffer_object_id;
extern GLuint        scalar_field_transform_feedback_object_id;
extern const GLchar* scalar_field_uniform_samples_per_axis_name;
extern GLuint        scalar_field_uniform_samples_per_axis_id;
extern const GLchar* scalar_field_uniform_spheres_name;
extern GLuint        scalar_field_uniform_spheres_id;
extern const GLchar* scalar_field_value_varying_name;
extern GLuint        scalar_field_texture_object_id;
void SetUpScalarField();
void DrawScalarField();
void CleanUpScalarField();



/* 3. Marching Cubes cell-splitting stage variable data. */
extern const GLchar* marching_cubes_cells_vert_shader;
extern const GLchar* marching_cubes_cells_frag_shader;
extern GLuint        marching_cubes_cells_program_id;
extern GLuint        marching_cubes_cells_vert_shader_id;
extern GLuint        marching_cubes_cells_frag_shader_id;
extern const GLchar* marching_cubes_cells_uniform_cells_per_axis_name;
extern GLuint        marching_cubes_cells_uniform_cells_per_axis_id;
extern const GLchar* marching_cubes_cells_uniform_isolevel_name;
extern GLuint        marching_cubes_cells_uniform_isolevel_id;
extern const GLchar* marching_cubes_cells_uniform_scalar_field_sampler_name;
extern GLuint        marching_cubes_cells_uniform_scalar_field_sampler_id;
extern const GLchar* marching_cubes_cells_varying_name;
extern GLuint        marching_cubes_cells_transform_feedback_object_id;
extern GLuint        marching_cubes_cells_types_buffer_id;
extern GLuint        marching_cubes_cells_types_texture_object_id;
void SetUpMarchingCubesCells();
void DrawMarchingCubesCells();
void CleanUpMarchingCubesCells();



/* 4. Marching Cubes algorithm triangle generation and rendering stage variable data. */
extern const GLchar* marching_cubes_triangles_vert_shader;
extern const GLchar* marching_cubes_triangles_frag_shader;
extern GLuint        marching_cubes_triangles_program_id;
extern GLuint        marching_cubes_triangles_frag_shader_id;
extern GLuint        marching_cubes_triangles_vert_shader_id;
extern const GLchar* marching_cubes_triangles_uniform_samples_per_axis_name;
extern GLuint        marching_cubes_triangles_uniform_samples_per_axis_id;
extern const GLchar* marching_cubes_triangles_uniform_isolevel_name;
extern GLuint        marching_cubes_triangles_uniform_isolevel_id;
extern const GLchar* marching_cubes_triangles_uniform_time_name;
extern GLuint        marching_cubes_triangles_uniform_time_id;
extern const GLchar* marching_cubes_triangles_uniform_mvp_name;
extern GLuint        marching_cubes_triangles_uniform_mvp_id;
extern const GLchar* marching_cubes_triangles_uniform_cell_types_sampler_name;
extern GLuint        marching_cubes_triangles_uniform_cell_types_sampler_id;
extern const GLchar* marching_cubes_triangles_uniform_scalar_field_sampler_name;
extern GLuint        marching_cubes_triangles_uniform_scalar_field_sampler_id;
extern const GLchar* marching_cubes_triangles_uniform_sphere_positions_name;
extern GLuint        marching_cubes_triangles_uniform_sphere_positions_id;
extern const GLchar* marching_cubes_triangles_uniform_tri_table_sampler_name;
extern GLuint        marching_cubes_triangles_uniform_tri_table_sampler_id;
extern GLuint        marching_cubes_triangles_lookup_table_texture_id;
extern GLuint        marching_cubes_triangles_vao_id;
void SetUpMarchingCubesTriangles();
void DrawMarchingCubesTriangles();
void CleanUpMarchingCubesTriangles();


/*5. Marching cubes using geometry shaders*/

extern const GLchar* marching_cubes_geo_shader;
extern const GLchar* marching_cubes_vert_shader;
extern const GLchar* marching_cubes_frag_shader;
extern GLuint        marching_cubes_program_id;
extern GLuint        marching_cubes_geo_shader_id;
extern GLuint        marching_cubes_vert_shader_id;
extern GLuint        marching_cubes_frag_shader_id;
extern const GLchar* marching_cubes_uniform_time_name;
extern GLuint 		 marching_cubes_uniform_time_id;
extern const GLchar* marching_cubes_uniform_samples_per_axis_name;
extern GLuint 		 marching_cubes_uniform_samples_per_axis_id;
extern const GLchar* marching_cubes_uniform_iso_level_name;
extern GLuint 		 marching_cubes_uniform_iso_level_id;
extern const GLchar* marching_cubes_uniform_mvp_name;
extern GLuint 		 marching_cubes_uniform_mvp_id;
extern const GLchar* marching_cubes_uniform_scalar_field_sampler_name;
extern GLuint 		 marching_cubes_uniform_scalar_field_sampler_id;
extern const GLchar* marching_cubes_uniform_tri_table_sampler_name;
extern GLuint 		 marching_cubes_uniform_tri_table_sampler_id;
extern GLuint        marching_cubes_lookup_table_texture_id;
extern GLuint        marching_cubes_vao_id;
void SetUpMarchingCubes();
void DrawMarchingCubes();
void CleanUpMarchingCubes();


#endif //SHADERCODE_H