# resize-window

Increase or decrease a window's size.

Alias: `resizew`

## USAGE

```text
tmux resize-window [OPTIONS] [adjustment]
```

## OPTIONS

- `-a` — Use the smallest session size for this window.
- `-A` — Use the largest session size for this window.
- `-D` — Increase the window's height by the adjustment.
- `-U` — Decrease the window's height by the adjustment.
- `-L` — Decrease the window's width by the adjustment.
- `-R` — Increase the window's width by the adjustment.
- `-t target-window` — Choose the window to resize.
- `-x width` — Set the absolute window width.
- `-y height` — Set the absolute window height.

## NOTES

`adjustment` is a positive number of lines or columns and defaults to 1. `-x` and `-y` set absolute dimensions; `-D`, `-U`, `-L`, and `-R` adjust height or width by `adjustment`. `-a` and `-A` set the window to the smallest or largest size of a session containing it, respectively; either option overrides the adjustment and absolute dimensions if combined with them. This command sets the window's `window-size` option to `manual`.

## EXAMPLES

Increase a window's width by five columns:

```sh
tmux resize-window -R -t work:2 5
```

Set a window to 120 columns by 40 lines:

```sh
tmux resize-window -x 120 -y 40 -t work:2
```
