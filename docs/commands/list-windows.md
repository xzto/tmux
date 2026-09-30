# list-windows

List windows in a session or across the server.

Alias: `lsw`

## USAGE

```text
tmux list-windows [OPTIONS]
```

## OPTIONS

- `-a` — List windows across all sessions. Ignores `-t`.
- `-t target-session` — Choose the session to list. Without `-a`, defaults to the current session.
- `-F format` — Set each output line's format. See FORMATS for available fields.
- `-f filter` — Filter windows by a format expression. Show only windows for which it evaluates true.
- `-O sort-order` — Choose the sort order. Use `index`, `name`, `size`, `creation`, or `activity`.
- `-r` — Reverse the selected sort order.

## NOTES

By default, lists windows in the current session. The default output for one session includes each window's index, name, pane count, size, layout, and ID. With `-a`, each line also includes the session name.

## EXAMPLES

List windows in the current session:

```sh
tmux list-windows
```

List all windows with their session, index, and name:

```sh
tmux list-windows -a -F '#{session_name}:#{window_index} #{window_name}'
```
