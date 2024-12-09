#include "errors.hpp"

using namespace PSR;

static std::shared_ptr<char[]> emptyMessage(new char[1]{ '\0' });

std::shared_ptr<char[]> ErrorMessage::errorMessage = emptyMessage;

void ErrorMessage::ClearMessage()
{
	errorMessage = emptyMessage;
}

const char* ErrorMessage::SetMessage(const char* message)
{
	errorMessage = std::shared_ptr<char[]>(new char[strlen(message) + 1]);
	strcpy(errorMessage.get(), message);

	return errorMessage.get();
}

const char* ErrorMessage::WrapMessage(const char* message)
{
	const char* innerError = errorMessage.get();
	
	size_t errLen = strlen(innerError);
	size_t mesLen = strlen(message);
	
	size_t length = errLen + mesLen + 2;	// 1 for space, 1 for null terminator

	std::shared_ptr<char[]> newMessage = std::shared_ptr<char[]>(new char[length]);
	snprintf(newMessage.get(), length, "%s %s", message, innerError);

	errorMessage = newMessage;

	return errorMessage.get();
}