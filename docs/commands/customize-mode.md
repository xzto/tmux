# customize-mode

Browse and change options and key bindings in a pane.

## USAGE

```text
tmux customize-mode [OPTIONS]
```

## OPTIONS

- `-F format` — Set the format for each item.
- `-f filter` — Filter the initial item list.
- `-k` — Kill the pane when the mode exits.
- `-N` — Start without option information.
- `-t target-pane` — Choose the target pane.
- `-y` — Automatically accept confirmation prompts.
- `-Z` — Zoom the pane.

## NOTES

The target defaults to the active pane. The initial `filter` is a format: items for which it evaluates to zero are hidden; a filter that would hide every item is ignored. `format` is expanded for each item. This mode works only if at least one client is attached.

## EXAMPLES

Open customize mode for the active pane:

```sh
tmux customize-mode
```

Open it in pane `%3` without option information:

```sh
tmux customize-mode -Nt %3
```
