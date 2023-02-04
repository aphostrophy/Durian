#include "oberon_loader.h"

int link_oberon_tracepoint_probe(const char *bpf_elf_path)
{
    struct bpf_object *bpf_obj;
    struct bpf_program *bpf_prog;
    struct bpf_link *link;
    char *license = "GPL";
    __u32 kernel_version = LINUX_VERSION_CODE;
    int err;

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

    link = bpf_program__attach(bpf_prog);

    if (!link)
    {
        fprintf(stderr, "ERROR ATTACHING TRACEPOINT\n");
        return -1;
    }

    assert(bpf_program__is_tracepoint(bpf_prog));

    /*
        bpf_link for perf based hooks isn't supported for the moment
    */

    // err = bpf_link__pin(link, bpf_link_pin_path);
    // if (err)
    // {
    //     fprintf(stderr, "ERR couldn't pin link %s %s\n", bpf_link__pin_path(link), strerror(errno));
    //     return -1;
    // }
    return err;
}