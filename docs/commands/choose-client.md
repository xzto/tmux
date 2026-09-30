# choose-client

Choose an attached client interactively from a list.

## USAGE

```text
tmux choose-client [OPTIONS] [template]
```

## OPTIONS

- `-F format` — Set the format for each client line.
- `-f filter` — Filter clients with a format.
- `-h` — Hide the pane containing the mode.
- `-i` — Start with client information shown.
- `-K key-format` — Set the format for shortcut keys.
- `-k` — Kill the pane when the mode exits.
- `-N` — Start without the preview.
- `-O sort-order` — Set the initial sort order.
- `-r` — Reverse the sort order.
- `-t target-pane` — Choose the pane to put in client mode.
- `-y` — Disable confirmation prompts.
- `-Z` — Zoom the pane.

## NOTES

Each client appears on one line. Press Enter to choose a client. If `template` is supplied, `%%` is replaced by the chosen client's name and the result is run as a command. Without a template, tmux detaches the selected client using `detach-client -t '%%'`.

Sort orders are `name`, `size`, `creation` (time), and `activity` (time); `-r` reverses the selected order. The filter is a format, and clients whose format evaluates to zero are hidden. If a filter would hide every client, it is ignored. `-F` formats each client line and `-K` formats its shortcut key; both formats are evaluated once per line. `-i` starts with client information instead of the preview. `-N` starts without the preview; if given twice, it starts with the larger preview. The mode is available only when at least one client is attached.

## EXAMPLES

Choose a client and show its name in the status line:

```sh
tmux choose-client 'display-message "Selected client: %%"'
```
