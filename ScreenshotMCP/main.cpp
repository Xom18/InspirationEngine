#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <string>
#include <vector>
#include <iostream>
#include "include/nlohmann/json.hpp"

using json = nlohmann::json;

// ─── Base64 ──────────────────────────────────────────────────────────────────

static const char* B64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static std::string base64Encode(const std::vector<uint8_t>& data)
{
    std::string out;
    out.reserve(((data.size() + 2) / 3) * 4);
    for (size_t i = 0; i < data.size(); i += 3)
    {
        uint32_t n = (uint32_t)data[i] << 16;
        if (i + 1 < data.size()) n |= (uint32_t)data[i + 1] << 8;
        if (i + 2 < data.size()) n |= (uint32_t)data[i + 2];
        out += B64[(n >> 18) & 63];
        out += B64[(n >> 12) & 63];
        out += (i + 1 < data.size()) ? B64[(n >> 6) & 63] : '=';
        out += (i + 2 < data.size()) ? B64[n & 63] : '=';
    }
    return out;
}

// ─── Win32 캡처 ──────────────────────────────────────────────────────────────

struct FindData { std::string search; HWND hwnd = nullptr; };

static BOOL CALLBACK findWindowProc(HWND hwnd, LPARAM lp)
{
    if (!IsWindowVisible(hwnd)) return TRUE;
    char buf[512];
    GetWindowTextA(hwnd, buf, sizeof(buf));
    auto* d = reinterpret_cast<FindData*>(lp);
    if (std::string(buf).find(d->search) != std::string::npos)
    {
        d->hwnd = hwnd;
        return FALSE;
    }
    return TRUE;
}

static BOOL CALLBACK listWindowsProc(HWND hwnd, LPARAM lp)
{
    if (!IsWindowVisible(hwnd)) return TRUE;
    char buf[512];
    GetWindowTextA(hwnd, buf, sizeof(buf));
    if (buf[0] != '\0')
        reinterpret_cast<std::vector<std::string>*>(lp)->push_back(buf);
    return TRUE;
}

// PrintWindow로 캡처 — 가려진 창도 정확히 캡처
static std::vector<uint8_t> captureWindowToBMP(const std::string& titleSearch)
{
    FindData fd;
    fd.search = titleSearch;
    EnumWindows(findWindowProc, reinterpret_cast<LPARAM>(&fd));
    if (!fd.hwnd) return {};

    RECT rect;
    GetClientRect(fd.hwnd, &rect);
    int w = rect.right;
    int h = rect.bottom;
    if (w <= 0 || h <= 0) return {};

    HDC hdcScreen = GetDC(nullptr);
    HDC hdcMem    = CreateCompatibleDC(hdcScreen);

    BITMAPINFOHEADER bi = {};
    bi.biSize        = sizeof(bi);
    bi.biWidth       = w;
    bi.biHeight      = -h;            // 음수 = top-down
    bi.biPlanes      = 1;
    bi.biBitCount    = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage   = w * h * 4;

    void*   pBits   = nullptr;
    HBITMAP hBitmap = CreateDIBSection(hdcMem,
                        reinterpret_cast<BITMAPINFO*>(&bi),
                        DIB_RGB_COLORS, &pBits, nullptr, 0);
    HGDIOBJ hOld    = SelectObject(hdcMem, hBitmap);

    PrintWindow(fd.hwnd, hdcMem, PW_CLIENTONLY);

    // 50% 다운스케일 (nearest-neighbor) — BMP 크기 감소
    int sw = w / 2;
    int sh = h / 2;
    if (sw < 1) sw = 1;
    if (sh < 1) sh = 1;
    std::vector<uint8_t> smallPixels(sw * sh * 4);
    uint8_t* src = static_cast<uint8_t*>(pBits);
    for (int sy = 0; sy < sh; sy++) {
        for (int sx = 0; sx < sw; sx++) {
            int srcOff = (sy * 2) * w * 4 + (sx * 2) * 4;
            int dstOff = sy * sw * 4 + sx * 4;
            smallPixels[dstOff+0] = src[srcOff+0];
            smallPixels[dstOff+1] = src[srcOff+1];
            smallPixels[dstOff+2] = src[srcOff+2];
            smallPixels[dstOff+3] = src[srcOff+3];
        }
    }

    // BMP 파일 메모리 조립 (스케일된 크기)
    BITMAPINFOHEADER biSmall = bi;
    biSmall.biWidth     = sw;
    biSmall.biHeight    = -sh;
    biSmall.biSizeImage = sw * sh * 4;

    BITMAPFILEHEADER bfh = {};
    bfh.bfType    = 0x4D42;   // 'BM'
    bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bfh.bfSize    = bfh.bfOffBits + biSmall.biSizeImage;

    std::vector<uint8_t> result(bfh.bfSize);
    uint8_t* p = result.data();
    memcpy(p, &bfh,    sizeof(bfh));    p += sizeof(bfh);
    memcpy(p, &biSmall, sizeof(biSmall)); p += sizeof(biSmall);
    memcpy(p, smallPixels.data(), biSmall.biSizeImage);

    SelectObject(hdcMem, hOld);
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(nullptr, hdcScreen);

    return result;
}

// ─── MCP 헬퍼 ────────────────────────────────────────────────────────────────

static void send(const json& msg)
{
    std::cout << msg.dump() << "\n";
    std::cout.flush();
}

static json makeResult(const json& id, json res)
{
    return { {"jsonrpc","2.0"}, {"id", id}, {"result", std::move(res)} };
}

static json makeError(const json& id, int code, const std::string& msg)
{
    return { {"jsonrpc","2.0"}, {"id", id}, {"error", {{"code",code},{"message",msg}}} };
}

static json toolList()
{
    return json::array({
        {
            {"name", "capture_window"},
            {"description", "창 제목 키워드로 게임 창을 캡처. 다른 창에 가려진 경우에도 Win32 PrintWindow로 정확히 캡처됨."},
            {"inputSchema", {
                {"type", "object"},
                {"properties", {{"title", {{"type","string"},{"description","창 제목 키워드 (부분 일치)"}}}}},
                {"required", json::array({"title"})}
            }}
        },
        {
            {"name", "list_windows"},
            {"description", "현재 열린 창 제목 목록 반환 (캡처 가능한 창 확인용)"},
            {"inputSchema", {{"type","object"},{"properties",json::object()}}}
        }
    });
}

// ─── main ────────────────────────────────────────────────────────────────────

int main()
{
    _setmode(_fileno(stdin),  _O_BINARY);
    _setmode(_fileno(stdout), _O_BINARY);

    std::string line;
    while (std::getline(std::cin, line))
    {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;

        json req;
        try { req = json::parse(line); }
        catch (...) { continue; }

        std::string method = req.value("method", "");
        json id = req.contains("id") ? req["id"] : json(nullptr);
        bool isNotif = !req.contains("id");

        if (method == "initialize")
        {
            send(makeResult(id, {
                {"protocolVersion", "2024-11-05"},
                {"capabilities", {{"tools", json::object()}}},
                {"serverInfo", {{"name","game-screenshot"},{"version","1.0.0"}}}
            }));
        }
        else if (method == "tools/list")
        {
            send(makeResult(id, {{"tools", toolList()}}));
        }
        else if (method == "tools/call")
        {
            std::string toolName;
            json args = json::object();
            try {
                toolName = req["params"]["name"].get<std::string>();
                if (req["params"].contains("arguments"))
                    args = req["params"]["arguments"];
            }
            catch (...) {
                if (!isNotif) send(makeError(id, -32600, "invalid params"));
                continue;
            }

            if (toolName == "capture_window")
            {
                std::string title = args.value("title", "");
                auto bmpBytes = captureWindowToBMP(title);

                if (bmpBytes.empty())
                {
                    send(makeResult(id, {{"content", json::array({{
                        {"type","text"}, {"text","창을 찾을 수 없음: " + title}
                    }})}}));
                }
                else
                {
                    send(makeResult(id, {{"content", json::array({{
                        {"type","image"},
                        {"data", base64Encode(bmpBytes)},
                        {"mimeType","image/bmp"}
                    }})}}));
                }
            }
            else if (toolName == "list_windows")
            {
                std::vector<std::string> titles;
                EnumWindows(listWindowsProc, reinterpret_cast<LPARAM>(&titles));
                json arr = json::array();
                for (auto& t : titles) arr.push_back(t);
                send(makeResult(id, {{"content", json::array({{
                    {"type","text"}, {"text", arr.dump(2)}
                }})}}));
            }
            else
            {
                send(makeError(id, -32601, "unknown tool: " + toolName));
            }
        }
        else if (!isNotif)
        {
            send(makeError(id, -32601, "unknown method: " + method));
        }
    }
    return 0;
}
