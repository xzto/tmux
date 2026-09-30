# choose-buffer

Choose a paste buffer interactively from a list.

## USAGE

```text
tmux choose-buffer [OPTIONS] [template]
```

## OPTIONS

- `-F format` — Set the format for each buffer line.
- `-f filter` — Filter buffers with a format.
- `-K key-format` — Set the format for shortcut keys.
- `-k` — Kill the pane when the mode exits.
- `-N` — Start without the preview.
- `-O sort-order` — Set the initial sort order.
- `-r` — Reverse the sort order.
- `-t target-pane` — Choose the pane to put in buffer mode.
- `-y` — Disable confirmation prompts.
- `-Z` — Zoom the pane.

## NOTES

Each buffer appears on one line. Press Enter to choose the selected buffer; by default, tmux pastes it into the target pane. If `template` is supplied, `%%` is replaced by the chosen buffer's name and the result is run as a command. The default template is `paste-buffer -p -b '%%'`.

Sort orders are `creation` (time), `name`, and `size`; `-r` reverses the selected order. The filter is a format, and buffers whose format evaluates to zero are hidden. If a filter would hide every buffer, it is ignored. `-F` formats each buffer line and `-K` formats its shortcut key; both formats are evaluated once per line. `-N` starts without the preview; if given twice, it starts with the larger preview. The mode is available only when at least one client is attached.

## EXAMPLES

Choose a buffer and show its name in the status line:

```sh
tmux choose-buffer 'display-message "Selected buffer: %%"'
```
