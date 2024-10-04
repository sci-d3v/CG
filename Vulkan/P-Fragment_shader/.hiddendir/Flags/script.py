
from PIL import Image

# Открываем изображение
image = Image.open("russia-flag.jpg")

# Создаем новое изображение с прозрачной частью сверху
new_image = Image.new("RGBA", (image.width, image.height *3/2.0), (0, 0, 0, 0))

# Копируем исходное изображение в новое изображение
new_image.paste(image, (image.width, image.height))

# Сохраняем измененное изображение
new_image.save("example_with_transparent_top.png")
