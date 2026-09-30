# display-message

Display a message in the status line or print it as command output.

Alias: `display`

## USAGE

```text
tmux display-message [OPTIONS] [message]
```

## OPTIONS

- `-a` — List format variables and their values.
- `-c target-client` — Choose the client for display and formats.
- `-C` — Keep pane output updating during the message.
- `-d delay` — Set the display time in milliseconds.
- `-F format` — Display this format instead of `message`.
- `-I` — Forward standard input to an empty pane.
- `-j` — Parse `message` as JSON before printing.
- `-l` — Display `message` without expanding formats.
- `-N` — Ignore key presses until the delay expires.
- `-p` — Print the result instead of showing a message.
- `-t target-pane` — Use this pane as the format context.
- `-v` — Log format parsing verbosely.

## NOTES

Without `-p`, the message is shown in the target client's status line. Its duration defaults to the `display-time` option; a delay of zero waits for a key press unless `-N` is used. `-C` lets the pane continue updating while the message is displayed.

`message` is a format unless `-l` is given. If neither `message` nor `-F` is supplied, tmux displays the default session, window, pane, and time message. `-F` cannot be used together with `message`. With `-j`, the selected text is parsed as JSON and printed.

`-a` lists format variables instead of displaying a message, except when combined with `-j`. `-I` forwards standard input to the empty target pane; it does not do so when `-j` is also given. The target pane defaults to the active pane. `-c` selects the client where the status message is shown and, when it belongs to the target session, supplies client format values.

## EXAMPLES

Print the current pane's working directory:

```sh
tmux display-message -p '#{pane_current_path}'
```

Show a formatted status message for five seconds:

```sh
tmux display-message -d 5000 'Session: #{session_name}'
```
