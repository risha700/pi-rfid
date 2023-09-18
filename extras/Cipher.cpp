#include <iostream>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <cstring>
#include <string>

// Function to perform encryption
std::string Encrypt(const std::string& plaintext, const std::string& key) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();

    if (!EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (const unsigned char*)key.c_str(), NULL)) {
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }

    int max_output_len = plaintext.length() + EVP_CIPHER_block_size(EVP_aes_256_cbc());
    std::string ciphertext(max_output_len, 0);
    int ciphertext_len = 0;

    if (!EVP_EncryptUpdate(ctx, (unsigned char*)&ciphertext[0], &ciphertext_len, (const unsigned char*)plaintext.c_str(), plaintext.length())) {
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }

    int final_len = 0;
    if (!EVP_EncryptFinal_ex(ctx, (unsigned char*)&ciphertext[ciphertext_len], &final_len)) {
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }

    EVP_CIPHER_CTX_free(ctx);

    return ciphertext.substr(0, ciphertext_len + final_len);
}

// Function to perform decryption
std::string Decrypt(const std::string& ciphertext, const std::string& key) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();

    if (!EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (const unsigned char*)key.c_str(), NULL)) {
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }

    int max_output_len = ciphertext.length();
    std::string plaintext(max_output_len, 0);
    int plaintext_len = 0;

    if (!EVP_DecryptUpdate(ctx, (unsigned char*)&plaintext[0], &plaintext_len, (const unsigned char*)ciphertext.c_str(), ciphertext.length())) {
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }

    int final_len = 0;
    if (!EVP_DecryptFinal_ex(ctx, (unsigned char*)&plaintext[plaintext_len], &final_len)) {
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }

    EVP_CIPHER_CTX_free(ctx);

    return plaintext.substr(0, plaintext_len + final_len);
}

int main() {
    std::string key = "mysecretkey"; // Replace with your own secret key
    std::string plaintext = "Hello, world!";

    // Encrypt the message
    std::string ciphertext = Encrypt(plaintext, key);

    if (!ciphertext.empty()) {
        std::cout << "Ciphertext: " << ciphertext << std::endl;

        // Decrypt the ciphertext
        std::string decrypted_text = Decrypt(ciphertext, key);

        if (!decrypted_text.empty()) {
            std::cout << "Decrypted text: " << decrypted_text << std::endl;
        } else {
            std::cerr << "Decryption failed." << std::endl;
        }
    } else {
        std::cerr << "Encryption failed." << std::endl;
    }

    return 0;
}
