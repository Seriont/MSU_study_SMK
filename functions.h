#include <cstring>


void normalizeString(char str[])
{
	int i = strlen(str);
	while (str[i] == '\n' || str[i] == ' ' || str[i] == '\0')
		i--;
	str[i+1] = '\0';
}
