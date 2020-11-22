#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ERR_OP "Error: Operation file corrupted"

#define die(msg, fp) ({\
		write(1, msg, ft_strlen(msg));\
		write(1, "\n", 1);\
		if (fp) fclose(fp);\
		return 1;})\

size_t ft_strlen(char *s)
{
	char *p = s;
	while (*p)
		p++;
	return (p - s);
}

void *ft_memset(void *s, int c, size_t n)
{
	char *ss = s;
	for (size_t i = 0; i < n; i++)
		ss[i] = c;
	return (s);
}

int is_range(int len)
{
	return (len > 0 && len <= 300);
}

struct rect {
	float tlx, tly, w, h;
	char type, c;
};

int is_rect(struct rect *rect)
{
	return ((rect->w > 0 && rect->h > 0) &&
			(rect->type == 'R' || rect->type == 'r'));
}

int should_fill(int x, int y, struct rect *rect)
{
	int brx = rect->tlx + rect->w;
	int bry = rect->tly + rect->h;

	if (x < rect->tlx || y < rect->tly)
		return (0);
	if (x > brx || y > bry)
		return (0);
	if (x - rect->tlx < 1 || y - rect->tly < 1)
		return (1);
	if (rect->tlx + rect->w - x < 1 || rect->tly + rect->h - y < 1)
		return (1);
	return (rect->type == 'R' ? 1 : 0);
}

int main(int argc, char *argv[])
{
	char output[90000];
	FILE *fp;

	if (argc != 2)
		die("Error: argument", 0);

	if (!(fp = fopen(argv[1], "r")))
		die(ERR_OP, 0);

	// 1. Parse zone
	struct zone {
		int w, h;
		char c;
	} zone;

	if (!(fscanf(fp, "%d %d %c\n", &zone.w, &zone.h, &zone.c) == 3))
		die(ERR_OP, fp);
	if (!is_range(zone.w) || !is_range(zone.h))
		die(ERR_OP, fp);

	ft_memset(output, zone.c, zone.w * zone.h);

	// 3. Parse and apply Rect
	struct rect rect;

	int n;
	while ((n = fscanf(fp, "%c %f %f %f %f %c\n",
					&rect.type, &rect.tlx, &rect.tly,
					&rect.w, &rect.h, &rect.c)) == 6)
	{
		if (!is_rect(&rect))
			die(ERR_OP, fp);
		for (int y = 0; y < zone.h; y++)
			for (int x = 0; x < zone.w; x++)
				if (should_fill(x, y, &rect))
					output[y * zone.w + x] = rect.c;
	}
	if (n != EOF)
		die(ERR_OP, fp);

	// 2. Output
	for (int i = 0; i < zone.h; i++)
	{
		write(1, output + i * zone.w, zone.w);
		write(1, "\n", 1);
	}
	fclose(fp);
	return (0);
}
