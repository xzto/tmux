# lock-client

Lock a client by running its configured lock command.

Alias: `lockc`

## USAGE

```text
tmux lock-client [OPTIONS]
```

## OPTIONS

- `-t target-client` — Select the client to lock.

## NOTES

The client is locked by running the command configured by the `lock-command` option. If `-t` is omitted, the target client is determined from the command context.

## EXAMPLES

Lock the client named `work`:

```sh
tmux lock-client -t work
```
