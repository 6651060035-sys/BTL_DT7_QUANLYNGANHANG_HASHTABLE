
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cmath>

using namespace std;

// ============================================================================
// PH?N 1: C?U TRÚC D? LI?U N?N T?NG (ACCOUNT & TRANSACTION)
// THÀNH VIÊN PH? TRÁCH: PHAN NAM (LEADER) - MSSV: 6651060035
// NHÁNH GIT: main / TV1-leader-core-hash-table
// NHI?M V?: Mô hình hóa c?u trúc Tài kho?n và L?ch s? Giao d?ch
// ============================================================================

// C?u trúc luu v?t Giao d?ch
struct Transaction {
    string transID;     // Mã giao d?ch
    string type;        // Lo?i giao d?ch ("N?p ti?n", "Rút ti?n", "Chuy?n kho?n")
    double amount;      // S? ti?n giao d?ch
    string timestamp;   // Th?i gian th?c hi?n giao d?ch
    string note;        // Ghi chú / S? tài kho?n nh?n

    void display() const {
        cout << "  [" << timestamp << "] " << left << setw(14) << type 
             << " | S? ti?n: " << right << setw(12) << fixed << setprecision(0) << amount << " VND"
             << " | Ghi chú: " << note << endl;
    }
};

// C?u trúc Tài kho?n Khách hàng (Luu trong t?ng Node c?a B?ng bam)
struct Account {
    string accountNumber;        // Khóa bam chính (Key - S? tài kho?n)
    string fullName;             // H? và tên khách hàng
    string idCard;               // S? CCCD / CMND
    double balance;              // S? du tài kho?n hi?n t?i (VND)
    vector<Transaction> history; // L?ch s? các giao d?ch

    Account(string accNum = "", string name = "", string id = "", double bal = 0.0)
        : accountNumber(accNum), fullName(name), idCard(id), balance(bal) {}

    void displayInfo() const {
        cout << "==================================================\n";
        cout << " S? Tài Kho?n : " << accountNumber << "\n";
        cout << " Ch? Tài Kho?n: " << fullName << "\n";
        cout << " S? CCCD/CMND : " << idCard << "\n";
        cout << " S? Du Hi?n T?i: " << fixed << setprecision(0) << balance << " VND\n";
        cout << " S? Giao D?ch  : " << history.size() << " giao d?ch\n";
        cout << "==================================================\n";
    }
};

// ============================================================================
// PH?N 2: CORE HASH TABLE & REHASH ALGORITHM
// THÀNH VIÊN PH? TRÁCH: PHAN NAM (LEADER) - MSSV: 6651060035
// NHÁNH GIT: main / TV1-leader-core-hash-table
// NHI?M V?: Cài d?t B?ng bam, Separate Chaining, Polynomial Hash, Rehash & Memory Management
// ============================================================================

// Node trong Danh sách liên k?t don (Separate Chaining)
struct Node {
    Account data;
    Node* next;

    Node(const Account& acc) : data(acc), next(nullptr) {}
};

class HashTable {
private:
    Node** table;                        // M?ng các con tr? tr? d?n DS liên k?t
    int capacity;                        // Kích thu?c m?ng bam
    int size;                            // S? lu?ng tài kho?n hi?n có
    const double MAX_LOAD_FACTOR = 0.75; // Ngu?ng kích ho?t Rehash

    // 1. Hàm bam chu?i Polynomial Rolling Hash Function
    int hashFunction(const string& key) const {
        long long hashValue = 0;
        long long p = 31;          // S? nguyên t? cho chu?i s?
        long long m = 1e9 + 9;     // Mô-dun s? l?n tránh tràn s?
        long long p_pow = 1;

        for (char c : key) {
            hashValue = (hashValue + (c - '0' + 1) * p_pow) % capacity;
            p_pow = (p_pow * p) % m;
        }
        return (hashValue % capacity + capacity) % capacity;
    }

    // 2. Thu?t toán Rehash m? r?ng B?ng bam t? d?ng khi Load Factor >= 0.75
    void rehash() {
        int oldCapacity = capacity;
        Node** oldTable = table;

        capacity = capacity * 2 + 1; // M? r?ng m?ng bam (G?p dôi + 1)
        table = new Node*[capacity]();
        size = 0;

        for (int i = 0; i < oldCapacity; ++i) {
            Node* current = oldTable[i];
            while (current != nullptr) {
                insertAccount(current->data); // Chèn l?i vào m?ng bam m?i
                Node* temp = current;
                current = current->next;
                delete temp; // Gi?i phóng Node cu
            }
        }
        delete[] oldTable; // Gi?i phóng m?ng con tr? cu
        cout << "=> [SYSTEM - PHAN NAM] Ðã Rehash m? r?ng B?ng bam! Kích thu?c m?i: " << capacity << endl;
    }

public:
    HashTable(int initCapacity = 11) : capacity(initCapacity), size(0) {
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

    double getLoadFactor() const {
        return (double)size / capacity;
    }

    int getSize() const { return size; }
    int getCapacity() const { return capacity; }

    // Chèn tài kho?n vào B?ng bam
    bool insertAccount(const Account& acc) {
        if (getLoadFactor() >= MAX_LOAD_FACTOR) {
            rehash();
        }

        int index = hashFunction(acc.accountNumber);
        Node* current = table[index];

        while (current != nullptr) {
            if (current->data.accountNumber == acc.accountNumber) {
                return false; // Trùng s? tài kho?n
            }
            current = current->next;
        }

        Node* newNode = new Node(acc);
        newNode->next = table[index];
        table[index] = newNode;
        size++;
        return true;
    }

    // Tra c?u tài kho?n O(1)
    Account* searchAccount(const string& accNum) {
        int index = hashFunction(accNum);
        Node* current = table[index];

        while (current != nullptr) {
            if (current->data.accountNumber == accNum) {
                return &(current->data);
            }
            current = current->next;
        }
        return nullptr;
    }

    // Xóa tài kho?n kh?i B?ng bam
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

    // Duy?t qua t?t c? tài kho?n (H? tr? luu file)
    vector<Account*> getAllAccounts() const {
        vector<Account*> result;
        for (int i = 0; i < capacity; ++i) {
            Node* current = table[i];
            while (current != nullptr) {
                result.push_back(&(current->data));
                current = current->next;
            }
        }
        return result;
    }
};

// ============================================================================
// PH?N 3: MODULE QU?N LÝ TÀI KHO?N (M? TÀI KHO?N, TRA C?U O(1) & B?T L?I)
// THÀNH VIÊN PH? TRÁCH: NGUY?N PHUONG HOÀNG TR?NG - MSSV: 6651060065
// NHÁNH GIT: TV2-quan-ly-tai-khoan
// NHI?M V?: M? tài kho?n m?i, Tra c?u O(1), Xóa tài kho?n, B?t l?i nh?p li?u
// ============================================================================

namespace AccountManagementModule {

    // Ki?m tra chu?i ch? ch?a ký t? s?
    bool isNumeric(const string& str) {
        if (str.empty()) return false;
        for (char c : str) {
            if (!isdigit(c)) return false;
        }
        return true;
    }

    // Ch?c nang 1: M? tài kho?n khách hàng m?i
    void openNewAccount(HashTable& ht) {
        cout << "\n--- [MODULE HOÀNG TR?NG] M? TÀI KHO?N M?I ---\n";
        string accNum, name, idCard;
        double initBalance;

        cout << "Nh?p s? tài kho?n (chu?i ch? s?): ";
        cin >> accNum;
        if (!isNumeric(accNum) || accNum.length() < 4) {
            cout << "=> ERROR: S? tài kho?n không h?p l?! Ph?i là chu?i s?.\n";
            return;
        }

        if (ht.searchAccount(accNum) != nullptr) {
            cout << "=> ERROR: S? tài kho?n " << accNum << " dã t?n t?i trong h? th?ng!\n";
            return;
        }

        cin.ignore();
        cout << "Nh?p h? và tên khách hàng: ";
        getline(cin, name);
        if (name.empty()) {
            cout << "=> ERROR: Tên khách hàng không du?c d? r?ng!\n";
            return;
        }

        cout << "Nh?p s? CCCD/CMND: ";
        cin >> idCard;

        cout << "Nh?p s? du ban d?u (T?i thi?u 50,000 VND): ";
        cin >> initBalance;
        if (initBalance < 50000) {
            cout << "=> ERROR: S? du ban d?u t?i thi?u ph?i t? 50,000 VND!\n";
            return;
        }

        Account newAcc(accNum, name, idCard, initBalance);
        if (ht.insertAccount(newAcc)) {
            cout << "=> THÀNH CÔNG: Ðã m? tài kho?n " << accNum << " cho khách hàng " << name << "!\n";
        } else {
            cout << "=> ERROR: Không th? thêm tài kho?n vào B?ng bam!\n";
        }
    }

    // Ch?c nang 2: Tra c?u thông tin tài kho?n t?c th?i O(1)
    void searchAccountInfo(HashTable& ht) {
        cout << "\n--- [MODULE HOÀNG TR?NG] TRA C?U TÀI KHO?N O(1) ---\n";
        string accNum;
        cout << "Nh?p s? tài kho?n c?n tra c?u: ";
        cin >> accNum;

        Account* acc = ht.searchAccount(accNum);
        if (acc != nullptr) {
            cout << "=> TÌM TH?Y TÀI KHO?N T?C TH?I O(1):\n";
            acc->displayInfo();
        } else {
            cout << "=> KHÔNG TÌM TH?Y tài kho?n: " << accNum << endl;
        }
    }

    // Ch?c nang 3: Xóa tài kho?n
    void closeAccount(HashTable& ht) {
        cout << "\n--- [MODULE HOÀNG TR?NG] XÓA TÀI KHO?N ---\n";
        string accNum;
        cout << "Nh?p s? tài kho?n c?n xóa: ";
        cin >> accNum;

        if (ht.deleteAccount(accNum)) {
            cout << "=> THÀNH CÔNG: Ðã xóa tài kho?n " << accNum << " kh?i h? th?ng.\n";
        } else {
            cout << "=> ERROR: Không tìm th?y tài kho?n " << accNum << " d? xóa.\n";
        }
    }
}

// ============================================================================
// PH?N 4: MODULE GIAO D?CH NGÂN HÀNG & SAO KÊ L?CH S?
// THÀNH VIÊN PH? TRÁCH: VÒNG TH? QUY?N - MSSV: 6651060051
// NHÁNH GIT: TV3-nghiep-vu-giao-dich
// NHI?M V?: N?p ti?n, Rút ti?n, Chuy?n kho?n an toàn, Xu?t sao kê giao d?ch
// ============================================================================

namespace TransactionModule {

    // Hàm l?y th?i gian th?c h? th?ng
    string getCurrentTime() {
        time_t now = time(0);
        tm *ltm = localtime(&now);
        stringstream ss;
        ss << setfill('0') << setw(2) << ltm->tm_mday << "/"
           << setfill('0') << setw(2) << 1 + ltm->tm_mon << "/"
           << 1900 + ltm->tm_year << " "
           << setfill('0') << setw(2) << ltm->tm_hour << ":"
           << setfill('0') << setw(2) << ltm->tm_min << ":"
           << setfill('0') << setw(2) << ltm->tm_sec;
        return ss.str();
    }

    // Ch?c nang 1: N?p ti?n vào tài kho?n
    void deposit(HashTable& ht) {
        cout << "\n--- [MODULE TH? QUY?N] N?P TI?N VÀO TÀI KHO?N ---\n";
        string accNum;
        double amount;

        cout << "Nh?p s? tài kho?n: ";
        cin >> accNum;

        Account* acc = ht.searchAccount(accNum);
        if (acc == nullptr) {
            cout << "=> ERROR: Tài kho?n không t?n t?i!\n";
            return;
        }

        cout << "Nh?p s? ti?n mu?n n?p (VND): ";
        cin >> amount;
        if (amount <= 0) {
            cout << "=> ERROR: S? ti?n n?p ph?i l?n hon 0!\n";
            return;
        }

        acc->balance += amount;

        Transaction trans;
        trans.transID = "TX" + to_string(rand() % 900000 + 100000);
        trans.type = "N?p ti?n";
        trans.amount = amount;
        trans.timestamp = getCurrentTime();
        trans.note = "N?p ti?n t?i qu?y";

        acc->history.push_back(trans);

        cout << "=> THÀNH CÔNG: Ðã n?p " << fixed << setprecision(0) << amount 
             << " VND. S? du m?i: " << acc->balance << " VND\n";
    }

    // Ch?c nang 2: Rút ti?n t? tài kho?n
    void withdraw(HashTable& ht) {
        cout << "\n--- [MODULE TH? QUY?N] RÚT TI?N T? TÀI KHO?N ---\n";
        string accNum;
        double amount;

        cout << "Nh?p s? tài kho?n: ";
        cin >> accNum;

        Account* acc = ht.searchAccount(accNum);
        if (acc == nullptr) {
            cout << "=> ERROR: Tài kho?n không t?n t?i!\n";
            return;
        }

        cout << "Nh?p s? ti?n mu?n rút (VND): ";
        cin >> amount;
        if (amount <= 0) {
            cout << "=> ERROR: S? ti?n rút ph?i l?n hon 0!\n";
            return;
        }

        if (acc->balance - amount < 50000) {
            cout << "=> ERROR: S? du không d?! Ph?i duy trì s? du t?i thi?u 50,000 VND.\n";
            return;
        }

        acc->balance -= amount;

        Transaction trans;
        trans.transID = "TX" + to_string(rand() % 900000 + 100000);
        trans.type = "Rút ti?n";
        trans.amount = amount;
        trans.timestamp = getCurrentTime();
        trans.note = "Rút ti?n t?i ATM/Qu?y";

        acc->history.push_back(trans);

        cout << "=> THÀNH CÔNG: Ðã rút " << fixed << setprecision(0) << amount 
             << " VND. S? du còn l?i: " << acc->balance << " VND\n";
    }

    // Ch?c nang 3: Chuy?n kho?n an toàn
    void transfer(HashTable& ht) {
        cout << "\n--- [MODULE TH? QUY?N] CHUY?N KHO?N AN TOÀN ---\n";
        string srcAccNum, destAccNum;
        double amount;

        cout << "Nh?p s? tài kho?n NGU?I G?I: ";
        cin >> srcAccNum;
        Account* srcAcc = ht.searchAccount(srcAccNum);
        if (srcAcc == nullptr) {
            cout << "=> ERROR: Tài kho?n ngu?i g?i không t?n t?i!\n";
            return;
        }

        cout << "Nh?p s? tài kho?n NGU?I NH?N: ";
        cin >> destAccNum;
        if (srcAccNum == destAccNum) {
            cout << "=> ERROR: Không th? chuy?n kho?n cho chính mình!\n";
            return;
        }

        Account* destAcc = ht.searchAccount(destAccNum);
        if (destAcc == nullptr) {
            cout << "=> ERROR: Tài kho?n ngu?i nh?n không t?n t?i!\n";
            return;
        }

        cout << "Nh?p s? ti?n chuy?n (VND): ";
        cin >> amount;
        if (amount <= 0) {
            cout << "=> ERROR: S? ti?n chuy?n ph?i l?n hon 0!\n";
            return;
        }

        if (srcAcc->balance - amount < 50000) {
            cout << "=> ERROR: S? du ngu?i g?i không d? d? chuy?n kho?n!\n";
            return;
        }

        // Th?c hi?n giao d?ch chuy?n kho?n
        srcAcc->balance -= amount;
        destAcc->balance += amount;

        string timeStr = getCurrentTime();
        string txID = "TX" + to_string(rand() % 900000 + 100000);

        // Luu l?ch s? ngu?i g?i
        Transaction t1;
        t1.transID = txID;
        t1.type = "Chuy?n ti?n di";
        t1.amount = amount;
        t1.timestamp = timeStr;
        t1.note = "Chuy?n t?i TK: " + destAccNum + " (" + destAcc->fullName + ")";
        srcAcc->history.push_back(t1);

        // Luu l?ch s? ngu?i nh?n
        Transaction t2;
        t2.transID = txID;
        t2.type = "Nh?n ti?n d?n";
        t2.amount = amount;
        t2.timestamp = timeStr;
        t2.note = "Nh?n t? TK: " + srcAccNum + " (" + srcAcc->fullName + ")";
        destAcc->history.push_back(t2);

        cout << "=> THÀNH CÔNG: Ðã chuy?n " << fixed << setprecision(0) << amount 
             << " VND t? TK " << srcAccNum << " sang TK " << destAccNum << "!\n";
    }

    // Ch?c nang 4: In sao kê l?ch s? giao d?ch
    void printStatement(HashTable& ht) {
        cout << "\n--- [MODULE TH? QUY?N] XU?T SAO KÊ GIAO D?CH ---\n";
        string accNum;
        cout << "Nh?p s? tài kho?n c?n in sao kê: ";
        cin >> accNum;

        Account* acc = ht.searchAccount(accNum);
        if (acc == nullptr) {
            cout << "=> ERROR: Tài kho?n không t?n t?i!\n";
            return;
        }

        cout << "\n======================================================================\n";
        cout << "                SAO KÊ L?CH S? GIAO D?CH NGÂN HÀNG                   \n";
        cout << " S? TK: " << acc->accountNumber << " | Ch? TK: " << acc->fullName << endl;
        cout << " S? du hi?n t?i: " << fixed << setprecision(0) << acc->balance << " VND\n";
        cout << "----------------------------------------------------------------------\n";

        if (acc->history.empty()) {
            cout << "   (Chua có giao d?ch nào du?c th?c hi?n)\n";
        } else {
            for (const auto& t : acc->history) {
                t.display();
            }
        }
        cout << "======================================================================\n";
    }
}

// ============================================================================
// PH?N 5: MODULE Ð?C/GHI FILE & GIAO DI?N CONSOLE MENU
// THÀNH VIÊN PH? TRÁCH: HU?NH ANH TÚ - MSSV: 6651060069
// NHÁNH GIT: TV4-doc-ghi-file-giao-dien
// NHI?M V?: Ð?c/Ghi d? li?u b?n v?ng xu?ng file CSV, Giao di?n Menu Console
// ============================================================================

namespace FileAndUIModule {

    const string DATA_FILE = "bank_data.csv";

    // Ch?c nang 1: Luu toàn b? d? li?u ra File CSV
    void saveData(const HashTable& ht) {
        ofstream outFile(DATA_FILE);
        if (!outFile.is_open()) {
            cout << "=> ERROR: Không th? m? file d? ghi d? li?u!\n";
            return;
        }

        vector<Account*> accounts = ht.getAllAccounts();
        for (Account* acc : accounts) {
            // Ð?nh d?ng dòng tài kho?n: ACC,accountNumber,fullName,idCard,balance
            outFile << "ACC," << acc->accountNumber << "," << acc->fullName << ","
                    << acc->idCard << "," << fixed << setprecision(0) << acc->balance << "\n";

            // Ð?nh d?ng dòng giao d?ch: TRANS,transID,type,amount,timestamp,note
            for (const auto& t : acc->history) {
                outFile << "TRANS," << t.transID << "," << t.type << ","
                        << fixed << setprecision(0) << t.amount << ","
                        << t.timestamp << "," << t.note << "\n";
            }
        }

        outFile.close();
        cout << "=> [MODULE ANH TÚ] Ðã luu thành công " << accounts.size() 
             << " tài kho?n ra file '" << DATA_FILE << "'!\n";
    }

    // Ch?c nang 2: Ð?c khôi ph?c d? li?u t? File CSV khi kh?i d?ng
    void loadData(HashTable& ht) {
        ifstream inFile(DATA_FILE);
        if (!inFile.is_open()) {
            cout << "=> [MODULE ANH TÚ] Không tìm th?y file d? li?u cu. S? kh?i t?o h? th?ng m?i.\n";
            return;
        }

        string line;
        Account* lastAcc = nullptr;
        int accCount = 0;

        while (getline(inFile, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string type;
            getline(ss, type, ',');

            if (type == "ACC") {
                string accNum, name, idCard, balStr;
                getline(ss, accNum, ',');
                getline(ss, name, ',');
                getline(ss, idCard, ',');
                getline(ss, balStr, ',');

                double bal = stod(balStr);
                Account acc(accNum, name, idCard, bal);
                ht.insertAccount(acc);
                lastAcc = ht.searchAccount(accNum);
                accCount++;
            } else if (type == "TRANS" && lastAcc != nullptr) {
                Transaction t;
                string amtStr;
                getline(ss, t.transID, ',');
                getline(ss, t.type, ',');
                getline(ss, amtStr, ',');
                getline(ss, t.timestamp, ',');
                getline(ss, t.note, ',');

                t.amount = stod(amtStr);
                lastAcc->history.push_back(t);
            }
        }

        inFile.close();
        cout << "=> [MODULE ANH TÚ] Ðã n?p thành công " << accCount 
             << " tài kho?n t? file '" << DATA_FILE << "'!\n";
    }

    // Ch?c nang 3: Giao di?n Menu Console tuong tác
    void showMenu() {
        cout << "\n================================================================\n";
        cout << "     H? TH?NG QU?N LÝ TÀI KHO?N & GIAO D?CH NGÂN HÀNG (ÐT07)    \n";
        cout << "================================================================\n";
        cout << " M? tài kho?n m?i               (Ph? trách: Hoàng Tr?ng)    \n";
        cout << " Tra c?u tài kho?n O(1)         (Ph? trách: Hoàng Tr?ng)    \n";
        cout << " Xóa tài kho?n                  (Ph? trách: Hoàng Tr?ng)    \n";
        cout << " N?p ti?n vào tài kho?n         (Ph? trách: Th? Quyên)      \n";
        cout << " Rút ti?n t? tài kho?n          (Ph? trách: Th? Quyên)      \n";
        cout << " Chuy?n kho?n an toàn           (Ph? trách: Th? Quyên)      \n";
        cout << " In sao kê l?ch s? giao d?ch    (Ph? trách: Th? Quyên)      \n";
        cout << " Ch?y Th? nghi?m & Benchmark    (Ph? trách: Phú Qu?c)       \n";
        cout << " Luu d? li?u ra File            (Ph? trách: Anh Tú)         \n";
        cout << " Thoát và Luu h? th?ng                                      \n";
        cout << "================================================================\n";
        cout << "L?a ch?n c?a b?n (0-9): ";
    }
}

// ============================================================================
// PH?N 6: MODULE TESTING, BENCHMARK & MOCK DATA
// THÀNH VIÊN PH? TRÁCH: NGUY?N PHÚ QU?C - MSSV: 6651060049
// NHÁNH GIT: TV5-testing-benchmark-demo
// NHI?M V?: Ki?m th?, Benchmark Rehash/Load Factor & T?o d? li?u m?u Demo
// ============================================================================

namespace TestingAndBenchmarkModule {

    // Kh?i t?o d? li?u th? nghi?m m?u cho 5 thành viên nhóm
    void generateMockData(HashTable& ht) {
        cout << "\n--- [MODULE PHÚ QU?C] KH?I T?O D? LI?U M?U Ð? DEMO ---\n";

        Account a1("1000000001", "Phan Nam", "012345678901", 5000000);
        Account a2("1000000002", "Nguyen Phuong Hoang Trong", "012345678902", 12000000);
        Account a3("1000000003", "Vong The Quyen", "012345678903", 8500000);
        Account a4("1000000004", "Huynh Anh Tu", "012345678904", 15000000);
        Account a5("1000000005", "Nguyen Phu Quoc", "012345678905", 20000000);

        ht.insertAccount(a1);
        ht.insertAccount(a2);
        ht.insertAccount(a3);
        ht.insertAccount(a4);
        ht.insertAccount(a5);

        cout << "=> THÀNH CÔNG: Ðã kh?i t?o 5 tài kho?n m?u cho 5 thành viên nhóm!\n";
    }

    // Th? nghi?m Benchmark Hash Table & Rehash
    void runBenchmark(HashTable& ht) {
        cout << "\n--- [MODULE PHÚ QU?C] TH? NGHI?M BENCHMARK HASH TABLE & REHASH ---\n";
        cout << " S? lu?ng tài kho?n hi?n t?i: " << ht.getSize() << endl;
        cout << " Kích thu?c M?ng bam (Capacity): " << ht.getCapacity() << endl;
        cout << " H? s? t?i hi?n t?i (Load Factor): " << fixed << setprecision(2) << ht.getLoadFactor() << endl;

        cout << "\n... Ðang chèn t? d?ng 20 tài kho?n d? ki?m tra tính nang REHASH ...\n";
        for (int i = 10; i < 30; ++i) {
            string accNum = "10000000" + to_string(i);
            string name = "Khach Hang Test " + to_string(i);
            Account acc(accNum, name, "099999999999", 1000000);
            ht.insertAccount(acc);
        }

        cout << "\n=> K?T QU? SAU KHI BENCHMARK CHÈN HÀNG LO?T:\n";
        cout << " S? lu?ng tài kho?n m?i: " << ht.getSize() << endl;
        cout << " Kích thu?c M?ng bam m?i: " << ht.getCapacity() << endl;
        cout << " H? s? t?i m?i (Load Factor): " << ht.getLoadFactor() << endl;
        cout << "=> K?T LU?N: Thu?t toán Rehash ho?t d?ng O(1) b?ng bam không b? quá t?i!\n";
    }
}

// ============================================================================
// PH?N 7: HÀM MAIN CHÍNH - K?T N?I TOÀN B? S?N PH?M C?A 5 THÀNH VIÊN
// T?NG H?P VÀ ÐI?U KHI?N CHUONG TRÌNH D? ÁN ÐT07
// ============================================================================

int main() {
    // T?i uu hóa I/O C++
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    srand(time(0));

    // Kh?i t?o B?ng bam core c?a Leader Phan Nam
    HashTable bankHashTable(11);

    // T?i d? li?u cu t? File do Hu?nh Anh Tú qu?n lý
    FileAndUIModule::loadData(bankHashTable);

    // N?u h? th?ng m?i hoàn toàn chua có d? li?u thì n?p d? li?u m?u c?a Nguy?nn Phú Qu?c
    if (bankHashTable.getSize() == 0) {
        TestingAndBenchmarkModule::generateMockData(bankHashTable);
    }

    int choice;
    do {
        FileAndUIModule::showMenu();
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "=> ERROR: Vui lòng nh?p s? t? 0 d?n 9!\n";
            continue;
        }

        switch (choice) {
            case 1:
                AccountManagementModule::openNewAccount(bankHashTable);
                break;
            case 2:
                AccountManagementModule::searchAccountInfo(bankHashTable);
                break;
            case 3:
                AccountManagementModule::closeAccount(bankHashTable);
                break;
            case 4:
                TransactionModule::deposit(bankHashTable);
                break;
            case 5:
                TransactionModule::withdraw(bankHashTable);
                break;
            case 6:
                TransactionModule::transfer(bankHashTable);
                break;
            case 7:
                TransactionModule::printStatement(bankHashTable);
                break;
            case 8:
                TestingAndBenchmarkModule::runBenchmark(bankHashTable);
                break;
            case 9:
                FileAndUIModule::saveData(bankHashTable);
                break;
            case 0:
                FileAndUIModule::saveData(bankHashTable);
                cout << "\n=> C?M ON B?N ÐÃ S? D?NG H? TH?NG NGÂN HÀNG. T?M BI?T!\n";
                break;
            default:
                cout << "=> ERROR: L?a ch?n không h?p l?! Vui lòng nh?p t? 0 d?n 9.\n";
                break;
        }
    } while (choice != 0);

    return 0;
}

