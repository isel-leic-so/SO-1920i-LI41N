#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MATHDLL_EXPORTS
	_declspec(dllexport)
#else
	//_declspec(dllimport)
#endif
		LONG Add(INT i1, INT i2);

#ifdef __cplusplus
}
#endif
