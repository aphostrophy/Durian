#ifndef __OBERON_LOADER_H
#define __OBERON_LOADER_H

#include <stdio.h>
#include <bpf/libbpf.h>
#include <bpf/bpf.h>
#include <linux/version.h>
#include <assert.h>
#include <errno.h>

int link_oberon_tracepoint_probe(const char *bpf_elf_path);

#endif