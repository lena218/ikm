#include "string_compressor.h"

#include <string>

StringCompressor::StringCompressor(std::string input)
    : input_string_(std::move(input)) {}

void StringCompressor::SetInputString(std::string input) {
    input_string_ = std::move(input);
    result_.reset();
}

std::string StringCompressor::CompressOnce(const std::string& str) const {
    if (str.empty()) return "";

    std::string result;
    size_t i = 0;
    while (i < str.length()) {
        char current = str[i];
        int count = 1;
        while (i + count < str.length() && str[i + count] == current)
            ++count;

        if (count > 1) {
            result += current;
            result += std::to_string(count);
        } else {
            result += current;
        }
        i += count;
    }
    return result;
}

void StringCompressor::Compress() {
    if (input_string_.empty()) {
        throw CompressionException("Input string is empty");
    }

    for (char c : input_string_) {
        bool is_latin = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
        if (!is_latin) {
            throw CompressionException(std::string("Invalid character: ") + c);
        }
    }

    // Запускаем цикл повторного сжатия, пока это возможно - нужно по заданию
    std::string current_str = input_string_;
    while (true) {
        std::string next_str = CompressOnce(current_str);
        
        if (next_str == current_str) {
            break;
        }
        current_str = next_str;
    }

    result_ = current_str;
}

std::string_view StringCompressor::GetInputString() const noexcept {
    return std::string_view(input_string_);
}

std::optional<std::string_view> StringCompressor::GetCompressedResult() const noexcept {
    if (result_.has_value()) {
        return std::string_view(*result_);
    }
    return std::nullopt;
}

bool StringCompressor::WasCompressed() const noexcept {
    if (!result_.has_value() || input_string_.empty()) {
        return false;
    }
    return input_string_ != *result_;
}

void StringCompressor::Clear() noexcept {
    input_string_.clear();
    result_.reset();
}
