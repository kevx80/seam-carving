import imageio

with imageio.get_writer('movie.gif', mode='I') as writer:
    for i in range(200):
        image = imageio.imread("img%d.bin" % i)
        writer.append_data(image)