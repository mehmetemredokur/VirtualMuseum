# Virtual Museum

Bu proje, C++/OpenGL ile yazılmış bir “Sanal Müze” uygulamasıdır. Beş adet `.obj` modeli, spot ışıkları, robot turu ve ImGui tabanlı bir kullanıcı arayüzü içerir.

---

## Gereksinimler

- **İşletim Sistemi:** Windows 10 / 11 (64-bit)  
- **Derleyici & IDE:** Visual Studio 2022 (Community veya üstü), “Desktop development with C++” iş yükü  
- **Araçlar:**
  - [CMake ≥ 3.15](https://cmake.org/download/)  
  - [Git for Windows](https://git-scm.com/download/win)  
  - [Python 3](https://python.org) (vcpkg’nin `opengl-registry` portu için)  
- **Paket Yöneticisi:** [vcpkg](https://github.com/microsoft/vcpkg)

---

## 1. vcpkg Kurulumu & Bağımlılıkların Yüklenmesi

1. **vcpkg’yi klonlayın**  
   ```powershell
   cd C:   git clone https://github.com/microsoft/vcpkg.git
   cd vcpkg
   .\bootstrap-vcpkg.bat
   .\vcpkg integrate install
   ```

2. **Gerekli paketleri yükleyin**  
   ```powershell
   cd C:\vcpkg
   .\vcpkg install glfw3:x64-windows glm:x64-windows assimp:x64-windows glad:x64-windows imgui:x64-windows
   ```

---

## 2. Projeyi Klonlama

```powershell
cd C:\Users\Teslime\Desktop
git clone <proje-repo-adresi> VirtualMuseum
cd VirtualMuseum
```

> Proje klasörünüz şöyle olmalı:  
> ```
> VirtualMuseum/
> ├─ models/
> ├─ shaders/
> ├─ src/
> └─ CMakeLists.txt
> ```

---

## 3. Derleme & Yapılandırma

1. **Build** klasörünü oluşturun ve içine girin:
   ```powershell
   mkdir build
   cd build
   ```

2. **CMake ile yapılandırma** (tek satırda):
   ```powershell
   cmake .. `
     -DCMAKE_TOOLCHAIN_FILE="C:/vcpkg/scripts/buildsystems/vcpkg.cmake" `
     -A x64
   ```
   - Windows PowerShell için satır sonu karakteri `` ` `` (backtick).  
   - CMD kullanıyorsanız hepsini tek satırda yazın.

3. **Derleme**:
   ```powershell
   cmake --build . --config Release
   ```

Derleme başarılıysa, `build/Release/VirtualMuseum.exe` oluşur.

---

## 4. Çalışma Dizinlerinin Ayarlanması

Program, çalışma dizininde `models/` ve `shaders/` klasörlerini arar.

### A) Proje kökünden çalıştırma (Önerilen)

```powershell
cd C:\Users\Teslime\Desktop\VirtualMuseum
.\build\Release\VirtualMuseum.exe
```

### B) build/Release altında çalıştırma

```powershell
cd C:\Users\Teslime\Desktop\VirtualMuseum\build\Release
xcopy /E /I ..\..\models .\models
xcopy /E /I ..\..\shaders .\shaders
.\VirtualMuseum.exe
```

---

## 5. Visual Studio’dan Çalıştırma

1. `.sln` dosyasını Visual Studio’da açın.  
2. **Solution Configurations**: `Release` veya `Debug` seçin.  
3. **Project → Properties → Debugging**  
   - **Working Directory**: `$(ProjectDir)\..` veya proje kökü olarak ayarlayın.  
4. **Ctrl+F5** ile çalıştırın.

---

## 6. Sorun Giderme

- **“Unable to open file models/*.obj”** → Kaynak dizinleri yanlış; 4. adımı kontrol edin.  
- **“Error: generator platform: x64”** → `build` klasörünü silip yeniden oluşturun.  
- **vcpkg build hataları** → `git pull`, `bootstrap-vcpkg.bat`, `vcpkg update` deneyin; Python kurulu mu kontrol edin.

---

İyi çalışmalar!
