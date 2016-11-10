//
// Created by Rohan Das on 10/29/2016.
//
/**
 * In this vertex shader we calculate the sphere positions according to the specified time moment.
 * The shader contains an initial sphere positions and data required to calculate the sphere positions.
 * Each shader instance updates one sphere position, which is returned in the sphere_position output variable.
 */
#ifndef MARCHINGCUBES_SHADERCODE_H
#define MARCHINGCUBES_SHADERCODE_H


const char* spheres_updater_vert_shader          = "#version 300 es\n"
"\n"
"/** Structure that describes parameters of a single sphere moving across the scalar field. */\n"
"struct sphere_descriptor\n"
"{\n"
"    /* Coefficients for Lissajou equations. Current coordinates calculated by formula:\n"
"     * v(t) = start_center + lissajou_amplitude * sin(lissajou_frequency * t + lissajou_phase) */\n"
"    vec3  start_center;        /* Center in space around which sphere moves.  */\n"
"    vec3  lissajou_amplitude;  /* Lissajou equation amplitudes for all axes.  */\n"
"    vec3  lissajou_frequency;  /* Lissajou equation frequencies for all axes. */\n"
"    vec3  lissajou_phase;      /* Lissajou equation phases for all axes.      */\n"
"    /* Other sphere parameters. */\n"
"    float size;                /* Size of a sphere (weight or charge).        */\n"
"};\n"
"\n"
"/* [Stage 1 Uniforms] */\n"
"/** Current time moment. */\n"
"uniform float time;\n"
"/* [Stage 1 Uniforms] */\n"
"\n"
"/* [Stage 1 Output data] */\n"
"/** Calculated sphere positions. */\n"
"out vec4 sphere_position;\n"
"/* [Stage 1 Output data] */\n"
"\n"
"/** Shader entry point. */\n"
"void main()\n"
"{\n"
"    /* Stores information on spheres moving across the scalar field. Specified in model coordinates (range 0..1]) */\n"
"    sphere_descriptor spheres[] = sphere_descriptor[]\n"
"    (\n"
"        /*                      (---- center ----)      (--- amplitude --)      (--- frequency ---)      (----- phase -----) (weight)*/\n"
"        sphere_descriptor(  vec3(0.50, 0.50, 0.50), vec3(0.20, 0.25, 0.25), vec3( 11.0, 21.0, 31.0), vec3( 30.0, 45.0, 90.0),  0.100),\n"
"        sphere_descriptor(  vec3(0.50, 0.50, 0.50), vec3(0.25, 0.20, 0.25), vec3( 22.0, 32.0, 12.0), vec3( 45.0, 90.0,120.0),  0.050),\n"
"        sphere_descriptor(  vec3(0.50, 0.50, 0.50), vec3(0.25, 0.25, 0.20), vec3( 33.0, 13.0, 23.0), vec3( 90.0,120.0,150.0),  0.250)\n"
"    );\n"
"\n"
"    /* Calculate new xyz coordinates of the sphere. */\n"
"    vec3 sphere_position3 = spheres[gl_VertexID].start_center\n"
"                          + spheres[gl_VertexID].lissajou_amplitude\n"
"                          * sin(radians(spheres[gl_VertexID].lissajou_frequency) * time + radians(spheres[gl_VertexID].lissajou_phase));\n"
"\n"
"    /* Update sphere position coordinates. w-coordinte represents sphere weight. */\n"
"    sphere_position = vec4(sphere_position3, spheres[gl_VertexID].size);\n"
"}\n";

/**
 *  Dummy fragment shader for a program object to successfully link.
 *  Fragment shader is not used in this stage, but needed for a program object to successfully link.
 */
const char* spheres_updater_frag_shader          = "#version 300 es\n"
"\n"
"/** Shader entry point. */\n"
"void main()\n"
"{\n"
"}\n";

/**
 * This vertex shader calculates a scalar field and stores it in the scalar_field_value output variable.
 * As input data we use sphere positions calculated in a previous stage and passed into the shader as
 * a uniform block.
 */
const char* scalar_field_vert_shader             = "#version 300 es\n"
"\n"
"/** Precision to avoid division-by-zero errors. */\n"
"#define EPSILON 0.000001f\n"
"\n"
"/** Amount of spheres defining scalar field. This value should be synchronized between all files. */\n"
"#define N_SPHERES 3\n"
"\n"
"/* [Stage 2 Uniforms] */\n"
"/* Uniforms: */\n"
"/** Amount of samples taken for each axis of a scalar field; */\n"
"uniform int samples_per_axis;\n"
"\n"
"/** Uniform block encapsulating sphere locations. */\n"
"uniform spheres_uniform_block\n"
"{\n"
"    vec4 input_spheres[N_SPHERES];\n"
"};\n"
"/* [Stage 2 Uniforms] */\n"
"\n"
"/* [Stage 2 Output data] */\n"
"/* Output data: */\n"
"/** Calculated scalar field value. */\n"
"out float scalar_field_value;\n"
"/* [Stage 2 Output data] */\n"
"\n"
"/* [Stage 2 decode_space_position] */\n"
"/** Decode coordinates in space from vertex number.\n"
" *  Assume 3D space of samples_per_axis length for each axis and following encoding:\n"
" *  encoded_position = x + y * samples_per_axis + z * samples_per_axis * samples_per_axis\n"
" *\n"
" *  @param  vertex_index Encoded vertex position\n"
" *  @return              Coordinates of a vertex in space ranged [0 .. samples_per_axis-1]\n"
" */\n"
"ivec3 decode_space_position(in int vertex_index)\n"
"{\n"
"    int   encoded_position = vertex_index;\n"
"    ivec3 space_position;\n"
"\n"
"    /* Calculate coordinates from vertex number. */\n"
"    space_position.x = encoded_position % samples_per_axis;\n"
"    encoded_position = encoded_position / samples_per_axis;\n"
"\n"
"    space_position.y = encoded_position % samples_per_axis;\n"
"    encoded_position = encoded_position / samples_per_axis;\n"
"\n"
"    space_position.z = encoded_position;\n"
"\n"
"    return space_position;\n"
"}\n"
"/* [Stage 2 decode_space_position] */\n"
"\n"
"/** Normalizes each coordinate interpolating input coordinates\n"
" *  from range [0 .. samples_per_axis-1] to [0.0 .. 1.0] range.\n"
" *\n"
" *  @param  space_position Coordinates in range [0 .. samples_per_axis-1]\n"
" *  @return Coordinates in range [0.0 .. 1.0]\n"
" */\n"
"/* [Stage 2 normalize_space_position_coordinates] */\n"
"vec3 normalize_space_position_coordinates(in ivec3 space_position)\n"
"{\n"
"    vec3 normalized_space_position = vec3(space_position) / float(samples_per_axis - 1);\n"
"\n"
"    return normalized_space_position;\n"
"}\n"
"/* [Stage 2 normalize_space_position_coordinates] */\n"
"\n"
"/** Calculates scalar field at user-defined location.\n"
" *\n"
" *  @param position Space position for which scalar field value is calculated\n"
" *  @return         Scalar field value\n"
" */\n"
"/* [Stage 2 calculate_scalar_field_value] */\n"
"float calculate_scalar_field_value(in vec3 position)\n"
"{\n"
"    float field_value = 0.0f;\n"
"\n"
"    /* Field value in given space position influenced by all spheres. */\n"
"    for (int i = 0; i < N_SPHERES; i++)\n"
"    {\n"
"        vec3  sphere_position         = input_spheres[i].xyz;\n"
"        float vertex_sphere_distance  = length(distance(sphere_position, position));\n"
"\n"
"        /* Field value is a sum of all spheres fields in a given space position.\n"
"         * Sphere weight (or charge) is stored in w-coordinate.\n"
"         */\n"
"        field_value += input_spheres[i].w / pow(max(EPSILON, vertex_sphere_distance), 2.0);\n"
"    }\n"
"\n"
"    return field_value;\n"
"}\n"
"/* [Stage 2 calculate_scalar_field_value] */\n"
"\n"
"/** Shader entry point. */\n"
"void main()\n"
"{\n"
"    /* Decode point space position defined by gl_VertexID. */\n"
"    ivec3 space_position      = decode_space_position(gl_VertexID);\n"
"\n"
"    /* Normalize point space position. */\n"
"    vec3  normalized_position = normalize_space_position_coordinates(space_position);\n"
"\n"
"    /* Calculate field value and assign field value to output variable. */\n"
"    scalar_field_value = calculate_scalar_field_value(normalized_position);\n"
"}\n";

/**
 *  Dummy fragment shader for a program object to successfully link.
 *  Fragment shader is not used in this stage, but needed for a program object to successfully link.
 */
const char* scalar_field_frag_shader             = "#version 300 es\n"
"\n"
"/** Shader entry point. */\n"
"void main()\n"
"{\n"
"}\n";

/**
 * The Marching Cube algorithm cell splitting stage vertex shader.
 *
 * In this vertex shader we analyse the isosurface in each cell of space and
 * assign one of 256 possible types to each cell. The cell type data
 * for each cell is returned in cell_type_index output variable.
 */
const char* marching_cubes_cells_vert_shader     = "#version 300 es\n"
"\n"
"/** Specify low precision for sampler3D type. */\n"
"precision lowp sampler3D;\n"
"\n"
"/* Uniforms: */\n"
"/** Scalar field is stored in a 3D texture. */\n"
"uniform sampler3D scalar_field;\n"
"\n"
"/** Amount of samples taken for each axis of a scalar field. */\n"
"uniform int cells_per_axis;\n"
"\n"
"/** Isosurface level. */\n"
"uniform float iso_level;\n"
"\n"
"/* Output data: */\n"
"/** Cell type index. */\n"
"flat out int cell_type_index;\n"
"\n"
"/** Calculates cell type index for provided cell and isosurface level.\n"
" *\n"
" *  @param cell_corner_field_value Scalar field values in cell corners\n"
" *  @param isolevel                Scalar field value which defines isosurface level\n"
" */\n"
"/* [Stage 3 get_cell_type_index] */\n"
"int get_cell_type_index(in float cell_corner_field_value[8], in float isolevel)\n"
"{\n"
"    int cell_type_index = 0;\n"
"\n"
"    /* Iterate through all cell corners. */\n"
"    for (int i = 0; i < 8; i++)\n"
"    {\n"
"        /* If corner is inside isosurface then set bit in cell type index index. */\n"
"        if (cell_corner_field_value[i] < isolevel)\n"
"        {\n"
"            /* Set appropriate corner bit in cell type index. */\n"
"            cell_type_index |= (1<<i);\n"
"        }\n"
"    }\n"
"\n"
"    return cell_type_index;\n"
"}\n"
"/* [Stage 3 get_cell_type_index] */\n"
"\n"
"/** Decode coordinates in space from cell number.\n"
" *  Assume cubical space of cells_per_axis cells length by each axis and following encoding:\n"
" *  encoded_position = x + y * cells_per_axis + z * cells_per_axis * cells_per_axis\n"
" *\n"
" *  @param  cell_index Encoded cell position\n"
" *  @return            Coordinates of a cell in space ranged [0 .. cells_per_axis-1]\n"
" */\n"
"/* [Stage 3 decode_space_position] */\n"
"ivec3 decode_space_position(in int cell_index)\n"
"{\n"
"    ivec3 space_position;\n"
"    int   encoded_position = cell_index;\n"
"\n"
"    /* Calculate coordinates from encoded position */\n"
"    space_position.x       = encoded_position % cells_per_axis;\n"
"    encoded_position       = encoded_position / cells_per_axis;\n"
"\n"
"    space_position.y       = encoded_position % cells_per_axis;\n"
"    encoded_position       = encoded_position / cells_per_axis;\n"
"\n"
"    space_position.z       = encoded_position;\n"
"\n"
"    return space_position;\n"
"}\n"
"/* [Stage 3 decode_space_position] */\n"
"\n"
"/** Shader entry point. */\n"
"void main()\n"
"{\n"
"    /* Cubic cell has exactly 8 corners. */\n"
"    const int corners_in_cell = 8;\n"
"\n"
"    /* Cell corners in space relatively to cell's base point [0]. */\n"
"    const ivec3 cell_corners_offsets[corners_in_cell] = ivec3[]\n"
"    (\n"
"        ivec3(0, 0, 0),\n"
"        ivec3(1, 0, 0),\n"
"        ivec3(1, 0, 1),\n"
"        ivec3(0, 0, 1),\n"
"        ivec3(0, 1, 0),\n"
"        ivec3(1, 1, 0),\n"
"        ivec3(1, 1, 1),\n"
"        ivec3(0, 1, 1)\n"
"    );\n"
"\n"
"    /* Scalar field texture size, used for normalization purposes. */\n"
"    vec3 scalar_field_normalizers = vec3(textureSize(scalar_field, 0)) - vec3(1, 1, 1);\n"
"\n"
"    /* Scalar field value in corners. Corners numbered according to Marching Cubes algorithm. */\n"
"    float scalar_field_in_cell_corners[8];\n"
"\n"
"    /* Find cell position processed by this shader instance (defined by gl_VertexID). */\n"
"    ivec3 space_position = decode_space_position(gl_VertexID);\n"
"\n"
"    /* [Stage 3 Gather values for the current cell] */\n"
"    /* Find scalar field values in cell corners. */\n"
"    for (int i = 0; i < corners_in_cell; i++)\n"
"    {\n"
"        /* Calculate cell corner processed at this iteration. */\n"
"        ivec3 cell_corner = space_position + cell_corners_offsets[i];\n"
"\n"
"        /* Calculate cell corner's actual position ([0.0 .. 1.0] range.) */\n"
"        vec3 normalized_cell_corner  = vec3(cell_corner) / scalar_field_normalizers;\n"
"\n"
"        /* Get scalar field value in cell corner from scalar field texture. */\n"
"        scalar_field_in_cell_corners[i] = textureLod(scalar_field, normalized_cell_corner, 0.0).r;\n"
"    }\n"
"    /* [Stage 3 Gather values for the current cell] */\n"
"\n"
"    /* Get cube type index. */\n"
"    cell_type_index = get_cell_type_index(scalar_field_in_cell_corners, iso_level);\n"
"}\n";

/**
 *  Dummy fragment shader for a program object to successfully link.
 *  Fragment shader is not used in this stage, but needed for a program object to successfully link.
 */
const char* marching_cubes_cells_frag_shader     = "#version 300 es\n"
"\n"
"/** Shader entry point. */\n"
"void main()\n"
"{\n"
"}\n";

/**
 * The vertex shader generates a set of triangles for each cell appropriate for the cell type.
 *
 * In this shader we generate exactly (3 vertices * 5 triangles per cell *
 * amount of cells the scalar field is split to) triangle vertices.
 * A one shader instance processes only one triangle vertex.
 * Due to requirement for a vertex shader instance to issue a vertex,
 * it issues a vertex in any case, including a dummy triangles, but
 * the dummy triangles has all vertices set to point O and will not be rendered.
 */
const char* marching_cubes_triangles_vert_shader = "#version 300 es\n"
"\n"
"precision highp isampler2D; /**< Specify high precision for isampler2D type. */\n"
"precision highp isampler3D; /**< Specify high precision for isampler3D type. */\n"
"precision highp sampler2D;  /**< Specify high precision for sampler2D type. */\n"
"precision highp sampler3D;  /**< Specify high precision for sampler3D type. */\n"
"\n"
"/** Precision to avoid division-by-zero errors. */\n"
"#define EPSILON 0.000001f\n"
"\n"
"/** Amount of cells taken for each axis of a scalar field. */\n"
"#define CELLS_PER_AXIS (samples_per_axis - 1)\n"
"\n"
"/** Maximum amount of vertices a single cell can define. */\n"
"const int mc_vertices_per_cell = 15;\n"
"\n"
"/* Uniforms: */\n"
"/** Amount of samples taken for each axis of a scalar field. */\n"
"uniform int samples_per_axis;\n"
"\n"
"/** A signed integer 3D texture is used to deliver cell type data. */\n"
"uniform isampler3D cell_types;\n"
"\n"
"/** A 3D texture is used to deliver scalar field data. */\n"
"uniform sampler3D scalar_field;\n"
"\n"
"/** A 2D texture representing tri_table lookup array. Array contains edge numbers (in sense of Marching Cubes algorithm).\n"
"    As input parameters (indices to texture) should be specified cell type index and combined vertex-triangle number. */\n"
"uniform isampler2D tri_table;\n"
"\n"
"/** Combined model view and projection matrices. */\n"
"uniform mat4 mvp;\n"
"\n"
"/** Isosurface level. */\n"
"uniform float iso_level;\n"
"\n"
"/* Phong shading output variables for fragment shader. */\n"
"out vec4 phong_vertex_position;      /**< position of the vertex in world space.  */\n"
"out vec3 phong_vertex_normal_vector; /**< surface normal vector in world space.   */\n"
"out vec3 phong_vertex_color;         /**< vertex color for fragment colorisation. */\n"
"\n"
"\n"
"/** Function approximates scalar field derivative along begin_vertex<->end_vertex axis.\n"
" *  Field derivative calculated as a scalar field difference between specified vertices\n"
" *  divided by distance between vertices.\n"
" *\n"
" *  @param begin_vertex begin vertex\n"
" *  @param end_vertex   end vertex\n"
" *  @return             scalar field derivative along begin_vertex<->end_vertex axis\n"
" */\n"
"float calc_partial_derivative(vec3 begin_vertex, vec3 end_vertex)\n"
"{\n"
"    float field_value_begin = textureLod(scalar_field, begin_vertex, 0.0).r;\n"
"    float field_value_end   = textureLod(scalar_field, end_vertex,   0.0).r;\n"
"\n"
"    return (field_value_end - field_value_begin) / distance(begin_vertex, end_vertex);\n"
"}\n"
"\n"
"/** Finds normal in given cell corner vertex. Normal calculated as a vec3(dF/dx, dF/dy, dF/dz)\n"
" *  dFs are calculated as difference of scalar field values in corners of this or adjacent cells.\n"
" *\n"
" *  @param p1 vertex for which normal is to be calculated\n"
" *  @return   normal vector to surface in p1\n"
" */\n"
"vec3 calc_cell_corner_normal(in vec3 p1)\n"
"{\n"
"    vec3 result;\n"
"    vec3 delta;\n"
"\n"
"    /* Use neighbour samples to calculate derivative. */\n"
"    delta = vec3(1.0/float(samples_per_axis - 1), 0, 0);\n"
"    result.x = calc_partial_derivative(p1 - delta, p1 + delta);\n"
"\n"
"    delta = vec3(0.0, 1.0/float(samples_per_axis - 1), 0.0);\n"
"    result.y = calc_partial_derivative(p1 - delta, p1 + delta);\n"
"\n"
"    delta = vec3(0.0, 0.0, 1.0/float(samples_per_axis - 1));\n"
"    result.z = calc_partial_derivative(p1 - delta, p1 + delta);\n"
"\n"
"    return result;\n"
"}\n"
"\n"
"/** Calculates normal for an edge vertex like in an orignal SIGGRAPH paper.\n"
" *  First finds normal vectors in edge begin vertex and in edge end vertex, then interpolate.\n"
" *\n"
" *  @param start_vertex_portion influence of edge_start vertex\n"
" *  @param edge_start           normalized coordinates of edge start vertex\n"
" *  @param edge_end             normalized coordinates of edge end vertex\n"
" *  @return                     normal to surface vector in edge position specified\n"
" */\n"
"vec3 calc_phong_normal(in float start_vertex_portion, in vec3 edge_start, in vec3 edge_end)\n"
"{\n"
"    /* Find normal vector in begin vertex of the edge. */\n"
"    vec3 edge_start_normal = calc_cell_corner_normal(edge_start);\n"
"    /* Find normal vector in end vertex of the edge. */\n"
"    vec3 edge_end_normal   = calc_cell_corner_normal(edge_end);\n"
"\n"
"    /* Interpolate normal vector. */\n"
"    return mix(edge_end_normal, edge_start_normal, start_vertex_portion);\n"
"}\n"
"\n"
"/** Decodes cell coordinates from vertex identifier.\n"
" *  Assumes 3D space of CELLS_PER_AXIS cells for each axis and\n"
" *  mc_vertices_per_cell triangles-generating vertices per cell\n"
" *  encoded in vertex identifier according to following formula:\n"
" *    encoded_position = mc_vertex_no + mc_vertices_per_cell * (x + CELLS_PER_AXIS * (y + CELLS_PER_AXIS * z))\n"
" *\n"
" *  @param  encoded_position_argument encoded position\n"
" *  @return                           cell coordinates ranged [0 .. CELLS_PER_AXIS-1] in x,y,z, and decoded vertex number in w.\n"
" */\n"
"/* [Stage 4 decode_cell_position] */\n"
"ivec4 decode_cell_position(in int encoded_position_argument)\n"
"{\n"
"    ivec4 cell_position;\n"
"    int   encoded_position = encoded_position_argument;\n"
"\n"
"    /* Decode combined triangle and vertex number. */\n"
"    cell_position.w  = encoded_position % mc_vertices_per_cell;\n"
"    encoded_position = encoded_position / mc_vertices_per_cell;\n"
"\n"
"    /* Decode coordinates from encoded position. */\n"
"    cell_position.x  = encoded_position % CELLS_PER_AXIS;\n"
"    encoded_position = encoded_position / CELLS_PER_AXIS;\n"
"\n"
"    cell_position.y  = encoded_position % CELLS_PER_AXIS;\n"
"    encoded_position = encoded_position / CELLS_PER_AXIS;\n"
"\n"
"    cell_position.z  = encoded_position;\n"
"\n"
"    return cell_position;\n"
"}\n"
"/* [Stage 4 decode_cell_position] */\n"
"\n"
"/** Identifies cell type for provided cell position.\n"
" *\n"
" *  @param cell_position non-normalized cell position in space\n"
" *  @return              cell type in sense of Macrhing Cubes algorithm\n"
" */\n"
"int get_cell_type(in ivec3 cell_position)\n"
"{\n"
"    vec3 cell_position_normalized = vec3(cell_position) / float(CELLS_PER_AXIS - 1);\n"
"\n"
"    /* Get cell type index of cell to which currently processed vertex (triangle_and_vertex_number) belongs */\n"
"    int  cell_type_index          = textureLod(cell_types, cell_position_normalized, 0.0).r;\n"
"\n"
"    return cell_type_index;\n"
"}\n"
"\n"
"/** Performs a table lookup with cell type index and combined vertex-triangle number specified\n"
" *  to locate an edge number which vertex is currently processed.\n"
" *\n"
" *  @param cell_type_index                    cell type index (in Marching Cubes algorthm sense)\n"
" *  @param combined_triangle_no_and_vertex_no combined vertex and triangle numbers (by formula tringle*3 + vertex)\n"
" *\n"
" *  @return                                   edge number (in sense of Marching Cubes algorithm) or -1 if vertex does not belong to any edge\n"
" */\n"
"int get_edge_number(in int cell_type_index, in int combined_triangle_no_and_vertex_no)\n"
"{\n"
"    /* Normalize indices for texture lookup: [0..14] -> [0.0..1.0], [0..255] -> [0.0..1.0]. */\n"
"    vec2 tri_table_index = vec2(float(combined_triangle_no_and_vertex_no)/14.0, float(cell_type_index)/255.0);\n"
"\n"
"    return textureLod(tri_table, tri_table_index, 0.0).r;\n"
"}\n"
"\n"
"/** Function calculates edge begin or edge end coordinates for specified cell and edge.\n"
" *\n"
" *  @param cell_origin_corner_coordinates normalized cell origin coordinates\n"
" *  @param edge_number                    edge number which coorinates being calculated\n"
" *  @param is_edge_start_vertex           true to request edge start vertex coordinates, false for end edge vertex\n"
" *  @return                               normalized edge start or end vertex coordinates\n"
"*/\n"
"vec3 get_edge_coordinates(in vec3 cell_origin_corner_coordinates, in int edge_number, in bool is_edge_start_vertex)\n"
"{\n"
"    /* These two arrays contain vertex indices which define a cell edge specified by index of arrays. */\n"
"    const int   edge_begins_in_cell_corner[12]  = int[] ( 0,1,2,3,4,5,6,7,0,1,2,3 );\n"
"    const int   edge_ends_in_cell_corner[12]    = int[] ( 1,2,3,0,5,6,7,4,4,5,6,7 );\n"
"    /* Defines offsets by axes for each of 8 cell corneres. */\n"
"    const ivec3 cell_corners_offsets[8]         = ivec3[8]\n"
"    (\n"
"        ivec3(0, 0, 0),\n"
"        ivec3(1, 0, 0),\n"
"        ivec3(1, 0, 1),\n"
"        ivec3(0, 0, 1),\n"
"        ivec3(0, 1, 0),\n"
"        ivec3(1, 1, 0),\n"
"        ivec3(1, 1, 1),\n"
"        ivec3(0, 1, 1)\n"
"    );\n"
"\n"
"    /* Edge corner number (number is in sense of Marching Cubes algorithm). */\n"
"    int edge_corner_no;\n"
"\n"
"    if (is_edge_start_vertex)\n"
"    {\n"
"        /* Use start cell corner of the edge. */\n"
"        edge_corner_no = edge_begins_in_cell_corner[edge_number];\n"
"    }\n"
"    else\n"
"    {\n"
"        /* Use end cell corner of the edge. */\n"
"        edge_corner_no = edge_ends_in_cell_corner[edge_number];\n"
"    }\n"
"\n"
"    /* Normalized cell corner coordinate offsets (to cell origin corner). */\n"
"    vec3 normalized_corner_offsets = vec3(cell_corners_offsets[edge_corner_no]) / float(samples_per_axis - 1);\n"
"\n"
"    /* Normalized cell corner coordinates. */\n"
"    vec3 edge_corner = cell_origin_corner_coordinates + normalized_corner_offsets;\n"
"\n"
"    return edge_corner;\n"
"}\n"
"\n"
"/** Function calculates how close start_corner vertex to intersetction point.\n"
" *\n"
" *  @param start_corner beginning of edge\n"
" *  @param end_corner   end of edge\n"
" *  @param iso_level    scalar field value level defining isosurface\n"
" *  @return             start vertex portion (1.0, if isosurface comes through start vertex)\n"
" */\n"
"float get_start_corner_portion(in vec3 start_corner, in vec3 end_corner, in float iso_level)\n"
"{\n"
"    float result;\n"
"    float start_field_value = textureLod(scalar_field, start_corner, 0.0).r;\n"
"    float end_field_value   = textureLod(scalar_field, end_corner, 0.0).r;\n"
"    float field_delta       = abs(start_field_value - end_field_value);\n"
"\n"
"    if (field_delta > EPSILON)\n"
"    {\n"
"        /* Calculate start vertex portion. */\n"
"        result = abs(end_field_value - iso_level) / field_delta;\n"
"    }\n"
"    else\n"
"    {\n"
"        /* Field values are too close in value to evaluate. Assume middle of an edge. */\n"
"        result = 0.5;\n"
"    }\n"
"\n"
"    return result;\n"
"}\n"
"\n"
"/** Shader entry point. */\n"
"void main()\n"
"{\n"
"    /* [Stage 4 Decode space position] */\n"
"    /* Split gl_vertexID into cell position and vertex number processed by this shader instance. */\n"
"    ivec4 cell_position_and_vertex_no = decode_cell_position(gl_VertexID);\n"
"    ivec3 cell_position               = cell_position_and_vertex_no.xyz;\n"
"    int   triangle_and_vertex_number  = cell_position_and_vertex_no.w;\n"
"    /* [Stage 4 Decode space position] */\n"
"\n"
"    /* [Stage 4 Get cell type and edge number] */\n"
"    /* Get cell type for cell current vertex belongs to. */\n"
"    int   cell_type_index             = get_cell_type(cell_position);\n"
"\n"
"    /* Get edge of the cell to which belongs processed vertex. */\n"
"    int   edge_number                 = get_edge_number(cell_type_index, triangle_and_vertex_number);\n"
"    /* [Stage 4 Get cell type and edge number] */\n"
"\n"
"    /* Check if this is not a vertex of dummy triangle. */\n"
"    if (edge_number != -1)\n"
"    {\n"
"        /* [Stage 4 Calculate cell origin] */\n"
"        /* Calculate normalized coordinates in space of cell origin corner. */\n"
"        vec3 cell_origin_corner    = vec3(cell_position) / float(samples_per_axis - 1);\n"
"        /* [Stage 4 Calculate cell origin] */\n"
"\n"
"        /* [Stage 4 Calculate start and end edge coordinates] */\n"
"        /* Calculate start and end edge coordinates. */\n"
"        vec3 start_corner          = get_edge_coordinates(cell_origin_corner, edge_number, true);\n"
"        vec3 end_corner            = get_edge_coordinates(cell_origin_corner, edge_number, false);\n"
"        /* [Stage 4 Calculate start and end edge coordinates] */\n"
"\n"
"        /* [Stage 4 Calculate middle edge vertex] */\n"
"        /* Calculate share of start point of an edge. */\n"
"        float start_vertex_portion = get_start_corner_portion(start_corner, end_corner, iso_level);\n"
"\n"
"        /* Calculate ''middle'' edge vertex. This vertex is moved closer to start or end vertices of the edge. */\n"
"        vec3 edge_middle_vertex    = mix(end_corner, start_corner, start_vertex_portion);\n"
"        /* [Stage 4 Calculate middle edge vertex] */\n"
"\n"
"        /* [Stage 4 Calculate middle edge normal] */\n"
"        /* Calculate normal to surface in the ''middle'' vertex. */\n"
"        vec3 vertex_normal         = calc_phong_normal(start_vertex_portion, start_corner, end_corner);\n"
"        /* [Stage 4 Calculate middle edge normal] */\n"
"\n"
"        /* Update vertex shader outputs. */\n"
"        gl_Position                = mvp * vec4(edge_middle_vertex, 1.0);        /* Transform vertex position with MVP-matrix.        */\n"
"        phong_vertex_position      = gl_Position;                                /* Set vertex position for fragment shader.          */\n"
"        phong_vertex_normal_vector = vertex_normal;                              /* Set normal vector to surface for fragment shader. */\n"
"        phong_vertex_color         = vec3(0.7);                                  /* Set vertex color for fragment shader.             */\n"
"    }\n"
"    else\n"
"    {\n"
"        /* [Stage 4 Discard dummy triangle] */\n"
"        /* This cell type generates fewer triangles, and this particular one should be discarded. */\n"
"        gl_Position                = vec4(0);                                    /* Discard vertex by setting its coordinate in infinity. */\n"
"        phong_vertex_position      = gl_Position;\n"
"        phong_vertex_normal_vector = vec3(0);\n"
"        phong_vertex_color         = vec3(0);\n"
"        /* [Stage 4 Discard dummy triangle] */\n"
"    }\n"
"}\n";

/**
 * In this shader we render triangles emitted by the mc_triangles_generator_shader vertex shader.
 * The shader uses one directional light source in Phong lighting model.
 * The light source moves on spherical surface around metaballs.
 */
const char* marching_cubes_triangles_frag_shader = "#version 300 es\n"
"\n"
"/** Specify low precision for float type. */\n"
"precision lowp float;\n"
"\n"
"/* Uniforms: */\n"
"/** Current time moment. */\n"
"uniform float time;\n"
"\n"
"/** Position of the vertex (and fragment) in world space. */\n"
"in  vec4 phong_vertex_position;\n"
"\n"
"/** Surface normal vector in world space. */\n"
"in  vec3 phong_vertex_normal_vector;\n"
"\n"
"/** Color passed from vertex shader. */\n"
"in  vec3 phong_vertex_color;\n"
"\n"
"/* Output data: */\n"
"/** Fragment color. */\n"
"out vec4 FragColor;\n"
"\n"
"/** Shader entry point. Main steps are described in comments below. */\n"
"void main()\n"
"{\n"
"    /* Distance to light source. */\n"
"    const float light_distance = 5.0;\n"
"\n"
"    /* Add some movement to light source. */\n"
"    float theta = float(time);\n"
"    float phi   = float(time)/3.0;\n"
"\n"
"    vec3 light_location = vec3\n"
"    (\n"
"        light_distance * cos(theta) * sin(phi),\n"
"        light_distance * cos(theta) * cos(phi),\n"
"        light_distance * sin(theta)\n"
"    );\n"
"\n"
"    /* Scene ambient color. */\n"
"    const vec3  ambient_color = vec3(0.1, 0.1, 0.1);\n"
"    const float attenuation   = 1.0;\n"
"    const float shiness       = 3.0;\n"
"\n"
"    /* Normalize directions. */\n"
"    vec3 normal_direction = normalize(phong_vertex_normal_vector);\n"
"    vec3 view_direction   = normalize(vec3(vec4(0.0, 0.0, 1.0, 0.0) - phong_vertex_position));\n"
"    vec3 light_direction  = normalize(light_location);\n"
"\n"
"    /** Calculate ambient lighting component of directional light. */\n"
"    vec3 ambient_lighting    = ambient_color * phong_vertex_color;\n"
"\n"
"    /** Calculate diffuse reflection lighting component of directional light. */\n"
"    vec3 diffuse_reflection  = attenuation * phong_vertex_color\n"
"                             * max(0.0, dot(normal_direction, light_direction));\n"
"\n"
"    /** Calculate specular reflection lighting component of directional light. */\n"
"    vec3 specular_reflection = vec3(0.0, 0.0, 0.0);\n"
"\n"
"    if (dot(normal_direction, light_direction) >= 0.0)\n"
"    {\n"
"        /* Light source on the right side. */\n"
"        specular_reflection = attenuation * phong_vertex_color\n"
"                            * pow(max(0.0, dot(reflect(-light_direction, normal_direction), view_direction)), shiness);\n"
"    }\n"
"\n"
"    /** Calculate fragment lighting as sum of previous three component. */\n"
"    FragColor = vec4(ambient_lighting + diffuse_reflection + specular_reflection, 1.0);\n"
"}\n";



#endif //MARCHINGCUBES_SHADERCODE_H