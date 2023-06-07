#ifndef create_data_H
#define create_date_H

#include <iostream>
#include <random>
#include <vector>
#include <string>

class ZipfianStringGenerator {
public:
    ZipfianStringGenerator(int numStrings, double zipfianAlpha, int stringLength)
        : numStrings_(numStrings), zipfianAlpha_(zipfianAlpha), stringLength_(stringLength) {
        generateStrings();
    }

    std::string getRandomString() {
        int index = generateZipfianIndex();
        return strings_[index];
    }

private:
    int numStrings_;
    double zipfianAlpha_;
    int stringLength_;
    std::vector<std::string> strings_;

    std::string generateRandomString(int length) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> dis('a', 'z');

        std::string result;
        for (int i = 0; i < length; ++i) {
            result.push_back(static_cast<char>(dis(gen)));
        }
        return result;
    }

    int generateZipfianIndex() {
        static std::mt19937 generator(std::random_device{}());
        static std::uniform_real_distribution<> distribution(0.0, 1.0);

        double sumProb = 0.0;
        for (int i = 0; i < numStrings_; ++i) {
            sumProb += 1.0 / pow(i + 1, zipfianAlpha_);
        }

        double target = distribution(generator) * sumProb;
        double cumulativeProb = 0.0;
        for (int i = 0; i < numStrings_; ++i) {
            cumulativeProb += 1.0 / pow(i + 1, zipfianAlpha_);
            if (cumulativeProb >= target) {
                return i;
            }
        }

        return numStrings_ - 1;
    }

    void generateStrings() {
        strings_.resize(numStrings_);
        for (int i = 0; i < numStrings_; ++i) {
            strings_[i] = generateRandomString(stringLength_);
        }
    }
};
#endif