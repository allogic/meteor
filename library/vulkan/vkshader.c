#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <common/macros.h>

#include <filesystem/fileutil.h>

#include <vulkan/vkinstance.h>
#include <vulkan/vkshader.h>

void VkShader_Alloc(struct xVkInstance_t* pxVkInstance, const char* pcVertFilePath, const char* pcFragFilePath, VkShaderModule* pxVertModule, VkShaderModule* pxFragModule) {
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

	VK_CHECK(vkCreateShaderModule(VkInstance_GetDevice(pxVkInstance), &xVertCreateInfo, 0, pxVertModule));
	VK_CHECK(vkCreateShaderModule(VkInstance_GetDevice(pxVkInstance), &xFragCreateInfo, 0, pxFragModule));

    free(pcVertShaderBytes);
	free(pcFragShaderBytes);
}

void VkShader_Free(struct xVkInstance_t* pxVkInstance, VkShaderModule xVertModule, VkShaderModule xFragModule) {
	vkDestroyShaderModule(VkInstance_GetDevice(pxVkInstance), xVertModule, 0);
	vkDestroyShaderModule(VkInstance_GetDevice(pxVkInstance), xFragModule, 0);
}
