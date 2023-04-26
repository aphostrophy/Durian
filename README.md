# Durian

BPF program to augment and monitor the Linux Completely Fair Scheduler

## Configure Durian CLI

You might want to copy durian.conf to `~/.config/durian` and rename it to `config`. The path `~/.config/durian` is the default config path for Durian CLI

Feel free to tweak the values inside `~/.config/durian/config`, you can read more about the documentation of each variables in `durian/src/app.rs`. Every values in the TOML config file is mapped directly into the corresponding attributes with the same name in App. Some variables also have a mini docs inside the TOML config itself.
