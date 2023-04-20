# Oberon

BPF program to augment and monitor the Linux Completely Fair Scheduler

## Configure Oberon CLI

You might want to copy oberon.conf to `~/.config/oberon` and rename it to `config`. The path `~/.config/oberon` is the default config path for Oberon CLI

Feel free to tweak the values inside `~/.config/oberon/config`, you can read more about the documentation of each variables in `oberon/src/app.rs`. Every values in the TOML config file is mapped directly into the corresponding attributes with the same name in App. Some variables also have a mini docs inside the TOML config itself.
