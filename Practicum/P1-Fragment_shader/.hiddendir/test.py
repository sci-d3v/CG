from PIL import ImageGrab

# set the coordinates of the preview window on the screen
left = 100
top = 100
right = 500
bottom = 500

# capture the contents of the preview window
image = ImageGrab.grab(bbox=(left, top, right, bottom))

# save the captured image as a file
filename = "russian_flag.png"
image.save(filename)
