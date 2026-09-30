# find-window

Search window names, pane titles, and visible pane content.

Alias: `findw`

## USAGE

```text
tmux find-window [OPTIONS] match-string
```

## OPTIONS

- `-C` — Match visible pane contents. Content in scrollback history is not searched.
- `-N` — Match window names only.
- `-T` — Match pane titles only.
- `-i` — Ignore case while matching.
- `-r` — Treat `match-string` as a regular expression. By default it is a glob pattern.
- `-t target-pane` — Choose the target pane for the search.
- `-Z` — Zoom the pane.

## NOTES

By default, searches window names, pane titles, and visible pane contents. Use `-C`, `-N`, and `-T` to select which of these to search; combinations are allowed. The command works only when at least one client is attached.

## EXAMPLES

Find windows or panes whose visible content matches a glob:

```sh
tmux find-window 'build*'
```

Find content matching a case-insensitive regular expression:

```sh
tmux find-window -Ci -r 'warning.*failed'
```
