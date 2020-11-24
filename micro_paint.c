#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define die(fp) ({\
		write(1, "Error: Operation file corrupted\n", 32);\
		if (fp) fclose(fp);\
		return 1;})\

struct rect {
	float xtl, ytl, w, h;
	char type, c;
};

int is_rect(struct rect *rect)
{
	return ((rect->w > 0 && rect->h > 0) &&
			(rect->type == 'R' || rect->type == 'r'));
}

int should_fill(int x, int y, struct rect *rect)
{
	float xa = x, ya = y;
	float xbr = rect->xtl + rect->w;
	float ybr = rect->ytl + rect->h;

	if (xa < rect->xtl || ya < rect->ytl)
		return (0);
	if (xa > xbr || ya > ybr)
		return (0);
	if (xa - rect->xtl < 1 || ya - rect->ytl < 1)
		return (1);
	if (xbr - xa < 1 || xbr - ya < 1)
		return (1);
	return (rect->type == 'R' ? 1 : 0);
}

int main(int argc, char *argv[])
{
	char output[90000];
	FILE *fp;

	if (argc != 2)
	{
		write(1, "Error: argument\n", 16);
		return (1);
	}
	if (!(fp = fopen(argv[1], "r")))
		die(NULL);

	// 1. Parse zone
	struct zone {
		int w, h;
		char c;
	} zone;

	if (!(fscanf(fp, "%d %d %c\n", &zone.w, &zone.h, &zone.c) == 3))
		die(fp);
	if (!(zone.w > 0 && zone.w <= 300 && zone.h > 0 && zone.h <= 300))
		die(fp);

	memset(output, zone.c, zone.w * zone.h);

	// 3. Parse and apply Rect
	struct rect rect;

	int n;
	while ((n = fscanf(fp, "%c %f %f %f %f %c\n",
					&rect.type, &rect.xtl, &rect.ytl,
					&rect.w, &rect.h, &rect.c)) == 6)
	{
		if (!is_rect(&rect))
			die(fp);

		for (int y = 0; y < zone.h; y++)
			for (int x = 0; x < zone.w; x++)
				if (should_fill(x, y, &rect))
					output[y * zone.w + x] = rect.c;
	}
	if (n != EOF)
		die(fp);

	// 2. Output
	for (int i = 0; i < zone.h; i++)
	{
		write(1, output + i * zone.w, zone.w);
		write(1, "\n", 1);
	}
	fclose(fp);
	return (0);
}
