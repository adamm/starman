#include <stdbool.h>
#include <math.h>

#include "display.h"
#include "util.h"

#include "draw.h"


static void swap(double* x, double* y) {
    double tmp = *x;
    *x = *y;
    *y = tmp;
}


static double ipart(double x) {
    return floor(x);
}


static double fpart(double x) {
    return x - floor(x);
}


static double rfpart(double x) {
    return 1 - fpart(x);
}


void draw_pixel(display_t* d, int x, int y, uint8_t c) {
    d->overlay[y][x] = c;
}


void draw_line(display_t* d, double x1, double x2, double y1, double y2, uint8_t c) {
    double x, y;
    float dx, dy, step;

    dx = x2 - x1;
    dy = y2 - y1;
    if (fabs(dx) >= fabs(dy))
        step = fabs(dx);
    else
        step = fabs(dy);

    dx = dx / step;
    dy = dy / step;
    x = x1;
    y = y1;
    int i = 1;
    while (i <= step) {
        draw_pixel(d, x, y, c);
        x += dx;
        y += dy;
        i++;
    }
}


// draw antialiased line
void draw_line_aa(display_t* d, double x1, double x2, double y1, double y2, uint8_t c) {
    bool steep = fabs(y2 - y1) > fabs(x2 - x1);

    if (steep) {
        swap(&x1, &y1);
        swap(&x2, &y2);
    }
    if (x1 > x2) {
        swap(&x1, &y1);
        swap(&x2, &y2);
    }

    double dx = x2 - x1;
    double dy = y2 - y1;
    double gradient = 1.0;
    if (dx != 0.0)
        gradient = dy / dx;

    // Handle first endpoint
    double xend = round(x1);
    double yend = y1 + gradient * (xend - x1);
    double xgap = rfpart(x1 + 0.5);
    double xpxl1 = xend;
    double ypxl1 = ipart(yend);
    if (steep) {
        draw_pixel(d, ypxl1, xpxl1, rfpart(yend) * xgap * c);
        draw_pixel(d, ypxl1+1, xpxl1, fpart(yend) * xgap * c);
    }
    else {
        draw_pixel(d, xpxl1, ypxl1, rfpart(yend) * xgap * c);
        draw_pixel(d, xpxl1, ypxl1+1, fpart(yend) * xgap * c);
    }

    double intery = yend + gradient; // first y-intersection for the main loop

    // Handle second endpoint
    xend = round(x2);
    yend = y2 + gradient * (xend - x2);
    xgap = fpart(x2 + 0.5);
    double xpxl2 = xend;
    double ypxl2 = ipart(yend);

    if (steep) {
        draw_pixel(d, ypxl2, xpxl2, rfpart(yend) * xgap * c);
        draw_pixel(d, ypxl2+1, xpxl2, fpart(yend) * xgap * c);
    }
    else {
        draw_pixel(d, xpxl2, ypxl2, rfpart(yend) * xgap * c);
        draw_pixel(d, xpxl2, ypxl2+1, fpart(yend) * xgap * c);
    }

    // Main loop
    if (steep) {
        for (int x = xpxl1 + 1; x < xpxl2 - 1; x++) {
            draw_pixel(d, ipart(intery), x, rfpart(intery) * c);
            draw_pixel(d, ipart(intery)+1, x, fpart(intery) * c);
            intery += gradient;
        }
    }
    else {
        for (int x = xpxl1 + 1; x < xpxl2 - 1; x++) {
            draw_pixel(d, x, ipart(intery), fpart(intery) * c);
            draw_pixel(d, x, ipart(intery)+1, fpart(intery) * c);
            intery += gradient;
        }
    }
}


static void draw_circle_slice(display_t* d, double xc, double yc, double x, double y, uint8_t c) {
    draw_pixel(d, xc+x, xc+y, c);
    draw_pixel(d, xc-x, xc+y, c);
    draw_pixel(d, xc+x, xc-y, c);
    draw_pixel(d, xc-x, xc-y, c);
    draw_pixel(d, xc+y, xc+x, c);
    draw_pixel(d, xc-y, xc+x, c);
    draw_pixel(d, xc+y, xc-x, c);
    draw_pixel(d, xc-y, xc-x, c);
}


void draw_circle(display_t* dpy, double xc, double yc, double r, uint8_t c) {
    double x = 0, y = r;
    double d = 3 - 2 * r;

    draw_circle_slice(dpy, xc, yc, x, y, c);
    while (y >= x) {
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        }
        else {
            d = d + 4 * x + 6;
        }
        draw_circle_slice(dpy, xc, yc, x, y, c);
    }
}


