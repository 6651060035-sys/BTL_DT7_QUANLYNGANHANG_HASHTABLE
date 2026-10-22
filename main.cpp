

### **1. File `Account.h` (C?u trúc d? li?u n?n t?ng)**
*(File này d?nh nghia c?u trúc Tài kho?n và L?ch s? Giao d?ch làm ki?u d? li?u luu tr? cho B?ng bam)*

```cpp
#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>
#include <vector>
#include <iostream>

using namespace std;

// C?u trúc luu v?t Giao d?ch
struct Transaction {
    string transID;     // Mã giao d?ch
    string type;        // "Nap tien", "Rut tien", "Chuyen khoan"
    double amount;      // S? ti?n giao d?ch
    string timestamp;   // Th?i gian th?c hi?n
    string note;        // Ghi chú / Tài kho?n nh?n

    void display() const {
        cout << "[" << timestamp << "] " << type << ": " 
             << amount << " VND | Ghi chú: " << note << endl;
    }
};

// C?u trúc Tài kho?n Khách hàng (Luu tr? trong t?ng Node c?a Hash Table)
struct Account {
    string accountNumber; // Khóa bam chính (Key)
    string fullName;      // H? và tên khách hàng
    string idCard;        // S? CCCD/CMND
    double balance;       // S? du hi?n t?i
    vector<Transaction> history; // L?ch s? giao d?ch

    Account(string accNum = "", string name = "", string id = "", double bal = 0.0)
        : accountNumber(accNum), fullName(name), idCard(id), balance(bal) {}
};

#endif // ACCOUNT_H
```

---

### **2. File `HashTable.h` (Mã ngu?n Core chính c?a Leader Phan Nam)**
*(Ch?a toàn b? thu?t toán B?ng bam, Separate Chaining, Rehash và qu?n lý con tr? b? nh?)*

```cpp
#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "Account.h"
#include <string>
#include <iostream>

using namespace std;

// C?u trúc Node trong Separate Chaining (Danh sách liên k?t don)
struct Node {
    Account data;
    Node* next;

    Node(const Account& acc) : data(acc), next(nullptr) {}
};

class HashTable {
private:
    Node** table;                        // M?ng ch?a các con tr? tr? t?i danh sách liên k?t
    int capacity;                        // Kích thu?c m?ng bam hi?n t?i
    int size;                            // S? lu?ng tài kho?n hi?n có
    const double MAX_LOAD_FACTOR = 0.75; // Ngu?ng kích ho?t Rehash

    // 1. Hàm bam chu?i (Polynomial Rolling Hash Function)
    int hashFunction(const string& key) const {
        long long hashValue = 0;
        long long p = 31;          // S? nguyên t? ch?n cho chu?i s?
        long long m = 1e9 + 9;
        long long p_pow = 1;

        for (char c : key) {
            hashValue = (hashValue + (c - '0' + 1) * p_pow) % capacity;
            p_pow = (p_pow * p) % m;
        }
        return (hashValue % capacity + capacity) % capacity;
    }

    // 2. Thu?t toán Rehash t? d?ng m? r?ng B?ng bam khi h? s? t?i >= 0.75
    void rehash() {
        int oldCapacity = capacity;
        Node** oldTable = table;

        // M? r?ng kích thu?c m?ng bam (G?p dôi + 1)
        capacity = capacity * 2 + 1;
        table = new Node*[capacity](); // C?p phát m?ng con tr? m?i
        size = 0;

        // Bam l?i toàn b? ph?n t? t? m?ng cu sang m?ng m?i
        for (int i = 0; i < oldCapacity; ++i) {
            Node* current = oldTable[i];
            while (current != nullptr) {
                insertAccount(current->data); // Chèn l?i vào m?ng m?i
                Node* temp = current;
                current = current->next;
                delete temp; // Gi?i phóng node cu
            }
        }
        delete[] oldTable; // Gi?i phóng m?ng con tr? cu
        cout << "=> [SYSTEM] Da Rehash mo rong Bang bam len Kich thuoc: " << capacity << endl;
    }

public:
    // Kh?i t?o B?ng bam
    HashTable(int initCapacity = 101) : capacity(initCapacity), size(0) {
        table = new Node*[capacity]();
    }

    // Destructor gi?i phóng b? nh? tri?t d? (Tránh rò r? b? nh? - Memory Leak)
    ~HashTable() {
        for (int i = 0; i < capacity; ++i) {
            Node* current = table[i];
            while (current != nullptr) {
                Node* temp = current;
                current = current->next;
                delete temp;
            }
        }
        delete[] table;
    }

    // L?y h? s? t?i (Load Factor)
    double getLoadFactor() const {
        return (double)size / capacity;
    }

    // 3. Chèn tài kho?n m?i vào B?ng bam (Dành cho Module M? TK c?a Tr?ng)
    bool insertAccount(const Account& acc) {
        if (getLoadFactor() >= MAX_LOAD_FACTOR) {
            rehash();
        }

        int index = hashFunction(acc.accountNumber);
        Node* current = table[index];

        // Ki?m tra trùng khóa
        while (current != nullptr) {
            if (current->data.accountNumber == acc.accountNumber) {
                return false; // S? tài kho?n dã t?n t?i
            }
            current = current->next;
        }

        // Chèn vào d?u danh sách liên k?t O(1)
        Node* newNode = new Node(acc);
        newNode->next = table[index];
        table[index] = newNode;
        size++;
        return true;
    }

    // 4. Tra c?u tài kho?n O(1) (Dành cho Module Tra c?u & Giao d?ch c?a Tr?ng và Quy?n)
    Account* searchAccount(const string& accNum) {
        int index = hashFunction(accNum);
        Node* current = table[index];

        while (current != nullptr) {
            if (current->data.accountNumber == accNum) {
                return &(current->data); // Tr? tr?c ti?p d?n tài kho?n
            }
            current = current->next;
        }
        return nullptr; // Không tìm th?y
    }

    // 5. Xóa tài kho?n kh?i B?ng bam
    bool deleteAccount(const string& accNum) {
        int index = hashFunction(accNum);
        Node* current = table[index];
        Node* prev = nullptr;

        while (current != nullptr) {
            if (current->data.accountNumber == accNum) {
                if (prev == nullptr) {
                    table[index] = current->next;
                } else {
                    prev->next = current->next;
                }
                delete current;
                size--;
                return true;
            }
            prev = current;
            current = current->next;
        }
        return false;
    }

    int getSize() const { return size; }
    int getCapacity() const { return capacity; }
};

#endif // HASHTABLE_H
