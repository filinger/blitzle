import argparse
import cv2


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

lower = FilterRange(FilterValue(0, 190, 205), FilterValue(2, 205, 225))
higher = FilterRange(FilterValue(175, 180, 170), FilterValue(180, 200, 240))
other = {'dilateX': 1, 'dilateY': 1}

resultWindow = 'Result'
cv2.namedWindow(resultWindow)


def recompute(update_func=None):
    if update_func is not None:
        update_func()

    src_hsv = cv2.cvtColor(src_image, cv2.COLOR_BGR2HSV)
    lower_hue = cv2.inRange(src_hsv, lower.lower.scalar(), lower.higher.scalar())
    higher_hue = cv2.inRange(src_hsv, higher.lower.scalar(), higher.higher.scalar())
    filtered = cv2.addWeighted(lower_hue, 1.0, higher_hue, 1.0, 0.0)

    d_x = other['dilateX']
    d_y = other['dilateY']
    kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (d_x, d_y))
    result = cv2.morphologyEx(filtered, cv2.MORPH_CLOSE, kernel)

    result = cv2.resize(result, (0, 0), None, 0.5, 0.5, cv2.INTER_AREA)
    cv2.imshow(resultWindow, result)


controlWindow1 = 'Lower HSV'
cv2.namedWindow(controlWindow1)
cv2.createTrackbar('H1', controlWindow1, lower.lower.x, 180, lambda x: recompute(lower.lower.set_x(x)))
cv2.createTrackbar('S1', controlWindow1, lower.lower.y, 255, lambda x: recompute(lower.lower.set_y(x)))
cv2.createTrackbar('V1', controlWindow1, lower.lower.z, 255, lambda x: recompute(lower.lower.set_z(x)))
cv2.createTrackbar('H2', controlWindow1, lower.higher.x, 180, lambda x: recompute(lower.higher.set_x(x)))
cv2.createTrackbar('S2', controlWindow1, lower.higher.y, 255, lambda x: recompute(lower.higher.set_y(x)))
cv2.createTrackbar('V2', controlWindow1, lower.higher.z, 255, lambda x: recompute(lower.higher.set_z(x)))

controlWindow2 = 'Higher HSV'
cv2.namedWindow(controlWindow2)
cv2.createTrackbar('H1', controlWindow2, higher.lower.x, 180, lambda x: recompute(higher.lower.set_x(x)))
cv2.createTrackbar('S1', controlWindow2, higher.lower.y, 255, lambda x: recompute(higher.lower.set_y(x)))
cv2.createTrackbar('V1', controlWindow2, higher.lower.z, 255, lambda x: recompute(higher.lower.set_z(x)))
cv2.createTrackbar('H2', controlWindow2, higher.higher.x, 180, lambda x: recompute(higher.higher.set_x(x)))
cv2.createTrackbar('S2', controlWindow2, higher.higher.y, 255, lambda x: recompute(higher.higher.set_y(x)))
cv2.createTrackbar('V2', controlWindow2, higher.higher.z, 255, lambda x: recompute(higher.higher.set_z(x)))

controlWindow3 = 'Post Filter'
cv2.namedWindow(controlWindow3)
cv2.createTrackbar('Dilate X', controlWindow3, other.get('dilateX'), 100,
                   lambda x: recompute(other.__setitem__('dilateX', x)))
cv2.createTrackbar('Dilate Y', controlWindow3, other.get('dilateY'), 100,
                   lambda x: recompute(other.__setitem__('dilateY', x)))


recompute()
cv2.waitKey(0)
