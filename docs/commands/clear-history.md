# clear-history

Remove and free the history for a pane.

Alias: `clearhist`

## USAGE

```text
tmux clear-history [OPTIONS]
```

## OPTIONS

- `-H` — Also clear pane hyperlinks.
- `-t target-pane` — Choose the pane to clear.

## NOTES

The target defaults to the active pane. Clearing history also exits modes on the pane. `-H` removes all hyperlinks from the pane.

## EXAMPLES

Clear history for the active pane:

```sh
tmux clear-history
```

Clear history and hyperlinks for pane `%3`:

```sh
tmux clear-history -Ht %3
```
