
#ifndef PT_TRY
#define TRY(func) \
	do { \
		if ((func) < 0) { \
			perror(#func); \
			goto err_out; \
		} \
	} while (0)

#define TRY_ERR(func, errcode) \
	do { \
		if ((func) < 0) { \
			perror(#func); \
			ret_err = (errcode); \
			goto err_out; \
		} \
	} while (0)

#define TRY_NOEQU(func, equal) \
	do { \
		if ((func) != (equal)) { \
			perror(#func); \
			goto err_out; \
		} \
	} while (0)

#define TRY_NOEQU_ERR(func, equal, errcode) \
	do { \
		if ((func) != (equal)) { \
			perror(#func); \
			ret_err = (errcode); \
			goto err_out; \
		} \
	} while (0)

#define TRY_GIVE(func, var) \
	do { \
		if (((var) = (func)) < 0) { \
			perror(#func); \
			goto err_out; \
		} \
	} while (0)
#define TRY_GIVE_ERR(func, var, errcode) \
	do { \
		if (((var) = (func)) < 0) { \
			perror(#func); \
			ret_err = (errcode); \
			goto err_out; \
		} \
	} while (0)
#endif
