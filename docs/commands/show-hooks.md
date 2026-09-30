# show-hooks

List hooks in a session, window, or pane scope.

## USAGE

```text
tmux show-hooks [OPTIONS] [hook]
```

## OPTIONS

- `-B` — List format-monitor hooks.
- `-F format` — Use a custom output format. The format can use hook and option fields.
- `-g` — Use global scope. Selects global session options, or global window options with `-w`.
- `-p` — Use pane scope. Select hooks from the target pane's options.
- `-w` — Use window scope. Select hooks from the target window's options.
- `-t target-pane` — Select the target pane. Defaults to the current pane.

## NOTES

Without `hook`, the command lists all hooks in the selected scope; with `hook`, it shows only that hook. The default scope is the current session. Use `-p` or `-w` to select pane or window hooks, and combine `-g` with `-w` for global window hooks.

`-B` lists hooks that monitor a format rather than ordinary event hooks. These are configured with `set-hook -B`.

## EXAMPLES

List global session hooks:

```sh
tmux show-hooks -g
```

List global window hooks:

```sh
tmux show-hooks -g -w
```
