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

        ::SelectObject (dc, old);

        ::DeleteDC (dc);
    }
   ~GDI_FONT() { if (handle) ::DeleteObject (handle); }
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
        dc = ::CreateCompatibleDC (NULL);
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

    auto ss = winstr(text); size_t len = winstrlen (ss);

    SIZE size;  ::GetTextExtentPoint32W (context.dc, ss.c_str(), (int)len, &size);

    int extral = 2; int extrar = 2;

    glyph.frame.size.x = size.cx + extral + extrar;
    glyph.frame.size.y = size.cy;
    glyph.xbearing = - extral;
    glyph.ybearing = context.font.metrics.ascent;
    glyph.advance = size.cx+1;

    return glyph;
}

void sys::font::render (GLYPH<XRGB> & glyph)
{
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

    auto ss = winstr(text); size_t len = winstrlen (ss);

    ::SetBkColor   ( dc, RGB (cback.r,cback.g,cback.b) );
    ::SetTextColor ( dc, RGB (color.r,color.g,color.b) );

    ::memcpy ( bits, g.bbox.begin (), w*h*4 );

    ::TextOutW ( dc, abc::clampround<int>(-g.bearing_x),0, *ss, (int) len );

    ::memcpy ( g.bbox.begin (), bits, w*h*4 );

    ::SelectObject ( dc, olf );
    ::SelectObject ( dc, old );
    ::DeleteObject ( bmp );
    ::DeleteDC     ( dc );

    for( int y=0; y<g.bbox.size.y; y++ )
    for( int x=0; x<g.bbox.size.x; x++ )
    {
        if( g.bbox (x,y).xrgb != RGB (cback.r,cback.g,cback.b) )  g.bbox (x,y).x = 255;
    }
}

//============================================================================================================================//
struct GlypherGDI : public GLYPHER
//============================================================================================================================//
{
    virtual void render ( GLYPH<XRGB> & g, XRGB color )
    {

        LPVOID   bits = NULL;
        HBITMAP  bmp  = ::CreateDIBSection   ( NULL, (BITMAPINFO*)&bi, DIB_RGB_COLORS, &bits, NULL, NULL );
        if(    ! bmp )
        {
            ::DeleteDC ( dc ); throw std::logic_error ( "GlypherGDI::render CreateDIBSection" );
        }
        HGDIOBJ  old  = ::SelectObject       ( dc, bmp );

        ::memcpy ( bits, g.bbox.begin (), w*h*4 );

        ::SetBkMode    ( dc, TRANSPARENT );
        ::SetTextColor ( dc, RGB (color.r,color.g,color.b) );

        strw ss = sys::winstrw (g.text);  size_t len = sys::winstrlen (ss);

        ::TextOutW ( dc, abc::clampround<int>(-g.bearing_x),0, *ss, (int) len );

        ::memcpy ( g.bbox.begin (), bits, w*h*4 );

        ::SelectObject ( dc, olf );
        ::SelectObject ( dc, old );
        ::DeleteObject ( bmp );
        ::DeleteDC     ( dc );

        { Cache [color][g.text] = g; }
    }

    virtual int kerning ( str8, str8 ){ return 0; }
    virtual int kern111 ( str8, str8 ){ return 0; }
};

#endif

//============================================================================================================================//

FONTPOOL FontPool;

FONTPOOL:: FONTPOOL (){ Default = new Dina; }
FONTPOOL::~FONTPOOL (){ if( Default ) delete Default; for( int n=0; n<Glyphers.size (); n++ ) if( Glyphers [n] ) delete Glyphers [n]; }

GLYPHER* FONTPOOL::find ( FONT font )
{
    if( ! ( font != FONT ( "Dina", 8 ) ) )  return Default;

    FONTMAP::iterator it = FontMap.find ( font );  if( it != FontMap.end () )  return it->second;

#ifdef      _WIN32

    Glyphers += new GlypherGDI (font);

    FontMap [font] = Glyphers.last ();

    return Glyphers.last ();

#endif

    return Default;
}
