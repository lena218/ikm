#ifndef STRING_COMPRESSOR_H_
#define STRING_COMPRESSOR_H_

#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>

class CompressionException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class StringCompressor {
public:
    StringCompressor() = default;
    explicit StringCompressor(std::string input);

    void SetInputString(std::string input);
    void Compress();

    std::string_view GetInputString() const noexcept;
    std::optional<std::string_view> GetCompressedResult() const noexcept;
    bool WasCompressed() const noexcept;
    void Clear() noexcept;

private:
    std::string input_string_;
    std::optional<std::string> result_;

    std::string CompressOnce(const std::string& str) const;
};

#endif
