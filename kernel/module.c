#include "module.h"

bool modules_init() {
	bool success = true;
	for (modfunc_t *fn = &_modinit_start; fn != &_modinit_end; fn++) {
		if (!(*fn)()) {
			success = false;
		}
	}
	return success;
}

bool modules_exit() {
	bool success = true;
	for (modfunc_t *fn = &_modexit_start; fn != &_modexit_end; fn++) {
		if (!(*fn)()) {
			success = false;
		}
	}
	return success;
}
