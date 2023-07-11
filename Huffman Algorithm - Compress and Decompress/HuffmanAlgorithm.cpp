#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>

// Node structure for Huffman tree
struct HuffmanNode {
    char data;
    int frequency;
    HuffmanNode* left;
    HuffmanNode* right;

    HuffmanNode(char data, int frequency) {
        this->data = data;
        this->frequency = frequency;
        left = right = nullptr;
    }
};

// Comparison function for priority queue
struct Compare {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->frequency > b->frequency;
    }
};

// Function to create the frequency table
std::unordered_map<char, int> buildFrequencyTable(const std::string& text) {
    std::unordered_map<char, int> freqTable;
    for (char c : text) {
        freqTable[c]++;
    }
    return freqTable;
}

// Function to build the Huffman tree
HuffmanNode* buildHuffmanTree(const std::unordered_map<char, int>& freqTable) {
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, Compare> pq;

    for (const auto& pair : freqTable) {
        pq.push(new HuffmanNode(pair.first, pair.second));
    }

    while (pq.size() > 1) {
        HuffmanNode* left = pq.top();
        pq.pop();
        HuffmanNode* right = pq.top();
        pq.pop();

        HuffmanNode* newNode = new HuffmanNode('\0', left->frequency + right->frequency);
        newNode->left = left;
        newNode->right = right;

        pq.push(newNode);
    }

    return pq.top();
}

// Function to build the Huffman code table
void buildHuffmanCodeTable(HuffmanNode* root, std::unordered_map<char, std::string>& codeTable, std::string code = "") {
    if (root == nullptr)
        return;

    if (root->data != '\0') {
        codeTable[root->data] = code;
    }

    buildHuffmanCodeTable(root->left, codeTable, code + "0");
    buildHuffmanCodeTable(root->right, codeTable, code + "1");
}

// Function to encode the input text using Huffman coding
std::string encodeText(const std::string& text, const std::unordered_map<char, std::string>& codeTable) {
    std::string encodedText;
    for (char c : text) {
        encodedText += codeTable.at(c);
    }
    return encodedText;
}

// Function to decode the encoded text using Huffman coding
std::string decodeText(const std::string& encodedText, HuffmanNode* root) {
    std::string decodedText;
    HuffmanNode* curr = root;

    for (char bit : encodedText) {
        if (bit == '0') {
            curr = curr->left;
        } else {
            curr = curr->right;
        }

        if (curr->left == nullptr && curr->right == nullptr) {
            decodedText += curr->data;
            curr = root;
        }
    }

    return decodedText;
}

// Function to compress the input file using Huffman coding
void compressFile(const std::string& inputFilename, const std::string& outputFilename) {
    std::ifstream inputFile(inputFilename);
    std::ofstream outputFile(outputFilename, std::ios::binary);

    if (!inputFile) {
        std::cerr << "Error opening input file: " << inputFilename << std::endl;
        return;
    }

    if (!outputFile) {
        std::cerr << "Error creating output file: " << outputFilename << std::endl;
        return;
    }

    // Read the input file contents
    std::string text((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());

    // Build the frequency table
    std::unordered_map<char, int> freqTable = buildFrequencyTable(text);

    // Build the Huffman tree
    HuffmanNode* root = buildHuffmanTree(freqTable);

    // Build the Huffman code table
    std::unordered_map<char, std::string> codeTable;
    buildHuffmanCodeTable(root, codeTable);

    // Encode the input text using Huffman coding
    std::string encodedText = encodeText(text, codeTable);

    // Write the Huffman code table and encoded text to the output file
    outputFile << freqTable.size() << std::endl;
    for (const auto& pair : freqTable) {
        outputFile << pair.first << " " << pair.second << std::endl;
    }
    outputFile << encodedText;

    // Close the input and output files
    inputFile.close();
    outputFile.close();

    std::cout << "File compressed successfully." << std::endl;
}

// Function to decompress the input file using Huffman coding
void decompressFile(const std::string& inputFilename, const std::string& outputFilename) {
    std::ifstream inputFile(inputFilename, std::ios::binary);
    std::ofstream outputFile(outputFilename);

    if (!inputFile) {
        std::cerr << "Error opening input file: " << inputFilename << std::endl;
        return;
    }

    if (!outputFile) {
        std::cerr << "Error creating output file: " << outputFilename << std::endl;
        return;
    }

    // Read the frequency table from the input file
    int tableSize;
    inputFile >> tableSize;
    inputFile.ignore(); // Ignore the newline character after table size

    std::unordered_map<char, int> freqTable;
    char c;
    int freq;
    for (int i = 0; i < tableSize; i++) {
        inputFile >> c >> freq;
        inputFile.ignore(); // Ignore the newline character after each line
        freqTable[c] = freq;
    }

    // Build the Huffman tree
    HuffmanNode* root = buildHuffmanTree(freqTable);

    // Read the encoded text from the input file
    std::string encodedText((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());

    // Decode the encoded text using Huffman coding
    std::string decodedText = decodeText(encodedText, root);

    // Write the decoded text to the output file
    outputFile << decodedText;

    // Close the input and output files
    inputFile.close();
    outputFile.close();

    std::cout << "File decompressed successfully." << std::endl;
}

int main() {
    std::string inputFilename = "input.txt";
    std::string compressedFilename = "compressed.bin";
    std::string decompressedFilename = "decompressed.txt";

    // Compress the input file
    compressFile(inputFilename, compressedFilename);

    // Decompress the compressed file
    decompressFile(compressedFilename, decompressedFilename);

    return 0;
}
