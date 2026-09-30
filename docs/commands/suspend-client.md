# suspend-client

Suspend a client by sending SIGTSTP (tty stop).

Alias: `suspendc`

## USAGE

```text
tmux suspend-client [OPTIONS]
```

## OPTIONS

- `-t target-client` — Select the client to suspend.

## NOTES

If `-t` is omitted, tmux targets the current client.

## EXAMPLES

Suspend the current client:

```sh
tmux suspend-client
```
