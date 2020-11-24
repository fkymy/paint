#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#define die(fp) ({\
		write(1, "Error: Operation file corrupted\n", 32);\
		if (fp) fclose(fp);\
		return 1;})\

struct circle {
	float x, y, r;
	char type, c;
};

int is_circle(struct circle *circle)
{
	return (circle->r > 0 && (circle->type == 'c' || circle->type == 'C'));
}

int should_fill(int x, int y, struct circle *circle)
{
	float xa = x, ya = y;
	float xb = circle->x, yb = circle->y;
	float dist;

	dist = sqrtf((xa - xb) * (xa - xb) + (ya - yb) * (ya - yb));
	if (dist > circle->r)
		return (0);
	if (circle->r - dist < 1)
		return (1);
	return (circle->type == 'C' ? 1 : 0);
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
		die(fp);

	// 1. Parse zone
	struct zone {
		int w, h;
		char c;
	} zone;

	if (fscanf(fp, "%d %d %c\n", &zone.w, &zone.h, &zone.c) != 3)
		die(fp);
	if (!(zone.w > 0 && zone.w <= 300 && zone.h > 0 && zone.h <= 300))
		die(fp);

	memset(output, zone.c, zone.w * zone.h);

	// 3. Parse and apply circles
	struct circle circle;

	int n;
	while ((n = fscanf(fp, "%c %f %f %f %c\n",
					&circle.type, &circle.x, &circle.y,
					&circle.r, &circle.c)) == 5)
	{
		if (!is_circle(&circle))
			die(fp);

		for (int y = 0; y < zone.h; y++)
			for (int x = 0; x < zone.w; x++)
				if (should_fill(x, y, &circle))
					output[y * zone.w + x] = circle.c;
	}
	if (n != EOF)
		die(fp);

	// 2. Output
	for (int i = 0; i < zone.h; i++)
	{
		write(1, output + zone.w * i, zone.w);
		write(1, "\n", 1);
	}
	fclose(fp);
	return (0);
}
