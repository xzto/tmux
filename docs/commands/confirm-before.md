# confirm-before

Ask before running a command.

Alias: `confirm`

## USAGE

```text
tmux confirm-before [OPTIONS] command
```

## OPTIONS

- `-b` — Show the prompt without blocking the command queue. The invoking client remains until it is dismissed.
- `-c confirm-key` — Set the key that confirms the command.
- `-p prompt` — Set the prompt text.
- `-t target-client` — Choose the client displaying the prompt.
- `-y` — Make Enter alone confirm by default.

## NOTES

The default prompt names the command and accepts `y` to confirm; Enter alone declines. With `-c`, the confirmation key defaults to `y` when the option is omitted; a supplied key must be one printable ASCII character. The prompt may use the special character sequences supported by the `status-left` option.

## EXAMPLES

Ask before displaying a confirmation message:

```sh
tmux confirm-before 'display-message "Confirmed"'
```

Ask for confirmation with a custom prompt and accept Enter by default:

```sh
tmux confirm-before -y -p 'Continue?' 'display-message "Continuing"'
```
