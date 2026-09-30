# rename-session

Rename a session.

Alias: `rename`

## USAGE

```text
tmux rename-session [OPTIONS] new-name
```

## OPTIONS

- `-t target-session` — Choose the session to rename.

## NOTES

The target defaults to the current session. `new-name` must be a valid, unused session name.

## EXAMPLES

Rename a session:

```sh
tmux rename-session -t work work-old
```
