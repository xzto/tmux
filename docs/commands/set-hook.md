# set-hook

Set, unset, or run a hook, monitor subscription, or user event.

## USAGE

```text
tmux set-hook [OPTIONS] [hook] [command]
```

## OPTIONS

- `-a` — Append the command to a hook. Adds it to the end of the hook's command array.
- `-B name:what:format` — Install a format-based monitor hook. The name must begin with `@`; use `-u` to remove the subscription. See NOTES for the subscription fields.
- `-E` — Fire a user event. The event name must begin with `@`.
- `-g` — Use global hook options. Selects global session scope, or global window scope with `-w`.
- `-p` — Use pane hook scope. Selects options attached to the target pane.
- `-R` — Run a hook immediately. Does not set or change its stored command.
- `-T` — Require a true monitor format. Applies with `-B`.
- `-t target-pane` — Choose the target context. It determines the session, window, or pane for the hook.
- `-u` — Unset a hook or monitor subscription. With `-B`, removes that subscription.
- `-w` — Use window hook scope. With `-g`, selects global window hooks.

## NOTES

Without `-E`, `-R`, or `-B`, `hook` sets a hook command; hooks are command arrays run in order. Setting a hook without an array key replaces its existing commands with the supplied command. Use `-a` to append and `-u` to unset the hook.

With `-E`, `hook` is a user event name beginning with `@`. With `-R`, the named hook runs immediately instead of being set.

With `-B`, `name:what:format` installs a monitor subscription: `name` is the `@`-prefixed hook to run, `what` selects a session, pane, all panes, window, or all windows, and `format` is expanded once a second. A `command` may be supplied to store as the `@` hook command; without one, only the monitor is installed. `-T` runs the hook only when the format is true. Monitor hooks are not inherited and run only in the scope where they are created; `-u -B` removes the subscription.

Use `-g` for global hooks, `-w` for window scope, and `-p` for pane scope. `-t` selects the target context.

## EXAMPLES

Run a layout command after each split:

```sh
tmux set-hook -g after-split-window 'select-layout even-vertical'
```
