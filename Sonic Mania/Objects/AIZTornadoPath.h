#ifndef OBJ_AIZTORNADOPATH_H
#define OBJ_AIZTORNADOPATH_H

#include "../SonicMania.h"

// Object Class
typedef struct {
	RSDK_OBJECT
} ObjectAIZTornadoPath;

// Entity Class
typedef struct {
	RSDK_ENTITY
} EntityAIZTornadoPath;

// Object Struct
extern ObjectAIZTornadoPath *AIZTornadoPath;

// Standard Entity Events
void AIZTornadoPath_Update();
void AIZTornadoPath_LateUpdate();
void AIZTornadoPath_StaticUpdate();
void AIZTornadoPath_Draw();
void AIZTornadoPath_Create(void* data);
void AIZTornadoPath_StageLoad();
void AIZTornadoPath_EditorDraw();
void AIZTornadoPath_EditorLoad();
void AIZTornadoPath_Serialize();

// Extra Entity Functions


#endif //!OBJ_AIZTORNADOPATH_H