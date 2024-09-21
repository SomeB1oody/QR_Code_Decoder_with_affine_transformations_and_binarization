#Author: Stan Yin
#GitHub Name: SomeB1oody
#This project is based on CC 4.0 BY, please mention my name if you use it.
#This project requires opencv.
import numpy as np
import cv2 as cv
from param import String

print("Please enter the path of the image\t")
print("Example: C:\\Wallpaper\\02.png\t")
path = input("Enter HERE:")
img_original = cv.imread(path)

if img_original is None:
    raise ValueError("Could not load image")

img = cv.resize(img_original,(500,500))

img_contrast = cv.convertScaleAbs(img,alpha = 1.8, beta = 30)
img_gray = cv.cvtColor(img,cv.COLOR_BGR2GRAY)
img_filter = cv.bilateralFilter(img_gray,13,26,6)
img_binary = cv.threshold(img_filter, 190, 255, cv.THRESH_BINARY_INV)
img_canny = cv.Canny(img_binary, 10, 100, 3, False)
ough_lines = cv.HoughLines(img_canny, 5, np.pi / 180, 100)
a = 50.0
b = np.pi /9
remove_index = set()
sign3 = True
while sign3:
    new_line = []

    for index1 in range(len(hough_lines)):
        for index2 in range(len(hough_lines)):
            if index1 == index2:
                continue
            rho1, theta1 = hough_lines[index1]
            rho2, theta2 = hough_lines[index2]

            if theta1 > np.pi:  theta1 -= np.pi
            if theta2 > np.pi: theta2 -= np.pi

            sign1 = abs(theta1 - theta2) <= b
            sign2 = abs(rho1- rho2) <= a

            if (theta1 > np.pi / 2 and theta2 < np.pi / 2 and np.pi - theta2 + theta1 < b) or \
                (theta2 > np.pi / 2 and theta1 < np.pi / 2 and np.pi - theta1 + theta2 < b):
                sign = True

            if sign1 and sign2:
                remove_index.add(index2)

    for index4 in range(len(hough_lines)):
        if index4 not in remove_index: new_line.append(hough_lines[index4])

    hough_lines = np.array(new_line)

    if len(hough_lines) == len(remove_index): sign3 = False

threshold_ = 0.2 * 708
points = []

for index5 in range(len(new_line)):
    for index6 in range(index5+1, len(new_line)):
        rho3, theta3 = new_line[index5]
        rho4, theta4 = new_line[index6]

        if theta3 == 0: theta3 = 0.01
        if theta4 == 0: theta4 = 0.01

        a1 = np.cos(theta3)
        a2 = np.sin(theta3)
        b1 = np.cos(theta3)
        b2 = np.sin(theta4)

        x = (rho4 * b1 - rho3 * b2) / (a2 * b1 - a1 * b2)
        y = (rho3 - a1 * x) / b1

        pt = (int(round(x)), int(round(y)))

        if(pt[0] <= 708 + threshold_ and pt[0] >= -threshold_) and \
        (pt[1] <= 708 + threshold_ and pt[1] >= -threshold_):
            points.append(pt)

current = np.array(points[:4], dtype = np.float32)
change = np.array([[0,0],[1,0], [1,1], [0,1]], dtype = np.float32)

transfer_matrix = cv.getPerspectiveTransform(current, change)

img_perspective = np.zeros((708, 708), dtype = np.uint8)
img_perspective = cv.warpPerspective(img, transfer_matrix, (708, 708))

qr_text, bbox, straight_qr_code = cv.QRCodeDetector.detectAndDecode(img_perspective)
if straight_qr_code is not None:
    raise ValueError("QR code not found")

print("Text is:", qr_text)

