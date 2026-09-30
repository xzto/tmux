# load-buffer

Load a file into a paste buffer.

Alias: `loadb`

## USAGE

```text
tmux load-buffer [OPTIONS] path
```

## OPTIONS

- `-b buffer-name` — Name the buffer to create or replace.
- `-t target-client` — Choose the client for clipboard use.
- `-w` — Send the loaded buffer to the terminal clipboard.

## NOTES

If `-b` is omitted, tmux creates an automatically named buffer. `-t` selects the target client when using `-w`.

Use `-` as `path` to read from standard input. `-w` sends the loaded contents to the selected client's terminal clipboard using the xterm escape sequence, if supported. An empty file does not create a buffer.

## EXAMPLES

Load a file into a named buffer:

```sh
tmux load-buffer -b snippet /tmp/snippet.txt
```
