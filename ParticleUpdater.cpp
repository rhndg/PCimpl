#include "ShaderCode.h"

GLuint particle_updater_pressure_update_program_id                       = 0;
GLuint particle_updater_overall_update_program_id                        = 0;

GLuint particle_updater_pressure_update_vert_shader_id                   = 0;
GLuint particle_updater_overall_update_vert_shader_id                    = 0;
GLuint particle_updater_dummy_frag_shader_id                             = 0;

GLuint particle_updater_position_buffer_id                               = 0;
GLuint particle_updater_velocity_buffer_id                               = 0;
GLuint particle_updater_pressure_buffer_id                               = 0;
GLuint particle_updater_space_arr_buffer_id                              = 0;
GLuint particle_updater_ordered_arr_buffer_id                            = 0;

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
const GLchar* particle_updater_pressure_update_uniform_n_particles_name           = "n_particles";

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
const GLchar* particle_updater_overall_update_uniform_n_particles_name           = "n_particles";

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

GLint n_particles = n_k_particles*1000;

const GLchar* pressure_update_vert_shader = GLSL(
    
    precision highp sampler2D;
    precision highp isampler2D;
    precision highp sampler3D;
    precision highp isampler3D;

    uniform int cells_per_axis;
    uniform int cells_in_3d;
    uniform int n_k_particles;

    uniform float effect_radius;
    
    uniform isampler3D space_arr;
    uniform isampler2D ordered_arr;
    uniform sampler2D position_arr;

    out float pressure;

    int getNearest(float f){
        int i = int(f);
        if(f-float(i)>0.5){
            return i+1;
        }
        return i;
    }

    ivec3 getNearestCell(vec3 position){
        vec3 exloded_position = position*float(cells_per_axis);
        return ivec3(getNearest(exloded_position.x),getNearest(exloded_position.y),getNearest(exloded_position.z));
    }

    int getEncodedCellIndex(ivec3 cell){
        int code = 0;
        code += cell.z;
        code *= cells_per_axis;
        code += cell.y;
        code *= cells_per_axis;
        code += cell.x;
        return code;
    }

    ivec3 decodeCellIndex(int index){
        ivec3 cellIndex;
        int temp = index;
        cellIndex.x = index%cells_per_axis;
        temp /= cells_per_axis;
        cellIndex.y = index%cells_per_axis;
        temp /= cells_per_axis;
        cellIndex.z = index;
        return cellIndex;
    }

    bool notInSpace(ivec3 cell){
        if(cell.x < 0 || cell.x >= cells_per_axis||
           cell.y < 0 || cell.y >= cells_per_axis||
           cell.z < 0 || cell.z >= cells_per_axis){
            return true;
        }
        return false;
    }
    
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

    ivec2 getIndexRange(ivec3 space_arr_index){
        vec3 norm_index = vec3(space_arr_index)/float(cells_per_axis - 1);
        int decoded_index = getEncodedCellIndex(space_arr_index);
        ivec2 range;
        range.x = textureLod(space_arr, norm_index, 0).r;
        if(decoded_index == cells_in_3d - 1){
            range.y = n_k_particles * 1000;
            return range;
        }
        range.y = textureLod(space_arr, vec3(decodeCellIndex(decoded_index+1))/float(cells_per_axis-1), 0).r;
        return range;
    }

    void main(){
        ivec3 offset[8] = ivec3[8](
            ivec3( 0,  0,  0),
            ivec3(-1,  0,  0),
            ivec3(-1,  0, -1),
            ivec3( 0,  0, -1),
            ivec3( 0, -1,  0),
            ivec3(-1, -1,  0),
            ivec3(-1, -1, -1),
            ivec3( 0, -1, -1)
            );
        // ivec2 decoded_index 
        vec3 particle_position = textureLod(position_arr,decodeParticleIndex(gl_VertexID),0).rgb;
        pressure = 0;

        // int temp = textureLod(ordered_arr, float(gl_VertexID)/float(n_particles - 1), 0).r;
        // vec3 pos = textureLod(position_arr,float(400)/float(n_particles-1),0).rgb;
        // pressure = pos.z;
        // return;

        ivec3 nearestCell = getNearestCell(particle_position);

        for(int offset_index = 0; offset_index != 8; offset_index++){
            
            ivec3 current_cell = nearestCell + offset[offset_index];
            if(notInSpace(current_cell)){
                continue;
            }

            ivec2 range = getIndexRange(current_cell);

            while(range.x != range.y){
                int particle_index = textureLod(ordered_arr, decodeParticleIndex(range.x), 0).r;
                if(particle_index == gl_VertexID){
                    range.x++;
                    continue;
                }
                vec3 new_particle_position = textureLod(position_arr,decodeParticleIndex(particle_index),0).rgb;
                // pressure += computePressure(particle_position,new_particle_position);
                if(particle_index == 400){
                    pressure += 1;
                }
                range.x++;
            }
        }
    }

);

const GLchar* overall_update_vert_shader = GLSL(
    
    precision highp sampler1D;
    precision highp isampler1D;

    uniform int cells_per_axis;
    uniform int cells_in_3d;
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
vector<int>* cells;
GLint*       ordered_index;
GLint*       space_distribution_index;

void ReorderPerticlesInCells(){
    
    for(int i = 0; i != space_cells_in_3d_space; i++){
        cells[i].clear();
    }

    for(int i = 0; i != n_particles; i++){
        float x = position[3*i];
        float y = position[3*i+1];
        float z = position[3*i+2];
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
            ordered_index[start_index] = cells[i][j];
            start_index++;
        }
    }
}

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
    cells = new vector<int>[space_cells_in_3d_space];
    ordered_index = new GLint[n_particles];
    space_distribution_index = new GLint[space_cells_in_3d_space];
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

    GL_CHECK(glGenBuffers(1, &particle_updater_space_arr_buffer_id));
    GL_CHECK(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, particle_updater_space_arr_buffer_id));
    GL_CHECK(glBufferData(GL_PIXEL_UNPACK_BUFFER, space_cells_in_3d_space*sizeof(GLint), NULL, GL_STREAM_READ));
    GL_CHECK(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0));

    GL_CHECK(glGenBuffers(1, &particle_updater_ordered_arr_buffer_id));
    GL_CHECK(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, particle_updater_ordered_arr_buffer_id));
    GL_CHECK(glBufferData(GL_PIXEL_UNPACK_BUFFER, n_particles*sizeof(GLint), NULL, GL_STREAM_READ));
    GL_CHECK(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0));
    
    Shader::processShader(&particle_updater_pressure_update_vert_shader_id, pressure_update_vert_shader, GL_VERTEX_SHADER);
    Shader::processShader(&particle_updater_overall_update_vert_shader_id, overall_update_vert_shader, GL_VERTEX_SHADER);
    Shader::processShader(&particle_updater_dummy_frag_shader_id, dummy_frag_shader, GL_FRAGMENT_SHADER);
    
    /*Pressure update program */
    particle_updater_pressure_update_program_id = GL_CHECK(glCreateProgram());

    GL_CHECK(glAttachShader(particle_updater_pressure_update_program_id, particle_updater_pressure_update_vert_shader_id));
    GL_CHECK(glAttachShader(particle_updater_pressure_update_program_id, particle_updater_dummy_frag_shader_id));
    
    GL_CHECK(glTransformFeedbackVaryings(particle_updater_pressure_update_program_id, 1, particle_updater_pressure_update_varying_name, GL_INTERLEAVED_ATTRIBS));

    GL_CHECK(glLinkProgram(particle_updater_pressure_update_program_id));
    
    particle_updater_pressure_update_uniform_cells_per_axis_id        = GL_CHECK(glGetUniformLocation(particle_updater_pressure_update_program_id, particle_updater_pressure_update_uniform_cells_per_axis_name      ));
    particle_updater_pressure_update_uniform_cells_in_3d_id           = GL_CHECK(glGetUniformLocation(particle_updater_pressure_update_program_id, particle_updater_pressure_update_uniform_cells_in_3d_name         ));
    particle_updater_pressure_update_uniform_n_particles_id           = GL_CHECK(glGetUniformLocation(particle_updater_pressure_update_program_id, particle_updater_pressure_update_uniform_n_particles_name         ));
    particle_updater_pressure_update_uniform_effect_radius_id         = GL_CHECK(glGetUniformLocation(particle_updater_pressure_update_program_id, particle_updater_pressure_update_uniform_effect_radius_name       ));
    particle_updater_pressure_update_uniform_sampler_space_arr_id     = GL_CHECK(glGetUniformLocation(particle_updater_pressure_update_program_id, particle_updater_pressure_update_uniform_sampler_space_arr_name   ));
    particle_updater_pressure_update_uniform_sampler_ordered_arr_id   = GL_CHECK(glGetUniformLocation(particle_updater_pressure_update_program_id, particle_updater_pressure_update_uniform_sampler_ordered_arr_name ));
    particle_updater_pressure_update_uniform_sampler_position_arr_id  = GL_CHECK(glGetUniformLocation(particle_updater_pressure_update_program_id, particle_updater_pressure_update_uniform_sampler_position_arr_name));
    
    GL_CHECK(glUseProgram(particle_updater_pressure_update_program_id));

    GL_CHECK(glUniform1i(particle_updater_pressure_update_uniform_cells_per_axis_id       ,space_cells_per_axis     ));
    GL_CHECK(glUniform1i(particle_updater_pressure_update_uniform_cells_in_3d_id          ,space_cells_in_3d_space  ));
    GL_CHECK(glUniform1i(particle_updater_pressure_update_uniform_n_particles_id          ,n_particles              ));
    GL_CHECK(glUniform1f(particle_updater_pressure_update_uniform_effect_radius_id        ,effect_radius            ));
    GL_CHECK(glUniform1i(particle_updater_pressure_update_uniform_sampler_space_arr_id    ,8                        ));
    GL_CHECK(glUniform1i(particle_updater_pressure_update_uniform_sampler_ordered_arr_id  ,9                        ));
    GL_CHECK(glUniform1i(particle_updater_pressure_update_uniform_sampler_position_arr_id ,5                        ));

    GL_CHECK(glGenTransformFeedbacks(1, &particle_updater_pressure_update_transform_feedback_object_id));
    GL_CHECK(glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, particle_updater_pressure_update_transform_feedback_object_id));

    GL_CHECK(glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, particle_updater_pressure_buffer_id));
    GL_CHECK(glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0));


    // /*overall update program */
    // particle_updater_overall_update_program_id = GL_CHECK(glCreateProgram());

    // GL_CHECK(glAttachShader(particle_updater_overall_update_program_id, particle_updater_overall_update_vert_shader_id));
    // GL_CHECK(glAttachShader(particle_updater_overall_update_program_id, particle_updater_dummy_frag_shader_id));
    
    // GL_CHECK(glTransformFeedbackVaryings(particle_updater_overall_update_program_id, 2, particle_updater_overall_update_varying_name, GL_INTERLEAVED_ATTRIBS));

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
    GL_CHECK(glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, 1000, n_k_particles));
    Set2DTexParams();

    GL_CHECK(glGenTextures(1, &particle_updater_velocity_texture_object_id));

    GL_CHECK(glActiveTexture(GL_TEXTURE6));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, particle_updater_velocity_texture_object_id));
    GL_CHECK(glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, 1000, n_k_particles));
    Set2DTexParams();

    GL_CHECK(glGenTextures(1, &particle_updater_pressure_texture_object_id));

    GL_CHECK(glActiveTexture(GL_TEXTURE7));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, particle_updater_pressure_texture_object_id));
    GL_CHECK(glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, 1000, n_k_particles));
    Set2DTexParams();
    
    GL_CHECK(glGenTextures(1, &particle_updater_spacial_index_array_texture_object_id));

    GL_CHECK(glActiveTexture(GL_TEXTURE8));
    GL_CHECK(glBindTexture(GL_TEXTURE_3D, particle_updater_spacial_index_array_texture_object_id));
    GL_CHECK(glTexStorage3D(GL_TEXTURE_3D, 1, GL_R32I, space_cells_per_axis, space_cells_per_axis, space_cells_per_axis));
    Set3DTexParams();
 
    GL_CHECK(glGenTextures(1, &particle_updater_ordered_index_array_texture_object_id));

    GL_CHECK(glActiveTexture(GL_TEXTURE9));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, particle_updater_ordered_index_array_texture_object_id));
    GL_CHECK(glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32I, 1000, n_k_particles));
    Set2DTexParams();

}
void InitializeTextures(){
    GL_CHECK(glActiveTexture(GL_TEXTURE8));
    GL_CHECK(glBindBuffer   (GL_PIXEL_UNPACK_BUFFER, particle_updater_space_arr_buffer_id));
    GL_CHECK(glBufferSubData(GL_PIXEL_UNPACK_BUFFER, 0, space_cells_in_3d_space*sizeof(GLint), space_distribution_index));
    GL_CHECK(glTexSubImage3D(GL_TEXTURE_3D,
                             0,
                             0,
                             0,
                             0,
                             space_cells_per_axis,
                             space_cells_per_axis,
                             space_cells_per_axis,
                             GL_RED_INTEGER,
                             GL_INT,
                             NULL
                            ));
    
    GL_CHECK(glActiveTexture(GL_TEXTURE9));
    GL_CHECK(glBindBuffer   (GL_PIXEL_UNPACK_BUFFER, particle_updater_ordered_arr_buffer_id));
    GL_CHECK(glBufferSubData(GL_PIXEL_UNPACK_BUFFER, 0, n_particles*sizeof(GLint), ordered_index));
    GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D,        
                             0,
                             0,
                             0,
                             1000,
                             n_k_particles,
                             GL_RED_INTEGER,
                             GL_INT,
                             NULL
                            ));


    GL_CHECK(glActiveTexture(GL_TEXTURE5));
    GL_CHECK(glBindBuffer   (GL_PIXEL_UNPACK_BUFFER, particle_updater_position_buffer_id));
    GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D,
                             0,
                             0,
                             0,
                             1000,
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
                             1000,
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
                             1000,
                             n_k_particles,
                             GL_R,
                             GL_FLOAT,
                             NULL
                            ));
    
}

void UpdatePositionAndVelocity(){

    // GL_CHECK(glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, particle_updater_overall_update_transform_feedback_object_id));

    // GL_CHECK(glEnable(GL_RASTERIZER_DISCARD));
    // {
    //     GL_CHECK(glUseProgram(particle_updater_overall_update_program_id));

    //     // input time GL_CHECK(glUniform1f(spheres_updater_uniform_time_id, model_time));

    //     GL_CHECK(glBeginTransformFeedback(GL_POINTS));
    //     {
    //         GL_CHECK(glDrawArrays(GL_POINTS, 0, n_particles));
    //     }
    //     GL_CHECK(glEndTransformFeedback());
    // }
    // GL_CHECK(glDisable(GL_RASTERIZER_DISCARD));

    // GL_CHECK(glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0));

    // //load position into array
}

void UpdateParticles(){
    ReorderPerticlesInCells();
    InitializeTextures();
    UpdatePressureValues();
    UpdatePositionAndVelocity();    
}

