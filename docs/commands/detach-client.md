# detach-client

Detach a client from its session.

Alias: `detach`

## USAGE

```text
tmux detach-client [OPTIONS]
```

## OPTIONS

- `-a` — Detach all clients except the target client.
- `-E shell-command` — Replace the client with `shell-command`.
- `-P` — Send SIGHUP to the client's parent process.
- `-s target-session` — Detach clients attached to this session.
- `-t target-client` — Select the client to detach.

## NOTES

When bound to a key, the command detaches the current client by default. Use `-t` to select a client. `-s` selects a session and detaches its attached clients; it takes precedence over `-a` if both are given. Without `-s`, `-a` detaches all clients except the selected target client. With `-E`, tmux runs `shell-command` to replace the client instead of detaching it. `-P` sends SIGHUP to the parent process of a client being detached, typically causing it to exit.

## EXAMPLES

Detach the current client:

```sh
tmux detach-client
```
