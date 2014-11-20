import os
import randomprime
import random
import sys

#extended euclid's algorithm
def egcd(a, b):
    if a == 0:
        return (b, 0, 1)
    else:
        g, y, x = egcd(b % a, a)
        return (g, x - (b // a) * y, y)

#calculating modular multiplicative inverse using extended euclid's alogrithm
def inverse(a, m):
    g, x, y = egcd(a, m)
    if g != 1:
        raise Exception('modular inverse does not exist')
    else:
        return x % m


class ElgamalCrypt(object):
    
    #Generate public private key pairs
    def __init__(self):
        self.p = randomprime.generateLargePrime(1024)
        if(self.p == -1):
            print "Failed to generate prime"
            sys.exit()    
        self.x = random.randrange(2,self.p-1)
        self.g = random.randrange(2,self.p-1)
        self.h = pow(self.g,self.x,self.p)
    
    #return public key
    def public_key(self):
        return (self.p,self.g,self.h)
    
    #given a message and a public_key tuple, return a tuple (g^y,encrypted_message)    
    def encrypt(self,plaintext,public_key):
        prime = public_key[0]
        y = random.randrange(2,prime-1)
        secret = pow(public_key[2],y,prime)
        message = (int(plaintext.encode('hex'),16)*secret)%prime
        h = pow(public_key[1],y,prime)
        return (h,message)
    #given a ciphertext tuple (g^y, encrypted_message), return plaintext
    def decrypt(self,ciphertext):
        pub_key, message = ciphertext
        secret = pow(pub_key,self.x,self.p)
        key = inverse(secret,self.p)
        plaintext = (message*key)%self.p
        return hex(plaintext)[2:-1].decode('hex')
    

alice = ElgamalCrypt()
bob = ElgamalCrypt()
pub_key_a = alice.public_key()
message = raw_input("Enter message: ")
print "Message : " + message
encrypted_text = bob.encrypt(message,pub_key_a)
print "Message in transit: " + hex(encrypted_text[1])
plaintext= alice.decrypt(encrypted_text)
print "Message after decrypting: " + plaintext
    
