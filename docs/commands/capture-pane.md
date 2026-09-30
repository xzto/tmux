# capture-pane

Capture pane contents to a buffer or standard output.

Alias: `capturep`

## USAGE

```text
tmux capture-pane [OPTIONS]
```

## OPTIONS

- `-a` — Capture the alternate screen. Its history is unavailable.
- `-b buffer-name` — Name the destination buffer. Ignored when `-p` is used.
- `-C` — Escape non-printable characters. They use octal or C-style escapes.
- `-e` — Include text and background escape sequences.
- `-E end-line` — Set the final line to capture. Zero is the first visible line; negative values refer to history.
- `-F` — Include flags for each captured line. Flags include dead, hyperlink, output, prompt, wrapped, and extended-cell markers.
- `-H` — Capture hyperlinks from the selected lines. Multiple links on a line are separated by spaces.
- `-I` — Include each line's history timestamp. Unavailable timestamps are shown as zero.
- `-J` — Join wrapped lines. This preserves trailing spaces and implies `-T`.
- `-L` — Include a line number at the start of each line.
- `-M` — Capture the screen for the pane's current mode. If the mode has no screen, the pane's normal screen is used.
- `-N` — Preserve trailing spaces at the end of each line.
- `-p` — Write the capture to standard output. By default, it is stored in a buffer.
- `-P` — Capture pending incomplete escape-sequence output. Other pane contents are not included.
- `-q` — Suppress an error when no alternate screen exists. Use with `-a`.
- `-R` — Dump the internal grid data. The diagnostic output includes grid and cell details.
- `-S start-line` — Set the first line to capture. Zero is the first visible line; negative values refer to history.
- `-T` — Ignore trailing empty positions. `-J` implies this behavior.
- `-t target-pane` — Choose the pane to capture. Defaults to the active pane.

## NOTES

By default, only the visible contents of the pane are captured and stored in a new buffer. Use `-b` to name that buffer or `-p` to print the result. `-a` selects the alternate screen, which has no history; if it is absent, `-q` suppresses the error and produces an empty capture. `-M` uses a mode's screen when available.

`-S` and `-E` select a range of line numbers: zero is the first visible line, negative numbers refer to history, and `-` means the beginning of history for `-S` or the end of the visible pane for `-E`. `-F` reports line markers: `D` for dead, `H` for hyperlinks, `O` for output, `P` for prompt, `W` for wrapped, and `X` for extended cells; `-` means no flags. With `-H`, only hyperlinks on the selected lines are captured. `-R` is intended for diagnostics.

## EXAMPLES

Print the visible contents of the active pane:

```sh
tmux capture-pane -p
```

Save the pane's full history and visible contents in a named buffer:

```sh
tmux capture-pane -S - -b pane-history
```
