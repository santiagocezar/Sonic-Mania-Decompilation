#include "SonicMania.h"

ObjectSmog *Smog;

void Smog_Update(void)
{
    RSDK_THIS(Smog);
    if (Smog->field_4 == 1) {
        OOZSetup->value10 = 0;
        entity->alpha     = 128;
        RSDK.SetLimitedFade(0, 1, 2, 224, 0, 255);
    }
    else {
        OOZSetup->value10 = 601;
        if (OOZSetup->value10 <= 600) {
            if (entity->alpha > 0) {
                RSDK.SetLimitedFade(0, 1, 2, 0, 0, 255);
                entity->alpha -= 8;
            }
            entity->timer = 0;
        }
        else {
            RSDK.SetLimitedFade(0, 1, 2, (OOZSetup->value10 - 600) >> 2, 0, 255);
            if (entity->alpha < 0x80)
                entity->alpha++;
        }
    }
    if (OOZSetup->value10 > 1800) {
        ++entity->timer;
        foreach_active(Player, player)
        {
            if (player->superState != 2) {
                foreach_all(HUD, hud)
                {
                    if (hud)
                        hud->field_154 = 1;
                    foreach_break;
                }
                if (entity->timer == 60 && !player->sidekick && player->rings > 0)
                    Player_GiveRings(-1, player, true);
            }
        }
        if (entity->timer == 60)
            entity->timer = 0;
    }
}

void Smog_LateUpdate(void) {}

void Smog_StaticUpdate(void)
{
    globals->tempFlags = OOZSetup->flags;
    if (Smog->starPostID != StarPost->postIDs[0]) {
        Smog->starPostID      = StarPost->postIDs[0];
        globals->restartFlags = OOZSetup->flags;
    }
}

void Smog_Draw(void)
{
    RSDK_THIS(Smog);
    ScanlineInfo *scanlinePtr = Smog->scanlines;

    int y       = (Zone->timer + 2 * RSDK_screens->position.y) << 14;
    byte defPos = ((RSDK_screens->position.y >> 1) + 2 * Zone->timer);

    ScanlineInfo *scanline = NULL;
    for (int i = 0; i < RSDK_screens->height; ++i) {
        scanline  = (ScanlineInfo *)&Smog->scanlineData[defPos++ * sizeof(ScanlineInfo)];
        scanlinePtr->position.x = scanline->position.x + (RSDK_screens->position.x << 16);
        scanlinePtr->position.y = y;
        scanlinePtr->deform.x   = scanline->deform.x;
        scanlinePtr->deform.y   = 0;

        ScanlineInfo *data = (ScanlineInfo *)&Smog->scanlineData[defPos * sizeof(ScanlineInfo)];
        y += data->deform.y;
        scanlinePtr++;
    }

    if (entity->alpha >= 0x80)
        RSDK.DrawDeformedSprite(Smog->aniFrames, INK_BLEND, 0xE0);
    else
        RSDK.DrawDeformedSprite(Smog->aniFrames, INK_ALPHA, entity->alpha);
}

void Smog_Create(void *data)
{
    RSDK_THIS(Smog);
    if (!RSDK_sceneInfo->inEditor) {
        entity->active    = ACTIVE_NORMAL;
        entity->visible   = true;
        entity->inkEffect = INK_ALPHA;
        entity->drawOrder = Zone->hudDrawOrder - 1;
        OOZSetup->value10 = 0;
        if (!(RSDK_sceneInfo->milliseconds || RSDK_sceneInfo->seconds || RSDK_sceneInfo->minutes)) {
            Smog->starPostID      = 0;
            globals->restartFlags = 1;
            globals->tempFlags    = 1;
            OOZSetup->flags       = 1;
            return;
        }
        else if (isMainGameMode() && globals->enableIntro) {
            if (!PlayerHelpers_CheckStageReload()) {
                Smog->starPostID      = 0;
                globals->restartFlags = 1;
                globals->tempFlags    = 1;
                OOZSetup->flags       = 1;
                return;
            }
        }

        if (RSDK_sceneInfo->minutes != globals->tempMinutes || RSDK_sceneInfo->seconds != globals->tempSeconds
            || RSDK_sceneInfo->milliseconds != globals->tempMilliseconds) {
            OOZSetup->flags = globals->restartFlags;
            Zone_Unknown2();
        }
        else {
            OOZSetup->flags = globals->tempFlags;
            Zone_Unknown2();
        }
    }
}

void Smog_StageLoad(void)
{
    Smog->aniFrames = RSDK.LoadSpriteSheet("OOZ/Smog.gif", SCOPE_STAGE);
    Smog->scanlines = RSDK.GetScanlines();

    ScanlineInfo *scanline = (ScanlineInfo *)Smog->scanlineData;
    int angle              = 0;
    for (int i = 0; i < 0x100; ++i) {
        scanline[i].deform.x   = (RSDK.Sin256(angle >> 1) + 0x400) << 6;
        scanline[i].deform.y   = (RSDK.Sin256(angle >> 1) + 0x800) << 5;
        scanline[i].position.x = (RSDK.Sin256(angle) << 10) - scanline->deform.x * RSDK_screens->centerX;
        scanline[i].position.y = 0;
        angle += 2;
    }

    RSDK.CopyPalette(0, 0, 1, 0, 128);
    RSDK.CopyPalette(0, 0, 2, 0, 128);
    RSDK.SetLimitedFade(2, 1, 3, 64, 0, 127);
    RSDK.SetLimitedFade(0, 1, 2, 0, 0, 255);
    RSDK.SetDrawLayerProperties(Zone->hudDrawOrder - 1, false, Smog_DrawLayerCB);
}

void Smog_DrawLayerCB(void) { RSDK.CopyPalette(1, 0, 0, 0, 128); }

void Smog_EditorDraw(void) {}

void Smog_EditorLoad(void) {}

void Smog_Serialize(void) {}