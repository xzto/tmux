# new-session

Create a new session and its initial window.

Alias: `new`

## USAGE

```text
tmux new-session [OPTIONS] [shell-command [argument ...]]
```

## OPTIONS

- `-A` — Attach to a matching session if one exists. Otherwise, create a session.
- `-c start-directory` — Set the session and initial window directory.
- `-d` — Create the session without attaching a client.
- `-D` — Detach other clients when attaching with `-A`.
- `-e VARIABLE=value` — Set an environment variable in the session. May be repeated.
- `-E` — Skip applying `update-environment` to the session.
- `-F format` — Set the output format used by `-P`.
- `-f flags` — Set comma-separated flags on the attached client.
- `-n window-name` — Name the initial window.
- `-P` — Print information about the created session. The default format is `#{session_name}:`.
- `-s session-name` — Set the new session's name. A duplicate name is an error unless `-A` is used.
- `-t target-session` — Choose a session or group to join. A new group is created if needed.
- `-x width` — Set the session's default width. `-` uses the current client's width, if any.
- `-y height` — Set the session's default height. `-` uses the current client's height, if any.
- `-X` — Send SIGHUP to other clients' parents. This applies with `-A`.

## NOTES

If `shell-command` is omitted, tmux runs the `default-command` in the initial window. The session is attached to the current terminal unless `-d` is given. The initial size normally comes from the attached client; for a detached session, tmux uses the global `default-size` option.

With `-A`, tmux attaches to an existing session selected by `-s` or `-t` instead of creating one. When attaching this way, `-D` detaches other clients and `-X` also sends SIGHUP to their parent processes. If no matching session exists, tmux creates a new session.

`-t` adds the new session to a session group: it may name an existing group, an existing session whose group should be shared, or a new group. `-n` and `shell-command` cannot be used with `-t`.

`-x` and `-y` set the session's default size; `-` uses the corresponding size of the current client, if one exists. For a detached session, a dimension not specified with `-x` or `-y` comes from the global `default-size` option. `-E` skips `update-environment`, while values supplied with `-e` are still added to the new session. `-F` has an effect only with `-P`.

## EXAMPLES

Create or attach to a session named `work`:

```sh
tmux new-session -A -s work
```

Create a detached session named `background`:

```sh
tmux new-session -d -s background
```
