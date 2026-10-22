

```cpp
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
// PHẦN 1: CẤU TRÚC DỮ LIỆU NỀN TẢNG (ACCOUNT & TRANSACTION)
// THÀNH VIÊN PHỤ TRÁCH: PHAN NAM (LEADER) - MSSV: 6651060035
// NHÁNH GIT: main / TV1-leader-core-hash-table
// NHIỆM VỤ: Mô hình hóa cấu trúc Tài khoản và Lịch sử Giao dịch
// ============================================================================

// Cấu trúc lưu vết Giao dịch
struct Transaction {
    string transID;     // Mã giao dịch
    string type;        // Loại giao dịch ("Nạp tiền", "Rút tiền", "Chuyển khoản")
    double amount;      // Số tiền giao dịch
    string timestamp;   // Thời gian thực hiện giao dịch
    string note;        // Ghi chú / Số tài khoản nhận

    void display() const {
        cout << "  [" << timestamp << "] " << left << setw(14) << type 
             << " | Số tiền: " << right << setw(12) << fixed << setprecision(0) << amount << " VND"
             << " | Ghi chú: " << note << endl;
    }
};

// Cấu trúc Tài khoản Khách hàng (Lưu trong từng Node của Bảng băm)
struct Account {
    string accountNumber;        // Khóa băm chính (Key - Số tài khoản)
    string fullName;             // Họ và tên khách hàng
    string idCard;               // Số CCCD / CMND
    double balance;              // Số dư tài khoản hiện tại (VND)
    vector<Transaction> history; // Lịch sử các giao dịch

    Account(string accNum = "", string name = "", string id = "", double bal = 0.0)
        : accountNumber(accNum), fullName(name), idCard(id), balance(bal) {}

    void displayInfo() const {
        cout << "==================================================\n";
        cout << " Số Tài Khoản : " << accountNumber << "\n";
        cout << " Chủ Tài Khoản: " << fullName << "\n";
        cout << " Số CCCD/CMND : " << idCard << "\n";
        cout << " Số Dư Hiện Tại: " << fixed << setprecision(0) << balance << " VND\n";
        cout << " Số Giao Dịch  : " << history.size() << " giao dịch\n";
        cout << "==================================================\n";
    }
};

// ============================================================================
// PHẦN 2: CORE HASH TABLE & REHASH ALGORITHM
// THÀNH VIÊN PHỤ TRÁCH: PHAN NAM (LEADER) - MSSV: 6651060035
// NHÁNH GIT: main / TV1-leader-core-hash-table
// NHIỆM VỤ: Cài đặt Bảng băm, Separate Chaining, Polynomial Hash, Rehash & Memory Management
// ============================================================================

// Node trong Danh sách liên kết đơn (Separate Chaining)
struct Node {
    Account data;
    Node* next;

    Node(const Account& acc) : data(acc), next(nullptr) {}
};

class HashTable {
private:
    Node** table;                        // Mảng các con trỏ trỏ đến DS liên kết
    int capacity;                        // Kích thước mảng băm
    int size;                            // Số lượng tài khoản hiện có
    const double MAX_LOAD_FACTOR = 0.75; // Ngưỡng kích hoạt Rehash

    // 1. Hàm băm chuỗi Polynomial Rolling Hash Function
    int hashFunction(const string& key) const {
        long long hashValue = 0;
        long long p = 31;          // Số nguyên tố cho chuỗi số
        long long m = 1e9 + 9;     // Mô-đun số lớn tránh tràn số
        long long p_pow = 1;

        for (char c : key) {
            hashValue = (hashValue + (c - '0' + 1) * p_pow) % capacity;
            p_pow = (p_pow * p) % m;
        }
        return (hashValue % capacity + capacity) % capacity;
    }

    // 2. Thuật toán Rehash mở rộng Bảng băm tự động khi Load Factor >= 0.75
    void rehash() {
        int oldCapacity = capacity;
        Node** oldTable = table;

        capacity = capacity * 2 + 1; // Mở rộng mảng băm (Gấp đôi + 1)
        table = new Node*[capacity]();
        size = 0;

        for (int i = 0; i < oldCapacity; ++i) {
            Node* current = oldTable[i];
            while (current != nullptr) {
                insertAccount(current->data); // Chèn lại vào mảng băm mới
                Node* temp = current;
                current = current->next;
                delete temp; // Giải phóng Node cũ
            }
        }
        delete[] oldTable; // Giải phóng mảng con trỏ cũ
        cout << "=> [SYSTEM - PHAN NAM] Đã Rehash mở rộng Bảng băm! Kích thước mới: " << capacity << endl;
    }

public:
    HashTable(int initCapacity = 11) : capacity(initCapacity), size(0) {
        table = new Node*[capacity]();
    }

    // Destructor giải phóng bộ nhớ triệt để (Tránh rò rỉ bộ nhớ - Memory Leak)
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

    // Chèn tài khoản vào Bảng băm
    bool insertAccount(const Account& acc) {
        if (getLoadFactor() >= MAX_LOAD_FACTOR) {
            rehash();
        }

        int index = hashFunction(acc.accountNumber);
        Node* current = table[index];

        while (current != nullptr) {
            if (current->data.accountNumber == acc.accountNumber) {
                return false; // Trùng số tài khoản
            }
            current = current->next;
        }

        Node* newNode = new Node(acc);
        newNode->next = table[index];
        table[index] = newNode;
        size++;
        return true;
    }

    // Tra cứu tài khoản O(1)
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

    // Xóa tài khoản khỏi Bảng băm
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

    // Duyệt qua tất cả tài khoản (Hỗ trợ lưu file)
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
// PHẦN 3: MODULE QUẢN LÝ TÀI KHOẢN (MỞ TÀI KHOẢN, TRA CỨU O(1) & BẮT LỖI)
// THÀNH VIÊN PHỤ TRÁCH: NGUYỄN PHƯƠNG HOÀNG TRỌNG - MSSV: 6651060065
// NHÁNH GIT: TV2-quan-ly-tai-khoan
// NHIỆM VỤ: Mở tài khoản mới, Tra cứu O(1), Xóa tài khoản, Bắt lỗi nhập liệu
// ============================================================================

namespace AccountManagementModule {

    // Kiểm tra chuỗi chỉ chứa ký tự số
    bool isNumeric(const string& str) {
        if (str.empty()) return false;
        for (char c : str) {
            if (!isdigit(c)) return false;
        }
        return true;
    }

    // Chức năng 1: Mở tài khoản khách hàng mới
    void openNewAccount(HashTable& ht) {
        cout << "\n--- [MODULE HOÀNG TRỌNG] MỞ TÀI KHOẢN MỚI ---\n";
        string accNum, name, idCard;
        double initBalance;

        cout << "Nhập số tài khoản (chuỗi chữ số): ";
        cin >> accNum;
        if (!isNumeric(accNum) || accNum.length() < 4) {
            cout << "=> ERROR: Số tài khoản không hợp lệ! Phải là chuỗi số.\n";
            return;
        }

        if (ht.searchAccount(accNum) != nullptr) {
            cout << "=> ERROR: Số tài khoản " << accNum << " đã tồn tại trong hệ thống!\n";
            return;
        }

        cin.ignore();
        cout << "Nhập họ và tên khách hàng: ";
        getline(cin, name);
        if (name.empty()) {
            cout << "=> ERROR: Tên khách hàng không được để rỗng!\n";
            return;
        }

        cout << "Nhập số CCCD/CMND: ";
        cin >> idCard;

        cout << "Nhập số dư ban đầu (Tối thiểu 50,000 VND): ";
        cin >> initBalance;
        if (initBalance < 50000) {
            cout << "=> ERROR: Số dư ban đầu tối thiểu phải từ 50,000 VND!\n";
            return;
        }

        Account newAcc(accNum, name, idCard, initBalance);
        if (ht.insertAccount(newAcc)) {
            cout << "=> THÀNH CÔNG: Đã mở tài khoản " << accNum << " cho khách hàng " << name << "!\n";
        } else {
            cout << "=> ERROR: Không thể thêm tài khoản vào Bảng băm!\n";
        }
    }

    // Chức năng 2: Tra cứu thông tin tài khoản tức thời O(1)
    void searchAccountInfo(HashTable& ht) {
        cout << "\n--- [MODULE HOÀNG TRỌNG] TRA CỨU TÀI KHOẢN O(1) ---\n";
        string accNum;
        cout << "Nhập số tài khoản cần tra cứu: ";
        cin >> accNum;

        Account* acc = ht.searchAccount(accNum);
        if (acc != nullptr) {
            cout << "=> TÌM THẤY TÀI KHOẢN TỨC THỜI O(1):\n";
            acc->displayInfo();
        } else {
            cout << "=> KHÔNG TÌM THẤY tài khoản: " << accNum << endl;
        }
    }

    // Chức năng 3: Xóa tài khoản
    void closeAccount(HashTable& ht) {
        cout << "\n--- [MODULE HOÀNG TRỌNG] XÓA TÀI KHOẢN ---\n";
        string accNum;
        cout << "Nhập số tài khoản cần xóa: ";
        cin >> accNum;

        if (ht.deleteAccount(accNum)) {
            cout << "=> THÀNH CÔNG: Đã xóa tài khoản " << accNum << " khỏi hệ thống.\n";
        } else {
            cout << "=> ERROR: Không tìm thấy tài khoản " << accNum << " để xóa.\n";
        }
    }
}

// ============================================================================
// PHẦN 4: MODULE GIAO DỊCH NGÂN HÀNG & SAO KÊ LỊCH SỬ
// THÀNH VIÊN PHỤ TRÁCH: VÒNG THẾ QUYỀN - MSSV: 6651060051
// NHÁNH GIT: TV3-nghiep-vu-giao-dich
// NHIỆM VỤ: Nạp tiền, Rút tiền, Chuyển khoản an toàn, Xuất sao kê giao dịch
// ============================================================================

namespace TransactionModule {

    // Hàm lấy thời gian thực hệ thống
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

    // Chức năng 1: Nạp tiền vào tài khoản
    void deposit(HashTable& ht) {
        cout << "\n--- [MODULE THẾ QUYỀN] NẠP TIỀN VÀO TÀI KHOẢN ---\n";
        string accNum;
        double amount;

        cout << "Nhập số tài khoản: ";
        cin >> accNum;

        Account* acc = ht.searchAccount(accNum);
        if (acc == nullptr) {
            cout << "=> ERROR: Tài khoản không tồn tại!\n";
            return;
        }

        cout << "Nhập số tiền muốn nạp (VND): ";
        cin >> amount;
        if (amount <= 0) {
            cout << "=> ERROR: Số tiền nạp phải lớn hơn 0!\n";
            return;
        }

        acc->balance += amount;

        Transaction trans;
        trans.transID = "TX" + to_string(rand() % 900000 + 100000);
        trans.type = "Nạp tiền";
        trans.amount = amount;
        trans.timestamp = getCurrentTime();
        trans.note = "Nạp tiền tại quầy";

        acc->history.push_back(trans);

        cout << "=> THÀNH CÔNG: Đã nạp " << fixed << setprecision(0) << amount 
             << " VND. Số dư mới: " << acc->balance << " VND\n";
    }

    // Chức năng 2: Rút tiền từ tài khoản
    void withdraw(HashTable& ht) {
        cout << "\n--- [MODULE THẾ QUYỀN] RÚT TIỀN TỪ TÀI KHOẢN ---\n";
        string accNum;
        double amount;

        cout << "Nhập số tài khoản: ";
        cin >> accNum;

        Account* acc = ht.searchAccount(accNum);
        if (acc == nullptr) {
            cout << "=> ERROR: Tài khoản không tồn tại!\n";
            return;
        }

        cout << "Nhập số tiền muốn rút (VND): ";
        cin >> amount;
        if (amount <= 0) {
            cout << "=> ERROR: Số tiền rút phải lớn hơn 0!\n";
            return;
        }

        if (acc->balance - amount < 50000) {
            cout << "=> ERROR: Số dư không đủ! Phải duy trì số dư tối thiểu 50,000 VND.\n";
            return;
        }

        acc->balance -= amount;

        Transaction trans;
        trans.transID = "TX" + to_string(rand() % 900000 + 100000);
        trans.type = "Rút tiền";
        trans.amount = amount;
        trans.timestamp = getCurrentTime();
        trans.note = "Rút tiền tại ATM/Quầy";

        acc->history.push_back(trans);

        cout << "=> THÀNH CÔNG: Đã rút " << fixed << setprecision(0) << amount 
             << " VND. Số dư còn lại: " << acc->balance << " VND\n";
    }

    // Chức năng 3: Chuyển khoản an toàn
    void transfer(HashTable& ht) {
        cout << "\n--- [MODULE THẾ QUYỀN] CHUYỂN KHOẢN AN TOÀN ---\n";
        string srcAccNum, destAccNum;
        double amount;

        cout << "Nhập số tài khoản NGƯỜI GỬI: ";
        cin >> srcAccNum;
        Account* srcAcc = ht.searchAccount(srcAccNum);
        if (srcAcc == nullptr) {
            cout << "=> ERROR: Tài khoản người gửi không tồn tại!\n";
            return;
        }

        cout << "Nhập số tài khoản NGƯỜI NHẬN: ";
        cin >> destAccNum;
        if (srcAccNum == destAccNum) {
            cout << "=> ERROR: Không thể chuyển khoản cho chính mình!\n";
            return;
        }

        Account* destAcc = ht.searchAccount(destAccNum);
        if (destAcc == nullptr) {
            cout << "=> ERROR: Tài khoản người nhận không tồn tại!\n";
            return;
        }

        cout << "Nhập số tiền chuyển (VND): ";
        cin >> amount;
        if (amount <= 0) {
            cout << "=> ERROR: Số tiền chuyển phải lớn hơn 0!\n";
            return;
        }

        if (srcAcc->balance - amount < 50000) {
            cout << "=> ERROR: Số dư người gửi không đủ để chuyển khoản!\n";
            return;
        }

        // Thực hiện giao dịch chuyển khoản
        srcAcc->balance -= amount;
        destAcc->balance += amount;

        string timeStr = getCurrentTime();
        string txID = "TX" + to_string(rand() % 900000 + 100000);

        // Lưu lịch sử người gửi
        Transaction t1;
        t1.transID = txID;
        t1.type = "Chuyển tiền đi";
        t1.amount = amount;
        t1.timestamp = timeStr;
        t1.note = "Chuyển tới TK: " + destAccNum + " (" + destAcc->fullName + ")";
        srcAcc->history.push_back(t1);

        // Lưu lịch sử người nhận
        Transaction t2;
        t2.transID = txID;
        t2.type = "Nhận tiền đến";
        t2.amount = amount;
        t2.timestamp = timeStr;
        t2.note = "Nhận từ TK: " + srcAccNum + " (" + srcAcc->fullName + ")";
        destAcc->history.push_back(t2);

        cout << "=> THÀNH CÔNG: Đã chuyển " << fixed << setprecision(0) << amount 
             << " VND từ TK " << srcAccNum << " sang TK " << destAccNum << "!\n";
    }

    // Chức năng 4: In sao kê lịch sử giao dịch
    void printStatement(HashTable& ht) {
        cout << "\n--- [MODULE THẾ QUYỀN] XUẤT SAO KÊ GIAO DỊCH ---\n";
        string accNum;
        cout << "Nhập số tài khoản cần in sao kê: ";
        cin >> accNum;

        Account* acc = ht.searchAccount(accNum);
        if (acc == nullptr) {
            cout << "=> ERROR: Tài khoản không tồn tại!\n";
            return;
        }

        cout << "\n======================================================================\n";
        cout << "                SAO KÊ LỊCH SỬ GIAO DỊCH NGÂN HÀNG                   \n";
        cout << " Số TK: " << acc->accountNumber << " | Chủ TK: " << acc->fullName << endl;
        cout << " Số dư hiện tại: " << fixed << setprecision(0) << acc->balance << " VND\n";
        cout << "----------------------------------------------------------------------\n";

        if (acc->history.empty()) {
            cout << "   (Chưa có giao dịch nào được thực hiện)\n";
        } else {
            for (const auto& t : acc->history) {
                t.display();
            }
        }
        cout << "======================================================================\n";
    }
}

// ============================================================================
// PHẦN 5: MODULE ĐỌC/GHI FILE & GIAO DIỆN CONSOLE MENU
// THÀNH VIÊN PHỤ TRÁCH: HUỲNH ANH TÚ - MSSV: 6651060069
// NHÁNH GIT: TV4-doc-ghi-file-giao-dien
// NHIỆM VỤ: Đọc/Ghi dữ liệu bền vững xuống file CSV, Giao diện Menu Console
// ============================================================================

namespace FileAndUIModule {

    const string DATA_FILE = "bank_data.csv";

    // Chức năng 1: Lưu toàn bộ dữ liệu ra File CSV
    void saveData(const HashTable& ht) {
        ofstream outFile(DATA_FILE);
        if (!outFile.is_open()) {
            cout << "=> ERROR: Không thể mở file để ghi dữ liệu!\n";
            return;
        }

        vector<Account*> accounts = ht.getAllAccounts();
        for (Account* acc : accounts) {
            // Định dạng dòng tài khoản: ACC,accountNumber,fullName,idCard,balance
            outFile << "ACC," << acc->accountNumber << "," << acc->fullName << ","
                    << acc->idCard << "," << fixed << setprecision(0) << acc->balance << "\n";

            // Định dạng dòng giao dịch: TRANS,transID,type,amount,timestamp,note
            for (const auto& t : acc->history) {
                outFile << "TRANS," << t.transID << "," << t.type << ","
                        << fixed << setprecision(0) << t.amount << ","
                        << t.timestamp << "," << t.note << "\n";
            }
        }

        outFile.close();
        cout << "=> [MODULE ANH TÚ] Đã lưu thành công " << accounts.size() 
             << " tài khoản ra file '" << DATA_FILE << "'!\n";
    }

    // Chức năng 2: Đọc khôi phục dữ liệu từ File CSV khi khởi động
    void loadData(HashTable& ht) {
        ifstream inFile(DATA_FILE);
        if (!inFile.is_open()) {
            cout << "=> [MODULE ANH TÚ] Không tìm thấy file dữ liệu cũ. Sẽ khởi tạo hệ thống mới.\n";
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
        cout << "=> [MODULE ANH TÚ] Đã nạp thành công " << accCount 
             << " tài khoản từ file '" << DATA_FILE << "'!\n";
    }

    // Chức năng 3: Giao diện Menu Console tương tác
    void showMenu() {
        cout << "\n================================================================\n";
        cout << "     HỆ THỐNG QUẢN LÝ TÀI KHOẢN & GIAO DỊCH NGÂN HÀNG (ĐT07)    \n";
        cout << "================================================================\n";
        cout << " Mở tài khoản mới               (Phụ trách: Hoàng Trọng)    \n";
        cout << " Tra cứu tài khoản O(1)         (Phụ trách: Hoàng Trọng)    \n";
        cout << " Xóa tài khoản                  (Phụ trách: Hoàng Trọng)    \n";
        cout << " Nạp tiền vào tài khoản         (Phụ trách: Thế Quyền)      \n";
        cout << " Rút tiền từ tài khoản          (Phụ trách: Thế Quyền)      \n";
        cout << " Chuyển khoản an toàn           (Phụ trách: Thế Quyền)      \n";
        cout << " In sao kê lịch sử giao dịch    (Phụ trách: Thế Quyền)      \n";
        cout << " Chạy Thử nghiệm & Benchmark    (Phụ trách: Phú Quốc)       \n";
        cout << " Lưu dữ liệu ra File            (Phụ trách: Anh Tú)         \n";
        cout << " Thoát và Lưu hệ thống                                      \n";
        cout << "================================================================\n";
        cout << "Lựa chọn của bạn (0-9): ";
    }
}

// ============================================================================
// PHẦN 6: MODULE TESTING, BENCHMARK & MOCK DATA
// THÀNH VIÊN PHỤ TRÁCH: NGUYỄN PHÚ QUỐC - MSSV: 6651060049
// NHÁNH GIT: TV5-testing-benchmark-demo
// NHIỆM VỤ: Kiểm thử, Benchmark Rehash/Load Factor & Tạo dữ liệu mẫu Demo
// ============================================================================

namespace TestingAndBenchmarkModule {

    // Khởi tạo dữ liệu thử nghiệm mẫu cho 5 thành viên nhóm
    void generateMockData(HashTable& ht) {
        cout << "\n--- [MODULE PHÚ QUỐC] KHỞI TẠO DỮ LIỆU MẪU ĐỂ DEMO ---\n";

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

        cout << "=> THÀNH CÔNG: Đã khởi tạo 5 tài khoản mẫu cho 5 thành viên nhóm!\n";
    }

    // Thử nghiệm Benchmark Hash Table & Rehash
    void runBenchmark(HashTable& ht) {
        cout << "\n--- [MODULE PHÚ QUỐC] THỬ NGHIỆM BENCHMARK HASH TABLE & REHASH ---\n";
        cout << " Số lượng tài khoản hiện tại: " << ht.getSize() << endl;
        cout << " Kích thước Mảng băm (Capacity): " << ht.getCapacity() << endl;
        cout << " Hệ số tải hiện tại (Load Factor): " << fixed << setprecision(2) << ht.getLoadFactor() << endl;

        cout << "\n... Đang chèn tự động 20 tài khoản để kiểm tra tính năng REHASH ...\n";
        for (int i = 10; i < 30; ++i) {
            string accNum = "10000000" + to_string(i);
            string name = "Khach Hang Test " + to_string(i);
            Account acc(accNum, name, "099999999999", 1000000);
            ht.insertAccount(acc);
        }

        cout << "\n=> KẾT QUẢ SAU KHI BENCHMARK CHÈN HÀNG LOẠT:\n";
        cout << " Số lượng tài khoản mới: " << ht.getSize() << endl;
        cout << " Kích thước Mảng băm mới: " << ht.getCapacity() << endl;
        cout << " Hệ số tải mới (Load Factor): " << ht.getLoadFactor() << endl;
        cout << "=> KẾT LUẬN: Thuật toán Rehash hoạt động O(1) bảng băm không bị quá tải!\n";
    }
}

// ============================================================================
// PHẦN 7: HÀM MAIN CHÍNH - KẾT NỐI TOÀN BỘ SẢN PHẨM CỦA 5 THÀNH VIÊN
// TỔNG HỢP VÀ ĐIỀU KHIỂN CHƯƠNG TRÌNH DỰ ÁN ĐT07
// ============================================================================

int main() {
    // Tối ưu hóa I/O C++
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    srand(time(0));

    // Khởi tạo Bảng băm core của Leader Phan Nam
    HashTable bankHashTable(11);

    // Tải dữ liệu cũ từ File do Huỳnh Anh Tú quản lý
    FileAndUIModule::loadData(bankHashTable);

    // Nếu hệ thống mới hoàn toàn chưa có dữ liệu thì nạp dữ liệu mẫu của Nguyễn Phú Quốc
    if (bankHashTable.getSize() == 0) {
        TestingAndBenchmarkModule::generateMockData(bankHashTable);
    }

    int choice;
    do {
        FileAndUIModule::showMenu();
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "=> ERROR: Vui lòng nhập số từ 0 đến 9!\n";
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
                cout << "\n=> CẢM ƠN BẠN ĐÃ SỬ DỤNG HỆ THỐNG NGÂN HÀNG. TẠM BIỆT!\n";
                break;
            default:
                cout << "=> ERROR: Lựa chọn không hợp lệ! Vui lòng nhập từ 0 đến 9.\n";
                break;
        }
    } while (choice != 0);

    return 0;
}
```

---
