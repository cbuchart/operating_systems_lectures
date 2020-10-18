# Spell-checker

Nowadays virtually all text editors have built-in spell-check feature. In this project you will develop your own offline checker module. An offline module means a) it will not require an Internet connection to work, b) will process the whole text at once and generate a report with the corrections.

The goal of this project is to drive you from simple and traditional console application to more sophisticated utilities that can be integrated in more complex workflows.

## Description
Your spell-checker application shall read an arbitrary plain text, and, based on a given dictionary (a file with valid words), generate an HTML document that will highlight errors while preserving the original document structure. Both the input text and the dictionary will correspond with English language.
 
Optionally, following features will be considered for accumulated points for the final exam. Note that you must have at least a 3.5 score in the exam to be able to use this extra points.
1.  For 2.5 points in the exam: The application will suggest the best replacement (or one if several can be considered the bests). This shall be done as a hovering.
 
2.  For 5 points in the exam: The application is a web server or mobile application with form to introduce the text, some kind of "Check spell" button, and shall show the aforementioned HTML document as a result. Note that for this part you can use any additional programming language you want, but the backend module must be your command-line application in C++.

## Language and compiler
-   It shall be developed in C++ (up to C++17), with no third-party libraries, only the C++ standard libraries are allowed.
-   You can use the IDE and compiler you want as long as you specify which one you used and the compilation command line, solution file or makefile necessary to build the project.
-   You can develop for either x86 or x64 platforms. No other architectures, like ARM, are allowed for the core project. Nevertheless, for the second optional part you can use the language and frameworks you consider the most appropriate.

## Input
-   The input text shall be read from standard input.
-   The input text will be in English.
-   The dictionary shall be read from a file, which path will be indicated as a command line argument.
-   The dictionary will contain all words to be marked as correct, but you shall consider additional cases such as possessive quotes (parents', President's...), contractions (didn't, shouldn't...).
-   Abbreviatures will not be considered a valid word unless it appears in the dictionary.
-   Dictionary will contain only characters from a to z.
-   Words with numbers shall be marked as incorrect (such as `p34r`).
-   Digit only words shall be marked as valid (`2020`, `40,000`, `3.141592`...).
-   Expect huge input text files (up to 1GB) and large dictionaries (>350K words). Sample input files and outputs can be found in the [test](test) directory.
-   You can download the dictionary from https://github.com/dwyl/english-words/blob/master/words_alpha.txt

## Output
-   Output shall be formatted as a standard HTML document. You can assume HTML 5 compliance.
-   All the necessary code to render the HTML shall be included into the same document: no external CSS or JavaScript code will be admitted.
-   You can choose the style and format of elements as long as misspelled words are highlighted.

## Rules
-   Source code and build instructions shall be sent through ADI before November 15th at 23:59:59 UTC+1. You can modify your submission until that time span, but no submission will be accepted after that.
-   As usual, execution time is critical for every application, and yours will not be the exception. Your application shall not exceed 5 seconds of execution time. Applications will be run 10 times and minimum time will be considered.
-   The dictionary used will always be the same.
-   You will receive both the dictionary and some test cases.
-   The application will be tested against a group of input files, some of them hidden to you.
-   You must parallelize the spell-checker with as many threads as the computer where it is running allow.
-   Sample execution line: `spell_checker.exe dictionary.txt < test_1.txt > test_1.html`

## Grading
-   The project scores for 20% of Operating System part (it is, 10% of the Course).
-   Total grading is proportional to the number of misspelled words found, minus penalizations, if any.
-   The project will not be graded at all if it does not compile.
-   If maximum execution time is exceeded, you will lose 30% of maximum grade.
-   Code elegance is particularly important: unorganized, not indented code will suppose a penalization of 10%.
-   Unsolved race conditions will suppose 50% of penalization since they may produce an incorrect output.
-   Cheating, plagiarism, or any other form of copy of other's work is a serious violation. It will be sanctioned with the absolute rejection of the project. If it involves two or more projects, all of them will be rejected, independently of who copied who.
-   Submit only the source code, instructions and project files, do not submit the dictionary nor binary files. Doing so will be penalized with a 5% off.
-   If you implement the web server or the mobile application, please submit their code too and contact the professor to arrange a live demonstration.
-   Penalizations also applies to extra points.

## Tips
-   Read about following data structures and algorithms: [std::string](https://en.cppreference.com/w/cpp/string/basic_string), [std::set](https://en.cppreference.com/w/cpp/container/set), trie, Levenshtein distance. They are only suggestions to start to work, it is not mandatory to use them and you can try with any other one you prefer.
-   Use an incremental development. For example: start with a basic application that reads the text and produce an HTML with words of more than 5 characters highlighted, then read the dictionary and do the actual check. Once you have this basic application working, parallelize it and solve the race conditions. Once completed, and only then, optimize your code.
-   Parallelization can be done at many levels: by line, by word
-   Regarding optimization, to do it measure the time with the most demanding input files then try to identify the bottlenecks and solve them (check critical sections are not too strict, data structures, word-comparison algorithms...).
-   Be aware that string comparison is case sensitive (`"alpha" != "AlPha"`).
-   Take care of memory consumption.
-   Every time you have a valid project, upload it. Do not way until the very last minute to send it. There are no excuses for missing the deadline.
-   Once you have the mandatory features, try adding the optional ones.

### Measuring time

You can use the following class to easily measure the execution time of a certain code block:

```
class TicToc
{
  std::chrono::time_point<std::chrono::steady_clock> m_begin;

public:
  TicToc() : m_begin(std::chrono::high_resolution_clock::now()) {}
  ~TicToc()
  {
    const auto end = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - m_begin).count();
    std::clog << duration / 1000.0 << "ms\n";
  }
};
```

Usage:

```
int main() {
   TicToc tic_toe;
   
   // ...
}
```

An extended version of this class can be found in my personal blog (in Spanish): https://headerfiles.com/2020/01/08/una-forma-sencilla-rapida-y-cuasi-automatica-de-medir-tiempos-de-ejecucion-en-funciones/
