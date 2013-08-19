#include "displaymenu.h"

cFlatDisplayMenu::cFlatDisplayMenu(void) {
    CreateFullOsd();
    TopBarCreate();
    ButtonsCreate();
    MessageCreate();

    itemHeight = fontHeight + 2;

    scrollBarWidth = 20;
    scrollBarHeight = osdHeight - (topBarHeight + buttonsHeight + marginItem*3 );
    scrollBarTop = topBarHeight + marginItem;

    menuWidth = osdWidth - scrollBarWidth;

    menuPixmap = osd->CreatePixmap(1, cRect(0, topBarHeight + marginItem, menuWidth, scrollBarHeight ));
    
    contentTop = topBarHeight + marginItem + fontHeight + (fontSmlHeight*2) + marginItem*3;
    ContentCreate(0, contentTop, menuWidth, scrollBarHeight - fontHeight - fontSmlHeight*2 - marginItem);

    scrollbarPixmap = osd->CreatePixmap(2, cRect(osdWidth - scrollBarWidth, scrollBarTop, scrollBarWidth, scrollBarHeight));

    menuPixmap->Fill(clrTransparent);
    scrollbarPixmap->Fill(clrTransparent);
    
    menuCategory = mcUndefined;
}

cFlatDisplayMenu::~cFlatDisplayMenu() {
    osd->DestroyPixmap(menuPixmap);
    osd->DestroyPixmap(scrollbarPixmap);
}

void cFlatDisplayMenu::SetMenuCategory(eMenuCategory MenuCategory) {
    menuCategory = MenuCategory;
}

void cFlatDisplayMenu::DrawScrollbar(int Total, int Offset, int Shown, int Top, int Height, bool CanScrollUp, bool CanScrollDown) {
    if (Total > 0 && Total > Shown) {
        int scrollHeight = max(int((Height) * double(Shown) / Total + 0.5), 5);
        int scrollTop = min(int(Top + (Height) * double(Offset) / Total + 0.5), Top + Height - scrollHeight);

        scrollbarPixmap->Fill(clrTransparent);
        scrollbarPixmap->DrawRectangle(cRect(5, Top, scrollBarWidth-5, Height), Theme.Color(clrScrollbarBg));

        scrollbarPixmap->DrawRectangle(cRect(10, Top, 2, Height), Theme.Color(clrScrollbarFg));
        scrollbarPixmap->DrawRectangle(cRect(10, scrollTop, 8, scrollHeight), Theme.Color(clrScrollbarBarFg));
    }
}

void cFlatDisplayMenu::SetScrollbar(int Total, int Offset) {
    DrawScrollbar(Total, Offset, MaxItems(), 0, ItemsHeight(), Offset > 0, Offset + MaxItems() < Total);
}

void cFlatDisplayMenu::Scroll(bool Up, bool Page) {
    // Wird das Men� gescrollt oder Content?
    if( ContentIsShown() )
    {
        bool scrolled = ContentScroll(Up, Page);
        if( scrolled )
        {
            DrawScrollbar(ContentScrollTotal(), ContentScrollOffset(), ContentVisibleLines(), contentTop - scrollBarTop, ContentGetHeight(), ContentScrollOffset() > 0, ContentScrollOffset() + ContentVisibleLines() < ContentScrollTotal());
        }
    } else {
        cSkinDisplayMenu::Scroll(Up, Page);
    }
}

int cFlatDisplayMenu::MaxItems(void) {
    return scrollBarHeight / itemHeight;
}

int cFlatDisplayMenu::ItemsHeight(void) {
    return MaxItems() * itemHeight;
}
void cFlatDisplayMenu::Clear(void) {
    textScroller.Reset();
    menuPixmap->Fill(clrTransparent);
    scrollbarPixmap->Fill(clrTransparent);
    ContentClear();
}

void cFlatDisplayMenu::SetTitle(const char *Title) {
    if( menuCategory == mcRecording || menuCategory == mcTimer ) {
        cVideoDiskUsage::HasChanged(VideoDiskUsageState);
        int DiskUsage = cVideoDiskUsage::UsedPercent();
        int FreeGB = cVideoDiskUsage::FreeMB() / 1024;
        cString extra1 = cString::sprintf("%s: %d%%", tr("disk usage"), DiskUsage);
        cString extra2 = cString::sprintf("%s: %d GB", tr("free space"), FreeGB);

        TopBarSetTitle(Title);
        TopBarSetTitleExtra(extra1, extra2);
    } else
        TopBarSetTitle(Title);
}

void cFlatDisplayMenu::SetButtons(const char *Red, const char *Green, const char *Yellow, const char *Blue) {
    ButtonsSet(Red, Green, Yellow, Blue);
}

void cFlatDisplayMenu::SetMessage(eMessageType Type, const char *Text) {
    if (Text)
        MessageSet(Type, Text);
    else
        MessageClear();
}

void cFlatDisplayMenu::SetItem(const char *Text, int Index, bool Current, bool Selectable) {
    int y = Index * itemHeight;
    tColor ColorFg, ColorBg;
    if (Current) {
        ColorFg = Theme.Color(clrItemCurrentFont);
        ColorBg = Theme.Color(clrItemCurrentBg);
    }
    else {
        if( Selectable ) {
            ColorFg = Theme.Color(clrItemSelableFont);
            ColorBg = Theme.Color(clrItemSelableBg);
        } else {
            ColorFg = Theme.Color(clrItemFont);
            ColorBg = Theme.Color(clrItemBg);
        }
    }

    for (int i = 0; i < MaxTabs; i++)
    {
        const char *s = GetTabbedText(Text, i);
        if (s) {
            int xt = Tab(i);
            menuPixmap->DrawText(cPoint(xt, y), s, ColorFg, ColorBg, font, menuWidth);
        }
        if (!Tab(i + 1))
            break;
    }
}

void cFlatDisplayMenu::SetEvent(const cEvent *Event) {
    if( !Event )
        return;

    menuPixmap->Fill(clrTransparent);
    menuPixmap->DrawRectangle(cRect(0, 0, menuWidth, fontHeight + fontSmlHeight*2 + marginItem*2), Theme.Color(clrScrollbarBg));

    cString date = Event->GetDateString();
    cString startTime = Event->GetTimeString();
    cString endTime = Event->GetEndTimeString();

    cString timeString = cString::sprintf("%s %s - %s", *date, *startTime, *endTime);

    cString title = Event->Title();
    cString shortText = Event->ShortText();

    menuPixmap->DrawText(cPoint(marginItem, marginItem), timeString, Theme.Color(clrMenuEventFontInfo), Theme.Color(clrMenuEventBg), fontSml, menuWidth - marginItem*2);
    menuPixmap->DrawText(cPoint(marginItem, marginItem + fontSmlHeight), title, Theme.Color(clrMenuEventFontTitle), Theme.Color(clrMenuEventBg), font, menuWidth - marginItem*2);
    menuPixmap->DrawText(cPoint(marginItem, marginItem + fontSmlHeight + fontHeight), shortText, Theme.Color(clrMenuEventFontInfo), Theme.Color(clrMenuEventBg), fontSml, menuWidth - marginItem*2);
    
    ContentSet( Event->Description(), Theme.Color(clrMenuEventFontInfo), Theme.Color(clrMenuEventBg) );
    if( ContentScrollable() )
        DrawScrollbar(ContentScrollTotal(), ContentScrollOffset(), ContentVisibleLines(), contentTop - scrollBarTop, ContentGetHeight(), ContentScrollOffset() > 0, ContentScrollOffset() + ContentVisibleLines() < ContentScrollTotal());
}

void cFlatDisplayMenu::SetRecording(const cRecording *Recording) {
    if( !Recording )
        return;
    menuPixmap->Fill(clrTransparent);
    menuPixmap->DrawRectangle(cRect(0, 0, menuWidth, fontHeight + fontSmlHeight*2 + marginItem*2), Theme.Color(clrScrollbarBg));

    const cRecordingInfo *recInfo = Recording->Info();
    cString timeString = cString::sprintf("%s  %s  %s", *DateString(Recording->Start()), *TimeString(Recording->Start()), recInfo->ChannelName() ? recInfo->ChannelName() : "");

    cString title = recInfo->Title();
    if( isempty(title) )
        title = Recording->Name();
    cString shortText = recInfo->ShortText();

    menuPixmap->DrawText(cPoint(marginItem, marginItem), timeString, Theme.Color(clrMenuRecFontInfo), Theme.Color(clrMenuRecBg), fontSml, menuWidth - marginItem*2);
    menuPixmap->DrawText(cPoint(marginItem, marginItem + fontSmlHeight), title, Theme.Color(clrMenuRecFontTitle), Theme.Color(clrMenuRecBg), font, menuWidth - marginItem*2);
    menuPixmap->DrawText(cPoint(marginItem, marginItem + fontSmlHeight + fontHeight), shortText, Theme.Color(clrMenuRecFontInfo), Theme.Color(clrMenuRecBg), fontSml, menuWidth - marginItem*2);

    ContentSet( recInfo->Description(), Theme.Color(clrMenuRecFontInfo), Theme.Color(clrMenuRecBg) );
    if( ContentScrollable() )
        DrawScrollbar(ContentScrollTotal(), ContentScrollOffset(), ContentVisibleLines(), contentTop - scrollBarTop, ContentGetHeight(), ContentScrollOffset() > 0, ContentScrollOffset() + ContentVisibleLines() < ContentScrollTotal());
}

void cFlatDisplayMenu::SetText(const char *Text, bool FixedFont) {
    if( !Text )
        return;
    menuPixmap->Fill(clrTransparent);

    ContentSet( Text, Theme.Color(clrMenuTextFont), Theme.Color(clrMenuTextBg) );
    if( ContentScrollable() )
        SetScrollbar( ContentScrollTotal(), 0 );
}

int cFlatDisplayMenu::GetTextAreaWidth(void) const {
    return menuWidth - (marginItem*2);
}

const cFont *cFlatDisplayMenu::GetTextAreaFont(bool FixedFont) const {
    const cFont *rfont = FixedFont ? fontFixed : font;
    return rfont;
}

void cFlatDisplayMenu::Flush(void) {
    TopBarUpdate();
    osd->Flush();
}
