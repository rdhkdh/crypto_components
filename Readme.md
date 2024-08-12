# CS581 Course Project
Components of a cryptocurrency  
27 April 2024  
Ridhiman Dhindsa, 210101088  
Aditya Gupta, 210101009  
Shivam Agrawal, 210101119  
Harsh Katara, 210101045 

## Video Link
Presentation link: [Google Drive](https://drive.google.com/file/d/1pliZm0Dcb7UWNcOpS2FiRLXc-2lMuC0Q/view?usp=sharing
)

## RSA cryptography
### Steps to Run
1) Open a Linux terminal in the directory containing the CPP file.  
2) Compile using: `g++ rsa.cpp -o rsa`  
3) Run using: `./rsa` 

### Features
* Prime Number Generation: Utilizes functions to generate random prime numbers within a specified range.
* Key Generation: Generates public and private keys necessary for encryption and decryption.
* Encryption: Encrypts a given message using the public key.
* Decryption: Decrypts an encrypted message using the private key

### Functions
* isPrime: Checks if a number is prime.
* gcd: Finds the greatest common divisor (GCD) of two numbers.
* generateRandomPrime: Generates a random prime number within a specified range.
* generateKeys: Generates public and private keys.
* encrypt: Encrypts a message using the public key.
* decrypt: Decrypts an encrypted message using the private key.

## Merkle Trees
This C++ program implements a Merkle tree, a data structure used in cryptography and computer science to efficiently verify the integrity and consistency of large datasets. The Merkle tree is constructed using the SHA-256 hashing algorithm for secure hashing.

### Features
#### 1.Merkle Tree Construction: 
The program constructs a Merkle tree from a given set of data elements. The data elements are hashed using the SHA-256 algorithm, and the tree is built recursively.
#### 2.Proof of Existence: 
It provides a method to check if a particular data element exists in the Merkle tree. If the data exists, it generates a proof consisting of hashes of sibling nodes along the path from the leaf node to the root.
#### 3.Proof of Non-Existence: 
If the data element does not exist in the tree, the program constructs a proof of non-existence. This proof demonstrates the absence of the data by including hashes of sibling nodes along the path to the closest leaf node.
#### 4.Root Hash Calculation: 
The program can calculate and output the root hash of the Merkle tree.
#### 5.Tree Visualization: 
It can print the structure of the Merkle tree, showing the hashes of nodes at each level.
### Usage
#### Compilation: 
1) Compile the program using a C++ compiler. `g++ -o merkle_tree merkle_tree.cpp -lssl -lcrypto`  
2) Run using: `./merkle_tree`  
3) Output: The program will display the root hash of the Merkle tree, the structure of the tree, and whether a specific data element exists in the tree along with its proof.
4) Sample Data :
The program includes sample data to demonstrate its functionality. You can modify the data vector in the main function to use your dataset.
5) Dependencies
This program relies on the OpenSSL library (libssl and libcrypto) for the SHA-256 hashing algorithm.

> The assumption we used is that we considered that the count of transactions is of 2^n where n > 0.

## Dolev Strong Protocol
### Steps to Run
1) Open a Linux terminal in the directory containing the CPP file.  
2) Compile using: `g++ dolev_strong.cpp -o dolev_strong`  
3) Run using: `./dolev_strong` 
4) Enter required input in PKI/non-PKI case.  