#include "ShaderCode.h"

GLuint particle_updater_pressure_update_program_id                       = 0;
GLuint particle_updater_overall_update_program_id                        = 0;

GLuint particle_updater_pressure_update_vert_shader_id                   = 0;
GLuint particle_updater_overall_update_vert_shader_id                    = 0;
GLuint particle_updater_dummy_frag_shader_id                             = 0;

GLuint particle_updater_position_buffer_id                               = 0;
GLuint particle_updater_velocity_buffer_id                               = 0;
GLuint particle_updater_pressure_buffer_id                               = 0;

GLuint particle_updater_pressure_update_transform_feedback_object_id     = 0;
const GLchar* particle_updater_pressure_update_varying_name[]            = {
                                                                                "pressure"
                                                                           };
const GLchar* particle_updater_overall_update_varying_name[]             = {
                                                                                "position",
                                                                                "gl_NextBuffer",
                                                                                "velocity"
                                                                            };
GLuint particle_updater_overall_update_transform_feedback_object_id     = 0;

GLuint particle_updater_position_texture_object_id                      = 0;
GLuint particle_updater_velocity_texture_object_id                      = 0;
GLuint particle_updater_pressure_texture_object_id                      = 0;
GLuint particle_updater_spacial_index_array_texture_object_id           = 0;
GLuint particle_updater_ordered_index_array_texture_object_id           = 0;

GLuint        particle_updater_pressure_update_uniform_cells_per_axis_id          = 0;
const GLchar* particle_updater_pressure_update_uniform_cells_per_axis_name        = "cells_per_axis";
GLuint        particle_updater_pressure_update_uniform_cells_in_3d_id             = 0;
const GLchar* particle_updater_pressure_update_uniform_cells_in_3d_name           = "cells_in_3d";
GLuint        particle_updater_pressure_update_uniform_n_particles_id             = 0;
const GLchar* particle_updater_pressure_update_uniform_n_particles_name           = "n_k_particles";

GLuint        particle_updater_pressure_update_uniform_effect_radius_id           = 0;
const GLchar* particle_updater_pressure_update_uniform_effect_radius_name         = "effect_radius";

GLuint        particle_updater_pressure_update_uniform_sampler_space_arr_id       = 0;
const GLchar* particle_updater_pressure_update_uniform_sampler_space_arr_name     = "space_arr";
GLuint        particle_updater_pressure_update_uniform_sampler_ordered_arr_id     = 0;
const GLchar* particle_updater_pressure_update_uniform_sampler_ordered_arr_name   = "ordered_arr";
GLuint        particle_updater_pressure_update_uniform_sampler_position_arr_id    = 0;
const GLchar* particle_updater_pressure_update_uniform_sampler_position_arr_name  = "position_arr";

GLuint        particle_updater_overall_update_uniform_cells_per_axis_id          = 0;
const GLchar* particle_updater_overall_update_uniform_cells_per_axis_name        = "cells_per_axis";
GLuint        particle_updater_overall_update_uniform_cells_in_3d_id             = 0;
const GLchar* particle_updater_overall_update_uniform_cells_in_3d_name           = "cells_in_3d";
GLuint        particle_updater_overall_update_uniform_n_particles_id             = 0;
const GLchar* particle_updater_overall_update_uniform_n_particles_name           = "n__k_particles";

GLuint        particle_updater_overall_update_uniform_effect_radius_id           = 0;
const GLchar* particle_updater_overall_update_uniform_effect_radius_name         = "effect_radius";

GLuint        particle_updater_overall_update_uniform_sampler_space_arr_id       = 0;
const GLchar* particle_updater_overall_update_uniform_sampler_space_arr_name     = "space_arr";
GLuint        particle_updater_overall_update_uniform_sampler_ordered_arr_id     = 0;
const GLchar* particle_updater_overall_update_uniform_sampler_ordered_arr_name   = "ordered_arr";
GLuint        particle_updater_overall_update_uniform_sampler_position_arr_id    = 0;
const GLchar* particle_updater_overall_update_uniform_sampler_position_arr_name  = "position_arr";
GLuint        particle_updater_overall_update_uniform_sampler_velocity_arr_id    = 0;
const GLchar* particle_updater_overall_update_uniform_sampler_velocity_arr_name  = "velocity_arr";
GLuint        particle_updater_overall_update_uniform_sampler_pressure_arr_id    = 0;
const GLchar* particle_updater_overall_update_uniform_sampler_pressure_arr_name  = "pressure_arr";

const GLchar* pressure_update_vert_shader = GLSL(
    
    precision highp sampler2D;

    uniform int n_k_particles;
    uniform float effect_radius;
    
    uniform sampler2D position_arr;

    out float pressure;
    
    float kernel(float dist){
        if(dist > 1.0 || dist < 0.0){
            return 0.0;
        }
        if(dist < 0.5){
            return (1.0 - 4.0*dist*dist*dist);
        }
        dist = 1.0 - dist;
        return 4.0*dist*dist*dist;
    }

    float computePressure(vec3 effectee, vec3 effector){
        float norm_dist = distance(effector,effectee)/effect_radius;
        return kernel(norm_dist);
    }

    vec2 decodeParticleIndex(int index){
        return vec2(float(index%1000)/999.0,float(index/1000)/float(n_k_particles - 1));
    }

    void main(){

        vec3 particle_position = textureLod(position_arr,decodeParticleIndex(gl_VertexID),0).rgb;
        pressure = 0;

        for(int i=0; i!= 500 ; i++){
               vec3 new_position = textureLod(position_arr,decodeParticleIndex(i),0).rgb;
               pressure += computePressure(particle_position,new_position);
        }
    }
);

const GLchar* overall_update_vert_shader = GLSL(
    
    precision highp sampler1D;
    precision highp isampler1D;

    uniform int n_k_particles;

    uniform float effect_radius;
    
    uniform isampler3D space_arr;
    uniform isampler2D ordered_arr;
    uniform sampler2D position_arr;
    uniform sampler2D velocity_arr;
    uniform sampler2D pressure_arr;


    out vec3 position;
    out vec3 velocity;

    void main(){

    }
);

const GLchar* dummy_frag_shader = GLSL(
    void main()
    {
    }
);


GLfloat*     position;
GLfloat*     velocity;

void InitializeDamBreak(){
    float begin_x = wall_offset;
    float begin_y = begin_x;
    float begin_z = begin_x;

    float end_x = 0.2;
    float end_y = 0.5;
    float end_z = 1 - wall_offset;

    float volume = (end_x-begin_x)*(end_y-begin_y)*(end_z-begin_z);
    float particles_per_unit = pow(float(n_particles)/volume,1/3.0);
    float delta = 1.0/particles_per_unit;
    int n_particles_x = int((end_x-begin_x)*particles_per_unit)+1;
    int n_particles_y = int((end_y-begin_y)*particles_per_unit)+1;
    int n_particles_z = int((end_z-begin_z)*particles_per_unit)+1;
    for(int i = 0; i != n_particles; i++){    
        int index = i;
        int x = index%n_particles_x;
        index /= n_particles_x;
        int z = index%n_particles_z;
        index /= n_particles_z;
        int y = index;
        position[3*i] = begin_x + float(x)*delta;
        position[3*i+1] = begin_y + float(y)*delta;
        position[3*i+2] = begin_z + float(z)*delta;
        velocity[3*i] = 0;
        velocity[3*i+1] = 0;
        velocity[3*i+2] = 0;
    }
}

void InitializeMemory(){
    position = new GLfloat[3*n_particles];
    velocity = new GLfloat[3*n_particles];
}

void Set2DTexParams(){
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST      ));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST      ));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0               ));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL,  0               ));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE));
}

void Set3DTexParams(){
    GL_CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST      ));
    GL_CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST      ));
    GL_CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0               ));
    GL_CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL,  0               ));
    GL_CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R,     GL_CLAMP_TO_EDGE));
}

void InitializeTextures(){

    GL_CHECK(glActiveTexture(GL_TEXTURE5));
    GL_CHECK(glBindBuffer   (GL_PIXEL_UNPACK_BUFFER, particle_updater_position_buffer_id));
    GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D,
                             0,
                             0,
                             0,
                             1024,
                             n_k_particles,
                             GL_RGB,
                             GL_FLOAT,
                             NULL
                            ));

    GL_CHECK(glActiveTexture(GL_TEXTURE6));
    GL_CHECK(glBindBuffer   (GL_PIXEL_UNPACK_BUFFER, particle_updater_velocity_buffer_id));
    GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D,
                             0,
                             0,
                             0,
                             1024,
                             n_k_particles,
                             GL_RGB,
                             GL_FLOAT,
                             NULL
                            ));
}

void SetUpParticleUpdater(){
    InitializeMemory();
    InitializeDamBreak();
   
    GL_CHECK(glGenBuffers(1, &particle_updater_position_buffer_id));
    GL_CHECK(glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, particle_updater_position_buffer_id));
    GL_CHECK(glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, 3*n_particles*sizeof(GLfloat), position, GL_STREAM_READ));
    GL_CHECK(glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0));

    GL_CHECK(glGenBuffers(1, &particle_updater_velocity_buffer_id));
    GL_CHECK(glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, particle_updater_velocity_buffer_id));
    GL_CHECK(glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, 3*n_particles*sizeof(GLfloat), velocity, GL_STREAM_READ));
    GL_CHECK(glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0));
    
    GL_CHECK(glGenBuffers(1, &particle_updater_pressure_buffer_id));
    GL_CHECK(glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, particle_updater_pressure_buffer_id));
    GL_CHECK(glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, n_particles*sizeof(GLfloat), NULL, GL_STREAM_READ));
    GL_CHECK(glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0));

    Shader::processShader(&particle_updater_pressure_update_vert_shader_id, pressure_update_vert_shader, GL_VERTEX_SHADER);
    Shader::processShader(&particle_updater_overall_update_vert_shader_id, overall_update_vert_shader, GL_VERTEX_SHADER);
    Shader::processShader(&particle_updater_dummy_frag_shader_id, dummy_frag_shader, GL_FRAGMENT_SHADER);
    
    /*Pressure update program */
    particle_updater_pressure_update_program_id = GL_CHECK(glCreateProgram());

    GL_CHECK(glAttachShader(particle_updater_pressure_update_program_id, particle_updater_pressure_update_vert_shader_id));
    GL_CHECK(glAttachShader(particle_updater_pressure_update_program_id, particle_updater_dummy_frag_shader_id));
    
    GL_CHECK(glTransformFeedbackVaryings(particle_updater_pressure_update_program_id, 1, particle_updater_pressure_update_varying_name, GL_INTERLEAVED_ATTRIBS));

    GL_CHECK(glLinkProgram(particle_updater_pressure_update_program_id));
    
    particle_updater_pressure_update_uniform_n_particles_id           = GL_CHECK(glGetUniformLocation(particle_updater_pressure_update_program_id, particle_updater_pressure_update_uniform_n_particles_name         ));
    particle_updater_pressure_update_uniform_effect_radius_id         = GL_CHECK(glGetUniformLocation(particle_updater_pressure_update_program_id, particle_updater_pressure_update_uniform_effect_radius_name       ));
    particle_updater_pressure_update_uniform_sampler_position_arr_id  = GL_CHECK(glGetUniformLocation(particle_updater_pressure_update_program_id, particle_updater_pressure_update_uniform_sampler_position_arr_name));
    
    GL_CHECK(glUseProgram(particle_updater_pressure_update_program_id));

    GL_CHECK(glUniform1i(particle_updater_pressure_update_uniform_n_particles_id          ,n_k_particles              ));
    GL_CHECK(glUniform1f(particle_updater_pressure_update_uniform_effect_radius_id        ,effect_radius            ));
    GL_CHECK(glUniform1i(particle_updater_pressure_update_uniform_sampler_position_arr_id ,5                        ));

    GL_CHECK(glGenTransformFeedbacks(1, &particle_updater_pressure_update_transform_feedback_object_id));
    GL_CHECK(glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, particle_updater_pressure_update_transform_feedback_object_id));

    GL_CHECK(glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, particle_updater_pressure_buffer_id));
    GL_CHECK(glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0));


    // /*overall update program */
    // particle_updater_overall_update_program_id = GL_CHECK(glCreateProgram());

    // GL_CHECK(glAttachShader(particle_updater_overall_update_program_id, particle_updater_overall_update_vert_shader_id));
    // GL_CHECK(glAttachShader(particle_updater_overall_update_program_id, particle_updater_dummy_frag_shader_id));
    
    // GL_CHECK(glTransformFeedbackVaryings(particle_updater_overall_update_program_id, 3, particle_updater_overall_update_varying_name, GL_INTERLEAVED_ATTRIBS));

    // GL_CHECK(glLinkProgram(particle_updater_overall_update_program_id));
    
    // particle_updater_overall_update_uniform_cells_per_axis_id        = GL_CHECK(glGetUniformLocation(particle_updater_overall_update_program_id, particle_updater_overall_update_uniform_cells_per_axis_name      ));
    // particle_updater_overall_update_uniform_cells_in_3d_id           = GL_CHECK(glGetUniformLocation(particle_updater_overall_update_program_id, particle_updater_overall_update_uniform_cells_in_3d_name         ));
    // particle_updater_overall_update_uniform_n_particles_id           = GL_CHECK(glGetUniformLocation(particle_updater_overall_update_program_id, particle_updater_overall_update_uniform_n_particles_name         ));
    // particle_updater_overall_update_uniform_effect_radius_id         = GL_CHECK(glGetUniformLocation(particle_updater_overall_update_program_id, particle_updater_overall_update_uniform_effect_radius_name       ));
    // particle_updater_overall_update_uniform_sampler_space_arr_id     = GL_CHECK(glGetUniformLocation(particle_updater_overall_update_program_id, particle_updater_overall_update_uniform_sampler_space_arr_name   ));
    // particle_updater_overall_update_uniform_sampler_ordered_arr_id   = GL_CHECK(glGetUniformLocation(particle_updater_overall_update_program_id, particle_updater_overall_update_uniform_sampler_ordered_arr_name ));
    // particle_updater_overall_update_uniform_sampler_position_arr_id  = GL_CHECK(glGetUniformLocation(particle_updater_overall_update_program_id, particle_updater_overall_update_uniform_sampler_position_arr_name));
    // particle_updater_overall_update_uniform_sampler_velocity_arr_id  = GL_CHECK(glGetUniformLocation(particle_updater_overall_update_program_id, particle_updater_overall_update_uniform_sampler_velocity_arr_name));
    // particle_updater_overall_update_uniform_sampler_pressure_arr_id  = GL_CHECK(glGetUniformLocation(particle_updater_overall_update_program_id, particle_updater_overall_update_uniform_sampler_pressure_arr_name));
    
    // GL_CHECK(glUseProgram(particle_updater_overall_update_program_id));

    // GL_CHECK(glUniform1i(particle_updater_overall_update_uniform_cells_per_axis_id       ,space_cells_per_axis     ));
    // GL_CHECK(glUniform1i(particle_updater_overall_update_uniform_cells_in_3d_id          ,space_cells_in_3d_space  ));
    // GL_CHECK(glUniform1i(particle_updater_overall_update_uniform_n_particles_id          ,n_particles              ));
    // GL_CHECK(glUniform1f(particle_updater_overall_update_uniform_effect_radius_id        ,effect_radius            ));
    // GL_CHECK(glUniform1i(particle_updater_overall_update_uniform_sampler_space_arr_id    ,8                        ));
    // GL_CHECK(glUniform1i(particle_updater_overall_update_uniform_sampler_ordered_arr_id  ,9                        ));
    // GL_CHECK(glUniform1i(particle_updater_overall_update_uniform_sampler_position_arr_id ,5                        ));
    // GL_CHECK(glUniform1i(particle_updater_overall_update_uniform_sampler_velocity_arr_id ,6                        ));
    // GL_CHECK(glUniform1i(particle_updater_overall_update_uniform_sampler_pressure_arr_id ,7                        ));

    // GL_CHECK(glGenTransformFeedbacks(1, &particle_updater_overall_update_transform_feedback_object_id));
    // GL_CHECK(glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, particle_updater_overall_update_transform_feedback_object_id));

    // GL_CHECK(glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, particle_updater_position_buffer_id));
    // GL_CHECK(glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, particle_updater_velocity_buffer_id));
    
    // GL_CHECK(glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0));

//1&2&4 off limits

    GL_CHECK(glGenTextures(1, &particle_updater_position_texture_object_id));

    GL_CHECK(glActiveTexture(GL_TEXTURE5));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, particle_updater_position_texture_object_id));
    GL_CHECK(glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, 1024, n_k_particles));
    Set2DTexParams();

    GL_CHECK(glGenTextures(1, &particle_updater_velocity_texture_object_id));

    GL_CHECK(glActiveTexture(GL_TEXTURE6));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, particle_updater_velocity_texture_object_id));
    GL_CHECK(glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, 1024, n_k_particles));
    Set2DTexParams();

    GL_CHECK(glGenTextures(1, &particle_updater_pressure_texture_object_id));

    GL_CHECK(glActiveTexture(GL_TEXTURE7));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, particle_updater_pressure_texture_object_id));
    GL_CHECK(glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, 1024, n_k_particles));
    Set2DTexParams();

    InitializeTextures();
}


void UpdatePressureValues(){

    GL_CHECK(glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, particle_updater_pressure_update_transform_feedback_object_id));

    GL_CHECK(glEnable(GL_RASTERIZER_DISCARD));
    {
        GL_CHECK(glUseProgram(particle_updater_pressure_update_program_id));

        GL_CHECK(glBeginTransformFeedback(GL_POINTS));
        {
            GL_CHECK(glDrawArrays(GL_POINTS, 0, n_particles));
        }
        GL_CHECK(glEndTransformFeedback());
    }
    GL_CHECK(glDisable(GL_RASTERIZER_DISCARD));

    GL_CHECK(glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0));

    GL_CHECK(glActiveTexture(GL_TEXTURE7));
    GL_CHECK(glBindBuffer   (GL_PIXEL_UNPACK_BUFFER, particle_updater_pressure_buffer_id));
    GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D,
                             0,
                             0,
                             0,
                             1024,
                             n_k_particles,
                             GL_RED,
                             GL_FLOAT,
                             NULL
                            ));
    
}

void UpdatePositionAndVelocity(){

    GL_CHECK(glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, particle_updater_overall_update_transform_feedback_object_id));

    GL_CHECK(glEnable(GL_RASTERIZER_DISCARD));
    {
        GL_CHECK(glUseProgram(particle_updater_overall_update_program_id));

        //input time GL_CHECK(glUniform1f(spheres_updater_uniform_time_id, model_time));

        GL_CHECK(glBeginTransformFeedback(GL_POINTS));
        {
            GL_CHECK(glDrawArrays(GL_POINTS, 0, n_particles));
        }
        GL_CHECK(glEndTransformFeedback());
    }
    GL_CHECK(glDisable(GL_RASTERIZER_DISCARD));

    GL_CHECK(glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0));

}

void UpdateParticles(){
    InitializeTextures();
    SortParticles();
    FindStartIndex();
    // UpdatePressureValues();
    // UpdatePositionAndVelocity();    
}

