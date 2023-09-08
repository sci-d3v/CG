from PIL import Image
image = Image.open("russia-flag.png")
new_image = Image.new("RGBA", (image.width, int(image.height *3/2.0)), (0, 0, 0, 0))
new_image.paste(image, (0, int(image.height *3/2.0)-image.height))
new_image.save("example_with_transparent_top.png")
