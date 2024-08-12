#include <bits/stdc++.h>
using namespace std;

// the collection of prime numbers, where we can select random primes p and q
set<int> prime;
int public_key;
int private_key;
int n;
const int NO_OF_PRIMES= 500;

// this function is used to fill the set of prime numbers
void primefiller()
{
    // seive of eratosthenes (a method to collect prime numbers)
    vector<bool> seive(NO_OF_PRIMES, true);
    seive[0] = false;
    seive[1] = false;
    for (int i = 2; i < NO_OF_PRIMES; i++)
    {
        for (int j = i * 2; j < NO_OF_PRIMES; j += i)
        {
            seive[j] = false; // mark all multiples as non-prime
        }
    }
    // filling the prime numbers
    for (int i = 0; i < seive.size(); i++)
    {
        if (seive[i])
        {
            prime.insert(i);
        }
    }
}

// picking a random prime number and erasing that prime number from list because p!=q
int pickrandomprime()
{
    // Seed the random number generator with the current time
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    int k = rand() % prime.size();

    auto it = prime.begin();
    while (k--)
    {
        it++;
    }
    int ret = *it;

    prime.erase(it);
    return ret;
}

// sets the public and private keys
void setkeys()
{
    int prime1 = pickrandomprime(); // first prime number
    int prime2 = pickrandomprime(); // second prime number

    // to check the prime numbers selected
    cout << "Primes selected:\n";
    cout << "p= " << prime1 << " q= " << prime2 << endl;

    n = prime1 * prime2;
    int z = (prime1 - 1) * (prime2 - 1);

    // find e such that gcd(e,z)= 1
    int e = 2;
    while (1)
    {
        if (__gcd(e, z) == 1)
            break;
        e++;
    }
    public_key = e; // public key = (e,n)

    // find d such that ed = 1 (mod z)
    int d = 2;
    while (1)
    {
        if ((d * e) % z == 1)
            break;
        d++;
    }
    private_key = d; // private key = (d,n)
}

// to encrypt the given number
long long int encrypt(double message) // message = m (numerical value)
{
    int d= private_key;
    long long int encrpyted_text = 1;
    while (d--)
    {
        encrpyted_text *= message; // calculating m^e (mod n)
        encrpyted_text %= n;
    }
    return encrpyted_text;
}

// to decrypt the given number
long long int decrypt(int encrpyted_text) // ciphertext = c (numerical value)
{
    int e = public_key;
    long long int decrypted = 1;
    while (e--)
    {
        decrypted *= encrpyted_text; // calculating c^d (mod n)
        decrypted %= n;
    }
    return decrypted;
}

// first converting each character to its ASCII value and then encrypting it
vector<int> encoder(string message)
{
    vector<int> form;
    // calling the encrypting function in encoding function
    for (auto &letter : message)
    {
        form.push_back(encrypt((int)letter));
    }
    return form;
}

// decoding the number to get the ASCII and converting it to character
string decoder(vector<int> encoded)
{
    string s;
    // calling the decrypting function decoding function
    for (auto &num : encoded)
    {
        s += decrypt(num); // decrypted value gets typecasted to char
    }
    return s;
}

int main()
{
    primefiller();
    setkeys();
    cout << "Public key: {" << public_key << ", " << n << "}" << endl;
    cout << "Private key: {" << private_key << ", " << n << "}\n" << endl;

    string message = "Test Message";
    cout << "Enter the message: ";
    getline(cin, message);

    // calling the encoding function
    vector<int> coded = encoder(message);

    cout << "\nInitial message: " << message;

    cout << "\n\nThe signed message (encrypted by private key):\n";
    for (auto &p : coded)
    {
        cout << p;
    }

    cout << "\n\nThe decoded message (decrypted by public key):\n";
    cout << decoder(coded) << endl;

    return 0;
}
