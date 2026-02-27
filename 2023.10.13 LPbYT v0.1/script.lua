-- basic variables
deFfont = intraFont.load('assets/regular.pgf')
local bg = Image.load('assets/bg.png')
local LP = Image.load('assets/LP.png')
pad, oldpad, currentfile, alpha = '', '', '', 0
lang = System.getLang()
if lang ~= 'Russian' then lang = 'English' end
-- include asyncCycle functions
dofile('samples/asyncCycle.lua')
-------------------------------------------------------------------------------

-- some basic functions
function print(x, y, color, text, size, font)
	-- if font isn't load
	if font == nil then font = deFfont end
	if size == nil then size = 0.9 end
	-- set size
	intraFont.size(font, size)
	-- correct the y coordinate
	y = y + intraFont.textH(font)
	intraFont.printColored(font, x, y, text, color, size)
end

function press(button)
	if pressed[button](pad) and not pressed[button](oldpad) then
		oldpad = pad
		return true
	end
end

function held(button)
	if pressed[button](pad) then
		return true
	end
end

local show_logo = function()
	local logo = Image.load('assets/logo.png')
	local timered = timer.create()
	timer.start(timered)
	local x, y, w, h, a, done = 0, -272, 480, 272, 255, false
	
	while true do
		screen.clear()
		
		Image.draw(logo, x, y, w, h, Color.new(0,0,0,0), 0, 0, 480, 272, 0, a)
		
		if timered and timer.time(timered) > 10 then
			if y < 0 then
				y = y + 2
			else
				if h > 204 and not done then
					y = y + 2
					h = h - 2
				elseif h == 204 or h < 272 then
					done = true
					y = y - 2
					h = h + 2
				elseif done and h == 272 then
					if a > 0 then
						a = a - 2
					else
						break
					end
				end
			end
			timer.reset(timered)
			timer.start(timered)
		end
		
		screen.flip()
	end
	
	timer.remove(timered)
	Image.unload(logo)
end

-------------------------------------------------------------------------------

-- color palette
colors = {
	white = Color.new(255,255,255),
	black = Color.new(0,0,0),
	clear = Color.new(0,0,0,0),
	green = Color.new(107,157,118),
	blue = Color.new(76, 102, 240),
}

-------------------------------------------------------------------------------

-- menu array
local menu = {
	num = 1,
	time = '',
	
	Russian = {
		{name = 'Цвет', file = 'samples/color.lua', desc = 'Демонстрация функций работы\nс цветом. Создание нового цвета\nи получение значений [r], [g], [b],\n[a] уже созданного цвета.'},
		{name = 'Изображения', file = 'samples/image.lua', desc = 'Демонстрация функций для\nотображения и деформации\nизображений.'},
		{name = 'Звук', file = 'samples/sound.lua', desc = 'Демонстрация функций для\nработы со звуком. В том числе\nсчитывание ID3 тегов.'},
		{name = 'Системные функции', file = 'samples/system.lua', desc = 'Демонстрация функций для\nполучения системной\nинформации.'},
		{name = 'Таймер', file = 'samples/timer.lua', desc = 'Примитивные функции таймера.'},
		exit = 'Нажмите START, чтобы выйти\nв XMB.',
		demo = 'Демонстрационный образец.',
		demo2 = 'Список всех функций находится в файле "docs.txt".',
	},
	
	English = {
		{name = 'Color', file = 'samples/color.lua', desc = 'Color functions. Creating a new\ncolor and getting [r], [g], [b], [a] \nvalues of current color.'},
		{name = 'Image', file = 'samples/image.lua', desc = 'Functions to draw, move and\ndeform images.'},
		{name = 'Sound', file = 'samples/sound.lua', desc = 'Shows the usage of [sound] module.\nPlay/pause/stop functions\nand getting ID3 tags.'},
		{name = 'System', file = 'samples/system.lua', desc = 'Functions to get some system\ninformation.'},
		{name = 'Timer', file = 'samples/timer.lua', desc = 'Simple timer functions.'},
		exit = 'Press START to exit to XMB.',
		demo = 'Technical demo.',
		demo2 = 'Complete list of functions сan be found in "docs.txt" file.'
	},
}

lines = {
	Russian = {
		'Нажмите Х, чтобы сгенерировать случайный цвет.',
		'Стрелочки - громкость.',
		'Х - воспроизвести/О - остановить.',
		'Х - запустить таймер/О - остановить таймер.',
		'Треугольник - сбросить таймер.',
		'Нажмите START, чтобы выйти в меню.'
	},
	
	English = {
		'Press X to create a random color.',
		'D-Pad Left/D-Pad Right - volume.',
		'X - play/O - stop.',
		'X - start timer/O - stop timer.',
		'Triangle - reset timer.',
		'Press START to exit to main menu.'
	}
}
-------------------------------------------------------------------------------

-- animation

function blinkUPD()
	if alpha == 0 then
		asyncCycle:new(0,255,3,10,function(i) alpha = i end)
	elseif alpha == 255 then
		asyncCycle:new(255,0,-3,10,function(i) alpha = i end)
	end
end

System.message("Function of автосохранения is отсутствует.")
show_logo()

sound.play('assets/bg.mp3', sound.MP3, false, true)
while true do
	-- clear the screen
	screen.clear()
	-- animations update
	asyncCycle:update()
	-- read buttons
	pad = buttons.read()
	
	
	menu.time = tostring(System.getTime().minutes):len() == 1 and '0'..System.getTime().minutes or System.getTime().minutes
	
	Image.draw(bg,0,0)
	blinkUPD()
	Image.draweasy(LP,270,40,colors.clear,0,alpha)
	print(478-intraFont.textW(deFfont,tostring(System.getTime().hour..':'..menu.time)),2,colors.white,tostring(System.getTime().hour..':'..menu.time))
	
	for i, v in ipairs(menu[lang]) do
		print(50,30 + i*12,(i == menu.num and colors.blue) or colors.white,v.name)
		if menu.num == i then
			currentfile = v.file
			print(260,150,colors.white,v.desc)
		end
	end
	
	print (260,200,colors.white,menu[lang].exit)
	print(240-intraFont.textW(deFfont, menu[lang].demo)/2,240,colors.white,menu[lang].demo)
	print(240-intraFont.textW(deFfont, menu[lang].demo2)/2,250,colors.white,menu[lang].demo2)
	
	if press('up') and menu.num > 1 then menu.num = menu.num - 1 end
	if press('down') and menu.num < #menu[lang] then menu.num = menu.num + 1 end
	if press('cross') then sound.stop(sound.MP3) dofile(currentfile) end
	if press('start') then LUA.quit() end
	
	-- read buttons x2
	oldpad = pad
	-- flip the screen
	screen.flip()
end