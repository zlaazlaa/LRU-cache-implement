#include <iostream>
#include <vector>
#include <random>
#include <string>

int job_num = 1000; // 假设总共有1000个字符串
int key_len = 5;    // 假设每个字符串的长度为5
int value_len =
int key_range = 1000;

std::vector<std::string> keys(job_num);
std::vector<std::string> values(job_num);

void generate_random_data() {
    std::vector<std::string> keys_base(key_range);
    std::vector<std::string> values_base(key_range);
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(97, 122); // 生成小写字母的 ASCII 值范围
    std::uniform_int_distribution<int> distribution2(1, key_range - 1);
    for (int i = 0; i < key_range; ++i) {
        std::string randomString;
        for (int j = 0; j < key_len; ++j) {
            randomString += static_cast<char>(distribution(generator));
        }
        keys_base[i] = randomString;
    }
    for (int i = 0; i < key_range; ++i) {
        std::string randomString;
        for (int j = 0; j < value_len; ++j) {
            randomString += static_cast<char>(distribution(generator));
        }
        values_base[i] = randomString;
    }
    for (int i = 0; i < job_num; ++i) {
        int randomIndex = distribution2(generator);
        keys[i] = keys_base[randomIndex];
    }
    for (int i = 0; i < job_num; ++i) {
        int randomIndex = distribution2(generator);
        values[i] = values_base[randomIndex];
    }
}

int main() {
    generate_random_data();

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(0, job_num - 1);

    for (const std::string& key : values) {
        std::cout << key << std::endl;
    }

    return 0;
}
