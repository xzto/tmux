# select-pane

Select a pane, change its input state, or mark it for pane commands.

Alias: `selectp`

## USAGE

```text
tmux select-pane [OPTIONS]
```

## OPTIONS

- `-D` — Select the pane below the target.
- `-U` — Select the pane above the target.
- `-L` — Select the pane to the left of the target.
- `-R` — Select the pane to the right of the target.
- `-d` — Disable input to the target pane.
- `-e` — Enable input to the target pane.
- `-g` — Print the deprecated window style. Prints `window-style` for the target pane.
- `-l` — Select the last pane. This is equivalent to `last-pane`.
- `-M` — Clear the marked pane.
- `-m` — Toggle the marked state of the target pane.
- `-P style` — Set the deprecated pane window style. Sets both `window-style` and `window-active-style` for the target pane.
- `-T title` — Set the target pane's title.
- `-t target-pane` — Choose the pane to select or act on.
- `-Z` — Keep the window zoomed if it was zoomed.

## NOTES

`-m` sets the target pane as the marked pane, clearing any previous mark; if it is already marked, the mark is cleared. It does nothing when the target pane is not visible. `-M` clears the marked pane. The marked pane is the default source for `join-pane`, `move-pane`, `swap-pane`, and `swap-window` when their `-s` option is omitted.

`-P` and `-g` are deprecated. `-P` sets both `window-style` and `window-active-style` on the target pane's options; `-g` prints its `window-style` value.

## EXAMPLES

Select the pane to the left of the active pane:

```sh
tmux select-pane -L
```
