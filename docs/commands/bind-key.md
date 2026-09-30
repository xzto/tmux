# bind-key

Bind a key to a tmux command.

Alias: `bind`

## USAGE

```text
tmux bind-key [OPTIONS] key [command [argument ...]]
```

## OPTIONS

- `-n` — Bind in the root key table. This is the same as `-T root`.
- `-T key-table` — Choose the key table to use. Defaults to `prefix`, unless `-n` is used.
- `-r` — Allow this binding to repeat. It uses the `initial-repeat-time` and `repeat-time` options. With no command, it can update an existing binding.
- `-N note` — Attach a note to the binding. Notes are shown by `list-keys -N`; an empty note clears it. With no command, it can update an existing binding.

## NOTES

Without `-T` or `-n`, the key is bound in the `prefix` table. If both are given, `-T` selects the table. The `root` table is for keys pressed without the prefix key. A key can be followed by a tmux command and its arguments; without a command, `-r` and `-N` can change those properties on an existing binding.

## EXAMPLES

Bind F12 in the prefix table to display a message:

```sh
tmux bind-key -T prefix F12 display-message 'F12 pressed'
```

Bind a repeatable pane-resize command to Ctrl-Left:

```sh
tmux bind-key -r -T prefix C-Left resize-pane -L 5
```
