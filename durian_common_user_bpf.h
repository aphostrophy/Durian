#ifndef __DURIAN_COMMON_USER_BPF_H
#define __DURIAN_COMMON_USER_BPF_H

#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <bpf/libbpf.h>
#include <bpf/bpf.h>
#include <linux/version.h>
#include "durian_def.h"

struct bpf_object *load_bpf_and_tracepoint_attach(const char *bpf_elf_path, const char *pin_dir);

#endif