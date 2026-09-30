# paste-buffer

Insert a paste buffer into a pane.

Alias: `pasteb`

## USAGE

```text
tmux paste-buffer [OPTIONS]
```

## OPTIONS

- `-b buffer-name` — Paste the named buffer.
- `-d` — Delete the buffer after pasting.
- `-p` — Use bracketed paste when supported.
- `-r` — Keep linefeeds instead of replacing them.
- `-s separator` — Set the text replacing each linefeed.
- `-S` — Do not sanitize control characters.
- `-t target-pane` — Choose the pane to receive the paste.

## NOTES

Without `-b`, tmux uses the most recently added automatically named buffer. The default target is the current pane. By default, control characters are sanitized. Linefeeds are replaced with carriage returns unless `-r` is used; `-s` sets a custom separator, and overrides `-r`. `-p` adds bracketed-paste control codes around the data only if the target application has requested bracketed paste. `-d` deletes the buffer after the command, even if the target pane has input disabled.

Each linefeed in the buffer is replaced by the separator; `-r` is equivalent to using a linefeed as the separator. The `-S` flag disables the default control-character sanitization.

## EXAMPLES

Paste a named buffer into the current pane:

```sh
tmux paste-buffer -b snippet
```
