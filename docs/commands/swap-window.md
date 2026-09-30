# swap-window

Swap the positions of two windows.

Alias: `swapw`

## USAGE

```text
tmux swap-window [OPTIONS]
```

## OPTIONS

- `-d` — Do not select the swapped-in window.
- `-s src-window` — Choose the source window to swap.
- `-t dst-window` — Choose the destination window.

## NOTES

The source and destination window links exchange their windows; neither link is moved to a different index. If `-s` is omitted and a marked pane exists, its window is used instead of the current window. Swapping two links to the same window has no effect. Sessions in the same session group cannot be swapped with each other.

## EXAMPLES

Swap two windows in the same session:

```sh
tmux swap-window -s work:2 -t work:4
```

Swap windows between sessions:

```sh
tmux swap-window -s work:2 -t archive:5
```
