# start-server

Start the tmux server without creating a session.

Alias: `start`

## USAGE

```text
tmux start-server
```

## OPTIONS

None.

## NOTES

The server starts only if it is not already running. By default, a server exits when it has no sessions, so this command is useful on its own only when the configuration creates a session, `exit-empty` is disabled, or another command runs in the same command sequence.

## EXAMPLES

Start the server and then show global options:

```sh
tmux start-server \; show -g
```
