# 3D projector thingy

![](image.jpg)

For [December Adventures](https://eli.li/december-adventure) 2023 I decided to finally learn some C by making a 3D projector inspired by [moogle](https://wiki.xxiivv.com/site/moogle.html) and [pinhole](https://git.sr.ht/~bellinitte/pinhole).

I'm trying things out as I go without a clear goal in mind. Right now it renders 3D models as "solid" wireframes by doing [back-face culling](https://en.wikipedia.org/wiki/Back-face_culling) and [hidden-line removal](https://en.wikipedia.org/wiki/Hidden-line_removal).

You can read the full log on [my website](https://ghettobastler.com/december_adventure_2023.html).

## Features

- STL file import
- 3D rotation and translation
- Variable focal length
- Back-face culling
- Frustum clipping
- Hidden-line removal (glitchy)
- Image export

## Things I might wanna add

- Second camera to show the effect of various optimizations from a different angle
- A script to generate 3D models
- Triangle rasterisation (for texturing the models)

## Disclaimer

This is a learning project. I'm sharing it here for others to see, but I wouldn't recommend actually using it, as it is quite slow, glitchy and resource hungry.

## Licensing

The code for this project is licensed under the terms of the GNU GPLv3 license.
