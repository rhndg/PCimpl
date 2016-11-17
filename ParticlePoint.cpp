#include "ShaderCode.h"


GLuint 		  draw_points_program_id        							 = 0;

GLuint        draw_points_frag_shader_id                    			 = 0;
GLuint        draw_points_vert_shader_id                    			 = 0;

const GLchar* draw_points_uniform_mvp_name                  		  	 = "mvp";
GLuint        draw_points_uniform_mvp_id                    		  	 = 0;

GLuint        draw_points_vao_id                            			 = 0;



const GLchar* draw_points_vert_shader = GLSL(
    
    layout (location = 0) in vec3 position;
    layout (location = 1) in vec3 velocity;
    layout (location = 2) in float pressure;

    uniform mat4 mvp;
    out vec3 color;

    void main(){
        vec3 t = position;
        t.x = pressure;
        gl_Position = mvp * vec4(position,1.0);
        color = vec3(0,0,pressure);
        // color = velocity ;
    }
);

const GLchar* draw_points_frag_shader = GLSL(
    
    out vec4 FragColor;
    in vec3 color;

    void main(){
        FragColor = vec4(color,1);
    }
);

void SetUpDrawPoints(){

	draw_points_program_id = GL_CHECK(glCreateProgram());

    Shader::processShader(&draw_points_vert_shader_id, draw_points_vert_shader, GL_VERTEX_SHADER  );
    Shader::processShader(&draw_points_frag_shader_id, draw_points_frag_shader, GL_FRAGMENT_SHADER);

    GL_CHECK(glAttachShader(draw_points_program_id, draw_points_vert_shader_id));
    GL_CHECK(glAttachShader(draw_points_program_id, draw_points_frag_shader_id));

    GL_CHECK(glLinkProgram(draw_points_program_id));

    draw_points_uniform_mvp_id = GL_CHECK(glGetUniformLocation  (draw_points_program_id, draw_points_uniform_mvp_name));
    
    GL_CHECK(glUseProgram(draw_points_program_id));

    GL_CHECK(glUniformMatrix4fv(draw_points_uniform_mvp_id, 1, GL_FALSE, mvp.getAsArray()));

	GL_CHECK(glGenVertexArrays(1, &draw_points_vao_id));

    GL_CHECK(glBindVertexArray(draw_points_vao_id));
    GL_CHECK(glPointSize(3));
}

void DrawPoints(){

	GL_CHECK(glUseProgram(draw_points_program_id));
    GL_CHECK(glEnableVertexAttribArray(0));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, particle_updater_position_buffer_id));
    GL_CHECK(glVertexAttribPointer(
        0,                  // attribute
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    ));
    GL_CHECK(glEnableVertexAttribArray(1));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, particle_updater_velocity_buffer_id));
    GL_CHECK(glVertexAttribPointer(
        1,                  // attribute
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    ));
    GL_CHECK(glEnableVertexAttribArray(2));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, particle_updater_pressure_buffer_id));
    GL_CHECK(glVertexAttribPointer(
        2,                  // attribute
        1,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    ));

	GL_CHECK(glDrawArrays(GL_POINTS, 0, 1000*n_k_particles));    
}