#include <fstream>
#include <iostream>
#include <sstream>

#include <string>
#include <vector>
// used to be hash_map, but it is deprecated now.
#include <unordered_map>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Not best practice... But good for brevity, which is more important here.
using namespace std;

template <class _Tp, class _Alloc>
ostream &operator<<(ostream &stream, const vector<_Tp, _Alloc> &v);

template <class _Key, class _Tp, class _HashFcn, class _EqlKey, class _Alloc>
ostream &
operator<<(ostream &stream,
           const unordered_map<_Key, _Tp, _HashFcn, _EqlKey, _Alloc> &hm);

class FreqTable {
private:
  unordered_map<string, int> table;
  int n_observations;

public:
  FreqTable(string word = "") {
    this->table = unordered_map<string, int>();
    if (!word.empty())
      this->table[word] = 1;
    this->n_observations = this->table.size();
  }

  void observed(string word) {
    if (this->table.find(word) != this->table.end()) {
      this->table[word] += 1;
    } else {
      this->table[word] = 1;
    }
    this->n_observations += 1;
  }

  string get_random_word(void) {

    unordered_map<string, int>::const_iterator it = this->table.begin();

    // Only one candidate word in table; simply return that one (simple
    // optimization)
    if (this->table.size() == 1)
      return it->first; // "first" refers to key, not first element

    // Multiple candidates; select randomly, weighted by probability
    int r = rand() % this->n_observations;
    int cumul = 0;
    while (it != this->table.end()) {
      cumul += it->second;
      if (r < cumul)
        return it->first;
      it++;
    }

    return string("."); // Something went wrong, return "." to end sentence.
  }

  friend ostream &operator<<(ostream &os, const FreqTable &ft);
};

ostream &operator<<(ostream &stream, const FreqTable &freq_table) {
  stream << freq_table.table;
  return stream;
}

vector<string> read_text_corpus(string filename) {
  vector<string> text_corpus;

  ifstream file(filename.c_str());
  if (file.bad()) {
    cerr << "ERROR: File '" << filename << "' not found.\n";
    return text_corpus;
  }

  string word;
  while (file >> word)
    text_corpus.push_back(word);
  file.close();

  text_corpus.resize(text_corpus.size() - 1); // Save some memory
  return text_corpus;
}

unordered_map<string, FreqTable>
train_n_gram_model(const vector<string> &text_corpus, int n) {

  unordered_map<string, FreqTable> n_gram_model;

  for (int ww = 0; ww < text_corpus.size() - n; ww++) {

    string n_gram_key_str;
    for (int i = 0; i < n; i++)
      n_gram_key_str += text_corpus[ww + i] + " ";
    string next_word = text_corpus[ww + n];

    if (n_gram_model.count(n_gram_key_str) == 0) {
      n_gram_model[n_gram_key_str] = FreqTable(next_word);
    } else {
      n_gram_model[n_gram_key_str].observed(next_word);
    }
  }

  return n_gram_model;
}

bool is_punctuation_mark(string s) {
  if (s == string("."))
    return true;
  if (s == string("!"))
    return true;
  if (s == string("?"))
    return true;
  return false;
}

vector<string>
generate_sentence(const unordered_map<string, FreqTable> &n_gram_model, int n,
                  const vector<string> &seed_words) {

  // Start sentence with the seed_words (deep copy)
  vector<string> words;
  vector<string>::const_iterator it;
  for (it = seed_words.begin(); it != seed_words.end(); it++)
    words.push_back(*it);

  // Stop once the last word is a punctuation mark
  while (!is_punctuation_mark(words.back())) {

    // Convert last n words to one string, i.e. the key
    string n_gram_key_str;
    for (int i = -n; i < 0; i++)
      n_gram_key_str += words[words.size() + i] + " ";

    string next_word("."); // Default: end sentence if no next_word found
    if (n_gram_model.find(n_gram_key_str) != n_gram_model.end()) {
      FreqTable freq_table = n_gram_model.at(n_gram_key_str);
      next_word = freq_table.get_random_word();
    }

    words.push_back(next_word);
  }
  return words;
}

tuple<string, int, vector<string>> process_arguments(int n_args, char **args) {

  string filename = (n_args > 1 ? args[1] : "data/sherlock_holmes.txt");
  int n = (n_args > 2 ? atoi(args[2]) : 2);
  string seed_sentence = (n_args > 3 ? args[3] : string("i confess"));

  // Read seed_sentence word by word into seed_words
  vector<string> seed_words;
  string word;
  istringstream sstream(seed_sentence);
  while (sstream >> word)
    seed_words.push_back(word);

  if (seed_words.size() < n) {
    ostringstream message;
    message << "'seed_sentence' should have at least " << n << " words";
    message << ", but '" << seed_sentence << "' has length "
            << seed_words.size() << ".";
    throw runtime_error(message.str());
  }

  return make_tuple(filename, n, seed_words);
}

int main(int n_args, char **args) {
  srand(time(NULL));

  string filename;
  int n;
  vector<string> seed_words;
  tie(filename, n, seed_words) = process_arguments(n_args, args);

  vector<string> text_corpus = read_text_corpus(filename);

  unordered_map<string, FreqTable> n_gram_model =
      train_n_gram_model(text_corpus, n);

  for (int s = 0; s < 10; s++)
    cout << generate_sentence(n_gram_model, n, seed_words) << endl;
}

template <class _Tp, class _Alloc>
ostream &operator<<(ostream &stream, const vector<_Tp, _Alloc> &v) {

  typename vector<_Tp>::const_iterator it;
  int count = 0;
  for (it = v.begin(); it != v.end(); it++) {
    if (it != v.begin())
      stream << " ";
    stream << *it;
  }

  return stream;
}

template <class _Key, class _Tp, class _HashFcn, class _EqlKey, class _Alloc>
ostream &
operator<<(ostream &stream,
           const unordered_map<_Key, _Tp, _HashFcn, _EqlKey, _Alloc> &hm) {
  typename unordered_map<_Key, _Tp>::const_iterator it;
  stream << "unordered_map<size=" << hm.size() << ">[";
  for (it = hm.begin(); it != hm.end(); it++) {
    if (it != hm.begin())
      stream << ", ";
    stream << it->first << "=" << it->second;
  }
  stream << "]";
  return stream;
}