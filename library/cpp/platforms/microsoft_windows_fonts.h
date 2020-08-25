#include "../sys.h"
#include "microsoft_windows.h"
#include <tchar.h>

MAKE_HASHABLE(sys::font, t.face, t.size, t.bold, t.italic);

extern HWND Hwnd;

struct GDI_FONT
{
    HFONT handle;
    sys::font::metrics metrics;

    GDI_FONT(sys::font font)
    {
        HDC hdc = GetDC(Hwnd); // EnumDisplayMonitors(hdc, &rc, MyPaintEnumProc, 0);
        HDC dc = ::CreateCompatibleDC (hdc);//(NULL);
        ReleaseDC (Hwnd, hdc);

        if (font.face == "") font.face = "Segoe UI";
        if (font.size ==  0) font.size = gui::metrics::text::height;

        LOGFONT lf;
        _tcscpy_s (
        lf.lfFaceName       , 32, str(font.face.upto(31)).c_str());
        lf.lfHeight         = font.size >= 0 ? -font.size : MulDiv (font.size, ::GetDeviceCaps (dc,LOGPIXELSY), 72);
        lf.lfWidth          = 0;
        lf.lfEscapement     = 0;
        lf.lfOrientation    = 0;
        lf.lfWeight         = font.bold ? FW_BOLD : FW_NORMAL;
        lf.lfItalic         = font.italic ? TRUE : FALSE;
        lf.lfStrikeOut      = FALSE;
        lf.lfUnderline      = FALSE;
        lf.lfCharSet        = DEFAULT_CHARSET;
        lf.lfOutPrecision   = 0;//OUT_TT_PRECIS;
        lf.lfClipPrecision  = 0;//CLIP_DEFAULT_PRECIS;
        lf.lfQuality        = DEFAULT_QUALITY;//CLEARTYPE_QUALITY;
        lf.lfPitchAndFamily = 0;//DEFAULT_PITCH | FF_DONTCARE;

        handle = ::CreateFontIndirect (&lf);
        
        HGDIOBJ old = ::SelectObject (dc, handle);

        TEXTMETRIC tm;  ::GetTextMetrics (dc, &tm);

        metrics.height   = tm.tmHeight;  // ascent + descent
        metrics.ascent   = tm.tmAscent;  // units above the base line
        metrics.descent  = tm.tmDescent; // units below the base line (positive value)
        metrics.linegap  = tm.tmExternalLeading; // baseline-to-baseline distance = ascent + descent + linegap
        metrics.average_char_width = tm.tmAveCharWidth;
        metrics.maximum_char_width = tm.tmMaxCharWidth;
        metrics.minimum_char_width = 0;

        ::SelectObject (dc, old);

        ::DeleteDC (dc);
    }
    ~GDI_FONT() {} // { if (handle) ::DeleteObject (handle); }
};

static GDI_FONT cache (sys::font font) {
    static std::unordered_map<sys::font, GDI_FONT> fonts;
    auto it = fonts.find(font); if (it == fonts.end())
    it = fonts.emplace (font, GDI_FONT(font)).first;
    return it->second;
}

sys::font::metrics sys::metrics (sys::font font)
{
    return cache(font).metrics;
}

struct GDI_CONTEXT
{
    HDC dc;
    HGDIOBJ old;
    GDI_FONT font;

    GDI_CONTEXT(sys::font f) : font (cache(f))
    {
        HDC hdc = GetDC(Hwnd); // EnumDisplayMonitors(hdc, &rc, MyPaintEnumProc, 0);
        dc = ::CreateCompatibleDC (hdc);//(NULL);
        old = ::SelectObject (dc, font.handle);
        ReleaseDC (Hwnd, hdc);
    }
   ~GDI_CONTEXT()
    {
        ::SelectObject (dc, old);
        ::DeleteDC     (dc);
    }
};

///////////////////////////////////////////////////////////////////////////////

struct cache_metrics_key
{
    aux::str text; sys::font font; 

    bool operator == (const cache_metrics_key & k) const { return
        text == k.text &&
        font == k.font;
    }
};

MAKE_HASHABLE(cache_metrics_key, t.text, t.font);

static std::unordered_map<cache_metrics_key, sys::glyph_metrics> cache_metrics;

static array<std::unordered_map<str, sys::glyph_metrics>> cache_metrics2;

sys::glyph::glyph (str text, sys::glyph_style_index style_index) : text(text), style_index(style_index)
{
    if (text == "") return;
    const auto & style = this->style();
    cache_metrics_key key {text, style.font};
    auto it = cache_metrics.find(key);
    if (it != cache_metrics.end()) glyph_metrics::operator = (it->second); else
    {
        GDI_CONTEXT context(style.font);

        SIZE Size;
        auto ss = winstr(text);
        auto rc = ::GetTextExtentPoint32W (context.dc, ss.c_str(), (int) ss.size(), &Size);
        if (!rc) throw std::runtime_error("sys::font::render : GetTextExtentPoint32W fail");

        advance = Size.cx;
        ascent  = context.font.metrics.ascent;
        descent = context.font.metrics.descent;
        width   = advance*2; // italics are wider

        static pix::image<RGBA> image;
        image.resize(XY(width, ascent+descent));
        image.fill(pix::white);

        sys::glyph_style simple_style;
        simple_style.font = style.font;
        simple_style.color = pix::black;
        simple_style.background = pix::white;

        sys::glyph simple_glyph = *this;
        simple_glyph.style_index = sys::glyph_style_index(simple_style);
        simple_glyph.render(image);

        for (bool stop = false; !stop && width > 0; width--)
            for (int y = 0; y < image.size.y; y++)
                if (image(width-1, y) != simple_style.background)
                    { stop = true; break; }

        if (width < advance) // could be 0 for spaces
            width = advance; // for continuity of strike/under-lines

        advance -= width; // the pen position increment = width + advance

        if (text == "\n") width = max(1, (ascent+descent)/16); // for text editor

        cache_metrics.emplace (key, *this);
    }

    // adapt metrics for style.outline/undeline/shadow here
}

///////////////////////////////////////////////////////////////////////////////

struct cache_glyphs_key
{
    aux::str  text;
    sys::font font;
    pix::RGBA fore, back;

    bool operator == (const cache_glyphs_key & k) const { return
        text == k.text &&
        font == k.font &&
        fore == k.fore &&
        back == k.back;
    }
};

MAKE_HASHABLE(cache_glyphs_key, t.text, t.font, t.fore, t.back);

void sys::glyph::render (pix::frame<RGBA> frame, XY offset, uint8_t alpha, int x)
{
    const auto & style = this->style();
    if (alpha == 0) return;
    if (text == "") return;
    if (text.contains_only(str::one_of(" \t\r\n"))
    &&  style.underline.color.a == 0
    &&  style.strikeout.color.a == 0
    &&  style.background     .a == 0) return;

    RGBA fore = style.color;
    RGBA back = style.background;

    int w = width;
    int h = ascent + descent;
    if (w <= 0 or h <= 0) return;

    if (false) { // test for rendering speed
        frame.blend(RGBA::random(), alpha);
        return;
    }

    bool solid_color_background = back.a == 255;
    if (!solid_color_background)
    {
        bool ok = true;
        RGBA c = frame(0,0);
        for (int y=0; y<frame.size.y; y++)
        for (int x=0; x<frame.size.x; x++) if (frame(x,y) != c) ok = false;
        if (ok) { solid_color_background = true; c.blend(back, alpha); back = c; }
    }

    static std::unordered_map<cache_glyphs_key, pix::image<RGBA>> cache;

    sys::glyph_style cacheable_style;
    cacheable_style.font = style.font;
    cacheable_style.color = style.color;
    cacheable_style.background = style.background;

    bool cacheable =
        text.size() <= 4 &&
        solid_color_background &&
        style == cacheable_style;

    if (cacheable)
    {
        auto it = cache.find(cache_glyphs_key{text, style.font, fore, back});
        if (it != cache.end())
        {
            frame.blend_from(it->second.crop()
                .from(offset.x, offset.y), alpha);
            return;
        }
    }

    GDI_CONTEXT context(style.font);

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

    auto ss = winstr(text);

    if (!solid_color_background)
    ::SetBkMode    (context.dc, TRANSPARENT); else
    ::SetBkColor   (context.dc, RGB(back.r, back.g, back.b));
    ::SetTextColor (context.dc, RGB(fore.r, fore.g, fore.b));

    pix::view<RGBA> view ((RGBA*)bits, XY(w,h), w);

    if (!solid_color_background) frame.copy_to(view.from(offset.x, offset.y));
    if (!solid_color_background) view.from(offset.x, offset.y).blend(back, alpha);

    for (int y=0; y<h; y++)
    for (int x=0; x<w; x++) view(x,y).a = 255;

    ::TextOutW (context.dc, 0, 0, ss.c_str(), (int) ss.size());
    ::GdiFlush ();

    for (int y=0; y<h; y++)
    for (int x=0; x<w; x++) view(x,y).a = min (fore.a, 255 - view(x,y).a);

    // https://github.com/wentin/underlineJS
    // default underline thickness = 1/6 of width of the period mark
    // optimal Y position is the goden ratio point between the baseline and the descender line

    frame.blend(back, alpha);
    frame.blend_from(view.from(offset.x, offset.y), alpha);
    
    if (cacheable)
    {
        pix::image<RGBA> image (XY(w,h));
        image.crop().copy_from(view);
        cache.emplace(cache_glyphs_key{text, style.font, fore, back}, image);
    }

    ::SelectObject (context.dc, old);
    ::DeleteObject (bmp);
}

