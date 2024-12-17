/**
 * @file errors.hpp
 * @author Purdue Solar Racing
 * @brief Contains error messages
 * @version 0.1
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

#include <cstring>
#include <memory>

namespace PSR
{

class ErrorMessage
{
  public:
	struct Error
	{
		std::shared_ptr<char[]> Message;
		std::shared_ptr<Error> InnerError;

		static std::shared_ptr<Error> EmptyError;

		Error(const char* message, const std::shared_ptr<Error>& innerError)
			: Message(std::make_shared<char[]>(strlen(message) + 1)), InnerError(innerError)
		{
			strcpy(Message.get(), message);
		}

		Error(const std::shared_ptr<char[]> message, const std::shared_ptr<Error>& innerError)
			: Message(message), InnerError(innerError) {}

		Error(const char* message)
			: Error(message, nullptr) {}

		Error(const std::shared_ptr<char[]> message)
			: Error(message, nullptr) {}
	};

  private:
	static std::shared_ptr<Error> error;

	static size_t GetRequiredMessageSize(const std::shared_ptr<Error>& error, size_t depth = 0);
	static std::shared_ptr<char[]> WriteInnerErrors(const std::shared_ptr<Error>& error);

  public:
	static void ClearMessage();

	static constexpr const char* GenericErrorMessage = "An Error Has Occurred:";

	static const char* GetMessage()
	{
		if (error == Error::EmptyError || error == nullptr)
			return Error::EmptyError->Message.get(); // Return empty error message

		return WriteInnerErrors(error).get();
	}

	static void PrintMessage();

	static void SetMessage(const char* message)
	{
		error = std::shared_ptr<Error>(new Error(message));
	}

	static void SetMessage(const std::shared_ptr<char[]>& message)
	{
		error = std::shared_ptr<Error>(new Error(message));
	}

	static void WrapMessage(const char* message)
	{
		error = std::shared_ptr<Error>(new Error(message, error));
	}

	static void WrapMessage(const std::shared_ptr<char[]>& message)
	{
		error = std::shared_ptr<Error>(new Error(message, error));
	}
};

} // namespace PSR