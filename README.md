# Logger #
Minimalistic Cpp logger

## Rules ##
Logger need setting output stream
Logger has to levels:
 - release - log only error messages
 - debug - log all messages

## Using ##
### Cmake configuration: ###
```cmake
add_subdirectory(${to_logger_path})
include_directories(${to_logger_path}/include)
# may output log with time
add_compile_definitions(LOG_WITH_TIME)
```
### In code usage: ###
```cpp
// set output stream (default is nothing)
Logger::setOutput(std::cout);

// set logger lvl
// default is release
Logger::setLogLvl(/* some log lvl */);

// RAII object to set debug log level
LoggerDebug _logger_debug;

// log with info mode
LOGI() << "Some msg";

// log with warning mode
LOGW() << "Some msg";

// log with error mode
LOGE() << "Some msg";
```
