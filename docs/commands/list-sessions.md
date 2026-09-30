# list-sessions

List the sessions managed by the server.

Alias: `ls`

## USAGE

```text
tmux list-sessions [OPTIONS]
```

## OPTIONS

- `-F format` — Set the format for each listed session.
- `-f filter` — Show only sessions matching the format filter.
- `-O sort-order` — Set the session sort field.
- `-r` — Reverse the sort order.

## NOTES

By default, each line shows the session name, number of windows, creation time, and whether the session is grouped or attached. `-F` replaces this default with the specified format. `-f` is a format expression evaluated for each session; only sessions for which it is true are listed.

`-O` accepts `index`, `name`, `creation`, or `activity` as its sort field.

## EXAMPLES

List all sessions:

```sh
tmux list-sessions
```

List session names and window counts:

```sh
tmux list-sessions -F '#{session_name}: #{session_windows}'
```
