# 🚀 Hướng dẫn build APK qua GitHub Actions

## Bước 1 — Tạo repo GitHub

1. Vào https://github.com/new
2. Đặt tên: `tv-remote`
3. Để **Public** (miễn phí dùng Actions)
4. Bấm **Create repository**

## Bước 2 — Upload code lên GitHub

Cách 1 — **Kéo thả** (dễ nhất):
1. Giải nén file `TVRemote.zip`
2. Vào repo → bấm **uploading an existing file**
3. Kéo toàn bộ thư mục vào
4. Bấm **Commit changes**

Cách 2 — **Git command**:
```bash
cd TVRemote
git init
git add .
git commit -m "Initial commit"
git remote add origin https://github.com/TÊN_BẠN/tv-remote.git
git push -u origin main
```

## Bước 3 — GitHub Actions tự động build

Sau khi push code, GitHub sẽ **tự động**:
1. Cài Qt 6.5 + Android NDK
2. Build APK
3. Tạo Release với file APK để tải

Xem tiến trình tại tab **Actions** trong repo.

---

## Tải APK

Sau khi build xong (~15-20 phút):

**Cách 1 — Từ Releases:**
```
https://github.com/TÊN_BẠN/tv-remote/releases/latest
```

**Cách 2 — Từ Artifacts:**
- Vào tab Actions → chọn build mới nhất
- Kéo xuống phần **Artifacts** → tải `TVRemote-APK`

---

## Cài APK lên điện thoại

1. Gửi file APK sang điện thoại (qua Drive, Telegram...)
2. Mở file → nếu bị chặn, vào:
   `Cài đặt → Bảo mật → Cài ứng dụng từ nguồn không rõ → Bật`
3. Cài và mở app

---

## Setup Keystore cho Release (tuỳ chọn)

Để ký APK bằng key riêng thay vì debug key:

```bash
# Tạo keystore
keytool -genkeypair -alias tvremote -keypass MẬT_KHẨU \
  -keystore tvremote.keystore -storepass MẬT_KHẨU \
  -dname "CN=TÊN, O=TỔ_CHỨC, C=VN" \
  -validity 10000 -keyalg RSA -keysize 2048

# Encode thành base64
base64 tvremote.keystore
```

Thêm vào **GitHub Secrets** (Settings → Secrets → Actions):

| Secret | Giá trị |
|--------|---------|
| `KEYSTORE_BASE64` | Chuỗi base64 từ lệnh trên |
| `KEYSTORE_PASSWORD` | Mật khẩu store |
| `KEY_ALIAS` | `tvremote` |
| `KEY_PASSWORD` | Mật khẩu key |

Sau đó bỏ comment phần **Decode release keystore** trong `build-apk.yml`.

---

## Thời gian build ước tính

| Bước | Thời gian |
|------|-----------|
| Cài Qt + NDK | ~8 phút |
| Configure CMake | ~1 phút |
| Build APK | ~5 phút |
| **Tổng lần đầu** | **~15 phút** |
| **Lần sau (có cache Qt)** | **~6 phút** |
