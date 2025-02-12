#include "utils.h"

int	ft_strlen(const char *s)
{
	int i = 0;
	while (s[i])
		i++;
	return (i);
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

void	ft_memmove(void *dst, const void *src, size_t size) {
	uint8_t *d = dst;
	uint8_t *s = (uint8_t *)src;

	if (d < s) {
		for (size_t i = 0; i < size; i++) {
			d[i] = s[i];
		}
	} else {
		for (size_t i = size; i > 0; i--) {
			d[i - 1] = s[i - 1];
		}
	}
}
//
//int	ft_memcmp(const void *s1, const void *s2, size_t size) {
//	uint8_t *str1 = (uint8_t *)s1;
//	uint8_t *str2 = (uint8_t *)s2;
//
//	for (size_t i = 0; i < size; i++) {
//		if (str1[i] != str2[i])
//			return (str1[i] - str2[i]);
//	}
//	return (0);
//}

void	ft_strcpy(char *dest, const char *src)
{
	int i = 0;
	while (src[i])
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
}

size_t ft_strlcat(char *dst, const char *src, size_t size)
{
	size_t i = 0;
	size_t j = 0;
	size_t len = 0;
	while (dst[i] && i < size)
		i++;
	len = i;
	while (src[j] && i + 1 < size)
	{
		dst[i] = src[j];
		i++;
		j++;
	}
	if (len < size)
		dst[i] = '\0';
	return (len + ft_strlen(src));
}

void	make_path(char *path, const char *dir, const char *file)
{
	char slash[] = "/";
	ft_strcpy(path, dir);
	ft_strlcat(path, slash, 1024);
	ft_strlcat(path, file, 1024);
}
