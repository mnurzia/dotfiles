from argparse import ArgumentParser
from os import environ, getcwd, scandir


# shorten prefix /path (slash belongs to path)
def shorten_path(prefix: str, path: str):
    current_path = prefix
    out_path = current_path
    print(out_path)
    while len(path):
        assert path[0] == "/"
        current_path += path[0]
        out_path += path[0]
        path = path[1:]
        # open directory
        entries = scandir(current_path)
        # segment name: path with leading slash removed
        name = path
        max_prefix_len = 0
        for entry in entries:
            entry = entry.name
            i = 0
            this_prefix_len = 0
            while True:
                name_end = i == len(name) or name[i] == "/"
                entry_end = i == len(entry)
                if name_end and entry_end:
                    # strings equal
                    break
                elif name_end:
                    # name shorter than entry
                    break
                elif entry_end or name[i].lower() != entry[i].lower():
                    # entry is a prefix of name
                    max_prefix_len = max(this_prefix_len + 1, max_prefix_len)
                    break
                i += 1
                this_prefix_len = max(this_prefix_len, i)
        i = 0
        seg_len = len(path) if path.find("/") == -1 else path.find("/")
        while len(path) and path[0] != "/":
            current_path += path[0]
            if i < max_prefix_len:
                out_path += path[0]
            elif i == max_prefix_len:
                out_path += f"~{seg_len - i}"
            path = path[1:]
            i += 1
        print(out_path)


if __name__ == "__main__":
    ap = ArgumentParser()
    ap.add_argument("path", default=getcwd(), nargs="?")

    args = ap.parse_args()

    prefix = environ["HOME"].rstrip("/")
    if not args.path.startswith(prefix):
        prefix = "/"
    else:
        args.path = args.path[len(prefix) :]

    shorten_path(prefix, args.path)
