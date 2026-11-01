/* Server checkpoint format. Bump RESTART_VERSION when its encoding changes. */
#ifndef TMUX_RESTART_H
#define TMUX_RESTART_H

#define RESTART_VERSION 1
#define RESTART_TIMEOUT 5000

struct restart {
	int	 fd;
	off_t	 offset;
	off_t	 size;
	int	 error;
};

extern char *executable_path;
void	 restart_init(const char *);
int	 restart_requested(int, char **);
__dead void restart_main(struct event_base *);
int	 restart_server(char **);
int	 restart_pending(void);
int	 restart_jobs_ready(void);
int	 restart_queue_ready(struct cmdq_item *);
int	 restart_wait_ready(void);
void	 restart_save_wait(struct restart *);
void	 restart_load_wait(struct restart *);
void	 restart_save_paste(struct restart *);
void	 restart_load_paste(struct restart *);
struct cmd_list *restart_parse_command(const char *);
void	 restart_client_attached(struct client *);
void	 restart_write(struct restart *, const void *, size_t);
void	 restart_read(struct restart *, void *, size_t);
void	 restart_put_number(struct restart *, int64_t);
int64_t	 restart_get_number(struct restart *);
void	 restart_put_string(struct restart *, const char *);
char	*restart_get_string(struct restart *);
void	 restart_save_input(struct restart *, struct input_ctx *);
void	 restart_load_input(struct restart *, struct input_ctx *);
int	 restart_input_ready(struct input_ctx *);
void	 restart_save_titles(struct restart *, struct screen *);
void	 restart_load_titles(struct restart *, struct screen *);
void	 restart_save_hyperlinks(struct restart *, struct hyperlinks *);
struct hyperlinks *restart_load_hyperlinks(struct restart *);
void	 restart_save_acl(struct restart *);
void	 restart_load_acl(struct restart *);
int	 server_restart_fd(void);
uint64_t server_restart_flags(void);
void	 server_restart_init(void);
__dead void server_restart_loop(int, uint64_t);
int	 proc_peer_fd(struct tmuxpeer *);
void	 window_restart_ids(struct restart *, int);
void	 window_restart_id(struct window *, u_int);
void	 window_pane_restart_id(struct window_pane *, u_int);

#endif
