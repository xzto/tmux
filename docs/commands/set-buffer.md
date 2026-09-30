# set-buffer

Set or append the contents of a paste buffer, or rename a buffer.

Alias: `setb`

## USAGE

```text
tmux set-buffer [OPTIONS] [data]
```

## OPTIONS

- `-a` — Append data to the selected buffer.
- `-b buffer-name` — Select or name a buffer.
- `-n new-buffer-name` — Rename a buffer.
- `-t target-client` — Choose the client for clipboard use.
- `-w` — Send the buffer to the terminal clipboard.

## NOTES

`-a` appends only when `-b` names an existing buffer; otherwise the data is stored in a new buffer. `-b` names the buffer to set, replacing it if it already exists. With `-n`, `-b` selects the buffer to rename; if omitted, the most recently added automatically named buffer is renamed. `-n` renames a buffer and does not set its data. `-t` selects the target client when using `-w`.

Without `-b`, data is stored in a new automatically named buffer. `-w` sends the buffer to the selected client's terminal clipboard using the xterm escape sequence, if supported. Empty data does not create or replace a buffer.

## EXAMPLES

Set a named buffer:

```sh
tmux set-buffer -b note 'deploy complete'
```

Append to that buffer:

```sh
tmux set-buffer -a -b note ' — verified'
```
