#include <sb7.h>
#include <vmath.h>
#include <shader.h>

class blendmatrix_app : public sb7::application
{
	void init();
	virtual void startup();
	virtual void render(double currentTime);
	virtual void shutdown();

private:
	GLuint          program;
	GLuint          vao;
	GLuint          position_buffer;
	GLuint          index_buffer;
	GLint           mv_location;
	GLint           proj_location;

	void load_shaders();
};

void blendmatrix_app::init()
{
	static const char title[] = "Blending Functions";

	sb7::application::init();

	memcpy(info.title, title, sizeof(title));
}

void blendmatrix_app::startup()
{
	load_shaders();

	mv_location = glGetUniformLocation(program, "mv_matrix");
	proj_location = glGetUniformLocation(program, "proj_matrix");

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	static const GLushort vertex_indices[] =
	{
		0, 1, 2,
		2, 1, 3,
		2, 3, 4,
		4, 3, 5,
		4, 5, 6,
		6, 5, 7,
		6, 7, 0,
		0, 7, 1,
		6, 0, 2,
		2, 4, 6,
		7, 5, 3,
		7, 3, 1
	};

	static const GLfloat vertex_positions[] =
	{
		-0.25f, -0.25f, -0.25f,
		-0.25f,  0.25f, -0.25f,
		0.25f, -0.25f, -0.25f,
		0.25f,  0.25f, -0.25f,
		0.25f, -0.25f,  0.25f,
		0.25f,  0.25f,  0.25f,
		-0.25f, -0.25f,  0.25f,
		-0.25f,  0.25f,  0.25f,
	};

	glGenBuffers(1, &position_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions), vertex_positions, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertex_indices), vertex_indices, GL_STATIC_DRAW);

	glEnable(GL_CULL_FACE);
	// glFrontFace(GL_CW);

	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);
}

void blendmatrix_app::render(double currentTime)
{
	int i, j;
	static const GLfloat orange[] = { 0.6f, 0.4f, 0.1f, 1.0f };
	static const GLfloat one = 1.0f;

	static const GLenum blend_func[] =
	{
		GL_ZERO,
		GL_ONE,
		GL_SRC_COLOR,
		GL_ONE_MINUS_SRC_COLOR,
		GL_DST_COLOR,
		GL_ONE_MINUS_DST_COLOR,
		GL_SRC_ALPHA,
		GL_ONE_MINUS_SRC_ALPHA,
		GL_DST_ALPHA,
		GL_ONE_MINUS_DST_ALPHA,
		GL_CONSTANT_COLOR,
		GL_ONE_MINUS_CONSTANT_COLOR,
		GL_CONSTANT_ALPHA,
		GL_ONE_MINUS_CONSTANT_ALPHA,
		GL_SRC_ALPHA_SATURATE,
		GL_SRC1_COLOR,
		GL_ONE_MINUS_SRC1_COLOR,
		GL_SRC1_ALPHA,
		GL_ONE_MINUS_SRC1_ALPHA
	};
	static const int num_blend_funcs = sizeof(blend_func) / sizeof(blend_func[0]);
	static const float x_scale = 20.0f / float(num_blend_funcs);
	static const float y_scale = 16.0f / float(num_blend_funcs);
	const float t = (float)currentTime;

	glViewport(0, 0, info.windowWidth, info.windowHeight);
	glClearBufferfv(GL_COLOR, 0, orange);
	glClearBufferfv(GL_DEPTH, 0, &one);

	glUseProgram(program);

	vmath::mat4 proj_matrix = vmath::perspective(50.0f,
		(float)info.windowWidth / (float)info.windowHeight,
		0.1f,
		1000.0f);
	glUniformMatrix4fv(proj_location, 1, GL_FALSE, proj_matrix);

	glEnable(GL_BLEND);
	glBlendColor(0.2f, 0.5f, 0.7f, 0.5f);
	for (j = 0; j < num_blend_funcs; j++)
	{
		for (i = 0; i < num_blend_funcs; i++)
		{
			vmath::mat4 mv_matrix =
				vmath::translate(9.5f - x_scale * float(i),
					7.5f - y_scale * float(j),
					-18.0f) *
				vmath::rotate(t * -45.0f, 0.0f, 1.0f, 0.0f) *
				vmath::rotate(t * -21.0f, 1.0f, 0.0f, 0.0f);
			glUniformMatrix4fv(mv_location, 1, GL_FALSE, mv_matrix);
			glBlendFunc(blend_func[i], blend_func[j]);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
		}
	}
}

void blendmatrix_app::shutdown()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteProgram(program);
	glDeleteBuffers(1, &position_buffer);
}

void blendmatrix_app::load_shaders()
{
	GLuint shaders[2];

	shaders[0] = sb7::shader::load("../extern/media/shaders/render.vs.glsl", GL_VERTEX_SHADER);
	shaders[1] = sb7::shader::load("../extern/media/shaders/render.fs.glsl", GL_FRAGMENT_SHADER);

	program = sb7::program::link_from_shaders(shaders, 2, true);

}


DECLARE_MAIN(blendmatrix_app)
