# copy-mode

Enter copy mode to view pane history and copy text.

## USAGE

```text
tmux copy-mode [OPTIONS]
```

## OPTIONS

- `-d` — Enter copy mode one page down.
- `-u` — Enter copy mode one page up.
- `-e` — Exit copy mode when scrolling reaches the bottom.
- `-H` — Hide the position indicator in copy mode.
- `-k` — Kill the target pane when copy mode exits.
- `-M` — Begin a mouse drag in the target pane.
- `-q` — Cancel copy mode and any other mode.
- `-S` — Scroll copy mode during a mouse drag.
- `-s src-pane` — Copy from `src-pane` instead of the target pane.
- `-t target-pane` — Choose the pane to enter copy mode.

## NOTES

The `-u` and `-d` flags enter copy mode and scroll one page up or down. The `-e` flag exits when scrolling reaches the bottom of the history, unless a selection is present; pressing a non-scrolling key disables this behavior. The `-k` flag kills the pane when copy mode is exited.

The `-H` flag hides the position indicator in the top-right corner. The `-q` flag cancels copy mode and any other mode. The `-M` flag begins a mouse drag and is only valid when bound to a mouse key binding. The `-S` flag enters copy mode and scrolls when bound to a mouse drag event. The `-s` flag uses `src-pane` as the pane to copy from instead of `target-pane`.

## EXAMPLES

Enter copy mode and scroll up one page:

```sh
tmux copy-mode -u
```

Enter copy mode and exit it when scrolling back to the bottom of history:

```sh
tmux copy-mode -eu
```
