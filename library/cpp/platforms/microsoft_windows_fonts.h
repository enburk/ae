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

        LOGFONT lf;
        _tcscpy_s (
        lf.lfFaceName       , 32, aux::str(font.face.upto(31)).c_str());
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

struct cache_metrics_key
{
    aux::str text; sys::font font; 

    bool operator == (const cache_metrics_key & k) const { return
        text == k.text &&
        font == k.font;
    }
};

MAKE_HASHABLE(cache_metrics_key, t.text, t.font);

struct cache_metrics_data { int ascent, descent, width, advance; };
static cache_metrics_data cache_metrics (const sys::glyph & glyph)
{
    static std::unordered_map<cache_metrics_key, cache_metrics_data> cache;
    cache_metrics_key key {glyph.text, glyph.style().font};
    auto it = cache.find(key);
    if (it == cache.end())
    {
        using namespace sys;
        cache_metrics_data data;
        static pix::image<RGBA> image;
        GDI_CONTEXT context(glyph.style().font);

        auto ss = winstr(glyph.text); size_t len = winstrlen(ss);

        SIZE size; auto rc = ::GetTextExtentPoint32W (context.dc, ss.c_str(), (int)len, &size);
        if (!rc) throw std::runtime_error ( "sys::font::render : GetTextExtentPoint32W fail" );

        image.resize(XY(size.cx*4/2, size.cy));
        image.fill(RGBA(0,0,0));

        data.width = image.size.x;
        data.advance = size.cx;
        data.ascent = context.font.metrics.ascent;
        data.descent = context.font.metrics.descent;

        sys::glyph_style style;
        style.font = key.font;
        style.color = pix::black;
        style.background = pix::white;

        sys::glyph g;
        g.text = glyph.text; 
        g.ascent  = data.ascent;
        g.descent = data.descent;
        g.advance = data.advance;
        g.size.x  = data.width;
        g.size.y  = data.ascent + data.descent;
        g.style_index = sys::style_index(style);
        sys::render (g, image);

        for (bool stop = false; !stop && data.width > 0; data.width--)
            for (int y = 0; y < image.size.y; y++)
                if (image(data.width-1, y) != style.background)
                    { stop = true; break; }

        if (data.width < data.advance) // could be 0 for spaces
            data.width = data.advance; // for continuity of strike/under-lines
        data.advance -= data.width; // the pen position increment = size.x + advance

        it = cache.emplace (key, data).first;
    }
    return it->second;
}

sys::glyph::glyph (str text, sys::glyph_style style) : text(text), style_index(sys::style_index(style))
{
    if (text == "") return;
    auto data = cache_metrics(*this);
    ascent  = data.ascent;
    descent = data.descent;
    advance = data.advance;
    size.x  = data.width;
    size.y  = ascent + descent;
}

sys::token::token (str text, sys::glyph_style style)
{
    this->text = text;
    this->style_index = sys::style_index(style);

    str t; text += "\n";
    for (char c : text)
    {
        if (t.size () == 0
        ||  t.size () == 1 && (static_cast<uint8_t>(t[0]) & 0b11000000) == 0b11000000 // UTF-8: 110xxxxx 10xxxxxx
        ||  t.size () == 2 && (static_cast<uint8_t>(t[0]) & 0b11100000) == 0b11100000 // UTF-8: 1110xxxx 10xxxxxx 10xxxxxx
        ||  t.size () == 3 && (static_cast<uint8_t>(t[0]) & 0b11110000) == 0b11110000 // UTF-8: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        )   t += c; else { glyphs += glyph(t, style); t = c; }
    }
    ascent  = 0;
    descent = 0;
    advance = 0;
    offsets.reserve(glyphs.size());

    for (auto & glyph : glyphs) {
        ascent  = max(ascent,  glyph.ascent);
        descent = max(descent, glyph.descent);
    }
    for (auto & glyph : glyphs) {
        offsets += XY(size.x + advance, ascent - glyph.ascent);
        size.x = offsets.back().x + glyph.size.x;
        advance = glyph.advance;
    }
    size.y = ascent + descent;
}

struct cache_glyph_key
{
    aux::str text; sys::font font; sys::RGBA fore, back;

    bool operator == (const cache_glyph_key & k) const { return
        text == k.text &&
        font == k.font &&
        fore == k.fore &&
        back == k.back;
    }
};

MAKE_HASHABLE(cache_glyph_key, t.text, t.font, t.fore, t.back);

void sys::render (glyph glyph, pix::frame<RGBA> frame, XY offset, uint8_t alpha, int x)
{
    auto style = glyph.style();

    if (alpha == 0) return;
    if (glyph.text == "") return;
    if (glyph.text.contains_only(str::one_of(" \t\r\n"))
    &&  style.underline.color.a == 0
    &&  style.strikeout.color.a == 0
    &&  style.background     .a == 0) return;

    int w = glyph.size.x; RGBA fore = style.color;
    int h = glyph.size.y; RGBA back = style.background;

    if (frame.size.x <= 0 || offset.x >= w) return;
    if (frame.size.y <= 0 || offset.y >= h) return;

    frame = frame.crop(XYWH(-offset.x,-offset.y, w, h));

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

    static std::unordered_map<cache_glyph_key, pix::image<RGBA>> cache;

    sys::glyph_style cacheable_style;
    cacheable_style.font = style.font;
    cacheable_style.color = style.color;
    cacheable_style.background = style.background;

    bool cacheable =
        solid_color_background &&
        glyph.text.size() <= 4 &&
        style == cacheable_style;

    if (cacheable)
    {
        auto it = cache.find(cache_glyph_key{glyph.text, style.font, fore, back});
        if (it != cache.end())
        {
            frame.blend_from(it->second, alpha);
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

    auto ss = winstr(glyph.text); size_t len = winstrlen(ss);

    if (!solid_color_background)
    ::SetBkMode    (context.dc, TRANSPARENT); else
    ::SetBkColor   (context.dc, RGB(back.r, back.g, back.b));
    ::SetTextColor (context.dc, RGB(fore.r, fore.g, fore.b));

    pix::view<RGBA> view ((RGBA*)bits, XY(w,h), w);

    if (!solid_color_background) frame.copy_to(view);
    if (!solid_color_background) view.blend(back, alpha);

    for (int y=0; y<h; y++)
    for (int x=0; x<w; x++) view(x,y).a = 255;

    ::TextOutW (context.dc, 0, 0, ss.c_str(), (int)len);
    ::GdiFlush ();

    for (int y=0; y<h; y++)
    for (int x=0; x<w; x++) view(x,y).a = min (fore.a, 255 - view(x,y).a);

    // https://github.com/wentin/underlineJS
    // default underline thickness = 1/6 of width of the period mark
    // optimal Y position is the goden ratio point between the baseline and the descender line

    frame.blend(back, alpha);
    frame.blend_from(view, alpha);
    
    if (cacheable)
    {
        pix::image<RGBA> image (XY(w,h));
        image.crop().copy_from(view);
        cache.emplace(cache_glyph_key{glyph.text, style.font, fore, back}, image);
    }

    ::SelectObject (context.dc, old);
    ::DeleteObject (bmp);
}

