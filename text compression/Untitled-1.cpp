#include<bits/stdc++.h>

// A simple BitString class to handle bits efficiently
class BitString {
private:
    std::vector<bool> bits;

public:
    BitString() = default;

    void append(bool bit) {
        bits.push_back(bit);
    }

    void append(const BitString& bitString) {
        for (bool bit : bitString.bits) {
            bits.push_back(bit);
        }
    }

    std::string toString() const {
        std::string result;
        for (bool bit : bits) {
            result += bit ? '1' : '0';
        }
        return result;
    }
    size_t size() const {
        return bits.size();
    }
    const std::vector<bool>& getBits() const {
        return bits;
    }
    void clear() {
        bits.clear();
    }
};
// Huffman Tree Node Structure
struct Node {
    char ch;
    int freq;
    std::shared_ptr<Node> left, right;

    Node(char ch, int freq) : ch(ch), freq(freq), left(nullptr), right(nullptr) {}

    bool operator>(const Node& other) const {
        return freq > other.freq;
    }
};

// Generate Huffman codes by traversing the Huffman tree
void generateCodes(const std::shared_ptr<Node>& root, const BitString& prefix, std::unordered_map<char, BitString>& huffmanCodes) {
    if (root == nullptr) {
        return;
    }

    if (!root->left && !root->right) {
        huffmanCodes[root->ch] = prefix;
    }

    BitString leftPrefix = prefix;
    leftPrefix.append(false);
    generateCodes(root->left, leftPrefix, huffmanCodes);

    BitString rightPrefix = prefix;
    rightPrefix.append(true);
    generateCodes(root->right, rightPrefix, huffmanCodes);
}


std::shared_ptr<Node> buildHuffmanTree(const std::string& text, std::unordered_map<char, BitString>& huffmanCodes) {
    std::unordered_map<char, int> freq;
    for (char ch : text) {
        freq[ch]++;
    }

    auto comp = [](const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b) {
        return *a > *b;
    };
    std::priority_queue<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>, decltype(comp)> pq(comp);

    for (const auto& pair : freq) {
        pq.push(std::make_shared<Node>(pair.first, pair.second));
    }

    while (pq.size() > 1) {
        std::shared_ptr<Node> left = pq.top();
        pq.pop();
        std::shared_ptr<Node> right = pq.top();
        pq.pop();

        std::shared_ptr<Node> sum = std::make_shared<Node>('\0', left->freq + right->freq);
        sum->left = left;
        sum->right = right;

        pq.push(sum);
    }

    std::shared_ptr<Node> root = pq.top();
    generateCodes(root, BitString(), huffmanCodes);
    return root;
}

BitString encode(const std::string& text, const std::unordered_map<char, BitString>& huffmanCodes) {
    BitString encoded;
    for (char ch : text) {
        encoded.append(huffmanCodes.at(ch));
    }
    return encoded;
}

std::string decode(const BitString& encoded, const std::shared_ptr<Node>& root) {
    std::string decoded;
    std::shared_ptr<Node> current = root;
    const std::vector<bool>& bits = encoded.getBits();

    for (bool bit : bits) {
        if (bit) {
            current = current->right;
        } else {
            current = current->left;
        }

        if (!current->left && !current->right) {
            decoded += current->ch;
            current = root;
        }
    }

    return decoded;
}

// Write encoded data to a binary file
void writeencoded(const BitString& encoded, const std::string& outputFile) {
    std::ofstream outFile(outputFile, std::ios::out | std::ios::binary);
    if (!outFile) {
        std::cerr << "Failed to open output file: " << outputFile << std::endl;
        return;
    }

    const std::vector<bool>& bits = encoded.getBits();
    for (size_t i = 0; i < bits.size(); i += 8) {
        std::bitset<8> byte;
        for (size_t j = 0; j < 8 && i + j < bits.size(); ++j) {
            byte[7 - j] = bits[i + j]; 
        }
        outFile.put(static_cast<char>(byte.to_ulong()));
    }

    outFile.close();
}

// Read encoded data (bit string) from a binary file
BitString readencoded(const std::string& inputFile) {
    BitString encoded;
    std::ifstream inFile(inputFile, std::ios::in | std::ios::binary);
    if (!inFile) {
        std::cerr << "Failed to open input file: " << inputFile << std::endl;
        return encoded;
    }

    char byte;
    while (inFile.get(byte)) {
        for (int i = 7; i >= 0; --i) {
            encoded.append((byte >> i) & 1);
        }
    }

    inFile.close();
    return encoded;
}

int main() {
    // Input and output file names
    std::string inputFile = "input.txt";
    std::string encodedFile = "encoded.bin";
    std::string decodedFile = "decoded.txt";

    // Read input text from file
    std::ifstream inFile(inputFile);
    if (!inFile) {
        std::cerr << "Failed to open input file: " << inputFile << std::endl;
        return 1;
    }

    std::string text;
    std::getline(inFile, text);
    inFile.close();

    // Build Huffman tree and generate codes
    std::unordered_map<char, BitString> huffmanCodes;
    std::shared_ptr<Node> root = buildHuffmanTree(text, huffmanCodes);

    // Encode text using Huffman codes
    BitString encoded = encode(text, huffmanCodes);

    // Write encoded data to binary file
    writeencoded(encoded, encodedFile);

    // Read encoded data from binary file
    BitString encoded = readencoded(encodedFile);

    // Decode encoded data using Huffman tree
    std::string decoded = decode(encoded, root);

    // Write decoded text to output file
    std::ofstream outFile(decodedFile);
    if (!outFile) {
        std::cerr << "Failed to open output file: " << decodedFile << std::endl;
        return 1;
    }
    outFile << decoded;
    outFile.close();

    std::cout << "Huffman compression and decompression completed successfully." << std::endl;

    return 0;
}
