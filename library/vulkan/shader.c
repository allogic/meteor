#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <common/macros.h>

#include <filesystem/fileutil.h>

#include <vulkan/instance.h>
#include <vulkan/shader.h>

void Shader_Alloc(struct xInstance_t* pxInstance, const char* pcVertFilePath, const char* pcFragFilePath, VkShaderModule* pxVertModule, VkShaderModule* pxFragModule) {
	char* pcVertShaderBytes;
	char* pcFragShaderBytes;
	
	uint64_t wVertShaderSize;
	uint64_t wFragShaderSize;

	FileUtil_ReadBinary(&pcVertShaderBytes, &wVertShaderSize, pcVertFilePath);
	FileUtil_ReadBinary(&pcFragShaderBytes, &wFragShaderSize, pcFragFilePath);

	VkShaderModuleCreateInfo xVertCreateInfo;
	memset(&xVertCreateInfo, 0, sizeof(xVertCreateInfo));
	xVertCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	xVertCreateInfo.codeSize = wVertShaderSize;
	xVertCreateInfo.pCode = (const uint32_t*)pcVertShaderBytes;

	VkShaderModuleCreateInfo xFragCreateInfo;
	memset(&xFragCreateInfo, 0, sizeof(xFragCreateInfo));
	xFragCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	xFragCreateInfo.codeSize = wFragShaderSize;
	xFragCreateInfo.pCode = (const uint32_t*)pcFragShaderBytes;

	VK_CHECK(vkCreateShaderModule(Instance_GetDevice(pxInstance), &xVertCreateInfo, 0, pxVertModule));
	VK_CHECK(vkCreateShaderModule(Instance_GetDevice(pxInstance), &xFragCreateInfo, 0, pxFragModule));

    free(pcVertShaderBytes);
	free(pcFragShaderBytes);
}

void Shader_Free(struct xInstance_t* pxInstance, VkShaderModule xVertModule, VkShaderModule xFragModule) {
	vkDestroyShaderModule(Instance_GetDevice(pxInstance), xVertModule, 0);
	vkDestroyShaderModule(Instance_GetDevice(pxInstance), xFragModule, 0);
}
