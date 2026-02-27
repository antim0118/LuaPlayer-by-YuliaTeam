-- load an Image
local image = Image.load('assets/pic.png')
local modes = {num = 1, 'lUp', 'Center', 'rUp', 'lDn', 'rDn'}

local x, y, w, h, color, xsrc, ysrc, cropw, croph, rotation, alpha = 0, 0, 480, 272, Color.new(0,0,0,0), 0, 0, 480, 272, 0, 255


while true do
	screen.clear()
	pad = buttons.read()
	
	Image.draw(image, x, y, w, h, color, xsrc, ysrc, cropw, croph, rotation, alpha, Image[modes[modes.num]])
	
	print(240-intraFont.textW(deFfont, lines[lang][#lines[lang]])/2,200,Color.new(245,68,24),lines[lang][#lines[lang]])
	
	print(10,20,Color.new(245,68,24),'x: '..x)
	print(60,20,Color.new(245,68,24),'y: '..y)
	print(10,35,Color.new(245,68,24),'w: '..w)
	print(60,35,Color.new(245,68,24),'h: '..h)
	print(10,50,Color.new(245,68,24),'xsrc: '..xsrc)
	print(10,65,Color.new(245,68,24),'ysrc: '..ysrc)
	print(10,80,Color.new(245,68,24),'cropw: '..cropw)
	print(10,95,Color.new(245,68,24),'croph: '..croph)
	print(10,110,Color.new(245,68,24),'Align mode: '..modes[modes.num])
	
	print(245,20,Color.new(245,68,24),'Move ANALOG STICK to move picture.')
	print(278,35,Color.new(245,68,24),'Hold X/O button to zoom picture.')
	print(278,50,Color.new(245,68,24),'Hold SQUARE/TRIANGLE button\nto crop picture.')
	print(253,110,Color.new(245,68,24),'Press SELECT to change align mode.')
	
	if math.abs(buttons.Lx(pad)) > 60 then x = x + 2*math.modf(buttons.Lx(pad)/100) end
	if math.abs(buttons.Ly(pad)) > 60 then y = y + 2*math.modf(buttons.Ly(pad)/100) end
	
	if press('select') then
		if modes.num == #modes then
			modes.num = 1
		else
			modes.num = modes.num + 1
		end
	end
	
	if held('cross') then
		-- zoom +
		w = w + 2
		h = h + 1
	elseif held('circle') then
		-- zoom -
		w = w - 2
		h = h - 1
	elseif held('square') then
		-- crop +
		if xsrc < 480 then xsrc = xsrc + 2 end
		if ysrc < 272 then ysrc = ysrc + 1 end
		if cropw > 0 then cropw = cropw - 2 end
		if croph > 0 then croph = croph - 1 end
	elseif held('triangle') then
		-- crop -
		if xsrc <= 0 then xsrc = 0 else xsrc = xsrc - 2 end
		if ysrc > 0 then ysrc = ysrc - 1 end
		if cropw < 480 then cropw = cropw + 2 end
		if croph < 272 then croph = croph + 1 end
	end
	
	if held('l') then rotation = rotation - 2 end
	if held('r') then rotation = rotation + 2 end
	
	
	if press('start') then break; end
	
	oldpad = pad
	screen.flip()
end

Image.unload(image)
sound.play('assets/bg.mp3', sound.MP3, false, true)