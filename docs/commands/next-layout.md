# next-layout

Move a window to the next layout and rearrange its panes to fit.

Alias: `nextl`

## USAGE

```text
tmux next-layout [OPTIONS]
```

## OPTIONS

- `-t target-window` — Choose the window whose layout to change.

## NOTES

The command advances the target window to the next preset layout. If `-t` is omitted, the current window is used. `next-layout` is bound to Space by default.

## EXAMPLES

Advance the current window to its next layout:

```sh
tmux next-layout
```

Advance a specific window to its next layout:

```sh
tmux next-layout -t 2
```
