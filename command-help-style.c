/* Terminal styling for command help. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "tmux.h"

static int
help_style_enabled(void)
{
	const char	*term, *no_colour;

	if (!isatty(STDOUT_FILENO))
		return (0);
	term = getenv("TERM");
	no_colour = getenv("NO_COLOR");
	return (term != NULL && strcmp(term, "dumb") != 0 &&
	    (no_colour == NULL || *no_colour == '\0'));
}

void
cmd_help_print(const char *text)
{
	const char	*line, *end, *label;
	size_t		 length;
	int		 styled, first = 1, options = 0, commands = 0;

	styled = help_style_enabled();
	if (!styled) {
		fputs(text, stdout);
		return;
	}
	for (line = text; *line != '\0'; line = end + (*end == '\n')) {
		end = strchr(line, '\n');
		if (end == NULL)
			end = line + strlen(line);
		length = end - line;
		if (length == 7 && strncmp(line, "OPTIONS", 7) == 0) {
			options = 1;
			commands = 0;
		} else if ((length == 5 && strncmp(line, "USAGE", 5) == 0) ||
		    (length == 8 && strncmp(line, "EXAMPLES", 8) == 0)) {
			options = 0;
			commands = 1;
		} else if (length == 5 && strncmp(line, "NOTES", 5) == 0) {
			options = 0;
			commands = 0;
		}
		if (first && length != 0) {
			fputs("\033[1m", stdout);
			fwrite(line, 1, length, stdout);
			fputs("\033[0m", stdout);
		} else if ((length == 5 && strncmp(line, "USAGE", 5) == 0) ||
		    (length == 7 && strncmp(line, "OPTIONS", 7) == 0) ||
		    (length == 8 && strncmp(line, "EXAMPLES", 8) == 0) ||
		    (length == 5 && strncmp(line, "NOTES", 5) == 0)) {
			fputs("\033[1;4m", stdout);
			fwrite(line, 1, length, stdout);
			fputs("\033[0m", stdout);
		} else if (options && length > 3 && line[0] == ' ' &&
		    line[1] == ' ' && line[2] == '-') {
			label = line + 2;
			while (label < end &&
			    !(label + 1 < end && label[0] == ' ' && label[1] == ' '))
				label++;
			fwrite(line, 1, 2, stdout);
			fputs("\033[1m", stdout);
			fwrite(line + 2, 1, label - (line + 2), stdout);
			fputs("\033[0m", stdout);
			fwrite(label, 1, end - label, stdout);
		} else if (commands && length >= 6 &&
		    strncmp(line, "  tmux", 6) == 0) {
			fputs("\033[1m", stdout);
			fwrite(line, 1, length, stdout);
			fputs("\033[0m", stdout);
		} else
			fwrite(line, 1, length, stdout);
		if (*end == '\n')
			putchar('\n');
		first = 0;
	}
}
