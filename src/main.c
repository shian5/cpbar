/* See LICENSE file for copyright and license details. */

#include <stdio.h>
#include <stdlib.h>
#include <cairo-xcb.h>

#include "options.h"
#include "window.h"
#include "engine.h"
#include "utils.h"

static bool init(int argc, char *argv[])
{
	options_init(argc, argv);
	window_init(options.bar_height, options.bar_on_bottom);

	cairo_surface_t *surface = cairo_xcb_surface_create(
			window.xcb_connection, window.xcb_window,
			window.xcb_visualtype, window.width, window.height);

	engine_init_canvas(surface, window.width, window.height,
			options.default_foreground, options.default_background);
	engine_init_sets(options.sizes, options.default_font);

	window_flush();
	return true;
}

static void main_loop()
{
	int id;
	char string[BUFSIZ];
	for (;;) {
		char buffer[BUFSIZ];
		if (fgets(buffer, BUFSIZ, stdin) == NULL)
			continue;
		if (!parse_input(buffer, &id, string, BUFSIZ))
			continue;
		puts(string);
		engine_update(string, id);
		window_flush();
	}
}

static void terminate()
{
	options_terminate();
	engine_terminate();
	window_terminate();
}

int main(int argc, char *argv[])
{
	if (init(argc, argv))
		main_loop();
	terminate();
}
