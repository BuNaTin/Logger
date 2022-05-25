#pragma once

#include <array>
#include <charconv>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string_view>

#include <ctime>

const size_t LOG_BUF_SIZE = 1024;

struct FastStackBuffer {
public:
    FastStackBuffer() : data_() {}
    ~FastStackBuffer() {}

    std::string_view Data() {
        return std::string_view(data_.data(), current_);
    }

    template<class T>
    void PutNum(const T &value) {
        if (current_ >= LOG_BUF_SIZE - 16) {
            return;
        }
        auto [ptr, ec] = std::to_chars(
                &data_[current_], data_.data() + data_.size(), value);
        current_ = ptr - data_.data();
    }

    void PutException(const std::exception &exc) {
        this->PutStr("Exception: ");
        this->PutStr(exc.what());
    }

    template<class Str>
    void PutStr(const Str &value) {
        if (current_ >= LOG_BUF_SIZE - 16) {
            return;
        }
        int i;
        for (i = 0;
             current_ + i < LOG_BUF_SIZE - 16 && value[i] != '\0';
             ++i) {
            data_[current_ + i] = value[i];
        }
        if (current_ + i == LOG_BUF_SIZE - 16 && value[i] != '\0') {
            data_[current_ + i] = ' ';
            ++i;
            data_[current_ + i] = '.';
            ++i;
            data_[current_ + i] = '.';
            ++i;
            data_[current_ + i] = '.';
            ++i;
        }
        current_ += i;
    }

    void PutChar(char value) {
        if (current_ >= LOG_BUF_SIZE - 16) {
            return;
        }
        data_[current_++] = value;
    }

    void addNewLine() { data_[current_++] = '\n'; }

public:
    int current_ = 0;
    std::array<char, LOG_BUF_SIZE> data_;
};

struct BufferStd final : public std::streambuf {
    explicit BufferStd(FastStackBuffer &impl) : impl_(impl) {}

private:
    int_type overflow(int_type c) override {
        impl_.PutChar(c);
        return c;
    }
    std::streamsize xsputn(const char_type *s,
                           std::streamsize n) override {
        impl_.PutStr(s);
        return impl_.current_;
    }

    FastStackBuffer &impl_;
};

class Logger {
public:
    enum class lvl {
        release = 0,
        debug = 1,
    };

public:
    static lvl log_lvl;

public:
    Logger() : buffer_{} {}
    ~Logger() {
        buffer_.addNewLine();
        *output << buffer_.Data();
        output->flush();
    }

    static void setOutput(std::ostream &out) { output = &out; }
    static void setLogLvl(int value) {
        log_lvl = static_cast<lvl>(value);
    }
    static void setLogLvl(lvl value) { log_lvl = value; }
    template<class T>
    Logger &operator<<(const T &value);

private:
    static std::ostream *output;
    FastStackBuffer buffer_;
    struct LazyInitedStream {
        BufferStd sbuf;
        std::ostream out;
        explicit LazyInitedStream(FastStackBuffer &impl)
                : sbuf(impl), out(&sbuf) {}
    };
    std::optional<LazyInitedStream> lazy_;
    std::ostream &Stream() {
        if (!lazy_) {
            lazy_.emplace(buffer_);
        }
        return lazy_->out;
    }
};

template<class T>
Logger &Logger::operator<<(const T &value) {
    // for string_view, string, char[]
    if constexpr (std::is_constructible<std::string_view, T>::value) {
        buffer_.PutStr(value);
    }
    // for std::exceptions
    else if constexpr (std::is_base_of<std::exception, T>::value) {
        buffer_.PutException(value);
    }
    // for char
    else if constexpr (std::is_same<char, T>::value) {
        buffer_.PutChar(value);
    }
    // for nums
    else if constexpr (std::is_same<int16_t, T>::value ||
                       std::is_same<int32_t, T>::value ||
                       std::is_same<int64_t, T>::value ||
                       std::is_same<uint16_t, T>::value ||
                       std::is_same<uint32_t, T>::value ||
                       std::is_same<uint64_t, T>::value) {
        buffer_.PutNum(value);
    }
    // for floats
    else if constexpr (std::is_same<float, T>::value ||
                       std::is_same<double, T>::value) {
        buffer_.PutStr(std::to_string(value));
    }
    // other types
    else {
        Stream() << value;
    }
    return *this;
}
