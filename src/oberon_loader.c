#include "oberon_loader.h"

int link_oberon_tracepoint_probe(const char *bpf_elf_path, const char *bpf_link_pin_path, const char *tp_category, const char *tp_name)
{
    struct bpf_object *bpf_obj;
    struct bpf_program *bpf_prog;
    struct bpf_link *link;
    char *license = "GPL";
    __u32 kernel_version = LINUX_VERSION_CODE;
    int err, prog_fd;

    bpf_obj = bpf_object__open(bpf_elf_path);
    bpf_prog = bpf_program__next(NULL, bpf_obj);

    err = bpf_program__set_tracepoint(bpf_prog);

    if (err)
    {

        fprintf(stderr, "ERR couldn't setup program type\n");

        return -1;
    }

    err = bpf_program__load(bpf_prog, license, kernel_version);

    if (err)
    {

        fprintf(stderr, "ERR couldn't setup program phase\n");
        return -1;
    }

    prog_fd = bpf_program__fd(bpf_prog);

    link = bpf_program__attach_tracepoint(bpf_prog, tp_category, tp_name);

    if (!link)
    {
        fprintf(stderr, "ERROR ATTACHING TRACEPOINT\n");
        return -1;
    }

    assert(bpf_program__is_tracepoint(bpf_prog));

    err = bpf_program__pin(bpf_prog, bpf_link_pin_path);
    if (err)
    {
        fprintf(stderr, "ERR couldn't pin program");
        return -1;
    }
    return err;
}