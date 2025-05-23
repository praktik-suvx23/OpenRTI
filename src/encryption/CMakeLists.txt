add_executable(encrypt 
encrypt.cpp)

add_compile_definitions(
    ENCRYPTED_TXT_PATH="${CMAKE_SOURCE_DIR}/src/encryption/encryptedData/encryptedData.txt"
    DECRYPTED_TXT_PATH="${CMAKE_SOURCE_DIR}/src/encryption/encryptedData/decryptedData.txt"
)