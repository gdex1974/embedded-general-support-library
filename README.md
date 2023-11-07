# General Support Library 

This library contains simple entities useful in embedded development.

Then I started my own DIY project, I found there are mostly two options for DIY embedded developers: learn a specific low lewel framework based on pure C, like ESP-IDF for ESP32 or HAL for STM32. Or use Arduino framowork for target platform.
And the community code almost entirely use the second approach. It's not very surprising for sure. Arduino is _very_ simple to begin with. With just few lines of code you can not only make the LED blinking, but to attach a temperature sensor and show its readings on a small OLED display for example.
It's really attractive - until your task is not so common. Choose different RTC clock source for ESP32. Go to deep sleep and preserve the driver's data. Attach multiple devices on SPI bus without conflicts. And so on.
And from other side - the platforms native frameworks have few orders of magnitude more complexity. They are not simplicity-oriented and the cultural shock after switching from Arduino to ESP-IDF would not be surprising. You would do things too hard or not available with Arduino, but the rest will be painful.
And there is nothing between.

Here, I started to fill this gap with the class library for the common embedded developers' tasks I faced in my own projects.
Also, it contains a common hardware interface (SPI, I2C, UART, RTC, etc) mapped to the framework-specific implementations.
Currently, there are three ways to use library:
- ESP-IDF framework, can be imported as a component. The CMake file is fully integrated into its native build system.
- PlatformIO framework (Arduino backend). The library.json file is provided, so it can be imported as a library in the PlatformIO projects.
- A standalone CMake project. The CMakeLists.txt can be used as a standalone CMake project, the tests are provided in this way.

## The main concepts

- **Framework-independency**. The vision of this library is to provide a clean and efficient interface to the vendor-specific frameworks and the general classes and functions missed in the standard library.
- **Testability**. The hardware-independent part is covered by GoogleTest-based UTs including embedded graphics library.
- **Simplicity**. The library is designed to be simple and easy to use but not by the cost of efficiency. 
- **Modern language**. The library is written on C++ 17 and use benefits of this standard like optional, variant, string_view, etc.
- **Efficiency** This library doesn't use heap at all. While it's not so important on ESP32, it's very actual on STM32 or other platforms with limited RAM. On such platform removing heap-related code from the runtime significantly reduce the memory footprint for both RAM and ROM and increase the performance.
- **Portability**. The library is designed to be portable. It's not only about the hardware abstraction layer, but also about the code itself. The code is written in a way to be portable between different platforms. The only requirement is the C++17 support.

## MemoryView

Common practice in embedded world is to pass the buffer as a pointer to its beginning and its size as two separate function parameters. MemoryView class encapsulates this information and allow to transfer it atomically what preserves the data integrity.
This is a templated class transparently convertible from C-style array and std::array. Also, it's transparently convertible to std::string_view if the template type compatible.

an example:

```cpp
char data[] = {  'A', '0', '1' };
MemoryView view { data }; // C++17: Type of view is deduced to MemoryView<char>. The size is deduced as 3.
std::string_view result = view; // The underlying types are compatible.
```

## BufferedOut

This class provide a regular C++ stream interface allowing to make the formatted strings inside the buffer. It behaves like simplified std::string_stream, but using externally provided buffer.
It's useful for logging and other cases when the formatted string is required.

an example:

```cpp
std::array<char, 128> buffer;
BufferedOut out { buffer };
out << "Hello, " << 123 << " world!";
```

## PersistentStorage

A manager above the memory region intended to be persistent allows store and load objects by name.
Useful with ESP32-like devices.

an example:

```cpp
struct MyData {
    int a;
    int b;
};

std::array<uint8_t, 1024> buffer {};
PersistentStorage storage { buffer };

MyData testObject = { 1, 2 };
auto result = storage.set("test", testObject1;

EXPECT_TRUE(result);

auto retrievedObject = storage.get<MyData>("test");
ASSERT_TRUE(retrievedObject);
EXPECT_EQ(*retrievedObject, testObject);
```


## Graphics library

This library contains various graphics primitives, fonts and frame buffer.
See the [Readme](graphics/Readme.md) for details.

## GFX Font Converter

This is a console utility to convert the font from the TTF format to the GFX format used by the graphics library.
For the 7 bit (ASCII) charset it's compatible with the Adafruit GFX library.
For the 8 bit charset it's possible to specify the codepage to map one-byte character codes to the Unicode code points for rendering.
The non-printable codes between 0X7E and 0XA0 are omitted to save space.
Another difference is to create two binary files instead of C++ generated code. One for the font itself and another for the glyph metrics.
The utility is written in C++ and uses FreeType library to render the glyphs.
The suppported command line options are:
- -f, --font - the path to the TTF font file
- -s, --size - the output font size in points
- -d, --dpi - the output font resolution in DPI, default 141
- -e, --encoding - charset to use, default is ASCII (7-bit)
- -b - use binary format for the font data (default is generated C++ header)
- -o, --output - the output directory, default the executable's one.

## Hardware-independent interfaces

The library contains the hardware-independent interfaces for the common hardware:
- SpiDevice
  - sendSync() - half-duplex blocking send
  - receiveSync() - half-duplex blocking receive
  - sendAndReceive() - full-duplex blocking send and receive
- I2CDevice
  - sendSync() - blocking send with timeout
  - receiveSync() - blocking receive with timeout
- PacketUart
  - sendSync() - blocking send with timeout
  - receiveSync() - blocking receive with timeout
  - receiveUntil() - blocking receive until the specified byte is received or timeout happened
  - receiveBetween() - blocking receive a packet between specified start and stop bytes, which are included in the result

## Hardware-specific implementations

Currently, ESP32 and generic Arduino are supported. STM32 native implementation is under development.
