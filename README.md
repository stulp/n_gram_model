# Using n-gram models to write new Sherlock Holmes adventures

## What is an n-gram model?

Wikipedia: https://en.wikipedia.org/wiki/N-gram

An n-gram is a sequence of n words found in a text. In the complete works on Sherlock Holmes by Arthur Conan Doyle for instance, the 2-gram "I confess" occurs 15 times. An n-gram model is a probabilistic model of which word will follow next after the n-gram. For instance, "I confess" is followed by the word "how" (1/15), "that" (11/15), "said" (1/15), "to" (1/15), or "is" (1/15), with the number of occurences in brackets.

An n-gram model can be used to generate new sentences, i.e. by taking a seed sentence such as "i confess", and probabilisitically finding the next word. Let's say the next word is "that" (by far the most likely one with probability 11/15). Then take the 2-gram "confess that" (with table "i" (12), "the" (2), "it (1)) to generate the next word, e.g. "i". This continues until a sentence ends with a punctuation mark. This may yield a sentence such as "i confess that i was saving considerable sums of money ." below some more examples:

* i confess that i was completely mistaken .
* i was irresistibly reminded of a skeleton .
* i'm not rich but still i had made a very serious case .
* holmes is a very stout florid faced elderly gentleman with fiery red hair .
* holmes is a myth .
* holmes was certainly not one who realizes the greatness of the mormons .

Note that I hand-picked these sentences. Often the model generates grammatically incorrect sentences such as:

* 2-gram: "i confess" that i depend upon your toe caps is quite a jump in his velveteen tunic .
* 2-gram: "i confess" that the whole pacific slope .
* 4-gram: "i confess that i" was of the opinion that stangerson was concerned in the death of drebber . (this is * 
* 1-gram: "i confess" now though he answered mr john ferrier's property to be useful life appears to england without a constancy almost womanly caress .
a literaly sentence from a book; not very creative)
* 1-gram: "holmes" john openshaw unbreakable tire and have heard him nearly half an absolute reliability is the farm he shouted to marry her to be an ordinary pills which i had come with a little more extraordinary matters little mystery in the generations who were many of the afternoon and made a criminal .

In general higher n leads to a higher probability of a grammatically correct sentence, but "less creative" sentences. 

## Background

During a Corona-quarantaine over Christmas, I was reorganizing directory structures and backups, and came across various programs I wrote in the early 2000s (e.g. [ChordFinder](https://github.com/stulp/chordfinder) and [LineFractals](https://github.com/stulp/linefractals)), including this small n-gram program in C++ (I think it was part of a course on language models). I ported the code to Python also as I was curious how much easier it would be. As I'm learning Rust, I ported it to Rust also. For good measure, I added Java also. By then if had become a more of personal project to understand advantages/disadvantages of different languages. Also, I have a theory that one becomes a better programmer when frequently switching between languages; this was a good exercise in that sense.

## Musings

At one end of a spectrum of complexity, a n-gram-model does not "understand" the sentences it generates. At the other end, humans do "understand" what these sentences and the individual words in them mean. Somewhere in the middle lies [ChatGPT](https://openai.com/blog/chatgpt/). 

In terms of performance and complexity. comparisons between ChatGPT and the code in this repo are rather absurd. But deep down, they both are both based on statistical models of language that are generated from large text corpora (note that ChatGPT adds an RL component to the pure supervised learning approach). Does this preclude ChatGPT from "understanding" the sentences it generates? My intuition is "yes", i.e. ChatGPT does not understand its sentences. And if you believe that ChatGPT does understand its sentences, would you still argue that an n-gram-model does not? What is the difference? (There are many! It's a question meant as food for thought, not for me to answer here).

A crucial difference between humans and these statistical language models is that we learn to associate words/sentences with the outside world. I.e. a "hair" is not just a string with four characters in it, it is something we associate with growing on animals and on people's heads (except bald people), something that we need to wash, and that there are hairdressers for making it look good, i.e. all concepts and processes that exist in the outside world. The outside world is also important to determine whether statements are true or not, e.g. "My hair is fiery red." is incorrect, because my hair is not. This is a problem for statistical approaches, because they aim at generating likely sentences (given the text corpus), not generating ones that are true, e.g. "ChatGPT sometimes writes plausible-sounding but incorrect or nonsensical answers." [1](https://openai.com/blog/chatgpt/). 

In this context, I find "holmes is a myth ." to be a profound statement by the n-gram-model, because it is true in the outside world, but not true in the Sherlock Holmes books. It has learned to think beyond the confines of the book! ;-)  On the other hand "holmes is a very stout florid faced elderly gentleman with fiery red hair ." is false within the books. As n-gram-model says: "i confess that i was completely mistaken ."

In fact, n_gram_model.py can generate the code necessary to generate such sentences! By simply calling `python3 n_gram_model.py n_gram_model.py 4 "import sys import random"` (which contains no information about n_gram_model!) the script generates code that has the same functionality as the script itself (including the comments!). Try it. In fact, the code that is generated can generate itself again. And this ad infinitum. Such strange loops are what Douglas Hofstadter considers to be the basis for conciousness. Just saying.

Also the question of copyright pops up here. The novel sentences it generates are new works, inspired by the Sherlock Holmes stories. But especially for large "n" it becomes more likely that sentences are literally quoted from the stories, and at some point, the stories themselves are reproduced verbatim. Luckily, [Sherlock Holmes stories have entered the public domain](https://www.rollingstone.com/culture/culture-news/metropolis-sherlock-holmes-public-domain-day-2023-1234654562/) (which I why I can include the text corpus in the repo), but the debate on this is timely given ChatGPT and [Stable Difusion](https://stablediffusionweb.com/).

## Documentation of functions

The functions are not documented with docstrings etc, because it's a bit tedious porting it between languages. And I wanted to keep the code nice and compact. For good measure, here are the function signatures and their documentation.

* `FreqTable`
    * A class to represent a frequency table, i.e. mapping words to how often they occured.
* `FreqTable::__init__(self, word: str = None)`
    * Initialize a frequency table with a word (or no word at all)
* `FreqTable::observed(self, word: str)`
    * Inform the frequency table that a certain word was observed.
* `FreqTable::get_random_word(self) -> str`
    * Get a random word from the table, where the probability is weighted with the number of occurences.
* `file_read_to_string(filename: str) -> str`
    * Read a file into one large string
* `train_n_gram_model(text_corpus: str, n: int) -> Dict[str, FreqTable]`
    * Train a n-gram-mode with a text_corpus and "n" value.
    * The return type maps strings with n words, e.g. "i confess" to a frequency table for the words that followed after "i confess".
* `generate_sentence(n_gram_model: Dict[str, FreqTable], n: int, seed_words: List[str]) -> str`
    * Generate a sentence from an n-gram-model, given some initial seed words
    * Keeps predicting new words with get_random_word(), until a punctuation mark (.!?) is found.
    * The length of seed_words must at least "n" 

## Remarks on the languages

* I have optimized for similar code between the different languages. This is most obvious in the Java code, which does not use the standard camelCased functions and variable names, and where the n-gram-model would probably be implemented as a class, rather having static functions all over the place.
* I've not used the latest versions of all languages here (e.g. not all features of C++20 or Java SE 17), so I am not exploting all recent features of those languages.
* In C++, it's annoying to have to overload `<<` for things like `vector`. Perhaps this is solved in newer versions of C++ (?)
* Rust enforces package structure more than others, e.g. there must be a Cargo.toml in the root.

### Todo

* C++: upgrade up to C++20
    * use slice instead of deque/vector: https://stackoverflow.com/questions/50549611/slicing-a-vector-in-c
    * use auto keyword
    * avoid ugly iterators
    * C++17 structured binding
* Java: upgrade to Java SE 17.
* Python: new style type annotations

## Appendix: more samples of generated sentences

* holmes was not an english paper at all ? (*here the, question marks seems to indicate n_gram_model is not sure that this is a true statement*)
* holmes was certainly not one who realizes the greatness of the mormons .
* holmes is a very stout florid faced elderly gentleman with fiery red hair .
* holmes desired to be considerably taller .
* holmes was certainly not a common experience among employers in this age .
* holmes was certainly not very communicative during the last entry ?
* holmes was certainly not one of the dirtiest and most daring criminals of london bridge .
* your hair is of the highest pitch of expectancy there was some information in them which would not have made myself clear ?
* i'm not rich but still i had made a very serious case . (*n_gram_model, this is certainly true*)
* i confess that i was saving considerable sums of money .
* i was irresistibly reminded of a skeleton . (*n_gram_model displaying associative thinking*)
* i was inclined to think that those seven weeks represented the difference between a stradivarius and an engagement  . (*I'm not sure what it means, but it sounds profound*)
* i am not mistaken .
* i confess that i was completely mistaken .
* i confess that i miss my rubber . (*n_gram_model! come on*)