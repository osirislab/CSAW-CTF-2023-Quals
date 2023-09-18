#!/usr/bin/env python
# -*- coding: utf-8 -*-
# author: @manojpandey

# Python 3 implementation for RC4 algorithm
# Brief: https://en.wikipedia.org/wiki/RC4

# Will use codecs, as 'str' object in Python 3 doesn't have any attribute 'decode'
import codecs

MOD = 256


def KSA(key):
    """ Key Scheduling Algorithm (from wikipedia):
        for i from 0 to 255
            S[i] := i
        endfor
        j := 0
        for i from 0 to 255
            j := (j + S[i] + key[i mod keylength]) mod 256
            swap values of S[i] and S[j]
        endfor
    """
    key_length = len(key)
    # create the array "S"
    S = list(range(MOD))  # [0,1,2, ... , 255]
    j = 0
    for i in range(MOD):
        j = (j + S[i] + key[i % key_length]) % MOD
        S[i], S[j] = S[j], S[i]  # swap values

    return S


def PRGA(S):
    """ Psudo Random Generation Algorithm (from wikipedia):
        i := 0
        j := 0
        while GeneratingOutput:
            i := (i + 1) mod 256
            j := (j + S[i]) mod 256
            swap values of S[i] and S[j]
            K := S[(S[i] + S[j]) mod 256]
            output K
        endwhile
    """
    i = 0
    j = 0
    while True:
        i = (i + 1) % MOD
        j = (j + S[i]) % MOD

        S[i], S[j] = S[j], S[i]  # swap values
        K = S[(S[i] + S[j]) % MOD]
        yield K


def get_keystream(key):
    """ Takes the encryption key to get the keystream using PRGA
        return object is a generator
    """
    S = KSA(key)
    return PRGA(S)


def encrypt_logic(key, text):
    """ :key -> encryption key used for encrypting, as hex string
        :text -> array of unicode values/ byte string to encrpyt/decrypt
    """
    # For plaintext key, use this
    key = [ord(c) for c in key]
    # If key is in hex:
    # key = codecs.decode(key, 'hex_codec')
    # key = [c for c in key]
    keystream = get_keystream(key)

    res = []
    for c in text:
        val = (c ^ next(keystream))  # XOR and taking hex
        res.append(val)
    return bytearray(res)


def encrypt(key, plaintext):
    """ :key -> encryption key used for encrypting, as hex string
        :plaintext -> plaintext string to encrpyt
    """
    plaintext = [ord(c) for c in plaintext]
    return encrypt_logic(key, plaintext)


def decrypt(key, ciphertext):
    """ :key -> encryption key used for encrypting, as hex string
        :ciphertext -> hex encoded ciphered text using RC4
    """
    ciphertext = codecs.decode(ciphertext, 'hex_codec')
    res = encrypt_logic(key, ciphertext)
    return codecs.decode(res, 'hex_codec').decode('utf-8')
