#include "baserender.h"
#include "flat.h"

cFlatBaseRender::cFlatBaseRender(void) {
    font = cFont::CreateFont(Setup.FontOsd, Setup.FontOsdSize );
    fontSml = cFont::CreateFont(Setup.FontSml, Setup.FontSmlSize);
    fontFixed = cFont::CreateFont(Setup.FontFix, Setup.FontFixSize);

    fontHeight = font->Height();
    fontSmlHeight = fontSml->Height();

    topBarTitle = "";
    topBarLastDate = "";
    topBarUpdateTitle = false;
    topBarHeight = 0;

    marginOsd = 5;
    marginItem = 10;

    buttonsHeight = 0;
    
    topBarPixmap = NULL;
    buttonsPixmap = NULL;
    messagePixmap = NULL;
    contentPixmap = NULL;
}

cFlatBaseRender::~cFlatBaseRender(void) {
    delete font;
    delete fontSml;
    delete fontFixed;

    if( osd )
    {
        if( topBarPixmap )
            osd->DestroyPixmap(topBarPixmap);
        if( buttonsPixmap )
            osd->DestroyPixmap(buttonsPixmap);
        if( messagePixmap )
            osd->DestroyPixmap(messagePixmap);
        if( contentPixmap )
            osd->DestroyPixmap(contentPixmap);

        delete osd;
    }
}

void cFlatBaseRender::CreateFullOsd(void) {
    CreateOsd(cOsd::OsdLeft() + marginOsd, cOsd::OsdTop() + marginOsd, cOsd::OsdWidth() - marginOsd*2, cOsd::OsdHeight() - marginOsd*2);
}

void cFlatBaseRender::CreateOsd(int left, int top, int width, int height) {
    osdWidth = width;
    osdHeight = height;

    osd = cOsdProvider::NewOsd(left, top);
    if (osd) {
        tArea Area = { 0, 0, width, height,  32 };
        if (osd->SetAreas(&Area, 1) == oeOk) {  
            return;
        }
    }
    return;
}

void cFlatBaseRender::TopBarCreate(void) {
    topBarHeight = fontHeight;

    topBarPixmap = osd->CreatePixmap(1, cRect(0, 0, osdWidth, topBarHeight));
    topBarPixmap->Fill(clrTransparent);
}

void cFlatBaseRender::TopBarSetTitle(cString title) {
    topBarTitle = title;
    topBarUpdateTitle = true;
}

// sollte bei jedum "Flush" aufgerufen werden!
void cFlatBaseRender::TopBarUpdate(void) {
    cString curDate = DayDateTime();
    if ( strcmp(curDate, topBarLastDate) || topBarUpdateTitle ) {
        topBarUpdateTitle = false;
        topBarLastDate = curDate;

        topBarPixmap->Fill(Theme.Color(clrTopBarBg));
        topBarPixmap->DrawText(cPoint(marginItem, 0), topBarTitle, Theme.Color(clrTopBarFont), clrTransparent, font);
        int width = font->Width(*curDate);
        topBarPixmap->DrawText(cPoint(osdWidth - width, 0), curDate, Theme.Color(clrTopBarDateTimeFont), clrTransparent, font);
    }
}

void cFlatBaseRender::ButtonsCreate(void) {
    buttonsHeight = fontHeight + 30;
    buttonsWidth = osdWidth;

    buttonsPixmap = osd->CreatePixmap(1, cRect(0, osdHeight - buttonsHeight, buttonsWidth, buttonsHeight));
    buttonsPixmap->Fill(clrTransparent);
}

void cFlatBaseRender::ButtonsSet(const char *Red, const char *Green, const char *Yellow, const char *Blue) {
    int buttonWidth = (buttonsWidth / 4) - marginItem;

    buttonsPixmap->Fill(clrTransparent);

    int x = 0;
    buttonsPixmap->DrawText(cPoint(x, 0), Red, Theme.Color(clrButtonFont), Theme.Color(clrButtonBg), font, buttonWidth, 0, taCenter);
    buttonsPixmap->DrawRectangle(cRect(x, fontHeight + 12, buttonWidth, 4), Theme.Color(clrButtonRed));

    x += buttonWidth + marginItem;
    buttonsPixmap->DrawText(cPoint(x, 0), Green, Theme.Color(clrButtonFont), Theme.Color(clrButtonBg), font, buttonWidth, 0, taCenter);
    buttonsPixmap->DrawRectangle(cRect(x, fontHeight + 12, buttonWidth, 4), Theme.Color(clrButtonGreen));

    x += buttonWidth + marginItem;
    buttonsPixmap->DrawText(cPoint(x, 0), Yellow, Theme.Color(clrButtonFont), Theme.Color(clrButtonBg), font, buttonWidth, 0, taCenter);
    buttonsPixmap->DrawRectangle(cRect(x, fontHeight + 12, buttonWidth, 4), Theme.Color(clrButtonYellow));

    x += buttonWidth + marginItem;
    buttonsPixmap->DrawText(cPoint(x, 0), Blue, Theme.Color(clrButtonFont), Theme.Color(clrButtonBg), font, buttonWidth, 0, taCenter);
    buttonsPixmap->DrawRectangle(cRect(x, fontHeight + 12, buttonWidth, 4), Theme.Color(clrButtonBlue));
}

void cFlatBaseRender::MessageCreate(void) {
    messageHeight = fontHeight;
    messagePixmap = osd->CreatePixmap(2, cRect(0, osdHeight - messageHeight - 50, osdWidth, messageHeight));
    messagePixmap->Fill(clrTransparent);
}

void cFlatBaseRender::MessageSet(eMessageType Type, const char *Text) {
    tColor col = Theme.Color(clrMessageStatus);
    switch (Type) {
        case mtStatus: 
            col = Theme.Color(clrMessageStatus);
            break;
        case mtInfo:
            col = Theme.Color(clrMessageInfo);
            break;
        case mtWarning:
            col = Theme.Color(clrMessageWarning);
            break;
        case mtError:
            col = Theme.Color(clrMessageError);
            break;
    }
    messagePixmap->Fill(Theme.Color(clrMessageBg));

    messagePixmap->DrawRectangle(cRect( 0, 0, 50, messageHeight), col);
    messagePixmap->DrawRectangle(cRect( osdWidth - 50, 0, 50, messageHeight), col);

    int textWidth = font->Width(Text);
    messagePixmap->DrawText(cPoint((osdWidth - textWidth) / 2, 0), Text, Theme.Color(clrMessageFont), Theme.Color(clrMessageBg), font);
}

void cFlatBaseRender::MessageClear(void) {
    messagePixmap->Fill(clrTransparent);
}

void cFlatBaseRender::ContentCreate(int Left, int Top, int Width, int Height) {
    contentHasScrollbar = false;
    contentShown = false;

    contentLeft = Left;
    contentTop = Top;
    contentWidth = Width;
    contentHeight = Height;
    int lines = ContentVisibleLines();
    contentHeight = lines * fontHeight;
}

void cFlatBaseRender::ContentSet(const char *Text, tColor ColorFg, tColor ColorBg) {
    contentWrapper.Set(Text, font, contentWidth);
    contentColorFg = ColorFg;
    contentColorBg = ColorBg;

    int contentWrapperHeight = (contentWrapper.Lines()+1) * fontHeight;

    if( contentWrapperHeight > contentHeight ) {
        contentDrawPortHeight = contentWrapperHeight;
        contentHasScrollbar = true;
    } else {
        contentDrawPortHeight = contentHeight;
        contentHasScrollbar = false;
    }

    contentPixmap = osd->CreatePixmap(2, cRect(contentLeft, contentTop, contentWidth, contentHeight),
            cRect(0, 0, contentWidth, contentDrawPortHeight));

    contentPixmap->Fill(contentColorBg);

    contentDraw();
    contentShown = true;
}

bool cFlatBaseRender::ContentScrollable(void) {
    return contentHasScrollbar;
}

double cFlatBaseRender::ScrollbarSize(void) {
    return (double)contentHeight / (double)contentDrawPortHeight;
}

int cFlatBaseRender::ContentScrollTotal(void) {
    return contentWrapper.Lines();
}

int cFlatBaseRender::ContentGetHeight(void) {
    return contentHeight;
}

int cFlatBaseRender::ContentScrollOffset(void) {
    double offset;
    if ( ((-1)*contentPixmap->DrawPort().Point().Y() + contentHeight + fontHeight) > contentDrawPortHeight)
        offset = (double)1 - ScrollbarSize();
    else
        offset = (double)((-1)*contentPixmap->DrawPort().Point().Y())/(double)((-1)*contentPixmap->DrawPort().Point().Y() + contentHeight);
    return ContentScrollTotal() * offset;
}

int cFlatBaseRender::ContentVisibleLines(void) {
    return (contentHeight / fontHeight);
}

bool cFlatBaseRender::ContentScroll(bool Up, bool Page) {
    int aktHeight = contentPixmap->DrawPort().Point().Y();
    int totalHeight = contentPixmap->DrawPort().Height();
    int screenHeight = contentPixmap->ViewPort().Height();
    int lineHeight = fontHeight;
    bool scrolled = false;
    if (Up) {
        if (Page) {
            int newY = aktHeight + screenHeight;
            if (newY > 0)
                newY = 0;
            contentPixmap->SetDrawPortPoint(cPoint(0, newY));
            scrolled = true;
        } else {
            if (aktHeight < 0) {
                contentPixmap->SetDrawPortPoint(cPoint(0, aktHeight + lineHeight));
                scrolled = true;
            }
        }
    } else {
        if (Page) {
            int newY = aktHeight - screenHeight;
            if ((-1)*newY > totalHeight - screenHeight)
                newY = (-1)*(totalHeight - screenHeight);
            contentPixmap->SetDrawPortPoint(cPoint(0, newY));
            scrolled = true;
        } else {
            if (totalHeight - ((-1)*aktHeight + lineHeight) > screenHeight) {
                contentPixmap->SetDrawPortPoint(cPoint(0, aktHeight - lineHeight));
                scrolled = true;
            }
        }
    }
    return scrolled;
}

bool cFlatBaseRender::ContentIsShown(void) {
    return contentShown;
}

void cFlatBaseRender::ContentClear(void) {
    if( contentPixmap )
        contentPixmap->Fill(clrTransparent);
    contentShown = false;
}

void cFlatBaseRender::contentDraw(void) {
    int linesText = contentWrapper.Lines();
    int currentHeight = 0;
    for (int i=0; i < linesText; i++) {
        //printf("drawLine: %d %s\n", i, contentWrapper.GetLine(i));
        currentHeight = (i)*fontHeight;
        contentPixmap->DrawText(cPoint(0, currentHeight), contentWrapper.GetLine(i), contentColorFg, contentColorBg, font, contentWidth);
    }
}
