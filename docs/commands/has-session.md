# has-session

Report whether a session exists.

Alias: `has`

## USAGE

```text
tmux has-session [OPTIONS]
```

## OPTIONS

- `-t target-session` — Select the session to check.

## NOTES

The command exits with status 0 if the target session exists. If it does not exist, tmux reports an error and exits with status 1.

## EXAMPLES

Check whether the `work` session exists:

```sh
tmux has-session -t work
```
