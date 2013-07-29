#include "symbols/audio.xpm"
#include "symbols/dolbydigital.xpm"

#include "displaychannel.h"

cFlatDisplayChannel::cFlatDisplayChannel(bool WithInfo) {
    present = NULL;
    channelName = "";

    CreateFullOsd();
    if ( !osd )
        return;
    TopBarCreate();
    MessageCreate();

    // von unten noch oben
    // 2 * EPG + 2 * EPGsml
    int heightProgress = 8;
    heightBottom = (fontHeight * 2) + (fontSmlHeight * 2) + heightProgress; // Top, Buttom, Between
    int heightTop = fontHeight;

    int height = heightBottom;
    chanInfoBottomPixmap = osd->CreatePixmap(1, cRect(0, osdHeight - height, osdWidth, heightBottom));

    height += heightProgress;
    chanInfoProgressPixmap = osd->CreatePixmap(2, cRect(0, osdHeight - height, osdWidth, heightProgress));
    chanInfoProgressPixmap->Fill( clrTransparent );

    height += heightTop + 2;
    chanInfoTopPixmap = osd->CreatePixmap(1, cRect(0, osdHeight - height, osdWidth, heightTop));
}

cFlatDisplayChannel::~cFlatDisplayChannel() {
    if (osd) {
        osd->DestroyPixmap(chanInfoTopPixmap);
        osd->DestroyPixmap(chanInfoProgressPixmap);
        osd->DestroyPixmap(chanInfoBottomPixmap);
    }
}

void cFlatDisplayChannel::SetChannel(const cChannel *Channel, int Number) {
    cString channelNumber("");
    if (Channel) {
        channelName = Channel->Name();
        if (!Channel->GroupSep())
            channelNumber = cString::sprintf("%d%s", Channel->Number(), Number ? "-" : "");
        else if (Number)
            channelNumber = cString::sprintf("%d-", Number);
    } else
        channelName = ChannelString(NULL, 0);

    cString channelString = cString::sprintf("%s  %s", *channelNumber, *channelName);

    chanInfoTopPixmap->Fill(Theme.Color(clrChannelBg));
    chanInfoTopPixmap->DrawText(cPoint(50, 0), channelString, Theme.Color(clrChannelFontTitle), Theme.Color(clrChannelBg), font);
}

void cFlatDisplayChannel::SetEvents(const cEvent *Present, const cEvent *Following) {
    present = Present;
    cString epgShort("");
    cString epg("");

    chanInfoBottomPixmap->Fill(Theme.Color(clrChannelBg));

    int imageHeight = heightBottom - marginItem*2;
    if( imgLoader.LoadLogo(*channelName, imageHeight, imageHeight) ) {
        int imageLeft = marginItem;
        int imageTop = (heightBottom - imgLoader.Height() ) / 2;
        
        chanInfoBottomPixmap->DrawImage( cPoint(imageLeft, imageTop), imgLoader.GetImage() );
    }

    int left = heightBottom + marginItem;

    if( Present ) {
        cString startTime = Present->GetTimeString();
        cString endTime = Present->GetEndTimeString();

        cString timeString = cString::sprintf("%s - %s", *startTime, *endTime);
        int timeStringWidth = fontSml->Width(*timeString);

        int epgWidth = font->Width(Present->Title());
        int epgShortWidth = fontSml->Width(Present->ShortText());
        
        int s = (int)(time(NULL) - Present->StartTime()) / 60;
        int sleft = (Present->Duration() / 60) - s;

        cString seen = cString::sprintf("%d-/%d+ (%d) min", s, sleft, Present->Duration() / 60);
        int seenWidth = fontSml->Width(*seen);

        if( epgWidth > osdWidth - left - timeStringWidth ) {
            int dotsWidth = font->Width("... ");
            cTextWrapper epgInfoWrapper(Present->Title(), font, osdWidth - left - timeStringWidth - dotsWidth);
            epg = epgInfoWrapper.GetLine(0);
            epg = cString::sprintf("%s...", *epg);
        } else {
            epg = Present->Title();
        }

        if( epgShortWidth > osdWidth - left - timeStringWidth ) {
            int dotsWidth = fontSml->Width("... ");
            cTextWrapper epgInfoWrapper(Present->ShortText(), fontSml, osdWidth - left - timeStringWidth - dotsWidth);
            epgShort = epgInfoWrapper.GetLine(0);
            epgShort = cString::sprintf("%s...", *epgShort);
        } else {
            epgShort = Present->ShortText();
        }
        
        chanInfoBottomPixmap->DrawText(cPoint(osdWidth - timeStringWidth, 0), *timeString,
                Theme.Color(clrChannelFontEpg), Theme.Color(clrChannelBg), fontSml);
        chanInfoBottomPixmap->DrawText(cPoint(osdWidth - seenWidth, fontSmlHeight), *seen,
                Theme.Color(clrChannelFontEpg), Theme.Color(clrChannelBg), fontSml);
        
        chanInfoBottomPixmap->DrawText(cPoint(left, 0), *epg, Theme.Color(clrChannelFontEpg), Theme.Color(clrChannelBg), font);
        chanInfoBottomPixmap->DrawText(cPoint(left, fontHeight), *epgShort, Theme.Color(clrChannelFontEpg), Theme.Color(clrChannelBg), fontSml);

    }
    if( Following ) {
        cString startTime = Following->GetTimeString();
        cString endTime = Following->GetEndTimeString();

        cString timeString = cString::sprintf("%s - %s", *startTime, *endTime);
        int timeStringWidth = fontSml->Width(*timeString);

        int epgWidth = fontSml->Width(Following->Title());
        int epgShortWidth = fontSml->Width(Following->ShortText());
        
        cString dur = cString::sprintf("%d min", Following->Duration() / 60);
        int durWidth = fontSml->Width(*dur);

        if( epgWidth > osdWidth - left - timeStringWidth ) {
            int dotsWidth = font->Width("... ");
            cTextWrapper epgInfoWrapper(Following->Title(), font, osdWidth - left - timeStringWidth - dotsWidth);
            epg = epgInfoWrapper.GetLine(0);
            epg = cString::sprintf("%s...", *epg);
        } else {
            epg = Following->Title();
        }
        if (epgShortWidth > osdWidth - left - timeStringWidth ) {
            int dotsWidth = fontSml->Width("... ");
            cTextWrapper epgInfoWrapper(Following->ShortText(), fontSml, osdWidth - left - timeStringWidth - dotsWidth);
            epgShort = epgInfoWrapper.GetLine(0);
            epgShort = cString::sprintf("%s...", *epgShort);
        } else {
            epgShort = Following->ShortText();
        }

        chanInfoBottomPixmap->DrawText(cPoint(osdWidth - timeStringWidth, fontHeight + fontSmlHeight), *timeString,
                Theme.Color(clrChannelFontEpg), Theme.Color(clrChannelBg), fontSml);
        chanInfoBottomPixmap->DrawText(cPoint(osdWidth - durWidth, fontHeight + fontSmlHeight*2), *dur,
                Theme.Color(clrChannelFontEpg), Theme.Color(clrChannelBg), fontSml);

        chanInfoBottomPixmap->DrawText(cPoint(left, fontHeight + fontSmlHeight), *epg,
                Theme.Color(clrChannelFontEpg), Theme.Color(clrChannelBg), font);
        chanInfoBottomPixmap->DrawText(cPoint(left, fontHeight*2 + fontSmlHeight), *epgShort,
                Theme.Color(clrChannelFontEpg), Theme.Color(clrChannelBg), fontSml);
    }
}

void cFlatDisplayChannel::DrawProgressBar(int Current, int Total) {
    int barFullWidth = chanInfoProgressPixmap->ViewPort().Width();
    double percentLeft = ((double)Current) / (double)Total;

    chanInfoProgressPixmap->Fill( clrTransparent );

    if (Current > 0) {
        chanInfoProgressPixmap->DrawRectangle(cRect( 0, 2, barFullWidth, 2), Theme.Color(clrChannelProgressBg));
        chanInfoProgressPixmap->DrawRectangle(cRect( 0, 0, (barFullWidth * percentLeft), 6), Theme.Color(clrChannelProgressFg));
    }
}

void cFlatDisplayChannel::SetMessage(eMessageType Type, const char *Text) {
    // Wenn es einen Text gibt, diesen Anzeigen ansonsten Message ausblenden
    if( Text )
        MessageSet(Type, Text);
    else
        MessageClear();
}

void cFlatDisplayChannel::Flush(void) {
    int Current = 0;
    int Total = 0;
    if( present ) {
        time_t t = time(NULL);
        if (t > present->StartTime())
            Current = t - present->StartTime();
        Total = present->Duration();
        DrawProgressBar(Current, Total);
    }
    TopBarUpdate();
    osd->Flush();
}
