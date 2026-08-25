#include "utf8.h"
#include <cstdio>

void test1() {
  printf("[test1]\n");

  const char *str = u8"abc中国こんにちは";

  printf("is-valid-utf8-str = %s\n", utf8::isValid(str) ? "true" : "false");

  auto codepointArray = utf8::decode(str);
  printf("[codepoints]\n");
  for (auto &codepoint : codepointArray) {
    printf("U+%04X\n", codepoint);
  }
}

#define EndStr  '\0'

void test2() {
  printf("\n[test2]\n");

  std::vector<uint8_t> bytes{'a', 'b', 'c', 0xE4, 0xB8, 0xAD, 0xE5, 0x9B, 0xBD, EndStr};
  const char *str = reinterpret_cast<const char *>(bytes.data());

  printf("is-valid-utf8-str = %s\n", utf8::isValid(str) ? "true" : "false");

  auto codepointArray = utf8::decode(str);
  printf("[codepoints]\n");
  for (auto &codepoint : codepointArray) {
    printf("U+%04X\n", codepoint);
  }
}

#define InvalidEncoding 0xFF

void test3() {
  printf("\n[test3: decode invalid utf8 string with error recovery.]\n");

  std::vector<uint8_t> bytes{0x61, 0x62, 0x63, InvalidEncoding, InvalidEncoding, 0xE4, 0xB8, 0xAD, 0xE5, 0x9B, 0xBD, EndStr};
  const char *str = reinterpret_cast<const char *>(bytes.data());

  size_t valid_size;
  printf("is-valid-utf8-str = %s", utf8::isValid(str, valid_size) ? "true" : "false");
  printf(", string-size = %lld, valid-size = %lld\n", bytes.size(), valid_size);

  // Decode invalid utf8 string with error recovery.
  auto codepointArray = utf8::decode(str, 0xFFFD);
  printf("[codepoints]\n");
  for (auto &codepoint : codepointArray) {
    printf("U+%04X\n", codepoint);
  }
}

int main() {
  test1();
  test2();
  test3();
  return 0;
}
