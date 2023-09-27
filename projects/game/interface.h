#ifndef GAME_INTERFACE_H
#define GAME_INTERFACE_H

struct xRenderer_t;

void Interface_Begin(const char* pcName);
void Interface_End(void);

void Interface_Text(const char* pcString);
void Interface_Button(const char* pcString);

void Interface_Draw(struct xRenderer_t* pxRenderer);

#endif
