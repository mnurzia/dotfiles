from argparse import ArgumentParser, FileType
from re import finditer, M
from sys import stdin, stdout

VOCAB_REGEX = r"^\s*- (.*\*(.*)\*.*)$"

if __name__ == "__main__":
  ap = ArgumentParser(description="parse a markdown file for definitions")
  ap.add_argument("input", type=FileType('r'), default=stdin, nargs='?')
  ap.add_argument("output", type=FileType('w'), default=stdout, nargs='?')
  ap.add_argument("-d", "--definition", action="store_true")

  args = ap.parse_args()

  for m in finditer(VOCAB_REGEX, args.input.read(), M):
    line, word = m.groups()
    if args.definition:
      print(line.replace(word, "________"), file=args.output)
    else:
      print(word, file=args.output)
