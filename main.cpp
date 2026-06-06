#include <iostream>
#include <string>
#include <vector>
#include <fstream> 
#include <limits> 
#include <cctype> 
using namespace std;

// ==========================================
// COLOR CODES (The Magic for UI)
// ==========================================
const string RESET   = "\033[0m";
const string RED     = "\033[31m";
const string GREEN   = "\033[32m";
const string YELLOW  = "\033[33m";
const string CYAN    = "\033[36m";
const string BOLD    = "\033[1m";

// ==========================================
// UTILITY 1: NUMBER GUARD
// ==========================================
int getValidNumber(string prompt) {
    int num;
    while (true) {
        cout << YELLOW << prompt << RESET;
        cin >> num;
        if (cin.fail()) {
            cin.clear(); 
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            cout << RED << BOLD << "[!] Error: Invalid Input. Please enter a NUMBER." << RESET << endl;
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            return num;
        }
    }
}

// ==========================================
// UTILITY 2: ALPHABET GUARD
// ==========================================
string getValidKeyword(string prompt) {
    string word;
    while (true) {
        cout << YELLOW << prompt << RESET;
        getline(cin, word);

        bool isValid = true;
        if(word.empty()) isValid = false;

        for (int i = 0; i < word.length(); i++) {
            if (!isalpha(word[i])) { 
                isValid = false;
                break;
            }
        }

        if (isValid) {
            return word;
        } else {
            cout << RED << BOLD << "[!] Error: Please enter ALPHABETS only (No numbers or spaces)." << RESET << endl;
        }
    }
}

// ==========================================
// 1. ABSTRACT BASE CLASS
// ==========================================
class Cipher {
public:
    virtual string encrypt(string text) = 0;
    virtual string decrypt(string text) = 0;
    virtual ~Cipher() {} 
};

// ==========================================
// 2. CAESAR CIPHER
// ==========================================
class CaesarCipher : public Cipher {
private:
    int key; 
public:
    CaesarCipher(int k) { key = k % 26; }
    string encrypt(string text) {
        string secret = "";
        for(int i = 0; i < text.length(); i++) {
            char c = text[i];
            if(c >= 'A' && c <= 'Z') secret += char(int(c - 'A' + key) % 26 + 'A');
            else if(c >= 'a' && c <= 'z') secret += char(int(c - 'a' + key) % 26 + 'a');
            else secret += c; 
        }
        return secret;
    }
    string decrypt(string text) {
        string original = "";
        for(int i = 0; i < text.length(); i++) {
            char c = text[i];
            if(c >= 'A' && c <= 'Z') original += char((int(c - 'A' - key) % 26 + 26) % 26 + 'A');
            else if(c >= 'a' && c <= 'z') original += char((int(c - 'a' - key) % 26 + 26) % 26 + 'a');
            else original += c;
        }
        return original;
    }
};

// ==========================================
// 3. ATBASH CIPHER
// ==========================================
class AtbashCipher : public Cipher {
public:
    string encrypt(string text) {
        string secret = "";
        for(int i = 0; i < text.length(); i++) {
            char c = text[i];
            if(c >= 'A' && c <= 'Z') secret += 'Z' - (c - 'A'); 
            else if(c >= 'a' && c <= 'z') secret += 'z' - (c - 'a');
            else secret += c; 
        }
        return secret;
    }
    string decrypt(string text) { return encrypt(text); }
};

// ==========================================
// 4. VIGENERE CIPHER
// ==========================================
class VigenereCipher : public Cipher {
private:
    string key;
public:
    VigenereCipher(string k) {
        key = k;
        for(int i = 0; i < key.length(); i++) key[i] = toupper(key[i]); 
    }
    string encrypt(string text) {
        string secret = "";
        int j = 0; 
        for(int i = 0; i < text.length(); i++) {
            char c = text[i];
            if(c >= 'A' && c <= 'Z') {
                secret += ((c - 'A') + (key[j] - 'A')) % 26 + 'A';
                j = (j + 1) % key.length(); 
            }
            else if(c >= 'a' && c <= 'z') {
                secret += ((c - 'a') + (key[j] - 'A')) % 26 + 'a';
                j = (j + 1) % key.length();
            }
            else secret += c; 
        }
        return secret;
    }
    string decrypt(string text) {
        string original = "";
        int j = 0;
        for(int i = 0; i < text.length(); i++) {
            char c = text[i];
            if(c >= 'A' && c <= 'Z') {
                original += ((c - 'A') - (key[j] - 'A') + 26) % 26 + 'A';
                j = (j + 1) % key.length();
            }
            else if(c >= 'a' && c <= 'z') {
                original += ((c - 'a') - (key[j] - 'A') + 26) % 26 + 'a';
                j = (j + 1) % key.length();
            }
            else original += c;
        }
        return original;
    }
};

// ==========================================
// 5. RAIL FENCE CIPHER
// ==========================================
class RailFenceCipher : public Cipher {
private:
    int rails; 
public:
    RailFenceCipher(int r) { rails = (r > 1) ? r : 2; }
    string encrypt(string text) {
        vector<string> rail(rails, "");
        int row = 0;
        bool down = false;
        for (int i = 0; i < text.length(); i++) {
            rail[row] += text[i];
            if (row == 0 || row == rails - 1) down = !down;
            row += down ? 1 : -1;
        }
        string secret = "";
        for (int i = 0; i < rails; i++) secret += rail[i];
        return secret;
    }
    string decrypt(string text) {
        int len = text.length();
        vector<vector<char>> rail(rails, vector<char>(len, '\n'));
        bool down = false;
        int row = 0, col = 0;
        for (int i = 0; i < len; i++) {
            if (row == 0 || row == rails - 1) down = !down;
            rail[row][col++] = '*';
            row += down ? 1 : -1;
        }
        int index = 0;
        for (int i = 0; i < rails; i++) {
            for (int j = 0; j < len; j++) {
                if (rail[i][j] == '*' && index < len) rail[i][j] = text[index++];
            }
        }
        string original = "";
        row = 0, col = 0;
        down = false;
        for (int i = 0; i < len; i++) {
            if (row == 0 || row == rails - 1) down = !down;
            original += rail[row][col++];
            row += down ? 1 : -1;
        }
        return original;
    }
};

// ==========================================
// 6. CIPHER CHAIN (Pipeline)
// ==========================================
class CipherChain : public Cipher {
private:
    vector<Cipher*> chain; 
public:
    void addCipher(Cipher* c) { chain.push_back(c); }
    string encrypt(string text) {
        string result = text;
        for (int i = 0; i < chain.size(); i++) result = chain[i]->encrypt(result); 
        return result;
    }
    string decrypt(string text) {
        string result = text;
        for (int i = chain.size() - 1; i >= 0; i--) result = chain[i]->decrypt(result);
        return result;
    }
    ~CipherChain() {
        for (int i = 0; i < chain.size(); i++) delete chain[i];
    }
};

// ==========================================
// 7. FILE MANAGER (Data Saver)
// ==========================================
class FileManager {
public:
    void saveHistory(string lockName, string original, string locked) {
        ofstream file("SecretHistory.txt", ios::app); 
        if(file.is_open()) {
            file << "Lock Used : " << lockName << endl;
            file << "Original  : " << original << endl;
            file << "Encrypted : " << locked << endl;
            file << "-----------------------------------" << endl;
            file.close();
        }
    }
};

// ==========================================
// 8. MAIN FUNCTION (Colored UI Loop)
// ==========================================
int main() {
    int choice;
    string text;
    FileManager fileSys; 

    while (true) {
        cout << CYAN << BOLD << "\n=========================================" << RESET << endl;
        cout << CYAN << BOLD << "              CIPHERBOX                  " << RESET << endl;
        cout << CYAN << "       Ultimate Security Engine          " << RESET << endl;
        cout << CYAN << BOLD << "=========================================" << RESET << endl;
        cout << "[1] Caesar Cipher" << endl;
        cout << "[2] Atbash Cipher" << endl;
        cout << "[3] Vigenere Cipher" << endl;
        cout << "[4] Rail Fence Cipher" << endl;
        cout << "[5] CIPHER CHAIN (Multiple Locks!)" << endl;
        cout << "[0] Exit Program" << endl;
        cout << CYAN << "=========================================" << RESET << endl;
        
        choice = getValidNumber("Enter your choice (0-5): ");

        if (choice == 0) {
            cout << GREEN << BOLD << "\nExiting CipherBox... Goodbye!" << RESET << endl;
            break; 
        }

        if (choice < 1 || choice > 5) {
            cout << RED << BOLD << "[!] Invalid choice! Please select between 0 and 5." << RESET << endl;
            continue; 
        }
        
        cout << YELLOW << "\nEnter your Secret Message: " << RESET;
        getline(cin, text); 

        cout << CYAN << "\n--- RESULTS ---" << RESET << endl;

        if (choice == 1) {
            int key = getValidNumber("Enter Key (Number e.g., 3): ");
            CaesarCipher lock(key);
            string enc = lock.encrypt(text);
            cout << RED << "[LOCKED]   : " << enc << RESET << endl;
            cout << GREEN << "[UNLOCKED] : " << lock.decrypt(enc) << RESET << endl;
            fileSys.saveHistory("Caesar Cipher (Key: " + to_string(key) + ")", text, enc);
        } 
        else if (choice == 2) {
            AtbashCipher lock;
            string enc = lock.encrypt(text);
            cout << RED << "[LOCKED]   : " << enc << RESET << endl;
            cout << GREEN << "[UNLOCKED] : " << lock.decrypt(enc) << RESET << endl;
            fileSys.saveHistory("Atbash Cipher", text, enc);
        } 
        else if (choice == 3) {
            string key = getValidKeyword("Enter Keyword (Alphabets only, e.g., CODE): "); 
            VigenereCipher lock(key);
            string enc = lock.encrypt(text);
            cout << RED << "[LOCKED]   : " << enc << RESET << endl;
            cout << GREEN << "[UNLOCKED] : " << lock.decrypt(enc) << RESET << endl;
            fileSys.saveHistory("Vigenere Cipher (Key: " + key + ")", text, enc);
        } 
        else if (choice == 4) {
            int rails = getValidNumber("Enter Rails (Number e.g., 2): ");
            RailFenceCipher lock(rails);
            string enc = lock.encrypt(text);
            cout << RED << "[LOCKED]   : " << enc << RESET << endl;
            cout << GREEN << "[UNLOCKED] : " << lock.decrypt(enc) << RESET << endl;
            fileSys.saveHistory("Rail Fence (Rails: " + to_string(rails) + ")", text, enc);
        }
        else if (choice == 5) {
            CipherChain myChain;
            int numLocks = getValidNumber("How many locks do you want to chain? (e.g., 2): ");
            
            for(int i = 0; i < numLocks; i++) {
                cout << CYAN << "\nSelect Lock " << i+1 << " (1:Caesar, 2:Atbash, 3:Vigenere, 4:RailFence)" << RESET << endl;
                int lockType = getValidNumber("Choice: ");
                
                if(lockType == 1) {
                    int k = getValidNumber("Enter Caesar Key (Number): ");
                    myChain.addCipher(new CaesarCipher(k)); 
                } else if(lockType == 2) {
                    myChain.addCipher(new AtbashCipher());
                } else if(lockType == 3) {
                    string k = getValidKeyword("Enter Vigenere Keyword (Alphabets only): "); 
                    myChain.addCipher(new VigenereCipher(k));
                } else if(lockType == 4) {
                    int r = getValidNumber("Enter Rails (Number): ");
                    myChain.addCipher(new RailFenceCipher(r));
                } else {
                    cout << RED << "[!] Invalid lock type! Skipping..." << RESET << endl;
                }
            }
            string enc = myChain.encrypt(text);
            cout << RED << "\n[CHAIN LOCKED]   : " << enc << RESET << endl;
            cout << GREEN << "[CHAIN UNLOCKED] : " << myChain.decrypt(enc) << RESET << endl;
            fileSys.saveHistory("Cipher Chain (" + to_string(numLocks) + " locks)", text, enc);
        }
        cout << CYAN << "-----------------------------------------" << RESET << endl;
        cout << YELLOW << "(Saved to SecretHistory.txt)" << RESET << endl; 
    }

    return 0;
}