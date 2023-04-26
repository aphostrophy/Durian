#include "durian_common_user_bpf.h"

static int reuse_maps(struct bpf_object *obj, const char *path)
{
    struct bpf_map *map;

    if (!obj)
        return -ENOENT;

    if (!path)
        return -EINVAL;

    bpf_object__for_each_map(map, obj)
    {
        int len, err;
        int pinned_map_fd;
        char buf[PATH_MAX];

        len = snprintf(buf, PATH_MAX, "%s/%s", path, bpf_map__name(map));
        if (len < 0)
        {
            return -EINVAL;
        }
        else if (len >= PATH_MAX)
        {
            return -ENAMETOOLONG;
        }

        pinned_map_fd = bpf_obj_get(buf);
        if (pinned_map_fd < 0)
        {
            return pinned_map_fd;
        }

        err = bpf_map__reuse_fd(map, pinned_map_fd);
        if (err)
            return err;
    }

    return 0;
}

struct bpf_object *load_bpf_object_file_reuse_maps(const char *file,
                                                   const char *pin_dir)
{
    int err;
    struct bpf_object *obj;

    obj = bpf_object__open(file);
    if (!obj)
    {
        fprintf(stderr, "ERR: failed to open object %s\n", file);
        return NULL;
    }

    err = reuse_maps(obj, pin_dir);
    if (err)
    {
        fprintf(stderr, "ERR: failed to reuse maps for object %s, pin_dir=%s\n",
                file, pin_dir);
        return NULL;
    }

    err = bpf_object__load(obj);
    if (err)
    {
        fprintf(stderr, "ERR: loading BPF-OBJ file(%s) (%d): %s\n",
                file, err, strerror(-err));
        return NULL;
    }

    return obj;
}

struct bpf_object *load_bpf_and_tracepoint_attach(const char *bpf_elf_path, const char *pin_dir)
{
    struct bpf_object *bpf_obj;
    struct bpf_program *bpf_prog;
    struct bpf_link *link;
    char *license = "GPL";
    int err;

    bpf_obj = load_bpf_object_file_reuse_maps(bpf_elf_path, pin_dir);

    bpf_prog = bpf_program__next(NULL, bpf_obj);

    if (!bpf_prog)
    {
        fprintf(stderr, "ERR: couldn't find a program in ELF section '%s'\n", strerror(errno));
        return NULL;
    }

    err = bpf_program__set_tracepoint(bpf_prog);

    if (err)
    {

        fprintf(stderr, "ERR couldn't setup program type\n");

        return NULL;
    }

    /*
        bpf_prog is already loaded into the kernel inside load_bpf_object_file_reuse_maps function call
    */

    link = bpf_program__attach(bpf_prog);

    if (!link)
    {
        fprintf(stderr, "ERROR ATTACHING TRACEPOINT\n");
        return NULL;
    }

    assert(bpf_program__is_tracepoint(bpf_prog));

    return bpf_obj;
}