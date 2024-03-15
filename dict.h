#ifndef DICT_H
#define DICT_H

#include <stdlib.h>
#include <string.h>

#define import_dict(key_type, val_type, container) \
\
	static val_type dummy_##container##_val; \
	\
	typedef struct _##container \
	{ \
		size_t size; \
		key_type *keys; \
		val_type *values; \
		const char *exptr; \
		char exception; \
		size_t enum_depth; \
		size_t (*dict_size)(struct _##container* dict); \
		val_type (*dict_get)(struct _##container* dict, key_type key); \
		void (*dict_set)(struct _##container* dict, key_type key, val_type value); \
		val_type (*dict_remove)(struct _##container* dict, key_type key); \
		void (*dict_enum)(struct _##container* dict, int (*enum_proc)(struct _##container* dict, key_type key, val_type value)); \
		int (*dict_exists)(struct _##container* dict, key_type key); \
		void (*dict_clear)(struct _##container* dict); \
		void (*dict_free)(struct _##container* dict); \
	} _##container; \
	\
	typedef _##container *container; \
	\
	static void container##_exception(container dict) \
	{ \
		dict->exception = *dict->exptr; \
	} \
	\
	static size_t container##_size(container list) \
	{ \
		return list->size; \
	} \
	\
	static val_type container##_get(container dict, key_type key) \
	{ \
		if (!dict || !dict->keys || !dict->values) \
		{ \
			container##_exception(dict); \
		} \
		for (size_t i = 0; i < dict->size; i++) \
		{ \
			if (!memcmp(&dict->keys[i], &key, sizeof(key))) \
			{ \
				return dict->values[i]; \
			} \
		} \
		container##_exception(dict); \
		return dummy_##container##_val; \
	} \
	\
	static void container##_set(container dict, key_type key, val_type value) \
	{ \
		if (!dict || dict->enum_depth) \
		{ \
			container##_exception(dict); \
		} \
		size_t index = dict->size; \
		if (dict->keys) \
		{ \
			for (size_t i = 0; i < dict->size; i++) \
			{ \
				if (!memcmp(&dict->keys[i], &key, sizeof(key))) \
				{ \
					index = i; \
				} \
			} \
		} \
		if (index == dict->size) \
		{ \
			dict->keys = (key_type *)realloc(dict->keys, (dict->size + 1) * sizeof(key_type)); \
			dict->values = (val_type *)realloc(dict->values, (dict->size + 1) * sizeof(val_type)); \
			dict->size++; \
		} \
		dict->keys[index] = key; \
		dict->values[index] = value; \
	} \
	\
	static val_type container##_remove(container dict, key_type key) \
	{ \
		if (!dict || !dict->keys || !dict->values || dict->enum_depth) \
		{ \
			container##_exception(dict); \
		} \
		for (size_t i = 0; i < dict->size; i++) \
		{ \
			if (!memcmp(&dict->keys[i], &key, sizeof(key))) \
			{ \
				val_type original = dict->values[i]; \
				memmove(&dict->keys[i], &dict->keys[i + 1], (dict->size - i - 1) * sizeof(key_type)); \
				memmove(&dict->values[i], &dict->values[i + 1], (dict->size - i - 1) * sizeof(val_type)); \
				dict->size--; \
				return original; \
			} \
		} \
		container##_exception(dict); \
		return dummy_##container##_val; \
	} \
	\
	static void container##_enum(container dict, int (*enum_proc)(container dict, key_type key, val_type value)) \
	{ \
		if (!dict || !dict->keys || !dict->values) \
		{ \
			container##_exception(dict); \
		} \
		dict->enum_depth++; \
		if (!dict->enum_depth) \
		{ \
			container##_exception(dict); \
		} \
		for (size_t i = 0; i < dict->size; i++) \
		{ \
			if (enum_proc(dict, dict->keys[i], dict->values[i])) \
			{ \
				break; \
			} \
		} \
		dict->enum_depth--; \
	} \
	\
	static int container##_exists(container dict, key_type key) \
	{ \
		if (!dict) \
		{ \
			container##_exception(dict); \
		} \
		if (dict->keys) \
		{ \
			for (size_t i = 0; i < dict->size; i++) \
			{ \
				if (!memcmp(&dict->keys[i], &key, sizeof(key))) \
				{ \
					return 1; \
				} \
			} \
		} \
		return 0; \
	} \
	\
	static void container##_clear(container dict) \
	{ \
		if (!dict || dict->enum_depth) \
		{ \
			container##_exception(dict); \
		} \
		free(dict->keys); \
		free(dict->values); \
		dict->size = 0; \
		dict->keys = NULL; \
		dict->values = NULL; \
	} \
	\
	static void container##_free(container dict) \
	{ \
		if (dict) \
		{ \
			container##_clear(dict); \
			free(dict); \
		} \
	} \
	\
	static container new_##container() \
	{ \
		container result = (container)malloc(sizeof(_##container)); \
		if (result) \
		{ \
			result->keys = NULL; \
			result->values = NULL; \
			result->size = 0; \
			result->exptr = NULL; \
			result->exception = 0; \
			result->enum_depth = 0; \
			result->dict_size = &container##_size; \
			result->dict_get = &container##_get; \
			result->dict_set = &container##_set; \
			result->dict_remove = &container##_remove; \
			result->dict_enum = &container##_enum; \
			result->dict_clear = &container##_clear; \
			result->dict_free = &container##_free; \
		} \
		return result; \
	}

#define new(container) new_##container()

#define dict_size(dict) dict->dict_size(dict)
#define dict_get(dict, key) dict->dict_get(dict, key)
#define dict_set(dict, key, value) dict->dict_set(dict, key, value)
#define dict_remove(dict, key) dict->dict_remove(dict, key)
#define dict_enum(dict, proc) dict->dict_enum(dict, proc)
#define dict_exists(dict, key) dict->dict_exists(dict, key)
#define dict_clear(dict) dict->dict_clear(dict)
#define dict_free(dict) dict->dict_free(dict)

#endif /* DICT_H */
