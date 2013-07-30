#pragma once

#include <vdr/skins.h>
#include <vdr/videodir.h>

class cFlatDisplayMenu;
extern cTheme Theme;

// BUTTONS
#define CLR_BUTTONRED                       0x99BB0000
#define CLR_BUTTONGREEN                     0x9900BB00
#define CLR_BUTTONYELLOW                    0x99BBBB00
#define CLR_BUTTONBLUE                      0x990000BB

// MESSAGES
#define CLR_MESSAGESTATUS                   0x900000FF
#define CLR_MESSAGEINFO                     0x90009900
#define CLR_MESSAGEWARNING                  0x90BBBB00
#define CLR_MESSAGEERROR                    0x90BB0000

// TopBar
THEME_CLR(Theme, clrTopBarBg,               0x90222222);
THEME_CLR(Theme, clrTopBarFont,             0xFFEEEEEE);
THEME_CLR(Theme, clrTopBarDateTimeFont,     0xFFEEEEEE);

// Buttons
THEME_CLR(Theme, clrButtonBg,               0x90222222);
THEME_CLR(Theme, clrButtonFont,             0xFFEEEEEE);
THEME_CLR(Theme, clrButtonRed,              CLR_BUTTONRED);
THEME_CLR(Theme, clrButtonGreen,            CLR_BUTTONGREEN);
THEME_CLR(Theme, clrButtonYellow,           CLR_BUTTONYELLOW);
THEME_CLR(Theme, clrButtonBlue,             CLR_BUTTONBLUE);

// Messages
THEME_CLR(Theme, clrMessageBg,              0x90222222);
THEME_CLR(Theme, clrMessageFont,            0xFFEEEEEE);

THEME_CLR(Theme, clrMessageStatus,          CLR_MESSAGESTATUS);
THEME_CLR(Theme, clrMessageInfo,            CLR_MESSAGEINFO);
THEME_CLR(Theme, clrMessageWarning,         CLR_MESSAGEWARNING);
THEME_CLR(Theme, clrMessageError,           CLR_MESSAGEERROR);

// Channel
THEME_CLR(Theme, clrChannelBg,              0x90222222);
THEME_CLR(Theme, clrChannelFontTitle,       0xFFEEEEEE);
THEME_CLR(Theme, clrChannelFontEpg,         0xFFEEEEEE);
THEME_CLR(Theme, clrChannelProgressFg,      0xFFEEEEEE);
THEME_CLR(Theme, clrChannelProgressBg,      0xFFEEEEEE);

// Menu
THEME_CLR(Theme, clrItemBg,                 0x90909090);
THEME_CLR(Theme, clrItemFont,               0xFFEEEEEE);
THEME_CLR(Theme, clrItemCurrentBg,          0x903090B0);
THEME_CLR(Theme, clrItemCurrentFont,        0xFFEEEEEE);
THEME_CLR(Theme, clrItemSelableBg,          0x90222222);
THEME_CLR(Theme, clrItemSelableFont,        0xFFEEEEEE);
THEME_CLR(Theme, clrScrollbarFg,            0xFFEEEEEE);
THEME_CLR(Theme, clrScrollbarBg,            0xFFEEEEEE);
// Menu Event
THEME_CLR(Theme, clrMenuEventBg,            0x90222222);
THEME_CLR(Theme, clrMenuEventFontTitle,     0xFFEEEEEE);
THEME_CLR(Theme, clrMenuEventFontInfo,      0xFFEEEEEE);
// Menu Recording
THEME_CLR(Theme, clrMenuRecBg,              0x90222222);
THEME_CLR(Theme, clrMenuRecFontTitle,       0xFFEEEEEE);
THEME_CLR(Theme, clrMenuRecFontInfo,        0xFFEEEEEE);
// Menu Text (Multiline)
THEME_CLR(Theme, clrMenuTextBg,             0x90222222);
THEME_CLR(Theme, clrMenuTextFont,           0xFFEEEEEE);

// Replay
THEME_CLR(Theme, clrReplayBg,               0x90222222);
THEME_CLR(Theme, clrReplayFont,             0xFFEEEEEE);
THEME_CLR(Theme, clrReplayProgressFg,       0xFFEEEEEE);
THEME_CLR(Theme, clrReplayProgressBg,       0xFFEEEEEE);
THEME_CLR(Theme, clrReplayMarkFg,           0xFF222222);
THEME_CLR(Theme, clrReplayMarkCurrentFg,    0xFF3090B0);

// Tracks
THEME_CLR(Theme, clrTrackItemBg,            0x90909090);
THEME_CLR(Theme, clrTrackItemFont,          0xFFEEEEEE);
THEME_CLR(Theme, clrTrackItemCurrentBg,     0x903090B0);
THEME_CLR(Theme, clrTrackItemCurrentFont,   0xFFEEEEEE);

// Volume
THEME_CLR(Theme, clrVolumeBg,               0x90222222);
THEME_CLR(Theme, clrVolumeFont,             0xFFEEEEEE);
THEME_CLR(Theme, clrVolumeProgressFg,       0xFFEEEEEE);
THEME_CLR(Theme, clrVolumeProgressBg,       0xFFEEEEEE);

class cFlat : public cSkin {
    private:
        cFlatDisplayMenu *displayMenu;
    public:
        cFlat(void);
        virtual const char *Description(void);
        virtual cSkinDisplayChannel *DisplayChannel(bool WithInfo);
        virtual cSkinDisplayMenu *DisplayMenu(void);
        virtual cSkinDisplayReplay *DisplayReplay(bool ModeOnly);
        virtual cSkinDisplayVolume *DisplayVolume(void);
        virtual cSkinDisplayTracks *DisplayTracks(const char *Title, int NumTracks, const char * const *Tracks);
        virtual cSkinDisplayMessage *DisplayMessage(void);
};
