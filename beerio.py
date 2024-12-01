from functools import cache
from typing import NamedTuple, Self, Iterator
from math import ceil, floor


class Round(NamedTuple):
    players: int
    races: int


class BracketChoice(NamedTuple):
    round: Round
    next: list["BracketChoice"]


class Bracket(list[Round]):
    def num_races(self) -> int:
        return sum([r.races for r in self])

    def num_beers(self) -> int:
        return sum([r.players for r in self])


@cache
def crunch(num_players: int, max_per_race: int) -> list[BracketChoice]:
    if num_players == 1:
        return []
    out = []
    for per_race in range(2, min(max_per_race + 1, num_players + 1)):
        for proceed in range(1, min(per_race, num_players + 1)):
            full_races, leftover = divmod(num_players, per_race)
            next_players = full_races * proceed + leftover
            out.append(
                BracketChoice(
                    Round(num_players, ceil(num_players / per_race)),
                    crunch(next_players, max_per_race),
                )
            )
    return out


def resolve_bracket(bs: list[BracketChoice]) -> Iterator[Bracket]:
    if not bs:
        yield Bracket([])
    for round, sub in bs:
        for out in resolve_bracket(sub):
            yield Bracket([round] + out)


for brack in sorted(
    filter(lambda b: True, resolve_bracket(crunch(64, 4))),
    key=lambda b: b.num_races(),
):
    print(
        f"{brack.num_races() * 5}min {brack.num_beers()}beer",
        *[f"{p}/{r}" for p, r in brack],
    )
