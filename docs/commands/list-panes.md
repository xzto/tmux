# list-panes

List panes in a window, session, or across the server.

Alias: `lsp`

## USAGE

```text
tmux list-panes [OPTIONS]
```

## OPTIONS

- `-a` — List panes across all sessions. Ignores the target and takes precedence over `-s`.
- `-s` — List panes in a session. Without `-t`, uses the current session.
- `-t target-window` — Choose the target window. With `-s`, choose a session instead.
- `-F format` — Set each output line's format. See FORMATS for available fields.
- `-f filter` — Filter panes by a format expression. Show only panes for which it evaluates true.
- `-O sort-order` — Choose the sort order. Use `name`, `index`, `size`, `creation`, or `activity`.
- `-r` — Reverse the selected sort order.

## NOTES

Without `-a` or `-s`, lists panes in the target window, or the current window if `-t` is omitted. The default output includes pane index, size, history usage, pane ID, and active or dead status.

## EXAMPLES

List panes in the current window:

```sh
tmux list-panes
```

List panes across the server with their IDs:

```sh
tmux list-panes -a -F '#{session_name}:#{window_index}.#{pane_index} #{pane_id}'
```
