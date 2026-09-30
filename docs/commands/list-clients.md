# list-clients

List clients attached to the tmux server.

Alias: `lsc`

## USAGE

```text
tmux list-clients [OPTIONS]
```

## OPTIONS

- `-F format` — Set each output line's format. See FORMATS for available fields.
- `-f filter` — Filter clients by a format expression. Show only clients for which it evaluates true.
- `-O sort-order` — Choose the sort order. Use `name`, `size`, `creation`, or `activity`.
- `-r` — Reverse the selected sort order.
- `-t target-session` — List only clients in this session.

## NOTES

By default, lists every client attached to the server. The default output includes each client's name, session, size, terminal name, remote user (when applicable), and client flags.

## EXAMPLES

List attached clients:

```sh
tmux list-clients
```

List clients in a session, showing their names and terminal types:

```sh
tmux list-clients -t work -F '#{client_name} #{client_termname}'
```
