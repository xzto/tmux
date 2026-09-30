# attach-session

Attach to a session or switch the current client to it.

Alias: `attach`

## USAGE

```text
tmux attach-session [OPTIONS]
```

## OPTIONS

- `-c working-directory` — Set the session directory used for new windows.
- `-d` — Detach other clients attached to the session.
- `-x` — Detach other clients and send SIGHUP to parents. This typically causes them to exit.
- `-E` — Skip applying `update-environment` to the session.
- `-f flags` — Set a comma-separated list of client flags.
- `-r` — Make the client read-only. It also prevents this client from affecting other clients' size.
- `-t target-session` — Choose the session to attach to or switch to.

## NOTES

The target session must already exist. When run outside tmux, this attaches it to the current terminal; from inside tmux, it switches the current client to that session. If no server is running, tmux attempts to start one, which succeeds only if the configuration creates a session.

`-d` detaches other clients attached to the target session. `-x` does the same and also sends SIGHUP to each other client's parent process, typically causing it to exit. If `-t` is omitted and tmux must choose the most recently used session, it prefers an unattached session.

`-c` sets the session working directory used for new windows. `-E` prevents `update-environment` from being applied. The client flags accepted by `-f` are `ignore-size`, `new-layouts`, `no-detach-on-destroy`, `no-output`, `pause-after=seconds`, `read-only`, and `wait-exit`; prefix a flag with `!` to turn it off if already set. `ignore-size` makes this client not affect the size of other clients. `-r` is an alias for `-f read-only,ignore-size`.

## EXAMPLES

Attach to the `work` session:

```sh
tmux attach-session -t work
```

Attach to `work` with a read-only client:

```sh
tmux attach-session -r -t work
```
