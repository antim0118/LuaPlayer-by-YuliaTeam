function string:split(symbols)
	local o = 1
	local k = 1
	local l = symbols:len()-1
	local res = {}
	while k <= self:len() do
		if self:sub(k, k+l) == symbols then
			table.insert(res, self:sub(o, k-1))
			k=k+l
			o=k+1
		end
		k=k+1
	end
	table.insert(res, self:sub(o, k))
	return res
end

function readRPY(name)
	local file = assert(io.open(name,"rb"))
	local str = file:read("*all")
	file:close()
	return str
end

function getRPY(name) return readRPY(name):split("\n") end

function getRPY_2(name)
	local lines = {}
    for line in io.lines(name) do
        table.insert(lines, line)
    end
    return lines
end

--dofile('asyncCycle.lua')
dofile('asyncCycle_new.lua')

function press(button)
	return buttons.pressed(buttons[button])
	--if buttons.pressed[button](pad) and not buttons.pressed[button](oldpad) then
		--oldpad = pad
		--return true
	--end
end

function hold(button)
	return buttons.held(buttons[button])
end

function release(button)
	return buttons.released(buttons[button])
end

function dump(o)
    if type(o) == 'table' then
			local s = '{'
			for k,v in ipairs(o) do
				s = tostring(s)..'['..tostring(k)..']='..dump(v)..','
			end
			for k,v in pairs(o) do
				if type(k) ~= 'number' then
					s = tostring(s)..'["'..(tostring(k))..'"]='..dump(v)..','
				end
			end
			return s..'}'
    elseif type(o) ~= 'string' then
			return tostring(o)
		else
			return '"'..tostring(o):gsub('"','\\"')..'"'
		end
end

function readDir(path)
	local dir = System.listDir(path)
	table.remove(dir, 1)
	return dir
end

local images = {}
local keepAnim = false

--[[startAnim = function(path, pisc)
	local pics, pics2 = {}, readDir(path)

	if keepAnim == false then
		for i in ipairs(pics2) do
			pics[i] = {pic = Image.load('piccs/'..pics2[i].name:lower()), num = i}
		end
		keepAnim = true
	else
		pics = pisc
	end

	asyncCycle:new(0, #pics+1, 1, 70, function(i)
		for _, pic in ipairs(pics) do
			if i == pic.num then
				if images[1] then
					images = {}
				end
				table.insert(images, pic)
			end
		end
		
		if i == #pics+1 then
			images = {}
			if keepAnim == false then
				for _, pic in ipairs(pics) do
					Image.unload(pic.pic)
					System.GC()
				end
			else
				startAnim(path, pics)
			end
			System.GC()
		end
	end)
end]]

startAnim = function(path)
	local pics, pics2 = {}, readDir(path)

	for i in ipairs(pics2) do
		pics[i] = {pic = path..'/'..pics2[i].name:lower(), num = i}
	end
	
	asyncCycle:new(0, #pics, 1, 1, function(i)
		for _, pic in ipairs(pics) do
			if i == pic.num then
			if images[1] then
				if type(images[1].pic) == 'userdata' then
					Image.unload(images[1].pic)
				end
				images[1].pic = nil
				images = {}
			end
				table.insert(images, pic)
			end
		end
		
		if i == #pics then
			if keepAnim == true then
				startAnim(path)
			else
				if type(images[1].pic) == 'userdata' then
					Image.unload(images[1].pic)
				end
				images[1].pic = nil
				images = {}
			end
			System.GC()
		end
	end)
end

--local sm = readDir('SPIDERMAN')

--local f = io.open('text.txt', 'w')
--for _, file in ipairs(sm) do
	--f:write('-----------------------------------------------------------------------------\n'..file.name..'\n\n')
--end
--f:close()

local pool = {
	"guitar.ogg",
	"Minded.ogg",
}

local num = 1

--local font = intraFont.load('w3-ip.pgf')
local font = intraFont.load('bank_gothic.ttf', 25)

--local img2 = Image.load('main_menu.png')
--local img = Image.load('piccs2/splashscreen_night_1.png')
local img2 = Image.load('piccs2/splashscreen_night_2.png')

local cur = 1

local letters = {"../", "Minded.ogg", "08 - In the End.wav"}
local tags = {"title: In the End", "artist: Linkin Park", "album: Hybrid Theory", "Year: 2000"}
local cover

local saveslots = {
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
}

--sound.play("afterword.at3", sound.AT3_1, false, true)
--sound.play("dryout.at3", sound.AT3_2, false, true)
--sound.play("guitar.wav", sound.WAV_1, false, true)

local a = 255
local counter

local files = {}
local lineC = 0

local color = Color.new(0,0,0,0)
--particles.new("rain/rain2.png", "rain", "rain", 500, -20)
--sound.play('sl_md_amb_rain.wav', sound.WAV_1, false, true)

local aalpha = 0
local yy = -136

sssaver = {
	x = 240-25,
	y = 136-25,
	w = 50,
	h = 50,
	color = Color.new(0,255,255),
	dx = 2,
	dy = 2
}

function drawScreensaver(sssaver)
	screen.drawRect(sssaver.x, sssaver.y, sssaver.w, sssaver.h, sssaver.color)  -- Отрисовка фигуры
      
    sssaver.x = sssaver.x + sssaver.dx
    sssaver.y = sssaver.y + sssaver.dy
        
    -- Проверка на касание границ
    if sssaver.x < 0 or sssaver.x + sssaver.w > 480 then
    sssaver.dx = -sssaver.dx  -- Изменение направления по X
    end
    
	if sssaver.y < 0 or sssaver.y + sssaver.h > 272 then
        sssaver.dy = -sssaver.dy  -- Изменение направления по Y
    end
end

local frame, allAlpha

--local sl1 = Image.load('piccs2/sl_1_body.png')
--local sl2 = Image.load('piccs2/sl_1_pioneer.png')
--local sl3 = Image.load('piccs2/sl_1_smile.png')

allAlpha = 255
local Zcenter = 0
sound.cloud('Minded.ogg', sound.OGG_3)
--sound.cloud('loading_theme.mp3', sound.MP3)
sound.cloud('guitar.wav', sound.WAV_3)

--PMP.Mp4_Info("sasha.mp4")

local x, y, x2 = 480, 50, -260

--local spritelist = Image.load('sptitelist.jpg')

while true do
	screen.clear(Color.new(0,0,0))
	asyncCycle:update()
	buttons.read()
	
	--if img then Image.draw(img, 0, -136) end
	if img2 then Image.draweasy(img2, 0, 0, nil, 0, a) end
	
	--if cover then Image.draw(cover, x2, 10) end
	
	for i, str in ipairs(tags) do
		LUA.print(x, 25 + (i-1)*12, str)
	end
	
	--Image.draw(spritelist, 100, 100, 64, 64, nil, 6, 6, 118, 118, 0, 255, Image.lUp, false, true)
	
	if PMP.getFrame(frame) then
		--Image.draw(frame, 0, 0, 240, 136, Color.new(235, 64, 194), 0, 0, 240, 136)
		--Image.draw(frame, 0, 136, 240, 136, Color.new(25, 64, 14), 0, 136, 240, 136)
		--Image.draw(frame, 240, 0, 240, 136, Color.new(255, 164, 124), 240, 0, 240, 136)
		--Image.draw(frame, 240, 136, 240, 136, Color.new(35, 87, 62), 240, 136, 240, 136)
		
		Image.draweasy(frame, 0, 0, nil, 0, allAlpha)
		--Image.draweasy(sl1, 340, 136, nil, 0, allAlpha, Image.Center)
		--Image.draweasy(sl2, 340, 136, nil, 0, allAlpha, Image.Center)
		--Image.draweasy(sl3, 340, 136, nil, 0, allAlpha, Image.Center)
		
		intraFont.print(240-intraFont.textW(nil, PMP.getSubs())/2, 200, PMP.getSubs())
		--LUA.print(100, 100, PMP.getSubs())
		
		if press('select') then
			PMP.pause()
		end
		
		if press('square') then
			PMP.seek(300)
		end
		
		if press('circle') then
			PMP.stop(frame)
			frame = nil
			System.GC()
		end
	end
	
	--LUA.print(0,0,dump(LUA.getRAM())..' '..dump(sound))--..' '..dump(lineC))--.." | gonna play/now playing: "..pool[num]))
	LUA.print(0, 200, tostring(LUA.getRAM()))--dump(math.max(90, 23)))
	--intraFont.print(20, 20, "Главное меню", Color.new(255,255,255), font2, 2)
	
	if hold('up') then
		a = a + 1
	end
	if hold('down') then
		a = a - 1
	end
	
	if hold('left') then
		xx = xx - 1
	elseif hold('right') then
		xx = xx + 1
	end
	
	--intraFont.print(20, 70, "Тест надписи поверх видео", Color.new(136,234,64), font, 0.8)
	
	for c = 1, 3 do
		--intraFont.print(380, 5 + (c-1)*25, letters[c], (cur == c and Color.new(234,34,135)) or Color.new(250,250,250,50), font, 0.8)
		--intraFont.print(380, 10 + (c-1)*25, letters[c], (cur == c and Color.new(136,234,64)) or Color.new(250,250,250,50), nil, 0.9)
		--intraFont.print(350, y + (c-1)*12, letters[c], (cur == c and Color.new(183,255,20)) or Color.new(183,209,231,128), font, 1, 0)
		--intraFont.print(20, 70 + c*25, letters[c], Color.new(234,34,135), font, 0.8, 35)
	end

	--intraFont.print(10, 10, "1234567890!@#$%^&*()-=+_.,?<>:;'\"№\nАБВГДЕЁЖЗИЙКЛМНОПРТУФХЦ\nЧШЩЪЫ ЬЭЮЯ\nабвгдеёжзийклмнопрстуфхц\nчшщъыьэюя\nABCDEFGHIJKLMNOPRSTQUVWXYZ\nabcdefghijklmnoprstquvwxyz", Color.new(255,255,255), font)

	if press('triangle') then Image.drawCircleOnTex(img2, 100, 100, 50, Color.new(23, 143, 57)) end
	screen.filledRect(102, 102, 50, 50, Color.new(255, 0, 0))

	if press('up') then
		if cur > 1 then cur = cur - 1 end--sound.play(sound.WAV_1, false) end
	elseif press('down') then
		if cur < 3 then cur = cur + 1 end--sound.play(sound.WAV_1, false) end
	elseif press('cross') then
		if cur == 1 then
			--GO.NAHUI()
			--sound.play("afterword.at3", sound.AT3_1, true, true)
			--[[for _, file in ipairs(System.listDir('scenario')) do
				if file.name ~= '..' then
					local temp, c = System.fileDumpCreate('scenario/'..file.name)
					table.insert(files, {pointer = temp, count = c, name = file.name})
				end
			end]]
			--Image.unload(img)
			--img:__gc()
			--asyncCycle.new(0, 255, 15, 20, "function func(i) img2 = i end", "func")
			--particles.new('rain.png', 'rain', 'rain', 90)
			--particles.new('rain.png', 'leaves', 'rain', 500)
			--asyncCycle.new(255, 0, -15, 20, "")
			--System.message(debug.traceback())
			--Image.unloadALL()
			--Image.unload(img)
			--Image.unload(img2)
			--img = nil
			--img2 = nil
			--frame = PMP.play('uv.pmp', true, true, 'sasha.srt', buttons.start, 29.97)
			sound.play(sound.OGG_2, false)
			--PMP.play('uv.pmp', false, 'sasha.srt', buttons.start)
			--sound.play(sound.WAV_1, true)
			--asyncCycle:new(0, 255, 13, 2, function(i) allAlpha = i end)
			--LUA.screenshot('temp.png', 480, 272)
			--PMP.setVolume(10)
			--lineC = PMP.play("SASHA.pmp", false, "SASHA.srt", buttons.start)
			--lineC = System.SaveData('','','','EBOOT.PBP', nil, true)
			--System.removeFile('temp.png')
			--sound.play(sound.MP3, true)
			--sound.pause(sound.WAV_1)
			--LUA.screenshot('test.png', 144, 80)
			--lineC = System.AutoSdve("Начало Игры", "Данные игрока", "Информация об общем прохождении игры", "EBOOT.PBP", "SPECIALDATA")
			--lineC = System.SaveData("Начало Игры", "Данные игрока", "Информация об общем прохождении игры", "EBOOT.PBP", 'testres', 'piccs2/candle_1.png')
			--System.removeFile('test.png')
			--System.message("gwrn9ui")
			--sound.pause(sound.WAV_1)
			--if rez then saveslots[tonumber(rez:sub(-1,-1))+1] = true end
			System.GC()
		elseif cur == 2 then
			--sound.unload(sound.WAV_1)
			--for _, file in ipairs(files) do
			--	System.fileDumpRemove(file.pointer, file.count)
			--end
			--PMP.play('opening.pmp', buttons.cross)
			--lineC = lineC + 1
			--particles.delete('snow')
			--PMP.stop(frame)
			--frame = PMP.play('SASHA.pmp', true, 'sasha.srt', buttons.start, 29.97)
			--sound.stop(sound.FLAC_1)
			PMP.Mp4_Info("sasha.mp4")
			--img = Image.load('piccs2/splashscreen_night_1.png')
			--img2 = Image.load('piccs2/splashscreen_night_2.png')
			--Image.stopVideo(lineC)
			--lineC = nil
			--Image.unload(lineC)
			--sound.play("afterword.at3", sound.AT3_1, false, true)
			--particles.delete("rain")
			--sound.stop(sound.AT3_1)
			--lineC = System.LoadData()
			--lineC = System.AutoLoad('SPECIALDATA')
			--files = {}
			System.GC()
		elseif cur == 3 then
			--LUA.exit()
			--particles.delete('leaves')
			--System.DeleteData('test.png')
			sound.cloud("8-In the End.mp3", sound.MP3)
			cover = Image.load("ht.tga")
			sound.play(sound.MP3, true)
			asyncCycle:new(0,4096,20,2,function(g) if g == 80 then LUA.sleep(100)
			end if x2 < 10 then x2 = x2 + 3 end if x > 300 then x = x - 3 end if y > -50 then y = y -3 end end)
			--sound.stop(sound.AT3_1)
			--System.GC()
			--PMP.play('schokk.pmp', buttons.cross)
			--lineC = lineC + 1
			--if rez then saveslots[tonumber(rez.id:sub(-1,-1))+1] = false end
		elseif cur == 4 then
			--LUA.screenshot('test.png')
			--LUA.screenshot('screen.png', 800, 600)
			System.message("test")
			--lineC = lineC + 1
		end
	end
	
	if press('start') then LUA.exit() end
	
	--LUA.print(10,10,tostring(LUA.getRAM()))--.." | gonna play/now playing: "..pool[num]))
	screen.flip()
end