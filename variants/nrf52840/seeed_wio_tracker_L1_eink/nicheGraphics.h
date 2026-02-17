#pragma once

#include "configuration.h"

#ifdef MESHTASTIC_INCLUDE_NICHE_GRAPHICS

// InkHUD-specific components
// ---------------------------
#include "graphics/niche/InkHUD/InkHUD.h"

// Applets
#include "graphics/niche/InkHUD/Applets/User/AllMessage/AllMessageApplet.h"
#include "graphics/niche/InkHUD/Applets/User/DM/DMApplet.h"
#include "graphics/niche/InkHUD/Applets/User/Heard/HeardApplet.h"
#include "graphics/niche/InkHUD/Applets/User/Positions/PositionsApplet.h"
#include "graphics/niche/InkHUD/Applets/User/RecentsList/RecentsListApplet.h"
#include "graphics/niche/InkHUD/Applets/User/ThreadedMessage/ThreadedMessageApplet.h"

// Shared NicheGraphics components
// --------------------------------
#include "graphics/niche/Drivers/EInk/ZJY122250_0213BAAMFGN.h"
#include "graphics/niche/Inputs/TwoButton.h"

void setupNicheGraphics()
{
    using namespace NicheGraphics;

    // SPI
    // -----------------------------

    // For NRF52 platforms, SPI pins are defined in variant.h
    SPI1.begin();

    // E-Ink Driver
    // -----------------------------

    Drivers::EInk *driver = new Drivers::ZJY122250_0213BAAMFGN;
    driver->begin(&SPI1, PIN_EINK_DC, PIN_EINK_CS, PIN_EINK_BUSY, PIN_EINK_RES);

    // InkHUD
    // ----------------------------

    InkHUD::InkHUD *inkhud = InkHUD::InkHUD::getInstance();

    // Set the E-Ink driver
    inkhud->setDriver(driver);

    // Set how many FAST updates per FULL update
    inkhud->setDisplayResilience(15);

    // Select fonts
    #ifdef INKHUD_RU
    InkHUD::Applet::fontLarge = FREESANS_12PT_WIN1251;
    InkHUD::Applet::fontMedium = FREESANS_9PT_WIN1251;
    InkHUD::Applet::fontSmall = FREESANS_6PT_WIN1251;
    #else 
    InkHUD::Applet::fontLarge = FREESANS_12PT_WIN1252;
    InkHUD::Applet::fontMedium = FREESANS_9PT_WIN1252;
    InkHUD::Applet::fontSmall = FREESANS_6PT_WIN1252;
    #endif

    // Customize default settings
    inkhud->persistence->settings.rotation = 1;                        // 90 degrees clockwise
    inkhud->persistence->settings.optionalFeatures.batteryIcon = true; // Device definitely has a battery
    inkhud->persistence->settings.userTiles.count = 1;    // One tile only by default, keep things simple for new users
    inkhud->persistence->settings.userTiles.maxCount = 2; // Two applets side-by-side

    // Pick applets
    // Note: order of applets determines priority of "auto-show" feature
    inkhud->addApplet(INK_APPLET_ALL_MESSAGES, new InkHUD::AllMessageApplet, true, true); // Activated, autoshown
    inkhud->addApplet("DMs", new InkHUD::DMApplet);                              // -
    inkhud->addApplet(INK_APPLET_CHANNEL_0, new InkHUD::ThreadedMessageApplet(0));        // -
    inkhud->addApplet(INK_APPLET_CHANNEL_1, new InkHUD::ThreadedMessageApplet(1));        // -
    inkhud->addApplet(INK_APPLET_POSITIONS, new InkHUD::PositionsApplet, true);           // Activated
    inkhud->addApplet(INK_APPLET_RECENT_LIST, new InkHUD::RecentsListApplet);            // -
    inkhud->addApplet(INK_APPLET_HEARD, new InkHUD::HeardApplet, true, false, 0);         // Activated, no autoshow, default on tile 0

    //  Start running InkHUD
    inkhud->begin();

    //  Buttons
    //  --------------------------

    Inputs::TwoButton *buttons = Inputs::TwoButton::getInstance(); // Shared NicheGraphics component

    // #0: Main User Button
    buttons->setWiring(0, Inputs::TwoButton::getUserButtonPin());
    buttons->setTiming(0, 75, 500);
    buttons->setHandlerShortPress(0, [inkhud]() { inkhud->shortpress(); });
    buttons->setHandlerLongPress(0, [inkhud]() { inkhud->longpress(); });

    // Begin handling button events
    buttons->start();
}

#endif