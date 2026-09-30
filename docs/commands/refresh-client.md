# refresh-client

Refresh a client or change its visible window area.

Alias: `refresh`

## USAGE

```text
tmux refresh-client [OPTIONS] [adjustment]
```

## OPTIONS

- `-A pane:state` — Set a pane's output state for a control client. The state is `on`, `off`, `continue`, or `pause`; repeat this option for different panes.
- `-B name:what:format` — Subscribe a control client to a format. The name identifies the subscription; `what` selects a pane or window, and `format` is evaluated for it. Use only the name to remove a subscription.
- `-C size` — Set a control client's size. Use `widthxheight` or `width,height`; prefix a window size with `@window-id:`. Use `@window-id:` alone to clear that window's size.
- `-f flags` — Set comma-separated client flags. See `attach-session` for the available flags.
- `-F flags` — Set client flags as with `-f`. This is an alias for `-f`.
- `-l` — Request and store the client's clipboard. tmux uses an xterm control sequence and saves the result in a new paste buffer.
- `-c` — Reset the visible area to follow the cursor.
- `-D` — Move the visible area down. Use `[adjustment]` to set the number of rows.
- `-U` — Move the visible area up. Use `[adjustment]` to set the number of rows.
- `-L` — Move the visible area left. Use `[adjustment]` to set the number of columns.
- `-R` — Move the visible area right. Use `[adjustment]` to set the number of columns.
- `-S` — Refresh only the client's status line.
- `-r pane:report` — Report pane information to a control client. Give a pane ID followed by a colon and a report escape sequence.
- `-t target-client` — Select the client to refresh.

## NOTES

`-A`, `-B`, and `-C` are for control mode clients. `-A` accepts a pane ID followed by `on`, `off`, `continue`, or `pause`: `off` stops sending that pane's output, while `continue` resumes paused output. `-B` can subscribe to a pane ID such as `%0`, a window ID such as `@0`, `%*` for all panes, `@*` for all windows, or an empty `what` to check the attached session. Updates are reported with `%subscription-changed` at most once a second. `-C` accepts a window ID and size as `@window-id:widthxheight`; a window ID with no size clears the explicit window size. These control-mode options cannot be used by a regular client.

`-D`, `-U`, `-L`, and `-R` pan the visible part of a window that is larger than the client. `[adjustment]` is a positive number of rows or columns and defaults to `1`; it applies to these directions. `-c` restores automatic cursor tracking. The visible position belongs to the client and resets when its attached session changes current window.

`-r` lets a control mode client report pane information, such as an OSC 10 response. The argument is a pane ID beginning with `%`, followed by a colon and the report escape sequence. `-S` limits the refresh to the status line; without it, tmux redraws the client.

## EXAMPLES

Refresh only the current client's status line:

```sh
tmux refresh-client -S
```

Move the visible area five columns to the left:

```sh
tmux refresh-client -L 5
```
