#include <sb7.h>
#include <vmath.h>
#include <shader.h>
#include <cmath>

class gstessellate_app : public sb7::application
{
	void init();
	virtual void startup();
	virtual void render(double currentTime);
	virtual void shutdown();

private:
	void			load_shaders();
	GLuint          program;
	GLint           mv_location;
	GLint           mvp_location;
	GLint           stretch_location;
	GLuint          vao;
	GLuint          buffer;
};


void gstessellate_app::init()
{
	static const char title[] = "Geometry Shader Tessellation";

	sb7::application::init();

	memcpy(info.title, title, sizeof(title));
}

void gstessellate_app::startup()
{
	load_shaders();

	static const GLfloat tetrahedron_verts[] =
	{
		0.000f,  0.000f,  1.000f,
		0.943f,  0.000f, -0.333f,
		-0.471f,  0.816f, -0.333f,
		-0.471f, -0.816f, -0.333f
	};

	static const GLushort tetrahedron_indices[] =
	{
		0, 1, 2,
		0, 2, 3,
		0, 3, 1,
		3, 2, 1
	};

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tetrahedron_verts) + sizeof(tetrahedron_indices), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(tetrahedron_indices), tetrahedron_indices);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tetrahedron_indices), sizeof(tetrahedron_verts), tetrahedron_verts);

	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)sizeof(tetrahedron_indices));
	glEnableVertexAttribArray(0);

	glEnable(GL_CULL_FACE);
	// glDisable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
}

void gstessellate_app::render(double currentTime)
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
	vmath::mat4 mv_matrix = vmath::translate(0.0f, 0.0f, -8.0f) *
		vmath::rotate((float)currentTime * 71.0f, 0.0f, 1.0f, 0.0f) *
		vmath::rotate((float)currentTime * 10.0f, 1.0f, 0.0f, 0.0f);

	glUniformMatrix4fv(mvp_location, 1, GL_FALSE, proj_matrix * mv_matrix);

	glUniformMatrix4fv(mv_location, 1, GL_FALSE, mv_matrix);

	glUniform1f(stretch_location, sinf(f * 4.0f) * 0.75f + 1.0f);

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_SHORT, NULL);
}

void gstessellate_app::shutdown()
{
	glDeleteProgram(program);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &buffer);
}

void gstessellate_app::load_shaders()
{
	GLuint shaders1[3];
	if (program)
		glDeleteProgram(program);
	shaders1[0] = sb7::shader::load("../extern/media/shaders/render.vs.glsl", GL_VERTEX_SHADER);
	shaders1[1] = sb7::shader::load("../extern/media/shaders/render.gs.glsl", GL_GEOMETRY_SHADER);
	shaders1[2] = sb7::shader::load("../extern/media/shaders/render.fs.glsl", GL_FRAGMENT_SHADER);
	program = sb7::program::link_from_shaders(shaders1, 3, true);


	mv_location = glGetUniformLocation(program, "mvMatrix");
	mvp_location = glGetUniformLocation(program, "mvpMatrix");
	stretch_location = glGetUniformLocation(program, "stretch");

}


DECLARE_MAIN(gstessellate_app)
