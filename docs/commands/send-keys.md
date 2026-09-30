# send-keys

Send keys to a pane or client.

Alias: `send`

## USAGE

```text
tmux send-keys [OPTIONS] [key ...]
```

## OPTIONS

- `-F` — Expand formats in arguments. This applies where the command supports formats.
- `-H` — Treat each key as a hexadecimal byte. Values must be in the range `00` to `ff`.
- `-K` — Send keys through a client's key table. They are looked up for `target-client` instead of being sent to `target-pane`.
- `-l` — Send literal UTF-8 characters. This disables key-name lookup.
- `-M` — Pass a mouse event through to a pane. Use only from a mouse key binding.
- `-N repeat-count` — Send the keys this many times. The count must be positive.
- `-R` — Reset the terminal state before sending. This also clears the pane's colour palette.
- `-X` — Send a command into copy mode. The pane must be in a mode that accepts commands.
- `-c target-client` — Choose the target client. This is used when sending keys through a client with `-K`.
- `-t target-pane` — Choose the destination pane. Defaults to the active pane.

## NOTES

Each `key` is first looked up as a tmux key name; an unrecognized string is sent as a sequence of characters. Use `-l` to bypass key-name lookup or `-H` to provide hexadecimal ASCII byte values. Keys are sent in argument order. If no keys are given and the command is bound to a key, that key is sent unless `-N` or `-R` is used. `-N` can set a mode's repeat count when used with `-X` or with no keys in a mode. `-M` forwards a mouse event, while `-X` invokes the pane's mode command; these modes do not send ordinary key strings.

## EXAMPLES

Send Ctrl-L to pane 0 in the current window:

```sh
tmux send-keys -t 0 C-l
```

Type literal text into pane 0 without key-name lookup:

```sh
tmux send-keys -l -t 0 'hello'
```
