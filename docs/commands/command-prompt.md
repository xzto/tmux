# command-prompt

Open a prompt in a client to run a command interactively.

## USAGE

```text
tmux command-prompt [OPTIONS] [template]
```

## OPTIONS

- `-1` — Accept one character as input.
- `-b` — Do not wait for the prompt response. The client remains until the prompt is dismissed.
- `-C` — Keep panes updating while the prompt is open.
- `-e` — Let Backspace cancel an empty prompt.
- `-F` — Expand `template` as a format.
- `-i` — Run the command whenever prompt input changes.
- `-k` — Accept one key and translate it to a key name.
- `-l` — Treat prompt and input lists literally.
- `-N` — Accept only numeric key presses.
- `-P` — Open a pane prompt instead of a status-line prompt.
- `-I inputs` — Set initial text for the prompts.
- `-p prompts` — Set the prompts to display.
- `-t target-client` — Choose the client displaying the prompt.
- `-T prompt-type` — Set the prompt type for completion.

## NOTES

If `template` is omitted, the prompt is `:`. Otherwise, the prompt is built from the template. Before the command runs, the first `%%` and all `%1` occurrences are replaced by the first response; `%2` through `%9` refer to later prompts. `%%%` is like `%%` but escapes quotation marks in the response.

`-p` accepts a comma-separated list of prompts, and `-I` accepts a comma-separated list of their initial text. `-l` disables splitting both lists at commas. `-T` selects prompt completions; the available types are `command` and `search`. `-1` accepts a single character, while `-k` supplies the key name; `-N` restricts input to numeric keys.

With `-b`, the command queue does not wait for the response, although the invoking client remains until the prompt is dismissed. `-i` runs the command on every edit rather than when the prompt is closed. `-P` opens the prompt inside the target pane, while the default prompt appears on the status line.

## EXAMPLES

Prompt for text and display the response:

```sh
tmux command-prompt -p 'Message' 'display-message "Entered: %1"'
```

Open a prompt for a command to run:

```sh
tmux command-prompt
```
