# previous-layout

Move the target window to the previous layout in the session.

Alias: `prevl`

## USAGE

```text
tmux previous-layout [OPTIONS]
```

## OPTIONS

- `-t target-window` — Choose the window whose layout to change.

## NOTES

The command moves the target window to the previous preset layout. If `-t` is omitted, the current window is used.

## EXAMPLES

Move the current window to its previous layout:

```sh
tmux previous-layout
```

Move window 2 to its previous layout:

```sh
tmux previous-layout -t 2
```
