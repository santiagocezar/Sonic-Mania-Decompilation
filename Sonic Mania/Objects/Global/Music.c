// ---------------------------------------------------------------------
// RSDK Project: Sonic Mania
// Object Description: Music Object
// Object Author: Christian Whitehead/Simon Thomley/Hunter Bridges
// Decompiled by: Rubberduckycooly & RMGRich
// ---------------------------------------------------------------------

#include "SonicMania.h"

ObjectMusic *Music;

void Music_Update(void)
{
    RSDK_THIS(Music);

    StateMachine_Run(self->state);
}

void Music_LateUpdate(void) {}

void Music_StaticUpdate(void) {}

void Music_Draw(void) {}

void Music_Create(void *data)
{
    RSDK_THIS(Music);

    if (!SceneInfo->inEditor) {
        // if frozen, ACTIVE_ALWAYS, else ACTIVE_NORMAL
        self->active = ACTIVE_ALWAYS + ((SceneInfo->state & 3) != ENGINESTATE_FROZEN);

        if (self->trackFile.length) {
            if (self->trackID != TRACK_NOLOAD) {
                char trackName[0x20];
                RSDK.GetCString(trackName, &self->trackFile);
                Music_SetMusicTrack(trackName, self->trackID, self->trackLoop);

                if (self->playOnLoad)
                    self->state = Music_State_PlayAutoMusic;
                else
                    destroyEntity(self);
            }
        }
    }
}

void Music_StageLoad(void)
{
    // Slot 0: stage music
    Music_SetMusicTrack("Invincible.ogg", TRACK_INVINCIBLE, 139263);
    Music_SetMusicTrack("Sneakers.ogg", TRACK_SNEAKERS, 120960);
    Music_SetMusicTrack("BossMini.ogg", TRACK_MINIBOSS, 276105);
    Music_SetMusicTrack("BossHBH.ogg", TRACK_HBHBOSS, 70560);
    Music_SetMusicTrack("BossEggman1.ogg", TRACK_EGGMAN1, 282240);
    Music_SetMusicTrack("BossEggman2.ogg", TRACK_EGGMAN2, 264600);
    Music_SetMusicTrack("ActClear.ogg", TRACK_ACTCLEAR, false);
    Music_SetMusicTrack("Drowning.ogg", TRACK_DROWNING, false);
    Music_SetMusicTrack("GameOver.ogg", TRACK_GAMEOVER, false);
    Music_SetMusicTrack("Super.ogg", TRACK_SUPER, 165375);
    Music_SetMusicTrack("HBHMischief.ogg", TRACK_HBHMISCHIEF, 381405);
    // Slot 12 (slot 11 in pre-plus): "no load"
    Music_SetMusicTrack("1up.ogg", TRACK_1UP, false);

    if (globals->suppressAutoMusic) {
        if (globals->restartMusicID > TRACK_STAGE)
            Music->activeTrack = globals->restartMusicID;
    }
    else {
        Music->activeTrack = TRACK_STAGE;
    }

    globals->restartMusicID = TRACK_STAGE;
    Music->nextTrack        = TRACK_NONE;

#if MANIA_USE_PLUS
    if (sku_platform == PLATFORM_DEV)
        RSDK.AddViewableVariable("Vape Mode", &globals->vapeMode, VIEWVAR_BOOL, false, true);
#endif
}

void Music_SetMusicTrack(const char *path, uint8 track, uint32 loopPoint)
{
    track &= 0xF;

    int32 charID = 0;
    for (; charID < 0x20; ++charID) {
        if (!path[charID])
            break;
        else
            Music->trackNames[track][charID] = path[charID];
    }
    Music->trackNames[track][charID] = 0;
    Music->trackLoops[track]         = loopPoint;
}

void Music_State_PlayAutoMusic(void)
{
    RSDK_THIS(Music);

    if (globals->suppressAutoMusic) {
        globals->suppressAutoMusic = false;

#if MANIA_USE_PLUS
        switch (Music->activeTrack) {
            case TRACK_INVINCIBLE:
            case TRACK_SNEAKERS:
            case TRACK_1UP: Music_TransitionTrack(self->trackID, 0.025); break;
            case TRACK_SUPER: Music_PlayAutoMusicQueuedTrack(Music->activeTrack); break;
            default: break;
        }
#endif
    }
    else {
        Music_PlayTrack(self->trackID);
    }

    destroyEntity(self);
}

void Music_PlayQueuedTrack(uint8 trackID)
{
    trackID &= 0xF;

#if MANIA_USE_PLUS
    // remove any existing vers of this on the stack
    for (int32 slot = SLOT_MUSICSTACK_START; slot < SLOT_MUSICSTACK_END; ++slot) {
        EntityMusic *music = RSDK_GET_ENTITY(slot, Music);
        if (music->classID == Music->classID && music->trackID == trackID) {
            destroyEntity(music);
        }
    }

    EntityMusic *entity = NULL;
    // get the first empty stack slot
    for (int32 slot = SLOT_MUSICSTACK_START; slot < SLOT_MUSICSTACK_END; ++slot) {
        entity = RSDK_GET_ENTITY(slot, Music);
        if (entity->classID != Music->classID) {
            RSDK.ResetEntityPtr(entity, Music->classID, NULL);
            break;
        }
    }

    entity->trackID       = trackID;
    entity->state         = Music_State_HandleQueuedTrack;
    entity->trackPriority = TRACK_PRIORITY_NONE;
    entity->fadeSpeed     = 0.08;

    switch (trackID) {
        case TRACK_INVINCIBLE:
        case TRACK_SNEAKERS:
            Music_HandleMusicStack_Powerups(entity);
            entity->trackPriority = TRACK_PRIORITY_SUPER;
            entity->restartTrack  = true;
            break;

        case TRACK_DROWNING:
            entity->timer         = 960;
            entity->trackPriority = TRACK_PRIORITY_DROWN;
            entity->restartTrack  = true;
            break;

        case TRACK_SUPER:
            entity->trackPriority = TRACK_PRIORITY_SUPER;
            entity->restartTrack  = true;
            break;

        case TRACK_1UP:
            entity->timer         = 224;
            entity->trackPriority = TRACK_PRIORITY_1UP;
            break;

        default: break;
    }

    Music_GetNextTrackStartPos(entity);

    for (int32 slot = SLOT_MUSICSTACK_START; slot < SLOT_MUSICSTACK_END; ++slot) {
        EntityMusic *music = RSDK_GET_ENTITY(slot, Music);
        if (music->classID == Music->classID && music != entity) {
            if (music->trackPriority > entity->trackPriority) {
                entity->volume = 0.0;
                return;
            }
        }
    }

    RSDK.StopChannel(Music->channelID);
    Music->activeTrack = trackID;
    Music->channelID   = RSDK.PlayStream(Music->trackNames[Music->activeTrack], 0, 0, Music->trackLoops[Music->activeTrack], true);

#if MANIA_USE_PLUS
    if (globals->vapeMode)
        RSDK.SetChannelAttributes(Music->channelID, 1.0, 0.0, 0.75);
#endif

    entity->volume = 1.0;
#else

    switch (trackID) {
        case TRACK_1UP: {
            Music->playing1UPTrack = true;

            EntityMusic *music = RSDK_GET_ENTITY(SLOT_MUSIC, Music);
            RSDK.ResetEntityPtr(music, Music->classID, NULL);
            music->state     = Music_State_1UPJingle;
            music->volume    = 0.0;
            music->fadeSpeed = 0.08;

            bool32 active = RSDK.ChannelActive(Music->channelID);
            if (Music->activeTrack != TRACK_1UP) {
                Music->trackStartPos = RSDK.GetChannelPos(Music->channelID);
                Music->queuedTrack   = Music->activeTrack;
            }

            RSDK.StopChannel(Music->channelID);
            Music->activeTrack = TRACK_1UP;
            Music->channelID   = RSDK.PlayStream(Music->trackNames[Music->activeTrack], 0, 0, Music->trackLoops[Music->activeTrack], true);
            break;
        }

        default:
            if (Music->playing1UPTrack || Music->playingDrownTrack) {
                Music->nextTrack = trackID;
            }
            else {
                destroyEntitySlot(SLOT_MUSIC);
                RSDK.StopChannel(Music->channelID);

                if (!Music->playingRegularTrack)
                    Music->nextTrack = Music->activeTrack;

                Music->playingRegularTrack = true;

                Music->nextTrack     = TRACK_NONE;
                Music->activeTrack   = trackID;
                Music->trackStartPos = 0;
                Music->channelID     = RSDK.PlayStream(Music->trackNames[Music->activeTrack], 0, 0, Music->trackLoops[Music->activeTrack], true);
            }
            break;
    }
#endif
}

void Music_PlayTrack(uint8 trackID)
{
    trackID &= 0xF;

#if MANIA_USE_PLUS
    if (trackID != TRACK_ACTCLEAR && Music_CheckMusicStack_Active()) {
        Music->nextTrack = trackID;
    }
    else {
        if (trackID == TRACK_ACTCLEAR)
            Music_ClearMusicStack();

        destroyEntitySlot(SLOT_MUSIC);
        RSDK.StopChannel(Music->channelID);

        Music->activeTrack   = trackID;
        Music->trackStartPos = 0;
        Music->channelID     = RSDK.PlayStream(Music->trackNames[trackID], 0, 0, Music->trackLoops[trackID], true);

        if (globals->vapeMode)
            RSDK.SetChannelAttributes(Music->channelID, 1.0, 0.0, 0.75);
    }
#else
    if (Music->playing1UPTrack || Music->playingDrownTrack) {
        Music->nextTrack = trackID;
    }
    else {
        EntityMusic *music = RSDK_GET_ENTITY(SLOT_MUSIC, Music);
        RSDK.ResetEntityPtr(music, Music->classID, NULL);

        if (!Music->playingRegularTrack)
            Music->prevTrack = Music->activeTrack;

        bool32 regularTrack = true;
        switch (trackID) {
            default: break;

            case TRACK_INVINCIBLE:
            case TRACK_SNEAKERS:
            case TRACK_SUPER:
            case TRACK_NOLOAD:
            case TRACK_HBHMISCHIEF:
            case TRACK_1UP: regularTrack = false; break;
        }

        if (regularTrack)
            Music->playingRegularTrack = true;

        Music->nextTrack     = TRACK_NONE;
        Music->activeTrack   = trackID;
        Music->trackStartPos = 0;
        Music->channelID     = RSDK.PlayStream(Music->trackNames[trackID], 0, 0, Music->trackLoops[trackID], true);
    }
#endif
}

void Music_PlayTrackPtr(EntityMusic *entity)
{
    RSDK.GetCString(Music->trackNames[0], &entity->trackFile);
    Music->trackLoops[0] = entity->trackLoop;

    destroyEntitySlot(SLOT_MUSIC);
    RSDK.StopChannel(Music->channelID);

    Music->activeTrack   = 0;
    Music->trackStartPos = 0;
    Music->channelID     = RSDK.PlayStream(Music->trackNames[0], 0, 0, Music->trackLoops[0], true);

#if MANIA_USE_PLUS
    if (globals->vapeMode)
        RSDK.SetChannelAttributes(Music->channelID, 1.0, 0.0, 0.75);
#endif
}

#if MANIA_USE_PLUS
void Music_PlayAutoMusicQueuedTrack(uint8 trackID)
{
    trackID &= 0xF;
    Music->nextTrack = TRACK_STAGE;

    // remove any existing vers of this on the stack
    for (int32 slot = SLOT_MUSICSTACK_START; slot < SLOT_MUSICSTACK_END; ++slot) {
        EntityMusic *music = RSDK_GET_ENTITY(slot, Music);
        if (music->classID == Music->classID && music->trackID == trackID) {
            destroyEntity(music);
        }
    }

    EntityMusic *entity = NULL;
    // get the first empty stack slot
    for (int32 slot = SLOT_MUSICSTACK_START; slot < SLOT_MUSICSTACK_END; ++slot) {
        entity = RSDK_GET_ENTITY(slot, Music);
        if (entity->classID != Music->classID) {
            destroyEntity(entity);
            break;
        }
    }

    entity->trackID       = trackID;
    entity->state         = Music_State_HandleQueuedTrack;
    entity->trackPriority = TRACK_PRIORITY_NONE;
    entity->volume        = 1.0;
    entity->fadeSpeed     = 0.08;

    switch (trackID) {
        case TRACK_INVINCIBLE:
        case TRACK_SNEAKERS:
            Music_HandleMusicStack_Powerups(entity);
            entity->trackPriority = TRACK_PRIORITY_POWERUP;
            entity->restartTrack  = true;
            break;

        case TRACK_DROWNING:
            entity->timer         = 960;
            entity->trackPriority = TRACK_PRIORITY_DROWN;
            entity->restartTrack  = true;
            break;

        case TRACK_SUPER:
            entity->trackPriority = TRACK_PRIORITY_SUPER;
            entity->restartTrack  = true;
            break;

        case TRACK_1UP:
            entity->timer         = 224;
            entity->trackPriority = TRACK_PRIORITY_1UP;
            break;

        default: break;
    }
}

void Music_HandleMusicStack_Powerups(EntityMusic *entity)
{
    for (int32 slot = SLOT_MUSICSTACK_START; slot < SLOT_MUSICSTACK_END; ++slot) {
        EntityMusic *music = RSDK_GET_ENTITY(slot, Music);
        if (music->classID == Music->classID && music->trackPriority == TRACK_PRIORITY_POWERUP && music->trackID != entity->trackID
            && music != entity)
            music->trackPriority = TRACK_PRIORITY_ANY;
    }
}

bool32 Music_CheckMusicStack_Active(void)
{
    bool32 active = false;

    for (int32 slot = SLOT_MUSICSTACK_START; slot < SLOT_MUSICSTACK_END; ++slot) {
        EntityMusic *music = RSDK_GET_ENTITY(slot, Music);
        if (music->classID == Music->classID && music->trackPriority > TRACK_PRIORITY_NONE)
            active = true;
    }

    return active;
}

void Music_GetNextTrackStartPos(EntityMusic *entity)
{
    int32 stackCount = 0;

    for (int32 slot = SLOT_MUSICSTACK_START; slot < SLOT_MUSICSTACK_END; ++slot) {
        EntityMusic *music = RSDK_GET_ENTITY(slot, Music);
        if (music->classID == Music->classID && entity != music) {
            if (music->trackID == Music->activeTrack) {
                entity->trackStartPos = RSDK.GetChannelPos(Music->channelID);
                entity->volume        = 0.0;
            }
            stackCount++;
        }
    }

    if (!stackCount) {
        if (Music->activeTrack > TRACK_NONE) {
            if (Music->nextTrack == TRACK_NONE) {
                Music->nextTrack = Music->activeTrack;
            }
            Music->trackStartPos = RSDK.GetChannelPos(Music->channelID);
        }
    }
}
#endif

void Music_EndQueuedTrack(uint8 trackID, bool32 transitionFade)
{
    trackID &= 0xF;

#if MANIA_USE_PLUS
    for (int32 slot = SLOT_MUSICSTACK_START; slot < SLOT_MUSICSTACK_END; ++slot) {
        EntityMusic *music = RSDK_GET_ENTITY(slot, Music);
        if (music->classID == Music->classID && music->trackID == trackID) {
            if (transitionFade) {
                music->state     = Music_State_FadeTrackOut;
                music->fadeSpeed = 0.05;
            }
            else {
                music->timer = 1;
            }
        }
    }
#else
    switch (trackID) {
        default: break;

        case TRACK_INVINCIBLE:
        case TRACK_SNEAKERS:
        case TRACK_SUPER:
            Music->playingRegularTrack = false;

            if (Music->nextTrack == TRACK_NONE)
                Music_TransitionTrack(trackID, 0.025);
            break;

        case TRACK_DROWNING:
            if (Music->playingDrownTrack) {
                Music->playingDrownTrack = false;

                if (Music->playing1UPTrack) {
                    if (Music->nextTrack == TRACK_NONE) {
                        Music->queuedTrack = Music->prevTrack == TRACK_NONE ? TRACK_STAGE : Music->prevTrack;
                    }
                    else {
                        Music->queuedTrack = Music->nextTrack;
                        Music->nextTrack   = TRACK_NONE;
                    }
                }
                else {
                    if (Music->nextTrack == TRACK_NONE) {
                        Music->activeTrack = Music->activeTrack == TRACK_NONE ? TRACK_STAGE : Music->prevTrack;
                    }
                    else {
                        Music->activeTrack = Music->nextTrack;
                        Music->nextTrack   = TRACK_NONE;
                    }

                    RSDK.StopChannel(Music->channelID);
                    Music->channelID = RSDK.PlayStream(Music->trackNames[Music->activeTrack], 0, 0, Music->trackLoops[Music->activeTrack], true);
                }
            }
            break;
    }
#endif
}

#if MANIA_USE_PLUS
void Music_HandleMusicStackTrackRemoval(EntityMusic *entity)
{
    if (entity) {
        EntityMusic *music = RSDK_GET_ENTITY(SLOT_MUSIC, Music);
        if (music->classID != Music->classID || music->state != Music_State_TransitionTrack) {
            destroyEntity(music);

            // remove all of these buggers that have higher priority and thus wont be played
            for (int32 slot = SLOT_MUSICSTACK_START; slot < SLOT_MUSICSTACK_END; ++slot) {
                EntityMusic *stack = RSDK_GET_ENTITY(slot, Music);
                if (stack->classID == Music->classID && stack->trackPriority > entity->trackPriority) {
                    destroyEntity(entity);
                    return;
                }
            }

            bool32 restartTrack = entity->restartTrack;
            destroyEntity(entity);
            int32 priority = 0;

            EntityMusic *trackPtr = NULL;
            // the next track to be played will be the track with the highest priority on the stack (may be none)
            for (int32 slot = SLOT_MUSICSTACK_START; slot < SLOT_MUSICSTACK_END; ++slot) {
                EntityMusic *stack = RSDK_GET_ENTITY(slot, Music);
                if (stack->classID == Music->classID && stack->trackPriority > priority) {
                    trackPtr = stack;
                    priority = stack->trackPriority;
                }
            }

            RSDK.StopChannel(Music->channelID);
            if (trackPtr) { // another track is on the music stack still
                if (trackPtr->trackID == Music->activeTrack) {
                    trackPtr->trackStartPos = 0;
                }
                else {
                    Music->activeTrack = trackPtr->trackID;
                    if (restartTrack)
                        trackPtr->trackStartPos = 0;
                    Music->channelID = RSDK.PlayStream(Music->trackNames[Music->activeTrack], 0, trackPtr->trackStartPos,
                                                       Music->trackLoops[Music->activeTrack], true);
                    if (trackPtr->trackStartPos) {
                        RSDK.SetChannelAttributes(Music->channelID, 0.0, 0.0, globals->vapeMode ? 0.75 : 1.0);
                    }
                    else if (globals->vapeMode) {
                        RSDK.SetChannelAttributes(Music->channelID, 1.0, 0.0, 0.75);
                    }
                }
            }
            else if (Music->nextTrack > TRACK_NONE) { // next track is queued
                Music->activeTrack = Music->nextTrack;
                Music->nextTrack   = TRACK_NONE;

                if (restartTrack)
                    Music->trackStartPos = 0;

                Music->channelID =
                    RSDK.PlayStream(Music->trackNames[Music->activeTrack], 0, Music->trackStartPos, Music->trackLoops[Music->activeTrack], true);

                if (Music->trackStartPos) {
                    RSDK.SetChannelAttributes(Music->channelID, 0.0, 0.0, globals->vapeMode ? 0.75 : 1.0);
                    music = RSDK_GET_ENTITY(SLOT_MUSIC, Music);
                    RSDK.ResetEntityPtr(music, Music->classID, NULL);
                    music->state     = Music_State_FadeTrackIn;
                    music->volume    = 0.0;
                    music->fadeSpeed = 0.08;
                }
                else if (globals->vapeMode) {
                    RSDK.SetChannelAttributes(Music->channelID, 1.0, 0.0, 0.75);
                }
            }
        }
        else {
            destroyEntity(entity);
        }
    }
}

void Music_ClearMusicStack(void)
{
    for (int32 slot = SLOT_MUSICSTACK_START; slot < SLOT_MUSICSTACK_END; ++slot) destroyEntity(RSDK_GET_ENTITY(slot, Music));
}
#endif

void Music_TransitionTrack(uint8 trackID, float fadeSpeed)
{
    trackID &= 0xF;

    EntityMusic *music = NULL;
#if MANIA_USE_PLUS
    switch (trackID) {
        case TRACK_ACTCLEAR:
            Music_ClearMusicStack();
            // [Fallthrough]

        case TRACK_MINIBOSS:
        case TRACK_HBHBOSS:
        case TRACK_EGGMAN1:
        case TRACK_EGGMAN2:
        case TRACK_HBHMISCHIEF:
            music            = RSDK_GET_ENTITY(SLOT_MUSIC, Music);
            Music->nextTrack = trackID;
            break;

        default:
            if (Music_CheckMusicStack_Active()) {
                Music->nextTrack = trackID;
            }
            else {
                music            = RSDK_GET_ENTITY(SLOT_MUSIC, Music);
                Music->nextTrack = trackID;
            }
            break;
    }
#else
    if (Music->playing1UPTrack || Music->playingDrownTrack) {
        Music->nextTrack = Music->prevTrack;
    }
    else if (Music->activeTrack == trackID) {
        Music->nextTrack = Music->prevTrack & 0xF;
        music            = RSDK_GET_ENTITY(SLOT_MUSIC, Music);
    }
#endif

    if (music && (music->classID != Music->classID || music->state != Music_State_TransitionTrack)) {
        RSDK.ResetEntityPtr(music, Music->classID, NULL);
        music->state     = Music_State_TransitionTrack;
        music->volume    = 1.0;
        music->fadeSpeed = fadeSpeed;
    }
}

void Music_FadeOut(float fadeSpeed)
{
    if (Music->activeTrack != TRACK_DROWNING) {
        EntityMusic *music = RSDK_GET_ENTITY(SLOT_MUSIC, Music);
        RSDK.ResetEntityPtr(music, Music->classID, NULL);
        music->state     = Music_State_FadeOut;
        music->volume    = 1.0;
        music->fadeSpeed = fadeSpeed;
    }
}

#if MANIA_USE_PLUS
void Music_State_HandleQueuedTrack(void)
{
    RSDK_THIS(Music);

    if (Music->activeTrack == self->trackID && RSDK.GetChannelPos(Music->channelID) > self->trackStartPos) {
        self->trackStartPos = 0;
        if (self->volume < 1.0) {
            self->volume += self->fadeSpeed;
            RSDK.SetChannelAttributes(Music->channelID, self->volume, 0.0, 1.0);
            if (self->volume >= 1.0)
                self->volume = 1.0;
        }
    }

    if (self->timer > 0) {
        if (!--self->timer)
            Music_HandleMusicStackTrackRemoval(self);
    }
}
void Music_State_FadeTrackOut(void)
{
    RSDK_THIS(Music);

    if (self->volume > -0.5) {
        self->volume -= self->fadeSpeed;

        if (Music->activeTrack == self->trackID)
            RSDK.SetChannelAttributes(Music->channelID, self->volume, 0.0, 1.0);

        if (self->volume <= -0.5)
            Music_HandleMusicStackTrackRemoval(self);
    }
}
#endif

void Music_State_FadeTrackIn(void)
{
    RSDK_THIS(Music);

#if MANIA_USE_PLUS
    if (RSDK.GetChannelPos(Music->channelID) > Music->trackStartPos) {
        Music->trackStartPos = 0;
        self->volume += self->fadeSpeed;

        RSDK.SetChannelAttributes(Music->channelID, self->volume, 0.0, 1.0);
        if (self->volume >= 1.0) {
            self->volume = 1.0;
            destroyEntity(self);
        }
    }
#else
    self->volume += self->fadeSpeed;
    RSDK.SetChannelAttributes(Music->channelID, self->volume, 0.0, 1.0);
    if (self->volume >= 1.0) {
        self->volume = 1.0;
        destroyEntity(self);
    }
#endif
}
void Music_State_FadeOut(void)
{
    RSDK_THIS(Music);

    self->volume -= self->fadeSpeed;
    RSDK.SetChannelAttributes(Music->channelID, self->volume, 0.0, 1.0);

    if (self->volume < -0.5) {
        RSDK.StopChannel(Music->channelID);
        destroyEntity(self);
    }
}

void Music_State_TransitionTrack(void)
{
    RSDK_THIS(Music);

    self->volume -= self->fadeSpeed;
    RSDK.SetChannelAttributes(Music->channelID, self->volume, 0.0, 1.0);

    if (self->volume < -0.5) {
#if MANIA_USE_PLUS
        RSDK.StopChannel(Music->channelID);

        Music->activeTrack   = Music->nextTrack;
        Music->trackStartPos = 0;
        Music->channelID     = RSDK.PlayStream(Music->trackNames[Music->activeTrack], 0, 0, Music->trackLoops[Music->activeTrack], true);

        if (globals->vapeMode)
            RSDK.SetChannelAttributes(Music->channelID, 1.0, 0.0, 0.75);
#else
        int32 trackID = Music->nextTrack & 0xF;
        if (Music->playing1UPTrack || Music->playingDrownTrack) {
            Music->nextTrack = trackID;
        }
        else {
            destroyEntitySlot(SLOT_MUSIC);
            RSDK.StopChannel(Music->channelID);

            if (!Music->playingRegularTrack)
                Music->prevTrack = Music->activeTrack;

            bool32 regularTrack = true;
            switch (trackID) {
                default: break;

                case TRACK_INVINCIBLE:
                case TRACK_SNEAKERS:
                case TRACK_SUPER:
                case TRACK_NOLOAD:
                case TRACK_HBHMISCHIEF:
                case TRACK_1UP: regularTrack = false; break;
            }

            if (regularTrack)
                Music->playingRegularTrack = true;

            Music->nextTrack     = TRACK_NONE;
            Music->activeTrack   = trackID;
            Music->trackStartPos = 0;
            Music->channelID     = RSDK.PlayStream(Music->trackNames[Music->activeTrack], 0, 0, Music->trackLoops[Music->activeTrack], true);
        }
#endif

        Music->nextTrack = TRACK_NONE;
        destroyEntity(self);
    }
}

#if !MANIA_USE_PLUS
void Music_State_1UPJingle(void)
{
    RSDK_THIS(Music);

    self->timer++;
    if (self->timer == 224) {
        RSDK.StopChannel(Music->channelID);

        if (Music->nextTrack == TRACK_NONE) {
            Music->activeTrack = Music->queuedTrack;
        }
        else {
            Music->activeTrack   = Music->nextTrack;
            Music->trackStartPos = 0;
            Music->nextTrack     = TRACK_NONE;
        }

        if (Music->playingDrownTrack) {
            if (Music->activeTrack != TRACK_DROWNING)
                Music->trackStartPos = 0;

            Music->activeTrack = TRACK_DROWNING;
        }

        Music->playing1UPTrack = false;
        Music->channelID       = RSDK.PlayStream(Music->trackNames[Music->activeTrack], 0, 0, Music->trackLoops[Music->activeTrack], true);

        if (Music->trackStartPos)
            RSDK.SetChannelAttributes(Music->channelID, self->volume, 0.0, 1.0);
        else
            destroyEntity(self);
    }
    else if (self->timer > 224) {
        if (RSDK.GetChannelPos(Music->channelID) > Music->trackStartPos) {
            self->timer          = 0;
            Music->trackStartPos = 0;
            self->state          = Music_State_FadeTrackIn;
        }
    }
}
#endif

#if RETRO_INCLUDE_EDITOR
void Music_EditorDraw(void)
{
    RSDK_THIS(Music);

    RSDK.SetSpriteAnimation(Music->aniFrames, 0, &self->animator, true, 1);
    RSDK.DrawSprite(&self->animator, NULL, false);
}

void Music_EditorLoad(void) { Music->aniFrames = RSDK.LoadSpriteAnimation("Editor/EditorIcons.bin", SCOPE_STAGE); }
#endif

void Music_Serialize(void)
{
    RSDK_EDITABLE_VAR(Music, VAR_STRING, trackFile);
    RSDK_EDITABLE_VAR(Music, VAR_STRING, soundTestTitle);
    RSDK_EDITABLE_VAR(Music, VAR_ENUM, trackID);
    RSDK_EDITABLE_VAR(Music, VAR_UINT32, trackLoop);
    RSDK_EDITABLE_VAR(Music, VAR_BOOL, playOnLoad);
}
