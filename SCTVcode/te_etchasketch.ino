// ------------------------------ Etch-A-Sketch -------------------------------

struct Point {
    int x;
    int y;
};

const int MAX_SEGMENTS = 1000;
const int END_TOKEN = -9999;

Point* segments;
Point* segment;
Point direction;
int x, y;

struct item etchList[] = {};

void getControlsXY() {
	// read the position controls for X,Y, and average for smooth readings
 	for (int i = 0; i < 40; i++) {
 		x += analogRead(XPosPin) - 512;	// read position controls
 		y += analogRead(YPosPin) - 512;	// make bipolar so midpoint is nominal
 	}
    // Center in the display area, scale for correct overshoot
    x = x / 20;
    y = y / 20;
}

void resetEtchASketch() {
    if (segments == 0) {
        segments = (Point*)malloc(sizeof(Point) * (MAX_SEGMENTS+2));
    }
    direction.x = 0;
    direction.y = 0;

    getControlsXY();
    segment = segments;
    segment[0].x = x;
    segment[0].y = y;
    segment[1].x = END_TOKEN;
}

void etchASketch() {
    if (pushed) {
        // selector button pushed: erase all traces
        resetEtchASketch();
        pushed = false;
    }

    // read X,Y control knobs and extend trace segments to it
    getControlsXY();
    int dx = x - segment->x;    
    int dy = y - segment->y;
    int ds = sqrt((dx*dx + dy*dy) * 64);
    if (ds != 0) {
        // Serial.printf("Etch: xy=(%d,%d), ds=%d\n", x, y, ds);
        // int n = segment - segments;
        // Serial.printf(" dir=(%d,%d) %d segs, (%d,%d)-(%d,%d)\n",
        //               direction.x, direction.y, n, segment[0].x, segment[0].y, x, y);
        // normalized direction vector for this segment
        dx = 64 * dx / ds;
        dy = 64 * dy / ds;
        int diff = abs(dx - direction.x) + abs(dy - direction.y);
        if (diff > 2 && segment < segments + MAX_SEGMENTS) {
            // have changed direction: start new segment
            // Serial.printf(" dx,dy=(%d,%d), ds=%d\n", dx, dy, ds);
            direction.x = dx;
            direction.y = dy;
            segment++;
            segment[0].x = x;
            segment[0].y = y;
            segment[1].x = END_TOKEN;
        } else {
            // else just extend last segment
            segment->x = x;
            segment->y = y;
        }
    }

    // draw current list of trace segments
    Scale = 1;
    Shape = lin;
    for (Point* seg = segments; seg[1].x != END_TOKEN; seg++) {
        XStart = seg[0].x;
        YStart = seg[0].y;
        XEnd = seg[1].x;
        YEnd = seg[1].y;
        DoSeg();    // draw a line from (XStart,YStart) to (XEnd,YEnd)
    }
}
