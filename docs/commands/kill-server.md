# kill-server

Kill the tmux server, its clients, and all its sessions.

## USAGE

```text
tmux kill-server
```

## OPTIONS

None.

## NOTES

This terminates every session on the selected server and detaches its clients. Use a separate server socket when testing this command.

## EXAMPLES

Create a temporary server on a distinct socket, then stop only that server:

```sh
tmux -L "docs-kill-demo-$$" new-session -d
tmux -L "docs-kill-demo-$$" kill-server
```
