#include "sys.h"
#include "sys_windows.h"
#include <tchar.h>

struct GDI_FONT
{
    HFONT handle;
    FONT::METRICS metrics;

    GDI_FONT(FONT font)
    {
        HDC dc = ::CreateCompatibleDC (NULL);

        LOGFONT lf;
        _tcscpy_s (
        lf.lfFaceName       , 32, font.face.till (31).c_str());
        lf.lfHeight         = -MulDiv (font.size, ::GetDeviceCaps (dc,LOGPIXELSY), 72);
        lf.lfWidth          = 0;
        lf.lfEscapement     = 0;
        lf.lfOrientation    = 0;
        lf.lfWeight         = font.bold ? FW_BOLD : FW_NORMAL;
        lf.lfItalic         = font.italic    ? TRUE : FALSE;
        lf.lfStrikeOut      = font.strike    ? TRUE : FALSE;
        lf.lfUnderline      = font.underline ? TRUE : FALSE;
        lf.lfCharSet        = DEFAULT_CHARSET;
        lf.lfOutPrecision   = 0;//OUT_TT_PRECIS;
        lf.lfClipPrecision  = 0;//CLIP_DEFAULT_PRECIS;
        lf.lfQuality        = DEFAULT_QUALITY;//CLEARTYPE_QUALITY;
        lf.lfPitchAndFamily = 0;//DEFAULT_PITCH | FF_DONTCARE;

        handle = ::CreateFontIndirect (&lf);
        
        HGDIOBJ old = ::SelectObject (dc, handle);

        TEXTMETRIC tm;  ::GetTextMetrics (dc, &tm);

        metrics.height   = tm.tmHeight; // ascent + descent
        metrics.ascent   = tm.tmAscent; // units above the base line
        metrics.descent  = tm.tmDescent; // units below the base line (positive value)
        metrics.linegap  = tm.tmExternalLeading; // baseline-to-baseline distance should be computed as: ascent + descent + linegap
        metrics.avecharw = tm.tmAveCharWidth; // average char width (usually width of 'x')
        metrics.maxcharw = tm.tmMaxCharWidth; // maximum char width
        metrics.overhang = tm.tmOverhang; // only necessary for non-TrueType raster fonts

        ::SelectObject (dc, old);

        ::DeleteDC (dc);
    }
    ~GDI_FONT() {} // { if (handle) ::DeleteObject (handle); }
};

static GDI_FONT cache (FONT font) {
    static std::unordered_map<FONT, GDI_FONT> fonts;
    auto it = fonts.find(font); if (it == fonts.end())
    it = fonts.emplace (font, GDI_FONT(font)).first;
    return it->second;
}

struct GDI_CONTEXT
{
    HDC dc;
    HGDIOBJ old;
    GDI_FONT font;

    GDI_CONTEXT(FONT f) : font (cache(f))
    {
        dc  = ::CreateCompatibleDC (NULL);
        old = ::SelectObject (dc, font.handle);
    }
   ~GDI_CONTEXT()
    {
        ::SelectObject (dc, old);
        ::DeleteDC     (dc);
    }
};

FONT::METRICS sys::font::metrics (FONT font)
{
    return cache(font).metrics;
}

GLYPH<XRGB> sys::font::glyph (FONT font, str text)
{
    GLYPH<XRGB> glyph;
    glyph.font = font;
    glyph.text = text; if (glyph.text == "") return glyph;

    GDI_CONTEXT context(font);

    auto ss = winstr(text); size_t len = winstrlen(ss);

    SIZE size;  ::GetTextExtentPoint32W (context.dc, ss.c_str(), (int)len, &size);

    int lmargin = 0; int rmargin = 0;

    glyph.frame.size.x = size.cx + lmargin + rmargin;
    glyph.frame.size.y = size.cy;
    glyph.bearing_x = - lmargin;
    glyph.bearing_y = context.font.metrics.ascent;
    glyph.advance = size.cx;
    glyph.advance -= context.font.metrics.overhang;

    return glyph;
}

void sys::font::render (GLYPH<XRGB> & glyph, bool blend)
{
    if (!(glyph.text.contains(str::one_not_of(" \t\r\n")))) return;
    if (!(glyph.frame.image != nullptr)) throw std::logic_error("sys::font::render : nullptr image");

    GDI_CONTEXT context(glyph.font);

    int w = glyph.frame.size.x;
    int h = glyph.frame.size.y;

    BITMAPINFO bi;
    ZeroMemory(&bi,               sizeof(bi));
    bi.bmiHeader.biSize         = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth        = w;
    bi.bmiHeader.biHeight       = -h;
    bi.bmiHeader.biPlanes       = 1;
    bi.bmiHeader.biBitCount     = 32; 
    bi.bmiHeader.biCompression  = BI_RGB;
    bi.bmiHeader.biSizeImage    = w*h*4;

    LPVOID  bits = NULL;
    HBITMAP bmp = ::CreateDIBSection (NULL, (BITMAPINFO*)&bi, DIB_RGB_COLORS, &bits, NULL, NULL);
    if    (!bmp) throw std::runtime_error ( "sys::font::render : CreateDIBSection fail" );
    HGDIOBJ old = ::SelectObject (context.dc, bmp);

    auto ss = winstr(glyph.text); size_t len = winstrlen(ss);

    if (blend)
    ::SetBkMode    (context.dc, TRANSPARENT); else
    ::SetBkColor   (context.dc, RGB(glyph.back.r, glyph.back.g, glyph.back.b));
    ::SetTextColor (context.dc, RGB(glyph.fore.r, glyph.fore.g, glyph.fore.b));

    if (blend)
    glyph.frame.copy_to((XRGB*)bits, w);

    ::TextOutW (context.dc, (int)std::lround(-glyph.bearing_x), 0, ss.c_str(), (int)len);

    glyph.frame.copy_from((XRGB*)bits, w);

    ::SelectObject (context.dc, old);
    ::DeleteObject (bmp);

    if (!blend)
    for (int y=0; y<glyph.frame.size.y; y++)
    for (int x=0; x<glyph.frame.size.x; x++)
        if (glyph.frame(x,y).xrgb != RGB(glyph.back.r, glyph.back.g, glyph.back.b))
            glyph.frame(x,y).x = 255;
}
