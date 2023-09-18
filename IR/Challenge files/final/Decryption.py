from Crypto.Cipher import AES, PKCS1_OAEP
from Crypto.PublicKey import RSA
from Crypto.Util.Padding import unpad

pemlines = open("RSA private key.txt","rb").read()
rsa = PKCS1_OAEP.new(RSA.import_key(pemlines))

data = open("Tiger_Woods_Hustlin.EXTEN","rb").read()
key_iv = rsa.decrypt(data[:512])
key = key_iv[:16]
iv = key_iv[16:]
aes = AES.new(key,AES.MODE_CBC,iv)
open("test","wb").write(unpad(aes.decrypt(data[512:]), 16))

data = open("Hudson_Hustles_new_flyer.EXTEN","rb").read()
key_iv = rsa.decrypt(data[:512])
key = key_iv[:16]
iv = key_iv[16:]
aes = AES.new(key,AES.MODE_CBC,iv)
open("test2","wb").write(unpad(aes.decrypt(data[512:]), 16))