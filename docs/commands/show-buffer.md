# show-buffer

Display the contents of a paste buffer.

Alias: `showb`

## USAGE

```text
tmux show-buffer [OPTIONS]
```

## OPTIONS

- `-b buffer-name` — Select the buffer to display.

## NOTES

If `-b` is omitted, tmux displays the most recently added automatically named buffer. If no such buffer exists, the command returns an error.

## EXAMPLES

Display the most recently added automatically named buffer:

```sh
tmux show-buffer
```

Display a named buffer:

```sh
tmux show-buffer -b my-buffer
```
