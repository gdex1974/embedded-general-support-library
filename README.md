# General Support Library 

This library contains simple entities useful in embedded development.
The root CMakeLists.txt file is compatible with esp-idf environment.

## MemoryView

An std::string_view-like class generalized for different types, more or less like std::span from C++20.

## BufferedOut

A stream-like class to be used for printing to buffer.

## PersistentStorage

A manager above the memory region intended to be persistent allows store and load objects by name.
Useful with ESP32-like devices.