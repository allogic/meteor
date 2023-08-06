#ifndef SHADER_H
#define SHADER_H

uint32_t Shader_AllocDisplay(const char* pcVertexFilePath, const char* pcFragmentFilePath);
uint32_t Shader_AllocCompute(const char* pcComputeFilePath);

void Shader_Free(uint32_t nProgramId);

#endif
