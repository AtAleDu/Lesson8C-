#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>

using namespace std;

class DataProcessor {
public:
    virtual void process(const string& input_filename, const string& output_filename) = 0;
};

class WordCounter : public DataProcessor {
public:
    void process(const string& input_filename, const string& output_filename) override {
        ifstream input_file(input_filename);
        if (!input_file.is_open()) {
            cerr << "Не удалось открыть входной файл: " << input_filename << endl;
            return;
        }

        ofstream output_file(output_filename);
        if (!output_file.is_open()) {
            cerr << "Не удалось открыть выходной файл: " << output_filename << endl;
            return;
        }

        string word;
        int word_count = 0;
        while (input_file >> word) {
            ++word_count;
        }

        output_file << "Количество слов: " << word_count << endl;
        cout << "Файл готов " << input_filename << ", количество слов: " << word_count << endl;
    }
};

class TextProcessor {
public:
    TextProcessor(DataProcessor& processor) : processor(processor) {}

    void processFiles(const vector<string>& input_files, const vector<string>& output_files) {
        if (input_files.size() != output_files.size()) {
            cerr << "Количество входных и выходных файлов не совпадает!" << endl;
            return;
        }

        vector<thread> threads;
        threads.reserve(input_files.size());

        for (size_t i = 0; i < input_files.size(); ++i) {
            threads.emplace_back([this, i, &input_files, &output_files]() {
                lock_guard<mutex> lock(mtx);
                processor.process(input_files[i], output_files[i]);
                });
        }

        for (auto& thread : threads) {
            thread.join();
        }
    }

private:
    DataProcessor& processor;
    mutex mtx;// Мьютекс для синхронизации доступа к общим данным
};

int main() {
    WordCounter word_counter;
    TextProcessor text_processor(word_counter);

    vector<string> input_files = { "input1.txt", "input2.txt", "input3.txt" };
    vector<string> output_files = { "output1.txt", "output2.txt", "output3.txt" };

    text_processor.processFiles(input_files, output_files);

    return 0;
}
