#include "ShaderCode.h"

GLfloat      model_time        = 0.0f;  /**< Time (in seconds), increased each rendering iteration.                                         */
const GLuint tesselation_level = 64;    /**< Level of details you would like to split model into. Please use values from th range [8..256]. */
GLfloat      isosurface_level  = 12.0f; /**< Scalar field's isosurface level.                                                               */
unsigned int window_width      = 256;   /**< Window width resolution (pixels).                                                              */
unsigned int window_height     = 256;   /**< Window height resolution (pixels).                                                             */

/* Marching Cubes algorithm-specific constants. */
const GLuint samples_per_axis      = tesselation_level;                                      /**< Amount of samples we break scalar space into (per each axis). */
const GLuint samples_in_3d_space   = samples_per_axis * samples_per_axis * samples_per_axis; /**< Amount of samples in 3D space. */
const GLuint cells_per_axis        = samples_per_axis - 1;                                   /**< Amount of cells per each axis. */
const GLuint cells_in_3d_space     = cells_per_axis * cells_per_axis * cells_per_axis;       /**< Amount of cells in 3D space. */

/** Instance of a timer to measure time moments. */
Timer timer;

/** Amount of spheres defining scalar field. This value should be synchronized between all files. */
const int n_spheres = 3;

/** Amount of components in sphere position varying. */
const int n_sphere_position_components = 4;

/** Matrix that transforms vertices from model space to perspective projected world space. */
Matrix        mvp;