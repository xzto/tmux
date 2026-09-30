# link-window

Link a window into a session without removing its existing link.

Alias: `linkw`

## USAGE

```text
tmux link-window [OPTIONS]
```

## OPTIONS

- `-a` — Insert after the destination window. Existing windows shift as needed.
- `-b` — Insert before the destination window. Existing windows shift as needed.
- `-d` — Leave the newly linked window unselected.
- `-k` — Replace the destination window if it exists.
- `-s src-window` — Choose the source window to link.
- `-t dst-window` — Choose the destination session and window index.

## NOTES

The source window remains linked to its original session. Without `-k`, linking to an occupied destination window is an error. `-a` and `-b` insert next to the destination window and shift existing windows as needed.

## EXAMPLES

Link a window from one session at an unused index in another:

```sh
tmux link-window -s work:2 -t archive:5
```

Link the current window after a destination window:

```sh
tmux link-window -a -t archive:4
```
