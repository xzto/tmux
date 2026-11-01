/*
 * Copyright (c) 2026 tmux contributors
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "tmux.h"
#include "restart.h"

char *executable_path;

static int restart_state_fd = -1;
static int restart_check_fd = -1;
static int restart_binary_fd = -1;
static int *restart_fds;
static u_int restart_nfds;

struct restart_window_size {
	u_int		 id;
	u_int		 sx;
	u_int		 sy;
};

struct restart_client {
	int		 fd;
	int64_t		 session;
	uint64_t	 flags;
	pid_t		 pid;
	char		*flagstr;
	char		*features;
	u_int		 sx;
	u_int		 sy;
	u_int		 xpixel;
	u_int		 ypixel;
	struct restart_window_size *windows;
	u_int		 nwindows;
};

static struct restart_client *restart_clients;
static u_int restart_nclients;
static struct event restart_timer;

int
restart_pending(void)
{
	u_int i;
	int pending = 0;

	for (i = 0; i < restart_nclients; i++) {
		if (restart_clients[i].pid == 0)
			continue;
		if (kill(restart_clients[i].pid, 0) == -1 && errno == ESRCH)
			restart_clients[i].pid = 0;
		else
			pending++;
	}
	return (pending);
}

void
restart_client_attached(struct client *c)
{
	struct restart_client		*rc;
	struct restart_window_size	*rw;
	u_int				 i, j;

	if (c->session == NULL)
		return;
	for (i = 0; i < restart_nclients; i++) {
		rc = &restart_clients[i];
		if (rc->pid != c->pid ||
		    rc->session != (int64_t)c->session->id)
			continue;
		if (c->flags & CLIENT_CONTROL) {
			tty_set_size(&c->tty, rc->sx, rc->sy,
			    rc->xpixel, rc->ypixel);
			c->flags |= rc->flags &
			    (CLIENT_SIZECHANGED|CLIENT_WINDOWSIZECHANGED);
			for (j = 0; j < rc->nwindows; j++) {
				rw = &rc->windows[j];
				control_set_window_size(c, rw->id, rw->sx, rw->sy);
			}
		}
		rc->pid = 0;
	}
}

static void
restart_timer_cb(__unused int fd, __unused short events, __unused void *arg)
{
	struct timeval tv = { .tv_sec = 1 };
	u_int i;

	if (restart_pending())
		evtimer_add(&restart_timer, &tv);
	else {
		for (i = 0; i < restart_nclients; i++)
			free(restart_clients[i].windows);
		free(restart_clients);
		restart_clients = NULL;
		restart_nclients = 0;
		server_check_unattached();
	}
}

void
restart_init(const char *argv0)
{
	const char	*cwd = find_cwd(), *path;
	char		*copy, *next, *cp, *candidate;

	if (cwd == NULL)
		cwd = "/";
	if (strchr(argv0, '/') != NULL) {
		if (*argv0 == '/')
			executable_path = xstrdup(argv0);
		else
			xasprintf(&executable_path, "%s/%s", cwd, argv0);
		return;
	}
	path = getenv("PATH");
	if (path == NULL)
		path = "/bin:/usr/bin";
	copy = cp = xstrdup(path);
	while ((next = strsep(&cp, ":")) != NULL) {
		if (*next == '/')
			xasprintf(&candidate, "%s/%s", next, argv0);
		else if (*next == '\0')
			xasprintf(&candidate, "%s/%s", cwd, argv0);
		else
			xasprintf(&candidate, "%s/%s/%s", cwd, next, argv0);
		if (access(candidate, X_OK) == 0) {
			executable_path = candidate;
			break;
		}
		free(candidate);
	}
	free(copy);
}

int
restart_requested(int argc, char **argv)
{
	const char	*errstr;

	if (argc != 5 || strcmp(argv[1], "--tmux-restart") != 0)
		return (0);
	restart_state_fd = strtonum(argv[2], 3, INT_MAX, &errstr);
	if (errstr != NULL)
		errx(1, "invalid restart descriptor");
	restart_check_fd = strtonum(argv[3], -1, INT_MAX, &errstr);
	if (errstr != NULL)
		errx(1, "invalid restart check descriptor");
	restart_binary_fd = strtonum(argv[4], 3, INT_MAX, &errstr);
	if (errstr != NULL)
		errx(1, "invalid restart executable descriptor");
	return (1);
}

void
restart_write(struct restart *r, const void *data, size_t size)
{
	const char	*p = data;
	ssize_t		 n;

	while (size != 0 && r->error == 0) {
		n = pwrite(r->fd, p, size, r->offset);
		if (n == -1 && errno == EINTR)
			continue;
		if (n <= 0) {
			r->error = n == 0 ? EIO : errno;
			break;
		}
		r->offset += n;
		p += n;
		size -= n;
	}
}

void
restart_read(struct restart *r, void *data, size_t size)
{
	char	*p = data;
	ssize_t	 n;

	if (r->offset > r->size || size > (uint64_t)(r->size - r->offset))
		fatalx("truncated restart checkpoint");
	while (size != 0) {
		n = pread(r->fd, p, size, r->offset);
		if (n == -1 && errno == EINTR)
			continue;
		if (n <= 0)
			fatalx("cannot read restart checkpoint");
		r->offset += n;
		p += n;
		size -= n;
	}
}

void
restart_put_number(struct restart *r, int64_t n)
{
	restart_write(r, &n, sizeof n);
}

int64_t
restart_get_number(struct restart *r)
{
	int64_t n;

	restart_read(r, &n, sizeof n);
	return (n);
}

void
restart_put_string(struct restart *r, const char *s)
{
	size_t size;

	if (s == NULL) {
		restart_put_number(r, -1);
		return;
	}
	size = strlen(s);
	restart_put_number(r, size);
	restart_write(r, s, size);
}

char *
restart_get_string(struct restart *r)
{
	int64_t	 size = restart_get_number(r);
	char	*s;

	if (size == -1)
		return (NULL);
	if (size < 0 || size > r->size - r->offset ||
	    (uint64_t)size > SIZE_MAX - 1)
		fatalx("invalid restart string");
	s = xmalloc(size + 1);
	restart_read(r, s, size);
	s[size] = '\0';
	return (s);
}

static void
restart_keep_fd(int fd)
{
	if (fd == -1)
		return;
	restart_fds = xreallocarray(restart_fds, restart_nfds + 1,
	    sizeof *restart_fds);
	restart_fds[restart_nfds++] = fd;
}

static void
restart_save_buffer(struct restart *r, struct evbuffer *evb)
{
	size_t size = evb == NULL ? 0 : EVBUFFER_LENGTH(evb);

	restart_put_number(r, size);
	if (size != 0)
		restart_write(r, EVBUFFER_DATA(evb), size);
}

static void
restart_load_buffer(struct restart *r, struct evbuffer *evb)
{
	int64_t	 size = restart_get_number(r);
	void	*data;

	if (size < 0 || size > r->size - r->offset)
		fatalx("invalid restart buffer");
	if (size == 0)
		return;
	data = xmalloc(size);
	restart_read(r, data, size);
	if (evbuffer_add(evb, data, size) != 0)
		fatalx("out of memory");
	free(data);
}

struct cmd_list *
restart_parse_command(const char *s)
{
	struct cmd_parse_input	 pi = { .flags = CMD_PARSE_NOALIAS };
	struct cmd_parse_result	*pr;

	pr = cmd_parse_from_string(s, &pi);
	if (pr->status != CMD_PARSE_SUCCESS)
		fatalx("restart command: %s", pr->error);
	return (pr->cmdlist);
}

static void
restart_save_options(struct restart *r, struct options *oo)
{
	struct options_entry		*o;
	struct options_array_item	*a;
	const struct options_table_entry	*oe;
	char				*value;

	for (o = options_first(oo); o != NULL; o = options_next(o)) {
		if (options_get_monitor_data(o) != NULL) {
			r->error = ENOTSUP;
			return;
		}
		restart_put_string(r, options_name(o));
		oe = options_table_entry(o);
		restart_put_number(r, oe == NULL ? -1 : (int)oe->type);
		restart_put_number(r, options_is_array(o));
		if (options_is_array(o)) {
			for (a = options_array_first(o); a != NULL;
			    a = options_array_next(a)) {
				restart_put_string(r,
				    options_array_item_key(a));
				value = options_to_string(o,
				    options_array_item_key(a), 0);
				restart_put_string(r, value);
				free(value);
			}
			restart_put_string(r, NULL);
		} else if (oe == NULL || oe->type == OPTIONS_TABLE_STRING ||
		    oe->type == OPTIONS_TABLE_COMMAND) {
			value = options_to_string(o, NULL, 0);
			restart_put_string(r, value);
			free(value);
		} else
			restart_put_number(r, options_get_number(oo,
			    options_name(o)));
	}
	restart_put_string(r, NULL);
}

static void
restart_load_options(struct restart *r, struct options *oo)
{
	const struct options_table_entry	*oe;
	struct options_entry		*o;
	char				*name, *key, *value, *cause;
	int				 type, array;

	while ((name = restart_get_string(r)) != NULL) {
		type = restart_get_number(r);
		array = restart_get_number(r);
		oe = *name == '@' ? NULL : options_search(name);
		if ((*name != '@' && oe == NULL) ||
		    (oe != NULL && ((int)oe->type != type ||
		    !!(oe->flags & OPTIONS_TABLE_IS_ARRAY) != !!array)))
			fatalx("incompatible restart option: %s", name);
		if (array) {
			o = options_empty(oo, oe);
			while ((key = restart_get_string(r)) != NULL) {
				value = restart_get_string(r);
				if (options_array_set(o, key, value, 0,
				    &cause) != 0)
					fatalx("restart option %s: %s", name,
					    cause);
				free(value);
				free(key);
			}
		} else if (oe == NULL || type == OPTIONS_TABLE_STRING) {
			value = restart_get_string(r);
			options_set_string(oo, name, 0, "%s", value);
			free(value);
		} else if (type == OPTIONS_TABLE_COMMAND) {
			value = restart_get_string(r);
			options_set_command(oo, name, *value == '\0' ? NULL :
			    restart_parse_command(value));
			free(value);
		} else
			options_set_number(oo, name, restart_get_number(r));
		free(name);
	}
}

static void
restart_save_environ(struct restart *r, struct environ *env)
{
	struct environ_entry *envent;

	for (envent = environ_first(env); envent != NULL;
	    envent = environ_next(envent)) {
		restart_put_string(r, envent->name);
		restart_put_string(r, envent->value);
		restart_put_number(r, envent->flags);
	}
	restart_put_string(r, NULL);
}

static struct environ *
restart_load_environ(struct restart *r)
{
	struct environ	*env = environ_create();
	char		*name, *value;
	int		 flags;

	while ((name = restart_get_string(r)) != NULL) {
		value = restart_get_string(r);
		flags = restart_get_number(r);
		if (value == NULL)
			environ_clear(env, name);
		else
			environ_set(env, name, flags, "%s", value);
		free(name);
		free(value);
	}
	return (env);
}

static void
restart_save_grid(struct restart *r, struct grid *gd)
{
	struct grid_line	*gl;
	struct grid_cell	*cells;
	u_int			 x, y;

	restart_put_number(r, gd != NULL);
	if (gd == NULL)
		return;
	restart_put_number(r, gd->flags);
	restart_put_number(r, gd->sx);
	restart_put_number(r, gd->sy);
	restart_put_number(r, gd->hsize);
	restart_put_number(r, gd->hlimit);
	restart_put_number(r, gd->hscrolled);
	for (y = 0; y < gd->hsize + gd->sy; y++) {
		gl = &gd->linedata[y];
		restart_put_number(r, gl->flags);
		restart_put_number(r, gl->time);
		restart_write(r, &gl->osc133_data, sizeof gl->osc133_data);
		restart_put_number(r, gl->cellsize);
		restart_put_number(r, gl->cellused);
		cells = gl->cellsize == 0 ? NULL :
		    xcalloc(gl->cellsize, sizeof *cells);
		for (x = 0; x < gl->cellsize; x++)
			grid_get_cell(gd, x, y, &cells[x]);
		restart_write(r, cells, gl->cellsize * sizeof *cells);
		free(cells);
	}
}

static struct grid *
restart_load_grid(struct restart *r)
{
	struct grid		*gd;
	struct grid_line	*gl;
	struct grid_cell	*cells;
	u_int			 sx, sy, hsize, hlimit, flags, x, y, used, size;

	if (!restart_get_number(r))
		return (NULL);
	flags = restart_get_number(r);
	sx = restart_get_number(r);
	sy = restart_get_number(r);
	hsize = restart_get_number(r);
	hlimit = restart_get_number(r);
	if (sx == 0 || sy == 0 || sx > USHRT_MAX || sy > USHRT_MAX ||
	    hsize > UINT_MAX - sy)
		fatalx("invalid restart grid");
	gd = grid_create(sx, sy, hlimit);
	gd->flags = flags;
	gd->hsize = hsize;
	gd->hscrolled = restart_get_number(r);
	grid_adjust_lines(gd, hsize + sy);
	memset(gd->linedata, 0, (hsize + sy) * sizeof *gd->linedata);
	for (y = 0; y < hsize + sy; y++) {
		gl = &gd->linedata[y];
		flags = restart_get_number(r);
		gl->time = restart_get_number(r);
		restart_read(r, &gl->osc133_data, sizeof gl->osc133_data);
		size = restart_get_number(r);
		used = restart_get_number(r);
		if (size > USHRT_MAX || used > size)
			fatalx("invalid restart grid line");
		cells = size == 0 ? NULL : xcalloc(size, sizeof *cells);
		restart_read(r, cells, size * sizeof *cells);
		for (x = 0; x < size; x++)
			grid_set_cell(gd, x, y, &cells[x]);
		gl->cellused = used;
		gl->flags = flags;
		free(cells);
	}
	return (gd);
}

static void
restart_save_screen(struct restart *r, struct screen *s)
{
	u_int x;

	restart_put_string(r, s->title);
	restart_put_string(r, s->path);
	restart_save_titles(r, s);
	restart_save_grid(r, s->grid);
	restart_save_grid(r, s->saved_grid);
	restart_put_number(r, s->cx);
	restart_put_number(r, s->cy);
	restart_put_number(r, s->cstyle);
	restart_put_number(r, s->default_cstyle);
	restart_put_number(r, s->ccolour);
	restart_put_number(r, s->default_ccolour);
	restart_put_number(r, s->rupper);
	restart_put_number(r, s->rlower);
	restart_put_number(r, s->mode);
	restart_put_number(r, s->default_mode);
	restart_put_number(r, s->saved_cx);
	restart_put_number(r, s->saved_cy);
	restart_write(r, &s->saved_cell, sizeof s->saved_cell);
	restart_put_number(r, s->saved_flags);
	for (x = 0; x < s->grid->sx; x++)
		restart_put_number(r, bit_test(s->tabs, x) != 0);
	restart_write(r, &s->progress_bar, sizeof s->progress_bar);
	restart_save_hyperlinks(r, s->hyperlinks);
}

static void
restart_load_screen(struct restart *r, struct screen *s)
{
	u_int x;

	screen_free(s);
	memset(s, 0, sizeof *s);
	s->title = restart_get_string(r);
	s->path = restart_get_string(r);
	restart_load_titles(r, s);
	s->grid = restart_load_grid(r);
	s->saved_grid = restart_load_grid(r);
	s->cx = restart_get_number(r);
	s->cy = restart_get_number(r);
	s->cstyle = restart_get_number(r);
	s->default_cstyle = restart_get_number(r);
	s->ccolour = restart_get_number(r);
	s->default_ccolour = restart_get_number(r);
	s->rupper = restart_get_number(r);
	s->rlower = restart_get_number(r);
	s->mode = restart_get_number(r);
	s->default_mode = restart_get_number(r);
	s->saved_cx = restart_get_number(r);
	s->saved_cy = restart_get_number(r);
	restart_read(r, &s->saved_cell, sizeof s->saved_cell);
	s->saved_flags = restart_get_number(r);
	s->tabs = bit_alloc(s->grid->sx);
	if (s->tabs == NULL)
		fatalx("out of memory");
	for (x = 0; x < s->grid->sx; x++) {
		if (restart_get_number(r))
			bit_set(s->tabs, x);
	}
	restart_read(r, &s->progress_bar, sizeof s->progress_bar);
	s->hyperlinks = restart_load_hyperlinks(r);
}

static void
restart_save_layout(struct restart *r, struct layout_cell *lc)
{
	struct layout_cell *child;

	restart_put_number(r, lc != NULL);
	if (lc == NULL)
		return;
	restart_put_number(r, lc->type);
	restart_put_number(r, lc->flags);
	restart_write(r, &lc->g, sizeof lc->g);
	restart_write(r, &lc->fg, sizeof lc->fg);
	restart_put_number(r, lc->wp == NULL ? -1 : (int64_t)lc->wp->id);
	TAILQ_FOREACH(child, &lc->cells, entry)
		restart_save_layout(r, child);
	restart_put_number(r, 0);
}

static struct layout_cell *
restart_load_layout(struct restart *r, struct layout_cell *parent, int saved)
{
	struct layout_cell *lc, *child;
	int64_t id;

	if (!restart_get_number(r))
		return (NULL);
	lc = layout_create_cell(parent);
	lc->type = restart_get_number(r);
	lc->flags = restart_get_number(r);
	restart_read(r, &lc->g, sizeof lc->g);
	restart_read(r, &lc->fg, sizeof lc->fg);
	id = restart_get_number(r);
	if (id != -1) {
		lc->wp = window_pane_find_by_id(id);
		if (lc->wp == NULL)
			fatalx("missing restart layout pane");
		if (saved)
			lc->wp->saved_layout_cell = lc;
		else
			lc->wp->layout_cell = lc;
	}
	while ((child = restart_load_layout(r, lc, saved)) != NULL)
		TAILQ_INSERT_TAIL(&lc->cells, child, entry);
	return (lc);
}

static void
restart_save_pane(struct restart *r, struct window_pane *wp)
{
	struct window_pane_resize *resize;
	int i;

	restart_put_number(r, wp->id);
	restart_put_number(r, wp->active_point);
	restart_put_number(r, wp->sx);
	restart_put_number(r, wp->sy);
	TAILQ_FOREACH(resize, &wp->resize_queue, entry) {
		restart_put_number(r, 1);
		restart_put_number(r, resize->sx);
		restart_put_number(r, resize->sy);
		restart_put_number(r, resize->osx);
		restart_put_number(r, resize->osy);
	}
	restart_put_number(r, 0);
	restart_put_number(r, wp->xoff);
	restart_put_number(r, wp->yoff);
	restart_put_number(r, wp->flags);
	restart_put_number(r, wp->pid);
	restart_put_number(r, wp->fd);
	restart_keep_fd(wp->fd);
	restart_put_number(r, wp->status);
	restart_write(r, &wp->dead_time, sizeof wp->dead_time);
	restart_put_number(r, wp->output_generation);
	restart_put_number(r, wp->last_output_time);
	restart_put_number(r, wp->last_prompt_time);
	restart_put_number(r, wp->cmd_start_time);
	restart_put_number(r, wp->cmd_end_time);
	restart_put_number(r, wp->cmd_status);
	restart_put_string(r, wp->cmd_line);
	restart_put_number(r, wp->control_bg);
	restart_put_number(r, wp->control_fg);
	restart_put_string(r, wp->tty);
	restart_put_string(r, wp->shell);
	restart_put_string(r, wp->cwd);
	restart_put_number(r, wp->argc);
	for (i = 0; i < wp->argc; i++)
		restart_put_string(r, wp->argv[i]);
	restart_save_options(r, wp->options);
	restart_save_screen(r, &wp->base);
	restart_put_number(r, wp->ictx != NULL);
	if (wp->ictx != NULL)
		restart_save_input(r, wp->ictx);
	restart_save_buffer(r, wp->event == NULL ? NULL : wp->event->input);
	restart_save_buffer(r, wp->event == NULL ? NULL : wp->event->output);
	restart_put_number(r, wp->base_offset);
	restart_write(r, &wp->offset, sizeof wp->offset);
	restart_put_number(r, wp->palette.fg);
	restart_put_number(r, wp->palette.bg);
	restart_put_number(r, wp->palette.palette != NULL);
	if (wp->palette.palette != NULL)
		restart_write(r, wp->palette.palette, 256 * sizeof(int));
}

static struct window_pane *
restart_load_pane(struct restart *r, struct window *w)
{
	struct window_pane	*wp;
	struct window_pane_resize *resize;
	char			*tty;
	u_int			 id;
	int			 i, fd;

	id = restart_get_number(r);
	wp = window_add_pane(w, TAILQ_LAST(&w->panes, window_panes), 0, 0);
	window_pane_restart_id(wp, id);
	wp->active_point = restart_get_number(r);
	wp->sx = restart_get_number(r);
	wp->sy = restart_get_number(r);
	while (restart_get_number(r)) {
		resize = xcalloc(1, sizeof *resize);
		resize->sx = restart_get_number(r);
		resize->sy = restart_get_number(r);
		resize->osx = restart_get_number(r);
		resize->osy = restart_get_number(r);
		TAILQ_INSERT_TAIL(&wp->resize_queue, resize, entry);
	}
	wp->xoff = restart_get_number(r);
	wp->yoff = restart_get_number(r);
	wp->flags = restart_get_number(r) | PANE_REDRAW | PANE_STYLECHANGED;
	wp->pid = restart_get_number(r);
	fd = restart_get_number(r);
	wp->status = restart_get_number(r);
	restart_read(r, &wp->dead_time, sizeof wp->dead_time);
	wp->output_generation = restart_get_number(r);
	wp->last_output_time = restart_get_number(r);
	wp->last_prompt_time = restart_get_number(r);
	wp->cmd_start_time = restart_get_number(r);
	wp->cmd_end_time = restart_get_number(r);
	wp->cmd_status = restart_get_number(r);
	wp->cmd_line = restart_get_string(r);
	wp->control_bg = restart_get_number(r);
	wp->control_fg = restart_get_number(r);
	tty = restart_get_string(r);
	strlcpy(wp->tty, tty, sizeof wp->tty);
	free(tty);
	wp->shell = restart_get_string(r);
	wp->cwd = restart_get_string(r);
	wp->argc = restart_get_number(r);
	wp->argv = xcalloc(wp->argc + 1, sizeof *wp->argv);
	for (i = 0; i < wp->argc; i++)
		wp->argv[i] = restart_get_string(r);
	restart_load_options(r, wp->options);
	style_set_scrollbar_style_from_option(&wp->scrollbar_style, wp->options);
	colour_palette_from_option(&wp->palette, wp->options);
	restart_load_screen(r, &wp->base);
	if (wp->base.mode & MODE_SYNC)
		screen_write_start_sync(wp);
	wp->fd = fd;
	if (fd != -1) {
		if (fcntl(fd, F_GETFD) == -1 || !isatty(fd))
			fatalx("invalid restart pty");
		window_pane_set_event(wp);
	}
	if (restart_get_number(r)) {
		if (wp->ictx == NULL)
			wp->ictx = input_init(wp, &wp->palette);
		restart_load_input(r, wp->ictx);
	}
	if (wp->event != NULL) {
		restart_load_buffer(r, wp->event->input);
		restart_load_buffer(r, wp->event->output);
	} else if (restart_get_number(r) != 0 || restart_get_number(r) != 0)
		fatalx("buffer for closed restart pane");
	wp->base_offset = restart_get_number(r);
	restart_read(r, &wp->offset, sizeof wp->offset);
	wp->palette.fg = restart_get_number(r);
	wp->palette.bg = restart_get_number(r);
	if (restart_get_number(r)) {
		wp->palette.palette = xreallocarray(wp->palette.palette,
		    256, sizeof(int));
		restart_read(r, wp->palette.palette, 256 * sizeof(int));
	}
	return (wp);
}

static void
restart_save_windows(struct restart *r)
{
	struct window		*w;
	struct window_pane	*wp;

	RB_FOREACH(w, windows, &windows) {
		restart_put_number(r, 1);
		restart_put_number(r, w->id);
		restart_put_string(r, w->name);
		restart_put_number(r, w->sx);
		restart_put_number(r, w->sy);
		restart_put_number(r, w->xpixel);
		restart_put_number(r, w->ypixel);
		restart_put_number(r, w->manual_sx);
		restart_put_number(r, w->manual_sy);
		restart_put_number(r, w->flags);
		restart_put_number(r, w->lastlayout);
		restart_write(r, &w->creation_time, sizeof w->creation_time);
		restart_write(r, &w->activity_time, sizeof w->activity_time);
		restart_save_options(r, w->options);
		TAILQ_FOREACH(wp, &w->panes, entry) {
			restart_put_number(r, 1);
			restart_save_pane(r, wp);
		}
		restart_put_number(r, 0);
		restart_put_number(r, w->active->id);
		TAILQ_FOREACH(wp, &w->last_panes, sentry)
			restart_put_number(r, wp->id);
		restart_put_number(r, -1);
		TAILQ_FOREACH(wp, &w->z_index, zentry)
			restart_put_number(r, wp->id);
		restart_put_number(r, -1);
		restart_save_layout(r, w->layout_root);
		restart_save_layout(r, w->saved_layout_root);
	}
	restart_put_number(r, 0);
}

static void
restart_load_windows(struct restart *r)
{
	struct window		*w;
	struct window_pane	*wp;
	u_int			 id, sx, sy, xp, yp;
	int64_t			 pane;

	while (restart_get_number(r)) {
		id = restart_get_number(r);
		/* Constructors must see the restored global options. */
		w = window_create(80, 24, 0, 0);
		window_restart_id(w, id);
		free(w->name);
		w->name = restart_get_string(r);
		sx = restart_get_number(r);
		sy = restart_get_number(r);
		xp = restart_get_number(r);
		yp = restart_get_number(r);
		w->sx = sx;
		w->sy = sy;
		w->xpixel = xp;
		w->ypixel = yp;
		w->manual_sx = restart_get_number(r);
		w->manual_sy = restart_get_number(r);
		w->flags = restart_get_number(r);
		w->lastlayout = restart_get_number(r);
		restart_read(r, &w->creation_time, sizeof w->creation_time);
		restart_read(r, &w->activity_time, sizeof w->activity_time);
		restart_load_options(r, w->options);
		w->sb = options_get_number(w->options, "pane-scrollbars");
		w->sb_pos = options_get_number(w->options,
		    "pane-scrollbars-position");
		while (restart_get_number(r))
			restart_load_pane(r, w);
		w->active = window_pane_find_by_id(restart_get_number(r));
		while ((pane = restart_get_number(r)) != -1) {
			wp = window_pane_find_by_id(pane);
			TAILQ_INSERT_TAIL(&w->last_panes, wp, sentry);
		}
		TAILQ_INIT(&w->z_index);
		while ((pane = restart_get_number(r)) != -1) {
			wp = window_pane_find_by_id(pane);
			TAILQ_INSERT_TAIL(&w->z_index, wp, zentry);
		}
		w->layout_root = restart_load_layout(r, NULL, 0);
		w->saved_layout_root = restart_load_layout(r, NULL, 1);
	}
}

static void
restart_save_sessions(struct restart *r)
{
	struct session		*s;
	struct session_group	*sg;
	struct winlink		*wl;

	RB_FOREACH(s, sessions, &sessions) {
		restart_put_string(r, s->name);
		restart_put_number(r, s->id);
		restart_put_string(r, s->cwd);
		restart_write(r, &s->creation_time, sizeof s->creation_time);
		restart_write(r, &s->last_attached_time,
		    sizeof s->last_attached_time);
		restart_write(r, &s->activity_time, sizeof s->activity_time);
		restart_write(r, &s->last_activity_time,
		    sizeof s->last_activity_time);
		restart_put_number(r, s->flags);
		restart_put_number(r, s->tio != NULL);
		if (s->tio != NULL)
			restart_write(r, s->tio, sizeof *s->tio);
		restart_save_options(r, s->options);
		restart_save_environ(r, s->environ);
		RB_FOREACH(wl, winlinks, &s->windows) {
			restart_put_number(r, wl->idx);
			restart_put_number(r, wl->window->id);
			restart_put_number(r, wl->flags);
		}
		restart_put_number(r, -1);
		restart_put_number(r, s->curw->idx);
		TAILQ_FOREACH(wl, &s->lastw, sentry)
			restart_put_number(r, wl->idx);
		restart_put_number(r, -1);
	}
	restart_put_string(r, NULL);
	restart_put_number(r, next_session_id);
	RB_FOREACH(sg, session_groups, &session_groups) {
		restart_put_string(r, sg->name);
		TAILQ_FOREACH(s, &sg->sessions, gentry)
			restart_put_number(r, s->id);
		restart_put_number(r, -1);
	}
	restart_put_string(r, NULL);
}

static void
restart_load_sessions(struct restart *r)
{
	struct session		*s;
	struct session_group	*sg;
	struct options		*oo;
	struct environ		*env;
	struct winlink		*wl;
	struct window		*w;
	struct termios		 tio;
	struct timeval		 created, attached, activity, last_activity;
	char			*name, *cwd;
	int			 id, idx, have_tio, flags;

	while ((name = restart_get_string(r)) != NULL) {
		id = restart_get_number(r);
		cwd = restart_get_string(r);
		restart_read(r, &created, sizeof created);
		restart_read(r, &attached, sizeof attached);
		restart_read(r, &activity, sizeof activity);
		restart_read(r, &last_activity, sizeof last_activity);
		flags = restart_get_number(r);
		have_tio = restart_get_number(r);
		if (have_tio)
			restart_read(r, &tio, sizeof tio);
		oo = options_create(global_s_options);
		restart_load_options(r, oo);
		env = restart_load_environ(r);
		s = session_create(NULL, name, cwd, env, oo,
		    have_tio ? &tio : NULL);
		s->id = id;
		s->creation_time = created;
		s->last_attached_time = attached;
		s->activity_time = activity;
		s->last_activity_time = last_activity;
		s->flags = flags;
		free(name);
		free(cwd);
		while ((idx = restart_get_number(r)) != -1) {
			w = window_find_by_id(restart_get_number(r));
			if (w == NULL)
				fatalx("missing restart window");
			wl = winlink_add(&s->windows, idx);
			if (wl == NULL)
				fatalx("duplicate restart window");
			wl->session = s;
			winlink_set_window(wl, w);
			wl->flags = restart_get_number(r);
		}
		s->curw = winlink_find_by_index(&s->windows,
		    restart_get_number(r));
		if (s->curw == NULL)
			fatalx("missing restart current window");
		while ((idx = restart_get_number(r)) != -1) {
			wl = winlink_find_by_index(&s->windows, idx);
			TAILQ_INSERT_TAIL(&s->lastw, wl, sentry);
		}
	}
	next_session_id = restart_get_number(r);
	while ((name = restart_get_string(r)) != NULL) {
		sg = session_group_new(name);
		free(name);
		while ((id = restart_get_number(r)) != -1) {
			s = session_find_by_id(id);
			if (s == NULL)
				fatalx("missing restart grouped session");
			session_group_add(sg, s);
		}
	}
}

static void
restart_save(struct restart *r)
{
	struct client		*c;
	struct window		*w;
	u_int			 i, sx, sy;

	restart_put_string(r, "tmux restart");
	restart_put_number(r, RESTART_VERSION);
	restart_put_number(r, PROTOCOL_VERSION);
	restart_put_number(r, sizeof(struct grid_cell));
	restart_put_number(r, sizeof(struct timeval));
	restart_put_number(r, sizeof(struct termios));
	restart_put_number(r, sizeof(struct layout_geometry));
	restart_put_number(r, sizeof(struct window_pane_offset));
	restart_put_number(r, sizeof(struct osc133_data));
	restart_put_number(r, sizeof(struct progress_bar));
	restart_put_number(r, sizeof(struct msg_restart));
	restart_put_number(r, sizeof(size_t));
	restart_put_string(r, socket_path);
	restart_put_string(r, executable_path);
	restart_put_number(r, server_restart_fd());
	restart_keep_fd(server_restart_fd());
	restart_put_number(r, server_restart_flags());
	restart_put_number(r, log_get_level());
	restart_write(r, &start_time, sizeof start_time);
	restart_put_number(r, cfg_quiet);
	for (i = 0; i < cfg_nfiles; i++)
		restart_put_string(r, cfg_files[i]);
	restart_put_string(r, NULL);
	restart_save_environ(r, global_environ);
	restart_save_wait(r);
	restart_save_acl(r);
	restart_save_windows(r);
	window_restart_ids(r, 1);
	restart_save_sessions(r);
	restart_put_number(r, server_check_marked());
	if (server_check_marked()) {
		restart_put_number(r, marked_pane.s->id);
		restart_put_number(r, marked_pane.wl->idx);
		restart_put_number(r, marked_pane.wp->id);
	}
	restart_save_paste(r);
	TAILQ_FOREACH(c, &clients, entry) {
		restart_put_number(r, 1);
		restart_put_number(r, proc_peer_fd(c->peer));
		restart_keep_fd(proc_peer_fd(c->peer));
		restart_put_number(r, c->session == NULL ? -1 :
		    (int64_t)c->session->id);
		restart_put_number(r, c->flags);
		restart_put_number(r, c->session == NULL ? 0 : c->pid);
		restart_put_string(r, server_client_get_flags(c));
		restart_put_string(r, tty_get_features(c->term_features));
		restart_put_number(r, c->tty.sx);
		restart_put_number(r, c->tty.sy);
		restart_put_number(r, c->tty.xpixel);
		restart_put_number(r, c->tty.ypixel);
		RB_FOREACH(w, windows, &windows) {
			if (!control_get_window_size(c, w->id, &sx, &sy))
				continue;
			restart_put_number(r, w->id);
			restart_put_number(r, sx);
			restart_put_number(r, sy);
		}
		restart_put_number(r, -1);
	}
	restart_put_number(r, 0);
	restart_put_string(r, "end restart");
}

static void
restart_peer_ignore(__unused struct imsg *imsg, __unused void *arg)
{
}

static enum cmd_retval
restart_ready(__unused struct cmdq_item *item, __unused void *data)
{
	struct restart_client	*rc = restart_clients;
	struct tmuxpeer		*peer;
	struct msg_restart	 msg;
	struct timeval		 tv = { .tv_sec = 1 };
	u_int			 i;
	int			 retval = 0;

	alerts_reset_all();
	for (i = 0; i < restart_nclients; i++) {
		peer = proc_add_peer(server_proc, rc[i].fd,
		    restart_peer_ignore, NULL);
		if (rc[i].session != -1) {
			memset(&msg, 0, sizeof msg);
			msg.session = rc[i].session;
			msg.flags = rc[i].flags;
			strlcpy(msg.path, executable_path, sizeof msg.path);
			strlcpy(msg.client_flags, rc[i].flagstr,
			    sizeof msg.client_flags);
			strlcpy(msg.features, rc[i].features,
			    sizeof msg.features);
			proc_send(peer, MSG_RESTART, dup(restart_binary_fd),
			    &msg, sizeof msg);
		} else {
			proc_send(peer, MSG_EXIT, -1, &retval, sizeof retval);
			proc_send(peer, MSG_EXITED, -1, NULL, 0);
		}
		proc_flush_peer(peer);
		proc_remove_peer(peer);
		free(rc[i].flagstr);
		free(rc[i].features);
	}
	close(restart_binary_fd);
	restart_binary_fd = -1;
	evtimer_add(&restart_timer, &tv);
	return (CMD_RETURN_NORMAL);
}

void
restart_main(__unused struct event_base *base)
{
	struct restart		 r = { .fd = restart_state_fd };
	struct stat		 sb;
	struct restart_client	*rc = NULL;
	struct restart_window_size *rw;
	char			*s;
	u_int			 nclients = 0, i;
	int64_t			 window;
	int			 fd, level, idx, listening;
	socklen_t		 listening_size = sizeof listening;
	char			 executable_header[2];
	struct session		*marked_session;
	struct window_pane	*marked_wp;
	uint64_t		 flags;

	if (fstat(r.fd, &sb) != 0 || !S_ISREG(sb.st_mode) ||
	    sb.st_uid != getuid() || (sb.st_mode & 077) != 0)
		fatalx("invalid restart checkpoint file");
	if (pread(restart_binary_fd, executable_header, 2, 0) != 2 ||
	    memcmp(executable_header, "#!", 2) == 0)
		fatalx("restart requires a native tmux executable");
	r.size = sb.st_size;
	s = restart_get_string(&r);
	if (s == NULL || strcmp(s, "tmux restart") != 0 ||
	    restart_get_number(&r) != RESTART_VERSION ||
	    restart_get_number(&r) != PROTOCOL_VERSION ||
	    restart_get_number(&r) != sizeof(struct grid_cell) ||
	    restart_get_number(&r) != sizeof(struct timeval) ||
	    restart_get_number(&r) != sizeof(struct termios) ||
	    restart_get_number(&r) != sizeof(struct layout_geometry) ||
	    restart_get_number(&r) != sizeof(struct window_pane_offset) ||
	    restart_get_number(&r) != sizeof(struct osc133_data) ||
	    restart_get_number(&r) != sizeof(struct progress_bar) ||
	    restart_get_number(&r) != sizeof(struct msg_restart) ||
	    restart_get_number(&r) != sizeof(size_t))
		fatalx("incompatible restart checkpoint");
	free(s);
	socket_path = restart_get_string(&r);
	free(executable_path);
	executable_path = restart_get_string(&r);
	fd = restart_get_number(&r);
	flags = restart_get_number(&r);
	if (flags & CLIENT_NOFORK)
		options_set_number(global_options, "exit-empty", 0);
	level = restart_get_number(&r);
	while (log_get_level() < level)
		log_add_level();
	if (getsockopt(fd, SOL_SOCKET, SO_ACCEPTCONN, &listening,
	    &listening_size) != 0 || !listening)
		fatalx("invalid restart listener");
	server_restart_init();
	restart_read(&r, &start_time, sizeof start_time);
	for (i = 0; i < cfg_nfiles; i++)
		free(cfg_files[i]);
	free(cfg_files);
	cfg_files = NULL;
	cfg_nfiles = 0;
	cfg_quiet = restart_get_number(&r);
	while ((s = restart_get_string(&r)) != NULL) {
		cfg_files = xreallocarray(cfg_files, cfg_nfiles + 1,
		    sizeof *cfg_files);
		cfg_files[cfg_nfiles++] = s;
	}
	environ_free(global_environ);
	global_environ = restart_load_environ(&r);
	restart_load_wait(&r);
	restart_load_acl(&r);
	restart_load_windows(&r);
	window_restart_ids(&r, 0);
	restart_load_sessions(&r);
	if (restart_get_number(&r)) {
		marked_session = session_find_by_id(restart_get_number(&r));
		idx = restart_get_number(&r);
		marked_wp = window_pane_find_by_id(restart_get_number(&r));
		if (marked_session == NULL || marked_wp == NULL)
			fatalx("missing restart marked pane");
		server_set_marked(marked_session,
		    winlink_find_by_index(&marked_session->windows, idx),
		    marked_wp);
	}
	restart_load_paste(&r);
	while (restart_get_number(&r)) {
		rc = xreallocarray(rc, nclients + 1, sizeof *rc);
		memset(&rc[nclients], 0, sizeof *rc);
		rc[nclients].fd = restart_get_number(&r);
		rc[nclients].session = restart_get_number(&r);
		rc[nclients].flags = restart_get_number(&r);
		rc[nclients].pid = restart_get_number(&r);
		rc[nclients].flagstr = restart_get_string(&r);
		rc[nclients].features = restart_get_string(&r);
		rc[nclients].sx = restart_get_number(&r);
		rc[nclients].sy = restart_get_number(&r);
		rc[nclients].xpixel = restart_get_number(&r);
		rc[nclients].ypixel = restart_get_number(&r);
		while ((window = restart_get_number(&r)) != -1) {
			rc[nclients].windows = xreallocarray(rc[nclients].windows,
			    rc[nclients].nwindows + 1, sizeof *rw);
			rw = &rc[nclients].windows[rc[nclients].nwindows++];
			rw->id = window;
			rw->sx = restart_get_number(&r);
			rw->sy = restart_get_number(&r);
		}
		if (fcntl(rc[nclients].fd, F_GETFD) == -1)
			fatalx("invalid restart client");
		nclients++;
	}
	s = restart_get_string(&r);
	if (s == NULL || strcmp(s, "end restart") != 0 || r.offset != r.size)
		fatalx("invalid restart checkpoint trailer");
	free(s);
	close(r.fd);
	if (restart_check_fd != -1) {
		if (write(restart_check_fd, "R", 1) != 1)
			_exit(1);
		_exit(0);
	}
	restart_clients = rc;
	restart_nclients = nclients;
	evtimer_set(&restart_timer, restart_timer_cb, NULL);
	key_bindings_init();
	start_cfg_no_client();
	cmdq_append(NULL, cmdq_get_callback(restart_ready, NULL));
	server_restart_loop(fd, flags);
}

struct restart_fdflags {
	int fd;
	int flags;
};

static int
restart_exec(int binary, int state, int check)
{
#ifdef HAVE_FEXECVE
	struct restart_fdflags	*saved = NULL;
	u_int			 nsaved = 0, i, j;
	DIR			*dir;
	struct dirent		*de;
	const char		*errstr;
	char			 statearg[32], checkarg[32], binaryarg[32];
	char			*argv[6];
	int			 fd, maxfd, flags, keep, error;

	/* Keep only checkpoint resources across exec; retain rollback flags. */
	dir = opendir("/proc/self/fd");
	if (dir == NULL)
		dir = opendir("/dev/fd");
	maxfd = dir == NULL ? getdtablesize() : 0;
	for (fd = 3; dir != NULL || fd < maxfd; fd++) {
		if (dir != NULL) {
			if ((de = readdir(dir)) == NULL)
				break;
			fd = strtonum(de->d_name, 3, INT_MAX, &errstr);
			if (errstr != NULL || fd == dirfd(dir))
				continue;
		}
		if ((flags = fcntl(fd, F_GETFD)) == -1)
			continue;
		keep = fd == binary || fd == state || fd == check;
		for (j = 0; !keep && j < restart_nfds; j++)
			keep = restart_fds[j] == fd;
		saved = xreallocarray(saved, nsaved + 1, sizeof *saved);
		saved[nsaved].fd = fd;
		saved[nsaved++].flags = flags;
		if (fcntl(fd, F_SETFD, keep ? flags & ~FD_CLOEXEC :
		    flags | FD_CLOEXEC) == -1)
			goto fail;
	}
	if (dir != NULL) {
		closedir(dir);
		dir = NULL;
	}
	xsnprintf(statearg, sizeof statearg, "%d", state);
	xsnprintf(checkarg, sizeof checkarg, "%d", check);
	xsnprintf(binaryarg, sizeof binaryarg, "%d", binary);
	argv[0] = executable_path;
	argv[1] = (char *)"--tmux-restart";
	argv[2] = statearg;
	argv[3] = checkarg;
	argv[4] = binaryarg;
	argv[5] = NULL;
	fexecve(binary, argv, environ);
fail:
	error = errno;
	if (dir != NULL)
		closedir(dir);
	for (i = 0; i < nsaved; i++)
		fcntl(saved[i].fd, F_SETFD, saved[i].flags);
	free(saved);
	errno = error;
#else
	(void)binary;
	(void)state;
	(void)check;
	errno = ENOSYS;
#endif
	return (-1);
}

static int
restart_check(int binary, int state, char **cause)
{
	int		 pair[2], status = 0, ready = 0, result = -1;
	pid_t		 pid, found;
	struct pollfd	 pfd;
	uint64_t	 deadline;
	char		 byte;

	if (pipe(pair) != 0) {
		xasprintf(cause, "restart pipe: %s", strerror(errno));
		return (-1);
	}
	setblocking(pair[0], 0);
	pid = fork();
	if (pid == -1) {
		xasprintf(cause, "restart fork: %s", strerror(errno));
		close(pair[0]);
		close(pair[1]);
		return (-1);
	}
	if (pid == 0) {
		setpgid(0, 0);
		close(pair[0]);
		restart_exec(binary, state, pair[1]);
		_exit(1);
	}
	setpgid(pid, pid);
	close(pair[1]);
	pfd.fd = pair[0];
	pfd.events = POLLIN;
	deadline = get_timer() + RESTART_TIMEOUT;
	for (;;) {
		found = waitpid(pid, &status, WNOHANG);
		if (found == -1 && errno != EINTR)
			break;
		if (found == pid) {
			/* exit(0) alone is not a readiness handshake. */
			if (!ready && read(pair[0], &byte, 1) == 1)
				ready = byte == 'R';
			if (WIFEXITED(status) && WEXITSTATUS(status) == 0 &&
			    ready)
				result = 0;
			break;
		}
		if (get_timer() >= deadline)
			break;
		if (poll(&pfd, 1, 10) > 0 && (pfd.revents & POLLIN)) {
			if (read(pair[0], &byte, 1) == 1)
				ready = byte == 'R';
		}
	}
	if (result != 0)
		kill(-pid, SIGKILL);
	if (found != pid) {
		kill(pid, SIGKILL);
		while (waitpid(pid, &status, 0) == -1 && errno == EINTR)
			/* nothing */;
	}
	close(pair[0]);
	if (result != 0)
		xasprintf(cause, "new tmux failed restart sanity check: %s",
		    executable_path);
	return (result);
}

int
restart_server(char **cause)
{
	struct restart		 r = { .fd = -1 };
	struct window		*w;
	struct window_pane	*wp;
	struct client		*c;
	char			 template[] = "/tmp/tmux-restart-XXXXXXXXXX";
	int			 binary = -1, error;

#ifndef HAVE_FEXECVE
	*cause = xstrdup("restart-server requires fexecve");
	return (-1);
#endif
	if (executable_path == NULL || strlen(executable_path) >= PATH_MAX) {
		*cause = xstrdup("cannot find logical tmux executable path");
		return (-1);
	}
	if (restart_pending()) {
		*cause = xstrdup("previous client restart "
		    "is still in progress");
		return (-1);
	}
	if (!cfg_finished || !restart_jobs_ready()) {
		*cause = xstrdup("cannot restart while configuration "
		    "or jobs are running");
		return (-1);
	}
	TAILQ_FOREACH(c, &clients, entry) {
		if (c->session != NULL && (!(c->flags & CLIENT_CAN_RESTART) ||
		    (c->flags & (CLIENT_SUSPENDED|CLIENT_EXIT)))) {
			*cause = xstrdup("attached client cannot restart "
			    "(old, suspended or exiting)");
			return (-1);
		}
		if (!(c->flags & CLIENT_IDENTIFIED) || !RB_EMPTY(&c->files) ||
		    c->prompt != NULL) {
			*cause = xstrdup("cannot restart during client "
			    "identification or file transfer");
			return (-1);
		}
	}
	RB_FOREACH(w, windows, &windows) {
		if (w->menu != NULL || w->modal != NULL) {
			*cause = xstrdup("close window menus and modal panes "
			    "before restarting");
			return (-1);
		}
		TAILQ_FOREACH(wp, &w->panes, entry) {
#ifdef ENABLE_IMAGES
			if (wp->base.grid->images != NULL ||
			    (wp->base.saved_grid != NULL &&
			    wp->base.saved_grid->images != NULL)) {
				*cause = xstrdup("cannot restart with "
				    "pane images");
				return (-1);
			}
#endif
			if (wp->pipe_fd != -1 || wp->wait_item != NULL ||
			    wp->editor != NULL || !TAILQ_EMPTY(&wp->modes) ||
			    wp->prompt != NULL ||
			    !restart_input_ready(wp->ictx)) {
				xasprintf(cause, "pane %%%u is busy "
				    "(mode, pipe or input request)", wp->id);
				return (-1);
			}
		}
	}
	binary = open(executable_path, O_RDONLY);
	if (binary == -1) {
		xasprintf(cause, "cannot open new tmux %s: %s", executable_path,
		    strerror(errno));
		return (-1);
	}
	r.fd = mkstemp(template);
	if (r.fd == -1) {
		xasprintf(cause, "restart checkpoint: %s", strerror(errno));
		goto fail;
	}
	unlink(template);
	restart_save(&r);
	if (r.error != 0) {
		if (r.error == ENOTSUP)
			*cause = xstrdup("cannot restart with active "
			    "format monitors");
		else
			xasprintf(cause, "restart checkpoint: %s",
			    strerror(r.error));
		goto fail;
	}
	if (restart_check(binary, r.fd, cause) != 0)
		goto fail;
	/* The probe must not touch terminals or consume pane output. */
	TAILQ_FOREACH(c, &clients, entry) {
		proc_flush_peer(c->peer);
		if (c->flags & CLIENT_TERMINAL)
			tty_stop_tty(&c->tty);
	}
	restart_exec(binary, r.fd, -1);
	error = errno;
	TAILQ_FOREACH(c, &clients, entry) {
		if ((c->flags & CLIENT_TERMINAL) && c->session != NULL) {
			tty_start_tty(&c->tty);
			server_redraw_client(c);
		}
	}
	xasprintf(cause, "restart exec: %s", strerror(error));
fail:
	if (r.fd != -1)
		close(r.fd);
	close(binary);
	free(restart_fds);
	restart_fds = NULL;
	restart_nfds = 0;
	return (-1);
}
