#include <sb7.h>
#include <vmath.h>
#include <shader.h>
#include <object.h>

class sb6mrender_app : public sb7::application
{
	void init();
	virtual void startup();
	virtual void render(double currentTime);
	virtual void shutdown();

private:
	void			load_shaders();
	GLuint          program;
	GLint           mv_location;
	GLint           proj_location;
	GLint           normal_length_location;

	sb7::object     object;
};


void sb6mrender_app::init()
{
	static const char title[] = "Normal Viewer";

	sb7::application::init();

	memcpy(info.title, title, sizeof(title));
}

void sb6mrender_app::startup()
{
	load_shaders();

	object.load("../extern/media/objects/torus.sbm");

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
}

void sb6mrender_app::render(double currentTime)
{
	static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	static const GLfloat one = 1.0f;
	float f = (float)currentTime;

	glViewport(0, 0, info.windowWidth, info.windowHeight);
	glClearBufferfv(GL_COLOR, 0, black);
	glClearBufferfv(GL_DEPTH, 0, &one);

	glUseProgram(program);

	vmath::mat4 proj_matrix = vmath::perspective(50.0f,
		(float)info.windowWidth / (float)info.windowHeight,
		0.1f,
		1000.0f);
	glUniformMatrix4fv(proj_location, 1, GL_FALSE, proj_matrix);

	vmath::mat4 mv_matrix = vmath::translate(0.0f, 0.0f, -3.0f) *
		vmath::rotate((float)currentTime * 15.0f, 0.0f, 1.0f, 0.0f) *
		vmath::rotate((float)currentTime * 21.0f, 1.0f, 0.0f, 0.0f);
	glUniformMatrix4fv(mv_location, 1, GL_FALSE, mv_matrix);

	glUniform1f(normal_length_location, sinf((float)currentTime * 8.0f) * cosf((float)currentTime * 6.0f) * 0.03f + 0.05f);

	object.render();
}

void sb6mrender_app::shutdown()
{
	object.free();
	glDeleteProgram(program);
}

void sb6mrender_app::load_shaders()
{
	GLuint shaders1[3];
	if (program)
		glDeleteProgram(program);
	shaders1[0] = sb7::shader::load("../extern/media/shaders/render.vs.glsl", GL_VERTEX_SHADER);
	shaders1[1] = sb7::shader::load("../extern/media/shaders/render.gs.glsl", GL_GEOMETRY_SHADER);
	shaders1[2] = sb7::shader::load("../extern/media/shaders/render.fs.glsl", GL_FRAGMENT_SHADER);
	program = sb7::program::link_from_shaders(shaders1, 3, true);

	mv_location = glGetUniformLocation(program, "mv_matrix");
	proj_location = glGetUniformLocation(program, "proj_matrix");
	normal_length_location = glGetUniformLocation(program, "normal_length");

}



DECLARE_MAIN(sb6mrender_app)
