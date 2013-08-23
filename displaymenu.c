#include "displaymenu.h"

cFlatDisplayMenu::cFlatDisplayMenu(void) {
    CreateFullOsd();
    TopBarCreate();
    ButtonsCreate();
    MessageCreate();

    VideoDiskUsageState = -1;

    itemHeight = fontHeight + 2;
    itemChannelHeight = fontHeight + 2;
    
    scrollBarWidth = 20;
    scrollBarHeight = osdHeight - (topBarHeight + buttonsHeight + marginItem*3 );
    scrollBarTop = topBarHeight + marginItem;

    menuWidth = osdWidth - scrollBarWidth;
    menuTop = topBarHeight + marginItem;
    menuPixmap = osd->CreatePixmap(1, cRect(0, menuTop, menuWidth, scrollBarHeight ));
    
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
    // Wird das Menü gescrollt oder Content?
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
    if( menuCategory == mcChannel )
        return scrollBarHeight / itemChannelHeight;

    return scrollBarHeight / itemHeight;
}

int cFlatDisplayMenu::ItemsHeight(void) {
    if( menuCategory == mcChannel )
        return MaxItems() * itemChannelHeight;

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

/*
bool cFlatDisplayMenu::SetItemChannel(const cChannel *Channel, int Index, bool Current, bool Selectable, bool WithProvider) {
    cSchedulesLock schedulesLock;
    const cSchedules *schedules = cSchedules::Schedules(schedulesLock);

    cString buffer;
    int y = Index * itemChannelHeight;
    
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
    menuPixmap->DrawRectangle(cRect(0, y, menuWidth, itemChannelHeight - 2), ColorBg);
    
    // event from channel
    const cSchedule *Schedule = schedules->GetSchedule( Channel->GetChannelID() );
    if( Schedule ) {
        const cEvent *Event = Schedule->GetPresentEvent();
        if( Event ) {
            // calculate progress bar
            float progress = (int)roundf( (float)(time(NULL) - Event->StartTime()) / (float) (Event->Duration()) * 100.0);
            if(progress < 0)
                progress = 0.;
            else if(progress > 100)
                progress = 100;

            if( WithProvider )
                buffer = cString::sprintf("%d\t%s - %s", Channel->Number(), Channel->Provider(), Channel->Name());
            else
                buffer = cString::sprintf("%d\t%s", Channel->Number(), Channel->Name());

            const char *s1 = GetTabbedText(buffer, 0);
            if( s1 ) {
                int xt = Tab(0);
                menuPixmap->DrawText(cPoint(marginItem + xt, y), s1, ColorFg, ColorBg, font);
            }
            const char *s2 = GetTabbedText(buffer, 1);
            if( s2 ) {
                int xt = Tab(1);
                int w = (menuWidth / 10 * 3) - marginItem;
                menuPixmap->DrawText(cPoint(marginItem + xt, y), s2, ColorFg, ColorBg, font, w);
            }
            
            menuPixmap->DrawRectangle(cRect( (menuWidth/10*3) + marginItem, y, marginItem, fontHeight), ColorBg);
            
            if( Current )
                ProgressBarDrawInline(menuPixmap, (menuWidth/10*3) + marginItem*2, y, menuWidth/10 - marginItem, fontHeight,
                    progress, 100, Theme.Color(clrMenuItemChanCurProgressFg), Theme.Color(clrMenuItemChanCurProgressBarFg),
                    Theme.Color(clrMenuItemChanCurProgressBg));
            else
                ProgressBarDrawInline(menuPixmap, (menuWidth/10*3) + marginItem*2, y, menuWidth/10 - marginItem, fontHeight,
                    progress, 100, Theme.Color(clrMenuItemChanProgressFg), Theme.Color(clrMenuItemChanProgressBarFg),
                    Theme.Color(clrMenuItemChanProgressBg));
            menuPixmap->DrawText(cPoint((menuWidth / 10 * 4) + marginItem*2, y), Event->Title(), ColorFg, ColorBg, font);
            
            return true;
        }
    }

    // without schedule, do it like vdr
    if (!Channel->GroupSep()) {
        if( WithProvider )
            buffer = cString::sprintf("%d\t%s - %s", Channel->Number(), Channel->Provider(), Channel->Name());
        else
            buffer = cString::sprintf("%d\t%s", Channel->Number(), Channel->Name());

        const char *s1 = GetTabbedText(buffer, 0);
        if( s1 ) {
            int xt = Tab(0);
            menuPixmap->DrawText(cPoint(marginItem + xt, y), s1, ColorFg, ColorBg, font);
        }
        const char *s2 = GetTabbedText(buffer, 1);
        if( s2 ) {
            int xt = Tab(1);
            int w = (menuWidth / 10 * 3) - marginItem;

            menuPixmap->DrawText(cPoint(marginItem + xt, y), s2, ColorFg, ColorBg, font, w);
        }
    }
    else {
        buffer = cString::sprintf("---%s ----------------------------------------------------------------", Channel->Name());
        menuPixmap->DrawText(cPoint(marginItem, y), buffer, ColorFg, ColorBg, font);
    }

    return true;
}
*/

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
