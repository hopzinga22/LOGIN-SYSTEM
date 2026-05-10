#include <iostream>
#include <string>
#include <random>
#include <conio.h>
#include <fstream>
using namespace std;

// Function Declarations
void registerValidator();
string getPassword();
void loginValidator();
string passEncrypter(string data, char key);
char getRandomChar();

int main() {
    int choice;
    while (true) {
        cout << "============================\n";
        cout << "      LOGIN SYSTEM MENU     \n";
        cout << "============================\n";
        cout << "[1] Register\n[2] Login\n[3] Exit\n";
        cout << "============================\n";
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1: registerValidator(); break;
            case 2: loginValidator();    break;
            case 3:
                cout << "EXITING...\n";
                return 0;
            default:
                cout << "Please key in a valid choice\n";
        }
    }
}

// Encrypt/Decrypt Logic
string passEncrypter(string data, char key) {
    string output = data;
    for (size_t i = 0; i < data.length(); i++) {
        output[i] = data[i] ^ key;
    }
    return output;
}

// Random Key generator for the Encrypt/Decrypt Logic
char getRandomChar() {
    random_device rd;
    mt19937 generator(rd());
    uniform_int_distribution<> distribution(33, 126);
    return static_cast<char>(distribution(generator));
}

//Password Masking Logic
string getPassword() {
    string localPass; 
    char ch;
    while ((ch = _getch()) != '\r') {
        if (ch == '\b') {
            if (!localPass.empty()) {
                cout << "\b \b";
                localPass.pop_back();
            }
        } else {
            localPass += ch;
            cout << '*';
        }
    }
    cout << '\n';
    return localPass;
}

// Handles the registeration
void registerValidator() {
    cout << "============================\n";
    cout << "    REGISTRATION MENU       \n";
    cout << "============================\n";

    string username, password;

    while (true) {
        cout << "Create a username: ";
        getline(cin, username);

        if (username.empty()) {
            cout << "Username cannot be empty. Try again.\n";
            continue;
        }

        // Check for duplicate usernames
        ifstream readHandler("details.txt");
        bool taken = false;
        string storedUser, storedKey, storedPass;
        //File stores 3 lines per user: username, key, encrypted password
        while (getline(readHandler, storedUser) &&
               getline(readHandler, storedKey)  &&
               getline(readHandler, storedPass)) {
            if (storedUser == username) {
                taken = true;
                break;
            }
        }
        readHandler.close();

        if (taken) {
            cout << "Username already exists. Please choose another.\n";
            continue;
        }
        break;
    }

    while (true) {
        cout << "Create a password (min 8 characters): ";
        password = getPassword();

        if (password.length() < 8) {
            cout << "Password must be at least 8 characters. Try again.\n";
            continue;
        }
        break;
    }

    //Generates key and saves it to file alongside the encrypted password
    char passHider = getRandomChar();
    string encrypted = passEncrypter(password, passHider);

    ofstream fileHandler("details.txt", ios::app);
    if (!fileHandler.is_open()) {
        cout << "Error: Could not open file for writing.\n";
        return;
    }
    fileHandler << username  << "\n";
    fileHandler << passHider << "\n"; // ← key saved so login can decrypt the stored Password
    fileHandler << encrypted << "\n";
    fileHandler.close();

    cout << "Account created successfully!\n";
}

void loginValidator() {
    const int maxTries = 3;

    fstream fileHandler("details.txt");
    if (!fileHandler.is_open()) {
        cout << "Error: Could not load credentials.\n";
        return;
    }

    for (int trialTracker = 0; trialTracker < maxTries; trialTracker++) {
        cout << "============================\n";
        cout << "       LOGIN MENU           \n";
        cout << "============================\n";

        string username, password;
        cout << "Enter your username: ";
        getline(cin, username);
        cout << "Enter your password: ";
        password = getPassword(); //

        // Scan file for matching credentials
        fileHandler.clear();
        fileHandler.seekg(0, ios::beg);

        bool found = false;
        string storedUser, storedKey, storedPass;

        //Reads 3 lines per record to validate account
        while (getline(fileHandler, storedUser) &&
               getline(fileHandler, storedKey)  &&
               getline(fileHandler, storedPass)) {

            // retrieve the saved XOR key
            char key = storedKey[0]; 
            string attempt = passEncrypter(password, key); // re-encrypt attempt

            if (storedUser == username && storedPass == attempt) {
                found = true;
                break;
            }
        }

        if (found) {
            cout << "Access granted. Welcome, " << username << "!\n";
            fileHandler.close();
            return;
        }

        int remaining = maxTries - trialTracker - 1;
        if (remaining > 0) {
            cout << "Wrong username or password. " << remaining << " attempt(s) remaining.\n";
        }
    }

    cout << "Too many failed attempts. Exiting...\n";
    fileHandler.close();
}