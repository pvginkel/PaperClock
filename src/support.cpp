#include "includes.h"

#include "support.h"

#include <openssl/evp.h>

static icu::Locale LOCALE(ICU_LOCALE);

string strformat(const char* fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    auto length = vsnprintf(nullptr, 0, fmt, ap);
    va_end(ap);

    if (length < 0) {
        abort();
    }

    auto buffer = (char*)malloc(length + 1);
    if (!buffer) {
        abort();
    }

    va_start(ap, fmt);
    vsprintf(buffer, fmt, ap);
    va_end(ap);

    auto result = string(buffer, length);

    free(buffer);

    return result;
}

string sha1(const string& input) {
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_length = 0;

    auto sha1 = EVP_MD_CTX_new();
    EVP_DigestInit_ex(sha1, EVP_sha1(), nullptr);
    EVP_DigestUpdate(sha1, input.c_str(), input.size());
    EVP_DigestFinal_ex(sha1, hash, &hash_length);
    EVP_MD_CTX_free(sha1);

    std::stringstream ss;
    for (unsigned int i = 0; i < hash_length; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

icu_result<string> icu_normalize(const string& input) {
    auto status = U_ZERO_ERROR;
    auto icu_input = icu::UnicodeString::fromUTF8(input.c_str());

    icu::UnicodeString normalized;
    icu::Normalizer::normalize(icu_input, UNORM_NFC, 0, normalized, status);

    if (U_FAILURE(status)) {
        return status;
    }

    std::string result;
    normalized.toUTF8String(result);

    return result;
}

icu_result<string> icu_simplify(const string& input) {
    auto status = U_ZERO_ERROR;

    static icu::Transliterator* transliterator = nullptr;
    if (!transliterator) {
        transliterator =
            icu::Transliterator::createInstance("NFD; [:Nonspacing Mark:] Remove; NFC", UTRANS_FORWARD, status);
    }

    auto icu_input = icu::UnicodeString::fromUTF8(input.c_str());

    transliterator->transliterate(icu_input);

    std::string result;
    icu_input.toUTF8String(result);

    return result;
}

string icu_lower(const string& input) {
    auto icu_input = icu::UnicodeString::fromUTF8(input.c_str());

    auto lower_str = icu_input.toLower(LOCALE);

    std::string result;
    lower_str.toUTF8String(result);

    return result;
}
