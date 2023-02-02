#ifndef __OBERON_LOADER_H
#define __OBERON_LOADER_H

#include <stdio.h>
#include <bpf/libbpf.h>
#include <bpf/bpf.h>
#include <linux/version.h>
#include <assert.h>

int link_oberon_tracepoint_probe(const char *bpf_elf_path, const char *bpf_link_pin_path, const char *tp_category, const char *tp_name);

#endif