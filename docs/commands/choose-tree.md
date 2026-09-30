# choose-tree

Choose a session, window, or pane interactively from a tree.

## USAGE

```text
tmux choose-tree [OPTIONS] [template]
```

## OPTIONS

- `-F format` — Set the format for each tree item.
- `-f filter` — Filter tree items with a format.
- `-G` — Include every session in a session group.
- `-h` — Hide the pane containing the mode.
- `-K key-format` — Set the format for shortcut keys.
- `-k` — Kill the pane when the mode exits.
- `-N` — Start without the preview.
- `-O sort-order` — Set the initial sort order.
- `-r` — Reverse the sort order.
- `-s` — Start with sessions collapsed.
- `-w` — Start with windows collapsed.
- `-t target-pane` — Choose the pane to put in tree mode.
- `-y` — Disable confirmation prompts.
- `-Z` — Zoom the pane.

## NOTES

Each session, window, or pane appears on one line. Press Enter to choose an item. If `template` is supplied, the first `%%` and all `%1` occurrences are replaced by the chosen target and the result is run as a command. Without a template, tmux switches the client to the chosen target using `switch-client -t '%%'`.

Sort orders are `index`, `name`, `activity` (time), and `z`; `-r` reverses the selected order. The filter is a format, and items whose format evaluates to zero are hidden. If a filter would hide every item, it is ignored. `-F` formats each tree item and `-K` formats its shortcut key; both formats are evaluated once per line. `-G` includes all sessions in a session group rather than only the first. `-N` starts without the preview; if given twice, it starts with the larger preview. `-h` and `-k` are intended to ease use of the mode in a floating pane. The mode is available only when at least one client is attached.

## EXAMPLES

Browse the tree with sessions initially collapsed:

```sh
tmux choose-tree -s
```
