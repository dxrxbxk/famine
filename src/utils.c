#include "utils.h"
#include <string.h>

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

size_t	ft_strncmp(const char *s1, const char *s2, size_t n) {
	for (size_t i = 0; i < n; i++) {
		if (s1[i] != s2[i])
			return (s1[i] - s2[i]);
		if (s1[i] == '\0')
			return 0;
	}
	return 0;
}

void *ft_strnstr(const char *haystack, const char *needle, size_t len) {
	size_t needle_len = ft_strlen(needle);
	if (needle_len == 0)
		return (char *)haystack;
	for (size_t i = 0; i < len; i++) {
		if (haystack[i] == needle[0]) {
			if (ft_strncmp(haystack + i, needle, needle_len) == 0)
				return (char *)haystack + i;
		}
	}
	return NULL;
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

int	ft_memcmp(const void *s1, const void *s2, size_t size) {
	uint8_t *str1 = (uint8_t *)s1;
	uint8_t *str2 = (uint8_t *)s2;

	for (size_t i = 0; i < size; i++) {
		if (str1[i] != str2[i])
			return (str1[i] - str2[i]);
	}
	return (0);
}

void *ft_memmem(const void *haystack, size_t haystack_len, const void *needle, size_t needle_len) {
    if (!haystack || !needle || needle_len == 0 || haystack_len < needle_len) {
        return NULL;
    }

    const unsigned char *h = (const unsigned char *)haystack;
    const unsigned char *n = (const unsigned char *)needle;

    for (size_t i = 0; i <= haystack_len - needle_len; i++) {
        if (h[i] == n[0] && ft_memcmp(&h[i], n, needle_len) == 0) {
            return (void *)&h[i];
        }
    }
    return NULL;
}

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
