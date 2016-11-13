#include "ShaderCode.h"


GLuint        marching_cubes_program_id                           = 0;
GLuint        marching_cubes_geo_shader_id                        = 0;
GLuint        marching_cubes_vert_shader_id                       = 0;
GLuint        marching_cubes_frag_shader_id                       = 0;

const GLchar* marching_cubes_uniform_time_name                    = "time";
GLuint 		  marching_cubes_uniform_time_id                      = 0;

const GLchar* marching_cubes_uniform_samples_per_axis_name        = "samples_per_axis";
GLuint 		  marching_cubes_uniform_samples_per_axis_id          = 0;

const GLchar* marching_cubes_uniform_iso_level_name               = "iso_level";
GLuint 	      marching_cubes_uniform_iso_level_id                 = 0;

const GLchar* marching_cubes_uniform_mvp_name                     = "mvp";
GLuint 		  marching_cubes_uniform_mvp_id                       = 0;

const GLchar* marching_cubes_uniform_scalar_field_sampler_name    = "scalar_field";
GLuint 		  marching_cubes_uniform_scalar_field_sampler_id      = 0;

const GLchar* marching_cubes_uniform_tri_table_sampler_name       = "tri_table";
GLuint 		  marching_cubes_uniform_tri_table_sampler_id         = 0;

GLuint        marching_cubes_lookup_table_texture_id  			   = 0;
GLuint        marching_cubes_vao_id								   = 0;

const GLchar* marching_cubes_vert_shader = GLSL(

	uniform int samples_per_axis;

	out ivec3 space_position_in;

	ivec3 decode_space_position(in int cell_index)
	{
	    int cells_per_axis = samples_per_axis - 1;
	    ivec3 space_position;
	    int   encoded_position = cell_index;

	    /* Calculate coordinates from encoded position */
	    space_position.x       = encoded_position % cells_per_axis;
	    encoded_position       = encoded_position / cells_per_axis;

	    space_position.y       = encoded_position % cells_per_axis;
	    encoded_position       = encoded_position / cells_per_axis;

	    space_position.z       = encoded_position;

	    return space_position;
	}


	/** Shader entry point. */
	void main()
	{
		space_position_in = decode_space_position(gl_VertexID);
	}
);

const GLchar* marching_cubes_geo_shader = GLSL(

	layout(points) in;
	layout(points, max_vertices = 15) out;


	in ivec3 space_position_in[];

	uniform mat4 mvp;

	void main()
	{
	    gl_Position = mvp * vec4(vec3(space_position_in[0]) / float(63),1);
	    EmitVertex();
	    EndPrimitive();
	}
);

const GLchar* marching_cubes_frag_shader = GLSL(

	
	out vec4 FragColor;

	void main()
	{
	    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	}
);

void SetUpMarchingCubes(){

    /*Marching cubes using geometry shaders*/

    /*geometry shaders*/

    marching_cubes_program_id = GL_CHECK(glCreateProgram());;

    Shader::processShader(&marching_cubes_geo_shader_id, marching_cubes_geo_shader, GL_GEOMETRY_SHADER);
    Shader::processShader(&marching_cubes_vert_shader_id, marching_cubes_vert_shader, GL_VERTEX_SHADER);
    Shader::processShader(&marching_cubes_frag_shader_id, marching_cubes_frag_shader, GL_FRAGMENT_SHADER);

    GL_CHECK(glAttachShader(marching_cubes_program_id, marching_cubes_geo_shader_id));
    GL_CHECK(glAttachShader(marching_cubes_program_id, marching_cubes_vert_shader_id));
    GL_CHECK(glAttachShader(marching_cubes_program_id, marching_cubes_frag_shader_id));
    
    GL_CHECK(glLinkProgram(marching_cubes_program_id));
    
    // marching_cubes_uniform_time_id                                  = GL_CHECK(glGetUniformLocation(marching_cubes_program_id, marching_cubes_uniform_time_name));
    marching_cubes_uniform_samples_per_axis_id                      = GL_CHECK(glGetUniformLocation(marching_cubes_program_id, marching_cubes_uniform_samples_per_axis_name));
    // marching_cubes_uniform_iso_level_id                             = GL_CHECK(glGetUniformLocation(marching_cubes_program_id, marching_cubes_uniform_iso_level_name));
    marching_cubes_uniform_mvp_id                                   = GL_CHECK(glGetUniformLocation(marching_cubes_program_id, marching_cubes_uniform_mvp_name));
    // marching_cubes_uniform_scalar_field_sampler_id                  = GL_CHECK(glGetUniformLocation(marching_cubes_program_id, marching_cubes_uniform_scalar_field_sampler_name));
    // marching_cubes_uniform_tri_table_sampler_id                     = GL_CHECK(glGetUniformLocation(marching_cubes_program_id, marching_cubes_uniform_tri_table_sampler_name));
    
    GL_CHECK(glUseProgram(marching_cubes_program_id));

    /* Initialize uniforms constant throughout rendering loop. */
    GL_CHECK(glUniform1i(marching_cubes_uniform_samples_per_axis_id,            samples_per_axis));
    // GL_CHECK(glUniform1f(marching_cubes_uniform_iso_level_id,                   isosurface_level));
    // GL_CHECK(glUniform1i(marching_cubes_uniform_tri_table_sampler_id,   4               ));
    // GL_CHECK(glUniform1i(marching_cubes_uniform_scalar_field_sampler_id,1               ));
    GL_CHECK(glUniformMatrix4fv(marching_cubes_uniform_mvp_id, 1, GL_FALSE,     mvp.getAsArray()));


    GL_CHECK(glGenTextures(1, &marching_cubes_lookup_table_texture_id));

    /* Lookup array (tri_table) uses GL_TEXTURE_2D target of texture unit 4. */
    GL_CHECK(glActiveTexture(GL_TEXTURE4));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, marching_cubes_lookup_table_texture_id));

    /* Tune texture settings to use it as a data source. */
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST      ));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST      ));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0               ));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL,  0               ));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE));

    /* Load lookup table (tri_table) into texture. */
    GL_CHECK(glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32I, mc_vertices_per_cell, mc_cells_types_count));
    GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D,        /* Use texture bound to GL_TEXTURE_2D               */
                             0,                    /* Base image level                                 */
                             0,                    /* From the texture origin                          */
                             0,                    /* From the texture origin                          */
                             mc_vertices_per_cell, /* Width will represent vertices in all 5 triangles */
                             mc_cells_types_count, /* Height will represent cell type                  */
                             GL_RED_INTEGER,       /* Texture will have only one component             */
                             GL_INT,               /* ... of type int                                  */
                             tri_table             /* Data will be copied directly from tri_table      */
                            ));



    GL_CHECK(glGenVertexArrays(1, &marching_cubes_vao_id));

    GL_CHECK(glBindVertexArray(marching_cubes_vao_id));
}


void DrawMarchingCubes(){

	GL_CHECK(glUseProgram(marching_cubes_program_id));

    GL_CHECK(glDrawArrays(GL_POINTS, 0, cells_in_3d_space));
}