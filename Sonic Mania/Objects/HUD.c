#include "../SonicMania.h"

ObjectHUD *HUD;

void HUD_Update()
{
    EntityHUD *entity = (EntityHUD *)RSDK_sceneInfo->entity;
    entity->field_150 = 0;
    entity->field_154 = 0;
    if (entity->taData2.animationID == 11)
        RSDK.ProcessAnimation(&entity->taData2);
}

void HUD_LateUpdate()
{
    EntityHUD *entity = (EntityHUD *)RSDK_sceneInfo->entity;
    if (options->gameMode == MODE_COMPETITION) {
        for (entity->screenID = 0; entity->screenID < RSDK.GetSettingsValue(SETTINGS_SCREENCOUNT); ++entity->screenID) {
            if (entity->competitionStates[entity->screenID])
                entity->competitionStates[entity->screenID]();
        }
    }
    else {
        if (entity->state)
            entity->state();
    }

    if (options->gameMode >= MODE_TIMEATTACK) {
        if (options->gameMode != MODE_TIMEATTACK)
            return;
        if (HUD->dwordC) {
            if (RSDK_sku->platform == PLATFORM_PC || RSDK_sku->platform == PLATFORM_SWITCH || RSDK_sku->platform == PLATFORM_DEV) {
                entity = (EntityHUD *)RSDK_sceneInfo->entity;
                HUD_GetKeyFrame(&entity->superButtonData1, 3);
                HUD_GetKeyFrame(&entity->superButtonData3, 3);
                HUD_GetKeyFrame(&entity->superButtonData2, 4);
            }
            if (entity->field_A0 < 0x180000)
                entity->field_A0 += 0x80000;
        }
        else {
            if (entity->field_A0 <= 0x400000)
                entity->field_A0 += 0x80000;
        }
    }
    else {
        EntityPlayer *player = (EntityPlayer *)RSDK.GetEntityByID(SLOT_PLAYER1);
        if (RSDK_sceneInfo->timeEnabled && player->rings >= 50 && player->superState < 2 && SaveGame->saveRAM[28] == 0x7F) {
            if (RSDK_sku->platform == PLATFORM_PC || RSDK_sku->platform == PLATFORM_SWITCH || RSDK_sku->platform == PLATFORM_DEV)
                HUD_GetSuperFrames();
            if (entity->field_A0 < 0x180000)
                entity->field_A0 += 0x80000;
        }
        else {
            if (entity->field_A0 >= -0x400000)
                entity->field_A0 -= 0x80000;
        }
    }
}

void HUD_StaticUpdate()
{

}

void HUD_Draw()
{
    EntityHUD *entity = (EntityHUD *)RSDK_sceneInfo->entity;
    EntityPlayer *player = (EntityPlayer *)RSDK.GetEntityByID(RSDK_sceneInfo->currentScreenID);

    Vector2 lifePos;

    Vector2 pos[4];

    if (options->gameMode == MODE_COMPETITION) {
        pos[0].x      = entity->dwordB4[RSDK_sceneInfo->currentScreenID].x;
        pos[0].y      = entity->dwordB4[RSDK_sceneInfo->currentScreenID].y;
        pos[1].x   = entity->dwordD4[RSDK_sceneInfo->currentScreenID].x;
        pos[1].y   = entity->dwordD4[RSDK_sceneInfo->currentScreenID].y;
        pos[2].x   = entity->dwordF4[RSDK_sceneInfo->currentScreenID].x;
        pos[2].y   = entity->dwordF4[RSDK_sceneInfo->currentScreenID].y;
        pos[3].x   = entity->dword114[RSDK_sceneInfo->currentScreenID].x;
        pos[3].y   = entity->dword114[RSDK_sceneInfo->currentScreenID].y;

        EntityPlayer *plr = NULL;
        while (RSDK.GetActiveEntities(Player->objectID, (Entity **)&plr)) {
            if (plr != player) {
                entity->playerIDData.frameID = plr->playerID;
                RSDK.DrawSprite(&entity->playerIDData, &plr->position, 0);
            }
        }
    }
    else {
        pos[0].x      = entity->dword5C[0].x;
        pos[0].y      = entity->dword5C[0].y;
        pos[1].x   = entity->dword5C[1].x;
        pos[1].y   = entity->dword5C[1].y;
        pos[2].x   = entity->dword5C[2].x;
        pos[2].y   = entity->dword5C[2].y;
        pos[3].x      = entity->dword5C[3].x;
        pos[3].y   = entity->dword5C[3].y;
        if (HUD->field_24 > 0) {
            RSDK.ProcessAnimation(&entity->playerIDData);
            RSDK.DrawSprite(&entity->playerIDData, &player->position, 0);
            --HUD->field_24;
        }
    }

    if (player->rings)
        entity->ringFlashFrame = 0;
    else
        entity->ringFlashFrame = (Zone->timer2 >> 3) & 1;

    if ((RSDK_sceneInfo->minutes != 9 || options->gameMode >= MODE_TIMEATTACK || options->medalMods & getMod(MEDAL_NOTIMEOVER))
        /*&& ActClear->field_30 != 1*/) {
        entity->timeFlashFrame = 0;
    }
    else {
        entity->timeFlashFrame = (Zone->timer2 >> 3) & 1;
    }
    entity->hudElementsData.frameID = 0;
    RSDK.DrawSprite(&entity->hudElementsData, &pos[0], true);

    lifePos.x = pos[0].x + 0x610000;
    lifePos.y = pos[0].y + 0xE0000;
    HUD_DrawNumbersBase10(&lifePos, player->score, 0);
    entity->hudElementsData.frameID = entity->timeFlashFrame + 1;
    RSDK.DrawSprite(&entity->hudElementsData, &pos[1], true);

    if (!entity->field_150 || Zone->timer2 & 8) {
        lifePos.x                       = pos[1].x + 0x340000;
        lifePos.y                       = pos[1].y - 0x20000;
        entity->hudElementsData.frameID = 12;
        RSDK.DrawSprite(&entity->hudElementsData, &lifePos, true);

        lifePos.x = pos[1].x + 0x610000;
        lifePos.y = pos[1].y + 0xE0000;
        HUD_DrawNumbersBase10(&lifePos, RSDK_sceneInfo->milliseconds, 2);

        lifePos.x -= 0x90000;
        HUD_DrawNumbersBase10(&lifePos, RSDK_sceneInfo->seconds, 2);

        lifePos.x -= 0x90000;

        if (RSDK_sceneInfo->minutes > 9 && options->medalMods & getMod(MEDAL_NOTIMEOVER))
            HUD_DrawNumbersBase10(&lifePos, RSDK_sceneInfo->minutes, 2);
        else
            HUD_DrawNumbersBase10(&lifePos, RSDK_sceneInfo->minutes, 1);
    }

    entity->hudElementsData.frameID = entity->ringFlashFrame + 3;
    RSDK.DrawSprite(&entity->hudElementsData, &pos[2], true);
    if (!entity->field_154 || Zone->timer2 & 8) {
        lifePos.x = pos[2].x + 0x610000;
        lifePos.y = pos[2].y + 0xE0000;

        if (player->hyperRing)
            HUD_DrawNumbersHyperRing(&lifePos, player->rings);
        else
            HUD_DrawNumbersBase10(&lifePos, player->rings, 0);
    }

    if (((Entity*)RSDK.GetEntityByID(SLOT_PLAYER1))->objectID == DebugMode->objectID) {
        if (player->camera) {
            lifePos.y = 0x180000;
            lifePos.x = (RSDK_screens[player->camera->screenID].width - 16) << 16;
            HUD_DrawNumbersBase16(&lifePos, RSDK_screens[player->camera->screenID].position.y);

            lifePos.x -= 0x90000;
            HUD_DrawNumbersBase16(&lifePos, RSDK_screens[player->camera->screenID].position.x);

            lifePos.y += 0x100000;
            lifePos.x = (RSDK_screens[player->camera->screenID].width - 16) << 16;
            HUD_DrawNumbersBase16(&lifePos, (player->position.y >> 0x10));

            lifePos.x -= 0x90000;
            HUD_DrawNumbersBase16(&lifePos, (player->position.x >> 0x10));
        }
    }
    else if (entity->field_A0 <= -0x400000 || options->gameMode != MODE_TIMEATTACK) {
        if (entity->field_A0 > -0x200000) {
            lifePos.y = 0x140000;
            lifePos.x = (RSDK_screens[RSDK_sceneInfo->currentScreenID].width << 16) - entity->field_A0;
            RSDK.DrawSprite(&entity->superButtonsData, &lifePos, true);
            lifePos.x -= 0x140000;
            bool32 flag = true;
            if (Player->field_A6C)
                flag = Player->field_A6C(1);
            if (player->state == Player_State_Air && player->jumpAbilityTimer == 1 && flag) {
                RSDK.DrawSprite(&entity->superButtonData1, &lifePos, true);
            }
            else {
                entity->inkEffect = INK_BLEND;
                RSDK.DrawSprite(&entity->superButtonData1, &lifePos, true);
                entity->inkEffect = INK_NONE;
            }
        }
    }
    else {
        lifePos.y = 0x140000;
        lifePos.x = (RSDK_screens[RSDK_sceneInfo->currentScreenID].width << 16) - entity->field_A0;
        if (User.CheckDLC(DLC_PLUS)) {
            RSDK.DrawSprite(&entity->taData2, &lifePos, true);
            lifePos.x -= 0x1C0000;

            if (HUD->gap10) {
                RSDK.DrawSprite(&entity->superButtonData3, &lifePos, true);
            }
            else {
                entity->inkEffect = INK_BLEND;
                RSDK.DrawSprite(&entity->superButtonData3, &lifePos, true);
                entity->inkEffect = INK_NONE;
            }

            lifePos.y += 0x1C0000;
            lifePos.x = (RSDK_screens[RSDK_sceneInfo->currentScreenID].width << 16) - entity->field_A0;
        }
        RSDK.DrawSprite(&entity->taData1, &lifePos, true);
        lifePos.x -= 0x1C0000;
        RSDK.DrawSprite(&entity->superButtonData2, &lifePos, true);
    }

    int cID    = -1;
    lifePos.x  = pos[3].x;
    lifePos.y  = pos[3].y;
    int charID       = player->characterID;
    int lives  = entity->playerLives[player->playerID];
    for (; charID > 0; ++cID) charID >>= 1;
    entity->lifeIconsData.frameID = cID;
    if (cID < 0) {
        entity->lifeIconsData.frameID = entity->field_7C[player->playerID];
        lives                         = lives - 1;
    }
    else {
        entity->field_7C[player->playerID]    = cID;
        entity->playerLives[player->playerID] = player->lives;
    }
    RSDK.DrawSprite(&entity->lifeIconsData, &lifePos, true);

    if (options->gameMode == MODE_ENCORE) {
        if (HUD->field_28 > 0)
            HUD->field_28--;
        if (HUD->field_2C > 0)
            HUD->field_2C--;
        if (HUD->field_30 > 0)
            HUD->field_30--;
        if (HUD->field_34 > 0)
            HUD->field_34--;

        lifePos.x += 0x140000;
        EntityPlayer *sidekick = (EntityPlayer *)RSDK.GetEntityByID(SLOT_PLAYER2);
        if (sidekick->objectID) {
            int charID = sidekick->characterID;
            int id     = -1;
            for (id = -1; charID > 0; ++id) charID >>= 1;
            entity->lifeIconsData.frameID = id;
            if (id >= 0 && !(HUD->field_28 & 4)) {
                if (sidekick->state != Player_State_Die && sidekick->state != Player_State_Drown
                    && (/*sidekick->state != Player_State_Unknown1*/ false || !sidekick->maxGlideSpeed)) {
                    RSDK.DrawSprite(&entity->lifeIconsData, &lifePos, true);
                }
            }
            lifePos.x += 0x140000;
            RSDK.SetSpriteAnimation(HUD->hudMappings, 12, &entity->lifeIconsData, true, 0);
            id = -1;
            int stock = options->stock & 0xFF;
            if (stock) {
                do {
                    stock >>= 1;
                    ++id;
                } while (stock > 0);
            }
            entity->lifeIconsData.frameID = id;
            if (id >= 0 && !(HUD->field_2C & 4))
                RSDK.DrawSprite(&entity->lifeIconsData, &lifePos, true);

            id = -1;
            lifePos.x += 0x100000;
            stock = (options->stock >> 8) & 0xFF;
            if (options->stock >> 8) {
                do {
                    stock >>= 1;
                    ++id;
                } while (stock > 0);
            }
            entity->lifeIconsData.frameID = id;
            if (id >= 0 && !(HUD->field_30 & 4))
                RSDK.DrawSprite(&entity->lifeIconsData, &lifePos, true);

            id = -1;
            lifePos.x += 0x100000;
            int flags = options->characterFlags;
            if (options->characterFlags) {
                do {
                    flags >>= 1;
                    ++id;
                } while (flags > 0);
            }
            entity->lifeIconsData.frameID = id;
            if (id >= 0 && !(HUD->field_34 & 4))
                RSDK.DrawSprite(&entity->lifeIconsData, &lifePos, true);
            RSDK.SetSpriteAnimation(HUD->hudMappings, 2, &entity->lifeIconsData, true, 0);
        }
    }
    else {
        entity->hudElementsData.frameID = 14;
        RSDK.DrawSprite(&entity->hudElementsData, &lifePos, true);
        lifePos.x += 0x300000;
        if (player->lives < 10)
            lifePos.x -= 0x80000;
        HUD_DrawNumbersBase10(&lifePos, lives, 0);
    }

    if (options->gameMode == MODE_COMPETITION) {
        switch (HUD->compSession[RSDK_sceneInfo->currentScreenID]) {
            case 1:
                RSDK.DrawRect(RSDK_screens->width - 1, 0, 1, RSDK_screens->height, 0, 255, INK_NONE, true);
                RSDK.DrawRect(0, RSDK_screens->height - 1, RSDK_screens->width, 1, 0, 255, INK_NONE, true);
                break;
            case 2: RSDK.DrawRect(0, RSDK_screens->height - 1, RSDK_screens->width, 1, 0, 255, INK_NONE, true); break;
            case 3:
                RSDK.DrawRect(0, 0, 1, RSDK_screens[1].height, 0, 255, INK_NONE, true);
                RSDK.DrawRect(0, RSDK_screens->height - 1, RSDK_screens->width, 1, 0, 255, INK_NONE, true);
                break;
            case 4:
                RSDK.DrawRect(RSDK_screens->width - 1, 0, 1, RSDK_screens->height, 0, 255, INK_NONE, true);
                RSDK.DrawRect(0, 0, RSDK_screens[1].width, 1, 0, 255, INK_NONE, true);
                break;
            case 5: RSDK.DrawRect(0, 0, RSDK_screens[1].width, 1, 0, 255, INK_NONE, true); break;
            case 6:
                RSDK.DrawRect(0, 0, 1, RSDK_screens[1].height, 0, 255, INK_NONE, true);
                RSDK.DrawRect(0, 0, RSDK_screens[1].width, 1, 0, 255, INK_NONE, true);
                break;
            default: break;
        }
    }
}

void HUD_Create(void* data)
{
    EntityHUD *entity = (EntityHUD *)RSDK_sceneInfo->entity;
    if (!RSDK_sceneInfo->inEditor) {
        //ActClear->field_30 = 0;
        entity->active    = ACTIVE_NORMAL;
        entity->visible   = 1;
        entity->drawOrder = Zone->uiDrawHigh;
        entity->dword5C[0].x   = 0x100000;
        entity->dword5C[0].y   = 0xC0000;
        entity->dword5C[1].x   = 0x100000;
        entity->dword5C[1].y   = 0x1C0000;
        entity->dword5C[2].x   = 0x100000;
        entity->dword5C[2].y   = 0x2C0000;
        entity->dword5C[3].x   = 0x100000;
        entity->field_A0       = -0x200000;

        entity->dword5C[3].y   = (RSDK_screens->height - 12) << 16;
        entity->dwordB4[0].x   = entity->dword5C[0].x;
        entity->dwordB4[0].y   = entity->dword5C[0].y;
        entity->dwordD4[0].x   = entity->dword5C[1].x;
        entity->dwordD4[0].y   = entity->dword5C[1].y;
        entity->dwordF4[0].x   = entity->dword5C[2].x;
        entity->dwordF4[0].y   = entity->dword5C[2].y;
        entity->dword114[0].x  = entity->dword5C[3].x;
        entity->dword114[0].y  = entity->dword5C[3].y;
        entity->dwordB4[1].x   = entity->dword5C[0].x;
        entity->dwordB4[1].y   = entity->dword5C[0].y;
        entity->dwordD4[1].x   = entity->dword5C[1].x;
        entity->dwordD4[1].y   = entity->dword5C[1].y;
        entity->dwordF4[1].x   = entity->dword5C[2].x;
        entity->dwordF4[1].y   = entity->dword5C[2].y;
        entity->dword114[1].x  = entity->dword5C[3].x;
        entity->dword114[1].y  = entity->dword5C[3].y;
        entity->dwordB4[2].x   = entity->dword5C[0].x;
        entity->dwordB4[2].y   = entity->dword5C[0].y;
        entity->dwordD4[2].x   = entity->dword5C[1].x;
        entity->dwordD4[2].y   = entity->dword5C[1].y;
        entity->dwordF4[2].x   = entity->dword5C[2].x;
        entity->dwordF4[2].y   = entity->dword5C[2].y;
        entity->dword114[2].x  = entity->dword5C[3].x;
        entity->dword114[2].y  = entity->dword5C[3].y;
        entity->dwordB4[3].x   = entity->dword5C[0].x;
        entity->dwordB4[3].y   = entity->dword5C[0].y;
        entity->dwordD4[3].x   = entity->dword5C[1].x;
        entity->dwordD4[3].y   = entity->dword5C[1].y;
        entity->dwordF4[3].x   = entity->dword5C[2].x;
        entity->dwordF4[3].y   = entity->dword5C[2].y;
        entity->dword114[3].x  = entity->dword5C[3].x;
        entity->dword114[3].y  = entity->dword5C[3].y;
        RSDK.SetSpriteAnimation(HUD->hudMappings, 0, &entity->hudElementsData, true, 0);
        RSDK.SetSpriteAnimation(HUD->hudMappings, 1, &entity->numbersData, true, 0);
        RSDK.SetSpriteAnimation(HUD->hudMappings, 9, &entity->hyperNumbersData, true, 0);
        RSDK.SetSpriteAnimation(HUD->hudMappings, 2, &entity->lifeIconsData, true, 0);
        if (options->gameMode == MODE_ENCORE)
            RSDK.SetSpriteAnimation(HUD->hudMappings, 13, &entity->playerIDData, true, 0);
        else
            RSDK.SetSpriteAnimation(HUD->hudMappings, 8, &entity->playerIDData, true, 0);
        RSDK.SetSpriteAnimation(HUD->superButtonMappings, 0, &entity->superButtonsData, true, 0);
        RSDK.SetSpriteAnimation(HUD->hudMappings, 10, &entity->taData1, true, 2);
        RSDK.SetSpriteAnimation(HUD->hudMappings, 10, &entity->taData2, true, 1);
        HUD_GetKeyFrame(&entity->superButtonData1, 3);
        HUD_GetKeyFrame(&entity->superButtonData3, 3);
        HUD_GetKeyFrame(&entity->superButtonData2, 4);
        RSDK.SetDebugValue("Show HUD", &entity->visible, 2, 0, 1);
    }
}

void HUD_StageLoad()
{
    HUD->hudMappings         = RSDK.LoadSpriteAnimation("Global/HUD.bin", SCOPE_STAGE);
    HUD->superButtonMappings = RSDK.LoadSpriteAnimation("Global/SuperButtons.bin", SCOPE_STAGE);
    HUD->sfx_Click           = RSDK.GetSFX("Stage/Click.wav");
    HUD->sfx_Starpost        = RSDK.GetSFX("Global/StarPost.wav");
    HUD->dwordC              = 0;
    if (options->gameMode == MODE_COMPETITION) {
        HUD->compSession[0] = options->competitionSession[84];
        HUD->compSession[1] = options->competitionSession[85];
        HUD->compSession[2] = options->competitionSession[86];
        HUD->compSession[3] = options->competitionSession[87];
    }
}

void HUD_DrawNumbersBase10(Vector2 *drawPos, int value, signed int maxDigits)
{
    EntityHUD *entity = (EntityHUD *)RSDK_sceneInfo->entity;
    int mult     = 1;
    if (!maxDigits) {
        if (value <= 0) {
            maxDigits = 1;
        }
        else {
            int v = value;
            do {
                ++maxDigits;
                v /= 10;
            } while (v > 0);
            RSDK_sceneInfo->entity;
        }
    }

    while (maxDigits--) {
        entity->numbersData.frameID = value / mult % 10;
        RSDK.DrawSprite(&entity->numbersData, drawPos, true);
        mult *= 10;
        drawPos->x -= 0x80000;
    }
}

void HUD_DrawNumbersBase16(Vector2 *drawPos, int value)
{
    int mult   = 1;
    EntityHUD *entity = (EntityHUD *)RSDK_sceneInfo->entity;
    for (int i = 4; i; --i) {
        entity->numbersData.frameID = value / mult & 0xF;
        RSDK.DrawSprite(&entity->numbersData, drawPos, true);
        drawPos->x -= 0x80000;
        mult *= 16;
    }
}

void HUD_DrawNumbersHyperRing(Vector2 *drawPos, int value)
{
    EntityHUD *entity = (EntityHUD *)RSDK_sceneInfo->entity;
    int cnt               = 0;
    int mult              = 1;
    int mult2             = 1;
    if (value <= 0) {
        cnt = 1;
    }
    else {
        int v = value;
        while (v) {
            ++cnt;
            v /= 10;
        }
    }

    if (cnt > 0) {
        while (cnt--) {
            entity->hyperNumbersData.frameID = value / mult % 10;
            RSDK.DrawSprite(&entity->hyperNumbersData, drawPos, true);
            drawPos->x -= 0x80000;
            mult = 10 * mult2;
            mult2 *= 10;
        }
    }

    entity->hyperNumbersData.frameID = 10;
    drawPos->x -= 0x40000;
    RSDK.DrawSprite(&entity->hyperNumbersData, drawPos, true);
}

void HUD_GetKeyFrame(EntityAnimationData *data, int buttonID)
{
    int val = 0; // UIButtonPrompt_Unknown1();
    if (User.GetConfirmButtonFlip() && buttonID <= 1)
        buttonID ^= 1u;
    if (val != 1 && (val <= 8 || val > 12)) {
        RSDK.SetSpriteAnimation(HUD->superButtonMappings, val, data, true, buttonID);
    }
    else {
        EntityPlayer *player = (EntityPlayer *)RSDK.GetEntityByID(SLOT_PLAYER1);
        int id                   = RSDK.ControllerIDForInputID(player->controllerID);
        int contID           = player->controllerID;
        if (id == -2)
            contID = 1;

        int map = 0;
        switch (buttonID) {
            default: break;
            case 0: map = RSDK_controller[contID].keyA.keyMap; break;
            case 1: map = RSDK_controller[contID].keyB.keyMap; break;
            case 2: map = RSDK_controller[contID].keyX.keyMap; break;
            case 3: map = RSDK_controller[contID].keyY.keyMap; break;
            case 4: map = RSDK_controller[contID].keyStart.keyMap; break;
        }

        int frame = 0; // UIButtonPrompt_Unknown2(map);
        RSDK.SetSpriteAnimation(HUD->superButtonMappings, 1, data, true, frame);
    }
}
void HUD_GetSuperFrames()
{
    EntityHUD *entity = (EntityHUD *)RSDK_sceneInfo->entity;
    HUD_GetKeyFrame(&entity->superButtonData1, 3);
    HUD_GetKeyFrame(&entity->superButtonData3, 3);
    HUD_GetKeyFrame(&entity->superButtonData2, 4);
}

void HUD_Unknown5()
{
    EntityHUD *entity = (EntityHUD *)RSDK_sceneInfo->entity;
    Vector2 *ptrs[4];
    void **statePtr = NULL;

    int *ptr = NULL;
    if (options->gameMode == MODE_COMPETITION) {
        statePtr = (void **)&entity->competitionStates[entity->screenID];
        ptrs[0]  = &entity->dwordB4[RSDK_sceneInfo->currentScreenID];
        ptrs[1]  = &entity->dwordD4[RSDK_sceneInfo->currentScreenID];
        ptrs[2] = &entity->dwordF4[RSDK_sceneInfo->currentScreenID];
        ptrs[3] = &entity->dword114[RSDK_sceneInfo->currentScreenID];
        ptr = &entity->field_134[RSDK_sceneInfo->currentScreenID];
    }
    else {
        statePtr = (void **)&entity->state;
        ptrs[0]  = &entity->dword5C[0];
        ptrs[1]  = &entity->dword5C[1];
        ptrs[2]  = &entity->dword5C[2];
        ptrs[3]  = &entity->dword5C[3];
        ptr      = &entity->field_9C;
    }
    if (ptrs[0]->x < *ptr)
        ptrs[0]->x += 0x80000;
    if (ptrs[1]->x < *ptr)
        ptrs[1]->x += 0x80000;
    if (ptrs[2]->x < *ptr)
        ptrs[2]->x += 0x80000;
    if (ptrs[3]->x >= *ptr) {
        *statePtr = NULL;
    }
    else {
        ptrs[3]->x += 0x80000;
    }
}

void HUD_Unknown6()
{
    EntityHUD * entity = (EntityHUD *)RSDK_sceneInfo->entity;
    Vector2 *ptrs[4];
    void **statePtr = NULL;

    if (options->gameMode == MODE_COMPETITION) {
        statePtr = (void **)&entity->competitionStates[entity->screenID];
        ptrs[0]  = &entity->dwordB4[entity->screenID];
        ptrs[1]  = &entity->dwordD4[entity->screenID];
        ptrs[2]  = &entity->dwordF4[entity->screenID];
        ptrs[3]  = &entity->dword114[entity->screenID];
    }
    else {
        statePtr = (void**)&entity->state;
        ptrs[0] = entity->dword5C;
        ptrs[1] = &entity->dword5C[1];
        ptrs[2] = &entity->dword5C[2];
        ptrs[3] = &entity->dword5C[3];
    }
    ptrs[0]->x -= 0x80000;
    if (ptrs[1]->x - ptrs[0]->x > 0x100000)
        ptrs[1]->x -= 0x80000;
    if (ptrs[2]->x - ptrs[1]->x > 0x100000)
        ptrs[2]->x -= 0x80000;
    if (ptrs[3]->x - ptrs[2]->x > 0x100000) {
        ptrs[3]->x = ptrs[3]->x - 0x80000;
    }

    if (ptrs[3]->x < -0x500000) {
        if (options->gameMode == MODE_COMPETITION) {
            *statePtr = NULL;
            //Competition_Unknown4(entity->screenID, 1);
            EntityGameOver *gameOver = (EntityGameOver *)RSDK.GetEntityByID(entity->screenID + Player->playerCount);
            //v9       = Competition->field_28;
            //if (!v9 || *(_DWORD *)(v9 + 108)) {
            //    RSDK.ResetEntityPtr(gameOver, GameOver->objectID, 0);
            //    gameOver->playerID = entity->screenID;
            //}
            //else {
                RSDK.ResetEntityPtr(gameOver, GameOver->objectID, (void *)1);
                RSDK.SetGameMode(ENGINESTATE_FROZEN);
                RSDK_sceneInfo->timeEnabled = false;
                gameOver->playerID          = entity->screenID;
            //}
        }
        else {
            RSDK.ResetEntityPtr(entity, 0, 0);
        }
    }
}

void HUD_EditorDraw()
{

}

void HUD_EditorLoad()
{

}

void HUD_Serialize()
{

}
