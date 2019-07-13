#pragma once

#ifdef BRYMEN869_EXPORTS
#define BRYMEN869_API __declspec(dllexport)
#else
#define BRYMEN869_API __declspec(dllimport)
#endif

#include <functional>

extern "C" {
	using Brymen_CallbackType = std::function<void(const char* xmlptr, size_t size, void * user_data)>; /* function to pass */

	void BRYMEN869_API Brymen_registerCallback(Brymen_CallbackType cb, void * user_data);

	int	BRYMEN869_API Brymen_start();

	void BRYMEN869_API Brymen_shutdown();
};
