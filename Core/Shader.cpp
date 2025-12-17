#include "SckVK_Shader.h"
#include "glslang/Include/glslang_c_interface.h"
#include "glslang/Public/resource_limits_c.h"
#include "glslang/Include/glslang_c_shader_types.h"
#include <vector>

namespace sckVK
{
	struct Shader
	{
		std::vector<uint32_t> spirv;
		VkShaderModule shaderModule = nullptr;

		void Init(glslang_program_t* program)
		{
			size_t size = glslang_program_SPIRV_get_size(program);
			spirv.resize(size);
			glslang_program_SPIRV_get(program, spirv.data());
		}
	};

	static glslang_stage_t ShaderStageFromFileName(const char* fileName)
	{
		std::string s(fileName);

		if (s.ends_with(".vert"))
		{
			return GLSLANG_STAGE_VERTEX;
		}
		else if (s.ends_with(".tesc"))
		{
			return GLSLANG_STAGE_TESSCONTROL;
		}
		else if (s.ends_with(".tese"))
		{
			return GLSLANG_STAGE_TESSEVALUATION;
		}
		else if (s.ends_with(".geom"))
		{
			return GLSLANG_STAGE_GEOMETRY;
		}
		else if (s.ends_with(".frag"))
		{
			return GLSLANG_STAGE_FRAGMENT;
		}
		else if (s.ends_with(".comp"))
		{
			return GLSLANG_STAGE_COMPUTE;
		}

		printf("Unknown shader stage in %s\n", fileName);
		exit(1);

		return GLSLANG_STAGE_VERTEX;
	}

	static bool CompileShader(VkDevice device, glslang_stage_t stage, const char* shaderCode, Shader& shaderModule)
	{
        glslang_input_t input = {
        .language = GLSLANG_SOURCE_GLSL,
        .stage = stage,
        .client = GLSLANG_CLIENT_VULKAN,
        .client_version = GLSLANG_TARGET_VULKAN_1_2,
        .target_language = GLSLANG_TARGET_SPV,
        .target_language_version = GLSLANG_TARGET_SPV_1_5,
        .code = shaderCode,
        .default_version = 100,
        .default_profile = GLSLANG_NO_PROFILE,
        .force_default_version_and_profile = false,
        .forward_compatible = false,
        .messages = GLSLANG_MSG_DEFAULT_BIT,
        .resource = glslang_default_resource(),
        };

        glslang_shader_t* shader = glslang_shader_create(&input);

        if (!glslang_shader_preprocess(shader, &input)) {
            printf("GLSL preprocessing failed\n");
            printf("%s\n", glslang_shader_get_info_log(shader));
            printf("%s\n", glslang_shader_get_info_debug_log(shader));
            printf("%s\n", input.code);
            glslang_shader_delete(shader);
            return 0;
        }

        if (!glslang_shader_parse(shader, &input)) {
            printf("GLSL parsing failed\n");
            printf("%s\n", glslang_shader_get_info_log(shader));
            printf("%s\n", glslang_shader_get_info_debug_log(shader));
            printf("%s\n", glslang_shader_get_preprocessed_code(shader));
            glslang_shader_delete(shader);
            return 0;
        }

        glslang_program_t* program = glslang_program_create();
        glslang_program_add_shader(program, shader);

        if (!glslang_program_link(program, GLSLANG_MSG_SPV_RULES_BIT | GLSLANG_MSG_VULKAN_RULES_BIT)) {
            printf("GLSL linking failed\n");
            printf("%s\n", glslang_program_get_info_log(program));
            printf("%s\n", glslang_program_get_info_debug_log(program));
            glslang_program_delete(program);
            glslang_shader_delete(shader);
            return 0;
        }

        glslang_program_SPIRV_generate(program, stage);

		shaderModule.Init(program);

        const char* spirv_messages = glslang_program_SPIRV_get_messages(program);
        if (spirv_messages)
            printf("%s\b", spirv_messages);

		VkShaderModuleCreateInfo shaderModuleCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.codeSize = shaderModule.spirv.size() * sizeof(uint32_t),
			.pCode = (const uint32_t*)shaderModule.spirv.data()
		};

		VkResult res = vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &shaderModule.shaderModule);
		CHECK_VK_RESULT(res, "vkCreateShaderModule error\n");

        glslang_program_delete(program);
        glslang_shader_delete(shader);

		bool ret = shaderModule.spirv.size() > 0;

        return ret;
	}

	VkShaderModule CreateShaderModuleFromText(VkDevice device, const char* fileName)
	{
		std::string source;

		if (!ReadFile(fileName, source))
		{
			exit(1);
		}

		Shader shaderModule;

		glslang_stage_t shaderStage = ShaderStageFromFileName(fileName);

		VkShaderModule ret = nullptr;

		glslang_initialize_process();

		bool success = CompileShader(device, shaderStage, source.c_str(), shaderModule);

		if (success)
		{
			printf("Shader created from file '%s'\n", fileName);
			ret = shaderModule.shaderModule;
			std::string binaryFileName = std::string(fileName) + ".spv";
			WriteBinaryFile(binaryFileName.c_str(), shaderModule.spirv.data(), (int)shaderModule.spirv.size() * sizeof(uint32_t));
		}

		glslang_finalize_process();

		return ret;
	}

	VkShaderModule CreateShaderModuleFromBinary(VkDevice device, const char* fileName)
	{
		int codeSize = 0;
		char* binaryCode = ReadBinaryFile(fileName, codeSize);
		if (!binaryCode)
		{
			printf("Error reading binary file '%s'\n", fileName);
			exit(1);
		}

		VkShaderModuleCreateInfo shaderModuleCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.codeSize = (size_t)codeSize,
			.pCode = (const uint32_t*)binaryCode
		};

		VkShaderModule shaderModule = nullptr;
		VkResult res = vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &shaderModule);
		CHECK_VK_RESULT(res, "vkCreateShaderModule error\n");
		printf("Created shader module from binary file\n");

		free(binaryCode);

		return shaderModule;
	}
}