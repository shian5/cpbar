/* See LICENSE file for copyright and license details. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cairo-xcb.h>

#include "options.h"
#include "window.h"
#include "engine.h"
#include "utils.h"

/*!
 * Initializes all modules. After their use, terminate should be called.
 * @param argc argc from the main function.
 * @param argv argv from the main function.
 * @see main()
 * @see terminate()
 * @return true if the initialization has succeeded, otherwise false.
 */
static bool init(int argc, char *argv[])
{
	options_init(argc, argv);
	window_init(options.bar_height, options.bar_on_bottom);

	cairo_surface_t *surface = cairo_xcb_surface_create(
			window.xcb_connection, window.xcb_window,
			window.xcb_visualtype, window.width, window.height);

	if (!engine_init_canvas(surface, window.width, window.height))
		return false;

	engine_init_sets(options.sizes, options.default_font,
			options.default_foreground, options.default_background);

	window_flush();
	return true;
}

/*!
 * Executes the main loop until it receives null input from STDIN.
 Requires all modules to have been initialized.
 * @see init()
 */
static void main_loop()
{
	for (;;) {
		int id;
		char string[BUFSIZ];

		char buffer[BUFSIZ];
		if (fgets(buffer, BUFSIZ, stdin) == NULL)
			break;

		buffer[strlen(buffer) - 1] = '\0';
		if (!parse_input(buffer, &id, string, BUFSIZ))
			continue;

		engine_update(string, id);
		window_flush();
	}
}

/*!
 * Frees resources allocated by init. At the time of writing, not guaranteed
 to succeed if init fails.
 * @see init()
 */
static void terminate()
{
	options_terminate();
	engine_terminate();
	window_terminate();
}

/*!
 * Main function. Calls init, main_loop and terminate.
 * @see init()
 * @see main_loop()
 * @see terminate()
 */
int main(int argc, char *argv[])
{
	atexit(terminate);

	if (init(argc, argv))
		main_loop();
}
