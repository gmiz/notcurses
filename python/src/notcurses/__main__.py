import sys
import locale
import notcurses
import _cffi_backend
from _notcurses import lib, ffi

locale.setlocale(locale.LC_ALL, "")
nc = notcurses.Notcurses()
c = Cell(nc.stdplane())
c.setBgRGB(0x80, 0xc0, 0x80)
nc.stdplane().setBaseCell(c)
dims = nc.stdplane().getDimensions()
r = 0x80
g = 0x80
b = 0x80
for y in range(dims[0]):
    for x in range(dims[1]):
        nc.stdplane().setFgRGB(r, g, b)
        nc.stdplane().setBgRGB(b, r, g)
        nc.stdplane().putSimpleYX(y, x, b'X')
        b = b + 2
        if b == 256:
            b = 0
            g = g + 2
            if g == 256:
                g = 0
                r = r + 2
nc.render()
