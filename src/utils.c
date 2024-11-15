#include "famine.h"

int	ft_strlen(const char *str) {
	int i = 0;
	while (str[i]) {
		i++;
	}
	return (i);
}

void print_hex(void *data, size_t size) {
	for (size_t i = 0; i < size; i++) {
		PRINT("%02x", ((unsigned char *)data)[i]);
			PRINT(" ");
		if (i % 16 == 15)
			PRINT("\n");
	}
	PRINT("\n");
}

void	*ft_memset(void *b, int c, size_t len) {
	uint8_t *ptr = b;
	for (size_t i = 0; i < len; i++) {
		ptr[i] = c;
	}
	return b;
}

void	*ft_memcpy(void *dst, const void *src, size_t size) {
	uint8_t *d = dst;
	uint8_t *s = (uint8_t *)src;

	for (size_t i = 0; i < size; i++) {
		d[i] = s[i];
	}
	return dst;
}

int	ft_memcmp(const void *s1, const void *s2, size_t size) {
	uint8_t *str1 = (uint8_t *)s1;
	uint8_t *str2 = (uint8_t *)s2;

	for (size_t i = 0; i < size; i++) {
		if (str1[i] != str2[i])
			return (str1[i] - str2[i]);
	}
	return (0);
}
