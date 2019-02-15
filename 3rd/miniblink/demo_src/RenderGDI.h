#ifndef RENDER_GDI_H
#define RENDER_GDI_H

#include <windows.h>

class CRenderGDI {
public:
    CRenderGDI()
        :m_hView(nullptr)
        ,m_hDC(nullptr)
        ,m_hBitmap(nullptr)
        ,m_pixels(nullptr)
        ,m_width(0)
        ,m_height(0)
    {}

    ~CRenderGDI()
    {
        if (m_hDC)
            ::DeleteDC(m_hDC);

        if (m_hBitmap)
            ::DeleteObject(m_hBitmap);
    }

    virtual bool init(HWND hView)
    {
        m_hView = hView;
        m_hDC = ::CreateCompatibleDC(0);

        RECT rect;
        ::GetClientRect(hView, &rect);
        resize(rect.right, rect.bottom);
        return true;
    }

    virtual void destroy()
    {
        delete this;
    }

    virtual void resize(unsigned int w, unsigned int h)
    {
        if (m_width == w && m_height == h)
            return;

        m_width = w;
        m_height = h;
        m_pixels = nullptr;
    }

    void renderOnBlinkPaint(wkeWebView webView, HDC hBlinkDC, int x, int y, int cx, int cy)
    {
        if (m_pixels == nullptr) {
            if (m_width != cx || m_height != cy)
                return;
            createBitmap();
        }

        HDC hScreenDC = ::GetDC(m_hView);
        ::BitBlt(m_hDC, x, y, m_width, m_height, hBlinkDC, x, y, SRCCOPY);
        ::BitBlt(hScreenDC, x, y, m_width, m_height, m_hDC, x, y, SRCCOPY);
        ::ReleaseDC(m_hView, hScreenDC);
    }

    void renderOnWindowPaint(wkeWebView webView, HDC hScreenDC)
    {
        if (m_pixels == nullptr) {
            return;
        }

        ::BitBlt(hScreenDC, 0, 0, m_width, m_height, m_hDC, 0, 0, SRCCOPY);
    }

    void createBitmap()
    {
        BITMAPINFO bi;
        memset(&bi, 0, sizeof(bi));
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth         = int(m_width);
        bi.bmiHeader.biHeight        = -int(m_height);
        bi.bmiHeader.biPlanes        = 1;
        bi.bmiHeader.biBitCount      = 32;
        bi.bmiHeader.biCompression   = BI_RGB;

        HBITMAP hbmp = ::CreateDIBSection(0, &bi, DIB_RGB_COLORS, &m_pixels, nullptr, 0);

        ::SelectObject(m_hDC, hbmp);

        if (m_hBitmap)
            ::DeleteObject(m_hBitmap);

        m_hBitmap = hbmp;
    }

private:
    HWND m_hView;
    HBITMAP m_hBitmap;
    HDC m_hDC;
    unsigned int m_width;
    unsigned int m_height;
    void* m_pixels;
};

#endif