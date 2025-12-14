#include <iostream>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <thread>
#include <future>

std::vector<size_t> read_array() {
    size_t length;
    unsigned long long a, b, p;
    std::cin >> length >> a >> b >> p;
    std::vector<size_t> result(length);
    if (length == 0) return result;
    result[0] = static_cast<size_t>(a % p);
    for (size_t i = 1; i < result.size(); ++i) {
        unsigned long long prev = static_cast<unsigned long long>(result[i - 1]);
        unsigned long long val = (prev * a + b) % p;
        result[i] = static_cast<size_t>(val);
    }
    return result;
}

std::vector<size_t> merge_vectors(const std::vector<size_t>& left, const std::vector<size_t>& right) {
    std::vector<size_t> result;
    result.reserve(left.size() + right.size());
    size_t i = 0, j = 0;
    while (i < left.size() && j < right.size()) {
        if (left[i] < right[j]) result.push_back(left[i++]);
        else result.push_back(right[j++]);
    }
    while (i < left.size()) result.push_back(left[i++]);
    while (j < right.size()) result.push_back(right[j++]);
    return result;
}

std::vector<size_t> parallel_sort(std::vector<size_t>& arr, size_t num_threads) {
    if (arr.size() < 2) return arr;

    if (num_threads == 0) {
        num_threads = std::thread::hardware_concurrency();
    }
    if (num_threads == 0) num_threads = 2;
    if (num_threads > arr.size()) num_threads = arr.size();
    if (num_threads == 0) num_threads = 1;

    size_t block_size = (arr.size() + num_threads - 1) / num_threads;
    std::vector<std::future<std::vector<size_t>>> futures;

    for (size_t start = 0; start < arr.size(); start += block_size) {
        size_t end = std::min(start + block_size, arr.size());
        futures.push_back(std::async(std::launch::async, [start, end, &arr]() {
            std::vector<size_t> part(arr.begin() + static_cast<std::ptrdiff_t>(start),
                                     arr.begin() + static_cast<std::ptrdiff_t>(end));
            std::sort(part.begin(), part.end());
            return part;
        }));
    }

    std::vector<std::vector<size_t>> sorted_parts;
    sorted_parts.reserve(futures.size());
    for (auto &fut : futures) {
        sorted_parts.push_back(fut.get());
    }

    while (sorted_parts.size() > 1) {
        std::vector<std::vector<size_t>> new_parts;
        for (size_t i = 0; i + 1 < sorted_parts.size(); i += 2) {
            new_parts.push_back(merge_vectors(sorted_parts[i], sorted_parts[i + 1]));
        }
        if (sorted_parts.size() % 2 == 1) {
            new_parts.push_back(sorted_parts.back());
        }
        sorted_parts = std::move(new_parts);
    }

    return sorted_parts[0];
}

int main() {
    auto array = read_array();

    size_t num_threads = std::thread::hardware_concurrency();
    array = parallel_sort(array, num_threads);

    size_t k;
    std::cin >> k;
    for (size_t i = k - 1; i < array.size(); i += k) {
        std::cout << array[i] << ' ';
    }
    std::cout << "\n";

    return 0;
}