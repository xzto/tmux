# show-messages

Show the server message log or debugging information.

Alias: `showmsgs`

## USAGE

```text
tmux show-messages [OPTIONS]
```

## OPTIONS

- `-J` — Show job debugging information.
- `-T` — Show terminal debugging information.
- `-t target-client` — Choose client for terminal details and formats.

## NOTES

With no debugging option, messages are listed newest first. The server keeps messages up to the `message-limit` setting. `-J` and `-T` may be used together. When `-T` is given with `-t`, terminal details are limited to that client's terminal.

## EXAMPLES

Show the recent server message log:

```sh
tmux show-messages
```

Show job and terminal debugging information:

```sh
tmux show-messages -JT
```
