import argparse
import random
import sys
import typing
from typing import Dict, List, Tuple


class FreqTable:
    def __init__(self, word: str = None):
        self.table = {word: 1} if word else {}
        self.n_observations = len(self.table)

    def observed(self, word: str):
        if word in self.table:
            self.table[word] += 1
        else:
            self.table[word] = 1
        self.n_observations += 1

    def get_random_word(self) -> str:

        # Only one candidate word in table; simply return that one (simple optimization)
        if self.n_observations == 1:
            return next(iter(self.table.keys()))

        # Multiple candidates; select randomly, weighted by probability
        r = random.randrange(self.n_observations)
        cumul = 0
        for next_word, count in self.table.items():
            cumul += count
            if r < cumul:
                return next_word

        return "."  # Something went wrong, return "." to end sentence.


def read_text_corpus(filename: str) -> List[str]:

    # Read the input data, and turn it into a list of words.
    with open(filename) as f:
        lines = f.readlines()
    return " ".join(lines).split(" ")


def train_n_gram_model(text_corpus: List[str], n: int) -> Dict[str, FreqTable]:

    n_gram_model: Dict[str, FreqTable] = {}

    for ww in range(len(text_corpus) - n):

        # Use the n words as key, and the n+1 word as the value to map to
        n_gram_key_str = " ".join(text_corpus[ww : ww + n])
        next_word = text_corpus[ww + n]

        if not n_gram_key_str in n_gram_model:
            # New n_gram: initialize frequency table for it.
            n_gram_model[n_gram_key_str] = FreqTable(next_word)
        else:
            # n_gram was already observed: update frequency table.
            n_gram_model[n_gram_key_str].observed(next_word)

    return n_gram_model


def generate_sentence(n_gram_model: Dict[str, FreqTable], n: int, seed_words: List[str]) -> str:

    # Start sentence with the seed_words (deep copy)
    words = [w for w in seed_words]

    # Stop once the last word is a punctuation mark
    while not words[-1] in [".", "?", "!"]:

        # Convert last n words to one string, i.e. the key
        n_gram_key_str = " ".join(words[-n:])

        next_word = "."  # Default: end sentence if no next_word found
        if n_gram_key_str in n_gram_model:
            next_word = n_gram_model[n_gram_key_str].get_random_word()

        words.append(next_word)

    return " ".join(words)


def process_arguments() -> Tuple[str, int, List[str]]:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "corpus_filename",
        default="data/sherlock_holmes.txt",
        nargs="?",
        help="file (txt) or directory to read cost vars from",
    )
    parser.add_argument("n", type=int, default=2, nargs="?", help="length of the n-gram, i.e. 'n'")
    parser.add_argument(
        "seed_sentence", default="i confess", nargs="?", help="seed sentence (with #words >= n)"
    )
    args = parser.parse_args()

    seed_words = args.seed_sentence.split(" ")
    if len(seed_words) < args.n:
        raise Exception(
            f"'seed_sentence' should have at least {args.n} words, but '{args.seed_sentence}' has length {len(seed_words)}."
        )

    return args.corpus_filename, args.n, seed_words


def main():

    corpus_filename, n, seed_words = process_arguments()

    text_corpus = read_text_corpus(corpus_filename)

    n_gram_model = train_n_gram_model(text_corpus, n)

    for _ in range(10):
        print(generate_sentence(n_gram_model, n, seed_words))


if __name__ == "__main__":
    main()
