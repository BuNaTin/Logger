#pragma once

#include <optional>
#include <array>
#include <sstream>
#include <fstream>
#include <iostream>
#include <string_view>
#include <charconv>

namespace {
	struct FastStackBuffer {
	public:
		FastStackBuffer() :data_() {}
		~FastStackBuffer() {}

		std::string_view Data() {
			return std::string_view(data_.data(), data_.size());
		}

		template <class T>
		void PutNum(const T& value) {
			auto [ptr, ec] = std::to_chars(&data_[current_], data_.data() + data_.size(), value);
			current_ = ptr - data_.data();
		}

		void PutException(const std::exception& exc) {
			this->PutStr("Exception: ");
			this->PutStr(exc.what());
		}

		template <class Str>
		void PutStr(const Str& value) {
			int i;
			for (i = 0; value[i] != '\0'; ++i) {
				data_[current_ + i] = value[i];
			}
			current_ += i;
		}

		void PutChar(char value) {
			data_[current_++] = value;
		}

	private:
		int current_ = 0;
		std::array<char, 512> data_;
	};

	struct BufferStd final : public std::streambuf {
		explicit BufferStd(FastStackBuffer& impl):impl_(impl) {}
	private:
		int_type overflow(int_type c) override {
			impl_.PutChar(c);
			return c;
		}
		std::streamsize xsputn(const char_type* s, std::streamsize n) override {
			impl_.PutStr(s);
			return sizeof(s);
		}

		FastStackBuffer& impl_;
	};

}

class Logger {
public:
	enum class lvl {
		release = 0,
		debug = 1,
	};
public:
	static lvl log_lvl;
public:
	Logger() :buffer_{} {

	}
	~Logger() {
		buffer_.PutChar('\n');
		*output << buffer_.Data();
	}

	static void setLogLvl(int value) {
		log_lvl = static_cast<lvl>(value);
	}
	static void setLogLvl(lvl value) {
		log_lvl = value;
	}
	template <class T>
	Logger& operator<<(const T& value);
private:
	static std::ostream* output;
	FastStackBuffer buffer_;
	struct LazyInitedStream {
		BufferStd sbuf;
		std::ostream out;
		explicit LazyInitedStream(FastStackBuffer& impl) : sbuf(impl), out(&sbuf) {}
	};
	std::optional<LazyInitedStream> lazy_;
	std::ostream& Stream() {
		if (!lazy_) {
			lazy_.emplace(buffer_);
		}
		return lazy_->out;
	}
};


template<class T>
Logger& Logger::operator<<(const T& value) {
	if constexpr (std::is_constructible<std::string_view, T>::value) {
		buffer_.PutStr(value);
	}
	else if constexpr (std::is_base_of<std::exception, T>::value) {
		buffer_.PutException(value);
	}
	else if constexpr (std::is_same<char, T>::value) {
		buffer_.PutChar(value);
	}
	else if constexpr (std::is_same<int, T>::value) {
		buffer_.PutNum(value);
	}
	else if constexpr (std::is_same<float, T>::value) {
		buffer_.PutNum(value);
	}
	else if constexpr (std::is_same<double, T>::value) {
		buffer_.PutNum(value);
	}
	else {
		Stream() << value;
	}
	return *this;
}

Logger::lvl Logger::log_lvl = lvl::debug;

#if defined(FILE_LOG)
std::ostream* Logger::output = &std::ofstream("_logger.out");
#else
std::ostream* Logger::output = &std::cout;
#endif

#define LOGI() if(Logger::lvl::debug == Logger::log_lvl) \
	Logger()<<"[INFO] "

#define LOGW() if(Logger::lvl::debug == Logger::log_lvl) \
	Logger()<<"[WARN] "

#define LOGE() Logger()<<"[ERR] "
