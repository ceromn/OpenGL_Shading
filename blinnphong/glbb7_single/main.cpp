#include <sb7.h>
#include <vmath.h>

#include <object.h>
#include <sb7ktx.h>
#include <shader.h>

#define MANY_OBJECTS

class blinnphong_app : public sb7::application
{
public:
	blinnphong_app() : per_fragment_program(0), per_vertex(false) {}

protected:
	void init();
	void startup();
	void render(double currentTime);
	void onKey(int key, int action);

	void load_shaders();

	GLuint          per_fragment_program;

	struct
	{
		GLuint      color;
		GLuint      normals;
	} textures;

	struct uniforms_block
	{
		vmath::mat4     mv_matrix;
		vmath::mat4     view_matrix;
		vmath::mat4     proj_matrix;
	};

	GLuint          uniforms_buffer;

	struct
	{
		GLint           diffuse_albedo;
		GLint           specular_albedo;
		GLint           specular_power;
	} uniforms[2];

	sb7::object     object;
	bool            per_vertex;

};

void blinnphong_app::init()
{
	sb7::application::init();

	static const char title[] = "Blinn-Phong Shading";
	memcpy(info.title, title, sizeof(title));
}

void blinnphong_app::startup()
{
	load_shaders();

	glGenBuffers(1, &uniforms_buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, uniforms_buffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(uniforms_block), NULL, GL_DYNAMIC_DRAW);

	object.load("../extern/media/objects/sphere.sbm");

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
}

void blinnphong_app::render(double currentTime)
{
	static const GLfloat zeros[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	static const GLfloat gray[] = { 0.1f, 0.1f, 0.1f, 0.0f };
	static const GLfloat ones[] = { 1.0f };
	const float f = (float)currentTime;

	glUseProgram(per_fragment_program);
	glViewport(0, 0, info.windowWidth, info.windowHeight);

	glClearBufferfv(GL_COLOR, 0, gray);
	glClearBufferfv(GL_DEPTH, 0, ones);

	/*
	vmath::mat4 model_matrix = vmath::rotate((float)currentTime * 14.5f, 0.0f, 1.0f, 0.0f) *
	vmath::rotate(180.0f, 0.0f, 0.0f, 1.0f) *
	vmath::rotate(20.0f, 1.0f, 0.0f, 0.0f);
	*/

	vmath::vec3 view_position = vmath::vec3(0.0f, 0.0f, 50.0f);
	vmath::mat4 view_matrix = vmath::lookat(view_position, vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec3(0.0f, 1.0f, 0.0f));

	vmath::vec3 light_position = vmath::vec3(-20.0f, -20.0f, 0.0f);

	vmath::mat4 light_proj_matrix = vmath::frustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 200.0f);
	vmath::mat4 light_view_matrix = vmath::lookat(light_position,
		vmath::vec3(0.0f), vmath::vec3(0.0f, 1.0f, 0.0f));

#if defined(MANY_OBJECTS)
	int i, j;

	for (j = 0; j < 7; j++)
	{
		for (i = 0; i < 7; i++)
		{
			glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniforms_buffer);
			uniforms_block * block = (uniforms_block *)glMapBufferRange(GL_UNIFORM_BUFFER,
				0,
				sizeof(uniforms_block),
				GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

			vmath::mat4 model_matrix = vmath::translate((float)i * 2.75f - 8.25f, 6.75f - (float)j * 2.25f, 0.0f);

			block->mv_matrix = view_matrix * model_matrix;
			block->view_matrix = view_matrix;
			block->proj_matrix = vmath::perspective(50.0f,
				(float)info.windowWidth / (float)info.windowHeight,
				0.1f,
				1000.0f);

			glUnmapBuffer(GL_UNIFORM_BUFFER);

			glUniform1f(uniforms[per_vertex ? 1 : 0].specular_power, powf(2.0f, (float)j + 2.0f));
			glUniform3fv(uniforms[per_vertex ? 1 : 0].specular_albedo, 1, vmath::vec3((float)i / 9.0f + 1.0f / 9.0f));

			object.render();
		}
	}
#else
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniforms_buffer);
	uniforms_block * block = (uniforms_block *)glMapBufferRange(GL_UNIFORM_BUFFER,
		0,
		sizeof(uniforms_block),
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

	vmath::mat4 model_matrix = vmath::scale(7.0f);

	block->mv_matrix = view_matrix * model_matrix;
	block->view_matrix = view_matrix;
	block->proj_matrix = vmath::perspective(50.0f,
		(float)info.windowWidth / (float)info.windowHeight,
		0.1f,
		1000.0f);

	glUnmapBuffer(GL_UNIFORM_BUFFER);

	glUniform1f(uniforms[0].specular_power, 30.0f);
	glUniform3fv(uniforms[0].specular_albedo, 1, vmath::vec3(1.0f));

	object.render();
#endif
}

void blinnphong_app::onKey(int key, int action)
{
	if (action)
	{
		switch (key)
		{
		case 'R':
			load_shaders();
			break;
		}
	}
}

void blinnphong_app::load_shaders()
{
	GLuint shaders[2];

	shaders[0] = sb7::shader::load("../extern/media/shaders/blinnphong/blinnphong.vs.glsl", GL_VERTEX_SHADER);
	shaders[1] = sb7::shader::load("../extern/media/shaders/blinnphong/blinnphong.fs.glsl", GL_FRAGMENT_SHADER);

	per_fragment_program = sb7::program::link_from_shaders(shaders, 2, true);


	uniforms[0].diffuse_albedo = glGetUniformLocation(per_fragment_program, "diffuse_albedo");
	uniforms[0].specular_albedo = glGetUniformLocation(per_fragment_program, "specular_albedo");
	uniforms[0].specular_power = glGetUniformLocation(per_fragment_program, "specular_power");
}

DECLARE_MAIN(blinnphong_app)
