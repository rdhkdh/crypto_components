#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <openssl/sha.h>

using namespace std;

// Define a struct for Merkle tree nodes
struct MerkleNode {
    std::string hash; // Hash value stored in the node
    MerkleNode* left; // Pointer to the left child node
    MerkleNode* right; // Pointer to the right child node

    // Constructor to initialize node with a hash value
    MerkleNode(const std::string& h) : hash(h), left(nullptr), right(nullptr) {}

    // Destructor for memory cleanup
    ~MerkleNode() {
        delete left;
        delete right;
    }
};

// Define a class for Merkle Trees
class MerkleTree {
private:
    MerkleNode* root; // Pointer to the root node of the tree

    // Private method to hash data using SHA256 algorithm
    std::string hashData(const std::string& data) {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256((const unsigned char*)data.c_str(), data.length(), hash);

        std::stringstream ss;
        for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
        }
        return ss.str();
    }

    // Private helper method to recursively build the Merkle tree
    MerkleNode* buildTree(const std::vector<std::string>& data, int start, int end) {
        // Base case: when a single data element is left
        if (start == end) {
            // Hash the individual data element
            std::string hash = hashData(data[start]);
            return new MerkleNode(hash);
        }

        // Find the midpoint
        int mid = (start + end) / 2;
        // Recursively build left and right subtrees
        MerkleNode* left = buildTree(data, start, mid);
        MerkleNode* right = buildTree(data, mid + 1, end);

        // Concatenate hashes of left and right nodes and hash the result
        std::string combinedHash = left->hash + right->hash;
        std::string hash = hashData(left->hash + right->hash);

        // Create a new node with the hashed value
        MerkleNode* newNode = new MerkleNode(hash);
        newNode->left = left;
        newNode->right = right;

        return newNode;
    }

    // Private method to print nodes at a specific level of the tree
    void printLevel(MerkleNode* node, int level) {
        if (node == nullptr)
            return;
        if (level == 1)
            std::cout << node->hash.substr(node->hash.size() - 5) << " ";
        else if (level > 1) {
            printLevel(node->left, level - 1);
            printLevel(node->right, level - 1);
        }
    }

    // Private method to calculate the height of the tree
    int height(MerkleNode* node) {
        if (node == nullptr)
            return 0;
        else {
            int lHeight = height(node->left);
            int rHeight = height(node->right);

            if (lHeight > rHeight)
                return (lHeight + 1);
            else
                return (rHeight + 1);
        }
    }

    // Private method to search for a target hash bottom-up and construct proof
    bool existsBottomUp(MerkleNode* node, const std::string& targetHash, std::vector<std::string>& proof, std::string& siblingHash, std::string& parentHash) {
        if (node == nullptr)
            return false;

        // If the node is a leaf node and matches the target hash
        if (node->left == nullptr && node->right == nullptr && node->hash == targetHash) {
            // If the target hash is found, store the sibling and parent hashes
            if (node->left && node->right) {
                if (node->left->hash == targetHash) {
                    siblingHash = node->right->hash;
                } else {
                    siblingHash = node->left->hash;
                }
                parentHash = getParentHash(root, node);
            }
            return true;
        }

        // If the node is not a leaf node, continue recursively searching
        if (existsBottomUp(node->left, targetHash, proof, siblingHash, parentHash)) {
            proof.push_back(node->right->hash);
            return true;
        } else if (existsBottomUp(node->right, targetHash, proof, siblingHash, parentHash)) {
            proof.push_back(node->left->hash);
            return true;
        }

        return false;
    }

    // Private method to find the parent hash of a given node
    std::string getParentHash(MerkleNode* root, MerkleNode* node) {
        if (root == nullptr || node == nullptr || root == node) {
            return "";
        }

        std::vector<MerkleNode*> path;
        findPath(root, node, path);

        std::string parentHash = "";
        for (int i = path.size() - 1; i >= 0; --i) {
            if (path[i]->left == node && i > 0) {
                parentHash = hashData(path[i]->hash + path[i - 1]->hash);
                break;
            } else if (path[i]->right == node && i > 0) {
                parentHash = hashData(path[i - 1]->hash + path[i]->hash);
                break;
            }
        }
        return parentHash;
    }

    // Private method to find the path from root to a given node
    void findPath(MerkleNode* root, MerkleNode* node, std::vector<MerkleNode*>& path) {
        if (root == nullptr || node == nullptr) {
            return;
        }

        if (root == node) {
            path.push_back(root);
            return;
        }

        findPath(root->left, node, path);
        if (!path.empty() && path.back() == root) {
            return;
        }
        findPath(root->right, node, path);
        if (!path.empty() && path.back() == root) {
            return;
        }
        if (!path.empty()) {
            path.pop_back();
        }
    }

    // Private method to construct proof of non-existence for a target
    void constructNonExistenceProof(MerkleNode* root, MerkleNode* node, std::vector<std::string>& proof) {
        if (root == nullptr || node == nullptr) {
            return;
        }

        // Traverse up the tree from the leaf node to the root
        while (root != nullptr && node != nullptr && root != node) {
            if (node == root->left) {
                if (root->right) {
                    // If the node is the left child, add the hash of the right sibling
                    proof.push_back(root->right->hash);
                } else {
                    // If the right child is null, hash an empty string and add to proof
                    proof.push_back(hashData(""));
                }
            } else if (node == root->right) {
                if (root->left) {
                    // If the node is the right child, add the hash of the left sibling
                    proof.push_back(root->left->hash);
                } else {
                    // If the left child is null, hash an empty string and add to proof
                    proof.push_back(hashData(""));
                }
            }

            // Move up to the parent node
            node = root;
            root = getParentNode(root);
        }
    }

    // Private method to get the parent node of a given node
    MerkleNode* getParentNode(MerkleNode* node) {
        if (node == nullptr || node == root) {
            return nullptr;
        }

        std::vector<MerkleNode*> path;
        findPath(root, node, path);

        if (path.size() >= 2) {
            return path[path.size() - 2];
        }

        return nullptr;
    }

    // Private method to find the leaf node closest to a target hash
    MerkleNode* findClosestLeaf(MerkleNode* root, const std::string& targetHash) {
        if (root == nullptr) {
            return nullptr;
        }

        if (root->left == nullptr && root->right == nullptr) {
            return root;
        }

        MerkleNode* leftClosest = findClosestLeaf(root->left, targetHash);
        MerkleNode* rightClosest = findClosestLeaf(root->right, targetHash);

        // Choose the closer leaf node
        if (leftClosest && rightClosest) {
            if (getDistance(leftClosest->hash, targetHash) < getDistance(rightClosest->hash, targetHash)) {
                return leftClosest;
            } else {
                return rightClosest;
            }
        } else if (leftClosest) {
            return leftClosest;
        } else {
            return rightClosest;
        }
    }

    // Private method to calculate the number of differing bits between two hashes
    int getDistance(const std::string& hash1, const std::string& hash2) {
        int distance = 0;
        for (size_t i = 0; i < hash1.size(); ++i) {
            if (hash1[i] != hash2[i]) {
                distance++;
            }
        }
        return distance;
    }

public:
    // Constructor to initialize the Merkle tree with given data
    MerkleTree(const std::vector<std::string>& data) {
        root = buildTree(data, 0, data.size() - 1);
    }

    // Destructor for memory cleanup
    ~MerkleTree() {
        delete root;
    }

    // Public method to get the root hash of the Merkle tree
    std::string getRootHash() {
        return root->hash;
    }

    // Public method to check if an element exists in the Merkle tree and get proof
    std::pair<bool, std::vector<std::string>> exists(const std::string& targetData) {
        std::vector<std::string> proof;
        std::string hashedTarget = hashData(targetData);
        std::string siblingHash, parentHash;

        // Start from the leaf nodes and search for the target hash
        bool result = existsBottomUp(root, hashedTarget, proof, siblingHash, parentHash);

        // If the target is found, return the proof
        if (result) {
            return std::make_pair(result, proof);
        } else {
            // If the target is not found, return proof of non-existence
            // Find the leaf node closest to the target
            MerkleNode* closestLeaf = findClosestLeaf(root, hashedTarget);
            // Construct proof of non-existence
            constructNonExistenceProof(root, closestLeaf, proof);
            return std::make_pair(false, proof);
        }
    }

    // Public method to print the Merkle tree
    void printTree() {
        int h = height(root);
        for (int i = 1; i <= h; i++) {
            printLevel(root, i);
            std::cout << std::endl;
        }
    }
};

// Main function
int main() {
    // Sample data for the Merkle tree
    std::vector<std::string> data = {"1", "10", "20","50","100", "200", "250","500"};

    // Create a Merkle tree with the sample data
    MerkleTree tree(data);

    // Get the root hash of the Merkle tree
    std::string rootHash = tree.getRootHash();

    std::cout << "Root Hash: " << rootHash << std::endl << endl << endl;

    // Print the Merkle tree
    std::cout << "Merkle Tree:" << std::endl;
    tree.printTree();
    std::cout << std::endl;

    // Check if an element exists in the Merkle tree
    std::string targetData = "15"; // Non-existing data
    auto result = tree.exists(targetData);
    if (result.first) {
        std::cout << "Element with data " << targetData << " exists in the Merkle tree." << std::endl;
        // generates all the siblings and uncles 
        std::cout << "Proof of existence : "<< std::endl;
        for (const auto& hash : result.second) {
            std::cout << hash.substr(hash.size() - 5) << " ";
        }
        std::cout << std::endl;
    } else {
        std::cout << "Element with data " << targetData << " does not exist in the Merkle tree." << std::endl;
    }

    return 0;
}
