# move-window

Move a window to another index or session.

Alias: `movew`

## USAGE

```text
tmux move-window [OPTIONS]
```

## OPTIONS

- `-a` — Insert after the destination window. Existing windows shift as needed.
- `-b` — Insert before the destination window. Existing windows shift as needed.
- `-d` — Leave the moved window unselected.
- `-k` — Replace the destination window if it exists.
- `-r` — Renumber the target session's windows. Use sequential indices.
- `-s src-window` — Choose the source window to move.
- `-t dst-window` — Choose the destination session and window index.

## NOTES

Unlike `link-window`, this command unlinks the source window from its original session after linking it at the destination. Without `-k`, moving to an occupied destination window is an error. `-a` and `-b` insert next to the destination window and shift existing windows as needed. With `-r`, the target session is renumbered in sequence, respecting the `base-index` option.

## EXAMPLES

Move a window to an unused index in another session:

```sh
tmux move-window -s work:2 -t archive:5
```

Move a window after a destination window:

```sh
tmux move-window -a -s work:2 -t archive:4
```
