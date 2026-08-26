#pragma once

#include <cstddef> // for size_t
#include <cstdint> // for uint32_t
#include <vector>  // for vector

namespace utf8{

namespace bjoern {
constexpr uint32_t UTF8_ACCEPT = 0;
constexpr uint32_t UTF8_REJECT = 12;

// Copyright (c) 2008-2009 Bjoern Hoehrmann <bjoern@hoehrmann.de>
// See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.
static const uint8_t utf8d[] = {
  // The first part of the table maps bytes to character classes that
  // to reduce the size of the transition table and create bitmasks.
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
  8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  10,3,3,3,3,3,3,3,3,3,3,3,3,4,3,3, 11,6,6,6,5,8,8,8,8,8,8,8,8,8,8,8,

  // The second part is a transition table that maps a combination
  // of a state of the automaton and a character class to a state.
  0,12,24,36,60,96,84,12,12,12,48,72, 12,12,12,12,12,12,12,12,12,12,12,12,
  12, 0,12,12,12,12,12, 0,12, 0,12,12, 12,24,12,12,12,12,12,24,12,24,12,12,
  12,12,12,12,12,12,12,24,12,12,12,12, 12,24,12,12,12,12,12,12,12,24,12,12,
  12,12,12,12,12,12,12,36,12,36,12,12, 12,36,12,12,12,12,12,36,12,36,12,12,
  12,36,12,12,12,12,12,12,12,12,12,12,
};

inline void decode(uint32_t* state, uint32_t* codepoint, uint32_t byte) {
  uint32_t type = utf8d[byte];
  if(*state != UTF8_ACCEPT) {
    *codepoint = (byte & 0x3fu) | (*codepoint << 6);
  } else {
    *codepoint = (0xff >> type) & (byte);
  }
  *state = utf8d[256 + *state + type];
}

/**
 * Reset the UTF-8 decoder state machine to initial state.
 * Call this after encountering an invalid byte to resume decoding.
 */
inline void reset(uint32_t* state, uint32_t* codepoint) {
  *state = UTF8_ACCEPT;
  *codepoint = 0;
}

} // end namespace bjoern;

/**
 * Check whether the input str is a valid UTF-8 string.
 *
 * @param str     Input str.
 *
 * @return true   Valid.
 * @return false  Invalid.
 */
static bool isValid(const char* str) {
  if(str == nullptr) return false;
  uint32_t codepoint = 0;
  uint32_t state = bjoern::UTF8_ACCEPT;
  auto s = reinterpret_cast<const uint8_t*>(str);
  for (; *s; ++s) {
    bjoern::decode(&state, &codepoint, *s);
    if(state == bjoern::UTF8_REJECT) {
      return false;
    }
  }
  return true;
}

/**
 * Check whether the input str is a valid UTF-8 string.
 *
 * @param str      Input str.
 * @param decoded  Output the successfully decoded str's size.
 *
 * @return true    Valid.
 * @return false   Invalid.
 */
static bool isValid(const char* str, size_t& decoded) {
  decoded = 0;
  if(str == nullptr) return false;
  uint32_t codepoint = 0;
  uint32_t state = bjoern::UTF8_ACCEPT;
  size_t i = 0;
  auto s = reinterpret_cast<const uint8_t*>(str);
  for (; *s; ++s) {
    bjoern::decode(&state, &codepoint, *s);
    if(state == bjoern::UTF8_ACCEPT) {
      decoded = i + 1;
    } else if(state == bjoern::UTF8_REJECT) {
      return false;
    }
    ++i;
  }
  return true;
}

/**
 * Decode utf8 string to codepoints(without error correction and recovery).
 *
 * If an invalid UTF-8 encoding is encountered, the decoding will stop and
 * return; it will not be automatically corrected and recovery.
 *
 * @param str     Input utf8 str.
 *
 * @return array  Codepoint array.
 */
static std::vector<uint32_t> toCodepointArray(const char* str) {
  std::vector<uint32_t> codepointArray;
  if(str == nullptr) return codepointArray;
  uint32_t codepoint = 0;
  uint32_t state = bjoern::UTF8_ACCEPT;
  auto s = reinterpret_cast<const uint8_t*>(str);
  for (; *s; ++s) {
    bjoern::decode(&state, &codepoint, *s);
    if(state == bjoern::UTF8_ACCEPT) {
      codepointArray.push_back(codepoint);
    } else if(state == bjoern::UTF8_REJECT) {
      break;
    }
  }
  return codepointArray;
}

/**
 * Decode utf8 string to codepoints(with error correction and recovery).
 *
 * @param str           Input utf8 str.
 * @param replacement   replacement for invalid utf8 encoding.
 *                      The default REPLACEMENT CHARACTER is (U+FFFD).
 *
 * @return array        Codepoint array.
 */
static std::vector<uint32_t> toCodepointArray(const char* str, uint32_t replacement) {
  std::vector<uint32_t> codepointArray;
  if(str == nullptr) return codepointArray;
  uint32_t codepoint = 0;
  uint32_t state = bjoern::UTF8_ACCEPT;
  auto s = reinterpret_cast<const uint8_t*>(str);
  for (; *s; ++s) {
    uint32_t prevState = state;
    bjoern::decode(&state, &codepoint, *s);
    if(state == bjoern::UTF8_ACCEPT) {
      codepointArray.push_back(codepoint);
    } else if(state == bjoern::UTF8_REJECT) {
      codepointArray.push_back(replacement);
      bjoern::reset(&state, &codepoint);
    }
  }
  return codepointArray;
}

/**
 * Decode utf8 string to a codepoint.
 *
 * Only decode the first codepoint in the string.
 *
 * @param str           Input utf8 str.
 * @param replacement   Replacement if error happened.
 *
 * @return uint32_t  Codepoint.
 */
static uint32_t toCodepoint(const char* str, uint32_t replacement = 0xFFFD) {
  if(str == nullptr) return replacement;
  uint32_t codepoint = 0;
  uint32_t state = bjoern::UTF8_ACCEPT;
  auto s = reinterpret_cast<const uint8_t*>(str);
  for (; *s; ++s) {
    bjoern::decode(&state, &codepoint, *s);
    if(state == bjoern::UTF8_ACCEPT) {
      return codepoint;
    } else if(state == bjoern::UTF8_REJECT) {
      return replacement;
    }
  }
  return replacement;
}

} // end namespace utf8;
