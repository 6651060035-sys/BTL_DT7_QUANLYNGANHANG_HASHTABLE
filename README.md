# BTL_DT7_QUANLYNGANHANG_HASHTABLE
# Hệ thống Quản lý Tài khoản & Giao dịch Khách hàng Ngân hàng (Dự án ĐT07)

Hệ thống quản lý tài khoản và giao dịch khách hàng ngân hàng được xây dựng bằng ngôn ngữ **C++**, sử dụng cấu trúc dữ liệu **Bảng băm (Hash Table)** kết hợp với kỹ thuật **Separate Chaining** và **Rehash tự động** nhằm đảm bảo tốc độ truy xuất tối ưu O(1).

---

## 1. Mô tả đề tài
* **Tên đề tài:** Hệ thống Quản lý Tài khoản & Giao dịch Khách hàng Ngân hàng (Dự án ĐT07).
* **Cấu trúc dữ liệu cốt lõi:** Sử dụng **Bảng băm (Hash Table)** kết hợp **Mảng con trỏ và Danh sách liên kết đơn (Separate Chaining)** để xử lý đụng độ. Sử dụng hàm băm chuỗi (**Polynomial Rolling Hash**) và tự động **Rehash** mở rộng bảng khi hệ số tải (Load Factor) vượt ngưỡng 0.75.
* **Nghiệp vụ quản lý & giao dịch:**
  * Mở tài khoản khách hàng mới, tự động cấp số tài khoản và băm lưu trữ.
  * Tra cứu thông tin tài khoản tức thời với độ phức tạp O(1).
  * Thực hiện các giao dịch nghiệp vụ: Nạp tiền, Rút tiền, Chuyển khoản an toàn giữa các tài khoản.
  * Quản lý lịch sử giao dịch và xuất sao kê tài khoản.
* **Lưu trữ dữ liệu:** Đọc/ghi dữ liệu bền vững xuống file (`.txt` / `.csv` / `.dat`).

---

## 2. Công nghệ sử dụng
| Thành phần | Công nghệ / Công cụ | Mục đích |
| :--- | :--- | :--- |
| **Ngôn ngữ lập trình** | C++ (C++11) | Xây dựng chương trình quản lý |
| **Cấu trúc dữ liệu** | Bảng băm (Hash Table) | Tổ chức, quản lý và tra cứu thông tin O(1) |
| **Lưu trữ dữ liệu** | File văn bản / Nhị phân | Lưu trữ và khôi phục dữ liệu bền vững |
| **Quản lý mã nguồn** | Git, GitHub | Quản lý phiên bản và phối hợp nhóm |
| **Theo dõi tiến độ** | Trello / Google Sheets | Phân công và giám sát tiến độ công việc |
| **Soạn thảo báo cáo** | Microsoft Word, LaTeX, PDF | Trình bày báo cáo học thuật (40–50 trang) |
| **Biên dịch & Trình diễn** | Dev-C++, GCC, GDB Online | Biên dịch, kiểm thử và chạy trình diễn |

---

## 3. Cấu trúc thư mục dự án
```text
.
├── Account.h                   # Khai báo cấu trúc Account & Transaction
├── HashTable.h                 # Cài đặt Bảng băm, Hàm băm và thuật toán Rehash
├── main.cpp                    # Chương trình chính và Giao diện Console Menu
├── BaoCao_BTL_DT07.docx        # Quyển báo cáo học thuật (40 - 50 trang)
├── du_lieu_tai_khoan.dat       # File dữ liệu lưu trữ tài khoản & lịch sử giao dịch
└── README.md                   # Tài liệu giới thiệu và hướng dẫn dự án
```

---

## 4. Phân công nhiệm vụ và nhánh Git

| Nhánh Git (Branch) | Thành viên phụ trách | MSSV | Nhiệm vụ và nội dung công việc |
| :--- | :--- | :---: | :--- |
| `main` / `TV1-leader-core-hash-table` | **Phan Nam** *(Leader)* | **6651060035** | Khởi tạo Repo, quản lý tiến độ Trello/Git; Cài đặt Core Hash Table, Polynomial Hash, Rehash & Soạn Báo cáo Chương 2. |
| `TV2-quan-ly-tai-khoan` | **Nguyễn Phương Hoàng Trọng** | **6651060065** | Phát triển phân hệ Mở tài khoản mới, Tra cứu tài khoản O(1), Bắt lỗi ngoại lệ & Soạn Báo cáo Chương 1. |
| `TV3-nghiep-vu-giao-dich` | **Vòng Thế Quyền** | **6651060051** | Phát triển phân hệ Giao dịch (Nạp, Rút, Chuyển khoản), Lịch sử sao kê & Soạn Báo cáo Chương 3 (Mã nguồn). |
| `TV4-doc-ghi-file-giao-dien` | **Huỳnh Anh Tú** | **6651060069** | Phát triển phân hệ Đọc/Ghi dữ liệu ra File, Thiết kế Menu Console & Format Báo cáo chuẩn 40–50 trang. |
| `TV5-testing-benchmark-demo` | **Nguyễn Phú Quốc** | **6651060049** | Kiểm thử, đánh giá độ phức tạp O(1)/Rehash, kiểm tra Memory Leak, Chuẩn bị Slide & Kịch bản Demo, Soạn Báo cáo Chương 4. |
````


## 5. Tiến độ và sản phẩm bàn giao
| Mã công việc | Hạng mục | Sản phẩm đầu ra |
| :---: | :--- | :--- |
| **YC01** | Quản lý mã nguồn & Kiểm soát phiên bản | Repository GitHub, các nhánh làm việc, lịch sử commit minh bạch và file README.md. |
| **YC02** | Quản lý & Giám sát tiến độ thành viên | Bảng Trello/WBS với minh chứng phân công nhiệm vụ, deadline và tỷ lệ % đóng góp. |
| **YC03** | Quyển báo cáo học thuật | Báo cáo BTL độ dài 40–50 trang, chuẩn bố cục 4 chương, sơ đồ kiến trúc và trích dẫn mã nguồn. |
| **YC04** | Chương trình C++ & Báo cáo Demo | Mã nguồn C++ hoàn chỉnh không rò rỉ bộ nhớ, file lưu trữ dữ liệu, Slide thuyết trình và Kịch bản Demo. |

---

## 6. Hướng dẫn thiết lập, biên dịch và chạy chương trình

### Cách 1: Chạy trực tuyến trên GDB Online
1. Truy cập GDB Online C++ Compiler (`https://www.onlinegdb.com/online_c++_compiler`).
2. Chọn ngôn ngữ **C++** và chuẩn **C++11**.
3. Sao chép toàn bộ mã nguồn vào trình soạn thảo và nhấn **Run**.

### Cách 2: Biên dịch bằng Dev-C++
1. Mở Dev-C++ và tạo Project mới.
2. Chọn **Tools → Compiler Options**.
3. Tích chọn thêm lệnh cho trình biên dịch và nhập tham số: `-std=c++11`.
4. Nhấn **OK**, sau đó chọn **Compile & Run** (hoặc phím **F11**).

### Cách 3: Biên dịch bằng Terminal / Command Prompt
1. Mở Terminal tại thư mục dự án.
2. Biên dịch chương trình bằng GCC với chuẩn C++11:
   ```bash
   g++ -std=c++11 main.cpp -o quan_ly_tai_khoan
   ```
3. Chạy chương trình:
   * **Windows:** `quan_ly_tai_khoan.exe`
   * **Linux / macOS:** `./quan_ly_tai_khoan`

---

## 7. Tác giả và thông tin dự án
* **Tên dự án:** Hệ thống Quản lý Tài khoản & Giao dịch Khách hàng Ngân hàng
* **Mã đề tài:** ĐT07
* **Ngôn ngữ:** C++ (C++11)
* **Cấu trúc dữ liệu chính:** Bảng băm (Hash Table)
* **Nền tảng quản lý:** GitHub & Trello
````
