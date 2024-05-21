#!/usr/bin/env python3
from argparse import ArgumentParser, FileType, Namespace
from glob import glob
from pathlib import Path
from sys import argv, exit
from configparser import ConfigParser, Interpolation
from typing import IO
from ipaddress import (
    IPv4Address,
    IPv6Address,
    ip_address,
    IPv4Network,
    IPv6Network,
    ip_network,
)
from subprocess import PIPE, run
from shutil import rmtree


class Cfg(ConfigParser):
    def __init__(self, file: IO, interp: dict[str, str] = {}):
        self._file = file

        class CfgInterpolation(Interpolation):
            def before_get(self, parser, section, option, value, defaults):
                for k, v in interp.items():
                    if (interp_var := f"%({k})") in value:
                        value = value.replace(interp_var, v)
                return value

        super().__init__(interpolation=CfgInterpolation())
        self.optionxform = str
        self.read_file(self._file)

    def get_ranges(self) -> list[IPv4Network | IPv6Network]:
        ranges_str = self.get("Config", "Ranges")
        ranges = list(map(ip_network, ranges_str.split(",")))
        return ranges

    def get_peers(self):
        for section_name in self.sections():
            if not section_name.startswith("Peer."):
                continue
            yield section_name.split(".")[1]

    def save(self, output_file: IO = None):
        if not output_file:
            output_file = self._file
        output_file.seek(0)
        output_file.truncate(0)
        for i, section in enumerate(self.sections()):
            if i:
                output_file.write("\n")
            output_file.write(f"[{section}]\n")
            for key, value in self.items(section):
                output_file.write(f"{key}={value}\n")
        output_file.flush()


def cmd_list(parser: ArgumentParser, args: Namespace) -> int:
    cfg = Cfg(args.config)
    for peer in cfg.get_peers():
        print(f"{peer}")
    return 0


def cmd_add(parser: ArgumentParser, args: Namespace) -> int:
    cfg = Cfg(args.config)
    if args.name in cfg.get_peers():
        parser.error(f"peer {args.name} already exists")
    used_ips: set[IPv4Address | IPv6Address] = set()
    for peer in cfg.get_peers():
        addrs_str = cfg.get(f"Peer.{peer}", "Addresses")
        used_ips |= set(map(ip_address, addrs_str.split(",")))
    if used := cfg.get(f"Config", "Reserved", fallback=None):
        used_ips |= set(map(ip_address, used.split(",")))
    ranges = cfg.get_ranges()
    addresses = []
    for network in ranges:
        for address in network.hosts():
            if address in used_ips:
                continue
            addresses.append(address)
            break
        else:
            parser.error(f"unable to find unused ip in {network}")
    peer_section_name = f"Peer.{args.name}"
    cfg.add_section(peer_section_name)
    cfg.set(peer_section_name, "Addresses", ",".join(map(str, addresses)))
    private_key = run(["wg", "genkey"], stdout=PIPE).stdout
    public_key = run(["wg", "pubkey"], stdout=PIPE, input=private_key).stdout
    cfg.set(peer_section_name, "PublicKey", public_key.decode().strip())
    templates = map(Path, glob(cfg.get("Config", "TemplatesDir") + "/*.cfg"))
    interpolations = {
        "private_key": private_key.decode().strip(),
        "public_key": public_key.decode().strip(),
        "addresses": ",".join(map(str, addresses)),
    }
    clients_path = Path(cfg.get("Config", "ClientsDir")) / args.name
    clients_path.mkdir(parents=True, exist_ok=True)
    for template in templates:
        with open(template, "r") as template_file:
            template_cfg = Cfg(template_file, interp=interpolations)
        with open(clients_path / template.name, "w") as client_file:
            template_cfg.save(client_file)
    cfg.save()
    return 0


def cmd_remove(parser: ArgumentParser, args: Namespace) -> int:
    cfg = Cfg(args.config)
    if args.name not in cfg.get_peers():
        parser.error(f"peer {args.name} does not exist")
    cfg.remove_section(f"Peer.{args.name}")
    cfg.save()
    if args.hard:
        rmtree(Path(cfg.get("Config", "ClientsDir")) / args.name)
    return 0


def cmd_conf(parser: ArgumentParser, args: Namespace) -> int:
    cfg = Cfg(args.config)
    for peer_name in cfg.get_peers():
        peer_cfg = f"Peer.{peer_name}"
        print("[Peer]")
        print(f"PublicKey={cfg.get(peer_cfg, "PublicKey")}")
        print(f"AllowedIPs={cfg.get(peer_cfg, "Addresses")}")
        print()
    return 0


def main(args: list[str]) -> int:
    parser = ArgumentParser()
    parser.add_argument("-c", "--config", type=FileType("r+"), default="wgpro.cfg")
    subs = parser.add_subparsers(required=True)

    parser_list = subs.add_parser("list", help="list profiles")
    parser_list.set_defaults(cmd=cmd_list)

    parser_add = subs.add_parser("add", help="add a profile")
    parser_add.set_defaults(cmd=cmd_add)
    parser_add.add_argument("name", type=str, help="name of the profile to add")

    parser_add = subs.add_parser("remove", help="delete a profile")
    parser_add.set_defaults(cmd=cmd_remove)
    parser_add.add_argument("name", type=str, help="name of the profile to remove")
    parser_add.add_argument(
        "--hard", action="store_true", help="remove data from clients directory too"
    )

    parser_conf = subs.add_parser(
        "conf", help="get the wireguard configuration to be used by `wg addconf`"
    )
    parser_conf.set_defaults(cmd=cmd_conf)

    args = parser.parse_args()
    return args.cmd(parser, args)


if __name__ == "__main__":
    exit(main(argv))
