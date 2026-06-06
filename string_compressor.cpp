#include "string_compressor.h"

#include <string>

namespace string_compressor {

constexpr int kMaxIterations = 1000;
constexpr char kLatinLowerStart = 'a';
constexpr char kLatinLowerEnd = 'z';
constexpr char kLatinUpperStart = 'A';
constexpr char kLatinUpperEnd = 'Z';

CompressionException::CompressionException(const std::string& message)
    : std::runtime_error(message) {}

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
    bool is_latin = (c >= kLatinLowerStart && c <= kLatinLowerEnd) ||
                    (c >= kLatinUpperStart && c <= kLatinUpperEnd);
    if (!is_latin) {
      throw CompressionException(std::string("Invalid character: ") + c);
    }
  }

  std::string current = input_string_;
  int iterations = 0;

  while (iterations < kMaxIterations) {
    std::string next = CompressOnce(current);
    if (current == next) {
      result_ = std::move(next);
      return;
    }
    current = std::move(next);
    ++iterations;
  }

  throw CompressionException("Max iterations exceeded");
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

}  // namespace string_compressor