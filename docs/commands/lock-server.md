# lock-server

Lock every client connected to the server.

Alias: `lock`

## USAGE

```text
tmux lock-server
```

## OPTIONS

None.

## NOTES

Each client is locked by running the command configured by the `lock-command` option.

## EXAMPLES

Lock all clients connected to the current server:

```sh
tmux lock-server
```
