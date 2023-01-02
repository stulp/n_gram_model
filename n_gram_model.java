import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Random;

class n_gram_model {

  static class FreqTable {
    private HashMap<String, Integer> table;
    private int n_observations;
    Random random = new Random();

    public FreqTable(String word) {
      this.table = new HashMap<>();
      this.table.put(word, 1);
      this.n_observations = 1;
    }

    public FreqTable() {
      this.table = new HashMap<>();
      this.n_observations = 0;
    }

    public void observed(String next_word) {
      if (this.table.containsKey(next_word)) {
        int val = this.table.get(next_word);
        this.table.put(next_word, val + 1);
      } else {
        this.table.put(next_word, 1);
      }
      this.n_observations += 1;
    }

    public String get_random_word() {

      // Only one candidate word in table; simply return that one (simple optimization)
      if (this.table.size() == 1) this.table.entrySet().iterator().next().getKey();

      // Multiple candidates; select randomly, weighted by probability
      int r = random.nextInt(this.n_observations);
      int cumul = 0;
      for (String next_word : table.keySet()) {
        cumul += table.get(next_word);
        if (r < cumul) return next_word;
      }

      return new String("."); // Something went wrong, return "." to end sentence.
    }

    public String toString() { // Exception: must be camelCased to blend with println()
      return table.toString();
    }
  }

  static HashMap<String, FreqTable> train_n_gram_model(List<String> text_corpus, int n) {
    HashMap<String, FreqTable> n_gram_model_obj = new HashMap<>();

    for (int ww = 0; ww < text_corpus.size() - n; ww++) {

      String n_gram_key_str = new String();
      for (int i = 0; i < n; i++) n_gram_key_str += text_corpus.get(ww + i) + " ";
      String next_word = text_corpus.get(ww + n);

      if (!n_gram_model_obj.containsKey(n_gram_key_str)) {
        n_gram_model_obj.put(n_gram_key_str, new FreqTable(next_word));
      } else {
        n_gram_model_obj.get(n_gram_key_str).observed(next_word);
      }
    }

    return n_gram_model_obj;
  }

  static String generate_sentence(
      HashMap<String, FreqTable> n_gram_model_obj, int n, List<String> seed_words) {

    // Start sentence with the seed_words (deep copy)
    ArrayList<String> words = new ArrayList<>(seed_words);

    // Stop once the last word is a punctuation mark
    List<String> punctuation_marks = Arrays.asList(".", "!", "?");
    while (!punctuation_marks.contains(words.get(words.size() - 1))) {

      // Convert last n words to one string, i.e. the key
      String n_gram_key_str = new String();
      for (int i = -n; i < 0; i++) n_gram_key_str += words.get(words.size() + i) + " ";

      String next_word = new String("."); // Default: end sentence if no next_word found
      if (n_gram_model_obj.containsKey(n_gram_key_str)) {
        FreqTable freq_table = n_gram_model_obj.get(n_gram_key_str);
        next_word = freq_table.get_random_word();
      }
      words.add(next_word);
    }
    return String.join(" ", words);
  }

  static List<String> read_text_corpus(String filename) {
    try {
      String content = Files.readString(Path.of(filename));
      return Arrays.asList(content.split(" "));
    } catch (IOException ex) {
      throw new RuntimeException("ERROR: Could not find filename '" + filename + "'");
    }
  }

  static class Arguments {
    public final String filename;
    public final int n;
    public final List<String> seed_words;

    public Arguments(String filename, int n, List<String> seed_words) {
      this.filename = filename;
      this.n = n;
      this.seed_words = seed_words;
    }
  }

  static Arguments process_arguments(String[] args) {
    String filename = (args.length > 0 ? args[0] : new String("data/sherlock_holmes.txt"));
    int n = (args.length > 1 ? Integer.parseInt(args[1]) : 2);
    String seed_sentence = (args.length > 2 ? args[2] : "i confess");
    List<String> seed_words = Arrays.asList(seed_sentence.split(" "));

    if (seed_words.size() < n)
      throw new RuntimeException(
          "'seed_sentence' should have at least "
              + n
              + " words, but '"
              + seed_sentence
              + "' has length "
              + seed_words.size()
              + ".");

    return new Arguments(filename, n, seed_words);
  }

  public static void main(String[] args_array) {

    Arguments args = process_arguments(args_array);

    List<String> text_corpus = read_text_corpus(args.filename);

    HashMap<String, FreqTable> n_gram_model_obj = train_n_gram_model(text_corpus, args.n);

    for (int s = 0; s < 10; s++)
      System.out.println(generate_sentence(n_gram_model_obj, args.n, args.seed_words));
  }
}
