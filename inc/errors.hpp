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
  private:
	static std::shared_ptr<char[]> errorMessage;

  public:
	static void ClearMessage();

	static constexpr const char* GenericErrorMessage = "\nAn Error Has Occurred:\n";

	static const char* GetMessage()
	{
		return errorMessage.get();
	}

	static const char* SetMessage(const char* message);

	static const char* WrapMessage(const char* message);
};

} // namespace PSR