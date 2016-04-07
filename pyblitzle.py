import argparse
import cv2
import numpy as np


class FilterValue(object):
    def __init__(self, x, y, z):
        super(FilterValue, self).__init__()
        self.x = x
        self.y = y
        self.z = z

    def set_x(self, x):
        self.x = x

    def set_y(self, y):
        self.y = y

    def set_z(self, z):
        self.z = z

    def scalar(self):
        return self.x, self.y, self.z


class FilterRange(object):
    def __init__(self, lower, higher):
        super(FilterRange, self).__init__()
        self.lower = lower
        self.higher = higher


ap = argparse.ArgumentParser()
ap.add_argument("-i", "--image", required=True, help="path to the input image")
args = vars(ap.parse_args())
src_image = cv2.imread(args["image"])

f_lower = FilterRange(FilterValue(0, 190, 205), FilterValue(2, 205, 225))
f_higher = FilterRange(FilterValue(175, 140, 160), FilterValue(180, 200, 240))
other = {'dilateX': 1, 'dilateY': 1, 'houghThresh': 1, 'houghLen': 1, 'houghGap': 1}

resultWindow = 'Result'
cv2.namedWindow(resultWindow)


def filter_red(hsv):
    lower_hue = cv2.inRange(hsv, f_lower.lower.scalar(), f_lower.higher.scalar())
    higher_hue = cv2.inRange(hsv, f_higher.lower.scalar(), f_higher.higher.scalar())
    filtered = cv2.addWeighted(lower_hue, 1.0, higher_hue, 1.0, 0.0)
    return filtered


def dilate(binary):
    d_x = other['dilateX']
    d_y = other['dilateY']
    kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (d_x, d_y))
    dilated = cv2.dilate(binary, kernel)
    return dilated


def get_lines(binary):
    h_thresh = other['houghThresh']
    h_len = other['houghLen']
    h_gap = other['houghGap']
    lines = cv2.HoughLinesP(binary, 1, np.pi / 2, h_thresh, None, h_len, h_gap)
    return lines


def get_hp_bars(binary):
    _, contours, _ = cv2.findContours(binary, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    for cnt in contours:
        center, (height, width), angle = cv2.minAreaRect(cnt)
        if 2 < height < 4 and 5 < width < 80 and 89 < -angle < 91:
            box = cv2.boxPoints((center, (height, width), angle))
            yield np.int0(box)


def get_players_pos(hp_bars):
    for bar in hp_bars:
        yield (bar[1][0] + 35, bar[1][1] + 60)


def recompute(update_func=None):
    if update_func is not None:
        update_func()

    hsv = cv2.cvtColor(src_image, cv2.COLOR_BGR2HSV)
    red = filter_red(hsv)
    hp_bars = get_hp_bars(red)
    players_pos = get_players_pos(hp_bars)

    for px, py in players_pos:
        cv2.circle(src_image, (px, py), 60, (0, 255, 0))

    result = cv2.resize(src_image, (0, 0), None, 0.5, 0.5, cv2.INTER_AREA)
    cv2.imshow(resultWindow, result)


controlWindow1 = 'Lower HSV'
cv2.namedWindow(controlWindow1)
cv2.createTrackbar('H1', controlWindow1, f_lower.lower.x, 180, lambda x: recompute(f_lower.lower.set_x(x)))
cv2.createTrackbar('S1', controlWindow1, f_lower.lower.y, 255, lambda x: recompute(f_lower.lower.set_y(x)))
cv2.createTrackbar('V1', controlWindow1, f_lower.lower.z, 255, lambda x: recompute(f_lower.lower.set_z(x)))
cv2.createTrackbar('H2', controlWindow1, f_lower.higher.x, 180, lambda x: recompute(f_lower.higher.set_x(x)))
cv2.createTrackbar('S2', controlWindow1, f_lower.higher.y, 255, lambda x: recompute(f_lower.higher.set_y(x)))
cv2.createTrackbar('V2', controlWindow1, f_lower.higher.z, 255, lambda x: recompute(f_lower.higher.set_z(x)))

controlWindow2 = 'Higher HSV'
cv2.namedWindow(controlWindow2)
cv2.createTrackbar('H1', controlWindow2, f_higher.lower.x, 180, lambda x: recompute(f_higher.lower.set_x(x)))
cv2.createTrackbar('S1', controlWindow2, f_higher.lower.y, 255, lambda x: recompute(f_higher.lower.set_y(x)))
cv2.createTrackbar('V1', controlWindow2, f_higher.lower.z, 255, lambda x: recompute(f_higher.lower.set_z(x)))
cv2.createTrackbar('H2', controlWindow2, f_higher.higher.x, 180, lambda x: recompute(f_higher.higher.set_x(x)))
cv2.createTrackbar('S2', controlWindow2, f_higher.higher.y, 255, lambda x: recompute(f_higher.higher.set_y(x)))
cv2.createTrackbar('V2', controlWindow2, f_higher.higher.z, 255, lambda x: recompute(f_higher.higher.set_z(x)))

controlWindow3 = 'Post Filter'
cv2.namedWindow(controlWindow3)
cv2.createTrackbar('Dilate X', controlWindow3, other.get('dilateX'), 100,
                   lambda x: recompute(other.__setitem__('dilateX', x)))
cv2.createTrackbar('Dilate Y', controlWindow3, other.get('dilateY'), 100,
                   lambda x: recompute(other.__setitem__('dilateY', x)))
cv2.createTrackbar('Hough Threshold', controlWindow3, other.get('houghThresh'), 100,
                   lambda x: recompute(other.__setitem__('houghThresh', x)))
cv2.createTrackbar('Hough Min Length', controlWindow3, other.get('houghLen'), 100,
                   lambda x: recompute(other.__setitem__('houghLen', x)))
cv2.createTrackbar('Hough Max Gap', controlWindow3, other.get('houghGap'), 100,
                   lambda x: recompute(other.__setitem__('houghGap', x)))

recompute()
cv2.waitKey(0)
