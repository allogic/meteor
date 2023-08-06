#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "shader.h"
#include "fileutl.h"

#include "glad.h"

static void Shader_CheckCompileStatus(uint32_t nShaderId) {
    int32_t nCompileStatus;
    int32_t nInfoLength;

    glGetShaderiv(nShaderId, GL_COMPILE_STATUS, &nCompileStatus);

    if (nCompileStatus == 0) {
        glGetShaderiv(nShaderId, GL_INFO_LOG_LENGTH, &nInfoLength);

        if (nInfoLength > 0) {
            char* pcInfo = (char*)malloc(nInfoLength);

            glGetShaderInfoLog(nShaderId, nInfoLength, &nInfoLength, pcInfo);

            printf("%s\n", pcInfo);

            free(pcInfo);
        }
    }
}

static void Shader_CheckLinkStatus(uint32_t nProgramId) {
    int32_t nLinkStatus;
    int32_t nInfoLength;

    glGetProgramiv(nProgramId, GL_LINK_STATUS, &nLinkStatus);

    if (nLinkStatus == 0) {
        glGetProgramiv(nProgramId, GL_INFO_LOG_LENGTH, &nInfoLength);

        if (nInfoLength > 0) {
            char* pcInfo = (char*)malloc(nInfoLength);

            glGetProgramInfoLog(nProgramId, nInfoLength, &nInfoLength, pcInfo);

            printf("%s\n", pcInfo);

            free(pcInfo);
        }
    }
}

uint32_t Shader_AllocDisplay(const char* pcVertexFilePath, const char* pcFragmentFilePath) {
    uint32_t nProgramId = glCreateProgram();
    uint32_t nVertexId = glCreateShader(GL_VERTEX_SHADER);
    uint32_t nFragmentId = glCreateShader(GL_FRAGMENT_SHADER);

    char* pcVertexSource;
    char* pcFragmentSource;

    uint32_t nVertexLength;
    uint32_t nFragmentLength;

    FileUtl_ReadText(&pcVertexSource, &nVertexLength, pcVertexFilePath);
    FileUtl_ReadText(&pcFragmentSource, &nFragmentLength, pcFragmentFilePath);

    char const* pcVertexShaderPtr = pcVertexSource;
    char const* pcFragmentShaderPtr = pcFragmentSource;
    
    glShaderSource(nVertexId, 1, &pcVertexShaderPtr, 0);
    glCompileShader(nVertexId);
    Shader_CheckCompileStatus(nVertexId);

    glShaderSource(nFragmentId, 1, &pcFragmentShaderPtr, 0);
    glCompileShader(nFragmentId);
    Shader_CheckCompileStatus(nFragmentId);

    glAttachShader(nProgramId, nVertexId);
    glAttachShader(nProgramId, nFragmentId);
    glLinkProgram(nProgramId);
    Shader_CheckLinkStatus(nProgramId);

    glDeleteShader(nVertexId);
    glDeleteShader(nFragmentId);

    return nProgramId;
}

uint32_t Shader_AllocCompute(const char* pcComputeFilePath) {
    uint32_t nProgramId = glCreateProgram();
    uint32_t nComputeId = glCreateShader(GL_COMPUTE_SHADER);

    char* pcComputeSource;

    uint32_t nComputeLength;

    FileUtl_ReadText(&pcComputeSource, &nComputeLength, pcComputeFilePath);

    char const* pcComputeShaderPtr = pcComputeSource;

    glShaderSource(nComputeId, 1, &pcComputeShaderPtr, 0);
    glCompileShader(nComputeId);
    Shader_CheckCompileStatus(nComputeId);

    glAttachShader(nProgramId, nComputeId);
    glLinkProgram(nProgramId);
    Shader_CheckLinkStatus(nProgramId);

    glDeleteShader(nComputeId);

    return nProgramId;
}

void Shader_Free(uint32_t nProgramId) {
    glDeleteProgram(nProgramId);
}
