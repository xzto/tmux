# lock-session

Lock all clients attached to a session.

Alias: `locks`

## USAGE

```text
tmux lock-session [OPTIONS]
```

## OPTIONS

- `-t target-session` — Choose the session whose clients to lock.

## NOTES

The target defaults to the current session. This locks every client attached to the target session.

## EXAMPLES

Lock clients attached to a named session:

```sh
tmux lock-session -t shared-demo
```
