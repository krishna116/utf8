# utf8 is a C++ utf8 string decoding library.

Features:
- Single include file "utf8.h"
- Only provide 4 public APIs.
- Very efficient(using bjoern utf-8 dfa).

## Usage
```C++
#include "utf8.h"
#include <cstdio>

int main(){
  const char *str = u8"abc中国こんにちは";
  printf("is-valid-utf8-str = %s\n", utf8::isValid(str) ? "true" : "false");
  
  auto codepoints = utf8::decode(str);
  printf("[codepoints]\n");
  for (auto &codepoint : codepoints) {
    printf("U+%04X\n", codepoint);
  }
  
  return 0;
}

```
## The 4 Public APIs
```C++
/**
 * Decode utf8 string to codepoints.
 *
 * If an invalid UTF-8 encoding is encountered, the process will stop and
 * return; it will not be automatically corrected and recovery.
 *
 * @param str     Input utf8 str.
 *
 * @return array  Codepoint array.
 */
std::vector<uint32_t> decode(const char* str);

/**
 * Decode utf8 string to codepoints(with error correction and recovery).
 *
 * @param str           Input utf8 str.
 * @param replacement   replacement for invalid utf8 encoding.
 *                      Default REPLACEMENT CHARACTER can be (U+FFFD).
 *
 * @return array        Codepoint array.
 */
std::vector<uint32_t> decode(const char* str, uint32_t replacement);

/**
 * Check whether the input str is a valid UTF-8 string.
 *
 * @param str     Input str.
 *
 * @return true   Valid.
 * @return false  Invalid.
 */
bool isValid(const char* str);

/**
 * Check whether the input str is a valid UTF-8 string.
 *
 * @param str      Input str.
 * @param decoded  Output the successfully decoded str size.
 *
 * @return true    Valid.
 * @return false   Invalid.
 */
bool isValid(const char* str, size_t& decoded);
```

## Reference

http://bjoern.hoehrmann.de/utf-8/decoder/dfa
