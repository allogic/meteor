#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <common/macros.h>

#include <filesystem/fileutil.h>

#include <vulkan/instance.h>
#include <vulkan/shader.h>

VkShaderModule Shader_Alloc(struct xInstance_t* pxInstance, const char* pcFilePath) {
	VkShaderModule xShaderModule;

	uint8_t* pcShaderBytes;	
	uint64_t wShaderSize;

	FileUtil_ReadBinary(&pcShaderBytes, &wShaderSize, pcFilePath);

	VkShaderModuleCreateInfo xShaderModuleCreateInfo;
	memset(&xShaderModuleCreateInfo, 0, sizeof(xShaderModuleCreateInfo));
	xShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	xShaderModuleCreateInfo.codeSize = wShaderSize;
	xShaderModuleCreateInfo.pCode = (const uint32_t*)pcShaderBytes;

	VK_CHECK(vkCreateShaderModule(Instance_GetDevice(pxInstance), &xShaderModuleCreateInfo, 0, &xShaderModule));

    free(pcShaderBytes);

	return xShaderModule;
}

void Shader_Free(struct xInstance_t* pxInstance, VkShaderModule xShaderModule) {
	vkDestroyShaderModule(Instance_GetDevice(pxInstance), xShaderModule, 0);
}
