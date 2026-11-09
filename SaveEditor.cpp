#include <windows.h>
#include <commdlg.h>
#include <shlobj.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>
#include <iomanip>
#include <ctime>

// IDs dos Controles da Interface
#define ID_BTN_OPEN 1
#define ID_BTN_SAVE 2
#define ID_EDIT_MONEY 3
#define ID_EDIT_LEVEL 4
#define ID_EDIT_POINTS 5

HWND hLblFile, hLblVersion, hLblCurrMoney, hLblCurrLevel, hLblCurrPoints;
HWND hEditMoney, hEditLevel, hEditPoints;

std::wstring g_filePath;
std::vector<char> g_fileData;
bool g_isXbox = false;

// Função robusta para converter string UTF-8 para wstring
std::wstring ToWString(const std::string& str) {
    if (str.empty()) return std::wstring();
    int size = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstr(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstr[0], size);
    return wstr;
}

// Função robusta para converter wstring para string UTF-8
std::string ToString(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    int size = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string str(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &str[0], size, NULL, NULL);
    return str;
}

// Lógica de injeção para Xbox (Compensação de bytes)
std::vector<char> PatchXboxBytes(const std::vector<char>& data, const std::vector<std::pair<std::string, std::string>>& targets) {
    std::vector<char> modified = data;
    for (const auto& target : targets) {
        std::string key = target.first;
        std::string newVal = target.second;

        std::string patternStr = key + R"(\s*:\s*([0-9.-]+))";
        std::regex rx(patternStr);
        std::string content(modified.begin(), modified.end());
        std::smatch match;

        if (std::regex_search(content, match, rx)) {
            size_t startPos = match.position(1);
            size_t matchLen = match.length(1);
            std::string oldVal = match.str(1);

            int diff = (int)newVal.size() - (int)oldVal.size();

            modified.erase(modified.begin() + startPos, modified.begin() + startPos + matchLen);
            modified.insert(modified.begin() + startPos, newVal.begin(), newVal.end());

            if (diff < 0) {
                modified.insert(modified.begin() + startPos + newVal.size(), -diff, ' ');
            }
            else if (diff > 0) {
                int removed = 0;
                size_t searchStart = (startPos > 5000) ? startPos - 5000 : 0;
                size_t searchEnd = min(modified.size(), startPos + newVal.size() + 5000);

                while (removed < diff) {
                    bool foundSpace = false;
                    for (size_t i = searchStart; i < searchEnd - 1; ++i) {
                        if (modified[i] == ' ' && modified[i + 1] == ' ') {
                            modified.erase(modified.begin() + i);
                            removed++;
                            searchEnd--;
                            foundSpace = true;
                            break;
                        }
                    }
                    if (!foundSpace) {
                        throw std::runtime_error("Not enough space structure to compensate for " + key + ". Try a smaller number.");
                    }
                }
            }
        }
    }
    return modified;
}

// Lógica de injeção para Steam (Escrita direta)
std::vector<char> PatchSteamBytes(const std::vector<char>& data, const std::vector<std::pair<std::string, std::string>>& targets) {
    std::vector<char> modified = data;
    for (const auto& target : targets) {
        std::string key = target.first;
        std::string newVal = target.second;

        std::string patternStr = key + R"(\s*:\s*([0-9.-]+))";
        std::regex rx(patternStr);
        std::string content(modified.begin(), modified.end());
        std::smatch match;

        if (std::regex_search(content, match, rx)) {
            size_t startPos = match.position(1);
            size_t matchLen = match.length(1);

            modified.erase(modified.begin() + startPos, modified.begin() + startPos + matchLen);
            modified.insert(modified.begin() + startPos, newVal.begin(), newVal.end());
        }
    }
    return modified;
}

void OpenFileDlg(HWND hwnd) {
    wchar_t szFile[260] = { 0 };
    OPENFILENAMEW ofn = { 0 };
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);
    ofn.lpstrFilter = L"All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameW(&ofn) == TRUE) {
        g_filePath = szFile;
        std::ifstream file(g_filePath, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            MessageBoxW(hwnd, L"Could not open file!", L"Error", MB_ICONERROR);
            return;
        }

        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        g_fileData.resize((size_t)size);
        if (!file.read(g_fileData.data(), size)) {
            MessageBoxW(hwnd, L"Could not read file data!", L"Error", MB_ICONERROR);
            return;
        }

        std::wstring fileName = g_filePath.substr(g_filePath.find_last_of(L"/\\") + 1);
        bool isEs3 = (fileName.length() > 4 && fileName.substr(fileName.length() - 4) == L".es3");

        std::string contentSample(g_fileData.begin(), g_fileData.begin() + min(g_fileData.size(), (size_t)100));
        bool isJsonStart = (contentSample.find('{') != std::string::npos);

        if (isEs3 || isJsonStart) {
            g_isXbox = false;
            SetWindowTextW(hLblVersion, L"Detected Version: STEAM (.es3 / Plain Text)");
        }
        else {
            g_isXbox = true;
            SetWindowTextW(hLblVersion, L"Detected Version: XBOX / GAME PASS (Binary)");
        }

        std::string fileStr(g_fileData.begin(), g_fileData.end());
        std::smatch mMoney, mLevel, mPoints;

        std::string curMoney = "0", curLevel = "1", curPoints = "0";
        if (std::regex_search(fileStr, mMoney, std::regex(R"("Money"\s*:\s*([0-9.-]+))"))) curMoney = mMoney[1];
        if (std::regex_search(fileStr, mLevel, std::regex(R"("CurrentStoreLevel"\s*:\s*([0-9.-]+))"))) curLevel = mLevel[1];
        if (std::regex_search(fileStr, mPoints, std::regex(R"("CurrentStorePoint"\s*:\s*([0-9.-]+))"))) curPoints = mPoints[1];

        SetWindowTextW(hLblFile, g_filePath.c_str());
        SetWindowTextW(hLblCurrMoney, (L"Current: " + ToWString(curMoney)).c_str());
        SetWindowTextW(hLblCurrLevel, (L"Current: " + ToWString(curLevel)).c_str());
        SetWindowTextW(hLblCurrPoints, (L"Current: " + ToWString(curPoints)).c_str());

        SetWindowTextW(hEditMoney, ToWString(curMoney).c_str());
        SetWindowTextW(hEditLevel, ToWString(curLevel).c_str());
        SetWindowTextW(hEditPoints, ToWString(curPoints).c_str());
    }
}

void SaveFileAction(HWND hwnd) {
    if (g_fileData.empty() || g_filePath.empty()) {
        MessageBoxW(hwnd, L"Please open a save file first.", L"Warning", MB_ICONWARNING);
        return;
    }

    wchar_t bufMoney[64], bufLevel[64], bufPoints[64];
    GetWindowTextW(hEditMoney, bufMoney, 64);
    GetWindowTextW(hEditLevel, bufLevel, 64);
    GetWindowTextW(hEditPoints, bufPoints, 64);

    try {
        std::string sMoney = ToString(bufMoney);
        std::string sLevel = ToString(bufLevel);
        std::string sPoints = ToString(bufPoints);

        // Validação idêntica à versão Python
        try {
            std::stod(sMoney);
            std::stoi(sLevel);
            std::stoi(sPoints);
        }
        catch (...) {
            MessageBoxW(hwnd, L"Invalid values! Use integers for Level/Points and a dot for decimals in Money.", L"Error", MB_ICONERROR);
            return;
        }

        // 1. Criação do backup na pasta Documentos exatamente como no Python (com timestamp)
        wchar_t docsPath[MAX_PATH];
        std::wstring backupPathStr = L"";
        if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_MYDOCUMENTS, NULL, 0, docsPath))) {
            std::wstring backupDir = std::wstring(docsPath) + L"\\Supermarket Simulator Backups";
            CreateDirectoryW(backupDir.c_str(), NULL);

            time_t t = time(nullptr);
            tm timeInfo;
            localtime_s(&timeInfo, &t);
            wchar_t timeBuf[64];
            wcsftime(timeBuf, 64, L"%Y%m%d_%H%M%S", &timeInfo);

            std::wstring fileName = g_filePath.substr(g_filePath.find_last_of(L"/\\") + 1);
            backupPathStr = backupDir + L"\\" + fileName + L"_" + timeBuf + L".backup";

            CopyFileW(g_filePath.c_str(), backupPathStr.c_str(), FALSE);
        }

        std::vector<std::pair<std::string, std::string>> targets = {
            {"\"Money\"", sMoney},
            {"\"CurrentStoreLevel\"", sLevel},
            {"\"CurrentStorePoint\"", sPoints}
        };

        std::vector<char> newBytes;
        if (g_isXbox) {
            newBytes = PatchXboxBytes(g_fileData, targets);
        }
        else {
            newBytes = PatchSteamBytes(g_fileData, targets);
        }

        std::ofstream outFile(g_filePath, std::ios::binary);
        if (!outFile.is_open()) {
            throw std::runtime_error("Could not write to original file!");
        }
        outFile.write(newBytes.data(), newBytes.size());
        outFile.close();

        g_fileData = newBytes;
        SetWindowTextW(hLblCurrMoney, (L"Current: " + ToWString(sMoney)).c_str());
        SetWindowTextW(hLblCurrLevel, (L"Current: " + ToWString(sLevel)).c_str());
        SetWindowTextW(hLblCurrPoints, (L"Current: " + ToWString(sPoints)).c_str());

        // Mensagem de sucesso idêntica à versão Python
        std::wstring platformUsed = g_isXbox ? L"Xbox/Game Pass (Byte compensation applied)" : L"Steam (Direct write applied)";
        std::wstring successMsg = L"Save successfully modified and overwritten!\nPlatform used: " + platformUsed + L"\n\nYour original backup is safely stored at:\n" + backupPathStr;

        MessageBoxW(hwnd, successMsg.c_str(), L"Success", MB_ICONINFORMATION);
    }
    catch (const std::exception& e) {
        MessageBoxW(hwnd, ToWString(e.what()).c_str(), L"Save Error", MB_ICONERROR);
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        CreateWindowW(L"STATIC", L"Supermarket Simulator Save Editor", WS_VISIBLE | WS_CHILD, 20, 20, 500, 25, hwnd, NULL, NULL, NULL);

        CreateWindowW(L"BUTTON", L"Open Save", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 20, 60, 120, 30, hwnd, (HMENU)ID_BTN_OPEN, NULL, NULL);
        hLblFile = CreateWindowW(L"STATIC", L"No file selected", WS_VISIBLE | WS_CHILD, 150, 65, 410, 20, hwnd, NULL, NULL, NULL);

        hLblVersion = CreateWindowW(L"STATIC", L"Detected Version: None", WS_VISIBLE | WS_CHILD, 20, 105, 540, 20, hwnd, NULL, NULL, NULL);

        // Money
        CreateWindowW(L"STATIC", L"Money:", WS_VISIBLE | WS_CHILD, 20, 145, 90, 20, hwnd, NULL, NULL, NULL);
        hLblCurrMoney = CreateWindowW(L"STATIC", L"Current: -", WS_VISIBLE | WS_CHILD, 120, 145, 170, 20, hwnd, NULL, NULL, NULL);
        hEditMoney = CreateWindowW(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 300, 142, 260, 25, hwnd, (HMENU)ID_EDIT_MONEY, NULL, NULL);

        // Store Level
        CreateWindowW(L"STATIC", L"Store Level:", WS_VISIBLE | WS_CHILD, 20, 185, 90, 20, hwnd, NULL, NULL, NULL);
        hLblCurrLevel = CreateWindowW(L"STATIC", L"Current: -", WS_VISIBLE | WS_CHILD, 120, 185, 170, 20, hwnd, NULL, NULL, NULL);
        hEditLevel = CreateWindowW(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 300, 182, 260, 25, hwnd, (HMENU)ID_EDIT_LEVEL, NULL, NULL);

        // Store Points
        CreateWindowW(L"STATIC", L"Store Points:", WS_VISIBLE | WS_CHILD, 20, 225, 90, 20, hwnd, NULL, NULL, NULL);
        hLblCurrPoints = CreateWindowW(L"STATIC", L"Current: -", WS_VISIBLE | WS_CHILD, 120, 225, 170, 20, hwnd, NULL, NULL, NULL);
        hEditPoints = CreateWindowW(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 300, 222, 260, 25, hwnd, (HMENU)ID_EDIT_POINTS, NULL, NULL);

        CreateWindowW(L"BUTTON", L"Save Edited File", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 400, 275, 160, 35, hwnd, (HMENU)ID_BTN_SAVE, NULL, NULL);
        break;
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == ID_BTN_OPEN) {
            OpenFileDlg(hwnd);
        }
        else if (LOWORD(wParam) == ID_BTN_SAVE) {
            SaveFileAction(hwnd);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"SaveEditorClass";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(
        0, L"SaveEditorClass", L"Supermarket Simulator Save Editor",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 610, 370,
        NULL, NULL, hInstance, NULL
    );

    if (!hwnd) return 0;

    ShowWindow(hwnd, nCmdShow);

    MSG msg = { 0 };
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return 0;
}