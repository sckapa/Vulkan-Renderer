#include "SckVK_Utils.h"

const char* MessageSeverity(VkDebugUtilsMessageSeverityFlagBitsEXT severity)
{
	switch (severity)
	{
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		return "Verbose";
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		return "Info";
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		return "Warning";
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		return "Error";
	default:
		break;
	}
}

const char* MessageType(VkDebugUtilsMessageTypeFlagsEXT type)
{
	switch (type)
	{
	case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
		return "General";
	case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
		return "Validation";
	case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
		return "Performance";
	default:
		break;
	}
}

bool ReadFile(const char* fileName, std::string& data)
{
	std::ifstream f(fileName);

	bool ret = false;

	if (f.is_open())
	{
		std::string line;

		while (std::getline(f, line))
		{
			data.append(line);
			data.append("\n");
		}

		f.close();

		ret = true;
	}
	else
	{
		printf("File could not be opened\n");
		exit(1);
	}

	return ret;
}

void WriteBinaryFile(const char* pFilename, const void* pData, int size)
{
	FILE* f = NULL;

	errno_t err = fopen_s(&f, pFilename, "wb");

	if (!f) {
		printf("Error opening '%s'\n", pFilename);
		exit(1);
	}

	size_t bytes_written = fwrite(pData, 1, size, f);

	if (bytes_written != size) {
		printf("Error write file\n");
		exit(1);
	}

	fclose(f);
}

char* ReadBinaryFile(const char* pFilename, int& size)
{
	FILE* f = NULL;

	errno_t err = fopen_s(&f, pFilename, "rb");

	if (!f) {
		char buf[256] = { 0 };
		strerror_s(buf, sizeof(buf), err);
		printf("Error opening '%s': %s\n", pFilename, buf);
		exit(1);
	}

	struct stat stat_buf;
	int error = stat(pFilename, &stat_buf);

	if (error) {
		char buf[256] = { 0 };
		strerror_s(buf, sizeof(buf), err);
		printf("Error getting file stats: %s\n", buf);
		return NULL;
	}

	size = stat_buf.st_size;

	char* p = (char*)malloc(size);
	if (!p)
	{
		printf("Resize error\n");
		exit(1);
	}

	size_t bytes_read = fread(p, 1, size, f);

	if (bytes_read != size) {
		char buf[256] = { 0 };
		strerror_s(buf, sizeof(buf), err);
		printf("Read file error file: %s\n", buf);
		exit(1);
	}

	fclose(f);

	return p;
}
