#ifndef OBJ_COVERLAY_H
#define OBJ_COVERLAY_H

#include "SonicMania.h"

// Object Class
typedef struct {
    RSDK_OBJECT
    ushort spriteIndex;
} ObjectCOverlay;

// Entity Class
typedef struct {
	RSDK_ENTITY
} EntityCOverlay;

// Object Struct
extern ObjectCOverlay *COverlay;

// Standard Entity Events
void COverlay_Update(void);
void COverlay_LateUpdate(void);
void COverlay_StaticUpdate(void);
void COverlay_Draw(void);
void COverlay_Create(void* data);
void COverlay_StageLoad(void);
void COverlay_EditorDraw(void);
void COverlay_EditorLoad(void);
void COverlay_Serialize(void);

// Extra Entity Functions
void COverlay_DebugDraw(void);
void COverlay_DebugSpawn(void);
void COverlay_DrawTile(void);

#endif //!OBJ_COVERLAY_H