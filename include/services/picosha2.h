#ifndef PICOSHA2_H
#define PICOSHA2_H

#include <algorithm>
#include <cassert>
#include <iterator>
#include <sstream>
#include <vector>
#include <fstream>
#include <iomanip>

namespace picosha2 {
typedef unsigned int sha256_word;

namespace detail {
inline sha256_word mask_32bit(sha256_word val) {
    return val & 0xffffffff;
}

inline sha256_word right_rotate(sha256_word val, unsigned int count) {
    return mask_32bit((val >> count) | (val << (32 - count)));
}

const sha256_word add_constant[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

const sha256_word initial_message_digest[8] = {
    0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
    0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
};

inline void hash256_block(sha256_word* state, const unsigned char* block) {
    sha256_word w[64];
    for (int i = 0; i < 16; ++i) {
        w[i] = (static_cast<sha256_word>(block[i * 4]) << 24) |
               (static_cast<sha256_word>(block[i * 4 + 1]) << 16) |
               (static_cast<sha256_word>(block[i * 4 + 2]) << 8) |
               (static_cast<sha256_word>(block[i * 4 + 3]));
    }
    for (int i = 16; i < 64; ++i) {
        sha256_word s0 = right_rotate(w[i - 15], 7) ^ right_rotate(w[i - 15], 18) ^ (w[i - 15] >> 3);
        sha256_word s1 = right_rotate(w[i - 2], 17) ^ right_rotate(w[i - 2], 19) ^ (w[i - 2] >> 10);
        w[i] = mask_32bit(w[i - 16] + s0 + w[i - 7] + s1);
    }

    sha256_word a = state[0], b = state[1], c = state[2], d = state[3];
    sha256_word e = state[4], f = state[5], g = state[6], h = state[7];

    for (int i = 0; i < 64; ++i) {
        sha256_word S1 = right_rotate(e, 6) ^ right_rotate(e, 11) ^ right_rotate(e, 25);
        sha256_word ch = (e & f) ^ ((~e) & g);
        sha256_word temp1 = mask_32bit(h + S1 + ch + add_constant[i] + w[i]);
        sha256_word S0 = right_rotate(a, 2) ^ right_rotate(a, 13) ^ right_rotate(a, 22);
        sha256_word maj = (a & b) ^ (a & c) ^ (b & c);
        sha256_word temp2 = mask_32bit(S0 + maj);

        h = g; g = f; f = e; e = mask_32bit(d + temp1);
        d = c; c = b; b = a; a = mask_32bit(temp1 + temp2);
    }

    state[0] = mask_32bit(state[0] + a); state[1] = mask_32bit(state[1] + b);
    state[2] = mask_32bit(state[2] + c); state[3] = mask_32bit(state[3] + d);
    state[4] = mask_32bit(state[4] + e); state[5] = mask_32bit(state[5] + f);
    state[6] = mask_32bit(state[6] + g); state[7] = mask_32bit(state[7] + h);
}
} // namespace detail

class hash256_one_by_one {
public:
    hash256_one_by_one() { init(); }

    void init() {
        buffer_.clear();
        std::copy(detail::initial_message_digest, detail::initial_message_digest + 8, state_);
        data_length_ = 0;
    }

    template <typename RaInIter>
    void process(RaInIter first, RaInIter last) {
        for (; first != last; ++first) {
            buffer_.push_back(static_cast<unsigned char>(*first));
            if (buffer_.size() == 64) {
                detail::hash256_block(state_, buffer_.data());
                buffer_.clear();
            }
            data_length_ += 8;
        }
    }

    void finish() {
        unsigned char pad[64] = {0x80};
        size_t pad_len = (buffer_.size() < 56) ? (56 - buffer_.size()) : (120 - buffer_.size());
        
        process(pad, pad + pad_len);

        unsigned char len_bytes[8];
        for (int i = 0; i < 8; ++i) {
            len_bytes[7 - i] = static_cast<unsigned char>((data_length_ >> (i * 8)) & 0xff);
        }
        process(len_bytes, len_bytes + 8);
    }

    template <typename OutIter>
    void get_hash_bytes(OutIter first) {
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 4; ++j) {
                *first++ = static_cast<unsigned char>((state_[i] >> (24 - j * 8)) & 0xff);
            }
        }
    }

private:
    sha256_word state_[8];
    std::vector<unsigned char> buffer_;
    unsigned long long data_length_;
};

inline std::string hash256_hex_string(const std::string& src) {
    hash256_one_by_one hasher;
    hasher.process(src.begin(), src.end());
    hasher.finish();
    
    unsigned char hash[32];
    hasher.get_hash_bytes(hash);

    std::ostringstream oss;
    for (int i = 0; i < 32; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    return oss.str();
}

} // namespace picosha2

#endif // PICOSHA2_H
