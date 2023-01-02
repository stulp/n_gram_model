#![allow(dead_code)]
#![allow(unused_imports)]
#![allow(unused_variables)]
#![allow(unreachable_code)]

use rand::Rng;
use std::collections::HashMap;
use std::env;
use std::error::Error;
use std::fs;

#[derive(Debug)]
struct FreqTable {
    table: HashMap<String, i32>,
    n_observations: i32,
}

impl FreqTable {
    fn new(word: String) -> FreqTable {
        FreqTable {
            table: HashMap::from([(word, 1)]),
            n_observations: 1,
        }
    }

    fn observed(&mut self, word: String) {
        self.table
            .entry(word)
            .and_modify(|counter| *counter += 1)
            .or_insert(1);
        self.n_observations += 1;
    }

    fn get_random_word(&self) -> String {
        // Only one candidate word in table; simply return that one (simple optimization)
        if self.n_observations == 1 {
            for (next_word, count) in &self.table {
                return next_word.to_string(); // Return first element in this "loop"
            }
        }

        // Multiple candidates; select randomly, weighted by probability
        let r = rand::thread_rng().gen_range(0..self.n_observations);
        let mut cumul = 0;
        for (next_word, count) in &self.table {
            cumul += count;
            if r < cumul {
                return next_word.to_string();
            }
        }

        ".".to_string() //Something went wrong, return "." to end sentence.
    }
}

fn train_n_gram_model(text_corpus: &Vec<&str>, n: usize) -> HashMap<String, FreqTable> {
    let mut hm: HashMap<String, FreqTable> = HashMap::new();
    for i in n..text_corpus.len() {
        let ngram = text_corpus[i - n..i].join(" ");
        let next_word = text_corpus[i].to_string();
        if let Some(ft) = hm.get_mut(&ngram) {
            ft.observed(next_word)
        } else {
            hm.insert(ngram, FreqTable::new(next_word));
        }
    }
    return hm;
}

fn generate_sentence(
    n_gram_model: &HashMap<String, FreqTable>,
    n: usize,
    seed_words: &Vec<String>,
) -> String {
    let mut sentence = seed_words.clone();
    let mut len = sentence.len();

    let mut next_word = "".to_string();
    while next_word.ne(".") && len < 100 {
        len = sentence.len();
        let ngram = sentence[len - n..len].join(" ");
        let freq_table_result = n_gram_model.get(&ngram);
        next_word = match freq_table_result {
            Some(freq_table) => freq_table.get_random_word(),
            None => ".".to_string(), // End the sentence
        };
        sentence.push(next_word.clone());
    }
    return sentence.join(" ");
}

fn process_arguments(env_args: env::Args) -> Result<(String, usize, Vec<String>), String> {
    let args: Vec<String> = env_args.collect();

    let default_filename = "data/sherlock_holmes.txt".to_string();
    let filename = if args.len() > 1 {
        args[1].to_string()
    } else {
        default_filename
    };
    let n: usize = if args.len() > 2 {
        args[2].parse().unwrap()
    } else {
        2
    };
    let seed_sentence = if args.len() > 3 {
        args[3].to_string()
    } else {
        "i confess".to_string()
    };
    let mut seed_words: Vec<String> = Vec::new();
    for word in seed_sentence.split_whitespace() {
        seed_words.push(word.to_string());
    }

    if seed_words.len() < n {
        return Err("'seed_sentence' should have at least {n} words".to_string());
        //, but '"+seed_sentence+"' has length "+seed_words.size()+".");
    }

    Ok((filename, n, seed_words))
}

fn main() {
    let (filename, n, seed_words) = process_arguments(env::args()).unwrap();

    let text_corpus = fs::read_to_string(filename).expect("Something went wrong reading the file");

    let text_corpus: Vec<&str> = text_corpus.split_whitespace().collect();

    let n_gram_model: HashMap<String, FreqTable> = train_n_gram_model(&text_corpus, n);

    for _ in 0..10 {
        println!("{:?}", generate_sentence(&n_gram_model, n, &seed_words));
    }
}
