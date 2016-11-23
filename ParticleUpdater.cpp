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
GLuint particle_updater_overall_update_transform_feedback_object_id      = 0;
const GLchar* particle_updater_overall_update_varying_name[]             = {
                                                                                "position",
                                                                                "velocity"
                                                                            };

GLuint particle_updater_position_texture_object_id                      = 0;
GLuint particle_updater_velocity_texture_object_id                      = 0;
GLuint particle_updater_pressure_texture_object_id                      = 0;

GLuint        particle_updater_pressure_update_uniform_cells_per_axis_id          = 0;
const GLchar* particle_updater_pressure_update_uniform_cells_per_axis_name        = "cells_per_axis";
GLuint        particle_updater_pressure_update_uniform_n_particles_id             = 0;
const GLchar* particle_updater_pressure_update_uniform_n_particles_name           = "n_particles";

GLuint        particle_updater_pressure_update_uniform_effect_radius_id           = 0;
const GLchar* particle_updater_pressure_update_uniform_effect_radius_name         = "effect_radius";

GLuint        particle_updater_pressure_update_uniform_sampler_space_arr_id       = 0;
const GLchar* particle_updater_pressure_update_uniform_sampler_space_arr_name     = "space_arr";
GLuint        particle_updater_pressure_update_uniform_sampler_index_arr_id       = 0;
const GLchar* particle_updater_pressure_update_uniform_sampler_index_arr_name     = "index_arr";
GLuint        particle_updater_pressure_update_uniform_sampler_position_arr_id    = 0;
const GLchar* particle_updater_pressure_update_uniform_sampler_position_arr_name  = "position_arr";

GLuint        particle_updater_overall_update_uniform_cells_per_axis_id          = 0;
const GLchar* particle_updater_overall_update_uniform_cells_per_axis_name        = "cells_per_axis";
GLuint        particle_updater_overall_update_uniform_n_particles_id             = 0;
const GLchar* particle_updater_overall_update_uniform_n_particles_name           = "n_particles";

GLuint        particle_updater_overall_update_uniform_effect_radius_id           = 0;
const GLchar* particle_updater_overall_update_uniform_effect_radius_name         = "effect_radius";
GLuint        particle_updater_overall_update_uniform_time_delta_id              = 0;
const GLchar* particle_updater_overall_update_uniform_time_delta_name            = "time_delta";
GLuint        particle_updater_overall_update_uniform_wall_offset_id             = 0;
const GLchar* particle_updater_overall_update_uniform_wall_offset_name           = "wall_offset";

GLuint        particle_updater_overall_update_uniform_sampler_space_arr_id       = 0;
const GLchar* particle_updater_overall_update_uniform_sampler_space_arr_name     = "space_arr";
GLuint        particle_updater_overall_update_uniform_sampler_index_arr_id       = 0;
const GLchar* particle_updater_overall_update_uniform_sampler_index_arr_name     = "index_arr";
GLuint        particle_updater_overall_update_uniform_sampler_position_arr_id    = 0;
const GLchar* particle_updater_overall_update_uniform_sampler_position_arr_name  = "position_arr";
GLuint        particle_updater_overall_update_uniform_sampler_velocity_arr_id    = 0;
const GLchar* particle_updater_overall_update_uniform_sampler_velocity_arr_name  = "velocity_arr";
GLuint        particle_updater_overall_update_uniform_sampler_pressure_arr_id    = 0;
const GLchar* particle_updater_overall_update_uniform_sampler_pressure_arr_name  = "pressure_arr";

const GLchar* pressure_update_vert_shader = GLSL(
    
    uniform float effect_radius;
    uniform int cells_per_axis;
    uniform int n_particles;
    
    uniform isampler3D space_arr;
    uniform isampler2D index_arr;
    uniform sampler2D position_arr;

    out float pressure;
    
    int cells_in_3d = cells_per_axis * cells_per_axis * cells_per_axis;

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

    float compute_pressure(vec3 effectee, vec3 effector){
        float norm_dist = distance(effector,effectee)/effect_radius;
        return kernel(norm_dist);
    }

    int nearest_int(float f){
        int i = int(f);
        if(f-float(i) < 0.5){
            return i;
        }
        return i+1;
    }
    
    ivec3 get_nearest_cell(vec3 position){
        return ivec3(nearest_int(position.x*float(cells_per_axis)), 
                     nearest_int(position.y*float(cells_per_axis)), 
                     nearest_int(position.z*float(cells_per_axis)));
    }

    vec2 decode_particle_index(int index){
        int n_k_particles = n_particles / 1024;
        return vec2(float(index%1024)/1023.0,float(index/1024)/float(n_k_particles - 1));
    }

    int get_cell_index(ivec3 cell){
        int index = cell.z;
        index *= cells_per_axis;
        index += cell.y;
        index *= cells_per_axis;
        index += cell.x;

        return index;
    }

    ivec3 decode_cell_index(int index){
        ivec3 cell;
        cell.x = index%cells_per_axis;
        index /= cells_per_axis;
        cell.y = index%cells_per_axis;
        index /= cells_per_axis;
        cell.z = index;
        return cell;
    }

    int get_start_index(ivec3 cell){
        return textureLod(space_arr,vec3(cell)/float(cells_per_axis - 1),0).r;
    }

    int get_end_index(ivec3 cell){
        int index = get_cell_index(cell);
        if(index == cells_in_3d - 1){
            return n_particles;
        }
        ivec3 new_cell = decode_cell_index(index + 1);
        return textureLod(space_arr,vec3(new_cell)/float(cells_per_axis - 1),0).r;
    }

    bool cell_in_space(ivec3 cell){
        return (cell.x >= 0 && cell.x < cells_per_axis &&
                cell.y >= 0 && cell.y < cells_per_axis &&
                cell.z >= 0 && cell.z < cells_per_axis);
    }

    void main(){

        const ivec3 offsets[8] = ivec3[8]
        (
            ivec3( 0,  0,  0),
            ivec3(-1,  0,  0),
            ivec3(-1,  0, -1),
            ivec3( 0,  0, -1),
            ivec3( 0, -1,  0),
            ivec3(-1, -1,  0),
            ivec3(-1, -1, -1),
            ivec3( 0, -1, -1)
        );
        vec3 particle_position = textureLod(position_arr,decode_particle_index(gl_VertexID),0).rgb;
        ivec3 nearest_cell = get_nearest_cell(particle_position);

        pressure = 0;

        for(int i=0; i!= 8 ; i++){
            ivec3 cell = nearest_cell + offsets[i];
            if(cell_in_space(cell)){
                int start_index = get_start_index(cell);
                int end_index = get_end_index(cell);
                for(int index = start_index; index != end_index; index++){
                    int extended_index = textureLod(index_arr,decode_particle_index(index),0).r;
                    
                    if(extended_index == gl_VertexID){
                        continue;
                    }

                    vec3 new_particle_position = textureLod(position_arr,decode_particle_index(extended_index),0).rgb;
                    pressure += compute_pressure(particle_position, new_particle_position);
                }
            }
        }
    }
);

const GLchar* overall_update_vert_shader = GLSL(
    
    uniform float time_delta;
    uniform float effect_radius;
    uniform float wall_offset;
    uniform int cells_per_axis;
    uniform int n_particles;

    uniform isampler3D space_arr;
    uniform isampler2D index_arr;
    uniform sampler2D position_arr;
    uniform sampler2D velocity_arr;
    uniform sampler2D pressure_arr;


    out vec3 position;
    out vec3 velocity;

    int cells_in_3d = cells_per_axis * cells_per_axis * cells_per_axis;

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

    vec3 compute_force(vec3 r1, vec3 v1, float p1, 
                       vec3 r2, vec3 v2, float p2){
        vec3 force = vec3(0);

        return force;
    }

    vec3 check_wall_collission(vec3 r, vec3 v){
        vec3 new_v = vec3(1);
        float coeff = 0.9;
        if((r.x < wall_offset && v.x < 0)||(r.x > 1 - wall_offset && v.x >0 )){
            new_v.x = -coeff ;
        }
        if((r.y < wall_offset && v.y < 0)||(r.y > 1 - wall_offset && v.y >0 )){
            new_v.y = -coeff ;
        }
        if((r.z < wall_offset && v.z < 0)||(r.z > 1 - wall_offset && v.z >0 )){
            new_v.z = -coeff ;
        }
        return v * new_v;
    }

    int nearest_int(float f){
        int i = int(f);
        if(f-float(i) < 0.5){
            return i;
        }
        return i+1;
    }
    
    ivec3 get_nearest_cell(vec3 pos){
        return ivec3(nearest_int(pos.x*float(cells_per_axis)), 
                     nearest_int(pos.y*float(cells_per_axis)), 
                     nearest_int(pos.z*float(cells_per_axis)));
    }

    vec2 decode_particle_index(int index){
        int n_k_particles = n_particles / 1024;
        return vec2(float(index%1024)/1023.0,float(index/1024)/float(n_k_particles - 1));
    }

    int get_cell_index(ivec3 cell){
        int index = cell.z;
        index *= cells_per_axis;
        index += cell.y;
        index *= cells_per_axis;
        index += cell.x;

        return index;
    }

    ivec3 decode_cell_index(int index){
        ivec3 cell;
        cell.x = index%cells_per_axis;
        index /= cells_per_axis;
        cell.y = index%cells_per_axis;
        index /= cells_per_axis;
        cell.z = index;
        return cell;
    }

    int get_start_index(ivec3 cell){
 
        return textureLod(space_arr,vec3(cell)/float(cells_per_axis - 1),0).r;
    }

    int get_end_index(ivec3 cell){
        int index = get_cell_index(cell);
        if(index == cells_in_3d - 1){
            return n_particles;
        }
        ivec3 new_cell = decode_cell_index(index + 1);
        return textureLod(space_arr,vec3(new_cell)/float(cells_per_axis - 1),0).r;
    }

    bool cell_in_space(ivec3 cell){
        return (cell.x >= 0 && cell.x < cells_per_axis &&
                cell.y >= 0 && cell.y < cells_per_axis &&
                cell.z >= 0 && cell.z < cells_per_axis);
    }

    vec3 get_space_force(vec3 pos){
        return vec3(0,-1,0);
    }

    void main(){

        const ivec3 offsets[8] = ivec3[8]
        (
            ivec3( 0,  0,  0),
            ivec3(-1,  0,  0),
            ivec3(-1,  0, -1),
            ivec3( 0,  0, -1),
            ivec3( 0, -1,  0),
            ivec3(-1, -1,  0),
            ivec3(-1, -1, -1),
            ivec3( 0, -1, -1)
        );
        vec3 particle_position = textureLod(position_arr,decode_particle_index(gl_VertexID),0).rgb;
        vec3 particle_velocity = textureLod(velocity_arr,decode_particle_index(gl_VertexID),0).rgb;
        float particle_pressure = textureLod(pressure_arr,decode_particle_index(gl_VertexID),0).r;
        
        ivec3 nearest_cell = get_nearest_cell(particle_position);

        vec3 force = vec3(0);

        for(int i=0; i!= 8 ; i++){
            ivec3 cell = nearest_cell + offsets[i];
            if(cell_in_space(cell)){
                int start_index = get_start_index(cell);
                int end_index = get_end_index(cell);
                for(int index = start_index; index != end_index; index++){

                    int extended_index = textureLod(index_arr,decode_particle_index(index),0).r;
                    if(extended_index == gl_VertexID){
                        continue;
                    }

                    vec3 new_particle_position = textureLod(position_arr,decode_particle_index(extended_index),0).rgb;
                    vec3 new_particle_velocity = textureLod(velocity_arr,decode_particle_index(extended_index),0).rgb;
                    float new_particle_pressure = textureLod(pressure_arr,decode_particle_index(extended_index),0).r;

                    force += compute_force(particle_position, particle_velocity, particle_pressure,
                                           new_particle_position, new_particle_velocity, new_particle_pressure);
                }
            }
        }
        force += get_space_force(particle_position);

        particle_velocity = particle_velocity + time_delta * force;
        particle_velocity = check_wall_collission(particle_position, particle_velocity);
        particle_position = particle_position + time_delta * particle_velocity;

        position = particle_position;
        velocity = particle_velocity;
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

    float end_x = 0.4;
    float end_y = 0.4;
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
    
    GL_CHECK(glTransformFeedbackVaryings(particle_updater_pressure_update_program_id, 1, particle_updater_pressure_update_varying_name, GL_SEPARATE_ATTRIBS));

    GL_CHECK(glLinkProgram(particle_updater_pressure_update_program_id));
    
    particle_updater_pressure_update_uniform_n_particles_id           = GL_CHECK(glGetUniformLocation(particle_updater_pressure_update_program_id, particle_updater_pressure_update_uniform_n_particles_name         ));
    particle_updater_pressure_update_uniform_cells_per_axis_id        = GL_CHECK(glGetUniformLocation(particle_updater_pressure_update_program_id, particle_updater_pressure_update_uniform_cells_per_axis_name      ));
    particle_updater_pressure_update_uniform_effect_radius_id         = GL_CHECK(glGetUniformLocation(particle_updater_pressure_update_program_id, particle_updater_pressure_update_uniform_effect_radius_name       ));
    particle_updater_pressure_update_uniform_sampler_space_arr_id     = GL_CHECK(glGetUniformLocation(particle_updater_pressure_update_program_id, particle_updater_pressure_update_uniform_sampler_space_arr_name   ));
    particle_updater_pressure_update_uniform_sampler_index_arr_id     = GL_CHECK(glGetUniformLocation(particle_updater_pressure_update_program_id, particle_updater_pressure_update_uniform_sampler_index_arr_name   ));
    particle_updater_pressure_update_uniform_sampler_position_arr_id  = GL_CHECK(glGetUniformLocation(particle_updater_pressure_update_program_id, particle_updater_pressure_update_uniform_sampler_position_arr_name));
    
    GL_CHECK(glUseProgram(particle_updater_pressure_update_program_id));

    GL_CHECK(glUniform1i(particle_updater_pressure_update_uniform_n_particles_id          ,n_particles              ));
    GL_CHECK(glUniform1i(particle_updater_pressure_update_uniform_cells_per_axis_id       ,space_cells_per_axis     ));
    GL_CHECK(glUniform1f(particle_updater_pressure_update_uniform_effect_radius_id        ,effect_radius            ));
    GL_CHECK(glUniform1i(particle_updater_pressure_update_uniform_sampler_space_arr_id    ,9                        ));
    GL_CHECK(glUniform1i(particle_updater_pressure_update_uniform_sampler_index_arr_id    ,8                        ));
    GL_CHECK(glUniform1i(particle_updater_pressure_update_uniform_sampler_position_arr_id ,5                        ));

    GL_CHECK(glGenTransformFeedbacks(1, &particle_updater_pressure_update_transform_feedback_object_id));
    GL_CHECK(glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, particle_updater_pressure_update_transform_feedback_object_id));

    GL_CHECK(glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, particle_updater_pressure_buffer_id));
    GL_CHECK(glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0));


    // /*overall update program */
    particle_updater_overall_update_program_id = GL_CHECK(glCreateProgram());

    GL_CHECK(glAttachShader(particle_updater_overall_update_program_id, particle_updater_overall_update_vert_shader_id));
    GL_CHECK(glAttachShader(particle_updater_overall_update_program_id, particle_updater_dummy_frag_shader_id));
    
    GL_CHECK(glTransformFeedbackVaryings(particle_updater_overall_update_program_id, 2, particle_updater_overall_update_varying_name, GL_SEPARATE_ATTRIBS));

    GL_CHECK(glLinkProgram(particle_updater_overall_update_program_id));
    
    particle_updater_overall_update_uniform_cells_per_axis_id        = GL_CHECK(glGetUniformLocation(particle_updater_overall_update_program_id, particle_updater_overall_update_uniform_cells_per_axis_name      ));
    particle_updater_overall_update_uniform_n_particles_id           = GL_CHECK(glGetUniformLocation(particle_updater_overall_update_program_id, particle_updater_overall_update_uniform_n_particles_name         ));
    particle_updater_overall_update_uniform_effect_radius_id         = GL_CHECK(glGetUniformLocation(particle_updater_overall_update_program_id, particle_updater_overall_update_uniform_effect_radius_name       ));
    particle_updater_overall_update_uniform_wall_offset_id           = GL_CHECK(glGetUniformLocation(particle_updater_overall_update_program_id, particle_updater_overall_update_uniform_wall_offset_name         ));
    particle_updater_overall_update_uniform_time_delta_id            = GL_CHECK(glGetUniformLocation(particle_updater_overall_update_program_id, particle_updater_overall_update_uniform_time_delta_name          ));
    particle_updater_overall_update_uniform_sampler_space_arr_id     = GL_CHECK(glGetUniformLocation(particle_updater_overall_update_program_id, particle_updater_overall_update_uniform_sampler_space_arr_name   ));
    particle_updater_overall_update_uniform_sampler_index_arr_id     = GL_CHECK(glGetUniformLocation(particle_updater_overall_update_program_id, particle_updater_overall_update_uniform_sampler_index_arr_name   ));
    particle_updater_overall_update_uniform_sampler_position_arr_id  = GL_CHECK(glGetUniformLocation(particle_updater_overall_update_program_id, particle_updater_overall_update_uniform_sampler_position_arr_name));
    particle_updater_overall_update_uniform_sampler_velocity_arr_id  = GL_CHECK(glGetUniformLocation(particle_updater_overall_update_program_id, particle_updater_overall_update_uniform_sampler_velocity_arr_name));
    particle_updater_overall_update_uniform_sampler_pressure_arr_id  = GL_CHECK(glGetUniformLocation(particle_updater_overall_update_program_id, particle_updater_overall_update_uniform_sampler_pressure_arr_name));
    
    GL_CHECK(glUseProgram(particle_updater_overall_update_program_id));

    GL_CHECK(glUniform1i(particle_updater_overall_update_uniform_cells_per_axis_id       ,space_cells_per_axis     ));
    GL_CHECK(glUniform1i(particle_updater_overall_update_uniform_n_particles_id          ,n_particles              ));
    GL_CHECK(glUniform1f(particle_updater_overall_update_uniform_effect_radius_id        ,effect_radius            ));
    GL_CHECK(glUniform1f(particle_updater_overall_update_uniform_wall_offset_id          ,wall_offset              ));
    GL_CHECK(glUniform1i(particle_updater_overall_update_uniform_sampler_space_arr_id    ,9                        ));
    GL_CHECK(glUniform1i(particle_updater_overall_update_uniform_sampler_index_arr_id    ,8                        ));
    GL_CHECK(glUniform1i(particle_updater_overall_update_uniform_sampler_position_arr_id ,5                        ));
    GL_CHECK(glUniform1i(particle_updater_overall_update_uniform_sampler_velocity_arr_id ,6                        ));
    GL_CHECK(glUniform1i(particle_updater_overall_update_uniform_sampler_pressure_arr_id ,7                        ));

    GL_CHECK(glGenTransformFeedbacks(1, &particle_updater_overall_update_transform_feedback_object_id));
    GL_CHECK(glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, particle_updater_overall_update_transform_feedback_object_id));

    GL_CHECK(glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, particle_updater_position_buffer_id));
    GL_CHECK(glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, particle_updater_velocity_buffer_id));
    
    GL_CHECK(glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0));

// texture

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

        GL_CHECK(glUniform1f(particle_updater_overall_update_uniform_time_delta_id, time_delta));

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
    UpdatePressureValues();
    UpdatePositionAndVelocity();    
}

