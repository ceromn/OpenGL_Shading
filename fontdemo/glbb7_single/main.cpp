#include <sb7.h>
#include <sb7ktx.h>
#include <sb7textoverlay.h>
#include <sb7color.h>

class fontdemo_app : public sb7::application
{
	void init();
	void startup();
	void render(double currentTime);
	void shutdown();

protected:
	sb7::text_overlay    text_overlay;
};


void fontdemo_app::init()
{
	static const char title[] = "Bitmap Font Rendering";

	sb7::application::init();

	memcpy(info.title, title, sizeof(title));
}

void fontdemo_app::startup()
{

	static const char string[] = "This a test. Can you hear me?";
	static const char string2[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*();:<>,./?~`'\"";
	static const char string3[] = "The quick brown fox jumped over the lazy dog.";

	// _text_overlay.init(48, 32, "media/textures/font16x16.ktx");
	text_overlay.init(64, 32, "../extern/media/textures/cp437_9x16.ktx");
	text_overlay.clear();
	text_overlay.print("This is a demo of bitmap font rendering.\n\n"
		"This was printed as one string with newlines.\n\n"
		"If you have a really, really, really, really, really, really, really, really, really, really long string like this one, it will wrap.\n\n"
		"The final text buffer is composited over whatever was drawn into the framebuffer below.\n\n\n\n\n"
		"It's not all that pretty, but it's fast:");
	text_overlay.print("\n\n\n\n");
	text_overlay.print(string2);
	text_overlay.print("\n");
	text_overlay.print(string3);
}

void fontdemo_app::render(double currentTime)
{
	static int frame = 0;

	char buffer[1024];
	frame++;
	if ((frame & 0x1FF) == 0x100)
	{
		sprintf(buffer, "%d frames in %lf secs = %3.3f fps", frame, currentTime, (float)frame / (float)currentTime);
		text_overlay.drawText(buffer, 0, 16);
	}

	glClearBufferfv(GL_COLOR, 0, sb7::color::Green);

	glViewport(0, 0, info.windowWidth, info.windowHeight);

	text_overlay.draw();
}

void fontdemo_app::shutdown()
{
	text_overlay.teardown();
}



DECLARE_MAIN(fontdemo_app)
